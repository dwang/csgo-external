#pragma once

struct color
{
	float r;
	float g;
	float b;
	float a;

	color()
	{
	}

	color(float red, float green, float blue, float alpha)
	{
		r = red;
		g = green;
		b = blue;
		a = alpha;
	}
};

struct model_color
{
	unsigned char r;
	unsigned char g;
	unsigned char b;

	model_color()
	{
	}

	model_color(unsigned char red, unsigned char green, unsigned char blue)
	{
		r = red;
		g = green;
		b = blue;
	}
};
