#include "./finder.h"
#include <argparse/argparse.hpp>
#include <iostream>
#include <vector>


argparse::ArgumentParser *&createProgram(int argc, char *argv[]) {
  static auto program = new argparse::ArgumentParser("tb-injector-cli.exe");
  program->set_prefix_chars("-/");
  program->set_assign_chars(":=");
  program->add_argument("-h", "--help")
      .help("show help message and exit")
      .implicit_value(true);
  program->add_argument("-v", "--debug")
      .help("log program's stdout and stderr to the given file")
      .default_value("");
  program->add_argument("-p", "--program")
      .help("path to a Toon Boom program")
      .default_value("")
      .nargs(0, 1);
  program->add_argument("-i", "--dep")
      .help("path to an additional dll to copy into program's install dir")
      .default_value<std::vector<std::string>>({})
      .append();
  program->add_argument("-d", "--dll").append().help("path to a dll to inject");

  return program;
}

void copyDll(const std::string &dllPath,
             const std::filesystem::directory_entry &entry, bool isDebug,
             bool isDep) {
	auto absPath = std::filesystem::absolute(dllPath);
  if (!std::filesystem::exists(absPath)) {
    std::cerr << "[warning] dll path " << absPath << " does not exist"
              << std::endl;
    return;
  }
  std::filesystem::copy(absPath,
                        entry.path().parent_path() /
                            std::filesystem::path(dllPath).filename().string(),
                        std::filesystem::copy_options::overwrite_existing);
  if (isDebug) {
    std::cout << "Copied " << (isDep ? "dependency dll" : "dll") << " at "
              << absPath << " to "
              << entry.path().parent_path() /
                     std::filesystem::path(dllPath).filename().string()
              << std::endl;
  }
}

int main(int argc, char *argv[]) {
  auto args = createProgram(argc, argv);
  try {
    args->parse_args(argc, argv);
  } catch (const std::exception &e) {
    std::cerr << "Error: " << e.what() << std::endl;
    std::cerr << args;
    return 1;
  }
  if (args->present<bool>("-h")) {
    std::cout << args->help().str() << std::endl;
    return 0;
  }

  bool isDebug = false;
  std::string logFile = "";
  std::filesystem::directory_entry entry;

  if (args->get<std::string>("-v") != "") {
    logFile = args->get<std::string>("-v");
    std::cout << "Logging to " << (logFile == "-" ? "console" : logFile)
              << std::endl;
  }

  std::string program = args->get<std::string>("-p");
  if (program == "" || !std::filesystem::exists(program)) {
    std::vector<std::pair<std::string, std::filesystem::directory_entry>>
        versions = findToonBoomVersions();
    bool isValidOption = false;
    std::cout
        << "The following Toon Boom software was detected on your system: "
        << std::endl;
    for (int i = 0; i < versions.size(); i++) {
      std::cout << "\t" << "#" << i + 1 << ": " << versions[i].first
                << std::endl;
    }
    while (!isValidOption) {
      std::cout << "Please pick a number between 1 and " << versions.size()
                << ": ";
      int choice;
      std::cin >> choice;
      if (choice >= 1 && choice <= versions.size()) {
        isValidOption = true;
        program = versions[choice - 1].second.path().filename().string();
        entry = versions[choice - 1].second;
      }
    }
  } else {
    entry = std::filesystem::directory_entry(program);
  }
  if (isDebug) {
    std::cout << "Target executable: " << program << std::endl;
  }
  auto dllPaths = args->get<std::vector<std::string>>("-d");
  if (dllPaths.size() == 0) {
    std::cout << "must provide at least one dll path" << std::endl;
    return 1;
  }

  auto dllDeps = args->get<std::vector<std::string>>("-i");
  for (auto dllPath : dllPaths) {
    copyDll(dllPath, entry, isDebug, false);
  }
  for (auto dllPath : dllDeps) {
    copyDll(dllPath, entry, isDebug, true);
  }
  STARTUPINFO si;
  PROCESS_INFORMATION pi;
  ZeroMemory(&si, sizeof(si));
  ZeroMemory(&pi, sizeof(pi));
  SECURITY_ATTRIBUTES sattr;
  ZeroMemory(&sattr, sizeof(sattr));
  sattr.bInheritHandle = FALSE;
  si.cb = sizeof(si);
  if (!CreateProcess(NULL, entry.path().string().data(), &sattr, NULL, FALSE,
                       CREATE_SUSPENDED | CREATE_NO_WINDOW, NULL,
                      entry.path().parent_path().string().data(), &si, &pi)) {
    std::cerr << "Failed to create process" << std::endl;
    return 1;
  }
  std::cout << "Target process ID: " << pi.dwProcessId << std::endl;
  HANDLE hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, pi.dwProcessId);

  HMODULE hKernel32 = GetModuleHandleA("kernel32.dll");
  FARPROC hLoadLibraryA = GetProcAddress(hKernel32, "LoadLibraryA");
  for (auto dllPath : dllPaths) {
		auto realPath = (std::filesystem::path(program).parent_path().string() + "/" + std::filesystem::absolute(dllPath).filename().string());

    LPVOID remoteBuffer =
        VirtualAllocEx(hProcess, NULL, dllPath.size() + 1,
                       MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
    if (remoteBuffer == NULL) {
      std::cerr << "Failed to allocate memory in target process" << std::endl;
      return 1;
    }
    if (!WriteProcessMemory(hProcess, remoteBuffer, realPath.data(),
                            realPath.size() + 1, NULL)) {
      std::cerr << "Failed to write process memory" << std::endl;
      CloseHandle(hProcess);
      return 1;
    }
    DWORD remoteTID;
    HANDLE hThread = CreateRemoteThread(hProcess, NULL, 0,
                                        (LPTHREAD_START_ROUTINE)hLoadLibraryA,
                                        remoteBuffer, 0, &remoteTID);
    if (hThread == NULL) {
      std::cerr << "Failed to create remote thread" << std::endl;
      VirtualFreeEx(hProcess, remoteBuffer, 0, MEM_RELEASE);
      CloseHandle(hProcess);
      return 1;
    }
      std::cout << "Remote thread ID: " << remoteTID << std::endl;
    WaitForSingleObject(hThread, INFINITE);
		DWORD exitCode;
		GetExitCodeThread(hThread, &exitCode);
		std::cout << "Exit code: " << exitCode << std::endl;
    CloseHandle(hThread);
		VirtualFreeEx(hProcess, remoteBuffer, 0, MEM_RELEASE);
  }
  ResumeThread(pi.hThread);
  CloseHandle(hProcess);
  std::cout << "Congratulations!!! you have been injected :3" << std::endl;

  return 0;
}