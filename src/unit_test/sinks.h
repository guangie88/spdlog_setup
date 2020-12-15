/**
 * Unit tests implementation.
 * @author Chen Weiguang
 * @version 0.3.2-pre
 */

#pragma once

#include "conf.h"

#include <memory>
#include <string>
#include <unordered_map>
#include <utility>

inline auto generate_stdout_sink_st() -> std::shared_ptr<cpptoml::table> {
    namespace names = spdlog_setup::details::names;

    auto sink_table = cpptoml::make_table();
    sink_table->insert(names::TYPE, std::string("stdout_sink_st"));
    return std::move(sink_table);
}

inline auto generate_stderr_sink_st() -> std::shared_ptr<cpptoml::table> {
    namespace names = spdlog_setup::details::names;

    auto sink_table = cpptoml::make_table();
    sink_table->insert(names::TYPE, std::string("stderr_sink_st"));
    return std::move(sink_table);
}

inline auto generate_stdout_sink_mt() -> std::shared_ptr<cpptoml::table> {
    namespace names = spdlog_setup::details::names;

    auto sink_table = cpptoml::make_table();
    sink_table->insert(names::TYPE, std::string("stdout_sink_mt"));
    return std::move(sink_table);
}

inline auto generate_stderr_sink_mt() -> std::shared_ptr<cpptoml::table> {
    namespace names = spdlog_setup::details::names;

    auto sink_table = cpptoml::make_table();
    sink_table->insert(names::TYPE, std::string("stderr_sink_mt"));
    return std::move(sink_table);
}

inline auto generate_dist_sink(std::string type)
    -> std::shared_ptr<cpptoml::table> {
    namespace names = spdlog_setup::details::names;

    auto ref_sinks = cpptoml::make_array();
    ref_sinks->push_back(std::string("sink1"));
    ref_sinks->push_back(std::string("sink2"));

    auto sink_table = cpptoml::make_table();
    sink_table->insert(names::TYPE, std::move(type));
    sink_table->insert(names::SINKS, ref_sinks);
    return std::move(sink_table);
}

inline auto generate_dist_sink_st() -> std::shared_ptr<cpptoml::table> {
    return generate_dist_sink("dist_sink_st");
}

inline auto generate_dist_sink_mt() -> std::shared_ptr<cpptoml::table> {
    return generate_dist_sink("dist_sink_mt");
}

inline auto generate_dist_sink_circular() -> std::shared_ptr<cpptoml::table> {
    namespace names = spdlog_setup::details::names;

    auto dist_sink1 = cpptoml::make_table();
    dist_sink1->insert(names::NAME, std::string("sink1"));
    dist_sink1->insert(names::TYPE, std::string("dist_sink_st"));
    auto dist_sink1_sinks = cpptoml::make_array();
    dist_sink1_sinks->push_back(std::string("sink2"));
    dist_sink1->insert(names::SINKS, dist_sink1_sinks);

    auto dist_sink2 = cpptoml::make_table();
    dist_sink2->insert(names::NAME, std::string("sink2"));
    dist_sink2->insert(names::TYPE, std::string("dist_sink_mt"));
    auto dist_sink2_sinks = cpptoml::make_array();
    dist_sink2_sinks->push_back(std::string("sink1"));
    dist_sink2->insert(names::SINKS, dist_sink2_sinks);

    auto sink_table = cpptoml::make_table_array();
    sink_table->push_back(dist_sink1);
    sink_table->push_back(dist_sink2);

    auto config = cpptoml::make_table();
    config->insert(names::SINK_TABLE, sink_table);
    return std::move(config);
}

inline auto generate_dist_sink_with_refs() -> std::shared_ptr<cpptoml::table> {
    namespace names = spdlog_setup::details::names;

    auto dist_sink = cpptoml::make_table();
    dist_sink->insert(names::NAME, std::string("dist_sink"));
    dist_sink->insert(names::TYPE, std::string("dist_sink_mt"));
    auto ref_sinks = cpptoml::make_array();
    ref_sinks->push_back(std::string("sink1"));
    ref_sinks->push_back(std::string("sink2"));
    dist_sink->insert(names::SINKS, ref_sinks);

    auto sink1 = cpptoml::make_table();
    sink1->insert(names::NAME, std::string("sink1"));
    sink1->insert(names::TYPE, std::string("null_sink_mt"));

    auto sink2 = cpptoml::make_table();
    sink2->insert(names::NAME, std::string("sink2"));
    sink2->insert(names::TYPE, std::string("null_sink_mt"));

    auto sink_table = cpptoml::make_table_array();
    sink_table->push_back(dist_sink);
    sink_table->push_back(sink1);
    sink_table->push_back(sink2);

    auto config = cpptoml::make_table();
    config->insert(names::SINK_TABLE, sink_table);
    return std::move(config);
}
