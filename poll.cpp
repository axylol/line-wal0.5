#include "poll.h"
#include <SDL2/SDL_syswm.h>
#include "lib.h"
#include "common.h"
#include <GLFW/glfw3.h>

#define KB_KEYCODE(name) {poll::KeybindingType::Keycode, name, SDL_CONTROLLER_BUTTON_A, poll::Axis::LeftStickLeft}
#define CB_BUTTON(name) {poll::KeybindingType::Button, GLFW_KEY_A, name, poll::Axis::LeftStickLeft}
#define CB_AXIS(name) {poll::KeybindingType::Axis, GLFW_KEY_A, SDL_CONTROLLER_BUTTON_A, name}

std::unordered_map<std::string, poll::KeyBinding> MAPPINGS = {
    {"F1", KB_KEYCODE(GLFW_KEY_F1)},
    {"F2", KB_KEYCODE(GLFW_KEY_F2)},
    {"F3", KB_KEYCODE(GLFW_KEY_F3)},
    {"F4", KB_KEYCODE(GLFW_KEY_F4)},
    {"F5", KB_KEYCODE(GLFW_KEY_F5)},
    {"F6", KB_KEYCODE(GLFW_KEY_F6)},
    {"F7", KB_KEYCODE(GLFW_KEY_F7)},
    {"F8", KB_KEYCODE(GLFW_KEY_F8)},
    {"F9", KB_KEYCODE(GLFW_KEY_F9)},
    {"F10", KB_KEYCODE(GLFW_KEY_F10)},
    {"F11", KB_KEYCODE(GLFW_KEY_F11)},
    {"F12", KB_KEYCODE(GLFW_KEY_F12)},
    {"NUM0", KB_KEYCODE(GLFW_KEY_0)},
    {"NUM1", KB_KEYCODE(GLFW_KEY_1)},
    {"NUM2", KB_KEYCODE(GLFW_KEY_2)},
    {"NUM3", KB_KEYCODE(GLFW_KEY_3)},
    {"NUM4", KB_KEYCODE(GLFW_KEY_4)},
    {"NUM5", KB_KEYCODE(GLFW_KEY_5)},
    {"NUM6", KB_KEYCODE(GLFW_KEY_6)},
    {"NUM7", KB_KEYCODE(GLFW_KEY_7)},
    {"NUM8", KB_KEYCODE(GLFW_KEY_8)},
    {"NUM9", KB_KEYCODE(GLFW_KEY_9)},
    {"UPARROW", KB_KEYCODE(GLFW_KEY_UP)},
    {"LEFTARROW", KB_KEYCODE(GLFW_KEY_LEFT)},
    {"DOWNARROW", KB_KEYCODE(GLFW_KEY_DOWN)},
    {"RIGHTARROW", KB_KEYCODE(GLFW_KEY_RIGHT)},
    {"ENTER", KB_KEYCODE(GLFW_KEY_ENTER)},
    {"SPACE", KB_KEYCODE(GLFW_KEY_SPACE)},
    {"CONTROL", KB_KEYCODE(GLFW_KEY_LEFT_CONTROL)},
    {"SHIFT", KB_KEYCODE(GLFW_KEY_LEFT_SHIFT)},
    {"TAB", KB_KEYCODE(GLFW_KEY_TAB)},
    {"ESCAPE", KB_KEYCODE(GLFW_KEY_ESCAPE)},
    {"A", KB_KEYCODE(GLFW_KEY_A)},
    {"B", KB_KEYCODE(GLFW_KEY_B)},
    {"C", KB_KEYCODE(GLFW_KEY_C)},
    {"D", KB_KEYCODE(GLFW_KEY_D)},
    {"E", KB_KEYCODE(GLFW_KEY_E)},
    {"F", KB_KEYCODE(GLFW_KEY_F)},
    {"G", KB_KEYCODE(GLFW_KEY_G)},
    {"H", KB_KEYCODE(GLFW_KEY_H)},
    {"I", KB_KEYCODE(GLFW_KEY_I)},
    {"J", KB_KEYCODE(GLFW_KEY_J)},
    {"K", KB_KEYCODE(GLFW_KEY_K)},
    {"L", KB_KEYCODE(GLFW_KEY_L)},
    {"M", KB_KEYCODE(GLFW_KEY_M)},
    {"N", KB_KEYCODE(GLFW_KEY_N)},
    {"O", KB_KEYCODE(GLFW_KEY_O)},
    {"P", KB_KEYCODE(GLFW_KEY_P)},
    {"Q", KB_KEYCODE(GLFW_KEY_Q)},
    {"R", KB_KEYCODE(GLFW_KEY_R)},
    {"S", KB_KEYCODE(GLFW_KEY_S)},
    {"T", KB_KEYCODE(GLFW_KEY_T)},
    {"U", KB_KEYCODE(GLFW_KEY_U)},
    {"V", KB_KEYCODE(GLFW_KEY_V)},
    {"W", KB_KEYCODE(GLFW_KEY_W)},
    {"X", KB_KEYCODE(GLFW_KEY_X)},
    {"Y", KB_KEYCODE(GLFW_KEY_Y)},
    {"Z", KB_KEYCODE(GLFW_KEY_Z)},
    {"SDL_A", CB_BUTTON(SDL_CONTROLLER_BUTTON_A)},
    {"SDL_B", CB_BUTTON(SDL_CONTROLLER_BUTTON_B)},
    {"SDL_X", CB_BUTTON(SDL_CONTROLLER_BUTTON_X)},
    {"SDL_Y", CB_BUTTON(SDL_CONTROLLER_BUTTON_Y)},
    {"SDL_BACK", CB_BUTTON(SDL_CONTROLLER_BUTTON_BACK)},
    {"SDL_GUIDE", CB_BUTTON(SDL_CONTROLLER_BUTTON_GUIDE)},
    {"SDL_START", CB_BUTTON(SDL_CONTROLLER_BUTTON_START)},
    {"SDL_LSHOULDER", CB_BUTTON(SDL_CONTROLLER_BUTTON_LEFTSHOULDER)},
    {"SDL_RSHOULDER", CB_BUTTON(SDL_CONTROLLER_BUTTON_RIGHTSHOULDER)},
    {"SDL_DPAD_UP", CB_BUTTON(SDL_CONTROLLER_BUTTON_DPAD_UP)},
    {"SDL_DPAD_LEFT", CB_BUTTON(SDL_CONTROLLER_BUTTON_DPAD_LEFT)},
    {"SDL_DPAD_DOWN", CB_BUTTON(SDL_CONTROLLER_BUTTON_DPAD_DOWN)},
    {"SDL_DPAD_RIGHT", CB_BUTTON(SDL_CONTROLLER_BUTTON_DPAD_RIGHT)},
    {"SDL_MISC", CB_BUTTON(SDL_CONTROLLER_BUTTON_MISC1)},
    {"SDL_PADDLE1", CB_BUTTON(SDL_CONTROLLER_BUTTON_PADDLE1)},
    {"SDL_PADDLE2", CB_BUTTON(SDL_CONTROLLER_BUTTON_PADDLE2)},
    {"SDL_PADDLE3", CB_BUTTON(SDL_CONTROLLER_BUTTON_PADDLE3)},
    {"SDL_PADDLE4", CB_BUTTON(SDL_CONTROLLER_BUTTON_PADDLE4)},
    {"SDL_TOUCHPAD", CB_BUTTON(SDL_CONTROLLER_BUTTON_TOUCHPAD)},
    {"SDL_LSTICK_PRESS", CB_BUTTON(SDL_CONTROLLER_BUTTON_LEFTSTICK)},
    {"SDL_RSTICK_PRESS", CB_BUTTON(SDL_CONTROLLER_BUTTON_RIGHTSHOULDER)},
    {"SDL_LSTICK_LEFT", CB_AXIS(poll::Axis::LeftStickLeft)},
    {"SDL_LSTICK_UP", CB_AXIS(poll::Axis::LeftStickUp)},
    {"SDL_LSTICK_DOWN", CB_AXIS(poll::Axis::LeftStickDown)},
    {"SDL_LSTICK_RIGHT", CB_AXIS(poll::Axis::LeftStickRight)},
    {"SDL_RSTICK_LEFT", CB_AXIS(poll::Axis::RightStickLeft)},
    {"SDL_RSTICK_UP", CB_AXIS(poll::Axis::RightStickUp)},
    {"SDL_RSTICK_DOWN", CB_AXIS(poll::Axis::RightStickDown)},
    {"SDL_RSTICK_RIGHT", CB_AXIS(poll::Axis::RightStickRight)},
    {"SDL_LTRIGGER_DOWN", CB_AXIS(poll::Axis::LeftTriggerDown)},
    {"SDL_LTRIGGER_UP", CB_AXIS(poll::Axis::LeftTriggerUp)},
    {"SDL_RTRIGGER_DOWN", CB_AXIS(poll::Axis::RightTriggerDown)},
    {"SDL_RTRIGGER_UP", CB_AXIS(poll::Axis::RightTriggerUp)}};

poll::KeyBindings poll::parse_keybinding(std::vector<std::string> toml)
{
    KeyBindings keybindings;
    for (size_t i = 0; i < toml.size(); i++)
    {
        if (MAPPINGS.count(toml[i]) > 0)
        {
            keybindings.keys.push_back(MAPPINGS[toml[i]]);
        }
        else
        {
            panic("Incorrect keybinding %s");
        }
    }
    return keybindings;
}

poll::PollState poll::PollState::create(void *window_handle, float axis_deadzone)
{
    if (SDL_Init(SDL_INIT_JOYSTICK | SDL_INIT_GAMECONTROLLER | SDL_INIT_EVENTS) < 0)
    {
        panic("Error initializing sdl");
    }

    std::unordered_map<uint32_t, SDL_GameController *> controllers = {};
    for (int i = 0; i < SDL_NumJoysticks(); i++)
    {
        SDL_GameController *controller = SDL_GameControllerOpen(i);

        if (!controller)
        {
            const char *name = SDL_GameControllerNameForIndex(i);
            if (!name)
                name = "Failed to get controller information";
            printf("Failed to open %s\n", name);
        }
        else
        {
            controllers.insert_or_assign(i, controller);
        }
    }

    PollState pollState;
    pollState.controllers = controllers;
    pollState.deadzone = axis_deadzone;
    pollState.keyboard_state = {};
    pollState.last_keyboard_state = {};
    pollState.button_state = {};
    pollState.last_button_state = {};
    pollState.axis_state = {};
    pollState.last_axis_state = {};
    pollState.window_handle = window_handle;
    return pollState;
}

void poll::PollState::update()
{
    this->last_keyboard_state = this->keyboard_state;
    this->last_button_state = this->button_state;
    this->last_axis_state = this->axis_state;

    SDL_Event event;
    while (SDL_PollEvent(&event) > 0)
    {
        switch (event.type)
        {
        case SDL_CONTROLLERDEVICEADDED:
        {
            SDL_GameController *controller = SDL_GameControllerOpen(event.cdevice.which);
            this->controllers.insert_or_assign(event.cdevice.which, controller);
            break;
        }
        case SDL_CONTROLLERDEVICEREMOVED:
        {
            if (this->controllers.count(event.cdevice.which) < 1)
                break;
            this->controllers.erase(event.cdevice.which);
            break;
        }
        case SDL_CONTROLLERBUTTONDOWN:
        {
            this->button_state.push_back((SDL_GameControllerButton)event.cbutton.button);
            break;
        }
        case SDL_CONTROLLERBUTTONUP:
        {
            SDL_GameControllerButton button = (SDL_GameControllerButton)event.cbutton.button;

            for (size_t i = 0; i < this->button_state.size(); i++)
            {
                if (this->button_state[i] == button)
                {
                    this->button_state.erase(this->button_state.begin() + i);
                    i--;
                }
            }
            break;
        }
        case SDL_CONTROLLERAXISMOTION:
        {
            float value = (float)event.caxis.value / (float)INT16_MAX;

            Axis axis_positive;
            Axis axis_negative;
            switch (event.caxis.axis)
            {
            case SDL_CONTROLLER_AXIS_LEFTX:
            {
                axis_negative = Axis::LeftStickLeft;
                break;
            }
            case SDL_CONTROLLER_AXIS_LEFTY:
            {
                axis_negative = Axis::LeftStickUp;
                break;
            }
            case SDL_CONTROLLER_AXIS_RIGHTX:
            {
                axis_negative = Axis::RightStickLeft;
                break;
            }
            case SDL_CONTROLLER_AXIS_RIGHTY:
            {
                axis_negative = Axis::RightStickUp;
                break;
            }
            case SDL_CONTROLLER_AXIS_TRIGGERLEFT:
            {
                axis_negative = Axis::LeftTriggerUp;
                break;
            }
            case SDL_CONTROLLER_AXIS_TRIGGERRIGHT:
            {
                axis_negative = Axis::RightTriggerUp;
                break;
            }
            }
            switch (event.caxis.axis)
            {
            case SDL_CONTROLLER_AXIS_LEFTX:
            {
                axis_positive = Axis::LeftStickRight;
                break;
            }
            case SDL_CONTROLLER_AXIS_LEFTY:
            {
                axis_positive = Axis::LeftStickDown;
                break;
            }
            case SDL_CONTROLLER_AXIS_RIGHTX:
            {
                axis_positive = Axis::RightStickRight;
                break;
            }
            case SDL_CONTROLLER_AXIS_RIGHTY:
            {
                axis_positive = Axis::RightStickDown;
                break;
            }
            case SDL_CONTROLLER_AXIS_TRIGGERLEFT:
            {
                axis_positive = Axis::LeftTriggerDown;
                break;
            }
            case SDL_CONTROLLER_AXIS_TRIGGERRIGHT:
            {
                axis_positive = Axis::RightTriggerDown;
                break;
            }
            }

            if (value > this->deadzone)
            {
                if (value > 1.f)
                    value = 1.f;

                this->axis_state.insert_or_assign(axis_positive, value);
                this->axis_state.insert_or_assign(axis_negative, 0.f);
            }
            else if (value < -this->deadzone)
            {
                value = -value;
                if (value > 1.f)
                    value = 1.f;

                this->axis_state.insert_or_assign(axis_negative, value);
                this->axis_state.insert_or_assign(axis_positive, 0.f);
            }
            else
            {
                this->axis_state.insert_or_assign(axis_positive, 0.f);
                this->axis_state.insert_or_assign(axis_negative, 0.f);
            }
            break;
        }
        }
    }

    for (size_t i = 0; i < this->key_events.size(); i++)
    {
        if (this->key_events[i].action == GLFW_PRESS)
        {
            this->keyboard_state.push_back(this->key_events[i].key);
        }
        else if (this->key_events[i].action == GLFW_RELEASE)
        {
            for (size_t k = 0; k < this->keyboard_state.size(); k++)
            {
                if (this->keyboard_state[k] == this->key_events[i].key)
                {
                    this->keyboard_state.erase(this->keyboard_state.begin() + k);
                    k--;
                }
            }
        }
    }
    this->key_events.clear();
}

void poll::PollState::key_callback(int key, int scancode, int action, int mods)
{
    this->key_events.push_back({key, scancode, action, mods});
}