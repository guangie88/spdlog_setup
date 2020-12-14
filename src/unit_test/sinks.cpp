/**
 * Unit tests implementation.
 * @author Chen Weiguang
 * @version 0.3.2-pre
 */

#include "catch.hpp"

#include "sinks.h"

#include <typeinfo>

TEST_CASE("Parse stdout sink st", "[parse_generate_stdout_sink_st]") {
    std::vector<std::string> ref_sinks;
    const auto sink =
        spdlog_setup::details::setup_sink(generate_stdout_sink_st(), ref_sinks);
    REQUIRE(typeid(*sink) == typeid(const spdlog::sinks::stdout_sink_st &));
    REQUIRE(ref_sinks.empty());
}

TEST_CASE("Parse stderr sink st", "[parse_generate_stderr_sink_st]") {
    std::vector<std::string> ref_sinks;
    const auto sink =
        spdlog_setup::details::setup_sink(generate_stderr_sink_st(), ref_sinks);
    REQUIRE(typeid(*sink) == typeid(const spdlog::sinks::stderr_sink_st &));
    REQUIRE(ref_sinks.empty());
}

TEST_CASE("Parse stdout sink mt", "[parse_generate_stdout_sink_mt]") {
    std::vector<std::string> ref_sinks;
    const auto sink =
        spdlog_setup::details::setup_sink(generate_stdout_sink_mt(), ref_sinks);
    REQUIRE(typeid(*sink) == typeid(const spdlog::sinks::stdout_sink_mt &));
    REQUIRE(ref_sinks.empty());
}

TEST_CASE("Parse stderr sink mt", "[parse_generate_stderr_sink_mt]") {
    std::vector<std::string> ref_sinks;
    const auto sink =
        spdlog_setup::details::setup_sink(generate_stderr_sink_mt(), ref_sinks);
    REQUIRE(typeid(*sink) == typeid(const spdlog::sinks::stderr_sink_mt &));
    REQUIRE(ref_sinks.empty());
}
