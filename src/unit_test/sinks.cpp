/**
 * Unit tests implementation.
 * @author Chen Weiguang
 * @version 0.3.1
 */

#include "catch.hpp"

#include "sinks.h"

#include <typeinfo>

TEST_CASE("Parse stdout sink st", "[parse_generate_stdout_sink_st]") {
    const auto sink =
        spdlog_setup::details::setup_sink(generate_stdout_sink_st());
    REQUIRE(typeid(*sink) == typeid(const spdlog::sinks::stdout_sink_st &));
}
