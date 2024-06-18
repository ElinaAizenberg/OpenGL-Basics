
#ifndef PROJECT_1_LOGGER_H
#define PROJECT_1_LOGGER_H

#include <cstddef>

#include "imguial_term.h"

enum class LogLevel
{
    Debug,
    Info,
    Warning,
    Error
};


class Logger
{
 public:
    static void init()
    {
        log_panel_ = new ImGuiAl::Log(static_cast<void*>(log_buffer_), kBufferSize);
    }

    static void openLogger() { p_open_ = true; }
    static void drawLogger();
    static void drawLoggerWindow();
    static void addMessage(LogLevel level, char const* message);

private:
    static bool p_open_;
    static constexpr size_t kBufferSize = 600000;
    static char log_buffer_[kBufferSize];
    static ImGuiAl::Log* log_panel_;
};

#endif  // PROJECT_1_LOGGER_H
