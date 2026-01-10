#include "../include/internal/sigscan.hpp"

#include <algorithm>
#include <cctype>
#include <cstring>
#include <cstdlib>
#include <stdexcept>

namespace toon_boom_module::sigscan {
namespace {

bool is_hex_digit(char c) {
  return std::isxdigit(static_cast<unsigned char>(c)) != 0;
}

std::uint8_t hex_byte_from_2chars(char hi, char lo) {
  auto nybble = [](char c) -> std::uint8_t {
    if (c >= '0' && c <= '9') return static_cast<std::uint8_t>(c - '0');
    c = static_cast<char>(std::tolower(static_cast<unsigned char>(c)));
    if (c >= 'a' && c <= 'f') return static_cast<std::uint8_t>(10 + (c - 'a'));
    throw std::invalid_argument("invalid hex digit");
  };

  return static_cast<std::uint8_t>((nybble(hi) << 4) | nybble(lo));
}

std::vector<std::string_view> split_ws(std::string_view s) {
  std::vector<std::string_view> out;
  std::size_t i = 0;
  while (i < s.size()) {
    while (i < s.size() && std::isspace(static_cast<unsigned char>(s[i]))) ++i;
    if (i >= s.size()) break;
    std::size_t j = i;
    while (j < s.size() && !std::isspace(static_cast<unsigned char>(s[j]))) ++j;
    out.emplace_back(s.substr(i, j - i));
    i = j;
  }
  return out;
}

}  // namespace

Pattern parse_ida_pattern(std::string_view ida_pattern) {
  Pattern p;

  auto toks = split_ws(ida_pattern);
  p.bytes.reserve(toks.size());
  p.mask.reserve(toks.size());

  for (auto tok : toks) {
    if (tok == "?" || tok == "??") {
      p.bytes.push_back(0);
      p.mask.push_back(false);
      continue;
    }

    if (tok.size() != 2 || !is_hex_digit(tok[0]) || !is_hex_digit(tok[1])) {
      throw std::invalid_argument("invalid IDA pattern token: expected 2 hex chars or ??");
    }

    p.bytes.push_back(hex_byte_from_2chars(tok[0], tok[1]));
    p.mask.push_back(true);
  }

  if (p.bytes.empty()) {
    throw std::invalid_argument("empty pattern");
  }
  return p;
}

std::optional<SectionView> get_pe_section(HMODULE module, std::string_view section_name) {
  if (!module) return std::nullopt;
  if (section_name.empty() || section_name.size() > 8) return std::nullopt;

  const auto base = reinterpret_cast<const std::byte*>(module);
  const auto* dos = reinterpret_cast<const IMAGE_DOS_HEADER*>(base);
  if (dos->e_magic != IMAGE_DOS_SIGNATURE) return std::nullopt;

  const auto* nt = reinterpret_cast<const IMAGE_NT_HEADERS*>(base + dos->e_lfanew);
  if (nt->Signature != IMAGE_NT_SIGNATURE) return std::nullopt;

  const IMAGE_SECTION_HEADER* sec = IMAGE_FIRST_SECTION(nt);
  for (unsigned i = 0; i < nt->FileHeader.NumberOfSections; ++i) {
    char name_buf[9] = {};
    std::memcpy(name_buf, sec[i].Name, 8);
    if (section_name == name_buf) {
      const auto* begin = base + sec[i].VirtualAddress;
      const auto size = static_cast<std::size_t>(sec[i].Misc.VirtualSize);
      return SectionView{begin, size};
    }
  }

  return std::nullopt;
}

std::vector<const std::byte*> find_all(SectionView region, const Pattern& pat) {
  std::vector<const std::byte*> matches;

  if (!region.begin || region.size == 0) return matches;
  if (pat.bytes.size() != pat.mask.size()) return matches;
  if (pat.bytes.empty()) return matches;
  if (region.size < pat.bytes.size()) return matches;

  const auto* hay = reinterpret_cast<const std::uint8_t*>(region.begin);
  const auto hay_size = region.size;
  const auto n = pat.bytes.size();

  for (std::size_t i = 0; i + n <= hay_size; ++i) {
    bool ok = true;
    for (std::size_t j = 0; j < n; ++j) {
      if (pat.mask[j] && hay[i + j] != pat.bytes[j]) {
        ok = false;
        break;
      }
    }
    if (ok) matches.push_back(region.begin + i);
  }

  return matches;
}

std::optional<const std::byte*> find_unique(SectionView region, const Pattern& pat) {
  auto all = find_all(region, pat);
  if (all.size() != 1) return std::nullopt;
  return all[0];
}

}  // namespace toon_boom_module::sigscan


