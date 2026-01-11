#pragma once
#include <windows.h>
#include <TlHelp32.h>
#include <string>
#include <vector>
#include <filesystem>
#include <set>


std::vector<std::pair<std::string, std::filesystem::directory_entry>> findToonBoomVersions();

std::vector<std::pair<std::string, std::filesystem::directory_entry>> findSubEntries(const std::filesystem::directory_entry& entry);

DWORD GetProcessIdByName(const std::string& processName);