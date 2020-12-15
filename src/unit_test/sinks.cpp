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

TEST_CASE("Parse dist sink st", "[parse_generate_dist_sink_st]") {
    std::vector<std::string> ref_sinks;
    const auto sink =
        spdlog_setup::details::setup_sink(generate_dist_sink_st(), ref_sinks);
    REQUIRE(typeid(*sink) == typeid(const spdlog::sinks::dist_sink_st &));
    REQUIRE_THAT(ref_sinks, Catch::Equals<std::string>({"sink1", "sink2"}));
}

TEST_CASE("Parse dist sink mt", "[parse_generate_dist_sink_mt]") {
    std::vector<std::string> ref_sinks;
    const auto sink =
        spdlog_setup::details::setup_sink(generate_dist_sink_mt(), ref_sinks);
    REQUIRE(typeid(*sink) == typeid(const spdlog::sinks::dist_sink_mt &));
    REQUIRE_THAT(ref_sinks, Catch::Equals<std::string>({"sink1", "sink2"}));
}

TEST_CASE("Parse dist sink circular", "[parse_generate_dist_sink_circular]") {
    REQUIRE_THROWS_AS(
        spdlog_setup::details::setup_sinks(generate_dist_sink_circular()),
        spdlog_setup::setup_error);
}

TEST_CASE("Parse dist ref sinks", "[parse_generate_dist_sink_refs]") {
    auto sinks_map =
        spdlog_setup::details::setup_sinks(generate_dist_sink_with_refs());
    auto sink = sinks_map["dist_sink"];
    REQUIRE(typeid(*sink) == typeid(const spdlog::sinks::dist_sink_mt &));
    // Can't check the dist sink's wiring because there's no getter for its
    // subordinate sinks, but setup_sinks returning normally rather than
    // exceptionally is enough
}
