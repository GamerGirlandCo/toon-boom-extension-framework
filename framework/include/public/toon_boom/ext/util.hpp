#pragma once

#include <format>
#include <iostream>
#include <streambuf>

#if !defined(TB_EXT_FRAMEWORK_DEBUG)
#define TB_EXT_FRAMEWORK_DEBUG 0
#endif
struct NullBuffer : std::streambuf {
  int overflow(int c) { return c; }
};

std::string addrToHex(void *addr);

std::string constAddrToHex(const void *addr);

extern std::ostream &out;
