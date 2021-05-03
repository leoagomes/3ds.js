#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <3ds.h>

#include <iostream>
#include <fstream>
#include <string>
#include <sstream>

#include "duktape.h"
#include "event_loop.hpp"

#include "globals.hpp"
#include "ev.hpp"
#include "js.hpp"

// forward declarations
void init_app();
void terminate_app();

int run_file(duk_context* context, std::string& filename);

int main(int argc, char* argv[]) {
    init_app();

    std::string filename = "romfs:/index.js";
    if (js::run_file(filename)) {
        printf("[ERROR][JS] %s\n", duk_safe_to_string(js::context, -1));
    }

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

        ev::loop->process();
    }

    terminate_app();
    return 0;
}

void init_app() {
    gfxInitDefault();
    consoleInit(GFX_TOP, NULL);
    if (romfsInit()) exit(1);

    ev::init();
    js::init();
}

void terminate_app() {
    js::terminate();
    ev::terminate();

    romfsExit();
    gfxExit();
}

