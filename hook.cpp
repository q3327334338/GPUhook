#include <dlfcn.h>
#include "dobby/Dobby.h"
#include <GLES2/gl2.h>
#include <android/log.h>

#define LOG_TAG "GPU_SPOOF"
#define LOGD(...) __android_log_print(ANDROID_LOG_DEBUG, LOG_TAG, __VA_ARGS__)

typedef const char* (*glGetString_t)(GLenum name);
glGetString_t g_ori_glGetString = nullptr;
static bool hooked = false;

const char* hook_glGetString(GLenum name)
{
    if (name == GL_VENDOR)
    {
        return "HUAWEI";
    }
    if (name == GL_RENDERER)
    {
        return "Maleoon 935";
    }
    if (name == GL_VERSION)
    {
        return "OpenGL ES 3.2 Maleoon 935 GPU";
    }
    return g_ori_glGetString(name);
}

__attribute__((constructor)) void init_gpu_hook()
{
    if (hooked) return;

    void* handle = dlopen("libGLESv2.so", RTLD_LAZY);
    if (!handle)
    {
        LOGD("open libGLESv2.so failed");
        return;
    }

    void* sym = dlsym(handle, "glGetString");
    if (sym)
    {
        DobbyHook(sym, (void*)hook_glGetString, (void**)&g_ori_glGetString);
        hooked = true;
        LOGD("glGetString hook success");
    }
}
