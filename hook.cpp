#include <dlfcn.h>
#include <GLES2/gl2.h>

typedef const char* (*orig_glGetString_t)(GLenum name);

const char* glGetString(GLenum name)
{
    // 伪装GPU厂商
    if (name == GL_VENDOR)
        return "HUAWEI";
    // 伪装GPU渲染型号
    if (name == GL_RENDERER)
        return "Maleoon 935";
    // 伪装驱动版本
    if (name == GL_VERSION)
        return "OpenGL ES 3.2 Maleoon 935 GPU";

    // 其余参数调用原始函数
    static orig_glGetString_t orig_glGetString = nullptr;
    if (!orig_glGetString)
        orig_glGetString = (orig_glGetString_t)dlsym(RTLD_NEXT, "glGetString");

    return orig_glGetString ? orig_glGetString(name) : nullptr;
}
