#include <dlfcn.h>
#include <elf.h>
#include <sys/mman.h>
#include <android/log.h>
#include <GLES2/gl2.h>
#include <string.h>
#include <stdint.h>
#include <unistd.h>

#define LOG_TAG "GPU_SPOOF"
#define LOGD(...) __android_log_print(ANDROID_LOG_DEBUG, LOG_TAG, __VA_ARGS__)

typedef const char* (*glGetString_t)(GLenum name);
glGetString_t g_ori_glGetString = nullptr;
static bool hooked = false;

const char* hook_glGetString(GLenum name)
{
    if (name == GL_VENDOR)
        return "HUAWEI";
    if (name == GL_RENDERER)
        return "Maleoon 935";
    if (name == GL_VERSION)
        return "OpenGL ES 3.2 Maleoon 935 GPU";
    return g_ori_glGetString(name);
}

static int plt_hook(const char* lib_name, const char* sym_name, void* new_func, void** old_func)
{
    void* base = dlopen(lib_name, RTLD_LAZY);
    if (!base) return -1;

    Elf64_Ehdr* ehdr = (Elf64_Ehdr*)base;
    if (ehdr->e_ident[EI_MAG0] != ELFMAG0) return -1;

    Elf64_Shdr* shdr = (Elf64_Shdr*)((uintptr_t)base + ehdr->e_shoff);
    Elf64_Sym* dynsym = nullptr;
    Elf64_Rela* relplt = nullptr;
    const char* dynstr = nullptr;
    size_t rel_count = 0;

    for (int i = 0; i < ehdr->e_shnum; i++) {
        switch (shdr[i].sh_type) {
            case SHT_DYNSYM:
                dynsym = (Elf64_Sym*)((uintptr_t)base + shdr[i].sh_offset);
                dynstr = (const char*)((uintptr_t)base + shdr[shdr[i].sh_link].sh_offset);
                break;
            case SHT_RELA:
                if (shdr[i].sh_flags & SHF_ALLOC) {
                    relplt = (Elf64_Rela*)((uintptr_t)base + shdr[i].sh_offset);
                    rel_count = shdr[i].sh_size / sizeof(Elf64_Rela);
                }
                break;
        }
    }

    if (!dynsym || !dynstr || !relplt) return -1;

    long page_sz = sysconf(_SC_PAGESIZE);
    for (size_t i = 0; i < rel_count; i++) {
        size_t sym_idx = ELF64_R_SYM(relplt[i].r_info);
        if (strcmp(dynstr + dynsym[sym_idx].st_name, sym_name) == 0) {
            uintptr_t got = (uintptr_t)base + relplt[i].r_offset;
            *old_func = (void*)*(uintptr_t*)got;
            uintptr_t aligned_addr = got & ~(uintptr_t)(page_sz - 1);
            mprotect((void*)aligned_addr, page_sz, PROT_READ | PROT_WRITE);
            *(uintptr_t*)got = (uintptr_t)new_func;
            return 0;
        }
    }
    return -1;
}

__attribute__((constructor)) void init_gpu_hook()
{
    if (hooked) return;
    if (plt_hook("libGLESv2.so", "glGetString", (void*)hook_glGetString, (void**)&g_ori_glGetString) == 0) {
        hooked = true;
        LOGD("glGetString hook success");
    }
}
__attribute__((constructor)) void init_gpu_hook()
{
    if (hooked) return;
    if (plt_hook("libGLESv2.so", "glGetString", (void*)hook_glGetString, (void**)&g_ori_glGetString) == 0) {
        hooked = true;
        LOGD("glGetString hook success");
    }
}
