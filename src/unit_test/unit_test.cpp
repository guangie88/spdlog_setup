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
