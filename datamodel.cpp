#include <string>
#include <ShlObj.h>
#include <vector>
#include <filesystem>
#include <regex>
#include <fstream>

#include "datamodel.hpp"

//task schedulerit sheidzleba chanacvldes
static std::wstring appdata_path()
{
    wchar_t path[MAX_PATH];
    if (SUCCEEDED(SHGetFolderPathW(NULL, CSIDL_LOCAL_APPDATA, NULL, 0, path)))
    {
        return std::wstring(path);
    }
    return L"";
}

std::vector<std::filesystem::path> get_roblox_file_logs()
{
    std::vector<std::filesystem::path> roblox_log;
    std::wstring app_data_path = appdata_path();
    std::wstring roblox_log_path = app_data_path + L"\\Roblox\\logs";

    for (const auto& entry : std::filesystem::directory_iterator(roblox_log_path))
    {
        if (entry.is_regular_file() && entry.path().extension() == ".log" && entry.path().filename().string().find("Player") != std::string::npos)
            roblox_log.push_back(entry.path());
    }

    return roblox_log;
}

std::filesystem::path get_latest_log()
{
    auto logs = get_roblox_file_logs();

    std::sort(logs.begin(), logs.end(), [](const std::wstring& a, const std::wstring& b) {
        return std::filesystem::last_write_time(a) > std::filesystem::last_write_time(b);
        });

    return logs[0];
}

/// <summary>
/// am funkciis sashvalebit shevdzlebt rom datamodel gavchitot, sheidzleba task schedulerit chanacvldes
/// </summary>
/// <returns>RenderView pointer</returns>
std::uint64_t datamodel::get_renderview()
{
    auto latest_log = get_latest_log();

    std::ifstream rbx_log(latest_log);
    std::string rbx_log_line;
    std::regex view_regex(R"(\[FLog::SurfaceController\] SurfaceController\[_:1\]::initialize view\(([0-9A-Fa-f]+)\))");
    std::smatch match;

    while (std::getline(rbx_log, rbx_log_line))
    {
        if (std::regex_search(rbx_log_line, match, view_regex))
        {
            std::string hex_value = match[1];
            std::uint64_t renderview = std::strtoull(hex_value.c_str(), nullptr, 16);
            return renderview;
        }
    }
}