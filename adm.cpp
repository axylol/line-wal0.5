#include "adm.h"
#include <string.h>
#include <stdint.h>
#include "jamma.h"
#include "opengl.h"
#include "shared/line.h"
#include "lib.h"
#include <GLFW/glfw3.h>
#define GLFW_EXPOSE_NATIVE_WIN32
#include <GLFW/glfw3native.h>
#include <GL/gl.h>
#include <GL/glext.h>
#include "common.h"

const char *adm_version()
{
    return "WanganArcadeLoader 0.1";
}

#pragma pack(push, 1)
struct AdmChooseMode
{
    uint8_t ident[4];
    uint32_t unk_0x04;
    uint32_t unk_0x08;
    uint32_t unk_0x0C;
    uint32_t unk_0x10;
    uint32_t unk_0x14;
    uint32_t width;
    uint32_t height;
    uint32_t refresh;
};

struct AdmWindow
{
    uint8_t ident[4];
    void *glfw;
    GLFWwindow *window;
    uint32_t fbo;
};
#pragma pack(pop)

AdmChooseMode **adm_config()
{
    AdmChooseMode *adm = new AdmChooseMode;
    memset(adm, 0, sizeof(AdmChooseMode));
    memcpy(adm->ident, "MOCF", 4);
    adm->refresh = 60 * 1000;
    adm->width = CONFIG.width;
    adm->height = CONFIG.height;

    AdmChooseMode **adm_ptr = new AdmChooseMode *[1];
    adm_ptr[0] = adm;
    return adm_ptr;
}

uint32_t *adm_fb_config()
{
    uint32_t *d = new uint32_t[1];
    *d = 0;
    return d;
}

GLFWwindow *window;
uint32_t fbo;
uint32_t texture;
void* window_hwnd = NULL;

extern void skibidigfx_glGenFramebuffers(GLsizei n, GLuint *framebuffers);
extern void skibidigfx_glBindFramebuffer(GLenum target, GLuint framebuffer);
extern void skibidigfx_glGenTextures(GLsizei n, GLuint *textures);
extern void skibidigfx_glBindTexture(GLenum target, GLuint texture);
extern void skibidigfx_glTexImage2D(GLenum target, GLint level, GLint internalformat, GLsizei width, GLsizei height, GLint border, GLenum format, GLenum type, const GLvoid *pixels);
extern void skibidigfx_glTexParameteri(GLenum target, GLenum pname, GLint param);
extern void skibidigfx_glFramebufferTexture2D(GLenum target, GLenum attachment, GLenum textarget, GLuint texture, GLint level);
extern void skibidigfx_glBlitFramebuffer(GLint srcX0, GLint srcY0, GLint srcX1, GLint srcY1, GLint dstX0, GLint dstY0, GLint dstX1, GLint dstY1, GLbitfield mask, GLenum filter);
extern void skibidigfx_glClear(GLbitfield mask);

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    jamma::key_callback(key, scancode, action, mods);
}

AdmWindow *adm_window()
{
    if (!glfwInit())
        return NULL;
    glfwWindowHint(GLFW_RESIZABLE, true);
    window = glfwCreateWindow(CONFIG.width, CONFIG.height, "WanganArcadeLoader", nullptr, nullptr);
    if (!window)
        return NULL;
    if (CONFIG.fullscreen)
        glfwSetWindowMonitor(window, glfwGetPrimaryMonitor(), 0, 0, CONFIG.width, CONFIG.height, GLFW_DONT_CARE);

    glfwMakeContextCurrent(window);
    glfwSwapInterval(1); // Enable vsync
    
    glfwSetKeyCallback(window, key_callback);

    window_hwnd = glfwGetWin32Window(window);

    opengl::load_gl_funcs();

    skibidigfx_glGenFramebuffers(1, &fbo);
    skibidigfx_glBindFramebuffer(GL_FRAMEBUFFER, fbo);

    skibidigfx_glGenTextures(1, &texture);
    skibidigfx_glBindTexture(GL_TEXTURE_2D, texture);
    skibidigfx_glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, CONFIG.width, CONFIG.height, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
    skibidigfx_glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    skibidigfx_glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    skibidigfx_glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texture, 0);
    skibidigfx_glBindTexture(GL_TEXTURE_2D, 0);
    skibidigfx_glBindFramebuffer(GL_FRAMEBUFFER, 0);

    AdmWindow *adm = new AdmWindow;
    memset(adm, 0, sizeof(AdmWindow));
    return adm;
}

int adm_swap_buffers(AdmWindow *window_ptr)
{
    uint16_t* graphics = *(uint16_t**)Line::DlSym(NULL, "_ZN11teSingletonI10clGraphicsE11sm_instanceE");

    bool should_blit = false;
    if (IS_BASE_MT3) {
        should_blit = *(uint16_t*)((uint8_t*)graphics + 0x48) != 0;
    } else {
        if (*(uint16_t*)((uint8_t*)graphics + 0x54) != 0) {
            should_blit = true;
        } else {
            uint32_t* buffer = *(uint32_t**)((uint8_t*)graphics + 0x0C);
            if (!buffer)
                buffer = *(uint32_t**)((uint8_t*)graphics + 0x08);

            if (buffer[1] == 0)
                should_blit = true;
        }
    }

    if (should_blit) {
        int window_width, window_height;
        glfwGetWindowSize(window, &window_width, &window_height);

        float window_ar = (float)window_width / (float)window_height;
        float ar = (float)CONFIG.width / (float)CONFIG.height;

        int viewport_width, viewport_height, viewport_x, viewport_y;

        if (window_ar > ar) {
            viewport_width = (float)window_height * ar;
            viewport_height = window_height;
            viewport_x = float(window_width - viewport_width) / 2.f;
            viewport_y = 0;
        } else {
            viewport_width = window_width;
            viewport_height = (float)window_width / ar;
            viewport_x = 0;
            viewport_y = float(window_height - viewport_height) / 2.f;
        }

        skibidigfx_glBindFramebuffer(GL_READ_FRAMEBUFFER, 0);
        skibidigfx_glBindFramebuffer(GL_DRAW_FRAMEBUFFER, fbo);
        skibidigfx_glBlitFramebuffer(0, 0, CONFIG.width, CONFIG.height, 0, 0, CONFIG.width, CONFIG.height, GL_COLOR_BUFFER_BIT, GL_NEAREST);
        skibidigfx_glBindFramebuffer(GL_FRAMEBUFFER, 0);
        skibidigfx_glClear(GL_COLOR_BUFFER_BIT);

        skibidigfx_glBindFramebuffer(GL_READ_FRAMEBUFFER, fbo);
        skibidigfx_glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
        skibidigfx_glBlitFramebuffer(0, 0, CONFIG.width, CONFIG.height, viewport_x, viewport_y, viewport_x + viewport_width, viewport_y + viewport_height, GL_COLOR_BUFFER_BIT, GL_NEAREST);

        skibidigfx_glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }

    glfwSwapBuffers(window);
    glfwPollEvents();
    return 0;
}

void *(*CL_APP_INSTANCE)();
bool (*CL_APP_IS_MAIN_THREAD)(void *);
void ***CL_MAIN_INSTANCE;
void *(*THREAD_MANAGER_CURRENT)(void *);
void (*CALL_FROM_MAIN_THREAD)(void *, void *, void *);

void (*ORIGINAL_DEL_SPRITE_MANAGER)(void *);
void del_sprite_manager(void *_this)
{
    void *cl_app = CL_APP_INSTANCE();
    if (CL_APP_IS_MAIN_THREAD(cl_app))
    {
        ORIGINAL_DEL_SPRITE_MANAGER(_this);
    }
    else
    {
        void *thread_manager = *(void **)((uint8_t *)*CL_MAIN_INSTANCE + 0x40);
        void *current = THREAD_MANAGER_CURRENT(thread_manager);
        CALL_FROM_MAIN_THREAD(current, (void *)del_sprite_manager, _this);
    }
}

void (*ORIGINAL_SAVE_IMAGE)(void *, void *);
void save_image_main(void **args)
{
    void *cl_app = CL_APP_INSTANCE();
    if (CL_APP_IS_MAIN_THREAD(cl_app))
    {
        ORIGINAL_SAVE_IMAGE(args[0], args[1]);
        free(args);
    }
    else
    {
        panic("Not main thread!");
    }
}
void save_image(void *render_buffer, void *filepath)
{
    void *cl_app = CL_APP_INSTANCE();
    if (CL_APP_IS_MAIN_THREAD(cl_app))
    {
        ORIGINAL_SAVE_IMAGE(render_buffer, filepath);
    }
    else
    {
        void **args = (void **)malloc(4 * 2);
        args[0] = render_buffer;
        args[1] = filepath;

        void *thread_manager = *(void **)((uint8_t *)*CL_MAIN_INSTANCE + 0x40);
        void *current = THREAD_MANAGER_CURRENT(thread_manager);
        CALL_FROM_MAIN_THREAD(current, (void *)save_image_main, args);
    }
}

int (*ORIGINAL_CREATE_TEXTURE_HANDLE)(void *, int, int);
void create_texture_handle_main(void **args)
{
    void *cl_app = CL_APP_INSTANCE();
    if (CL_APP_IS_MAIN_THREAD(cl_app))
    {
        ORIGINAL_CREATE_TEXTURE_HANDLE(args[0], (int)args[1], (int)args[2]);
        free(args);
    }
    else
    {
        panic("Not main thread!");
    }
}
int create_texture_handle(void *_this, int a1, int a2)
{
    void *cl_app = CL_APP_INSTANCE();
    if (CL_APP_IS_MAIN_THREAD(cl_app))
    {
        return ORIGINAL_CREATE_TEXTURE_HANDLE(_this, a1, a2);
    }
    else
    {
        void **args = (void **)malloc(4 * 3);
        args[0] = _this;
        args[1] = (void *)a1;
        args[2] = (void *)a2;

        void *thread_manager = *(void **)((uint8_t *)*CL_MAIN_INSTANCE + 0x40);
        void *current = THREAD_MANAGER_CURRENT(thread_manager);
        CALL_FROM_MAIN_THREAD(current, (void *)create_texture_handle_main, args);
        return 1;
    }
}

int (*ORIGINAL_SET_TEXTURE)(void *, int, int);
void set_texture_main(void **args)
{
    void *cl_app = CL_APP_INSTANCE();
    if (CL_APP_IS_MAIN_THREAD(cl_app))
    {
        ORIGINAL_SET_TEXTURE(args[0], (int)args[1], (int)args[2]);
        free(args);
    }
    else
    {
        panic("Not main thread!");
    }
}
int set_texture(void *_this, int a1, int a2)
{
    void *cl_app = CL_APP_INSTANCE();
    if (CL_APP_IS_MAIN_THREAD(cl_app))
    {
        return ORIGINAL_SET_TEXTURE(_this, a1, a2);
    }
    else
    {
        void **args = (void **)malloc(4 * 3);
        args[0] = _this;
        args[1] = (void *)a1;
        args[2] = (void *)a2;

        void *thread_manager = *(void **)((uint8_t *)*CL_MAIN_INSTANCE + 0x40);
        void *current = THREAD_MANAGER_CURRENT(thread_manager);
        CALL_FROM_MAIN_THREAD(current, (void *)set_texture_main, args);
        return 1;
    }
}

int (*ORIGINAL_SET_TEXTURE_REGION)(void *, int, int, int, int, int, int, void *);
void set_texture_region_main(void **args)
{
    void *cl_app = CL_APP_INSTANCE();
    if (CL_APP_IS_MAIN_THREAD(cl_app))
    {
        ORIGINAL_SET_TEXTURE_REGION(args[0], (int)args[1], (int)args[2], (int)args[3], (int)args[4], (int)args[5], (int)args[6], args[7]);
        free(args);
    }
    else
    {
        panic("Not main thread!");
    }
}
int set_texture_region(void *_this, int a1, int a2, int a3, int a4, int a5, int a6, void *a7)
{
    void *cl_app = CL_APP_INSTANCE();
    if (CL_APP_IS_MAIN_THREAD(cl_app))
    {
        return ORIGINAL_SET_TEXTURE_REGION(_this, a1, a2, a3, a4, a5, a6, a7);
    }
    else
    {
        void **args = (void **)malloc(4 * 8);
        args[0] = _this;
        args[1] = (void *)a1;
        args[2] = (void *)a2;
        args[3] = (void *)a3;
        args[4] = (void *)a4;
        args[5] = (void *)a5;
        args[6] = (void *)a6;
        args[7] = a7;

        void *thread_manager = *(void **)((uint8_t *)*CL_MAIN_INSTANCE + 0x40);
        void *current = THREAD_MANAGER_CURRENT(thread_manager);
        CALL_FROM_MAIN_THREAD(current, (void *)set_texture_region_main, args);
        return 1;
    }
}

void adm::init()
{
    Line::Hook(Line::DlSym(NULL, "admvt_setup"), (void *)adachi);
    Line::Hook(Line::DlSym(NULL, "admShutdown"), (void *)adachi);
    Line::Hook(Line::DlSym(NULL, "admGetString"), (void *)adm_version);
    Line::Hook(Line::DlSym(NULL, "admGetNumDevices"), (void *)adachi);
    Line::Hook(Line::DlSym(NULL, "admInitDevicei"), (void *)adachi);
    Line::Hook(Line::DlSym(NULL, "admChooseModeConfigi"), (void *)adm_config);
    Line::Hook(Line::DlSym(NULL, "admModeConfigi"), (void *)adachi);
    Line::Hook(Line::DlSym(NULL, "admChooseFBConfigi"), (void *)adm_fb_config);
    Line::Hook(Line::DlSym(NULL, "admCreateScreeni"), (void *)adachi);
    Line::Hook(Line::DlSym(NULL, "admCreateGraphicsContext"), (void *)adachi);
    Line::Hook(Line::DlSym(NULL, "admCreateWindowi"), (void *)adm_window);
    Line::Hook(Line::DlSym(NULL, "admDisplayScreen"), (void *)adachi);
    Line::Hook(Line::DlSym(NULL, "admMakeContextCurrent"), (void *)adachi);
    Line::Hook(Line::DlSym(NULL, "admSwapInterval"), (void *)adachi);
    Line::Hook(Line::DlSym(NULL, "admCursorAttribi"), (void *)adachi);
    Line::Hook(Line::DlSym(NULL, "admGetDeviceAttribi"), (void *)adachi);
    Line::Hook(Line::DlSym(NULL, "admSwapBuffers"), (void *)adm_swap_buffers);
    Line::Hook(Line::DlSym(NULL, "admSetMonitorGamma"), (void *)adachi);

    *(void **)&CL_APP_INSTANCE = Line::DlSym(NULL,
                                             "_ZN11clAppSystem11getInstanceEv");
    *(void **)&CL_APP_IS_MAIN_THREAD = Line::DlSym(NULL,
                                                   "_ZN11clAppSystem12isMainThreadEv");
    *(void **)&CL_MAIN_INSTANCE = Line::DlSym(NULL,
                                              "_ZN11teSingletonI10teSequenceI6clMainEE11sm_instanceE");
    *(void **)&THREAD_MANAGER_CURRENT = Line::DlSym(NULL,
                                                    "_ZN17clNPThreadManager7currentEv");
    *(void **)&CALL_FROM_MAIN_THREAD = Line::DlSym(NULL,
                                                   "_ZN10clNPThread26callFunctionFromMainThreadEPFvPvES0_");

    Line::Hook(Line::DlSym(NULL,
                           "_ZN15clSpriteManagerD1Ev"),
               (void *)del_sprite_manager,
               (void **)&ORIGINAL_DEL_SPRITE_MANAGER);
    Line::Hook(Line::DlSym(NULL,
                           "_ZN14clRenderBuffer9saveImageEPKc"),
               (void *)save_image,
               (void **)&ORIGINAL_SAVE_IMAGE);
    Line::Hook(Line::DlSym(NULL,
                           "_ZN24clAlchemyTextureAccessor19createTextureHandleEii"),
               (void *)create_texture_handle,
               (void **)&ORIGINAL_CREATE_TEXTURE_HANDLE);
    Line::Hook(Line::DlSym(NULL,
                           "_ZN3Gap3Gfx19igAGLEVisualContext10setTextureEii"),
               (void *)set_texture,
               (void **)&ORIGINAL_SET_TEXTURE);
    Line::Hook(Line::DlSym(NULL,
                           "_ZN3Gap3Gfx19igAGLEVisualContext16setTextureRegionEiiiiiiPNS0_7igImageE"),
               (void *)set_texture_region,
               (void **)&ORIGINAL_SET_TEXTURE_REGION);
}

void* adm::get_window_handle()
{
    return window_hwnd;
}