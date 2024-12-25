#pragma once
#include <SDL2/SDL.h>
#include <unordered_map>
#include <vector>
#include <string>

namespace poll
{
    enum struct Axis
    {
        LeftStickLeft,
        LeftStickUp,
        LeftStickDown,
        LeftStickRight,
        RightStickLeft,
        RightStickUp,
        RightStickDown,
        RightStickRight,
        LeftTriggerDown,
        LeftTriggerUp,
        RightTriggerDown,
        RightTriggerUp,
    };

    enum struct KeybindingType
    {
        Keycode,
        Button,
        Axis
    };

    struct KeyBinding
    {
        KeybindingType type;
        int keycode;
        SDL_GameControllerButton button;
        Axis axis;
    };

    struct KeyBindings
    {
        std::vector<KeyBinding> keys = {};
    };

    struct KeyEvent {
        int key; int scancode; int action; int mods;
    };

    struct PollState
    {
        std::unordered_map<uint32_t, SDL_GameController *> controllers;
        float deadzone;
        std::vector<int> keyboard_state;
        std::vector<int> last_keyboard_state;
        std::vector<SDL_GameControllerButton> button_state;
        std::vector<SDL_GameControllerButton> last_button_state;
        std::unordered_map<Axis, float> axis_state;
        std::unordered_map<Axis, float> last_axis_state;
        void *window_handle;
        std::vector<KeyEvent> key_events;

        static PollState create(void *window_handle, float axis_deadzone);

        void update();

        bool keycode_is_down(int keycode)
        {
            for (size_t i = 0; i < this->keyboard_state.size(); i++)
                if (this->keyboard_state[i] == keycode)
                    return true;
            return false;
        }
        bool keycode_is_up(int keycode)
        {
            return !this->keycode_is_down(keycode);
        }
        bool keycode_was_down(int keycode)
        {
            for (size_t i = 0; i < this->last_keyboard_state.size(); i++)
                if (this->last_keyboard_state[i] == keycode)
                    return true;
            return false;
        }
        bool keycode_was_up(int keycode)
        {
            return !this->keycode_was_down(keycode);
        }
        bool keycode_is_tapped(int keycode)
        {
            return this->keycode_is_down(keycode) && this->keycode_was_up(keycode);
        }
        bool keycode_is_released(int keycode)
        {
            return this->keycode_is_up(keycode) && this->keycode_was_down(keycode);
        }

        bool button_is_down(SDL_GameControllerButton button)
        {
            for (size_t i = 0; i < this->button_state.size(); i++)
                if (this->button_state[i] == button)
                    return true;
            return false;
        }
        bool button_is_up(SDL_GameControllerButton button)
        {
            return !this->button_is_down(button);
        }
        bool button_was_down(SDL_GameControllerButton button)
        {
            for (size_t i = 0; i < this->last_button_state.size(); i++)
                if (this->last_button_state[i] == button)
                    return true;
            return false;
        }
        bool button_was_up(SDL_GameControllerButton button)
        {
            return !this->button_was_down(button);
        }
        bool button_is_tapped(SDL_GameControllerButton button)
        {
            return this->button_is_down(button) && this->button_was_up(button);
        }
        bool button_is_released(SDL_GameControllerButton button)
        {
            return this->button_was_down(button) && this->button_is_up(button);
        }

        float axis_is_down(Axis axis)
        {
            if (this->axis_state.count(axis) > 0)
                return this->axis_state[axis];
            return 0.f;
        }
        bool axis_is_up(Axis axis)
        {
            return this->axis_is_down(axis) == 0.f;
        }
        float axis_was_down(Axis axis)
        {
            if (this->last_axis_state.count(axis) > 0)
                return this->last_axis_state[axis];
            return 0.f;
        }
        bool axis_was_up(Axis axis)
        {
            return this->axis_was_down(axis) == 0.f;
        }
        bool axis_is_tapped(Axis axis)
        {
            return this->axis_is_down(axis) != 0.f && this->axis_was_up(axis);
        }
        bool axis_is_released(Axis axis)
        {
            return this->axis_was_down(axis) != 0.f && this->axis_is_up(axis);
        }

        float binding_is_down(const KeyBinding &keybinding)
        {
            switch (keybinding.type)
            {
            case KeybindingType::Keycode:
                return this->keycode_is_down(keybinding.keycode) ? 1.f : 0.f;
            case KeybindingType::Button:
                return this->button_is_down(keybinding.button) ? 1.f : 0.f;
            case KeybindingType::Axis:
                return this->axis_is_down(keybinding.axis);
            }
            return 0.f;
        }
        bool binding_is_up(const KeyBinding &keybinding)
        {
            switch (keybinding.type)
            {
            case KeybindingType::Keycode:
                return this->keycode_is_up(keybinding.keycode);
            case KeybindingType::Button:
                return this->button_is_up(keybinding.button);
            case KeybindingType::Axis:
                return this->axis_is_up(keybinding.axis);
            }
            return false;
        }
        float binding_was_down(const KeyBinding &keybinding)
        {
            switch (keybinding.type)
            {
            case KeybindingType::Keycode:
                return this->keycode_was_down(keybinding.keycode) ? 1.f : 0.f;
            case KeybindingType::Button:
                return this->button_was_down(keybinding.button) ? 1.f : 0.f;
            case KeybindingType::Axis:
                return this->axis_was_down(keybinding.axis);
            }
            return 0.f;
        }
        bool binding_was_up(const KeyBinding &keybinding)
        {
            switch (keybinding.type)
            {
            case KeybindingType::Keycode:
                return this->keycode_was_up(keybinding.keycode);
            case KeybindingType::Button:
                return this->button_was_up(keybinding.button);
            case KeybindingType::Axis:
                return this->axis_was_up(keybinding.axis);
            }
            return false;
        }
        bool binding_is_tapped(const KeyBinding &keybinding)
        {
            switch (keybinding.type)
            {
            case KeybindingType::Keycode:
                return this->keycode_is_tapped(keybinding.keycode);
            case KeybindingType::Button:
                return this->button_is_tapped(keybinding.button);
            case KeybindingType::Axis:
                return this->axis_is_tapped(keybinding.axis);
            }
            return false;
        }
        bool binding_is_released(const KeyBinding &keybinding)
        {
            switch (keybinding.type)
            {
            case KeybindingType::Keycode:
                return this->keycode_is_released(keybinding.keycode);
            case KeybindingType::Button:
                return this->button_is_released(keybinding.button);
            case KeybindingType::Axis:
                return this->axis_is_released(keybinding.axis);
            }
            return false;
        }

        float is_down(const KeyBindings &keybindings)
        {
            for (size_t i = 0; i < keybindings.keys.size(); i++)
            {
                float value = this->binding_is_down(keybindings.keys[i]);
                if (value > 0.f)
                    return value;
            }
            return 0.f;
        }
        bool is_up(const KeyBindings &keybindings)
        {
            for (size_t i = 0; i < keybindings.keys.size(); i++)
                if (this->binding_is_up(keybindings.keys[i]))
                    return true;
            return false;
        }
        float was_down(const KeyBindings &keybindings)
        {
            for (size_t i = 0; i < keybindings.keys.size(); i++)
            {
                float value = this->binding_was_down(keybindings.keys[i]);
                if (value > 0.f)
                    return value;
            }
            return 0.f;
        }
        bool was_up(const KeyBindings &keybindings)
        {
            for (size_t i = 0; i < keybindings.keys.size(); i++)
                if (this->binding_was_up(keybindings.keys[i]))
                    return true;
            return false;
        }
        bool is_tapped(const KeyBindings &keybindings)
        {
            for (size_t i = 0; i < keybindings.keys.size(); i++)
                if (this->binding_is_tapped(keybindings.keys[i]))
                    return true;
            return false;
        }
        bool is_released(const KeyBindings &keybindings)
        {
            for (size_t i = 0; i < keybindings.keys.size(); i++)
                if (this->binding_is_released(keybindings.keys[i]))
                    return true;
            return false;
        }

        void key_callback(int key, int scancode, int action, int mods);
    };

    KeyBindings parse_keybinding(std::vector<std::string> toml);
};