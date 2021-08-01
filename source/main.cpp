#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <3ds.h>
#include <unistd.h>
#include <sys/stat.h>

#include <iostream>
#include <fstream>
#include <string>
#include <sstream>

#include "duktape.h"
#include "events/loop.hpp"

#include "ev.hpp"
#include "js.hpp"

#define RUN_TESTSUITE_JS 1

// forward declarations
void init_app();
void terminate_app();

int run_file(duk_context* context, std::string& filename);

int main(int argc, char* argv[]) {
    init_app();

#if RUN_TESTSUITE_JS
    do {
        std::string testsuite = "romfs:/testsuite.js";
        if (js::run_file(testsuite)) {
            printf("[ERROR][JS] %s\n", duk_safe_to_string(js::context, -1));
        }
    } while(0);
#endif

    std::string filename = "romfs:/index.js";
    if (js::run_file(filename)) {
        printf("[ERROR][JS] %s\n", duk_safe_to_string(js::context, -1));
    }

    while (aptMainLoop()) {
        gspWaitForVBlank();
        gfxSwapBuffers();
        hidScanInput();

        u32 kDown = hidKeysDown();
        if (kDown & KEY_START)
            break;

        ev::loop->process();
    }

    terminate_app();
    return 0;
}

void init_app() {
    gfxInitDefault();
    consoleInit(GFX_TOP, NULL);
    if (romfsInit()) exit(1);
    if (fsInit()) exit(1);

    ev::init();
    js::init();
}

void terminate_app() {
    js::terminate();
    ev::terminate();

    fsExit();
    romfsExit();
    gfxExit();
}

