#pragma once
#include <cstdint>
namespace offsets
{
    inline int64_t print = 0x124D0D0;

    //Lua State
    inline int64_t getGlobalState = 0xCC3B40;
    inline int64_t decryptState = 0xA9F330;

    //execution
    inline int64_t luavm_load = 0xAA1860;
    inline int64_t taskDefer = 0xE336D0;
}