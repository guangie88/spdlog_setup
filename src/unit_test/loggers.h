/**
 * Unit tests implementation.
 * @author Chen Weiguang
 * @version 0.3.0-alpha.2
 */

#pragma once

#include "conf.h"

#include <memory>
#include <string>
#include <utility>

static constexpr auto TEST_LOGGER_NAME = "default";
static constexpr auto TEST_SINK_NAME = "default";

inline auto generate_simple_default_logger_table()
    -> std::shared_ptr<cpptoml::table> {
    namespace names = spdlog_setup::details::names;

    auto logger_table = cpptoml::make_table();
    logger_table->insert(names::NAME, std::string(TEST_LOGGER_NAME));
    logger_table->insert(names::SINKS, cpptoml::make_array());
    return std::move(logger_table);
}

inline auto generate_simple_sync_logger_table()
    -> std::shared_ptr<cpptoml::table> {
    namespace names = spdlog_setup::details::names;

    auto logger_table = cpptoml::make_table();
    logger_table->insert(names::NAME, std::string(TEST_LOGGER_NAME));
    logger_table->insert(names::TYPE, names::SYNC);
    logger_table->insert(names::SINKS, cpptoml::make_array());
    return std::move(logger_table);
}

inline auto generate_simple_async_logger_table()
    -> std::shared_ptr<cpptoml::table> {
    namespace names = spdlog_setup::details::names;

    auto logger_table = cpptoml::make_table();
    logger_table->insert(names::NAME, std::string(TEST_LOGGER_NAME));
    logger_table->insert(names::TYPE, names::ASYNC);
    logger_table->insert(names::SINKS, cpptoml::make_array());
    return std::move(logger_table);
}

inline auto generate_global_async_logger_table()
    -> std::shared_ptr<cpptoml::table> {
    namespace names = spdlog_setup::details::names;

    auto logger_table = cpptoml::make_table();
    logger_table->insert(names::NAME, std::string(TEST_LOGGER_NAME));
    logger_table->insert(names::TYPE, names::ASYNC);
    logger_table->insert(names::SINKS, cpptoml::make_array());
    return std::move(logger_table);
}

inline auto generate_invalid_sync_logger_table()
    -> std::shared_ptr<cpptoml::table> {
    namespace names = spdlog_setup::details::names;

    auto logger_table = cpptoml::make_table();
    logger_table->insert(names::NAME, std::string(TEST_LOGGER_NAME));
    logger_table->insert(names::TYPE, "xxx");
    logger_table->insert(names::SINKS, cpptoml::make_array());
    return std::move(logger_table);
}

inline auto generate_async_overflow_policy_block_logger_table()
    -> std::shared_ptr<cpptoml::table> {
    namespace names = spdlog_setup::details::names;

    auto logger_table = cpptoml::make_table();
    logger_table->insert(names::NAME, std::string(TEST_LOGGER_NAME));
    logger_table->insert(names::TYPE, names::ASYNC);
    logger_table->insert(names::OVERFLOW_POLICY, names::BLOCK);
    logger_table->insert(names::SINKS, cpptoml::make_array());
    return std::move(logger_table);
}

inline auto generate_async_overflow_policy_overrun_oldest_logger_table()
    -> std::shared_ptr<cpptoml::table> {
    namespace names = spdlog_setup::details::names;

    auto logger_table = cpptoml::make_table();
    logger_table->insert(names::NAME, std::string(TEST_LOGGER_NAME));
    logger_table->insert(names::TYPE, names::ASYNC);
    logger_table->insert(names::OVERFLOW_POLICY, names::OVERRUN_OLDEST);
    logger_table->insert(names::SINKS, cpptoml::make_array());
    return std::move(logger_table);
}

inline auto generate_invalid_async_overflow_policy_logger_table()
    -> std::shared_ptr<cpptoml::table> {
    namespace names = spdlog_setup::details::names;

    auto logger_table = cpptoml::make_table();
    logger_table->insert(names::NAME, std::string(TEST_LOGGER_NAME));
    logger_table->insert(names::TYPE, names::ASYNC);
    logger_table->insert(names::OVERFLOW_POLICY, "xxx");
    logger_table->insert(names::SINKS, cpptoml::make_array());
    return std::move(logger_table);
}

inline auto generate_simple_logger_with_sink_table()
    -> std::shared_ptr<cpptoml::table> {
    namespace names = spdlog_setup::details::names;

    auto logger_table = cpptoml::make_table();
    logger_table->insert(names::NAME, std::string(TEST_LOGGER_NAME));

    auto sinks = cpptoml::make_array();
    sinks->push_back(std::string(TEST_SINK_NAME));
    logger_table->insert(names::SINKS, std::move(sinks));

    return std::move(logger_table);
}
