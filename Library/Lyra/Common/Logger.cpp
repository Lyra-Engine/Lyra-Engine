#include <Lyra/Common/Logger.h>

using namespace lyra;

Console::Console(size_t capacity)
    : capacity(capacity), start(0), count(0)
{
    logs.resize(capacity);
}

void Console::clear()
{
    start = 0;
    count = 0;

    changed = true;
}

void Console::resize(size_t new_capacity)
{
    logs.resize(new_capacity);
    capacity = new_capacity;
    start    = start % capacity;
    count    = std::min(count, capacity);

    changed = true;
}

void Console::append(LogLevel level, LogView component, String&& payload)
{
    size_t current = (start + count) % capacity;

    auto& log     = logs.at(current);
    log.verbosity = level;
    log.component = component;
    log.payload   = std::move(payload);

    if (++count > capacity) {
        count = capacity;
        start = (start + 1) % capacity;
    }

    changed = true;
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
    auto logger = create_logger("core", LogLevel::warn);
    spdlog::set_default_logger(logger);
    return logger;
}
