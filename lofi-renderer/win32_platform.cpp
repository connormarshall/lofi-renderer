#include <windows.h>
#include <time.h>

bool running = true;

struct Render_State {
	//	Pointer to rendering buffer in memory
	void* memory;
	//	Current dimensions of the rendering buffer
	int width, height, size;
	//	Bitmap info for buffer
	BITMAPINFO bitmap_info;

};

struct Colour_Palette {
	unsigned int onyx = 0x36393B;
	unsigned int baby_blue = 0xA5D8FF;
	unsigned int light_blue = 0xAFD0D6;
	unsigned int black_shadows = 0xBFB6BB;
	unsigned int tuscany = 0xC49799;
};

Render_State render_state;
Colour_Palette colours;

#include "render.cpp"
#include "3d.cpp"

//	Callback for processing messages between window and user
LRESULT CALLBACK window_callback(_In_ HWND hwnd, _In_ UINT uMsg, _In_ WPARAM wParam, _In_ LPARAM lParam) {
	
	LRESULT result = 0;

	//	Handling different user messages
	switch (uMsg) {
		//	If uMsg is to close or destroy the window, stop running
		case WM_CLOSE:
		case WM_DESTROY:
			running = false;
			break;
		//	If window resized
		case WM_SIZE:
			//	Get a rect representing the new size of the window
			RECT rect;
			GetClientRect(hwnd, &rect);
			//	Allocate some memory to serve as a newly-sized rendering buffer
			render_state.width = rect.right - rect.left;
			render_state.height = rect.bottom - rect.top;
			
			render_state.size = render_state.width * render_state.height * sizeof(unsigned int);
			
			//	If the buffer already exists, free up the current memory it's stored in
			if (render_state.memory) VirtualFree(render_state.memory, 0, MEM_RELEASE);

			//	Buffer bitmap: how the raw buffer data should be rendered
			render_state.bitmap_info.bmiHeader.biSize = sizeof(render_state.bitmap_info.bmiHeader);
			render_state.bitmap_info.bmiHeader.biWidth = render_state.width;
			render_state.bitmap_info.bmiHeader.biHeight = render_state.height;
			render_state.bitmap_info.bmiHeader.biPlanes = 1;
			render_state.bitmap_info.bmiHeader.biBitCount = 32;
			render_state.bitmap_info.bmiHeader.biCompression = BI_RGB;

			render_state.memory = VirtualAlloc(
				0, //	Ip address (none)
				render_state.size, //	Size of memory 
				MEM_COMMIT | MEM_RESERVE, //	What we want to do with it
				PAGE_READWRITE //	Permissions for this memory	
			);

			break;
		
		default:
			//	returns info to window
			result = DefWindowProc(hwnd, uMsg, wParam, lParam);
			break;
	}

	return result;

};

//	Application entry point
int WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd) {

	//	Window Class
	WNDCLASS window_class = {};
	//	Redraw window horizontally or vertically
	window_class.style = CS_HREDRAW || CS_VREDRAW;
	//	Window name
	window_class.lpszClassName = "Window Class";
	//	Callback for passing messages to the user
	window_class.lpfnWndProc = window_callback;


	//	Register Window Class
	RegisterClass(&window_class);


	//	Create Window
	HWND window = CreateWindow(
		window_class.lpszClassName, //	Name of window class
		"Lo-Fi Renderer", //	Window name
		WS_OVERLAPPEDWINDOW | WS_VISIBLE, //	Window settings
		CW_USEDEFAULT,	//	X, Y co-ords of window on create
		CW_USEDEFAULT,
		600,	//	Window dimensions
		600,
		0,	//	Window Parent (none)
		0,	//	Window Menu (none)
		hInstance,	//	Window instance
		0	//	lpParam: points to a CREATESTRUCT struct containing additional settings
	);

	//	Window device context
	HDC hdc = GetDC(window);


	//	Timing
	float delta_time = 0.016666f;
	LARGE_INTEGER frame_begin_time, program_begin_time;
	QueryPerformanceCounter(&frame_begin_time);
	program_begin_time = frame_begin_time;

	float performance_frequency;
	{
		LARGE_INTEGER perf;
		QueryPerformanceFrequency(&perf);
		performance_frequency = (float)perf.QuadPart;
	
	}

	//	Mesh init
	mesh cubeMesh;

	cubeMesh.tris = {

		// SOUTH
		{ 0.0f, 0.0f, 0.0f,    0.0f, 1.0f, 0.0f,    1.0f, 1.0f, 0.0f },
		{ 0.0f, 0.0f, 0.0f,    1.0f, 1.0f, 0.0f,    1.0f, 0.0f, 0.0f },

		// EAST                                                      
		{ 1.0f, 0.0f, 0.0f,    1.0f, 1.0f, 0.0f,    1.0f, 1.0f, 1.0f },
		{ 1.0f, 0.0f, 0.0f,    1.0f, 1.0f, 1.0f,    1.0f, 0.0f, 1.0f },

		// NORTH                                                     
		{ 1.0f, 0.0f, 1.0f,    1.0f, 1.0f, 1.0f,    0.0f, 1.0f, 1.0f },
		{ 1.0f, 0.0f, 1.0f,    0.0f, 1.0f, 1.0f,    0.0f, 0.0f, 1.0f },

		// WEST                                                      
		{ 0.0f, 0.0f, 1.0f,    0.0f, 1.0f, 1.0f,    0.0f, 1.0f, 0.0f },
		{ 0.0f, 0.0f, 1.0f,    0.0f, 1.0f, 0.0f,    0.0f, 0.0f, 0.0f },

		// TOP                                                       
		{ 0.0f, 1.0f, 0.0f,    0.0f, 1.0f, 1.0f,    1.0f, 1.0f, 1.0f },
		{ 0.0f, 1.0f, 0.0f,    1.0f, 1.0f, 1.0f,    1.0f, 1.0f, 0.0f },

		// BOTTOM                                                    
		{ 1.0f, 0.0f, 1.0f,    0.0f, 0.0f, 1.0f,    0.0f, 0.0f, 0.0f },
		{ 1.0f, 0.0f, 1.0f,    0.0f, 0.0f, 0.0f,    1.0f, 0.0f, 0.0f },
	
	};


	// Camera
	vec3d vCamera;
	vCamera.x = 0; vCamera.y = 0; vCamera.z = 0;

	//	Projection Matrix

	float fNear = 0.1f;
	float fFar = 1000.0f;
	float fFov = 90.0f;
	float fAspectRatio = (float)render_state.height / (float)render_state.width;
	float fFovRad = 1.0f / tanf(fFov * 0.5f / 180.0f * 3.14159f);

	matrix4x4 matProj; //	Projection matrix for rendering scene to camera

	matProj.m[0][0] = fAspectRatio * fFovRad;
	matProj.m[1][1] = fFovRad;
	matProj.m[2][2] = fFar / (fFar - fNear);
	matProj.m[3][2] = (-fFar * fNear) / (fFar - fNear);
	matProj.m[2][3] = 1.0f;
	matProj.m[3][3] = 0.0f;

	float fTheta = 0.0f;

	//	Window runtime loop
	while (running) {


		//	INPUT

		//	Handle messages from the user
		MSG message;

		while (PeekMessage(&message, window, 0, 0, PM_REMOVE)) {
			TranslateMessage(&message);
			DispatchMessage(&message);
		}

		fTheta += 1.0f * delta_time;

		//	SIMULATE
		clear_screen(colours.onyx);

		matrix4x4 matRotZ, matRotX;

		// Rotation Z
		matRotZ.m[0][0] = cosf(fTheta);
		matRotZ.m[0][1] = sinf(fTheta);
		matRotZ.m[1][0] = -sinf(fTheta);
		matRotZ.m[1][1] = cosf(fTheta);
		matRotZ.m[2][2] = 1;
		matRotZ.m[3][3] = 1;

		// Rotation X
		matRotX.m[0][0] = 1;
		matRotX.m[1][1] = cosf(fTheta * 0.5f);
		matRotX.m[1][2] = sinf(fTheta * 0.5f);
		matRotX.m[2][1] = -sinf(fTheta * 0.5f);
		matRotX.m[2][2] = cosf(fTheta * 0.5f);
		matRotX.m[3][3] = 1;

		// Draw Triangles
		for (auto tri : cubeMesh.tris) {

			triangle triProjected, triTranslated, triRotatedZ, triRotatedZX;

			// Rotate in Z-Axis
			matrixmult(tri.p[0], triRotatedZ.p[0], matRotZ);
			matrixmult(tri.p[1], triRotatedZ.p[1], matRotZ);
			matrixmult(tri.p[2], triRotatedZ.p[2], matRotZ);

			// Rotate in X-Axis
			matrixmult(triRotatedZ.p[0], triRotatedZX.p[0], matRotX);
			matrixmult(triRotatedZ.p[1], triRotatedZX.p[1], matRotX);
			matrixmult(triRotatedZ.p[2], triRotatedZX.p[2], matRotX);

			// Offset into the screen
			triTranslated = triRotatedZX;
			triTranslated.p[0].z = triRotatedZX.p[0].z + 3.0f;
			triTranslated.p[1].z = triRotatedZX.p[1].z + 3.0f;
			triTranslated.p[2].z = triRotatedZX.p[2].z + 3.0f;

			// Use Cross-Product to get surface normal
			vec3d normal, line1, line2;
			line1.x = triTranslated.p[1].x - triTranslated.p[0].x;
			line1.y = triTranslated.p[1].y - triTranslated.p[0].y;
			line1.z = triTranslated.p[1].z - triTranslated.p[0].z;

			line2.x = triTranslated.p[2].x - triTranslated.p[0].x;
			line2.y = triTranslated.p[2].y - triTranslated.p[0].y;
			line2.z = triTranslated.p[2].z - triTranslated.p[0].z;

			normal.x = line1.y * line2.z - line1.z * line2.y;
			normal.y = line1.z * line2.x - line1.x * line2.z;
			normal.z = line1.x * line2.y - line1.y * line2.x;

			// Normalize the vector
			float l = sqrtf(normal.x * normal.x + normal.y * normal.y + normal.z * normal.z);
			normal.x /= l; normal.y /= l; normal.z /= l;

			//if (normal.z < 0) 
			if (normal.x * (triTranslated.p[0].x - vCamera.x) +
				normal.y * (triTranslated.p[0].y - vCamera.y) +
				normal.z * (triTranslated.p[0].z - vCamera.z) 
				< 0.0f)
			{

				// Project triangles from 3D --> 2D
				matrixmult(triTranslated.p[0], triProjected.p[0], matProj);
				matrixmult(triTranslated.p[1], triProjected.p[1], matProj);
				matrixmult(triTranslated.p[2], triProjected.p[2], matProj);

				// Scale into view
				triProjected.p[0].x += 1.0f; triProjected.p[0].y += 1.0f;
				triProjected.p[1].x += 1.0f; triProjected.p[1].y += 1.0f;
				triProjected.p[2].x += 1.0f; triProjected.p[2].y += 1.0f;
				triProjected.p[0].x *= 0.5f * (float)render_state.width;
				triProjected.p[0].y *= 0.5f * (float)render_state.height;
				triProjected.p[1].x *= 0.5f * (float)render_state.width;
				triProjected.p[1].y *= 0.5f * (float)render_state.height;
				triProjected.p[2].x *= 0.5f * (float)render_state.width;
				triProjected.p[2].y *= 0.5f * (float)render_state.height;

				// Rasterize triangle
				draw_triangle(
					triProjected.p[0].x, triProjected.p[0].y,
					triProjected.p[1].x, triProjected.p[1].y,
					triProjected.p[2].x, triProjected.p[2].y,
					colours.baby_blue
				);
			}
		}

		
		/*
		draw_rect_by_pixels(20, 20, 80, 80, colours.baby_blue);
		draw_rect_by_pixels(20, 100, 80, 160, colours.tuscany);
		draw_rect_by_pixels(20, 180, 80, 240, colours.light_blue);
		draw_rect_by_pixels(20, 260, 80, 320, colours.black_shadows);
		*/



		//	RENDER

		StretchDIBits(
			hdc,
			0,
			0,
			render_state.width,
			render_state.height,
			0,
			0,
			render_state.width,
			render_state.height,
			render_state.memory,
			&render_state.bitmap_info,
			DIB_RGB_COLORS,
			SRCCOPY
		);

		LARGE_INTEGER frame_end_time;
		QueryPerformanceCounter(&frame_end_time);
		delta_time = (float)(frame_end_time.QuadPart - frame_begin_time.QuadPart) / performance_frequency;
		frame_begin_time = frame_end_time;
	}

}
