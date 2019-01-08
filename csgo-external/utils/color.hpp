#pragma once

struct color {
	float r;
	float g;
	float b;
	float a;

	color()
	{
	}

	color(float red, float green, float blue, float alpha) {
		r = red;
		g = green;
		b = blue;
		a = alpha;
	}
};
