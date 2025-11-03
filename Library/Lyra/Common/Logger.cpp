#include <Lyra/Common/Logger.h>

using namespace lyra;

Console::Console(size_t capacity)
{
    resize(capacity);
}

void Console::clear()
{
    for (auto& count : counts)
        count = 0;

    logs.clear();

    changed = true;
}

void Console::resize(size_t new_capacity)
{
    logs.resize(new_capacity);
    changed = true;
}

void Console::append(LogLevel level, LogView component, String&& payload)
{
    ConsoleLog log;
    log.verbosity = level;
    log.component = component;
    log.payload   = std::move(payload);
    logs.push_back(log);
    counts.at(level)++;
    changed = true;
}

size_t Console::count(spdlog::level_t level) const
{
    return counts.at(level);
}

auto lyra::get_stdout_sink() -> Ref<spdlog::sinks::stdout_color_sink_mt>
{
    static auto sink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
    return sink;
}

auto lyra::get_stderr_sink() -> Ref<spdlog::sinks::stderr_color_sink_mt>
{
    static auto sink = std::make_shared<spdlog::sinks::stderr_color_sink_mt>();
    return sink;
}

auto lyra::get_console_sink() -> Ref<console_sink_mt>
{
    static auto sink = std::make_shared<console_sink_mt>();
    return sink;
}

auto lyra::create_logger(const String& name, LogLevel level) -> Logger
{
    auto logger = std::make_shared<spdlog::logger>(name.c_str(),
        spdlog::sinks_init_list{
            get_stdout_sink(),
            get_console_sink(),
        });

    // pattern includes:
    // - timestamp (HH:MM:SS.mmm)
    // - thread ID
    // - log level (colored)
    // - logger name
    // - source file and line number
    // - the actual log message
    logger->set_pattern("[%H:%M:%S] [thread %t] [%^%l%$] [%n] %v");

    logger->set_level(level);
    spdlog::register_logger(logger);
    return logger;
}

auto lyra::create_default_logger() -> Logger
{
    auto logger = create_logger("Core", LogLevel::trace);
    spdlog::set_default_logger(logger);
    return logger;
}
