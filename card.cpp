#include "card.h"
#include "shared/line.h"
#include "common.h"
#include <fstream>
#include <ostream>

#define INIT 0x10
#define READSTATUS 0x20
#define CANCEL 0x40
#define EJECT 0x80
#define DISPENSECARD 0xB0
#define PRINTSETTING 0x78
#define READ 0x33
#define WRITE 0x53

void exec(uint8_t *a1)
{
    uint32_t has_command = *(uint32_t *)a1;
    if (has_command == 0)
        return;
    uint8_t *request = *(uint8_t **)(a1 + 0x08);
    uint8_t start = request[0];
    if (start != 2)
        return;
    uint8_t count = request[1];
    uint8_t command = request[2];

    std::vector<uint8_t> data = {};
    for (uint8_t i = 6; i < count; i++)
        data.push_back(request[i]);

    switch (command)
    {
    case INIT:
    {
        *(uint32_t *)a1 = 0x00;
        break;
    }
    case READSTATUS:
    {
        *(uint32_t *)a1 = 0x00;
        break;
    }
    case DISPENSECARD:
    {
        bool check = data[0] == 0x32;

        *(uint32_t *)a1 = 0x00;
        if (check)
        {
            *(uint32_t *)(a1 + 0x06) = 0x37;
        }
        else
        {
            *(uint32_t *)(a1 + 0x04) = 0x33;
        }
        break;
    }
    case READ:
    {
        *(uint32_t *)a1 = 0x00;
        *(uint32_t *)(a1 + 0x04) = 0x31;
        if (data[0] == 0x32)
        {
            if (card::CARD_DATA.empty())
            {
                *(uint32_t *)(a1 + 0x04) = 0x30;
                *(uint32_t *)(a1 + 0x06) = 0x34;
            }
            break;
        }

        uint8_t *write_buf = *(uint8_t **)(a1 + 0x10);
        *(uint8_t *)(write_buf) = 0x00;
        *(uint8_t *)(write_buf + 0x04) = 0x33;
        *(uint8_t *)(write_buf + 0x05) = 0x30;
        *(uint8_t *)(write_buf + 0x06) = 0x30;
        for (int i = 0; i < 0x45 * 3; i++)
            *(uint8_t *)(write_buf + i + 0x06) = card::CARD_DATA[i];
        break;
    }
    case WRITE:
    {
        *(uint32_t *)a1 = 0x00;

        if (data[2] == 0x30)
        {
            // track 1
            for (int i = 0; i < 0x45; i++)
            {
                card::CARD_DATA[i] = data[3 + i];
            }
        }
        else if (data[2] == 0x35)
        {
            // track 23
            for (int i = 0; i < 0x45 * 2; i++)
            {
                card::CARD_DATA[i + 0x45] = data[3 + i];
            }
        }
        else if (data[2] == 0x36)
        {
            card::CARD_DATA.clear();
            for (int i = 0; i < 0x45 * 3; i++)
                card::CARD_DATA.push_back(data[3 + i]);
        }
        else
        {
            panic("Unknown track combination %d\n", data[2]);
        }

        std::ofstream file("card.bin", std::ios_base::out | std::ios_base::binary);
        if (file.is_open()) {
            file.write((char *)card::CARD_DATA.data(), card::CARD_DATA.size());
            file.close();
        } else {
            panic("Cant open card.bin for saving");
        }
        break;
    }
    case CANCEL:
    {
        *(uint32_t *)a1 = 0x00;
        break;
    }
    case EJECT:
    {
        card::CARD_DATA.clear();
        *(uint32_t *)a1 = 0x00;
        *(uint32_t *)(a1 + 0x04) = 0x30;
        break;
    }
    case PRINTSETTING:
    {
        *(uint32_t *)a1 = 0x00;
        break;
    }
    default:
    {
        panic("Unhandled command %d\n", command);
    }
    }
}

std::vector<uint8_t> card::CARD_DATA = {};
void card::init()
{
    Line::Hook(Line::DlSym(NULL, "_ZN13clCardPrinter4execEv"), (void *)exec);
}