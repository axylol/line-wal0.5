#include "al.h"
#include <vector>
#include "shared/line.h"
#include <dlfcn.h>
#include <string.h>
#include "common.h"

std::vector<const char *> AL_FUNCS = {
    "alcSuspendContext",
    "alcCloseDevice",
    "alListener3f",
    "alGetFloat",
    "alcGetString",
    "alIsExtensionPresent",
    "alcIsExtensionPresent",
    "alGetBooleanv",
    "alDopplerFactor",
    "alSourcePausev",
    "alDisable",
    "alcGetCurrentContext",
    "alEnable",
    "alDeleteBuffers",
    "alGetSourcef",
    "alSourcefv",
    "alGetIntegerv",
    "alGetDouble",
    "alGetEnumValue",
    "alSourcei",
    "alSourceRewind",
    "alcDestroyContext",
    "alGetSourcefv",
    "alGetBufferi",
    "alSourcePlay",
    "alSourcef",
    "alSourceStop",
    "alcGetError",
    "alGetSource3f",
    "alSource3f",
    "alGetListenerfv",
    "alGetError",
    "alIsBuffer",
    "alcGetContextsDevice",
    "alGetListener3f",
    "alcGetIntegerv",
    "alDopplerVelocity",
    "alSourcePlayv",
    "alSourceUnqueueBuffers",
    "alGetFloatv",
    "alcOpenDevice",
    "alcProcessContext",
    "alListeneri",
    "alListenerfv",
    "alDistanceModel",
    "alSourcePause",
    "alGenSources",
    "alIsEnabled",
    "alcMakeContextCurrent",
    "alDeleteSources",
    "alcGetEnumValue",
    "alSourceStopv",
    "alGetProcAddress",
    "alSourceRewindv",
    "alGetString",
    "alBufferData",
    "alIsSource",
    "alGetInteger",
    "alGetSourcei",
    "alSourceQueueBuffers",
    "alListenerf",
    "alGenBuffers",
    "alcGetProcAddress",
    "alGetBufferf",
    "alGetListeneri",
    "alGetDoublev",
    "alcCreateContext",
    "alGetBoolean",
    "alGetListenerf",
};

void *resolveAl(const char *name)
{
    void *soft_oal = dlopen("soft_oal.dll", RTLD_NOW);
    if (!soft_oal)
    {
        panic("can't load soft_oal.dll %s", dlerror());
    }
    return dlsym(soft_oal, name);
}

void al::load_al_funcs()
{
    for (size_t i = 0; i < AL_FUNCS.size(); i++)
    {
        void *sym = Line::DlSym(NULL, AL_FUNCS[i]);
        if (!sym)
            continue;
        void *h = resolveAl(AL_FUNCS[i]);
        if (!h)
            continue;

        uint8_t data[7];
        data[0] = 0xB8;
        memcpy(&data[1], &h, 4);
        data[5] = 0xFF;
        data[6] = 0xE0;
        memcpy(sym, data, 7);
    }
}