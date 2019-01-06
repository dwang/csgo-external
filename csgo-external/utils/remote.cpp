#pragma once
#include "memutils.hpp"
#include "remote.hpp"
#include <algorithm>
#include <cassert>
#include <malloc.h>
#include <Shlwapi.h>
#include <vector>
#include <Windows.h>
#include <winternl.h>

#pragma comment(lib, "ntdll.lib")

#define IN_RANGE(x, a, b) (x >= a && x <= b)
#define GET_BITS(x) (IN_RANGE(x, '0', '9') ? (x - '0') : ((x & (~0x20)) - 'A' + 0xa))
#define GET_BYTE(x) (GET_BITS(x[0]) << 4 | GET_BITS(x[1]))

EXTERN_C NTSYSAPI
NTSTATUS NTAPI NtReadVirtualMemory(
	_In_		HANDLE	ProcessHandle,
	_In_		PVOID	BaseAddress,
	_Out_		PVOID	Buffer,
	_In_		SIZE_T	NumberOfBytesToRead,
	_Out_opt_	PSIZE_T	NumberOfBytesRead
);

EXTERN_C NTSYSAPI
NTSTATUS NTAPI NtWriteVirtualMemory(
	_In_		HANDLE	ProcessHandle,
	_In_		PVOID	BaseAddress,
	_In_		PVOID	Buffer,
	_In_		SIZE_T	NumberOfBytesToWrite,
	_Out_opt_	PSIZE_T	NumberOfBytesWritten
);

EXTERN_C NTSYSAPI
NTSTATUS NTAPI NtOpenProcess(
	_Out_		PHANDLE				ProcessHandle,
	_In_		ACCESS_MASK			DesiredAccess,
	_In_		POBJECT_ATTRIBUTES	ObjectAttributes,
	_In_opt_	CLIENT_ID*			ClientId
);

typedef struct x_LDR_DATA_TABLE_ENTRY
{
	LIST_ENTRY InLoadOrderLinks;
	LIST_ENTRY InMemoryOrderLinks;
	LIST_ENTRY InInitializationOrderLinks;
	PVOID DllBase;
	PVOID EntryPoint;
	ULONG SizeOfImage;
	UNICODE_STRING FullDllName;
	UNICODE_STRING BaseDllName;
	ULONG Flags;
	WORD LoadCount;
	WORD TlsIndex;
	union
	{
		LIST_ENTRY HashLinks;
		struct
		{
			PVOID SectionPointer;
			ULONG CheckSum;
		};
	};
	union
	{
		ULONG TimeDateStamp;
		PVOID LoadedImports;
	};
	_ACTIVATION_CONTEXT * EntryPointActivationContext;
	PVOID PatchInformation;
	LIST_ENTRY ForwarderLinks;
	LIST_ENTRY ServiceTagLinks;
	LIST_ENTRY StaticLinks;
} xLDR_DATA_TABLE_ENTRY, *xPLDR_DATA_TABLE_ENTRY;

typedef struct x_PEB_LDR_DATA
{
	ULONG Length;
	UCHAR Initialized;
	PVOID SsHandle;
	LIST_ENTRY InLoadOrderModuleList;
	LIST_ENTRY InMemoryOrderModuleList;
	LIST_ENTRY InInitializationOrderModuleList;
	PVOID EntryInProgress;
} xPEB_LDR_DATA, *xPPEB_LDR_DATA;

namespace remote
{
	static auto s_attached_process = INVALID_HANDLE_VALUE;

	__declspec(noinline)
	static auto get_process_id_by_image_name(const fnv::hash name) -> HANDLE
	{
		auto len = ULONG(0);
		NtQuerySystemInformation(SystemProcessInformation, nullptr, 0, &len);
		const auto buf = alloca(len);
		auto ret = NtQuerySystemInformation(SystemProcessInformation, buf, len, nullptr);
		assert(NT_SUCCESS(ret));
		if (!NT_SUCCESS(ret))
			return INVALID_HANDLE_VALUE;

		auto it = PSYSTEM_PROCESS_INFORMATION(buf);
		do
		{
			char image_name[MAX_PATH + 1];
			ANSI_STRING image_name_astr
			{
				0,
				sizeof(image_name),
				image_name
			};
			ret = RtlUnicodeStringToAnsiString(&image_name_astr, &it->ImageName, FALSE);
			assert(NT_SUCCESS(ret));

			image_name[image_name_astr.Length] = 0;
			for (auto& c : image_name)
				c = tolower(c);

			if (fnv::hash_runtime(image_name) == name)
				return it->UniqueProcessId;

		} while (it->NextEntryOffset && ((it = offset_ptr(it, it->NextEntryOffset))));

		return INVALID_HANDLE_VALUE;
	}

	auto attach_process(const fnv::hash name) -> void
	{
		const auto pid = get_process_id_by_image_name(name);

		constexpr auto access = PROCESS_ALL_ACCESS;

		CLIENT_ID client_id;

		client_id.UniqueProcess = pid;
		client_id.UniqueThread = nullptr;

		OBJECT_ATTRIBUTES attr;
		InitializeObjectAttributes(&attr,
			nullptr,
			FALSE,
			nullptr,
			nullptr);

		auto ret = NtOpenProcess(
			&s_attached_process,
			access,
			&attr,
			&client_id);

		assert(NT_SUCCESS(ret));
	}

	auto detach_process() -> void
	{
		NtClose(s_attached_process);
	}

	auto raw_read(std::uintptr_t address, void* buffer, std::size_t size) -> void
	{
		NtReadVirtualMemory(s_attached_process,
			reinterpret_cast<void*>(address),
			buffer,
			size,
			nullptr
		);
	}

	auto raw_write(std::uintptr_t address, void* buffer, std::size_t size) -> void
	{
		NtWriteVirtualMemory(s_attached_process,
			reinterpret_cast<void*>(address),
			buffer,
			size,
			nullptr
		);
	}

	auto read_data(void* dest, void* src, const std::size_t size) -> void
	{
		NtReadVirtualMemory(s_attached_process,
			src,
			dest,
			size,
			nullptr
		);
	}

	auto write_data(void* dest, void* src, const std::size_t size) -> void
	{
		NtWriteVirtualMemory(s_attached_process,
			dest,
			src,
			size,
			nullptr
		);
	}

	auto find_module_by_name(const fnv::hash name) -> std::pair<std::uintptr_t, std::size_t>
	{
		PROCESS_BASIC_INFORMATION basic_information;
		auto ret = NtQueryInformationProcess(s_attached_process,
			ProcessBasicInformation,
			&basic_information,
			sizeof(basic_information),
			nullptr);

		assert(NT_SUCCESS(ret));
		if (!NT_SUCCESS(ret))
			return { 0, 0 };

		xPPEB_LDR_DATA loader_data;
		ret = NtReadVirtualMemory(s_attached_process,
			&(basic_information.PebBaseAddress->Ldr),
			&loader_data,
			sizeof(loader_data),
			nullptr);

		assert(NT_SUCCESS(ret));
		if (!NT_SUCCESS(ret))
			return { 0, 0 };

		const auto list_head = &loader_data->InLoadOrderModuleList;

		PLIST_ENTRY current;
		ret = NtReadVirtualMemory(s_attached_process,
			&(loader_data->InLoadOrderModuleList.Flink),
			&current,
			sizeof(current),
			nullptr);
		assert(NT_SUCCESS(ret));
		if (!NT_SUCCESS(ret))
			return { 0, 0 };

		while (current != list_head)
		{
			xLDR_DATA_TABLE_ENTRY loader_module;
			ret = NtReadVirtualMemory(s_attached_process,
				CONTAINING_RECORD(current, xLDR_DATA_TABLE_ENTRY, InLoadOrderLinks),
				&loader_module,
				sizeof(loader_module),
				nullptr);

			assert(NT_SUCCESS(ret));
			if (!NT_SUCCESS(ret))
				return { 0, 0 };

			wchar_t image_name_wide[MAX_PATH + 1];
			ret = NtReadVirtualMemory(s_attached_process,
				loader_module.BaseDllName.Buffer,
				image_name_wide,
				max(sizeof(image_name_wide), loader_module.BaseDllName.MaximumLength),
				nullptr);
			loader_module.BaseDllName.Buffer = image_name_wide;

			char image_name[MAX_PATH + 1];
			ANSI_STRING image_name_astr
			{
				0,
				sizeof(image_name),
				image_name
			};
			ret = RtlUnicodeStringToAnsiString(&image_name_astr, &loader_module.BaseDllName, FALSE);
			assert(NT_SUCCESS(ret));

			image_name[image_name_astr.Length] = 0;
			for (auto& c : image_name)
				c = tolower(c);
		
			if (fnv::hash_runtime(image_name) == name)
				return
				{
					std::uintptr_t(loader_module.DllBase),
					std::size_t(loader_module.SizeOfImage)
				};

			current = loader_module.InLoadOrderLinks.Flink;
		}

		return { 0, 0 };
	}

	auto find_pattern(const fnv::hash module_name, const char *pattern) -> std::uintptr_t
	{
		auto mod = find_module_by_name(module_name);

		static auto compare_bytes = [](const byte *bytes, const char *pattern) -> bool
		{
			for (; *pattern; *pattern != ' ' ? ++bytes : bytes, ++pattern)
			{
				if (*pattern == ' ' || *pattern == '?')
					continue;

				if (*bytes != GET_BYTE(pattern))
					return false;

				++pattern;
			}

			return true;
		};

		auto get_text_section = [&](std::uintptr_t &start, std::size_t &size)
		{
			auto header = (byte*)malloc(0x1000);
			raw_read(mod.first, header, 0x1000);

			auto pDosHdr = (const IMAGE_DOS_HEADER *)(header);
			if (pDosHdr->e_magic != IMAGE_DOS_SIGNATURE)
			{
				::free(header);
				return false;
			}

			const IMAGE_NT_HEADERS *pImageHdr = (const IMAGE_NT_HEADERS *)((uint8_t *)pDosHdr + pDosHdr->e_lfanew);
			if (pImageHdr->Signature != IMAGE_NT_SIGNATURE)
			{
				::free(header);
				return false;
			}

			auto pSection = (const IMAGE_SECTION_HEADER*)((uint8_t*)pImageHdr + sizeof(IMAGE_NT_HEADERS));
			for (int i = 0; i < pImageHdr->FileHeader.NumberOfSections; ++i, pSection++)
			{
				if (_stricmp((LPCSTR)pSection->Name, ".text") == 0)
				{
					start = (std::uintptr_t)(mod.first + pSection->VirtualAddress);
					size = pSection->Misc.VirtualSize;
					::free(header);
					return true;
				}
			}
			::free(header);

			return false;
		};

		std::uintptr_t base;
		std::size_t size;
		if (!get_text_section(base, size))
		{
			base = mod.first;
			size = mod.second;
		}

		auto pb = (byte *)malloc(size);
		raw_read(base, pb, size);

		std::uintptr_t offset = 0;
		for (auto off = 0UL; off < size; ++off)
		{
			if (compare_bytes(pb + off, pattern))
			{
				offset = base + off;
				break;
			}
		}

		return offset;
	}
}
