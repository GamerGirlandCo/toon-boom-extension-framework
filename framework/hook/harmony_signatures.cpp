#include "harmony_signatures.hpp"

#include "sigscan.hpp"

#include <algorithm>
#include <array>
#include <cstddef>
#include <cstdint>
#include <vector>

namespace toon_boom_module::harmony {
namespace {

std::size_t count_forward(const std::byte* p, const std::byte* end, std::uint8_t value) {
  std::size_t n = 0;
  while (p < end && static_cast<std::uint8_t>(*p) == value) {
    ++n;
    ++p;
  }
  return n;
}

std::size_t count_backward(const std::byte* begin, const std::byte* p, std::uint8_t value) {
  std::size_t n = 0;
  while (p > begin && static_cast<std::uint8_t>(p[-1]) == value) {
    ++n;
    --p;
  }
  return n;
}

bool looks_like_function_boundary(const std::byte* text_begin,
                                  std::size_t text_size,
                                  const std::byte* match_addr,
                                  std::size_t pattern_size) {
  const auto* text_end = text_begin + text_size;
  if (match_addr < text_begin || match_addr + pattern_size > text_end) return false;

  // For HarmonyPremium's SCR_ScriptRuntime_getEngine thunk, IDA shows:
  //   48 8B 01 48 8B 40 28 C3 CC CC CC ...
  // So: require a run of int3 padding immediately after the ret.
  constexpr std::size_t kMinCcAfter = 4;

  const auto* after = match_addr + pattern_size;
  if (after >= text_end) return false;

  const auto cc_after = count_forward(after, text_end, 0xCC);
  if (cc_after < kMinCcAfter) return false;

  // Optional: also prefer that the match is preceded by at least one 0xCC,
  // unless it happens to be at the start of the section.
  if (match_addr != text_begin) {
    const auto cc_before = count_backward(text_begin, match_addr, 0xCC);
    if (cc_before == 0) return false;
  }

  return true;
}

struct FunctionRange {
  std::uintptr_t begin{};
  std::uintptr_t end{};
};

std::optional<FunctionRange> function_range_from_unwind(HMODULE target_module, std::uintptr_t addr) {
  if (!target_module) return std::nullopt;

  DWORD64 image_base = 0;
  const auto* rf = ::RtlLookupFunctionEntry(static_cast<DWORD64>(addr), &image_base, nullptr);
  if (!rf || image_base == 0) return std::nullopt;

  // BeginAddress/EndAddress are RVAs from image_base.
  const auto begin = static_cast<std::uintptr_t>(image_base + rf->BeginAddress);
  const auto end = static_cast<std::uintptr_t>(image_base + rf->EndAddress);
  if (begin >= end) return std::nullopt;

  return FunctionRange{begin, end};
}

}  // namespace

std::optional<std::uintptr_t> find_SCR_ScriptRuntime_getEngine(HMODULE target_module) {
  // Exact bytes from IDA at HarmonyPremium.exe:0x14082BCD0:
  //   48 8B 01 48 8B 40 28 C3
  constexpr std::string_view kPattern = "48 8B 01 48 8B 40 28 C3";

  auto text = toon_boom_module::sigscan::get_pe_section(target_module, ".text");
  if (!text) return std::nullopt;

  const auto pat = toon_boom_module::sigscan::parse_ida_pattern(kPattern);
  auto matches = toon_boom_module::sigscan::find_all(*text, pat);
  if (matches.empty()) return std::nullopt;

  // Filter for plausible function boundaries to reduce collisions with other
  // identical byte sequences embedded in the middle of code.
  std::vector<const std::byte*> filtered;
  filtered.reserve(matches.size());
  for (const auto* m : matches) {
    if (looks_like_function_boundary(text->begin, text->size, m, pat.bytes.size())) {
      filtered.push_back(m);
    }
  }

  if (filtered.size() != 1) return std::nullopt;
  return reinterpret_cast<std::uintptr_t>(filtered[0]);
}

std::optional<std::uintptr_t> find_SCR_ScriptManager_ctor(HMODULE target_module) {
  // This is a mid-function signature extracted from HarmonyPremium.exe around:
  //   QString("___scriptManager___"); defineGlobalQObject(...)
  //   QString("include");           defineGlobalFunction(QS_include)
  //   QString("require");           defineGlobalFunction(QS_require)
  //
  // RIP-relative displacements and call targets are wildcarded.
  //
  // Source bytes were pulled from IDA around 0x14081FEE0.
  constexpr std::string_view kPattern =
      "48 8B 18 "
      "48 8D 15 ?? ?? ?? ?? "
      "48 8D 4C 24 30 "
      "FF 15 ?? ?? ?? ?? "
      "90 "
      "4C 8B C6 "
      "48 8D 54 24 30 "
      "48 8B CB "
      "E8 ?? ?? ?? ?? "
      "90 "
      "48 8D 4C 24 30 "
      "FF 15 ?? ?? ?? ?? "
      "48 8B 46 20 "
      "48 8B 18 "
      "48 8D 15 ?? ?? ?? ?? "
      "48 8D 4C 24 30 "
      "FF 15 ?? ?? ?? ?? "
      "90 "
      "4C 8D 05 ?? ?? ?? ?? "
      "48 8D 54 24 30 "
      "48 8B CB "
      "E8 ?? ?? ?? ?? "
      "90 "
      "48 8D 4C 24 30 "
      "FF 15 ?? ?? ?? ?? "
      "48 8B 46 20 "
      "48 8B 18 "
      "48 8D 15 ?? ?? ?? ?? "
      "48 8D 4C 24 30 "
      "FF 15 ?? ?? ?? ?? "
      "90 "
      "4C 8D 05 ?? ?? ?? ?? "
      "48 8D 54 24 30 "
      "48 8B CB "
      "E8 ?? ?? ?? ?? "
      "90 "
      "48 8D 4C 24 30 "
      "FF 15 ?? ?? ?? ??";

  auto text = toon_boom_module::sigscan::get_pe_section(target_module, ".text");
  if (!text) return std::nullopt;

  const auto pat = toon_boom_module::sigscan::parse_ida_pattern(kPattern);
  auto hits = toon_boom_module::sigscan::find_all(*text, pat);
  if (hits.empty()) return std::nullopt;

  // Convert each hit to its containing function start via unwind info, and
  // keep only plausible ctor-sized functions (~0x280 in the analyzed build).
  constexpr std::size_t kMinSize = 0x200;
  constexpr std::size_t kMaxSize = 0x400;

  std::vector<std::uintptr_t> candidates;
  candidates.reserve(hits.size());

  for (const auto* hit : hits) {
    const auto hit_addr = reinterpret_cast<std::uintptr_t>(hit);
    auto fr = function_range_from_unwind(target_module, hit_addr);
    if (!fr) continue;

    const auto size = static_cast<std::size_t>(fr->end - fr->begin);
    if (size < kMinSize || size > kMaxSize) continue;

    // Ensure the function is inside .text.
    const auto text_begin = reinterpret_cast<std::uintptr_t>(text->begin);
    const auto text_end = text_begin + text->size;
    if (fr->begin < text_begin || fr->end > text_end) continue;

    candidates.push_back(fr->begin);
  }

  if (candidates.empty()) return std::nullopt;

  std::sort(candidates.begin(), candidates.end());
  candidates.erase(std::unique(candidates.begin(), candidates.end()), candidates.end());

  if (candidates.size() != 1) return std::nullopt;
  return candidates[0];
}

}  // namespace toon_boom_module::harmony


