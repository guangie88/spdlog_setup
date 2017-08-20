#include "spdlog_setup.h"

#define CATCH_CONFIG_MAIN
#include "catch.hpp"

#include <iostream>
#include <string>
#include <utility>

// std
using std::cerr;
using std::make_pair;
using std::string;

TEST_CASE("Parse max size no suffix", "[parse_max_size_no_suffix]") {
    const auto res = ::spdlog_setup::details::parse_max_size("123");
    REQUIRE(res.is_ok());
    REQUIRE(123 == res.get_unchecked());
}

TEST_CASE("Parse max size byte", "[parse_max_size_byte]") {
    const auto res = ::spdlog_setup::details::parse_max_size("123B");
    REQUIRE(res.is_ok());
    REQUIRE(123 == res.get_unchecked());
}

TEST_CASE("Parse max size kilo", "[parse_max_size_kilo]") {
    const auto res = ::spdlog_setup::details::parse_max_size("123K");
    REQUIRE(res.is_ok());
    REQUIRE(123 * 1024 == res.get_unchecked());
}

TEST_CASE("Parse max size kilobyte", "[parse_max_size_kilobyte]") {
    const auto res = ::spdlog_setup::details::parse_max_size("123KB");
    REQUIRE(res.is_ok());
    REQUIRE(123 * 1024 == res.get_unchecked());
}

TEST_CASE("Parse max size mega", "[parse_max_size_mega]") {
    const auto res = ::spdlog_setup::details::parse_max_size("123M");
    REQUIRE(res.is_ok());
    REQUIRE(123 * 1024 * 1024 == res.get_unchecked());
}

TEST_CASE("Parse max size megabyte", "[parse_max_size_megabyte]") {
    const auto res = ::spdlog_setup::details::parse_max_size("123MB");
    REQUIRE(res.is_ok());
    REQUIRE(123 * 1024 * 1024 == res.get_unchecked());
}

TEST_CASE("Parse max size giga", "[parse_max_size_giga]") {
    const auto res = ::spdlog_setup::details::parse_max_size("123G");
    REQUIRE(res.is_ok());
    REQUIRE(123ull * 1024 * 1024 * 1024 == res.get_unchecked());
}

TEST_CASE("Parse max size gigabyte", "[parse_max_size_gigabyte]") {
    const auto res = ::spdlog_setup::details::parse_max_size("123GB");
    REQUIRE(res.is_ok());
    REQUIRE(123ull * 1024 * 1024 * 1024 == res.get_unchecked());
}

TEST_CASE("Parse max size tera", "[parse_max_size_tera]") {
    const auto res = ::spdlog_setup::details::parse_max_size("123T");
    REQUIRE(res.is_ok());
    REQUIRE(123ull * 1024 * 1024 * 1024 * 1024 == res.get_unchecked());
}

TEST_CASE("Parse max size terabyte", "[parse_max_size_terabyte]") {
    const auto res = ::spdlog_setup::details::parse_max_size("123TB");
    REQUIRE(res.is_ok());
    REQUIRE(123ull * 1024 * 1024 * 1024 * 1024 == res.get_unchecked());
}

TEST_CASE("Parse max size weird", "[parse_max_size_weird]") {
    const auto res = ::spdlog_setup::details::parse_max_size("\n  45672  MB\t");
    REQUIRE(res.is_ok());
    REQUIRE(45672ull * 1024 * 1024 == res.get_unchecked());
}

TEST_CASE("Parse max size error", "[parse_max_size_error]") {
    const auto res = ::spdlog_setup::details::parse_max_size(" 1x2x3K");
    REQUIRE(res.is_err());
}

TEST_CASE("Parse TOML file for set-up", "[from_file]") {
    const auto res = ::spdlog_setup::from_file("config/log_conf.toml");

    res.match(
        [](auto) {
            const auto root_logger = ::spdlog::get("root");

            if (root_logger != nullptr) {
                root_logger->trace("Test Message - Trace!");
                root_logger->debug("Test Message - Debug!");
                root_logger->info("Test Message - Info!");
                root_logger->warn("Test Message - Warn!");
                root_logger->error("Test Message - Error!");
                root_logger->critical("Test Message - Critical!");
            } else {
                cerr << "Unable to get root logger!\n";
            }
        },
        [](const string &msg) {
            cerr << "spdlog_setup::from_file error: " << msg << '\n';
        });

    REQUIRE(res.is_ok());
    ::spdlog::drop_all();
}

TEST_CASE("Parse pre-TOML file for set-up", "[from_file_with_tag_replacment]") {
    const auto res = ::spdlog_setup::from_file_with_tag_replacement(
        "config/log_conf.pre.toml",
        make_pair("index", 123));

    res.match(
        [](auto) {
            const auto root_logger = ::spdlog::get("root");

            if (root_logger != nullptr) {
                root_logger->trace("Test Message - Trace!");
                root_logger->debug("Test Message - Debug!");
                root_logger->info("Test Message - Info!");
                root_logger->warn("Test Message - Warn!");
                root_logger->error("Test Message - Error!");
                root_logger->critical("Test Message - Critical!");
            } else {
                cerr << "Unable to get root logger!\n";
            }
        },
        [](const string &msg) {
            cerr << "spdlog_setup::from_file error: " << msg << '\n';
        });

    REQUIRE(res.is_ok());
    ::spdlog::drop_all();
}

TEST_CASE("Parse TOML file that does not exist", "[from_file_no_such_file]") {
    const auto res = ::spdlog_setup::from_file("config/no_such_file");
    REQUIRE(res.is_err());
}