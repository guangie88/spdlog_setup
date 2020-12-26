/**
 * Unit tests implementation.
 * @author Chen Weiguang
 * @version 0.3.2-pre
 */

#define CATCH_CONFIG_MAIN
#include "catch.hpp"

#if defined(unix) || defined(__unix__) || defined(__unix)
#ifndef SPDLOG_ENABLE_SYSLOG
#define SPDLOG_ENABLE_SYSLOG
#endif
#endif

#include "examples.h"

#include "spdlog_setup/conf.h"

#include <fstream>
#include <iostream>
#include <iterator>
#include <string>
#include <unordered_map>

using namespace examples;

// spdlog_setup
using fmt::arg;
using spdlog::level::level_enum;
using spdlog_setup::setup_error;
using spdlog_setup::details::render;

namespace names = spdlog_setup::details::names;
using names::LEVEL;
using names::LOGGER_TABLE;
using names::NAME;
using names::PATTERN_TABLE;
using names::VALUE;

// std
using std::cerr;
using std::distance;
using std::getline;
using std::ifstream;
using std::string;
using std::unordered_map;

TEST_CASE("Parse max size no suffix", "[parse_max_size_no_suffix]") {
    REQUIRE(123 == spdlog_setup::details::parse_max_size("123"));
}

TEST_CASE("Parse max size byte", "[parse_max_size_byte]") {
    REQUIRE(123 == spdlog_setup::details::parse_max_size("123B"));
}

TEST_CASE("Parse max size kilo", "[parse_max_size_kilo]") {
    REQUIRE(123 * 1024 == spdlog_setup::details::parse_max_size("123K"));
}

TEST_CASE("Parse max size kilobyte", "[parse_max_size_kilobyte]") {
    REQUIRE(123 * 1024 == spdlog_setup::details::parse_max_size("123KB"));
}

TEST_CASE("Parse max size mega", "[parse_max_size_mega]") {
    REQUIRE(123 * 1024 * 1024 == spdlog_setup::details::parse_max_size("123M"));
}

TEST_CASE("Parse max size megabyte", "[parse_max_size_megabyte]") {
    REQUIRE(
        123 * 1024 * 1024 == spdlog_setup::details::parse_max_size("123MB"));
}

TEST_CASE("Parse max size giga", "[parse_max_size_giga]") {
    REQUIRE(
        123ull * 1024 * 1024 * 1024 ==
        spdlog_setup::details::parse_max_size("123G"));
}

TEST_CASE("Parse max size gigabyte", "[parse_max_size_gigabyte]") {
    REQUIRE(
        123ull * 1024 * 1024 * 1024 ==
        spdlog_setup::details::parse_max_size("123GB"));
}

TEST_CASE("Parse max size tera", "[parse_max_size_tera]") {
    REQUIRE(
        123ull * 1024 * 1024 * 1024 * 1024 ==
        spdlog_setup::details::parse_max_size("123T"));
}

TEST_CASE("Parse max size terabyte", "[parse_max_size_terabyte]") {
    REQUIRE(
        123ull * 1024 * 1024 * 1024 * 1024 ==
        spdlog_setup::details::parse_max_size("123TB"));
}

TEST_CASE("Parse max size weird", "[parse_max_size_weird]") {
    REQUIRE(
        45672ull * 1024 * 1024 ==
        spdlog_setup::details::parse_max_size("\n  45672  MB\t"));
}

TEST_CASE("Parse max size error", "[parse_max_size_error]") {
    REQUIRE_THROWS_AS(
        spdlog_setup::details::parse_max_size(" 1x2x3K"), setup_error);
}

TEST_CASE("Parse TOML file for set-up", "[from_file]") {
    spdlog::drop_all();

    const auto tmp_file = get_full_conf_tmp_file();
    spdlog_setup::from_file(tmp_file.get_file_path());

    const auto root_logger = spdlog::get("root");
    REQUIRE(root_logger != nullptr);

    root_logger->trace("Test Message - Trace!");
    root_logger->debug("Test Message - Debug!");
    root_logger->info("Test Message - Info!");
    root_logger->warn("Test Message - Warn!");
    root_logger->error("Test Message - Error!");
    root_logger->critical("Test Message - Critical!");

    const auto console_logger = spdlog::get("console");
    REQUIRE(console_logger != nullptr);

    console_logger->info("Console Message - Info!");
    console_logger->error("Console Message - Error!");
}

TEST_CASE(
    "Parse pre-TOML file for set-up", "[from_file_with_tag_replacement]") {
    spdlog::drop_all();

    const auto index_arg = arg("index", 123);
    const auto path_arg = arg("path", "spdlog_setup");
    const auto tmp_file = get_pre_conf_tmp_file();

    spdlog_setup::from_file_with_tag_replacement(
        tmp_file.get_file_path(), index_arg, path_arg);

    const auto root_logger = spdlog::get("root");
    REQUIRE(root_logger != nullptr);

    root_logger->trace("Test Message - Trace!");
    root_logger->debug("Test Message - Debug!");
    root_logger->info("Test Message - Info!");
    root_logger->warn("Test Message - Warn!");
    root_logger->error("Test Message - Error!");
    root_logger->critical("Test Message - Critical!");
}

TEST_CASE(
    "Parse TOML file with override for set-up", "[from_file_with_override]") {
    spdlog::drop_all();

    const auto full_conf_tmp_file = get_full_conf_tmp_file();
    const auto override_conf_tmp_file = get_override_conf_tmp_file();

    const auto use_override = spdlog_setup::from_file_and_override(
        full_conf_tmp_file.get_file_path(),
        override_conf_tmp_file.get_file_path());

    REQUIRE(use_override);

    const auto console_logger = spdlog::get("console");
    REQUIRE(console_logger != nullptr);

    console_logger->trace("Console Message - Trace!");
    console_logger->debug("Console Message - Debug!");
    console_logger->info("Console Message - Info!");
    console_logger->warn("Console Message - Warn!");
    console_logger->error("Console Message - Error!");
    console_logger->critical("Console Message - Critical!");
}

TEST_CASE(
    "Parse TOML file with empty override for set-up",
    "[from_file_with_override_empty]") {

    spdlog::drop_all();

    const auto full_conf_tmp_file = get_full_conf_tmp_file();

    const auto use_override = spdlog_setup::from_file_and_override(
        full_conf_tmp_file.get_file_path(), "no_such_file");

    REQUIRE(!use_override);

    const auto console_logger = spdlog::get("console");
    REQUIRE(console_logger != nullptr);

    console_logger->trace("Console Message - Trace!");
    console_logger->debug("Console Message - Debug!");
    console_logger->info("Console Message - Info!");
    console_logger->warn("Console Message - Warn!");
    console_logger->error("Console Message - Error!");
    console_logger->critical("Console Message - Critical!");
}

TEST_CASE(
    "Parse pre-TOML file with override for set-up",
    "[from_file_with_override_with_tag_replacement]") {
    spdlog::drop_all();

    const auto index_arg = arg("index", 123);
    const auto path_arg = arg("path", "spdlog_setup");
    const auto hash_arg = arg("hash", "qwerty");

    const auto pre_conf_tmp_file = get_pre_conf_tmp_file();
    const auto override_pre_conf_tmp_file = get_override_pre_conf_tmp_file();

    const auto use_override =
        spdlog_setup::from_file_and_override_with_tag_replacement(
            pre_conf_tmp_file.get_file_path(),
            override_pre_conf_tmp_file.get_file_path(),
            index_arg,
            path_arg,
            hash_arg);

    REQUIRE(use_override);

    const auto root_logger = spdlog::get("root");
    REQUIRE(root_logger != nullptr);

    root_logger->trace("Test Message - Trace!");
    root_logger->debug("Test Message - Debug!");
    root_logger->info("Test Message - Info!");
    root_logger->warn("Test Message - Warn!");
    root_logger->error("Test Message - Error!");
    root_logger->critical("Test Message - Critical!");
}

TEST_CASE(
    "Parse pre-TOML file with missing override for set-up",
    "[from_file_with_missing_override_with_tag_replacement]") {
    spdlog::drop_all();

    const auto index_arg = arg("index", 123);
    const auto path_arg = arg("path", "spdlog_setup");

    const auto pre_conf_tmp_file = get_pre_conf_tmp_file();

    const auto use_override =
        spdlog_setup::from_file_and_override_with_tag_replacement(
            pre_conf_tmp_file.get_file_path(),
            "no_such_file",
            index_arg,
            path_arg);

    REQUIRE(!use_override);

    const auto root_logger = spdlog::get("root");
    REQUIRE(root_logger != nullptr);

    root_logger->trace("Test Message - Trace!");
    root_logger->debug("Test Message - Debug!");
    root_logger->info("Test Message - Info!");
    root_logger->warn("Test Message - Warn!");
    root_logger->error("Test Message - Error!");
    root_logger->critical("Test Message - Critical!");
}

TEST_CASE("Parse TOML file that does not exist", "[from_file_no_such_file]") {
    spdlog::drop_all();

    REQUIRE_THROWS_AS(
        spdlog_setup::from_file("config/no_such_file"), setup_error);
}

TEST_CASE(
    "Parse pre-TOML file that does not exist",
    "[from_file_with_tag_replacement_no_such_file]") {

    spdlog::drop_all();

    REQUIRE_THROWS_AS(
        spdlog_setup::from_file_with_tag_replacement("config/no_such_file"),
        setup_error);
}

TEST_CASE("Save logger to new file", "[save_logger_to_file_new]") {
    spdlog::drop_all();
    const auto logger = spdlog::stdout_logger_mt("console");
    logger->set_level(level_enum::warn);

    const auto tmp_file = examples::tmp_file();
    const auto &tmp_file_path = tmp_file.get_file_path();

    spdlog_setup::save_logger_to_file(logger, tmp_file_path);

    const auto config = cpptoml::parse_file(tmp_file_path);
    REQUIRE(config != nullptr);

    const auto loggers = config->get_table_array(LOGGER_TABLE);
    REQUIRE(loggers != nullptr);

    const auto &loggers_ref = *loggers;
    REQUIRE(dist(loggers_ref) == 1);

    const auto &console = get_index(loggers_ref, 0);
    REQUIRE(console != nullptr);

    const auto &console_ref = *console;
    REQUIRE(dist(console_ref) == 2);

    const auto name = console_ref.get_as<string>(NAME);
    REQUIRE(static_cast<bool>(name));
    REQUIRE(*name == "console");

    const auto level = console_ref.get_as<string>(LEVEL);
    REQUIRE(static_cast<bool>(level));
    REQUIRE(*level == "warn");
}

TEST_CASE(
    "Save logger to file with existing logger",
    "[save_logger_to_file_no_overwrite]") {

    spdlog::drop_all();
    const auto logger = spdlog::stdout_logger_mt("console");
    logger->set_level(level_enum::critical);

    const auto tmp_file = get_simple_console_logger_conf_tmp_file();
    const auto &tmp_file_path = tmp_file.get_file_path();

    spdlog_setup::save_logger_to_file(logger, tmp_file_path);

    const auto config = cpptoml::parse_file(tmp_file_path);
    REQUIRE(config != nullptr);

    // patterns

    const auto patterns = config->get_table_array(PATTERN_TABLE);
    REQUIRE(patterns != nullptr);

    const auto &patterns_ref = *patterns;
    REQUIRE(dist(patterns_ref) == 1);

    const auto &pattern = get_index(patterns_ref, 0);
    REQUIRE(pattern != nullptr);

    const auto &pattern_ref = *pattern;

    {
        const auto name = pattern_ref.get_as<string>(NAME);
        REQUIRE(static_cast<bool>(name));
        REQUIRE(*name == "easy");

        const auto value = pattern_ref.get_as<string>(VALUE);
        REQUIRE(static_cast<bool>(value));
        REQUIRE(*value == "%L: %v");
    }

    // loggers

    const auto loggers = config->get_table_array(LOGGER_TABLE);
    REQUIRE(loggers != nullptr);

    const auto &loggers_ref = *loggers;
    REQUIRE(dist(loggers_ref) == 2);

    const auto &not_console = get_index(loggers_ref, 0);
    REQUIRE(not_console != nullptr);

    const auto &not_console_ref = *not_console;
    REQUIRE(dist(not_console_ref) == 3);

    {
        const auto name = not_console_ref.get_as<string>(NAME);
        REQUIRE(static_cast<bool>(name));
        REQUIRE(*name == "not-console");

        const auto pattern = not_console_ref.get_as<string>(names::PATTERN);
        REQUIRE(static_cast<bool>(pattern));
        REQUIRE(*pattern == "easy");

        const auto level = not_console_ref.get_as<string>(LEVEL);
        REQUIRE(static_cast<bool>(level));
        REQUIRE(*level == "info");
    }

    const auto &console = get_index(loggers_ref, 1);
    REQUIRE(console != nullptr);

    const auto &console_ref = *console;
    REQUIRE(dist(console_ref) == 3);

    {
        const auto name = console_ref.get_as<string>(NAME);
        REQUIRE(static_cast<bool>(name));
        REQUIRE(*name == "console");

        const auto pattern = console_ref.get_as<string>(names::PATTERN);
        REQUIRE(static_cast<bool>(pattern));
        REQUIRE(*pattern == "easy");

        const auto level = console_ref.get_as<string>(LEVEL);
        REQUIRE(static_cast<bool>(level));
        REQUIRE(*level == "critical");
    }
}

TEST_CASE(
    "Save logger to file overwriting existing file",
    "[save_logger_to_file_overwrite]") {

    spdlog::drop_all();
    const auto logger = spdlog::stdout_logger_mt("console");
    logger->set_level(level_enum::err);

    const auto tmp_file = get_simple_console_logger_conf_tmp_file();
    const auto &tmp_file_path = tmp_file.get_file_path();

    // true to overwrite the file completely
    spdlog_setup::save_logger_to_file(logger, tmp_file_path, true);

    const auto config = cpptoml::parse_file(tmp_file_path);
    REQUIRE(config != nullptr);

    // patterns

    const auto patterns = config->get_table_array(PATTERN_TABLE);
    REQUIRE(patterns == nullptr);

    // loggers

    const auto loggers = config->get_table_array(LOGGER_TABLE);
    REQUIRE(loggers != nullptr);

    const auto &loggers_ref = *loggers;
    REQUIRE(dist(loggers_ref) == 1);

    const auto &console = get_index(loggers_ref, 0);
    REQUIRE(console != nullptr);

    const auto &console_ref = *console;
    REQUIRE(dist(console_ref) == 2);

    const auto name = console_ref.get_as<string>(NAME);
    REQUIRE(static_cast<bool>(name));
    REQUIRE(*name == "console");

    const auto level = console_ref.get_as<string>(LEVEL);
    REQUIRE(static_cast<bool>(level));
    REQUIRE(*level == "err");
}

TEST_CASE("Delete logger from file simple", "[delete_logger_in_file_simple]") {
    spdlog::drop_all();

    const auto tmp_file = get_simple_console_logger_conf_tmp_file();
    const auto &tmp_file_path = tmp_file.get_file_path();

    {
        const auto entry_deleted =
            spdlog_setup::delete_logger_in_file("not-console", tmp_file_path);

        REQUIRE(entry_deleted);

        const auto config = cpptoml::parse_file(tmp_file_path);
        REQUIRE(config != nullptr);

        // patterns

        const auto patterns = config->get_table_array(PATTERN_TABLE);
        REQUIRE(patterns != nullptr);

        const auto &patterns_ref = *patterns;
        REQUIRE(dist(patterns_ref) == 1);

        const auto &pattern = get_index(patterns_ref, 0);
        REQUIRE(pattern != nullptr);

        const auto &pattern_ref = *pattern;

        {
            const auto name = pattern_ref.get_as<string>(NAME);
            REQUIRE(static_cast<bool>(name));
            REQUIRE(*name == "easy");

            const auto value = pattern_ref.get_as<string>(VALUE);
            REQUIRE(static_cast<bool>(value));
            REQUIRE(*value == "%L: %v");
        }

        // loggers

        const auto loggers = config->get_table_array(LOGGER_TABLE);
        REQUIRE(loggers != nullptr);

        const auto &loggers_ref = *loggers;
        REQUIRE(dist(loggers_ref) == 1);

        const auto &console = get_index(loggers_ref, 0);
        REQUIRE(console != nullptr);

        const auto &console_ref = *console;
        REQUIRE(dist(console_ref) == 3);

        {
            const auto name = console_ref.get_as<string>(NAME);
            REQUIRE(static_cast<bool>(name));
            REQUIRE(*name == "console");

            const auto pattern = console_ref.get_as<string>(names::PATTERN);
            REQUIRE(static_cast<bool>(pattern));
            REQUIRE(*pattern == "easy");

            const auto level = console_ref.get_as<string>(LEVEL);
            REQUIRE(static_cast<bool>(level));
            REQUIRE(*level == "trace");
        }
    }

    {
        const auto entry_deleted =
            spdlog_setup::delete_logger_in_file("no-such-thing", tmp_file_path);

        REQUIRE(!entry_deleted);

        const auto config = cpptoml::parse_file(tmp_file_path);
        REQUIRE(config != nullptr);

        const auto loggers = config->get_table_array(LOGGER_TABLE);
        REQUIRE(loggers != nullptr);

        const auto &loggers_ref = *loggers;
        REQUIRE(dist(loggers_ref) == 1);
    }

    {
        const auto entry_deleted =
            spdlog_setup::delete_logger_in_file("console", tmp_file_path);

        REQUIRE(entry_deleted);

        const auto config = cpptoml::parse_file(tmp_file_path);
        REQUIRE(config != nullptr);

        const auto loggers = config->get_table_array(LOGGER_TABLE);
        REQUIRE(loggers == nullptr);
    }

    // opening the file without any loggers will throw an exception

    REQUIRE_THROWS_AS(
        spdlog_setup::delete_logger_in_file("console", tmp_file_path),
        setup_error);
}

TEST_CASE("Delete logger from empty file", "[delete_logger_in_file_empty]") {
    spdlog::drop_all();

    const auto tmp_file = examples::tmp_file();
    const auto &tmp_file_path = tmp_file.get_file_path();

    // opening the file without any loggers will throw an exception

    REQUIRE_THROWS_AS(
        spdlog_setup::delete_logger_in_file("any-log", tmp_file_path),
        setup_error);
}

TEST_CASE("Check templating", "[check_templating]") {
    REQUIRE(render("", {{}}) == "");
    REQUIRE(render("abc", {{}}) == "abc");
    REQUIRE(render("{{ a }}", {{"a", "Alpha"}}) == "Alpha");

    REQUIRE(
        render(
            "{{ a }}{{bb}}{{ ccc}}{{dddd }}",
            {
                {"a", "Alpha"},
                {"bb", "Beta"},
                {"ccc", "Ceta"},
                {"dddd", "Delta"},
            }) == "AlphaBetaCetaDelta");

    REQUIRE(
        render("{{\"Hello\"}} {{ \"{{\" }} {{\"}}\" }}", {{}}) ==
        "Hello {{ }}");

    REQUIRE(
        render(
            "a{{b}}cd{{e}}f{{\"g\"}}hij{{ k}}{{l }}) ({{ b }}{{e}}",
            {
                {"b", "BBB"},
                {"k", "KKK"},
                {"l", "LLL"},
            }) == "aBBBcdfghijKKKLLL) (BBB");
}
