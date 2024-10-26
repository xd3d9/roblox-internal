#pragma once
#include "memory.hpp"
#include "execution.hpp"



#define DBOUT( s )            \
{                             \
   std::ostringstream os_;    \
   os_ << s;                   \
   OutputDebugString( os_.str().c_str() );  \
}



#define version std::format("{}.{}.{}.{}", VERSION_MAJOR,VERSION_MINOR,VERSION_PATCH,VERSION_TEST)
#define robloxversion "eadc3c90bb1a4267"

//nu pirveli rac daiprinteba esaa karoche
void print_banner()
{
    r_print(0, "#    #  ######   #         ##    #    #   ####   #    #   ####   #       #     ##  ");
    r_print(0, "##  ##  #        #        #  #   ##   #  #    #  #    #  #    #  #       #    #  # ");
    r_print(0, "# ## #  #####    #       #    #  # #  #  #       ######  #    #  #       #   #    #");
    r_print(0, "#    #  #        #       ######  #  # #  #       #    #  #    #  #       #   ######");
    r_print(0, "#    #  #        #       #    #  #   ##  #    #  #    #  #    #  #       #   #    #");
    r_print(0, "#    #  ######   ######  #    #  #    #   ####   #    #   ####   ######  #   #    #");
    r_print(2, std::format("Current version is {} ({}), {} {}", version, robloxversion, __DATE__, __TIME__).c_str());
}

std::string uintptrToString(uintptr_t value) {
    std::stringstream ss;
    ss << value;
    return ss.str();
}