#pragma once

#include <cstdint>
#include "Windows.h"
#include <format>
#include "offsets.h"

class instance
{
public:
    char pad_0000[8]; //0x0000
    class instance* This; //0x0008
    char pad_0010[8]; //0x0010
    class class_descriptor* class_descriptor; //0x0018
    char pad_0020[40]; //0x0020
    char* Name; //0x0048
}; //Size: 0x0050

class class_descriptor
{
public:
    char pad_0000[8]; //0x0000
    char* classname; //0x0008
}; //Size: 0x0010



//ofsetis amoghebis martivi xerxi
inline uintptr_t offset(uintptr_t address) {
    return address + reinterpret_cast<uintptr_t>(GetModuleHandleA(NULL));
}

//print tavistavad
using PrintFunc = int(*)(int, const char*, ...);
inline PrintFunc r_print = reinterpret_cast<PrintFunc>(offset(offsets::print));

/// <summary>
/// rogorc taskscheduleris jobebze vighebt kvela jobs isea magram ak datamodelis childrens vighebt da chamovuvlit sanam ScriptContext s ar gavchitavt
/// </summary>
/// <param name="game">-> DataModelis pointeri</param>
/// <returns>ScriptContextis Instance</returns>
inline instance* getScriptContext(instance* game) {

    std::uintptr_t childrenPtr1 = *((uintptr_t*)(reinterpret_cast<std::uintptr_t>(game) + 0x58)); //address + 0x50; //children 0x50

    if (childrenPtr1 == 0)
        r_print(3, ":(");
    std::uintptr_t childrenptr2 = *((uintptr_t*)(childrenPtr1 + 0x10)); // start (nu ak ari motavsebuli kvela children ar vici ra mak arasworad mara ikos)

    for (int i = 0; i < 250; i++)
    {
        instance* childrenaddress = (instance*)(*((uintptr_t*)((childrenptr2 + (i * 0x10))))); // erterti instance, kvela instance 0x10 tia dashorebuli
        if (childrenaddress == 0) { // xd vamowmebt martla arsebobs tuara
            continue;
        }

        std::string classname = childrenaddress->class_descriptor->classname;
        if (classname == "ScriptContext") {
            return childrenaddress;
        }

    }
}





class job
{
public:
    char pad_0000[144]; //0x0000
    char* name; //0x0090
}; //Size: 0x0098

/// <summary>
/// task schedulers vighebt yleoba memorydan da titeul jobs vkitxulobt
/// </summary>
/// <param name="taskscheduler">-> task scheduleris pointeri</param>
/// <param name="jobname">-> job romelic unda amovighot task scheduleridan, sanam amis saxels ar amoikitxavs makamde gaagrdzelebs mushaobas</param>
/// <returns>Amoghebuli Job rogorc pointerad</returns>
inline job* getTaskSchedulerJob(uintptr_t taskscheduler, std::string jobname) {

    std::uintptr_t Jobs = *((uintptr_t*)(taskscheduler + 0x198)); //address + 0x198; //job 0x198

    if (Jobs == 0)
        r_print(3, ":(");

    for (int i = 0; i < 100; i++)
    {
        job* jobaddress = (job*)(*((uintptr_t*)((Jobs + (i * 0x10))))); // erterti job, kvela job 0x10 tia dashorebuli



        if (jobaddress == 0) { // xd vamowmebt martla arsebobs tuara
            continue;
        }

        //ATLTRACE("xd %s", jobaddress->name);

        //r_print(1, std::format("jobtype: {}", typeid(jobaddress->name).name()).c_str());


        char* name = (char*)(jobaddress + 0x90);

        r_print(1, std::format("job name: {}", *name).c_str());

        //MessageBoxW(NULL, (LPCWSTR)name, L"yleo", MB_OK);


        if (typeid(jobaddress->name).name() != "char * __ptr64") { // ar mkitxot am kods tu naxavt
            continue;
        }


        if ((std::string)jobaddress->name == jobname) {

            return jobaddress;
        }

    }
}