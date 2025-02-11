#ifndef A_LOGGER_H
#define A_LOGGER_H
#include <chrono>
#include <fmt/chrono.h>
#include <fmt/color.h>
#include <fmt/format.h>

namespace logger
{

namespace impl
{
const auto DebugColor = fmt::color::gray;
const auto InfoColor = fmt::color::white;
const auto WarnColor = fmt::color::yellow;
const auto ErrorColor = fmt::color::red;
const auto FatalColor = fmt::color::magenta;
using ColorType = decltype(DebugColor);

template <typename... Args>
inline auto log(const std::string &level, ColorType col, const std::string &message, Args... args)
    -> void
{
    auto now = std::chrono::system_clock::now();
    auto output = fmt::format(fg(fmt::color::gray), "{:%Y-%m-%d %H:%M:%S} ", now);
    output += fmt::format(fg(col) | fmt::emphasis::bold, "[{}] ", level);
    output += fmt::format(fg(col), message, args...);
    fmt::println(output);
}
}; // namespace impl

template <typename... Args> inline auto debug(const std::string &message, Args... args) -> void
{
    impl::log("DEBUG", impl::DebugColor, message, args...);
}

template <typename... Args> inline auto info(const std::string &message, Args... args) -> void
{
    impl::log("INFO", impl::InfoColor, message, args...);
}

template <typename... Args> inline auto warn(const std::string &message, Args... args) -> void
{
    impl::log("WARN", impl::WarnColor, message, args...);
}

template <typename... Args> inline auto error(const std::string &message, Args... args) -> void
{
    impl::log("ERROR", impl::ErrorColor, message, args...);
}

template <typename... Args> inline auto fatal(const std::string &message, Args... args) -> void
{
    impl::log("FATAL", impl::FatalColor, message, args...);
}

}; // namespace logger
#endif // A_LOGGER_H