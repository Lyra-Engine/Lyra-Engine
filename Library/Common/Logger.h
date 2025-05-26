#ifndef LYRA_LIBRARY_COMMON_LOGGER_H
#define LYRA_LIBRARY_COMMON_LOGGER_H

// library headers
#include <spdlog/spdlog.h>
#include <spdlog/sinks/stdout_color_sinks.h>

// local headers
#include <Common/String.h>
#include <Common/Pointer.h>

namespace lyra
{
    using Logger = Ref<spdlog::logger>;

    using LogLevel = spdlog::level;

    inline Logger init_stdout_logger(const CString& name, LogLevel verbosity)
    {
        auto logger = spdlog::stdout_color_mt(name);
        logger->set_level(verbosity);
        logger->set_pattern("[%H:%M:%S %z][%^%L%$][thread %t] %v");
        return logger;
    }

    inline Logger init_stderr_logger(const CString& name, LogLevel verbosity)
    {
        auto logger = spdlog::stderr_color_mt(name);
        logger->set_level(verbosity);
        logger->set_pattern("[%H:%M:%S %z][%^%L%$][thread %t] %v");
        return logger;
    }

} // end of namespace lyra

#endif // LYRA_LIBRARY_COMMON_LOGGER_H
