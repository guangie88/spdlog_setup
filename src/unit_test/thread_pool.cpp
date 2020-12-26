/**
 * Unit tests implementation.
 * @author Chen Weiguang
 * @version 0.3.1
 */

#include "catch.hpp"

#include "thread_pool.h"

TEST_CASE("Parse global thread pool", "[parse_global_thread_pool]") {
    // Cannot test queue size and thread count as they are not exposed publicly
    // Can only test that the thread pool was changed to another instance
    const auto original = spdlog::thread_pool();

    const auto thread_pools = spdlog_setup::details::setup_thread_pools(
        generate_global_thread_pool());
    const auto modified = spdlog::thread_pool();

    REQUIRE(original != modified);
    REQUIRE(thread_pools.empty());
}

TEST_CASE("Parse simple thread pool", "[parse_simple_thread_pool]") {
    // No way to test non-global thread pool from spdlog sadly
    const auto thread_pools = spdlog_setup::details::setup_thread_pools(
        generate_simple_thread_pool());

    REQUIRE(thread_pools.size() == 1);
}

TEST_CASE("Parse multiple thread pools", "[parse_multiple_thread_pools]") {
    const auto thread_pools = spdlog_setup::details::setup_thread_pools(
        generate_multiple_thread_pools(10));

    REQUIRE(thread_pools.size() == 10);
}

TEST_CASE(
    "Parse invalid no name thread pool",
    "[parse_invalid_no_name_thread_pool]") {
    REQUIRE_THROWS_AS(
        spdlog_setup::details::setup_thread_pools(
            generate_invalid_no_name_thread_pool()),
        spdlog_setup::setup_error);
}

TEST_CASE(
    "Parse invalid no queue size thread pool",
    "[parse_invalid_no_queue_size_thread_pool]") {
    REQUIRE_THROWS_AS(
        spdlog_setup::details::setup_thread_pools(
            generate_invalid_no_queue_size_thread_pool()),
        spdlog_setup::setup_error);
}

TEST_CASE(
    "Parse invalid no num threads thread pool",
    "[parse_invalid_no_num_threads_thread_pool]") {
    REQUIRE_THROWS_AS(
        spdlog_setup::details::setup_thread_pools(
            generate_invalid_no_num_threads_thread_pool()),
        spdlog_setup::setup_error);
}
