#include "execution.hpp"


//compression
#include "zstd.h"
#include "xxhash.h"

//..
#include "memory.hpp"


//encoder
class bytecode_encoder_t : public Luau::BytecodeEncoder {
    inline void encode(uint32_t* data, size_t count) override
    {
        for (auto i = 0u; i < count;)
        {
            auto& opcode = *reinterpret_cast<uint8_t*>(data + i);
            i += Luau::getOpLength(LuauOpcode(opcode));
            opcode *= 227;
        }
    }
};


//compression
static const char kBytecodeMagic[4] = { 'R', 'S', 'B', '1' };
static const unsigned int kBytecodeHashSeed = 42;
static const unsigned int kBytecodeHashMultiplier = 41;

std::string compress_bytecode(const std::string_view& source)
{
    std::uint32_t dataSize = source.size();

    std::uint32_t maxSize = ::ZSTD_compressBound(dataSize);

    std::vector<char> compressed(maxSize);
    std::uint32_t compressedSize = ::ZSTD_compress(&compressed[0], maxSize, &source[0], dataSize, ::ZSTD_maxCLevel());

    std::string result = kBytecodeMagic;
    result.append(reinterpret_cast<char*>(&dataSize), sizeof(dataSize));
    result.append(&compressed[0], compressedSize);

    std::uint32_t hash = ::XXH32(&result[0], result.size(), kBytecodeHashSeed);

    byte hashBytes[4];
    std::memcpy(hashBytes, &hash, sizeof(hash));

    constexpr const int CHUNK_SIZE = 16;

    byte keyBytes[CHUNK_SIZE] = {};

    for (std::size_t i = 0; i < CHUNK_SIZE; i++)
        keyBytes[i] = hashBytes[i & 3] + char(kBytecodeHashMultiplier * i);

    std::size_t index = 0;

    std::size_t resultSize = result.size();

    if (resultSize >= CHUNK_SIZE)
    {
        do
        {
            for (int i = 0; i < CHUNK_SIZE; i++)
            {
                result[index + i] ^= keyBytes[i];
                keyBytes[i] += 0x90;
            }

            index += CHUNK_SIZE;
        } while ((index + CHUNK_SIZE) <= resultSize);
    }


    while (index < resultSize)
    {
        result[index] ^= hashBytes[index & 3] + char(kBytecodeHashMultiplier * index);
        index++;
    }
    return result;
}

static auto deobfuscate_proto_p(uintptr_t proto) -> uintptr_t
{
    return (proto + 8) - *reinterpret_cast<uintptr_t*>(proto + 8);
}

static void raise_proto(uintptr_t proto)
{
    uintptr_t maxcap = 0xFFFFFFFF | 0x3F;
    *reinterpret_cast<uintptr_t**>(proto + 0x78) = &maxcap; //proto->userdata это не меняется
    uintptr_t* p = (uintptr_t*)(deobfuscate_proto_p(proto)); //proto->p
    for (int i = 0; i < *reinterpret_cast<int*>(proto + 152); i++) //proto->sizep
        raise_proto(p[i]);
}

void SetIdentity(lua_State* L, int identity)
{
    *reinterpret_cast<uintptr_t*>(*reinterpret_cast<uintptr_t*>(L + 0x78) + 0x30) = identity;
}

static auto set_context(uintptr_t userdata, int level) {

    *reinterpret_cast<uintptr_t*>(userdata + 0x30) = level;
    *reinterpret_cast<uintptr_t*>(userdata + 0x48) = 0xFFFFFFFF;
}

//execution
typedef int(__fastcall* r_luaUVM_Load)(lua_State*, void* source, const char* name, int env);
r_luaUVM_Load rbx_luavmload = reinterpret_cast<r_luaUVM_Load>(offset(offsets::luavm_load));

typedef int(__fastcall* task_defer)(lua_State*);
task_defer taskdefer = reinterpret_cast<task_defer>(offset(offsets::taskDefer));

#define incrementtoplol(L) *(std::uintptr_t*)(L + 0x10) -= 0x10 * 1

/// <summary>
/// Robloxshi kodis gashvebis sashualeba
/// </summary>
/// <param name="LuaState">-> mtavari state sadac warmoebda kvelaferi</param>
/// <param name="source">-> kodi romelic unda gavushvat</param>
/// <returns>statusis kods abrunebs</returns>
int execute(lua_State* LuaState, std::string source)
{
    const char* name = "melancholia";
    source = "spawn(function()\n" + source + "\nend)";
    r_print(0, "1");
    
    bytecode_encoder_t encoder;
    std::string bytecode = Luau::compile(source, { 2, 1, 0 }, {}, &encoder);
    r_print(0, "2");
    std::string compressed_src = compress_bytecode(bytecode);
    r_print(0, "3");
    int status = rbx_luavmload(LuaState, &compressed_src, name, NULL);
    //raise_proto(reinterpret_cast<uintptr_t>(&LuaState->gclist->p));
    //set_context(reinterpret_cast<uintptr_t>(LuaState->gclist->p.userdata), 8);
    //SetIdentity(LuaState, 8);
    r_print(0, "4");
    taskdefer(LuaState);
    r_print(0, "5");
    *(uintptr_t*)(LuaState + 0x10) -= 1 * 0x18;
    return status;
}