/**
 * Unit tests implementation.
 * @author Chen Weiguang
 * @version 0.3.1-pre
 */

#include "catch.hpp"

#include "loggers.h"
#include "sinks.h"

#include <memory>
#include <string>
#include <typeinfo>
#include <unordered_map>

const std::unordered_map<std::string, std::shared_ptr<spdlog::sinks::sink>>
    EMPTY_SINKS_MAP;
const std::
    unordered_map<std::string, std::shared_ptr<spdlog::details::thread_pool>>
        EMPTY_THREAD_POOLS_MAP;

TEST_CASE("Parse default logger", "[parse_simple_default_logger]") {
    const auto logger = spdlog_setup::details::setup_logger(
        generate_simple_default_logger_table(),
        EMPTY_SINKS_MAP,
        EMPTY_THREAD_POOLS_MAP);

    REQUIRE(logger->name() == TEST_LOGGER_NAME);
    REQUIRE(typeid(*logger) == typeid(const spdlog::logger &));
}

TEST_CASE("Parse sync logger", "[parse_simple_sync_logger]") {
    const auto logger = spdlog_setup::details::setup_logger(
        generate_simple_sync_logger_table(),
        EMPTY_SINKS_MAP,
        EMPTY_THREAD_POOLS_MAP);

    REQUIRE(logger->name() == TEST_LOGGER_NAME);
    REQUIRE(typeid(*logger) == typeid(const spdlog::logger &));
}

TEST_CASE("Parse async logger", "[parse_simple_async_logger]") {
    const auto logger = spdlog_setup::details::setup_logger(
        generate_simple_async_logger_table(),
        EMPTY_SINKS_MAP,
        EMPTY_THREAD_POOLS_MAP);

    REQUIRE(logger->name() == TEST_LOGGER_NAME);
    REQUIRE(typeid(*logger) == typeid(const spdlog::async_logger &));
}

TEST_CASE("Parse invalid sync logger", "[parse_invalid_sync_logger]") {
    REQUIRE_THROWS_AS(
        spdlog_setup::details::setup_logger(
            generate_invalid_sync_logger_table(),
            EMPTY_SINKS_MAP,
            EMPTY_THREAD_POOLS_MAP),
        spdlog_setup::setup_error);
}

TEST_CASE(
    "Parse async overflow policy block logger",
    "[parse_simple_async_overflow_policy_block_logger]") {
    const auto logger = spdlog_setup::details::setup_logger(
        generate_async_overflow_policy_block_logger_table(),
        EMPTY_SINKS_MAP,
        EMPTY_THREAD_POOLS_MAP);

    REQUIRE(logger->name() == TEST_LOGGER_NAME);
    REQUIRE(typeid(*logger) == typeid(const spdlog::async_logger &));
}

TEST_CASE(
    "Parse async overflow policy overrun oldest logger",
    "[parse_simple_async_overflow_policy_overrun_oldest_logger]") {
    const auto logger = spdlog_setup::details::setup_logger(
        generate_async_overflow_policy_overrun_oldest_logger_table(),
        EMPTY_SINKS_MAP,
        EMPTY_THREAD_POOLS_MAP);

    REQUIRE(logger->name() == TEST_LOGGER_NAME);
    REQUIRE(typeid(*logger) == typeid(const spdlog::async_logger &));
}

TEST_CASE(
    "Parse invalid async overflow policy logger",
    "[parse_invalid_async_overflow_policy_logger]") {
    REQUIRE_THROWS_AS(
        spdlog_setup::details::setup_logger(
            generate_invalid_async_overflow_policy_logger_table(),
            EMPTY_SINKS_MAP,
            EMPTY_THREAD_POOLS_MAP),
        spdlog_setup::setup_error);
}

TEST_CASE("Parse logger with sink", "[parse_logger_with_sink]") {
    auto sink = spdlog_setup::details::setup_sink(generate_stdout_sink_st());
    const auto sinks_map =
        std::unordered_map<std::string, std::shared_ptr<spdlog::sinks::sink>>{
            {TEST_SINK_NAME, std::move(sink)}};

    const auto logger = spdlog_setup::details::setup_logger(
        generate_simple_logger_with_sink_table(),
        sinks_map,
        EMPTY_THREAD_POOLS_MAP);

    REQUIRE(logger->name() == TEST_LOGGER_NAME);
    REQUIRE(logger->sinks()[0] == sinks_map.at(TEST_SINK_NAME));
}

TEST_CASE(
    "Parse logger with missing sink", "[parse_logger_with_missing_sink]") {
    auto sink = spdlog_setup::details::setup_sink(generate_stdout_sink_st());
    const auto sinks_map =
        std::unordered_map<std::string, std::shared_ptr<spdlog::sinks::sink>>{
            {"xxx", std::move(sink)}};

    REQUIRE_THROWS_AS(
        spdlog_setup::details::setup_logger(
            generate_simple_logger_with_sink_table(),
            sinks_map,
            EMPTY_THREAD_POOLS_MAP),
        spdlog_setup::setup_error);
}
