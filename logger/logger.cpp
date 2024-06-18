#include "logger.h"
#include <stdexcept>
#include "imgui.h"

void Logger::drawLogger()
/** Prints all logger messages. */
{
    if (log_panel_ != nullptr)
    {
        log_panel_->draw();
    }
}

void Logger::drawLoggerWindow()
/** Creates a simple ImGui window and prints all logger messages there. */
{
    if (log_panel_ != nullptr && p_open_)
    {
          ImGui::Begin("Logger", &p_open_);
          log_panel_->draw();
          ImGui::End();
    }
}

void Logger::addMessage(LogLevel level, char const* message)
/** Adds a message to a logger with a selected level: info, warning, debug or error. */
{
    if (log_panel_ == nullptr)
    {
        throw std::runtime_error("Logger is not initialized.");
    }
    if (level == LogLevel::Info)
    {
        log_panel_->info(message);
    }
    else if (level == LogLevel::Warning)
    {
        log_panel_->warning(message);
    }
    else if (level == LogLevel::Error)
    {
        log_panel_->error(message);
    }
    else if (level == LogLevel::Debug)
    {
        log_panel_->debug(message);
    }
    else
    {
        throw std::runtime_error("Log: this LogLevel doesn't exist.");
    }
}
