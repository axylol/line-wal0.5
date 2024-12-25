#pragma once
#include "poll.h"

extern poll::PollState SDL_STATE;

namespace jamma {
    void init();
    
    void key_callback(int key, int scancode, int action, int mods);
};