#include "finder.h"
#include <iostream>

namespace fs = std::filesystem;

std::vector<std::pair<std::string, std::filesystem::directory_entry>> findToonBoomVersions() {
  std::string programFiles("C:\\Program Files\\Toon Boom Animation\\");
  std::string programFilesX86("C:\\Program Files (x86)\\Toon Boom Animation\\");

  std::vector<std::pair<std::string, std::filesystem::directory_entry>> versions;
  try {
    for (const auto &entry : fs::directory_iterator(programFiles)) {
      auto subEntries = findSubEntries(entry);
      versions.insert(versions.end(), subEntries.begin(), subEntries.end());
    }
  } catch (const std::filesystem::filesystem_error &e) {
  }
  try {
    for (const auto &entry : fs::directory_iterator(programFilesX86)) {
      auto subEntries = findSubEntries(entry);
      versions.insert(versions.end(), subEntries.begin(), subEntries.end());
    }
  } catch (const std::filesystem::filesystem_error &e) {
    std::cerr << "Error: " << e.what() << std::endl;
  }

  return versions;
}

std::vector<std::pair<std::string, std::filesystem::directory_entry>>
findSubEntries(const std::filesystem::directory_entry &entry) {
  std::vector<std::pair<std::string, std::filesystem::directory_entry>> versions;
  std::set<std::string> exeNames = {"StoryboardPro.exe", "HarmonyPremium.exe",
                                    "HarmonyAdvanced.exe",
                                    "HarmonyEssentials.exe"};
  if (fs::is_directory(entry.path())) {
    if (entry.path().filename().string().find("Toon Boom") !=
        std::string::npos) {
      for (const auto &subEntry :
           fs::recursive_directory_iterator(entry.path())) {
        try {
          if (fs::is_regular_file(subEntry.path()) &&
              exeNames.contains(subEntry.path().filename().string())) {
            versions.push_back(std::make_pair(entry.path().filename().string(), subEntry));
          }
        } catch (const std::filesystem::filesystem_error &e) {
					continue;
        }
      }
    }
  }
  return versions;
}

DWORD GetProcessIdByName(const std::string& processName) {
    DWORD pid = 0;
    HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if (INVALID_HANDLE_VALUE == hSnapshot) return 0;

    PROCESSENTRY32 pe = { sizeof(pe) };
    if (Process32First(hSnapshot, &pe)) {
        do {
            // std::cout << "Process name: " << pe.szExeFile << std::endl;
            if (processName == std::string(pe.szExeFile)) {
                pid = pe.th32ProcessID;
                break;
            }
        } while (Process32Next(hSnapshot, &pe));
    }

    CloseHandle(hSnapshot);
    return pid;
}