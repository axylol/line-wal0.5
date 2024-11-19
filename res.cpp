#include "res.h"
#include "shared/line.h"
#include <string.h>
#include "lib.h"
#include <math.h>

int (*LUA_GETGLOBAL)(void*, const char*);
void (*LUA_SETGLOBAL)(void*, const char*);
void (*LUA_PUSHNUMBER)(void*, double);

int lua_getglobal(void* state, const char* global) {
    if (!strcmp(global, "SCREEN_XSIZE")) {
        LUA_PUSHNUMBER(state, CONFIG.width);
		LUA_SETGLOBAL(state, global);
    } else if (!strcmp(global, "SCREEN_YSIZE")) {
        LUA_PUSHNUMBER(state, CONFIG.height);
		LUA_SETGLOBAL(state, global);
    } else if (!strcmp(global, "MINIMAP_DISP_X")) {
        LUA_PUSHNUMBER(state, round((double)CONFIG.width * 0.0265625));
		LUA_SETGLOBAL(state, global);
    } else if (!strcmp(global, "MINIMAP_DISP_Y")) {
        LUA_PUSHNUMBER(state, round((double)CONFIG.height * 0.2364));
		LUA_SETGLOBAL(state, global);
    }

	return LUA_GETGLOBAL(state, global);
}

void* (*ORIGINAL_SET_VIEWPORT)(void*, int, int, int, int, float, float);
void* set_viewport(void* _this, int a1, int a2, int width, int height, float a5, float a6) {
	if (width == 88 && height == 82) {
		float width = (float)CONFIG.width * 0.1375f;
		float height = (float)CONFIG.height* 0.17f;
		return ORIGINAL_SET_VIEWPORT(
			_this,
			a1,
			a2,
			width,
			height,
			a5,
			a6
		);
	}
	return ORIGINAL_SET_VIEWPORT(_this, a1, a2, width, height, a5, a6);
}

void (*ORIGINAL_MAKE_PERSPECTIVE)(void*, float, float, float, float, float);
void make_perspective(void* _this, float fov, float a2, float aspect_ratio, float a4, float a5) {
	float width = CONFIG.width;
	float height = CONFIG.height;
	float original_aspect_ratio = 640.0 / 480.0;
    if (aspect_ratio == original_aspect_ratio)
        aspect_ratio = width / height;
	fov = fov / original_aspect_ratio * aspect_ratio;
	ORIGINAL_MAKE_PERSPECTIVE(_this, fov, a2, aspect_ratio, a4, a5);
}

void res::init() {
    Line::Hook(
        Line::DlSym(NULL, "lua_getglobal"),
        (void*)lua_getglobal,
        (void**)&LUA_GETGLOBAL
    );
    *(void**)&LUA_SETGLOBAL = Line::DlSym(NULL, "lua_setglobal");
    *(void**)&LUA_PUSHNUMBER = Line::DlSym(NULL, "lua_pushnumber");

    Line::Hook(
        Line::DlSym(NULL, "_ZN3Gap3Gfx19igAGLEVisualContext11setViewportEiiiiff"),
        (void*)set_viewport,
        (void**)&ORIGINAL_SET_VIEWPORT
    );
    Line::Hook(
        Line::DlSym(NULL, "_ZN3Gap4Math11igMatrix44f32makePerspectiveProjectionRadiansEfffff"),
        (void*)make_perspective,
        (void**)&ORIGINAL_MAKE_PERSPECTIVE
    );
}