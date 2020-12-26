/**
 * Unit tests implementation.
 * @author Chen Weiguang
 * @version 0.3.1
 */

#pragma once

#include "conf.h"

#include <memory>
#include <string>
#include <utility>

static constexpr auto TEST_THREAD_POOL_NAME = "default";

inline auto generate_global_thread_pool() -> std::shared_ptr<cpptoml::table> {
    namespace names = spdlog_setup::details::names;

    auto global_thread_inner_pool_table = cpptoml::make_table();
    global_thread_inner_pool_table->insert(names::QUEUE_SIZE, size_t(1234));
    global_thread_inner_pool_table->insert(names::NUM_THREADS, size_t(9));

    auto conf = cpptoml::make_table();
    conf->insert(
        names::GLOBAL_THREAD_POOL_TABLE,
        std::move(global_thread_inner_pool_table));

    return std::move(conf);
}

inline auto generate_simple_thread_pool() -> std::shared_ptr<cpptoml::table> {
    namespace names = spdlog_setup::details::names;

    auto thread_pool_table_array = cpptoml::make_table_array();

    auto thread_pool_table = cpptoml::make_table();
    thread_pool_table->insert(names::NAME, TEST_THREAD_POOL_NAME);
    thread_pool_table->insert(names::QUEUE_SIZE, size_t(1234));
    thread_pool_table->insert(names::NUM_THREADS, size_t(9));
    thread_pool_table_array->push_back(std::move(thread_pool_table));

    auto conf = cpptoml::make_table();
    conf->insert(names::THREAD_POOL_TABLE, std::move(thread_pool_table_array));

    return std::move(conf);
}

inline auto generate_multiple_thread_pools(const size_t count)
    -> std::shared_ptr<cpptoml::table> {
    namespace names = spdlog_setup::details::names;

    auto thread_pool_table_array = cpptoml::make_table_array();

    for (size_t i = 0; i < count; ++i) {
        auto thread_pool_table = cpptoml::make_table();
        thread_pool_table->insert(
            names::NAME, TEST_THREAD_POOL_NAME + std::to_string(i));
        thread_pool_table->insert(names::QUEUE_SIZE, size_t(1234));
        thread_pool_table->insert(names::NUM_THREADS, size_t(9));
        thread_pool_table_array->push_back(std::move(thread_pool_table));
    }

    auto conf = cpptoml::make_table();
    conf->insert(names::THREAD_POOL_TABLE, std::move(thread_pool_table_array));

    return std::move(conf);
}

inline auto generate_invalid_no_name_thread_pool()
    -> std::shared_ptr<cpptoml::table> {
    namespace names = spdlog_setup::details::names;

    auto thread_pool_table_array = cpptoml::make_table_array();

    auto thread_pool_table = cpptoml::make_table();
    thread_pool_table->insert(names::QUEUE_SIZE, size_t(1234));
    thread_pool_table->insert(names::NUM_THREADS, size_t(9));
    thread_pool_table_array->push_back(std::move(thread_pool_table));

    auto conf = cpptoml::make_table();
    conf->insert(names::THREAD_POOL_TABLE, std::move(thread_pool_table_array));

    return std::move(conf);
}

inline auto generate_invalid_no_queue_size_thread_pool()
    -> std::shared_ptr<cpptoml::table> {
    namespace names = spdlog_setup::details::names;

    auto thread_pool_table_array = cpptoml::make_table_array();

    auto thread_pool_table = cpptoml::make_table();
    thread_pool_table->insert(names::NAME, TEST_THREAD_POOL_NAME);
    thread_pool_table->insert(names::NUM_THREADS, size_t(9));
    thread_pool_table_array->push_back(std::move(thread_pool_table));

    auto conf = cpptoml::make_table();
    conf->insert(names::THREAD_POOL_TABLE, std::move(thread_pool_table_array));

    return std::move(conf);
}

inline auto generate_invalid_no_num_threads_thread_pool()
    -> std::shared_ptr<cpptoml::table> {
    namespace names = spdlog_setup::details::names;

    auto thread_pool_table_array = cpptoml::make_table_array();

    auto thread_pool_table = cpptoml::make_table();
    thread_pool_table->insert(names::NAME, TEST_THREAD_POOL_NAME);
    thread_pool_table->insert(names::QUEUE_SIZE, size_t(1234));
    thread_pool_table_array->push_back(std::move(thread_pool_table));

    auto conf = cpptoml::make_table();
    conf->insert(names::THREAD_POOL_TABLE, std::move(thread_pool_table_array));

    return std::move(conf);
}
