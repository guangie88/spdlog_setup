# Changelog

## Unreleased

## v0.3.2

- Add support for stderr sinks
  ([#63](https://github.com/guangie88/spdlog_setup/pull/63))
- Add support for setting logging flush level
  ([#62](https://github.com/guangie88/spdlog_setup/pull/62))

## v0.3.1

- Add option for CMake to target and link only `spdlog` headers
  ([#61](https://github.com/guangie88/spdlog_setup/pull/61/))
  - Fix [#60](https://github.com/guangie88/spdlog_setup/issues/60)
- Add Windows `msvc_sink` support ([#58](https://github.com/guangie88/spdlog_setup/pull/58))
- Fix loop not using const-ref to resolve compile errors on Xcode 12
  ([#56](https://github.com/guangie88/spdlog_setup/pull/56))
- Add option for CMake to disable generation of install target
  ([#51](https://github.com/guangie88/spdlog_setup/pull/51))

## v0.3.0

- Add support for async logger. This fixes
  ([#44](https://github.com/guangie88/spdlog_setup/issues/44))
  - This makes logger accepts `type`, which takes in `sync` or `async`.
    `sync` is default. Refer to example in [README](README.md) for more details.
- Change support to `spdlog` `v1.y.z` tag release, currently tested all `v1.0.0`
  to `v1.3.1` to be working. This fixes
  ([#26](https://github.com/guangie88/spdlog_setup/issues/26)).
  - `simple_file_sink_st` becomes `basic_file_sink_st`
  - `simple_file_sink_mt` becomes `basic_file_sink_mt`
  - `syslog_sink` splits into `syslog_sink_st` and `syslog_sink_mt`
- Fix CMake package finding failure when installed using submodule `spdlog`
  ([#23](https://github.com/guangie88/spdlog_setup/issues/23)).
- Fix global pattern not used by subsequent loggers other than the first
  ([#19](https://github.com/guangie88/spdlog_setup/issues/19)).
- Add functions that allow configuration overrides:
  - `from_file_and_override`
  - `from_file_and_override_with_tag_replacement`
  - `save_logger_to_file`
  - `delete_logger_in_file`

## v0.2.0 (Tested to support `v0.16.z` to `v0.17.0`)

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

## v0.1.0

- Basic implementation that reads from TOML configuration for set-up.
