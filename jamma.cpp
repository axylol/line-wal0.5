#include "jamma.h"
#include "shared/line.h"
#include "lib.h"
#include "adm.h"
#include "card.h"
#include <fstream>

poll::PollState SDL_STATE;
bool SDL_STATE_INITIALIZED = false;

uint8_t GEAR_INDEX = 0;

const uint32_t GEAR_LEFT = 1;
const uint32_t GEAR_RIGHT = 2;
const uint32_t GEAR_UP = 4;
const uint32_t GEAR_DOWN = 8;

uint32_t set_gear_bits(uint8_t index)
{
    switch (index)
    {
    case 0:
        return 0;
    case 1:
        return GEAR_LEFT | GEAR_UP;
    case 2:
        return GEAR_LEFT | GEAR_DOWN;
    case 3:
        return GEAR_UP;
    case 4:
        return GEAR_DOWN;
    case 5:
        return GEAR_RIGHT | GEAR_UP;
    case 6:
        return GEAR_RIGHT | GEAR_DOWN;
    }
    return 0;
}

void handle_inputs(uint8_t *a1)
{
    if (!adm::get_window_handle())
        return;
    if (!SDL_STATE_INITIALIZED)
    {
        SDL_STATE_INITIALIZED = true;

        // initialize pollstate first for window
        SDL_STATE = poll::PollState::create(adm::get_window_handle(), CONFIG.deadzone);
    }

    SDL_STATE.update();

    if (SDL_STATE.is_tapped(KEYCONFIG.card_insert))
    {
        std::ifstream file("card.bin", std::ios::binary);
        if (file.is_open())
        {
            file.seekg(0, std::ios::end);
            size_t file_size = file.tellg();
            file.seekg(0, std::ios::beg);

            card::CARD_DATA.clear();
            if (file_size > 0)
            {
                uint8_t *data = (uint8_t *)new uint8_t[file_size];
                file.read((char *)data, file_size);
                for (size_t i = 0; i < file_size; i++)
                    card::CARD_DATA.push_back(data[i]);
                delete[] data;
            }

            file.close();
        }
        else
        {
            printf("Cannot open card.bin\n");
        }
    }

    *(uint32_t *)(a1 + 0x24) = 0;

    uint32_t bits = 0;
    if (SDL_STATE.is_down(KEYCONFIG.test) > 0.f)
    {
        bits |= 0x20000000;
    }
    if (SDL_STATE.is_down(KEYCONFIG.service) > 0.f)
    {
        bits |= 0x80000000;
    }
    if (SDL_STATE.is_down(KEYCONFIG.perspective) > 0.f)
    {
        bits |= 0x40;
    }
    if (SDL_STATE.is_down(KEYCONFIG.intrude) > 0.f)
    {
        bits |= 0x80;
    }
    if (SDL_STATE.is_down(KEYCONFIG.brake) > 0.f)
    {
        bits |= 0x20;
    }
    if (SDL_STATE.is_down(KEYCONFIG.gas) > 0.f)
    {
        bits |= 0x10;
    }
    if (SDL_STATE.is_tapped(KEYCONFIG.gear_next))
    {
        if (GEAR_INDEX < 6)
            GEAR_INDEX++;
    }
    else if (SDL_STATE.is_tapped(KEYCONFIG.gear_previous))
    {
        if (GEAR_INDEX > 0)
            GEAR_INDEX--;
    }
    bits |= set_gear_bits(GEAR_INDEX);
    if (SDL_STATE.is_down(KEYCONFIG.gear_up) > 0.f)
    {
        GEAR_INDEX = 0;
        bits |= GEAR_UP;
    }
    if (SDL_STATE.is_down(KEYCONFIG.gear_left) > 0.f)
    {
        GEAR_INDEX = 0;
        bits |= GEAR_LEFT;
    }
    if (SDL_STATE.is_down(KEYCONFIG.gear_down) > 0.f)
    {
        GEAR_INDEX = 0;
        bits |= GEAR_DOWN;
    }
    if (SDL_STATE.is_down(KEYCONFIG.gear_right) > 0.f)
    {
        GEAR_INDEX = 0;
        bits |= GEAR_RIGHT;
    }
    *(uint32_t *)(a1 + 0x8) = bits;

    uint8_t *n2jvio = (uint8_t *)Line::DlSym(NULL, "n2jvio");

    float wheel_left = SDL_STATE.is_down(KEYCONFIG.wheel_left);
    float wheel_right = SDL_STATE.is_down(KEYCONFIG.wheel_right);
    *(uint16_t *)(n2jvio + 0x1A8) = (uint16_t)INT16_MAX - uint16_t(wheel_left * (float)INT16_MAX) + uint16_t(wheel_right * (float)INT16_MAX);
    *(float *)(a1 + 0x20) = 0.f - wheel_left + wheel_right;

    float gas = SDL_STATE.is_down(KEYCONFIG.gas);
    *(uint16_t *)(n2jvio + 0x1AA) = gas * (float)INT16_MAX;
    *(float *)(a1 + 0x30) = gas;

    float brake = SDL_STATE.is_down(KEYCONFIG.brake);
    *(uint16_t *)(n2jvio + 0x1AC) = brake * (float)INT16_MAX;
    *(float *)(a1 + 0x34) = brake;
}

void jamma::init()
{
    Line::Hook(Line::DlSym(NULL, "_ZN10clSystemN24initEb"), (void *)adachi, NULL);
    Line::Hook(Line::DlSym(NULL, "_ZN10clSystemN212initSystemN2Ev"), (void *)adachi, NULL);
    Line::Hook(Line::DlSym(NULL, "_ZN18clInputDeviceJamma8checkUseEv"), (void *)adachi, NULL);
    Line::Hook(Line::DlSym(NULL, "_ZN18clInputDeviceJamma12handleEventsEv"), (void *)handle_inputs, NULL);
    Line::Hook(Line::DlSym(NULL, "_ZN16clInputDevicePad12handleEventsEv"), (void *)adachi, NULL);
    Line::Hook(Line::DlSym(NULL, "_ZN16clInputDevicePad13joyButtonDownEPN3Gap7Display12igControllerENS2_7BUTTONSE"), (void *)adachi, NULL);
    Line::Hook(Line::DlSym(NULL, "_ZN16clInputDevicePad17joyButtonPressureEPN3Gap7Display12igControllerENS2_7BUTTONSEf"), (void *)adachi, NULL);
    Line::Hook(Line::DlSym(NULL, "_ZN16clInputDevicePad11joyButtonUpEPN3Gap7Display12igControllerENS2_7BUTTONSE"), (void *)adachi, NULL);
    Line::Hook(Line::DlSym(NULL, "_ZN16clInputDevicePad8joyStickEPN3Gap7Display12igControllerEtff"), (void *)adachi, NULL);

    Line::Hook(Line::DlSym(NULL, "n2JvioTxVsync"), (void *)adachi, NULL);
    Line::Hook(Line::DlSym(NULL, "n2JvioAckTxVsync"), (void *)adachi, NULL);
}