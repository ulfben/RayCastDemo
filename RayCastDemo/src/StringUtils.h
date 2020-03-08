#pragma once
#include <algorithm>
#include <cassert>
#include <numeric>
#include <sstream>
#include <string>
#include <string_view>
#include <vector>
namespace StringUtils {
using namespace std::literals::string_literals;
using namespace std::literals::string_view_literals;
constexpr std::string_view WHITEPACE(" \t\n\r\f\v"sv);

void to_upper(std::string& text)
{
    std::transform(std::begin(text),
        std::end(text),
        std::begin(text),
        [](char c) { return static_cast<char>(std::toupper(c)); });
}

void to_lower(std::string& text)
{
    std::transform(std::begin(text),
        std::end(text),
        std::begin(text),
        [](char c) { return static_cast<char>(std::toupper(c)); });
}

[[nodiscard]] std::vector<std::string>
split(const std::string& text, std::string_view delimiter = " "sv)
{
    std::vector<std::string> tokens;
    const size_t delimiterSize = delimiter.length();
    size_t start = 0;
    size_t end = 0;
    std::string token;
    do {
        end = text.find(delimiter, start);
        const size_t distance = end - start;
        token = text.substr(start, distance);
        if (!token.empty()) {
            tokens.emplace_back(token);
        }
        start += distance + delimiterSize;
    } while (end != std::string::npos);
    return tokens;
}

template <class Container>
[[nodiscard]] std::string join(const Container values, size_t size, std::string delimiter = ","s){
    using T = typename Container::value_type;    
    auto end = std::begin(values);
    std::advance(end, size);
    return std::accumulate(
        std::begin(values),  
        end,
        std::string(),
        [delimiter](std::string a, T b) -> std::string {
            std::string delim(a.length() > 0 ? delimiter : ""s);
            return a + delim + std::to_string(b);
        });
}

[[nodiscard]] std::string
join(const std::vector<std::string> strings, std::string_view delimiter = ","sv)
{
    // accumulate on std::string will be O(n) from C++20.
    // https://stackoverflow.com/a/18703743
    return std::accumulate(
        std::begin(strings),
        std::end(strings),
        std::string(),
        [delimiter](const std::string& a, const std::string& b) -> std::string {
            std::string delim(a.length() > 0 ? delimiter : ""sv);
            return a + delim + b;
        });
}

void replace(std::string& haystack, std::string_view needle, std::string_view replacement)
{
    size_t pos = 0;
    while ((pos = haystack.find(needle, pos)) != std::string::npos) {
        haystack.replace(pos, needle.length(), replacement);
        pos += replacement.length();
    }
}

inline void
rtrim(std::string& text, std::string_view t = WHITEPACE) noexcept
{
    text.erase(text.find_last_not_of(t) + 1);
}
inline void
ltrim(std::string& text, std::string_view t = WHITEPACE) noexcept
{
    text.erase(0, text.find_first_not_of(t));
}
inline void
trim(std::string& text, std::string_view t = WHITEPACE) noexcept
{
    rtrim(text, t);
    ltrim(text, t);
}

void remove(std::string& string, char letterToErase) noexcept
{
    string.erase(std::remove(std::begin(string), std::end(string), letterToErase),
        std::end(string));
}

void remove(std::string& text, std::string_view wordToErase) noexcept
{
    auto pos = std::string::npos;
    const auto count = wordToErase.length();
    while (count && (pos = text.find(wordToErase)) != std::string::npos) {
        text.erase(pos, count);
    }
}

template <class UnaryPredicate>
void remove_if(std::string& text, UnaryPredicate pred) noexcept
{
    static_assert(noexcept(pred(' ')),
        "saft::remove_if: UnaryPredicate must be noexcept!");
    text.erase(std::remove_if(std::begin(text), std::end(text), pred),
        std::end(text));
}
// remove duplicates from sequence containers
void unique(std::string& text) noexcept
{
    text.erase(std::unique(std::begin(text), std::end(text)),
        std::end(text));
}

[[nodiscard]] bool
contains(std::string_view haystack, std::string_view needle) noexcept
{
    return haystack.find(needle) != std::string_view::npos;
}

}; // namespace saft
