#include "platform.h"
#include "schnitzel_lib.h"

#define APIENTRY
#include "glcorearb.h"

#ifdef _WIN32
#include "win32_platform.cpp"
#endif

#include "gl_renderer.h"

int main()
{
	platfrom_create_window(1200,720,"My Clester");
	while (running)
	{
		// Update
		platform_update_window();
	}

	return 0;
}
