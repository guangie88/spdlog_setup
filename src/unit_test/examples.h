/**
 * Unit tests implementation.
 * @author Chen Weiguang
 * @version 0.3.2
 */

#pragma once

#include "conf.h"

#include <cstddef>
#include <cstdio>
#include <fstream>
#include <iterator>
#include <sstream>
#include <string>

namespace examples {
#ifdef _WIN32
static constexpr auto FULL_CONF = R"x(
    global_pattern = "[%Y-%m-%dT%T%z] [%L] <%n>: %v"

    [[sink]]
    name = "console_st"
    type = "stdout_sink_st"

    [[sink]]
    name = "console_mt"
    type = "stdout_sink_mt"

    [[sink]]
    name = "color_console_st"
    type = "color_stdout_sink_st"

    [[sink]]
    name = "color_console_mt"
    type = "color_stdout_sink_mt"

    [[sink]]
    name = "file_out"
    type = "basic_file_sink_st"
    filename = "log/spdlog_setup.log"
    level = "info"
    create_parent_dir = true

    [[sink]]
    name = "file_err"
    type = "basic_file_sink_mt"
    filename = "log/spdlog_setup_err.log"
    truncate = true
    level = "err"

    [[sink]]
    name = "rotate_out"
    type = "rotating_file_sink_st"
    base_filename = "log/rotate_spdlog_setup.log"
    max_size = "1M"
    max_files = 10
    level = "info"

    [[sink]]
    name = "rotate_err"
    type = "rotating_file_sink_mt"
    base_filename = "log/rotate_spdlog_setup_err.log"
    max_size = "1M"
    max_files = 10
    level = "err"

    [[sink]]
    name = "daily_out"
    type = "daily_file_sink_st"
    base_filename = "log/daily_spdlog_setup.log"
    rotation_hour = 17
    rotation_minute = 30
    level = "info"

    [[sink]]
    name = "daily_err"
    type = "daily_file_sink_mt"
    base_filename = "log/daily_spdlog_setup_err.log"
    rotation_hour = 17
    rotation_minute = 30
    level = "err"

    [[sink]]
    name = "null_sink_st"
    type = "null_sink_st"

    [[sink]]
    name = "null_sink_mt"
    type = "null_sink_mt"

    [[sink]]
    name = "msvc_sink_st"
    type = "msvc_sink_st"

    [[sink]]
    name = "msvc_sink_mt"
    type = "msvc_sink_mt"

    [[pattern]]
    name = "succient"
    value = "%c-%L: %v"

    [[logger]]
    name = "root"
    sinks = [
        "console_st", "console_mt",
        "color_console_st", "color_console_mt",
        "daily_out", "daily_err",
        "file_out", "file_err",
        "rotate_out", "rotate_err",
        "null_sink_st", "null_sink_mt",
        "msvc_sink_st", "msvc_sink_mt"]
    level = "trace"

    [[logger]]
    name = "console"
    sinks = ["console_st", "console_mt"]
    pattern = "succient"

    [global_thread_pool]
    queue_size = 8192
    num_threads = 1

    [[thread_pool]]
    name = "tp"
    queue_size = 4096
    num_threads = 2

    [[logger]]
    type = "async"
    name = "global_async"
    sinks = ["console_mt"]
    pattern = "succient"

    [[logger]]
    type = "async"
    name = "local_async"
    sinks = ["console_mt"]
    pattern = "succient"
    thread_pool = "tp"
    overflow_policy = "overrun_oldest"  # block (default) | overrun_oldest
)x";
#else
static constexpr auto FULL_CONF = R"x(
    global_pattern = "[%Y-%m-%dT%T%z] [%L] <%n>: %v"

    [[sink]]
    name = "console_st"
    type = "stdout_sink_st"

    [[sink]]
    name = "console_mt"
    type = "stdout_sink_mt"

    [[sink]]
    name = "color_console_st"
    type = "color_stdout_sink_st"

    [[sink]]
    name = "color_console_mt"
    type = "color_stdout_sink_mt"

    [[sink]]
    name = "file_out"
    type = "basic_file_sink_st"
    filename = "log/spdlog_setup.log"
    level = "info"
    create_parent_dir = true

    [[sink]]
    name = "file_err"
    type = "basic_file_sink_mt"
    filename = "log/spdlog_setup_err.log"
    truncate = true
    level = "err"

    [[sink]]
    name = "rotate_out"
    type = "rotating_file_sink_st"
    base_filename = "log/rotate_spdlog_setup.log"
    max_size = "1M"
    max_files = 10
    level = "info"

    [[sink]]
    name = "rotate_err"
    type = "rotating_file_sink_mt"
    base_filename = "log/rotate_spdlog_setup_err.log"
    max_size = "1M"
    max_files = 10
    level = "err"

    [[sink]]
    name = "daily_out"
    type = "daily_file_sink_st"
    base_filename = "log/daily_spdlog_setup.log"
    rotation_hour = 17
    rotation_minute = 30
    level = "info"

    [[sink]]
    name = "daily_err"
    type = "daily_file_sink_mt"
    base_filename = "log/daily_spdlog_setup_err.log"
    rotation_hour = 17
    rotation_minute = 30
    level = "err"

    [[sink]]
    name = "null_sink_st"
    type = "null_sink_st"

    [[sink]]
    name = "null_sink_mt"
    type = "null_sink_mt"

    [[sink]]
    name = "syslog_st"
    type = "syslog_sink_st"

    [[sink]]
    name = "syslog_mt"
    type = "syslog_sink_mt"

    [[pattern]]
    name = "succient"
    value = "%c-%L: %v"

    [[logger]]
    name = "root"
    sinks = [
        "console_st", "console_mt",
        "color_console_st", "color_console_mt",
        "daily_out", "daily_err",
        "file_out", "file_err",
        "rotate_out", "rotate_err",
        "null_sink_st", "null_sink_mt",
        "syslog_st", "syslog_mt"]
    level = "trace"

    [[logger]]
    name = "console"
    sinks = ["console_st", "console_mt"]
    pattern = "succient"

    [global_thread_pool]
    queue_size = 8192
    num_threads = 1

    [[thread_pool]]
    name = "tp"
    queue_size = 4096
    num_threads = 2

    [[logger]]
    type = "async"
    name = "global_async"
    sinks = ["console_mt"]
    pattern = "succient"

    [[logger]]
    type = "async"
    name = "local_async"
    sinks = ["console_mt"]
    pattern = "succient"
    thread_pool = "tp"
    overflow_policy = "overrun_oldest"  # block (default) | overrun_oldest
)x";
#endif

static constexpr auto PRE_CONF = R"x(
    [[sink]]
    name = "console"
    type = "stdout_sink_mt"

    [[sink]]
    name = "rotate_out"
    type = "rotating_file_sink_mt"
    base_filename = "log/{index}-info/simple-{path}.log"
    max_size = "1M"
    max_files = 10
    level = "info"
    create_parent_dir = true

    [[sink]]
    name = "simple_err"
    type = "basic_file_sink_mt"
    filename = "log/{index}-err/simple-{path}.log"
    truncate = false
    level = "err"
    create_parent_dir = true

    [[logger]]
    name = "root"
    sinks = ["console", "rotate_out", "simple_err"]
    level = "trace"
)x";

static constexpr auto OVERRIDE_CONF = R"x(
    [[sink]]
    name = "console_st"
    level = "err"

    [[sink]]
    name = "console_mt"
    level = "critical"

    [[sink]]
    name = "console_another"
    type = "stdout_sink_st"
    level = "trace"

    [[pattern]]
    name = "super-succient"
    value = "%L: %v"

    [[logger]]
    name = "console"
    sinks = ["console_st", "console_mt", "console_another"]
    pattern = "super-succient"
    level = "trace"
)x";

static constexpr auto OVERRIDE_PRE_CONF = R"x(
    [[sink]]
    name = "rotate_out"
    base_filename = "log/{index}-info/simple-{path}-{hash}.log"
    max_size = "1K"
    max_files = 3
    level = "trace"
    create_parent_dir = true

    [[sink]]
    name = "simple_err"
    filename = "log/{index}-err/simple-{path}-{hash}.log"
    level = "warn"
    create_parent_dir = true
)x";

static constexpr auto SIMPLE_CONSOLE_LOGGER_CONF = R"x(
    [[pattern]]
    name = "easy"
    value = "%L: %v"

    [[logger]]
    name = "not-console"
    pattern = "easy"
    level = "info"

    [[logger]]
    name = "console"
    pattern = "easy"
    level = "trace"
)x";

class tmp_file {
  public:
    tmp_file(const std::string &content) : file_path(std::tmpnam(nullptr)) {
        std::ofstream ostr(file_path);
        ostr << content;
    }

    tmp_file() : tmp_file("") {}

    ~tmp_file() { std::remove(file_path.c_str()); }

    auto get_file_path() const -> const std::string & { return file_path; }

  private:
    std::string file_path;
};

inline auto get_full_conf_tmp_file() -> tmp_file { return tmp_file(FULL_CONF); }

inline auto get_pre_conf_tmp_file() -> tmp_file { return tmp_file(PRE_CONF); }

inline auto get_override_conf_tmp_file() -> tmp_file {
    return tmp_file(OVERRIDE_CONF);
}

inline auto get_override_pre_conf_tmp_file() -> tmp_file {
    return tmp_file(OVERRIDE_PRE_CONF);
}

inline auto get_simple_console_logger_conf_tmp_file() -> tmp_file {
    return tmp_file(SIMPLE_CONSOLE_LOGGER_CONF);
}

template <class Iterable> auto dist(const Iterable &iterable) -> ptrdiff_t {
    return std::distance(std::begin(iterable), std::end(iterable));
}

template <class Iterable>
auto get_index(const Iterable &iterable, const size_t index) ->
    typename Iterable::const_iterator::reference {

    return *std::next(std::begin(iterable), index);
}
} // namespace examples
