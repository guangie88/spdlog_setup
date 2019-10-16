/**
 * Unit tests implementation.
 * @author Chen Weiguang
 * @version 0.3.0-alpha.2
 */

#define CATCH_CONFIG_MAIN
#include "catch.hpp"

#if defined(unix) || defined(__unix__) || defined(__unix)
#ifndef SPDLOG_ENABLE_SYSLOG
#define SPDLOG_ENABLE_SYSLOG
#endif
#endif

#include "examples.h"

#include "spdlog_setup/conf.h"

#include <fstream>
#include <iostream>
#include <iterator>
#include <memory>
#include <string>
#include <unordered_map>
#include <utility>

using namespace examples;

#include "third_party/cpptoml.h"
#if defined(SPDLOG_SETUP_CPPTOML_EXTERNAL)
#include "cpptoml.h"
#endif

// spdlog_setup
using fmt::arg;
using spdlog::level::level_enum;
using spdlog_setup::setup_error;
using spdlog_setup::details::render;

namespace names = spdlog_setup::details::names;
using names::LEVEL;
using names::LOGGER_TABLE;
using names::NAME;
using names::PATTERN_TABLE;
using names::SINKS;
using names::TYPE;
using names::VALUE;

// std
using std::make_shared;
using std::move;
using std::string;
using std::unordered_map;

auto generate_simple_default_logger_table() -> std::shared_ptr<cpptoml::table> {
    auto logger_table = make_shared<cpptoml::table>();
    logger_table->insert(NAME, string("default"));
    logger_table->insert(SINKS, cpptoml::make_array());
    return move(logger_table);
}

auto generate_simple_async_logger_table() -> std::shared_ptr<cpptoml::table> {
    auto logger_table = make_shared<cpptoml::table>();
    logger_table->insert(NAME, string("default"));
    logger_table->insert(TYPE, string("async"));
    return move(logger_table);
}

// TEST_CASE("Parse async logger", "[parse_async_logger]") {
//     REQUIRE(render("", {{}}) == "");
//     REQUIRE(render("abc", {{}}) == "abc");
//     REQUIRE(render("{{ a }}", {{"a", "Alpha"}}) == "Alpha");
// }
