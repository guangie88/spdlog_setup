/**
 * Unit tests implementation.
 * @author Chen Weiguang
 * @version 0.3.0
 */

#define CATCH_CONFIG_MAIN
#include "catch.hpp"

#if defined(unix) || defined(__unix__) || defined(__unix)
#ifndef SPDLOG_ENABLE_SYSLOG
#define SPDLOG_ENABLE_SYSLOG
#endif
#endif

#include "spdlog_setup/conf.h"

#include <iostream>
#include <string>

// spdlog_setup
using fmt::arg;
using spdlog_setup::setup_error;

// std
using std::cerr;
using std::string;

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
    ::spdlog::drop_all();

#ifdef _WIN32
    try {
        spdlog_setup::from_file("config/log_conf_win.toml");
    } catch (const setup_error &) {
        spdlog_setup::from_file("../config/log_conf_win.toml");
    }
#else
    try {
        spdlog_setup::from_file("config/log_conf_linux.toml");
    } catch (const setup_error &) {
        spdlog_setup::from_file("../config/log_conf_linux.toml");
    }
#endif

    const auto root_logger = ::spdlog::get("root");
    REQUIRE(root_logger != nullptr);

    root_logger->trace("Test Message - Trace!");
    root_logger->debug("Test Message - Debug!");
    root_logger->info("Test Message - Info!");
    root_logger->warn("Test Message - Warn!");
    root_logger->error("Test Message - Error!");
    root_logger->critical("Test Message - Critical!");

    const auto console_logger = ::spdlog::get("console");
    REQUIRE(console_logger != nullptr);

    console_logger->info("Console Message - Info!");
    console_logger->error("Console Message - Error!");
}

TEST_CASE("Parse pre-TOML file for set-up", "[from_file_with_tag_replacment]") {
    ::spdlog::drop_all();

    const auto index_arg = arg("index", 123);
    const auto path_arg = arg("path", "spdlog_setup");

    try {
        spdlog_setup::from_file_with_tag_replacement(
            "config/log_conf.pre.toml", index_arg, path_arg);
    } catch (const setup_error &) {
        spdlog_setup::from_file_with_tag_replacement(
            "../config/log_conf.pre.toml", index_arg, path_arg);
    }

    const auto root_logger = ::spdlog::get("root");
    REQUIRE(root_logger != nullptr);

    root_logger->trace("Test Message - Trace!");
    root_logger->debug("Test Message - Debug!");
    root_logger->info("Test Message - Info!");
    root_logger->warn("Test Message - Warn!");
    root_logger->error("Test Message - Error!");
    root_logger->critical("Test Message - Critical!");
}

TEST_CASE("Parse TOML file that does not exist", "[from_file_no_such_file]") {
    REQUIRE_THROWS_AS(
        spdlog_setup::from_file("config/no_such_file"), setup_error);
}

TEST_CASE(
    "Parse pre-TOML file that does not exist",
    "[from_file_with_tag_replacement_no_such_file]") {

    REQUIRE_THROWS_AS(
        spdlog_setup::from_file_with_tag_replacement("config/no_such_file"),
        setup_error);
}
