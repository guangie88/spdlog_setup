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

inline auto generate_global_thread_pool() -> std::shared_ptr<cpptoml::table> {
    namespace names = spdlog_setup::details::names;

    auto global_thread_inner_pool_table = cpptoml::make_table();
    global_thread_inner_pool_table->insert(names::QUEUE_SIZE, size_t(1234));
    global_thread_inner_pool_table->insert(names::NUM_THREADS, size_t(9));

    auto global_thread_pool_table = cpptoml::make_table();
    global_thread_pool_table->insert(
        names::GLOBAL_THREAD_POOL_TABLE,
        std::move(global_thread_inner_pool_table));

    return std::move(global_thread_pool_table);
}
