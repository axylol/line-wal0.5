#include "lib.h"
#include "shared/line.h"
#include "adm.h"
#include "al.h"
#include "card.h"
#include "jamma.h"
#include "poll.h"
#include "res.h"
#include <string.h>
#include <dlfcn.h>
#include "common.h"
#include <toml.hpp>
#include <arpa/inet.h>
#include "yac.h"
#include <dirent.h>
#include <algorithm>

Config CONFIG;
KeyConfig KEYCONFIG;

int undachi()
{
    return 0;
}

int adachi()
{
    return 1;
}

int HASP_ID = 1;
int hasp_login(int, int, int *id)
{
    *id = HASP_ID;
    HASP_ID++;
    return 0;
}

int hasp_size(int, int, int *size)
{
    *size = 0xD40;
    return 0;
}

int hasp_read(int, int, int offset, int length, uint8_t *buffer)
{
    uint8_t data[0xD40];
    memset(data, 0, sizeof(data));

    std::string dongle;
    if (CONFIG.dongle.length() < 12)
        dongle = "285013501138";
    else
        dongle = CONFIG.dongle;
    memcpy(data + 0xD00, dongle.c_str(), 12);

    {
        uint8_t crc = 0;
        for (int i = 0; i < 0x0D; i++)
            crc += data[i];
        data[0x0D] = crc;
        data[0x0F] = crc ^ 0xFF;
    }

    {
        uint8_t crc = 0;
        for (int i = 0; i < 0x3E; i++)
            crc += data[0xD00 + i];
        data[0xD3E] = crc;
        data[0xD3F] = crc ^ 0xFF;
    }

    memcpy(buffer, data + offset, length);
    return 0;
}

void (*ORIGINAL_CL_MAIN)(void **);
void cl_main(void **log)
{
    ORIGINAL_CL_MAIN(log);
    *log = Line::DlSym(NULL, "_ZSt4cout");
}

uint32_t get_address(void **clnet)
{
    uint32_t ip = htonl(inet_addr(CONFIG.local_ip.c_str()));
    uint8_t *net = *(uint8_t **)((uint8_t *)clnet + 0x24);
    if (!net)
        net = *(uint8_t **)((uint8_t *)clnet + 0x1C);
    *(uint32_t *)(net + 0x04) = ip;
    *(uint32_t *)(net + 0x08) = htonl(inet_addr("255.255.255.0"));
    return ip;
}

void findTarget(const char *path, const char *filter)
{
    std::vector<std::string> fileNames = {};
    DIR *dir = opendir(path);
    if (dir)
    {
        while (true)
        {
            struct dirent *d = readdir(dir);
            if (!d)
                break;
            if (!strstr(d->d_name, filter))
                continue;
            fileNames.push_back(d->d_name);
        }
        closedir(dir);

        std::sort(fileNames.begin(), fileNames.end());
    }
    else
    {
        printf("Error opening %s\n", path);
    }

    std::ofstream f("tmp/find.txt");
    if (f.is_open())
    {
        for (size_t i = 0; i < fileNames.size(); i++)
            f << path << "/" << fileNames[i] << "\n";
        f.close();
    }
    else
    {
        printf("Error opening tmp/find.txt\n");
    }
}

int(__cdecl *old_system)(const char *command);
int jmp_system(const char *command)
{
    printf("%s\n", command);

    if (strcmp(command, "find /tmp/data/target/ -type f -name \"*.target.gz\"  -type f | sort >/tmp/find.txt") == 0)
    {
        findTarget("tmp/data/target", ".target.gz");
        return 0;
    }

    if (strcmp(command, "find data/target/us -type f -name \"*.target.gz\"  -type f | sort >/tmp/find.txt") == 0)
    {
        findTarget("data/target/us", ".target.gz");
        return 0;
    }

    if (strcmp(command, "find data/target/jp -type f -name \"*.target.gz\"  -type f | sort >/tmp/find.txt") == 0)
    {
        findTarget("data/target/jp", ".target.gz");
        return 0;
    }

    if (strcmp(command, "find /tmp/data/ranking/ -type f -name \"*.rank\"  -type f | sort >/tmp/find.txt") == 0)
    {
        findTarget("tmp/data/ranking", ".rank");
        return 0;
    }

    if (strcmp(command, "find /tmp/data/maxicoin/ -type f -name \"*.maxicoin\"  -type f | sort >/tmp/find.txt") == 0)
    {
        findTarget("tmp/data/maxicoin", ".maxicoin");
        return 0;
    }

    if (strcmp(command, "find /tmp/data/joinstar/ -type f -name \"*.joinstar\"  -type f | sort >/tmp/find.txt") == 0)
    {
        findTarget("tmp/data/joinstar", ".joinstar");
        return 0;
    }

    if (strcmp(command, "cp -f data/target/*.target.gz /tmp/data/target/ 2>/dev/null") == 0)
    {
        // this is dumb
        DIR *dir = opendir("data/target");
        if (dir)
        {
            while (true)
            {
                struct dirent *d = readdir(dir);
                if (!d)
                    break;
                if (!strstr(d->d_name, ".target.gz"))
                    continue;
                std::string srcName = "data/target/";
                srcName += d->d_name;

                std::string dstName = "tmp/data/target/";
                dstName += d->d_name;

                std::ifstream src(srcName, std::ios::binary);
                if (!src.is_open())
                {
                    printf("Can't open %s\n", srcName.c_str());
                    continue;
                }

                src.seekg(0, std::ios::end);
                size_t size = src.tellg();
                src.seekg(0, std::ios::beg);

                char *data = new char[size];
                if (!src.read((char *)data, size))
                {
                    delete[] data;
                    src.close();
                    printf("Can't read %s\n", srcName.c_str());
                    continue;
                }

                std::ofstream dst(dstName, std::ios::binary);
                if (!dst.is_open())
                {
                    delete[] data;
                    src.close();
                    printf("Can't open %s\n", dstName.c_str());
                    continue;
                }

                dst.write(data, size);

                src.close();
                dst.close();
            }
            closedir(dir);
        }
        else
        {
            printf("can't open data/target\n");
        }
        return 0;
    }

    return old_system(command);
}

void init()
{
    try
    {
        toml::table config = toml::parse_file("config.toml");

        if (config["fullscreen"].is_boolean())
        {
            CONFIG.fullscreen = config["fullscreen"].as_boolean()->get();
        }
        else
        {
            CONFIG.fullscreen = false;
        }

        if (config["input_emu"].is_boolean())
        {
            CONFIG.input_emu = config["input_emu"].as_boolean()->get();
        }
        else
        {
            CONFIG.input_emu = true;
        }

        if (config["card_emu"].is_boolean())
        {
            CONFIG.card_emu = config["card_emu"].as_boolean()->get();
        }
        else
        {
            CONFIG.card_emu = true;
        }

        if (config["dongle"].is_string())
        {
            CONFIG.dongle = config["dongle"].as_string()->get();
        }
        else
        {
            CONFIG.dongle = "";
        }

        if (config["local_ip"].is_string())
        {
            CONFIG.local_ip = config["local_ip"].as_string()->get();
        }
        else
        {
            CONFIG.local_ip = "";
        }

        if (config["deadzone"].is_number())
        {
            CONFIG.deadzone = config["deadzone"].as_floating_point()->get();
        }
        else
        {
            CONFIG.deadzone = 0.01f;
        }

        if (config["width"].is_number())
        {
            CONFIG.width = config["width"].as_integer()->get();
        }
        else
        {
            CONFIG.width = 640;
        }

        if (config["height"].is_number())
        {
            CONFIG.height = config["height"].as_integer()->get();
        }
        else
        {
            CONFIG.height = 480;
        }

        if (config["ya_card_emu"].is_boolean())
        {
            CONFIG.ya_card_emu = config["ya_card_emu"].as_boolean()->get();
        }
        else
        {
            CONFIG.ya_card_emu = false;
        }
    }
    catch (const toml::parse_error &err)
    {
    }

    try
    {
        toml::table config = toml::parse_file("keyconfig.toml");
#define KEYCONFIG_ARRAY(cfgname, structname)                      \
    {                                                             \
        std::vector<std::string> keys = {};                       \
        if (config[cfgname].is_array())                           \
        {                                                         \
            toml::array configkeys = *config[cfgname].as_array(); \
            for (size_t i = 0; i < configkeys.size(); i++)        \
            {                                                     \
                if (!configkeys[i].is_string())                   \
                    continue;                                     \
                keys.push_back(configkeys[i].as_string()->get()); \
            }                                                     \
        }                                                         \
        KEYCONFIG.structname = poll::parse_keybinding(keys);      \
    }

        KEYCONFIG_ARRAY("TEST", test);
        KEYCONFIG_ARRAY("SERVICE", service);
        KEYCONFIG_ARRAY("CARD_INSERT", card_insert);

        KEYCONFIG_ARRAY("GEAR_NEXT", gear_next);
        KEYCONFIG_ARRAY("GEAR_PREVIOUS", gear_previous);
        KEYCONFIG_ARRAY("GEAR_NEUTRAL", gear_neutral);
        KEYCONFIG_ARRAY("GEAR_FIRST", gear_first);
        KEYCONFIG_ARRAY("GEAR_SECOND", gear_second);
        KEYCONFIG_ARRAY("GEAR_THIRD", gear_third);
        KEYCONFIG_ARRAY("GEAR_FOURTH", gear_fourth);
        KEYCONFIG_ARRAY("GEAR_FIFTH", gear_fifth);
        KEYCONFIG_ARRAY("GEAR_SIXTH", gear_sixth);
        KEYCONFIG_ARRAY("GEAR_UP", gear_up);
        KEYCONFIG_ARRAY("GEAR_LEFT", gear_left);
        KEYCONFIG_ARRAY("GEAR_DOWN", gear_down);
        KEYCONFIG_ARRAY("GEAR_RIGHT", gear_right);

        KEYCONFIG_ARRAY("PERSPECTIVE", perspective);
        KEYCONFIG_ARRAY("INTRUDE", intrude);
        KEYCONFIG_ARRAY("GAS", gas);
        KEYCONFIG_ARRAY("BRAKE", brake);
        KEYCONFIG_ARRAY("WHEEL_LEFT", wheel_left);
        KEYCONFIG_ARRAY("WHEEL_RIGHT", wheel_right);
#undef KEYCONFIG_ARRAY
    }
    catch (const toml::parse_error &err)
    {
    }

    Line::Hook(Line::DlSym(NULL, "hasp_cleanup"), (void *)undachi);
    Line::Hook(Line::DlSym(NULL, "hasp_decrypt"), (void *)undachi);
    Line::Hook(Line::DlSym(NULL, "hasp_encrypt"), (void *)undachi);
    Line::Hook(Line::DlSym(NULL, "hasp_free"), (void *)undachi);
    Line::Hook(Line::DlSym(NULL, "hasp_get_rtc"), (void *)undachi);
    Line::Hook(Line::DlSym(NULL, "hasp_get_sessioninfo"), (void *)undachi);
    Line::Hook(Line::DlSym(NULL, "hasp_get_size"), (void *)hasp_size);
    Line::Hook(Line::DlSym(NULL, "hasp_legacy_set_rtc"), (void *)undachi);
    Line::Hook(Line::DlSym(NULL, "hasp_login"), (void *)hasp_login);
    Line::Hook(Line::DlSym(NULL, "hasp_logout"), (void *)undachi);
    Line::Hook(Line::DlSym(NULL, "hasp_read"), (void *)hasp_read);
    Line::Hook(Line::DlSym(NULL, "hasp_write"), (void *)undachi);
    Line::Hook(Line::DlSym(NULL, "hasp_get_rtc"), (void *)undachi);
    Line::Hook(Line::DlSym(NULL, "hasp_hasptime_to_datetime"), (void *)undachi);
    Line::Hook(Line::DlSym(NULL, "_ZNK6clHasp7isAvailEv"), (void *)adachi);

    Line::Hook(Line::DlSym(NULL, "_ZN6clMainC1Ev"), (void *)cl_main, (void **)&ORIGINAL_CL_MAIN);

    if (!CONFIG.local_ip.empty())
    {
        Line::Hook(Line::DlSym(NULL, "_ZNK5clNet10getAddressEv"), (void *)get_address);
    }
    else
    {
        Line::Hook(Line::DlSym(NULL, "_ZN18clSeqBootNetThread3runEPv"), (void *)adachi);
    }

    adm::init();
    al::load_al_funcs();

    if (CONFIG.input_emu)
        jamma::init();
    if (CONFIG.card_emu)
        card::init();
    if (CONFIG.width != 640 || CONFIG.height != 480)
        res::init();

    void *proc_bus_usb_devices = Line::Signature("2f 70 72 6f 63 2f 62 75 73");
    if (proc_bus_usb_devices)
        Line::PatchString(proc_bus_usb_devices, "tmp/usb-devices");

    Line::Hook(Line::DlSym(NULL, "_ZN5clNet19setInterfaceAddressEv"), (void *)adachi);

    if (CONFIG.ya_card_emu)
        yac_init();

    Line::Hook(Line::ResolveStub("system"), (void *)jmp_system, (void **)&old_system);
}

void *lib_cg_so = NULL;
void *lib_cgGL_so = NULL;

extern "C"
{
    __declspec(dllexport) void OnInitialize(int version, void **func_tables)
    {
        Line::Init(func_tables);
    }

    __declspec(dllexport) void OnPreExecute(const char *lib_name, uintptr_t base_address)
    {
        if (!strcmp(lib_name, "main"))
        {
            init();
        }
    }

    __declspec(dllexport) bool OnDlOpen(const char *lib_name, void **result)
    {
        if (strcmp(lib_name, "libCg.so") == 0)
        {
            lib_cg_so = dlopen("cg.dll", RTLD_NOW);
            if (!lib_cg_so)
            {
                panic("can't load cg.dll %s", dlerror());
            }
            *result = lib_cg_so;
            return true;
        }

        if (strcmp(lib_name, "libCgGL.so") == 0)
        {
            lib_cgGL_so = dlopen("cgGL.dll", RTLD_NOW);
            if (!lib_cgGL_so)
            {
                panic("can't load cgGL.dll %s", dlerror());
            }
            *result = lib_cgGL_so;
            return true;
        }

        return false;
    }

    __declspec(dllexport) bool OnDlSym(void *handle, const char *symbol, void **result)
    {
        if (handle == lib_cg_so || handle == lib_cgGL_so)
        {
            *result = (void *)dlsym(handle, symbol);
            return true;
        }
        return false;
    }

    // Skibidi rizz fix
    __declspec(dllexport) int __cdecl __mingw_vsscanf(const char *_Str, const char *Format, va_list argp)
    {
        return vsscanf(_Str, Format, argp);
    }

    __declspec(dllexport) int __cdecl __mingw_vsnprintf(char *_DstBuf, size_t _MaxCount, const char *_Format,
                                                        va_list _ArgList)
    {
        return vsnprintf(_DstBuf, _MaxCount, _Format, _ArgList);
    }

    __declspec(dllexport) int __cdecl __mingw_vsprintf(char *a1, const char *a2, va_list a3)
    {
        return vsprintf(a1, a2, a3);
    }
};