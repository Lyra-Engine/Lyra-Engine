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
}

void Console::resize(size_t new_capacity)
{
    logs.resize(new_capacity);
    capacity = new_capacity;
    start    = start % capacity;
    count    = std::min(count, capacity);
}

void Console::append(LogLevel level, const String& message)
{
    size_t current = (start + count) % capacity;

    logs.at(current).level   = level;
    logs.at(current).message = message;

    if (++count > capacity) {
        count = capacity;
        start = (start + 1) % capacity;
    }
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
