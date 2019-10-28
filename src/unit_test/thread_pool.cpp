/**
 * Unit tests implementation.
 * @author Chen Weiguang
 * @version 0.3.0-alpha.2
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
