/**
 * Implementation of spdlog_setup in entirety.
 * @author Chen Weiguang
 * @version 0.1.0
 */

#pragma once

#define FMT_HEADER_ONLY

#include "cpptoml.h"
#include "fmt/format.h"
#include "rustfp/let.h"
#include "rustfp/result.h"
#include "rustfp/unit.h"
#include "spdlog/sinks/null_sink.h"
#include "spdlog/sinks/sink.h"

#ifdef _WIN32
#include "spdlog/sinks/wincolor_sink.h"
#else
#include "spdlog/sinks/ansicolor_sink.h"
#endif

#include "spdlog/spdlog.h"

#include <cstdint>
#include <exception>
#include <memory>
#include <regex>
#include <string>
#include <type_traits>
#include <unordered_map>
#include <utility>
#include <vector>

namespace spdlog_setup {
    // declaration section

    /**
     * Describes the sink types in enumeration form.
     */
    enum class SinkType {
        /** Represents either wincolor_stdout_sink_mt (Windows) or ansicolor_stdout_sink_mt (Linux) **/
        ColorStdoutSinkMt,

        /** Represents rotating_file_mt **/
        RotatingFileMt,
    };

    /**
     * Performs spdlog configuration setup from file.
     * @param toml_path Path to the TOML configuration file path.
     * @return true if setup is successful, otherwise false.
     */
    auto from_file(const std::string &toml_path) ->
        ::rustfp::Result<::rustfp::unit_t, std::string>;

    // implementation section

    namespace details {
        template <class T, class Fn>
        auto if_value_from_table(
            const std::shared_ptr<::cpptoml::table> &table,
            const char field[],
            Fn &&if_value_fn) -> std::result_of_t<Fn(const std::string &)> {

            // rustfp
            using ::rustfp::Ok;
            using ::rustfp::Unit;

            static constexpr auto LEVEL = "level";
            const auto value_opt = table->get_as<T>(field);

            if (value_opt) {
                return if_value_fn(*value_opt);
            }

            return Ok(Unit);
        }

        template <class T>
        auto value_from_table(
            const std::shared_ptr<::cpptoml::table> &table,
            const char field[],
            const std::string &err_msg) ->
            ::rustfp::Result<T, std::string> {

            // rustfp
            using ::rustfp::Err;
            using ::rustfp::Ok;

            const auto value_opt = table->get_as<T>(field);

            if (!value_opt) {
                return Err(err_msg);
            }

            return Ok(*value_opt);
        }

        template <class T>
        auto array_from_table(
            const std::shared_ptr<::cpptoml::table> &table,
            const char field[],
            const std::string &err_msg) ->
            ::rustfp::Result<std::vector<T>, std::string> {

            // rustfp
            using ::rustfp::Err;
            using ::rustfp::Ok;

            const auto array_opt = table->get_array_of<T>(field);

            if (!array_opt) {
                return Err(err_msg);
            }

            return Ok(*array_opt);
        }

        template <class Map, class Key>
        auto find_value_from_map(
            const Map &m,
            const Key &key,
            const std::string &err_msg) -> ::rustfp::Result<typename Map::mapped_type, std::string> {

            // rustfp
            using ::rustfp::Err;
            using ::rustfp::Ok;

            const auto iter = m.find(key);

            if (iter == m.cend()) {
                return Err(err_msg);
            }

            return Ok(iter->second);
        }

        template <class T, class Fn>
        inline auto add_msg_on_err(
            ::rustfp::Result<T, std::string> &&res,
            Fn &&add_msg_on_err_fn) ->
            ::rustfp::Result<T, std::string> {

            // std
            using std::move;
            using std::string;

            return move(res).map_err(
                [&add_msg_on_err_fn](string &&err_msg) {
                    return add_msg_on_err_fn(move(err_msg));
                });
        }

        inline auto parse_max_size(const std::string &max_size_str) ->
            ::rustfp::Result<uint64_t, std::string> {

            // fmt
            using ::fmt::format;

            // rustfp
            using ::rustfp::Err;
            using ::rustfp::Ok;

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
                        return Err(format("Unexpected suffix '{}' for max size parsing", suffix));
                    }
                } else {
                    return Err(format("Invalid string '{}' for max size parsing", max_size_str));
                }
            } catch (const exception &e) {
                return Err(format("Unexpected exception for max size parsing on string '{}': {}",
                    max_size_str, e.what()));
            }
        }

        inline auto sink_type_from_str(const std::string &type) ->
            ::rustfp::Result<SinkType, std::string> {

            // fmt
            using ::fmt::format;

            // rustup
            using ::rustfp::Err;
            using ::rustfp::Ok;

            // std
            using std::string;
            using std::unordered_map;

            static const unordered_map<string, SinkType> MAPPING {
                {"color_stdout_sink_mt", SinkType::ColorStdoutSinkMt},
                {"rotating_file_sink_mt", SinkType::RotatingFileMt},
            };

            return find_value_from_map(MAPPING, type, format("Invalid sink type '{}' found", type));
        }

        inline auto rotating_file_sink_mt_from_table(const std::shared_ptr<::cpptoml::table> &sink_table) ->
            ::rustfp::Result<std::shared_ptr<::spdlog::sinks::sink>, std::string> {

            // fmt
            using ::fmt::format;

            // rustfp
            using ::rustfp::Err;
            using ::rustfp::Ok;
            using ::rustfp::Result;

            // std
            using std::make_shared;
            using std::shared_ptr;
            using std::string;
        
            static constexpr auto BASE_FILENAME = "base_filename";
            static constexpr auto MAX_SIZE = "max_size";
            static constexpr auto MAX_FILES = "max_files";

            RUSTFP_LET(base_filename, value_from_table<string>(
                sink_table, BASE_FILENAME, format("Missing '{}' field of string value for rotating_file_sink_mt", BASE_FILENAME)));

            RUSTFP_LET(max_filesize_str, value_from_table<string>(
                sink_table, MAX_SIZE, format("Missing '{}' field of string value for rotating_file_sink_mt", MAX_SIZE)));

            RUSTFP_LET(max_filesize, parse_max_size(max_filesize_str));

            RUSTFP_LET(max_files, value_from_table<uint64_t>(
                sink_table, MAX_FILES, format("Missing '{}' field of u64 value for rotating_file_sink_mt", MAX_FILES)));

            return Ok(shared_ptr<::spdlog::sinks::sink>(make_shared<::spdlog::sinks::rotating_file_sink_mt>(
                base_filename, max_filesize, max_files)));
        }

        auto level_from_str(const std::string &level) ->
            ::rustfp::Result<::spdlog::level::level_enum, std::string> {

            // fmt
            using ::fmt::format;

            // rustfp
            using ::rustfp::Err;
            using ::rustfp::Ok;
            using ::rustfp::Result;

            // spdlog
            namespace lv = ::spdlog::level;

            // std
            using std::move;
            using std::string;

            using level_result_t = Result<lv::level_enum, string>;

            auto level_res =
                (level == "trace") ? Ok(lv::trace) :
                (level == "debug") ? Ok(lv::debug) :
                (level == "info") ? Ok(lv::info) :
                (level == "warn") ? Ok(lv::warn) :
                (level == "err") ? Ok(lv::err) :
                (level == "critical") ? Ok(lv::critical) :
                (level == "off") ? Ok(lv::off) :
                level_result_t(Err(format("Invalid level string '{}' provided", level)));

            return move(level_res);
        }
        
        inline auto set_sink_level_if_present(
            const std::shared_ptr<::cpptoml::table> &sink_table,
            const std::shared_ptr<::spdlog::sinks::sink> &sink) ->
            ::rustfp::Result<::rustfp::unit_t, std::string> {

            // rustfp
            using ::rustfp::Ok;
            using ::rustfp::Unit;
            using ::rustfp::unit_t;
            using ::rustfp::Result;

            // std
            using std::string;

            using unit_result_t = Result<unit_t, string>;
            static constexpr auto LEVEL = "level";

            return if_value_from_table<string>(sink_table, LEVEL,
                [&sink](const string &level) -> unit_result_t {
                    RUSTFP_LET(level_enum, level_from_str(level));
                    sink->set_level(level_enum);
                    return Ok(Unit);
                });
        }

        inline auto sink_from_table(const std::shared_ptr<::cpptoml::table> &sink_table) ->
            ::rustfp::Result<std::shared_ptr<::spdlog::sinks::sink>, std::string> {

            // fmt
            using ::fmt::format;

            // rustfp
            using ::rustfp::Err;
            using ::rustfp::Ok;
            using ::rustfp::Result;

            // std
            using std::make_shared;
            using std::move;
            using std::shared_ptr;
            using std::string;

            static constexpr auto TYPE = "type";
            using sink_result_t = Result<shared_ptr<::spdlog::sinks::sink>, string>;

#ifdef _WIN32
            using color_stdout_sink_mt = ::spdlog::sinks::wincolor_stdout_sink_mt;
#else
            using color_stdout_sink_mt = ::spdlog::sinks::ansicolor_stdout_sink_mt;
#endif

            RUSTFP_LET(type, value_from_table<string>(sink_table, TYPE, format("Sink missing '{}' field", TYPE)));

            return sink_type_from_str(type)
                .and_then([&sink_table, &type](const SinkType sink_type) -> sink_result_t {
                    // find the correct sink type to create
                    switch (sink_type) {
                        case SinkType::ColorStdoutSinkMt:
                            return Ok(shared_ptr<::spdlog::sinks::sink>(make_shared<color_stdout_sink_mt>()));

                        case SinkType::RotatingFileMt:
                            return rotating_file_sink_mt_from_table(sink_table);

                        default:
                            return Err(format("Unexpected sink error with sink type '{}'", type));
                    }
                })
                .and_then([&sink_table](shared_ptr<::spdlog::sinks::sink> &&sink) -> sink_result_t {
                    // set optional parts and return back the same sink
                    RUSTFP_LET(_sink_level_set, set_sink_level_if_present(sink_table, sink));
                    return Ok(move(sink));
                });
        }

        inline auto set_logger_level_if_present(
            const std::shared_ptr<::cpptoml::table> &logger_table,
            const std::shared_ptr<::spdlog::logger> &logger) ->
            ::rustfp::Result<::rustfp::unit_t, std::string> {

            // rustfp
            using ::rustfp::Ok;
            using ::rustfp::Unit;
            using ::rustfp::unit_t;
            using ::rustfp::Result;

            // std
            using std::string;

            using unit_result_t = Result<unit_t, string>;
            static constexpr auto LEVEL = "level";

            return if_value_from_table<string>(logger_table, LEVEL,
                [&logger](const string &level) -> unit_result_t {
                    RUSTFP_LET(level_enum, level_from_str(level));
                    logger->set_level(level_enum);
                    return Ok(Unit);
                });
        }
    }

    inline auto from_file(const std::string &toml_path) ->
        ::rustfp::Result<::rustfp::unit_t, std::string> {

        // fmt
        using ::fmt::format;

        // rustfp
        using ::rustfp::Err;
        using ::rustfp::Ok;
        using ::rustfp::Unit;

        // std
        using std::exception;
        using std::make_shared;
        using std::move;
        using std::shared_ptr;
        using std::string;
        using std::unordered_map;
        using std::vector;
        
        // table names
        static constexpr auto SINK_TABLE = "sink";
        static constexpr auto LOGGER_TABLE = "logger";

        // common fields
        static constexpr auto NAME = "name";
        static constexpr auto SINKS = "sinks";

        try {
            const auto config = ::cpptoml::parse_file(toml_path);

            // set up sinks
            const auto sinks = config->get_table_array(SINK_TABLE);

            if (!sinks) {
                return Err(string("No sinks configured for set-up"));
            }

            unordered_map<string, shared_ptr<::spdlog::sinks::sink>> sinks_map;

            for (const auto &sink_table : *sinks) {
                RUSTFP_LET(name, details::value_from_table<string>(
                    sink_table, NAME, format("One of the sinks does not have a '{}' field", NAME)));

                RUSTFP_LET_MUT(sink, details::add_msg_on_err(details::sink_from_table(sink_table),
                    [&name](const string &err_msg) {
                        return format("Sink '{}' error:\n > {}", name, err_msg);
                    }));

                sinks_map.emplace(name, move(sink));
            }

            // set up loggers 
            const auto loggers = config->get_table_array(LOGGER_TABLE);

            if (!loggers) {
                return Err(string("No loggers configured for set-up"));
            }

            for (const auto &logger_table : *loggers) {
                RUSTFP_LET(name, details::value_from_table<string>(
                    logger_table, NAME, format("One of the loggers does not have a '{}' field", NAME)));

                RUSTFP_LET(sinks, details::array_from_table<string>(
                    logger_table, SINKS, format("Logger '{}' does not have a '{}' field of sink names", name, SINKS)));

                vector<shared_ptr<::spdlog::sinks::sink>> logger_sinks;
                logger_sinks.reserve(sinks.size());

                for (const auto &sink_name : sinks) {
                    RUSTFP_LET_MUT(sink, details::find_value_from_map(sinks_map, sink_name,
                        format("Unable to find sink '{}' for logger '{}'", sink_name, name)));

                    logger_sinks.push_back(move(sink));
                }

                const auto logger = make_shared<::spdlog::logger>(name, logger_sinks.cbegin(), logger_sinks.cend());

                // optional fields
                RUSTFP_LET(_level, details::add_msg_on_err(details::set_logger_level_if_present(logger_table, logger),
                    [&name](const string &err_msg) {
                        return format("Logger '{}' set level error:\n > {}", name, err_msg);
                    }));

                ::spdlog::register_logger(logger);
            }

            return Ok(Unit);

        } catch (const exception &e) {
            return Err(string(e.what()));
        }
    }
}