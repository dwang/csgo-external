#include "netvar_manager.hpp"
#include "offsets.hpp"
#include <array>
#include <cctype>

namespace sdk
{
	class CRecvProxyData;
	class IClientNetworkable;

	using RecvVarProxyFn = void(*)(const CRecvProxyData* pData, void* pStruct, void* pOut);
	using CreateClientClassFn = IClientNetworkable * (*)(int, int);
	using CreateEventFn = IClientNetworkable * (*)();

	enum SendPropType : int
	{
		DPT_Int = 0,
		DPT_Float,
		DPT_Vector,
		DPT_VectorXY,
		DPT_String,
		DPT_Array,
		DPT_DataTable,
		DPT_Int64,
		DPT_NUMSendPropTypes
	};

	class RecvProp
	{
	public:
		remote::variable<std::array<char, 64>>*	m_pVarName;
		SendPropType m_RecvType;
		int	 m_Flags;
		int m_StringBufferSize;
		bool m_bInsideArray;
		const void* m_pExtraData;
		remote::variable<RecvProp>* m_pArrayProp;
		void* m_ArrayLengthProxy;
		RecvVarProxyFn m_ProxyFn;
		void* m_DataTableProxyFn;
		remote::variable<RecvTable>* m_pDataTable;
		int	m_Offset;
		int	m_ElementStride;
		int m_nElements;
		remote::variable<std::array<char, 64>>*	m_pParentArrayPropName;
	};

	class RecvTable
	{
	public:
		remote::variable<RecvProp>* m_pProps;
		int	m_nProps;
		void* m_pDecoder;
		remote::variable<std::array<char, 64>>*	m_pNetTableName;
		bool m_bInitialized;
		bool m_bInMainList;
	};

	class ClientClass
	{
	public:
		CreateClientClassFn	m_pCreateFn;
		CreateEventFn m_pCreateEventFn;
		remote::variable<std::array<char, 64>>*	m_pNetworkName;
		remote::variable<RecvTable>* m_pRecvTable;
		remote::variable<ClientClass>* m_pNext;
		int m_ClassID;
	};
}

#ifdef _DEBUG
#define IF_DUMPING(...) __VA_ARGS__
#else
#define IF_DUMPING(...)
#endif

IF_DUMPING(static FILE* s_fp;)

netvar_manager::netvar_manager()
{
	auto sig = remote::find_pattern(offsets::get().client_dll, "A1 ? ? ? ? 8B ? 0C 85 C0 74 18");
	auto headclass_ppp = reinterpret_cast<void*>(sig + 1);
	auto headclass_pp = remote::read<void*>(headclass_ppp);
	auto headclass_p = remote::read<remote::variable<sdk::ClientClass>*>(headclass_pp);

	IF_DUMPING(fopen_s(&s_fp, "netvar_dump.txt", "w");)
		for (auto it = headclass_p; it; it = it->get().m_pNext)
		{
			auto clazz = it->get();
			if (clazz.m_pRecvTable)
			{
				auto net_name = clazz.m_pNetworkName->get();
				dump_recursive(net_name.data(), clazz.m_pRecvTable, 0);
			}
		}

	IF_DUMPING(fclose(s_fp);)
}

auto netvar_manager::dump_recursive(const char* base_class, remote::variable<sdk::RecvTable>* table_ptr, const std::uint16_t offset) -> void
{
	const auto table = table_ptr->get();
	for (auto i = 0; i < table.m_nProps; ++i)
	{
		const auto prop_ptr = &table.m_pProps[i];
		const auto prop = prop_ptr->get();

		auto var_name = prop.m_pVarName->get();

		if (!prop_ptr || isdigit(var_name[0]))
			continue;

		if (fnv::hash_runtime(var_name.data()) == FNV("baseclass"))
			continue;

		if (prop.m_RecvType == sdk::DPT_DataTable &&
			prop.m_pDataTable != nullptr &&
			prop.m_pDataTable->get().m_pNetTableName->get()[0] == 'D')
		{
			dump_recursive(base_class, prop.m_pDataTable, std::uint16_t(offset + prop.m_Offset));
		}

		char hash_name[256];

		strcpy_s(hash_name, base_class);
		strcat_s(hash_name, "->");
		strcat_s(hash_name, var_name.data());

		const auto hash = fnv::hash_runtime(hash_name);
		const auto total_offset = std::uint16_t(offset + prop.m_Offset);

		IF_DUMPING(fprintf(s_fp, "%s\t0x%04X\t%s\n", base_class, total_offset, var_name.data());)

		m_props[hash] =
		{
			prop_ptr,
			total_offset
		};
	}
}
