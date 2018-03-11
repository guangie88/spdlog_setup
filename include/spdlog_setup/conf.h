/**
 * Implementation of public facing functions in spdlog_setup.
 * @author Chen Weiguang
 * @version 0.3.0
 */

#pragma once

#include "details/conf_impl.h"

namespace spdlog_setup {
// declaration section

using details::setup_error;

/**
 * Performs spdlog configuration setup from file, with tag values to be
 * replaced into various primitive values.
 * @param pre_toml_path Path to the pre-TOML configuration file path.
 * @throw setup_error
 */
template <class... Ps>
void from_file_with_tag_replacement(
    const std::string &pre_toml_path, Ps &&... ps);

/**
 * Performs spdlog configuration setup from both base and override files, with
 * tag values to be replaced into various primitive values for both files. The
 * base file is required while the override file is optional.
 * @param base_pre_toml_path Path to the base pre-TOML configuration file path.
 * @param override_pre_toml_path Path to the override pre-TOML configuration
 * file path.
 * @throw setup_error
 */
template <class... Ps>
void from_file_and_override_with_tag_replacement(
    const std::string &base_pre_toml_path,
    const std::string &override_pre_toml_path,
    const Ps &... ps);

/**
 * Performs spdlog configuration setup from file.
 * @param toml_path Path to the TOML configuration file path.
 * @throw setup_error
 */
void from_file(const std::string &toml_path);

/**
 * Performs spdlog configuration setup from both base and override files. The
 * base file is required while the override file is optional.
 * The configuration values from the override file will be merged into the base
 * configuration values to form the overall configuration values.
 * @param base_toml_path Path to the base TOML configuration file path.
 * @param override_toml_path Path to the override TOML configuration file path.
 * @throw setup_error
 */
void from_file_and_override(
    const std::string &base_toml_path, const std::string &override_toml_path);

/**
 * Serializes the current logger level tagged with its logger name, and saves
 * into the a file. Currently only able to save the logger name and level.
 * Useful for creating override file. May choose to add the serialized content
 * to the file, or completely overwrite the file with just this serialized
 * content.
 * @param logger Logger to serialize and save.
 * @param toml_path Path to save the serialized content into.
 * @param overwrite Default false to add content into override file, true to
 * ignore the existing file if present, and overwrite the file.
 * @throw setup_error
 */
void save_logger_to_file(
    const std::shared_ptr<spdlog::logger> &logger,
    const std::string &toml_path,
    const bool overwrite = true);

void load_logger_from_file(
    const std::shared_ptr<spdlog::logger> &logger,
    const std::string &toml_path);

void load_logger_from_file_and_override(
    const std::shared_ptr<spdlog::logger> &logger,
    const std::string &base_toml_path,
    const std::string &override_toml_path);

/**
 * Resets the given logger back to its base configuration from file, while
 * removing the entry in the override file. Throws exception if the base file
 * does not contain any configuration for the logger.
 * @param logger_name Logger name whose configuration to remove in file.
 * @param toml_path Path whose file to delete the logger entry from.
 * @return True if entry of logger name is found for deletion, else false and
 * deletion has no effect on the file.
 * @throw setup_error
 */
auto delete_logger_in_file(
    const std::string &logger_name, const std::string &toml_path) -> bool;

// implementation section

template <class... Ps>
void from_file_with_tag_replacement(
    const std::string &pre_toml_path, Ps &&... ps) {

    // std
    using std::exception;
    using std::forward;

    try {
        auto toml_ss = details::read_template_file_into_stringstream(
            pre_toml_path, forward<Ps>(ps)...);

        cpptoml::parser parser{toml_ss};
        const auto config = parser.parse();

        return details::setup_impl(config);
    } catch (const setup_error &) {
        throw;
    } catch (const exception &e) {
        throw setup_error(e.what());
    }
}

template <class... Ps>
void from_file_and_override_with_tag_replacement(
    const std::string &base_pre_toml_path,
    const std::string &override_pre_toml_path,
    const Ps &... ps) {

    // std
    using std::exception;
    using std::forward;

    try {
        auto base_toml_ss = details::read_template_file_into_stringstream(
            base_pre_toml_path, ps...);

        cpptoml::parser base_parser{base_toml_ss};
        auto merged_config = base_parser.parse();

        auto override_toml_ss = details::read_template_file_into_stringstream(
            override_pre_toml_path, ps...);

        cpptoml::parser override_parser{override_toml_ss};
        const auto override_config = override_parser.parse();

        details::merge_config_root(merged_config, override_config);
        return details::setup_impl(merged_config);
    } catch (const setup_error &) {
        throw;
    } catch (const exception &e) {
        throw setup_error(e.what());
    }
}

inline void from_file(const std::string &toml_path) {
    // std
    using std::exception;
    using std::string;

    try {
        const auto config = cpptoml::parse_file(toml_path);
        return details::setup_impl(config);
    } catch (const exception &e) {
        throw setup_error(e.what());
    }
}

inline void from_file_and_override(
    const std::string &base_toml_path, const std::string &override_toml_path) {

    // std
    using std::exception;
    using std::string;

    try {
        // mutate base for efficiency
        // while const shared_ptr is mutable in interior, for clarity reason
        // non-const is used instead
        auto merged_config = cpptoml::parse_file(base_toml_path);
        const auto override_config = cpptoml::parse_file(override_toml_path);

        details::merge_config_root(merged_config, override_config);
        return details::setup_impl(merged_config);
    } catch (const exception &e) {
        throw setup_error(e.what());
    }
}

inline void save_logger_to_file(
    const std::shared_ptr<spdlog::logger> &logger,
    const std::string &toml_path,
    const bool overwrite) {

    using details::find_item_by_name;
    using details::level_to_str;
    using details::write_to_config_file;
    using details::names::LEVEL;
    using details::names::LOGGER_TABLE;
    using details::names::NAME;

    // fmt
    using fmt::format;

    // std
    using std::exception;
    using std::find_if;
    using std::shared_ptr;
    using std::string;

    try {
        const auto config = ([overwrite, &toml_path] {
            if (overwrite) {
                return cpptoml::make_table();
            } else {
                return cpptoml::parse_file(toml_path);
            }
        })();

        if (!config) {
            throw setup_error(
                format("Unable to parse file at '{}' for saving", toml_path));
        }

        auto &config_ref = *config;

        const auto curr_loggers = ([&config_ref] {
            const auto curr_loggers = config_ref.get_table_array(LOGGER_TABLE);

            if (curr_loggers) {
                return curr_loggers;
            } else {
                const auto new_loggers = cpptoml::make_table_array();
                config_ref.insert(LOGGER_TABLE, new_loggers);
                return new_loggers;
            }
        })();

        auto &curr_loggers_ref = *curr_loggers;

        const auto found_curr_logger =
            find_item_by_name(curr_loggers_ref, logger->name());

        const auto curr_logger =
            ([&found_curr_logger, &curr_loggers_ref, &logger] {
                if (found_curr_logger) {
                    return found_curr_logger;
                } else {
                    const auto new_curr_logger = cpptoml::make_table();
                    new_curr_logger->insert(NAME, logger->name());

                    curr_loggers_ref.insert(
                        curr_loggers_ref.end(), new_curr_logger);

                    return new_curr_logger;
                }
            }());

        // insert can overwrite the value
        auto &curr_logger_ref = *curr_logger;
        curr_logger_ref.insert(LEVEL, level_to_str(logger->level()));
        write_to_config_file(*config, toml_path);
    } catch (const setup_error &) {
        throw;
    } catch (const exception &e) {
        throw setup_error(e.what());
    }
}

inline auto delete_logger_in_file(
    const std::string &logger_name, const std::string &toml_path) -> bool {

    using details::find_item_iter_by_name;
    using details::level_to_str;
    using details::write_to_config_file;
    using details::names::LEVEL;
    using details::names::LOGGER_TABLE;
    using details::names::NAME;

    // fmt
    using fmt::format;

    // std
    using std::exception;
    using std::find_if;
    using std::shared_ptr;
    using std::string;

    try {
        const auto config = cpptoml::parse_file(toml_path);

        if (!config) {
            throw setup_error(format(
                "Unable to parse file at '{}' for deleting logger '{}'",
                toml_path,
                logger_name));
        }

        const auto &config_ref = *config;
        const auto curr_loggers = config_ref.get_table_array(LOGGER_TABLE);

        if (!curr_loggers) {
            throw setup_error(format(
                "Unable to find any logger table array for file at '{}'",
                toml_path));
        }

        auto &curr_loggers_ref = *curr_loggers;

        const auto found_curr_logger_it =
            find_item_iter_by_name(curr_loggers_ref, logger_name);

        curr_loggers_ref.erase(found_curr_logger_it);
        write_to_config_file(*config, toml_path);
    } catch (const setup_error &) {
        throw;
    } catch (const exception &e) {
        throw setup_error(e.what());
    }
}
} // namespace spdlog_setup
