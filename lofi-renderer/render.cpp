#include <math.h>
#include "utils.cpp"

using namespace std;

//
//	Rendering functions
//


void render_background() {
	//	Gets the first pixel of the buffer
	u32* pixel = (u32*)render_state.memory;
	//	Renders out for every pixel of the buffer
	for (int y = 0; y < render_state.height; y++) {
		for (int x = 0; x < render_state.width; x++) {
			*pixel++ = 0xff00ff * x + 0x00ff00 * y;

		}
	}
}

void clear_screen(u32 colour) {
	//	Gets the first pixel of the buffer
	u32* pixel = (u32*)render_state.memory;
	//	Renders out for every pixel of the buffer
	for (int y = 0; y < render_state.height; y++) {
		for (int x = 0; x < render_state.width; x++) {
			*pixel++ = colour;
		}
	}
}

void draw_rect_by_pixels(int x1, int y1, int x2, int y2, u32 colour) {
	u32* pixel;

	//	Clamp co-ords to prevent attempts to draw outside of buffer

	x1 = clamp(0, x1, render_state.width);
	y1 = clamp(0, y1, render_state.height);
	x2 = clamp(0, x2, render_state.width);
	y2 = clamp(0, y2, render_state.height);


	//	Renders out for every pixel of the buffer
	for (int y = y1; y < y2; y++) {
		//	Gets the first pixel of the buffer, move x1 along, and y down ( * length of one row)
		pixel = (u32*)render_state.memory + x1 + (y * render_state.width);
		for (int x = x1; x < x2; x++) {
			*pixel++ = colour;
		}
	}
}

inline void plot_pixel(int x, int y, u32 colour) {
	x = clamp(0, x, render_state.width);
	y = clamp(0, y, render_state.height);

	u32* pixel = (u32*)render_state.memory + x + ( (y - 1) * render_state.width);
	*pixel = colour;

}

inline void draw_line(int x1, int y1, int x2, int y2, u32 colour) {
	u32* pixel;

	x1 = clamp(0, x1, render_state.width);
	y1 = clamp(0, y1, render_state.height);
	x2 = clamp(0, x2, render_state.width);
	y2 = clamp(0, y2, render_state.height);


	int dx = x2 - x1;
	int dy = y2 - y1;

	if (dx == 0) { //	Vertical line

		int x = x1;

		if (y1 < y2) {
			for (int y = y1; y < y2; y++) {
				plot_pixel(x, y, colour);
			}
		}
		if (y1 > y2) {
			for (int y = y1; y > y2; y--) {
				plot_pixel(x, y, colour);
			}
		}
		return;
	}

	if (dy == 0) { //	Horizontal line

		int y = y1;

		if (x1 < x2) {
			for (int x = x1; x < x2; x++) {
				plot_pixel(x, y, colour);
			}
		}
		if(x1 > x2) {
			for (int x = x1; x > x2; x--) {
				plot_pixel(x, y, colour);
			}
		}
		return;
	}

	int x = x1;
	bool drawing = true;

	while (drawing) {
		int y = y1 + dy * (x - x1) / dx;

		plot_pixel(x, y, colour);

		if (x < x2) {
			x++;

		} else {
			x--;
		}

		(x != x2) ? drawing = true : drawing = false;

	}

	
}

void draw_triangle(int x1, int y1, int x2, int y2, int x3, int y3, u32 colour) {
	
	draw_line(x1, y1, x2, y2, colour);
	draw_line(x2, y2, x3, y3, colour);
	draw_line(x3, y3, x1, y1, colour);

}


//	Gets the max and min y positions of a line
void yMinMax(int y1, int y2, int &yMin, int &yMax) {

	(y2 > y1) ? yMax = y2, yMin = y1 : yMax = y1, yMin = y2;

}

struct vertice {
	int x, y;
};

struct edge {
	vertice v1, v2;
	int yMin, yMax;
};

/*
void x () {
	float invslope1 = (v2.x - v1.x) / (v2.y - v1.y);
	float invslope2 = (v3.x - v1.x) / (v3.y - v1.y);

	float curx1 = v1.x;
	float curx2 = v1.x;

	for (int scanlineY = v1.y; scanlineY <= v2.y; scanlineY++)
	{
		drawLine((int)curx1, scanlineY, (int)curx2, scanlineY);
		curx1 += invslope1;
		curx2 += invslope2;
	}
}

*/

//	Fill a triangle with a flat (dy = 0) bottom edge
void fill_bottom_flat_tri(int x1, int y1, int x2, int y2, int x3, int y3, u32 colour) {
	//	v1 is the top vertice
	
	float invslope1 = (float) (x2 - x1) / (float) (y2 - y1);
	float invslope2 = (float) (x3 - x1) / (float) (y3 - y1);

	float curx1 = x1;
	float curx2 = x1;

	// draw_rect_by_pixels(20, 20, 80, 80, colours.baby_blue);

	//	Iteratively draw a scanning line down the triangle
	for (int scanlineY = y1; scanlineY <= y2; scanlineY++) {
		draw_line( (int)curx1, scanlineY, (int)curx2, scanlineY, colour);
		curx1 += invslope1;
		curx2 += invslope2;
	}

}

void fill_top_flat_tri(int x1, int y1, int x2, int y2, int x3, int y3, u32 colour) {
	//	v1 is the bottom vertice

	float invslope1 = (float)(x3 - x1) / (float)(y3 - y1);
	float invslope2 = (float)(x3 - x2) / (float)(y3 - y2);

	float curx1 = x3;
	float curx2 = x3;

	// draw_rect_by_pixels(20, 20, 80, 80, colours.baby_blue);

	//	Iteratively draw a scanning line down the triangle
	for (int scanlineY = y3; scanlineY > y2; scanlineY--) {
		draw_line((int)curx1, scanlineY, (int)curx2, scanlineY, colour);
		curx1 -= invslope1;
		curx2 -= invslope2;
	}

}


void fill_triangle(int x1, int y1, int x2, int y2, int x3, int y3, u32 colour) {

	//get length of all sides
	float d1 = sqrt( ((y2 - y1) * (y2 - y1)) + ((x2 - x1) * (x2 - x1)) );
	float d2 = sqrt( ((y3 - y2) * (y3 - y2)) + ((x3 - x2) * (x3 - x2)) );
	float d3 = sqrt( ((y1 - y3) * (y1 - y3)) + ((x1 - x3) * (x1 - x3)) );

	if (((d1 < d2) or (d1 = d2)) and ((d1 < d2) or (d1 = d2))) //the first side is the shortest
	{
		float tx = x1;
		float ty = y1;
		float vx = (x2 - x1) / d1;
		float vy = (y2 - y1) / d1;
		int counter = 0;

		while (counter < d1) {
			draw_line(x3, y3, tx, ty, colour);
			//drawing a line from point(x3,y3) to point(tx,ty).
			tx = tx + vx;
			ty = ty + vy;
			counter = counter + 1;
		}
	}
	else if ((d2 < d3) or (d2 = d3)) //the second side is the shortest
	{
		float tx = x2;
		float ty = y2;
		float vx = (x3 - x2) / d2;
		float vy = (y3 - y2) / d2;
		int counter = 0;

		while (counter < d2) {
			draw_line(x1, y1, tx, ty, colour);
			tx = tx + vx;
			ty = ty + vy;
			counter = counter + 1;
		}
	}
	else // the third side is shortest
	{
		float tx = x3;
		float ty = y3;
		float vx = (x1 - x3) / d3;
		float vy = (y1 - y3) / d3;
		int counter = 0;
		while (counter < d3) {
			draw_line(x2, y2, tx, ty, colour);
			tx = tx + vx;
			ty = ty + vy;
			counter = counter + 1;
		}
	}
}