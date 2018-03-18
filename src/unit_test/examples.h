#pragma once

#include <cstdio>
#include <fstream>
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
    type = "simple_file_sink_st"
    filename = "log/spdlog_setup.log"
    level = "info"
    create_parent_dir = true

    [[sink]]
    name = "file_err"
    type = "simple_file_sink_mt"
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
        "null_sink_st", "null_sink_mt"]
    level = "trace"

    [[logger]]
    name = "console"
    sinks = ["console_st", "console_mt"]
    pattern = "succient"
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
    type = "simple_file_sink_st"
    filename = "log/spdlog_setup.log"
    level = "info"
    create_parent_dir = true

    [[sink]]
    name = "file_err"
    type = "simple_file_sink_mt"
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
    name = "syslog"
    type = "syslog_sink"

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
        "syslog"]
    level = "trace"

    [[logger]]
    name = "console"
    sinks = ["console_st", "console_mt"]
    pattern = "succient"
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
    type = "simple_file_sink_mt"
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

class tmp_file {
  public:
    tmp_file(const std::string &content) : file_path(std::tmpnam(nullptr)) {
        std::ofstream ostr(file_path);
        ostr << content;
    }

    ~tmp_file() { std::remove(file_path.c_str()); }

    auto get_file_path() const -> const std::string & { return file_path; }

  private:
    std::string file_path;
};

auto get_full_conf_tmp_file() -> tmp_file { return tmp_file(FULL_CONF); }

auto get_pre_conf_tmp_file() -> tmp_file { return tmp_file(PRE_CONF); }

auto get_override_conf_tmp_file() -> tmp_file {
    return tmp_file(OVERRIDE_CONF);
}
} // namespace examples