#include "include/public/toon_boom/ext/util.hpp"

std::string addrToHex(void* addr) {
  return std::format("0x{:010X}", reinterpret_cast<uintptr_t>(addr));
}

std::string constAddrToHex(const void* addr) {
  return std::format("0x{:010X}", reinterpret_cast<const uintptr_t>(addr));
}