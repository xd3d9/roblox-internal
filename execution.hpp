//luau
#include <Luau/Compiler/include/Luau/Compiler.h>
#include "Luau/Compiler/include/Luau/BytecodeBuilder.h"
#include <Luau/BytecodeUtils.h>
#include <Luau/VM/src/lstate.h>

//w32
#include <string>
#include <ShlObj.h>

std::string compress_bytecode(const std::string_view& source);
int execute(lua_State* LuaState, std::string source);