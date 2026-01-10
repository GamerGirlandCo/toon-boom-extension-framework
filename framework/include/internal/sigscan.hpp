#pragma once

#include <cstddef>
#include <cstdint>
#include <optional>
#include <span>
#include <string_view>
#include <vector>

#include <windows.h>

namespace toon_boom_module::sigscan {

struct Pattern {
  std::vector<std::uint8_t> bytes; // pattern bytes (wildcard bytes may be any value)
  std::vector<bool> mask;          // true = match byte, false = wildcard
};

// IDA-style pattern string parser. Examples:
// - "48 8B 01 48 8B 40 28 C3"
// - "48 8B ?? ?? 89"
Pattern parse_ida_pattern(std::string_view ida_pattern);

struct SectionView {
  const std::byte* begin{};
  std::size_t size{};
};

// Reads a PE section by name (e.g. ".text") from a loaded module.
std::optional<SectionView> get_pe_section(HMODULE module, std::string_view section_name);

// Returns all matches in the provided memory region.
std::vector<const std::byte*> find_all(SectionView region, const Pattern& pat);

// Returns the single match or std::nullopt (0 or >1 matches).
std::optional<const std::byte*> find_unique(SectionView region, const Pattern& pat);

}  // namespace toon_boom_module::sigscan


