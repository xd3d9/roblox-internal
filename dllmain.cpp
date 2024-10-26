#include"Windows.h"
#include <iostream>
#include <vector>
#include <sstream>
#include <string>
#include <chrono>
#include <thread>
#include <filesystem>
#include <ShlObj.h>
#include <fstream>
#include <regex>
#include <format>

#include "datamodel.hpp"
#include "version.hpp"

#include <atltrace.h>
#include "utils.h"
#include "Luau/VM/src/lstate.h"
#include "offsets.h"


void SetIdentity(int Identity, lua_State* LuaState)
{
    const auto Userdata = (__int64)(LuaState->userdata);
    *(__int64*)(Userdata + 0x30) = Identity;
    *(__int64*)(Userdata + 0x48) = 0x3FFFFF00 | 0x3F; /* Locked to WebService capabilities */
}

DWORD WINAPI EntryPoint(LPVOID lpParam) {
    print_banner();

    //datamodel
    instance* game = (instance*)(*((uintptr_t*)(datamodel::get_renderview() + 0x118)) + 0x198);
    r_print(1, std::format("Datamodel: {:#x}", reinterpret_cast<std::uintptr_t>(game)).c_str());

    //scriptcontext
    instance* scriptcontext = getScriptContext(game);
    r_print(1, std::format("{}: {:#x}", scriptcontext->class_descriptor->classname, reinterpret_cast<std::uintptr_t>(scriptcontext)).c_str());

    using GlobalState = uintptr_t(*)(uintptr_t, uintptr_t*, uintptr_t*);
    GlobalState getGlobalState = reinterpret_cast<GlobalState>(offset(offsets::getGlobalState));



    /*
                    v37 = sub_1097180(v36 + 0x110, a3, a6);         getGlobalState
                    v19 = sub_E7FE00(v37 + 0x88);                   LuaState
     */
     //global state vchitavt
    uintptr_t Identity = { 8 };
    uintptr_t Script[2] = { 0,0 };
    uintptr_t K = NULL;
    uintptr_t G = getGlobalState((std::uintptr_t)scriptcontext + 0x110, &Identity, Script);
    r_print(1, std::format("Global State: {:#x}", G).c_str());

    //luastate shi
    using decryptLuaStateFunction = uintptr_t(*)(uintptr_t a1);
    decryptLuaStateFunction decryptLuaState = reinterpret_cast<decryptLuaStateFunction>(offset(offsets::decryptState));
    lua_State* LuaState = reinterpret_cast<lua_State*>(decryptLuaState(G + 0x88));
    r_print(1, std::format("Decrypted Lua State: {:#x}", reinterpret_cast<std::uintptr_t>(LuaState)).c_str());

    //shetkobineba imisa rom kvelaferi dainitda
    r_print(0, "Kvelaferi Gamzadebulia !\n\n\n");
    execute(LuaState, "printidentity()");
    /*
    while (true) {
        std::vector<uint32_t> identity_capabilities = { 0, 3, 0, 11, 3, 1, 11, 63, 63, 12 };
        int64_t userdata = *(int64_t*)(LuaState + 0x78);
        uintptr_t identity = *reinterpret_cast<uintptr_t*>(userdata + 0x30);
        uintptr_t enc_capabilities = identity_capabilities[identity] ^ 0xFFFFFFFF;
        *reinterpret_cast<std::int64_t*>(userdata + 0x30) = 8;
        *reinterpret_cast<std::int64_t*>(userdata + 0x48) = enc_capabilities;

        //execution
        execute(LuaState, "printidentity()");
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
    */
}


BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
                     )
{
    DisableThreadLibraryCalls(hModule);
    switch (ul_reason_for_call) {
    case DLL_PROCESS_ATTACH: {
        std::thread(EntryPoint, nullptr).detach();
    }
                           break;

    case DLL_PROCESS_DETACH:
    case DLL_THREAD_ATTACH:
    case DLL_THREAD_DETACH:
        break;
    }

    return TRUE;
}


__declspec(dllexport) LRESULT CALLBACK callback(
    int code,
    WPARAM wParam,
    LPARAM lParam
) {
    // Call the next hook in the chain
    return CallNextHookEx(NULL, code, wParam, lParam);
}