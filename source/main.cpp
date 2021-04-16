#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <3ds.h>
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>

#include "duktape.h"

#include "js_console.hpp"

int run_file(duk_context* context, std::string& filename) {
	std::ifstream file(filename);
	std::stringstream buffer;
	buffer << file.rdbuf();
	std::string contents = buffer.str();

	duk_push_string(context, filename.c_str());
	if (duk_pcompile_string_filename(context, 0, contents.c_str())) {
		return 1;
	}

	return duk_pcall(context, 0);
}

int main(int argc, char* argv[])
{
	gfxInitDefault();
	consoleInit(GFX_TOP, NULL);

	if (romfsInit()) { exit(1); }

	duk_context* context = duk_create_heap_default();
	if (!context) { exit(1); }

	duk_console_init(context, DUK_CONSOLE_FLUSH);

	std::string filename = "romfs:/index.js";
	if (run_file(context, filename)) {
		printf("[ERROR][JS] %s\n", duk_safe_to_string(context, -1));
	}

	// if (duk_peval_string(context, "console.log('hello!\\n')")) {
	// 	printf("[ERROR][JS] %s\n", duk_safe_to_string(context, -1));
	// }

	// Main loop
	while (aptMainLoop())
	{
		gspWaitForVBlank();
		gfxSwapBuffers();
		hidScanInput();

		// Your code goes here
		u32 kDown = hidKeysDown();
		if (kDown & KEY_START)
			break; // break in order to return to hbmenu
	}

	duk_destroy_heap(context);

	romfsExit();

	gfxExit();
	return 0;
}
