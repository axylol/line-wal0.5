#pragma once
#include <string>
#include "poll.h"

struct Config
{
    bool fullscreen;
    bool input_emu;
    bool card_emu;
    std::string dongle;
    
    // TODO: Port local_ip stuff
    std::string local_ip;

    float deadzone;
    uint32_t width;
    uint32_t height;

    bool ya_card_emu;
};
extern Config CONFIG;

struct KeyConfig {
    poll::KeyBindings test;
    poll::KeyBindings service;
    poll::KeyBindings card_insert;

    poll::KeyBindings gear_next;
    poll::KeyBindings gear_previous;
    poll::KeyBindings gear_neutral;
    poll::KeyBindings gear_first;
    poll::KeyBindings gear_second;
    poll::KeyBindings gear_third;
    poll::KeyBindings gear_fourth;
    poll::KeyBindings gear_fifth;
    poll::KeyBindings gear_sixth;
    poll::KeyBindings gear_up;
    poll::KeyBindings gear_left;
    poll::KeyBindings gear_down;
    poll::KeyBindings gear_right;

    poll::KeyBindings perspective;
    poll::KeyBindings intrude;
    poll::KeyBindings gas;
    poll::KeyBindings brake;
    poll::KeyBindings wheel_left;
    poll::KeyBindings wheel_right;
};
extern KeyConfig KEYCONFIG;

extern bool IS_BASE_MT3;

int adachi();
int undachi();