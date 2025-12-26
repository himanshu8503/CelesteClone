#include "platform.h"
#include "schnitzel_lib.h"
#include "input.h"

#include "game.cpp"


#define APIENTRY
#define GL_GLEXT_PROTOTYPES
#include "glcorearb.h"

#ifdef _WIN32
#include "win32_platform.cpp"
#endif

#include "gl_renderer.cpp"


int main()
{
	BumpAllocator transientStorage = make_bump_allocator(MB(50));
	platfrom_create_window(1200,720,"My Clester");
	input.ScreenSizeX = 1200;
	input.ScreenSizeY = 720;

	if(!gl_Init(&transientStorage))
	{
		SM_ASSERT(false,"CanNot initialize Opengl things for the window");
		return 0;
	}
	

	while (running)
	{
		// Update
		platform_update_window();
		update_game();
		gl_render();
		

		platform_Swap_Buffer();
	}

	return 0;
}
