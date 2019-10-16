/**
 * Unit tests implementation.
 * @author Chen Weiguang
 * @version 0.3.0-alpha.2
 */

#include "catch.hpp"

#include "examples.h"

#include "spdlog_setup/conf.h"

#include <memory>
#include <string>
#include <typeinfo>
#include <utility>

using namespace examples;

// spdlog_setup
namespace names = spdlog_setup::details::names;

static constexpr auto TEST_LOGGER_NAME = "default";

auto generate_simple_default_logger_table() -> std::shared_ptr<cpptoml::table> {
    auto logger_table = cpptoml::make_table();
    logger_table->insert(names::NAME, std::string(TEST_LOGGER_NAME));
    logger_table->insert(names::SINKS, cpptoml::make_array());
    return std::move(logger_table);
}

auto generate_simple_sync_logger_table() -> std::shared_ptr<cpptoml::table> {
    auto logger_table = cpptoml::make_table();
    logger_table->insert(names::NAME, std::string(TEST_LOGGER_NAME));
    logger_table->insert(names::TYPE, names::SYNC);
    logger_table->insert(names::SINKS, cpptoml::make_array());
    return std::move(logger_table);
}

auto generate_simple_async_logger_table() -> std::shared_ptr<cpptoml::table> {
    auto logger_table = cpptoml::make_table();
    logger_table->insert(names::NAME, std::string(TEST_LOGGER_NAME));
    logger_table->insert(names::TYPE, names::ASYNC);
    logger_table->insert(names::SINKS, cpptoml::make_array());
    return std::move(logger_table);
}

auto generate_global_async_logger_table() -> std::shared_ptr<cpptoml::table> {
    auto logger_table = cpptoml::make_table();
    logger_table->insert(names::NAME, std::string(TEST_LOGGER_NAME));
    logger_table->insert(names::TYPE, names::ASYNC);
    logger_table->insert(names::SINKS, cpptoml::make_array());
    return std::move(logger_table);
}

auto generate_invalid_sync_logger_table() -> std::shared_ptr<cpptoml::table> {
    auto logger_table = cpptoml::make_table();
    logger_table->insert(names::NAME, std::string(TEST_LOGGER_NAME));
    logger_table->insert(names::TYPE, "xxx");
    logger_table->insert(names::SINKS, cpptoml::make_array());
    return std::move(logger_table);
}

auto generate_async_overflow_policy_block_logger_table()
    -> std::shared_ptr<cpptoml::table> {
    auto logger_table = cpptoml::make_table();
    logger_table->insert(names::NAME, std::string(TEST_LOGGER_NAME));
    logger_table->insert(names::TYPE, names::ASYNC);
    logger_table->insert(names::OVERFLOW_POLICY, names::BLOCK);
    logger_table->insert(names::SINKS, cpptoml::make_array());
    return std::move(logger_table);
}

auto generate_async_overflow_policy_overrun_oldest_logger_table()
    -> std::shared_ptr<cpptoml::table> {
    auto logger_table = cpptoml::make_table();
    logger_table->insert(names::NAME, std::string(TEST_LOGGER_NAME));
    logger_table->insert(names::TYPE, names::ASYNC);
    logger_table->insert(names::OVERFLOW_POLICY, names::OVERRUN_OLDEST);
    logger_table->insert(names::SINKS, cpptoml::make_array());
    return std::move(logger_table);
}

auto generate_invalid_async_overflow_policy_logger_table()
    -> std::shared_ptr<cpptoml::table> {
    auto logger_table = cpptoml::make_table();
    logger_table->insert(names::NAME, std::string(TEST_LOGGER_NAME));
    logger_table->insert(names::TYPE, names::ASYNC);
    logger_table->insert(names::OVERFLOW_POLICY, "xxx");
    logger_table->insert(names::SINKS, cpptoml::make_array());
    return std::move(logger_table);
}

TEST_CASE("Parse default logger", "[parse_simple_default_logger]") {
    const auto logger = spdlog_setup::details::setup_logger_impl(
        generate_simple_default_logger_table(), {}, {});

    REQUIRE(logger->name() == TEST_LOGGER_NAME);
    REQUIRE(typeid(*logger) == typeid(const spdlog::logger &));
}

TEST_CASE("Parse sync logger", "[parse_simple_sync_logger]") {
    const auto logger = spdlog_setup::details::setup_logger_impl(
        generate_simple_sync_logger_table(), {}, {});

    REQUIRE(logger->name() == TEST_LOGGER_NAME);
    REQUIRE(typeid(*logger) == typeid(const spdlog::logger &));
}

TEST_CASE("Parse async logger", "[parse_simple_async_logger]") {
    const auto logger = spdlog_setup::details::setup_logger_impl(
        generate_simple_async_logger_table(), {}, {});

    REQUIRE(logger->name() == TEST_LOGGER_NAME);
    REQUIRE(typeid(*logger) == typeid(const spdlog::async_logger &));
}

TEST_CASE("Parse invalid sync logger", "[parse_invalid_sync_logger]") {
    REQUIRE_THROWS_AS(
        spdlog_setup::details::setup_logger_impl(
            generate_invalid_sync_logger_table(), {}, {}),
        spdlog_setup::setup_error);
}

TEST_CASE(
    "Parse async overflow policy block logger",
    "[parse_simple_async_overflow_policy_block_logger]") {
    const auto logger = spdlog_setup::details::setup_logger_impl(
        generate_async_overflow_policy_block_logger_table(), {}, {});

    REQUIRE(logger->name() == TEST_LOGGER_NAME);
    REQUIRE(typeid(*logger) == typeid(const spdlog::async_logger &));
}

TEST_CASE(
    "Parse async overflow policy overrun oldest logger",
    "[parse_simple_async_overflow_policy_overrun_oldest_logger]") {
    const auto logger = spdlog_setup::details::setup_logger_impl(
        generate_async_overflow_policy_overrun_oldest_logger_table(), {}, {});

    REQUIRE(logger->name() == TEST_LOGGER_NAME);
    REQUIRE(typeid(*logger) == typeid(const spdlog::async_logger &));
}

TEST_CASE(
    "Parse invalid async overflow policy logger",
    "[parse_invalid_async_overflow_policy_logger]") {
    REQUIRE_THROWS_AS(
        spdlog_setup::details::setup_logger_impl(
            generate_invalid_async_overflow_policy_logger_table(), {}, {}),
        spdlog_setup::setup_error);
}
