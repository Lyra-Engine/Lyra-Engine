#pragma once

#ifndef LYRA_LIBRARY_COMMON_LOGGER_H
#define LYRA_LIBRARY_COMMON_LOGGER_H

// vendor headers
#include <spdlog/spdlog.h>
#include <spdlog/sinks/stdout_color_sinks.h>

// library headers
#include <Lyra/Common/Macros.h>
#include <Lyra/Common/String.h>
#include <Lyra/Common/Pointer.h>

namespace lyra
{
    using Logger   = Ref<spdlog::logger>;
    using LogLevel = spdlog::level::level_enum;

    inline Logger init_stdout_logger(const String& name, LogLevel verbosity)
    {
        auto logger = spdlog::stdout_color_mt(name);
        logger->set_level(verbosity);
        logger->set_pattern("[%H:%M:%S %z][%^%L%$][thread %t] %v");
        return logger;
    }

    inline Logger init_stderr_logger(const String& name, LogLevel verbosity)
    {
        auto logger = spdlog::stderr_color_mt(name);
        logger->set_level(verbosity);
        logger->set_pattern("[%H:%M:%S %z][%^%L%$][thread %t] %v");
        return logger;
    }

} // end of namespace lyra

namespace lyra::engine
{
    FORCE_INLINE Logger logger()
    {
        static Logger logger = init_stdout_logger("engine", LogLevel::warn);
        return logger;
    }
} // namespace lyra::engine

#endif // LYRA_LIBRARY_COMMON_LOGGER_H
