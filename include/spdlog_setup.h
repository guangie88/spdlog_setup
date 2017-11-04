/**
 * Implementation of spdlog_setup in entirety.
 * @author Chen Weiguang
 * @version 0.1.0
 */

#pragma once

#ifndef FMT_HEADER_ONLY
#define FMT_HEADER_ONLY
#endif

#include "cpptoml.h"
#include "fmt/format.h"
#include "rustfp/let.h"
#include "rustfp/result.h"
#include "rustfp/unit.h"
#include "spdlog/sinks/file_sinks.h"
#include "spdlog/sinks/null_sink.h"
#include "spdlog/sinks/sink.h"
#include "spdlog/sinks/stdout_sinks.h"
#include "spdlog/sinks/syslog_sink.h"

#ifdef _WIN32
#include "spdlog/sinks/wincolor_sink.h"
#else
#include "spdlog/sinks/ansicolor_sink.h"
#endif

#include "spdlog/spdlog.h"

#include <cstdint>
#include <exception>
#include <fstream>
#include <memory>
#include <regex>
#include <sstream>
#include <string>
#include <type_traits>
#include <unordered_map>
#include <utility>
#include <vector>

#ifdef _WIN32
#include <Windows.h>
#include <io.h>

#ifndef F_OK
#define F_OK 0
#endif
#else
#include <sys/stat.h>
#include <unistd.h>
#endif

namespace spdlog_setup {
// declaration section

/**
 * Describes the sink types in enumeration form.
 */
enum class SinkType {
    /** Represents stdout_sink_st */
    StdoutSinkSt,

    /** Represents stdout_sink_mt */
    StdoutSinkMt,

    /**
     * Represents either wincolor_stdout_sink_st (Windows) or
     * ansicolor_stdout_sink_st (Linux)
     */
    ColorStdoutSinkSt,

    /**
     * Represents either wincolor_stdout_sink_mt (Windows) or
     * ansicolor_stdout_sink_mt (Linux)
     */
    ColorStdoutSinkMt,

    /** Represents simple_file_sink_st */
    SimpleFileSinkSt,

    /** Represents simple_file_sink_mt */
    SimpleFileSinkMt,

    /** Represents rotating_file_sink_st */
    RotatingFileSinkSt,

    /** Represents rotating_file_sink_mt */
    RotatingFileSinkMt,

    /** Represents daily_file_sink_st */
    DailyFileSinkSt,

    /** Represents daily_file_sink_mt */
    DailyFileSinkMt,

    /** Represents null_sink_st */
    NullSinkSt,

    /** Represents null_sink_mt */
    NullSinkMt,

    /** Represents syslog_sink */
    SyslogSink,
};

/**
 * Performs spdlog configuration setup from file, with tag values to be
 * replaced into various primitive values.
 * @param pre_toml_path Path to the pre-TOML configuration file path.
 * @return true if setup is successful, otherwise false.
 */
template <class... Ps>
auto from_file_with_tag_replacement(
    const std::string &pre_toml_path, Ps &&... ps)
    -> rustfp::Result<rustfp::unit_t, std::string>;

/**
 * Performs spdlog configuration setup from file.
 * @param toml_path Path to the TOML configuration file path.
 * @return true if setup is successful, otherwise false.
 */
auto from_file(const std::string &toml_path)
    -> rustfp::Result<rustfp::unit_t, std::string>;

// implementation section

namespace details {
namespace names {
// table names
static constexpr auto LOGGER_TABLE = "logger";
static constexpr auto PATTERN_TABLE = "pattern";
static constexpr auto SINK_TABLE = "sink";

// field names
static constexpr auto BASE_FILENAME = "base_filename";
static constexpr auto CREATE_PARENT_DIR = "create_parent_dir";
static constexpr auto FILENAME = "filename";
static constexpr auto GLOBAL_PATTERN = "global_pattern";
static constexpr auto IDENT = "ident";
static constexpr auto LEVEL = "level";
static constexpr auto MAX_FILES = "max_files";
static constexpr auto MAX_SIZE = "max_size";
static constexpr auto NAME = "name";
static constexpr auto PATTERN = "pattern";
static constexpr auto ROTATION_HOUR = "rotation_hour";
static constexpr auto ROTATION_MINUTE = "rotation_minute";
static constexpr auto SINKS = "sinks";
static constexpr auto SYSLOG_FACILITY = "syslog_facility";
static constexpr auto SYSLOG_OPTION = "syslog_option";
static constexpr auto TRUNCATE = "truncate";
static constexpr auto TYPE = "type";
static constexpr auto VALUE = "value";
} // namespace names

inline auto get_parent_path(const std::string &file_path) -> std::string {
    // string
    using std::string;

#ifdef _WIN32
    static constexpr auto DIR_SLASHES = "\\/";
#else
    static constexpr auto DIR_SLASHES = '/';
#endif

    const auto last_slash_index = file_path.find_last_of(DIR_SLASHES);

    if (last_slash_index == string::npos) {
        return "";
    }

    return file_path.substr(0, last_slash_index);
}

inline void native_create_dir(const std::string &dir_path) {
#ifdef _WIN32
    CreateDirectoryA(dir_path.c_str(), nullptr);
#else
    mkdir(dir_path.c_str(), 0775);
#endif
}

inline auto file_exists(const std::string &file_path) -> bool {
    static constexpr auto FILE_NOT_FOUND = -1;

#ifdef _WIN32
    return _access(file_path.c_str(), F_OK) != FILE_NOT_FOUND;
#else
    return access(file_path.c_str(), F_OK) != FILE_NOT_FOUND;
#endif
}

inline void create_dirs_impl(const std::string &dir_path) {
#ifdef _WIN32
    // check for both empty and drive letter
    if (dir_path.empty() || (dir_path.length() == 2 && dir_path[1] == ':')) {
        return;
    }
#else
    if (dir_path.empty()) {
        return;
    }
#endif

    if (!file_exists(dir_path)) {
        create_dirs_impl(get_parent_path(dir_path));
        native_create_dir(dir_path);
    }
}

inline void create_directories(const std::string &dir_path) {
    create_dirs_impl(dir_path);
}

template <class T, class Fn>
auto if_value_from_table(
    const std::shared_ptr<cpptoml::table> &table,
    const char field[],
    Fn &&if_value_fn) -> std::result_of_t<Fn(const T &)> {

    // rustfp
    using rustfp::Ok;
    using rustfp::Unit;

    const auto value_opt = table->get_as<T>(field);

    if (value_opt) {
        return if_value_fn(*value_opt);
    }

    return Ok(Unit);
}

template <class T>
auto value_from_table_opt(
    const std::shared_ptr<cpptoml::table> &table, const char field[])
    -> rustfp::Option<T> {

    // rustfp
    using rustfp::None;
    using rustfp::Some;

    // std
    using std::string;

    const auto value_ptr = table->get_as<string>(field);
    return value_ptr ? Some(*value_ptr) : None;
}

template <class T>
auto value_from_table_or(
    const std::shared_ptr<cpptoml::table> &table,
    const char field[],
    const T &alt_val) -> T {

    // rustfp
    using rustfp::Err;
    using rustfp::Ok;

    const auto value_opt = table->get_as<T>(field);

    if (value_opt) {
        return *value_opt;
    } else {
        return alt_val;
    }
}

template <class T>
auto value_from_table(
    const std::shared_ptr<cpptoml::table> &table,
    const char field[],
    const std::string &err_msg) -> rustfp::Result<T, std::string> {

    // rustfp
    using rustfp::Err;
    using rustfp::Ok;

    const auto value_opt = table->get_as<T>(field);

    if (!value_opt) {
        return Err(err_msg);
    }

    return Ok(*value_opt);
}

template <class T>
auto array_from_table(
    const std::shared_ptr<cpptoml::table> &table,
    const char field[],
    const std::string &err_msg) -> rustfp::Result<std::vector<T>, std::string> {

    // rustfp
    using rustfp::Err;
    using rustfp::Ok;

    const auto array_opt = table->get_array_of<T>(field);

    if (!array_opt) {
        return Err(err_msg);
    }

    return Ok(*array_opt);
}

template <class Map, class Key>
auto find_value_from_map(
    const Map &m, const Key &key, const std::string &err_msg)
    -> rustfp::Result<typename Map::mapped_type, std::string> {

    // rustfp
    using rustfp::Err;
    using rustfp::Ok;

    const auto iter = m.find(key);

    if (iter == m.cend()) {
        return Err(err_msg);
    }

    return Ok(iter->second);
}

template <class T, class Fn>
auto add_msg_on_err(
    rustfp::Result<T, std::string> &&res, Fn &&add_msg_on_err_fn)
    -> rustfp::Result<T, std::string> {

    // std
    using std::move;
    using std::string;

    return move(res).map_err([&add_msg_on_err_fn](string &&err_msg) {
        return add_msg_on_err_fn(move(err_msg));
    });
}

inline auto parse_max_size(const std::string &max_size_str)
    -> rustfp::Result<uint64_t, std::string> {

    // fmt
    using fmt::format;

    // rustfp
    using rustfp::Err;
    using rustfp::Ok;

    // std
    using std::exception;
    using std::regex;
    using std::regex_constants::icase;
    using std::regex_match;
    using std::smatch;
    using std::stoull;
    using std::string;

    try {
        static const regex RE(R"_(^\s*(\d+)\s*(T|G|M|K|)(:?B|)\s*$)_", icase);

        smatch matches;
        const auto has_match = regex_match(max_size_str, matches, RE);

        if (has_match && matches.size() == 4) {
            const uint64_t base_val = stoull(matches[1]);
            const string suffix = matches[2];

            if (suffix == "") {
                // byte
                return Ok(base_val);
            } else if (suffix == "K") {
                // kilobyte
                return Ok(base_val * 1024);
            } else if (suffix == "M") {
                // megabyte
                return Ok(base_val * 1024 * 1024);
            } else if (suffix == "G") {
                // gigabyte
                return Ok(base_val * 1024 * 1024 * 1024);
            } else if (suffix == "T") {
                // terabyte
                return Ok(base_val * 1024 * 1024 * 1024 * 1024);
            } else {
                return Err(format(
                    "Unexpected suffix '{}' for max size parsing", suffix));
            }
        } else {
            return Err(format(
                "Invalid string '{}' for max size parsing", max_size_str));
        }
    } catch (const exception &e) {
        return Err(format(
            "Unexpected exception for max size parsing on string '{}': {}",
            max_size_str,
            e.what()));
    }
}

inline auto sink_type_from_str(const std::string &type)
    -> rustfp::Result<SinkType, std::string> {

    // fmt
    using fmt::format;

    // rustup
    using rustfp::Err;
    using rustfp::Ok;

    // std
    using std::string;
    using std::unordered_map;

    static const unordered_map<string, SinkType> MAPPING{
        {"stdout_sink_st", SinkType::StdoutSinkSt},
        {"stdout_sink_mt", SinkType::StdoutSinkMt},
        {"color_stdout_sink_st", SinkType::ColorStdoutSinkSt},
        {"color_stdout_sink_mt", SinkType::ColorStdoutSinkMt},
        {"simple_file_sink_st", SinkType::SimpleFileSinkSt},
        {"simple_file_sink_mt", SinkType::SimpleFileSinkMt},
        {"rotating_file_sink_st", SinkType::RotatingFileSinkSt},
        {"rotating_file_sink_mt", SinkType::RotatingFileSinkMt},
        {"daily_file_sink_st", SinkType::DailyFileSinkSt},
        {"daily_file_sink_mt", SinkType::DailyFileSinkMt},
        {"null_sink_st", SinkType::NullSinkSt},
        {"null_sink_mt", SinkType::NullSinkMt},

#ifdef SPDLOG_ENABLE_SYSLOG
        {"syslog_sink", SinkType::SyslogSink},
#endif
    };

    return find_value_from_map(
        MAPPING, type, format("Invalid sink type '{}' found", type));
}

inline auto create_parent_dir_if_present(
    const std::shared_ptr<cpptoml::table> &sink_table,
    const std::string &filename)
    -> rustfp::Result<rustfp::unit_t, std::string> {

    using names::CREATE_PARENT_DIR;

    // rustfp
    using rustfp::Ok;
    using rustfp::Result;
    using rustfp::Unit;
    using rustfp::unit_t;

    // std
    using std::string;

    return if_value_from_table<bool>(
        sink_table,
        CREATE_PARENT_DIR,
        [&filename](const bool flag) -> Result<unit_t, string> {
            if (flag) {
                create_directories(get_parent_path(filename));
            }

            return Ok(Unit);
        });
}

inline auto level_from_str(const std::string &level)
    -> rustfp::Result<spdlog::level::level_enum, std::string> {

    // fmt
    using fmt::format;

    // rustfp
    using rustfp::Err;
    using rustfp::Ok;
    using rustfp::Result;

    // spdlog
    namespace lv = spdlog::level;

    // std
    using std::move;
    using std::string;

    using level_result_t = Result<lv::level_enum, string>;

    auto level_res = [&level]() -> level_result_t {
        if (level == "trace") {
            return Ok(lv::trace);
        } else if (level == "debug") {
            return Ok(lv::debug);
        } else if (level == "info") {
            return Ok(lv::info);
        } else if (level == "warn") {
            return Ok(lv::warn);
        } else if (level == "err") {
            return Ok(lv::err);
        } else if (level == "critical") {
            return Ok(lv::critical);
        } else if (level == "off") {
            return Ok(lv::off);
        } else {
            return level_result_t(
                Err(format("Invalid level string '{}' provided", level)));
        }
    }();

    return move(level_res);
}

inline auto set_sink_level_if_present(
    const std::shared_ptr<cpptoml::table> &sink_table,
    const std::shared_ptr<spdlog::sinks::sink> &sink)
    -> rustfp::Result<rustfp::unit_t, std::string> {

    using names::LEVEL;

    // rustfp
    using rustfp::Ok;
    using rustfp::Result;
    using rustfp::Unit;
    using rustfp::unit_t;

    // std
    using std::string;

    return if_value_from_table<string>(
        sink_table,
        LEVEL,
        [&sink](const string &level) -> Result<unit_t, string> {
            auto level_enum_res = level_from_str(level);
            RUSTFP_LET(level_enum, level_enum_res);

            sink->set_level(level_enum);
            return Ok(Unit);
        });
}

template <class SimpleFileSink>
auto simple_file_sink_from_table(
    const std::shared_ptr<cpptoml::table> &sink_table)
    -> rustfp::Result<std::shared_ptr<spdlog::sinks::sink>, std::string> {

    using names::FILENAME;
    using names::LEVEL;
    using names::TRUNCATE;

    // fmt
    using fmt::format;

    // rustfp
    using rustfp::Ok;

    // std
    using std::make_shared;
    using std::move;
    using std::shared_ptr;
    using std::string;

    static constexpr auto DEFAULT_TRUNCATE = false;

    auto filename_res = value_from_table<string>(
        sink_table,
        FILENAME,
        format(
            "Missing '{}' field of string value for simple_file_sink",
            FILENAME));

    RUSTFP_LET(filename, filename_res);

    // must create the directory before creating the sink
    auto create_parent_dir_res =
        create_parent_dir_if_present(sink_table, filename);

    RUSTFP_RET_IF_ERR(create_parent_dir_res);

    const auto truncate =
        value_from_table_or<bool>(sink_table, TRUNCATE, DEFAULT_TRUNCATE);

    return Ok(shared_ptr<spdlog::sinks::sink>(
        make_shared<SimpleFileSink>(filename, truncate)));
}

template <class RotatingFileSink>
auto rotating_file_sink_from_table(
    const std::shared_ptr<cpptoml::table> &sink_table)
    -> rustfp::Result<std::shared_ptr<spdlog::sinks::sink>, std::string> {

    using names::BASE_FILENAME;
    using names::MAX_FILES;
    using names::MAX_SIZE;

    // fmt
    using fmt::format;

    // rustfp
    using rustfp::Ok;

    // std
    using std::make_shared;
    using std::shared_ptr;
    using std::string;

    auto base_filename_res = value_from_table<string>(
        sink_table,
        BASE_FILENAME,
        format(
            "Missing '{}' field of string value for rotating_file_sink",
            BASE_FILENAME));

    RUSTFP_LET(base_filename, base_filename_res);

    // must create the directory before creating the sink
    auto create_parent_dir_res =
        create_parent_dir_if_present(sink_table, base_filename);

    RUSTFP_RET_IF_ERR(create_parent_dir_res);

    auto max_filesize_str_res = value_from_table<string>(
        sink_table,
        MAX_SIZE,
        format(
            "Missing '{}' field of string value for rotating_file_sink",
            MAX_SIZE));

    RUSTFP_LET(max_filesize_str, max_filesize_str_res);

    auto parse_max_size_res = parse_max_size(max_filesize_str);
    RUSTFP_LET(max_filesize, parse_max_size_res);

    auto max_files_res = value_from_table<uint64_t>(
        sink_table,
        MAX_FILES,
        format(
            "Missing '{}' field of u64 value for rotating_file_sink",
            MAX_FILES));

    RUSTFP_LET(max_files, max_files_res);

    return Ok(shared_ptr<spdlog::sinks::sink>(
        make_shared<RotatingFileSink>(base_filename, max_filesize, max_files)));
}

template <class DailyFileSink>
auto daily_file_sink_from_table(
    const std::shared_ptr<cpptoml::table> &sink_table)
    -> rustfp::Result<std::shared_ptr<spdlog::sinks::sink>, std::string> {

    using names::BASE_FILENAME;
    using names::ROTATION_HOUR;
    using names::ROTATION_MINUTE;

    // fmt
    using fmt::format;

    // rustfp
    using rustfp::Ok;

    // std
    using std::make_shared;
    using std::shared_ptr;
    using std::string;

    auto base_filename_res = value_from_table<string>(
        sink_table,
        BASE_FILENAME,
        format(
            "Missing '{}' field of string value for daily_file_sink",
            BASE_FILENAME));

    RUSTFP_LET(base_filename, base_filename_res);

    // must create the directory before creating the sink
    auto create_parent_dir_res =
        create_parent_dir_if_present(sink_table, base_filename);

    RUSTFP_RET_IF_ERR(create_parent_dir_res);

    auto rotation_hour_res = value_from_table<int32_t>(
        sink_table,
        ROTATION_HOUR,
        format(
            "Missing '{}' field of string value for daily_file_sink",
            ROTATION_HOUR));

    RUSTFP_LET(rotation_hour, rotation_hour_res);

    auto rotation_minute_res = value_from_table<int32_t>(
        sink_table,
        ROTATION_MINUTE,
        format(
            "Missing '{}' field of string value for daily_file_sink",
            ROTATION_MINUTE));

    RUSTFP_LET(rotation_minute, rotation_minute_res);

    return Ok(shared_ptr<spdlog::sinks::sink>(make_shared<DailyFileSink>(
        base_filename, rotation_hour, rotation_minute)));
}

#ifdef SPDLOG_ENABLE_SYSLOG

inline auto
syslog_sink_from_table(const std::shared_ptr<cpptoml::table> &sink_table)
    -> rustfp::Result<std::shared_ptr<spdlog::sinks::sink>, std::string> {

    using names::IDENT;
    using names::SYSLOG_FACILITY;
    using names::SYSLOG_OPTION;

    // spdlog
    using spdlog::sinks::syslog_sink;

    // fmt
    using fmt::format;

    // rustfp
    using rustfp::Ok;

    // std
    using std::make_shared;
    using std::shared_ptr;
    using std::string;

    // all are optional fields
    static constexpr auto DEFAULT_IDENT = "";
    static constexpr auto DEFAULT_SYSLOG_OPTION = 0;
    static constexpr auto DEFAULT_SYSLOG_FACILITY = LOG_USER;

    const auto ident =
        value_from_table_or<string>(sink_table, IDENT, DEFAULT_IDENT);

    const auto syslog_option = value_from_table_or<int32_t>(
        sink_table, SYSLOG_OPTION, DEFAULT_SYSLOG_OPTION);

    const auto syslog_facility = value_from_table_or<int32_t>(
        sink_table, SYSLOG_FACILITY, DEFAULT_SYSLOG_FACILITY);

    return Ok(shared_ptr<spdlog::sinks::sink>(
        make_shared<syslog_sink>(ident, syslog_option, syslog_facility)));
}

#endif

inline auto sink_from_table(const std::shared_ptr<cpptoml::table> &sink_table)
    -> rustfp::Result<std::shared_ptr<spdlog::sinks::sink>, std::string> {

    using names::TYPE;

    // fmt
    using fmt::format;

    // rustfp
    using rustfp::Err;
    using rustfp::Ok;
    using rustfp::Result;

    // spdlog
    using spdlog::sinks::daily_file_sink_mt;
    using spdlog::sinks::daily_file_sink_st;
    using spdlog::sinks::null_sink_mt;
    using spdlog::sinks::null_sink_st;
    using spdlog::sinks::rotating_file_sink_mt;
    using spdlog::sinks::rotating_file_sink_st;
    using spdlog::sinks::simple_file_sink_mt;
    using spdlog::sinks::simple_file_sink_st;
    using spdlog::sinks::sink;
    using spdlog::sinks::stdout_sink_mt;
    using spdlog::sinks::stdout_sink_st;

#ifdef _WIN32
    using color_stdout_sink_st = spdlog::sinks::wincolor_stdout_sink_st;
    using color_stdout_sink_mt = spdlog::sinks::wincolor_stdout_sink_mt;
#else
    using color_stdout_sink_st = spdlog::sinks::ansicolor_stdout_sink_st;
    using color_stdout_sink_mt = spdlog::sinks::ansicolor_stdout_sink_mt;
#endif

    // std
    using std::make_shared;
    using std::move;
    using std::shared_ptr;
    using std::string;

    using sink_result_t = Result<shared_ptr<sink>, string>;

    auto type_res = value_from_table<string>(
        sink_table, TYPE, format("Sink missing '{}' field", TYPE));

    RUSTFP_LET(type, type_res);

    return sink_type_from_str(type)
        .and_then(
            [&sink_table, &type](const SinkType sink_type) -> sink_result_t {
                // find the correct sink type to create
                switch (sink_type) {
                case SinkType::StdoutSinkSt:
                    return Ok(shared_ptr<sink>(make_shared<stdout_sink_st>()));

                case SinkType::StdoutSinkMt:
                    return Ok(shared_ptr<sink>(make_shared<stdout_sink_mt>()));

                case SinkType::ColorStdoutSinkSt:
                    return Ok(
                        shared_ptr<sink>(make_shared<color_stdout_sink_st>()));

                case SinkType::ColorStdoutSinkMt:
                    return Ok(
                        shared_ptr<sink>(make_shared<color_stdout_sink_mt>()));

                case SinkType::SimpleFileSinkSt:
                    return simple_file_sink_from_table<simple_file_sink_st>(
                        sink_table);

                case SinkType::SimpleFileSinkMt:
                    return simple_file_sink_from_table<simple_file_sink_mt>(
                        sink_table);

                case SinkType::RotatingFileSinkSt:
                    return rotating_file_sink_from_table<rotating_file_sink_st>(
                        sink_table);

                case SinkType::RotatingFileSinkMt:
                    return rotating_file_sink_from_table<rotating_file_sink_mt>(
                        sink_table);

                case SinkType::DailyFileSinkSt:
                    return daily_file_sink_from_table<daily_file_sink_st>(
                        sink_table);

                case SinkType::DailyFileSinkMt:
                    return daily_file_sink_from_table<daily_file_sink_mt>(
                        sink_table);

                case SinkType::NullSinkSt:
                    return Ok(shared_ptr<sink>(make_shared<null_sink_st>()));

                case SinkType::NullSinkMt:
                    return Ok(shared_ptr<sink>(make_shared<null_sink_mt>()));

#ifdef SPDLOG_ENABLE_SYSLOG
                case SinkType::SyslogSink:
                    return syslog_sink_from_table(sink_table);
#endif

                default:
                    return Err(format(
                        "Unexpected sink error with sink type '{}'", type));
                }
            })
        .and_then([&sink_table](shared_ptr<sink> &&sink) -> sink_result_t {
            // set optional parts and return back the same sink
            auto set_sink_level_res =
                set_sink_level_if_present(sink_table, sink);

            RUSTFP_RET_IF_ERR(set_sink_level_res);
            return Ok(move(sink));
        });
}

inline auto set_logger_level_if_present(
    const std::shared_ptr<cpptoml::table> &logger_table,
    const std::shared_ptr<spdlog::logger> &logger)
    -> rustfp::Result<rustfp::unit_t, std::string> {

    using names::LEVEL;

    // rustfp
    using rustfp::Ok;
    using rustfp::Result;
    using rustfp::Unit;
    using rustfp::unit_t;

    // std
    using std::string;

    using unit_result_t = Result<unit_t, string>;

    return if_value_from_table<string>(
        logger_table, LEVEL, [&logger](const string &level) -> unit_result_t {
            auto level_enum_res = level_from_str(level);
            RUSTFP_LET(level_enum, level_enum_res);

            logger->set_level(level_enum);
            return Ok(Unit);
        });
}

inline auto setup_sinks_impl(const std::shared_ptr<cpptoml::table> &config)
    -> rustfp::Result<
        std::unordered_map<std::string, std::shared_ptr<spdlog::sinks::sink>>,
        std::string> {

    using names::NAME;
    using names::SINK_TABLE;

    // fmt
    using fmt::format;

    // rustfp
    using rustfp::Err;
    using rustfp::Ok;

    // std
    using std::move;
    using std::shared_ptr;
    using std::string;
    using std::unordered_map;

    const auto sinks = config->get_table_array(SINK_TABLE);

    if (!sinks) {
        return Err(string("No sinks configured for set-up"));
    }

    unordered_map<string, shared_ptr<spdlog::sinks::sink>> sinks_map;

    for (const auto &sink_table : *sinks) {
        auto name_res = value_from_table<string>(
            sink_table,
            NAME,
            format("One of the sinks does not have a '{}' field", NAME));

        RUSTFP_LET(name, name_res);

        auto sink_res = add_msg_on_err(
            sink_from_table(sink_table), [&name](const string &err_msg) {
                return format("Sink '{}' error:\n > {}", name, err_msg);
            });

        RUSTFP_LET_MUT(sink, sink_res);
        sinks_map.emplace(move(name), move(sink));
    }

    return Ok(move(sinks_map));
}

inline auto setup_formats_impl(const std::shared_ptr<cpptoml::table> &config)
    -> rustfp::
        Result<std::unordered_map<std::string, std::string>, std::string> {

    using names::NAME;
    using names::PATTERN_TABLE;
    using names::VALUE;

    // fmt
    using fmt::format;

    // rustfp
    using rustfp::Err;
    using rustfp::Ok;

    // std
    using std::move;
    using std::string;
    using std::unordered_map;

    // possible to return an entire empty pattern map
    unordered_map<string, string> patterns_map;

    const auto formats = config->get_table_array(PATTERN_TABLE);

    if (formats) {
        for (const auto &format_table : *formats) {
            auto name_res = value_from_table<string>(
                format_table,
                NAME,
                format("One of the formats does not have a '{}' field", NAME));

            RUSTFP_LET(name, name_res);

            auto value_res = value_from_table<string>(
                format_table,
                VALUE,
                format("Format '{}' does not have '{}' field", name, VALUE));

            RUSTFP_LET_MUT(value, value_res);
            patterns_map.emplace(move(name), move(value));
        }
    }

    return Ok(move(patterns_map));
}

inline auto setup_loggers_impl(
    const std::shared_ptr<cpptoml::table> &config,
    const std::unordered_map<std::string, std::shared_ptr<spdlog::sinks::sink>>
        &sinks_map,
    const std::unordered_map<std::string, std::string> &patterns_map)
    -> rustfp::Result<rustfp::unit_t, std::string> {

    using names::GLOBAL_PATTERN;
    using names::LOGGER_TABLE;
    using names::NAME;
    using names::PATTERN;
    using names::SINKS;

    // fmt
    using fmt::format;

    // rustfp
    using rustfp::Err;
    using rustfp::None;
    using rustfp::Ok;
    using rustfp::Option;
    using rustfp::Result;
    using rustfp::Some;
    using rustfp::Unit;

    // std
    using std::exception;
    using std::make_shared;
    using std::move;
    using std::shared_ptr;
    using std::string;
    using std::vector;

    const auto loggers = config->get_table_array(LOGGER_TABLE);

    if (!loggers) {
        return Err(string("No loggers configured for set-up"));
    }

    // set up possibly the global pattern if present
    const auto global_pattern_opt =
        value_from_table_opt<string>(config, GLOBAL_PATTERN);

    for (const auto &logger_table : *loggers) {
        auto name_res = value_from_table<string>(
            logger_table,
            NAME,
            format("One of the loggers does not have a '{}' field", NAME));

        RUSTFP_LET(name, name_res);

        auto sinks_res = array_from_table<string>(
            logger_table,
            SINKS,
            format(
                "Logger '{}' does not have a '{}' field of sink names",
                name,
                SINKS));

        RUSTFP_LET(sinks, sinks_res);

        vector<shared_ptr<spdlog::sinks::sink>> logger_sinks;
        logger_sinks.reserve(sinks.size());

        for (const auto &sink_name : sinks) {
            auto sink_res = find_value_from_map(
                sinks_map,
                sink_name,
                format(
                    "Unable to find sink '{}' for logger '{}'",
                    sink_name,
                    name));

            RUSTFP_LET_MUT(sink, sink_res);
            logger_sinks.push_back(move(sink));
        }

        const auto logger = make_shared<spdlog::logger>(
            name, logger_sinks.cbegin(), logger_sinks.cend());

        // optional fields
        auto add_msg_res = add_msg_on_err(
            set_logger_level_if_present(logger_table, logger),
            [&name](const string &err_msg) {
                return format(
                    "Logger '{}' set level error:\n > {}", name, err_msg);
            });

        RUSTFP_RET_IF_ERR(add_msg_res);

        const auto pattern_name_opt =
            value_from_table_opt<string>(logger_table, PATTERN);

        auto pattern_value_opt_res = pattern_name_opt.match(
            [&name, &patterns_map](
                const string &pattern_name) -> Result<Option<string>, string> {
                auto pattern_value_res = find_value_from_map(
                    patterns_map,
                    pattern_name,
                    format(
                        "Pattern name '{}' cannot be found for logger '{}'",
                        pattern_name,
                        name));

                return move(pattern_value_res).map([](string &&pattern_value) {
                    return Some(move(pattern_value));
                });
            },

            [] { return Ok(Option<string>(None)); });

        RUSTFP_LET_MUT(pattern_value_opt, pattern_value_opt_res);
        auto cloned_global_pattern_opt = global_pattern_opt;

        const auto selected_pattern_opt =
            move(pattern_value_opt).or_else([&cloned_global_pattern_opt] {
                return move(cloned_global_pattern_opt);
            });

        try {
            selected_pattern_opt.match_some([&logger](const string &pattern) {
                logger->set_pattern(pattern);
            });
        } catch (const exception &e) {
            return Err(format(
                "Error setting pattern to logger '{}': {}", name, e.what()));
        }

        spdlog::register_logger(logger);
    }

    return Ok(Unit);
}

inline auto setup_impl(const std::shared_ptr<cpptoml::table> &config)
    -> rustfp::Result<rustfp::unit_t, std::string> {

    // rustfp
    using rustfp::Err;
    using rustfp::Ok;
    using rustfp::Unit;

    // set up sinks
    auto sinks_map_res = setup_sinks_impl(config);
    RUSTFP_LET(sinks_map, sinks_map_res);

    // set up patterns
    auto patterns_map_res = setup_formats_impl(config);
    RUSTFP_LET(patterns_map, patterns_map_res);

    // set up loggers, setting the respective sinks and patterns
    auto loggers_setup_res =
        setup_loggers_impl(config, sinks_map, patterns_map);

    RUSTFP_LET(loggers_setup, loggers_setup_res);
    return Ok(Unit);
}
} // namespace details

template <class... Ps>
auto from_file_with_tag_replacement(
    const std::string &pre_toml_path, Ps &&... ps)
    -> rustfp::Result<rustfp::unit_t, std::string> {

    // fmt
    using fmt::format;

    // rustfp
    using rustfp::Err;
    using rustfp::Ok;
    using rustfp::Unit;

    // std
    using std::exception;
    using std::forward;
    using std::ifstream;
    using std::string;
    using std::stringstream;

    try {
        ifstream file_stream(pre_toml_path);

        if (!file_stream) {
            return Err(format("Error reading file at '{}'", pre_toml_path));
        }

        stringstream pre_toml_ss;
        pre_toml_ss << file_stream.rdbuf();

        const auto pre_toml_content = pre_toml_ss.str();

        const auto toml_content =
            format(pre_toml_content, std::forward<Ps>(ps)...);

        stringstream toml_ss;
        toml_ss << toml_content;

        cpptoml::parser parser{toml_ss};
        const auto config = parser.parse();

        return details::setup_impl(config);
    } catch (const exception &e) {
        return Err(string(e.what()));
    }
}

inline auto from_file(const std::string &toml_path)
    -> rustfp::Result<rustfp::unit_t, std::string> {

    // rustfp
    using rustfp::Err;

    // std
    using std::exception;
    using std::string;

    try {
        const auto config = cpptoml::parse_file(toml_path);
        return details::setup_impl(config);
    } catch (const exception &e) {
        return Err(string(e.what()));
    }
}
} // namespace spdlog_setup
