# Changelog

## Unreleased

- Add functions that allow configuration overrides:
  - `from_file_and_override`
  - `from_file_and_override_with_tag_replacement`
  - `save_logger_to_file`
  - `delete_logger_in_file`
- Fix CMake package finding failure when installed using submodule `spdlog`
  ([#23](https://github.com/guangie88/spdlog_setup/issues/23))
- Fix global pattern not used by subsequent loggers other than the first
  ([#19](https://github.com/guangie88/spdlog_setup/issues/19)).

## v0.2

- Use `spdlog_setup/conf.h` instead of `spdlog_setup.h`. Reason for change is
  to allow inlining of `cpptoml` and `fmt` libraries.
- No changes to the `TOML` configuration format.
- Switch back to exception based strategy for handling error.
  `spdlog_setup::setup_error` exception contains error messages similar to
  `v0.1`. Rationale of change is to align to C++ idiomatic approach of using
  exception of handling error.
- Change to exception based handling means a change of API, so check out the
  [examples](./README.md#use-examples) to observe the changes.
- Strip down many `git` submodules, leaving only `Catch2` (unit-test) and
  `spdlog` only, which is much better for installation. For `spdlog`, it is
  replaceable and likely to work if replaced with custom and newer versions,
  unless there are major API changes.

## v0.1

- Basic implementation that reads from TOML configuration for set-up.
