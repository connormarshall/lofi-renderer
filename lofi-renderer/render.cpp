
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

void draw_line(int x1, int y1, int x2, int y2, u32 colour) {
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

void fill_triangle(int x1, int y1, int x2, int y2, int x3, int y3, u32 colour) {

	//	Scan fill algorithm
	//	Step 1: get yMin and yMax

	//	Step 2: get intersection points of a ScanLine with each edge

	//	Step 3: sort intersection points in order of increasing x

	//	Step 4: draw line on alternating pairs of intersection points

	//	Step 5: increase ScanLine y-pos by 1, loop back to Step 2 until yMax is reached

}