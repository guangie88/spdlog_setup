# `spdlog_setup`

## Overview
[`spdlog`](https://github.com/gabime/spdlog) file configuration setup for convenience in initializing spdlog. Inspired by [`spdlog-config`](https://github.com/objectx/spdlog-config) for using [`TOML`](https://github.com/toml-lang/toml) configuration, a format that is simple and easy-to-read. Also heavily influenced by [`Rust`](https://www.rust-lang.org) language, leaning towards monadic return type style instead of throwing exceptions. Not battle-tested for production use yet.

[![Build Status](https://travis-ci.org/guangie88/spdlog_setup.svg?branch=master)](https://travis-ci.org/guangie88/spdlog_setup)
[![Build status](https://ci.appveyor.com/api/projects/status/srek5xih80104eds/branch/master?svg=true)](https://ci.appveyor.com/project/guangie88/spdlog-setup/branch/master)
[![codecov](https://codecov.io/gh/guangie88/spdlog_setup/branch/master/graph/badge.svg)](https://codecov.io/gh/guangie88/spdlog_setup)

## Requirements
Requires at least `CMake 3.6`, `GCC 5.0` for Linux, or `MSVC2015` with `MSBuild` for Windows to support C++14 features.

## Features
- Initialization of `spdlog` sinks and loggers based on `TOML` configuration file.
- Tag replacement (e.g. "{tagname}-log.txt") within the `TOML` configuration file.
- Does not throw exception (unless `std::string` constructor throws `bad_alloc` for now).

## Repository Checkout
Since this repository has other git-based dependencies as `git` submodules, use the command:
`git clone --recursive https://github.com/guangie88/spdlog_setup.git`
in order to clone all the submodule dependencies.

If the repository has already been cloned without the submodules, then instead run:
`git submodule update --init --recursive`
in order to clone all the submodule dependencies.

## Dependencies
This repository uses the following dependencies directly:
- [`Catch`](https://github.com/philsquared/Catch)
- [`cpptoml`](https://github.com/skystrife/cpptoml)
- [`spdlog`](https://github.com/gabime/spdlog)
- [`tag_fmt`](https://github.com/guangie88/tag_fmt)

## How to Build
This guide prefers a `CMake` out-of-source build style. For build with unit tests, add `-DSPDLOG_SETUP_INCLUDE_UNIT_TESTS=ON` during the CMake configuration.

## How to Install
The installation step of `CMake` copies out the entire list of header files required for `spdlog_setup` into the installation directory. To change the installation directory, add `-DCMAKE_INSTALL_PREFIX:PATH=install` during the CMake configuration.

### Linux (`GCC`)
In the root directory after `git` cloning:

#### Debug without Installation
- `mkdir build-debug`
- `cd build-debug`
- `cmake .. -DCMAKE_BUILD_TYPE=Debug -DSPDLOG_SETUP_INCLUDE_UNIT_TESTS=ON`
- `cmake --build .`

Now the unit test executable should be compiled and residing in `build-debug/spdlog_setup_unit_test`.

#### Release with Installation
- `mkdir build-release`
- `cd build-release`
- `cmake .. -DCMAKE_BUILD_TYPE=Release -DSPDLOG_SETUP_INCLUDE_UNIT_TESTS=ON -DCMAKE_INSTALL_PREFIX:PATH=install`
- `cmake --build . --target install`

Now the unit test executable should be compiled and residing in `build-release/spdlog_setup_unit_test`.

The header files should be installed in `build-release/install/include`.

### Windows (`MSVC2015`)
Ensure that [`Microsoft Build Tools 2015`](https://www.microsoft.com/en-sg/download/details.aspx?id=48159) and [`Visual C++ Build Tools 2015`](http://landinghub.visualstudio.com/visual-cpp-build-tools) (or `Visual Studio 2015`) have been installed.

In the root directory after `git` cloning:

- `mkdir build`
- `cd build`
- `cmake .. -G "Visual Studio 14 Win64" -DSPDLOG_SETUP_INCLUDE_UNIT_TESTS=ON -DCMAKE_INSTALL_PREFIX:PATH=install`
- (Debug) `cmake --build . --config Debug`
- (Release with installation) `cmake --build . --config Release --target install`

Now the unit test executable should be compiled and residing in
- (Debug) `build/Debug/spdlog_setup_unit_test.exe` or
- (Release) `build/Release/spdlog_setup_unit_test.exe`.

The header files should be installed in `build/install/include`.

## Supported Sinks
- `stdout_sink_st`
- `stdout_sink_mt`
- `color_stdout_sink_st`
- `color_stdout_sink_mt`
- `simple_file_sink_st`
- `simple_file_sink_mt`
- `rotating_file_sink_st`
- `rotating_file_sink_mt`
- `daily_file_sink_st`
- `daily_file_sink_mt`
- `null_sink_st`
- `null_sink_mt`
- `syslog_sink` (only for Linux, `SPDLOG_ENABLE_SYSLOG` preprocessor definition must be defined before any `spdlog`/`spdlog_setup` header is included)

Currently `ostream_sink` and `dist_sink` do not fit into the use case and are not supported.

For more information about how the above sinks work in `spdlog`, please refer to the original `spdlog` sinks wiki page at: [https://github.com/gabime/spdlog/wiki/4.-Sinks](https://github.com/gabime/spdlog/wiki/4.-Sinks).

## TOML Configuration Example

### Static File Configuration

```toml
# level is optional for both sinks and loggers
# level for error logging is 'err', not 'error'
# _st => single threaded, _mt => multi threaded
# syslog_sink is automatically thread-safe by default, no need for _mt suffix

# max_size supports suffix
# - T (terabyte)
# - G (gigabyte)
# - M (megabyte)
# - K (kilobyte)
# - or simply no suffix (byte)

[[sink]]
name = "console_st"
type = "stdout_sink_st"

[[sink]]
name = "console_mt"
type = "stdout_sink_mt"

[[sink]]
name = "color_console_st"
type = "color_stdout_sink_st"

[[sink]]
name = "color_console_mt"
type = "color_stdout_sink_mt"

[[sink]]
name = "file_out"
type = "simple_file_sink_st"
filename = "log/spdlog_setup.log"
# truncate field is optional
# truncate = false (default)
level = "info"
# optional flag to indicate the set-up to create the log dir first
create_parent_dir = true

[[sink]]
name = "file_err"
type = "simple_file_sink_mt"
filename = "log/spdlog_setup_err.log"
truncate = true
level = "err"
# to show that create_parent_dir is indeed optional (defaults to false)

[[sink]]
name = "rotate_out"
type = "rotating_file_sink_st"
base_filename = "log/rotate_spdlog_setup.log"
max_size = "1M"
max_files = 10
level = "info"

[[sink]]
name = "rotate_err"
type = "rotating_file_sink_mt"
base_filename = "log/rotate_spdlog_setup_err.log"
max_size = "1M"
max_files = 10
level = "err"

[[sink]]
name = "daily_out"
type = "daily_file_sink_st"
base_filename = "log/daily_spdlog_setup.log"
rotation_hour = 17
rotation_minute = 30
level = "info"

[[sink]]
name = "daily_err"
type = "daily_file_sink_mt"
base_filename = "log/daily_spdlog_setup_err.log"
rotation_hour = 17
rotation_minute = 30
level = "err"

[[sink]]
name = "null_sink_st"
type = "null_sink_st"

[[sink]]
name = "null_sink_mt"
type = "null_sink_mt"

# only works for Linux
[[sink]]
name = "syslog"
type = "syslog_sink"
# generally no need to fill up the optional fields below
# ident = "" (default)
# syslog_option = 0 (default)
# syslog_facility = LOG_USER (default macro value)

[[logger]]
name = "root"
sinks = [
    "console_st", "console_mt",
    "color_console_st", "color_console_mt",
    "daily_out", "daily_err",
    "file_out", "file_err",
    "rotate_out", "rotate_err",
    "null_sink_st", "null_sink_mt",
    "syslog"]
level = "trace"

# unused, for demonstrating that multiple loggers can be created
[[logger]]
name = "console"
sinks = ["console_st", "console_mt"]
```

### Tagged-Base Pre-TOML File Configuration

```toml
# level is optional for both sinks and loggers
# level for error logging is 'err', not 'error'

# max_size supports suffix
# - T (terabyte)
# - G (gigabyte)
# - M (megabyte)
# - K (kilobyte)
# - or simply no suffix (byte)

[[sink]]
name = "console"
type = "stdout_sink_mt"

[[sink]]
name = "rotate_out"
type = "rotating_file_sink_mt"
base_filename = "log/{index}-info/simple-{path}.log"
max_size = "1M"
max_files = 10
level = "info"
# optional flag to indicate the set-up to create the log dir first
create_parent_dir = true

[[sink]]
name = "simple_err"
type = "simple_file_sink_mt"
filename = "log/{index}-err/simple-{path}.log"
truncate = false
level = "err"
# optional flag to indicate the set-up to create the log dir first
create_parent_dir = true

[[logger]]
name = "root"
sinks = ["console", "rotate_out", "simple_err"]
level = "trace"
```

## Use Example

### Static Configuration File

```c++
#include "spdlog_setup.h"

#include <iostream>
#include <string>

int main() {
    // generally no exception will be thrown unless std::bad_alloc due to std::string
    const auto res = spdlog_setup::from_file("log_conf.toml");

    // if-else syntax, has advantage of being able to perform early return out of main
    if (res.is_err()) {
        std::cerr << res.get_err_unchecked() << '\n';
        return 127;
    }

    // (alternative) Rust-like match (error) syntax
    // able to type check for error safely but unable to perform early return out of main
    res.match_err([](const std::string &err_msg) {
        // error parsing the TOML config file
        // you may choose to throw an exception with err_msg if preferred
    });

    // assumes that root logger has been initialized
    auto logger = spdlog::get("root");
    logger->trace("trace message");
    logger->debug("debug message");
    logger->info("info message");
    logger->warn("warn message");
    logger->error("error message");
    logger->critical("critical message");

    return 0;
}
```

### Tagged Based Configuration File

```c++
#include "spdlog_setup.h"

#include <string>
#include <utility>

int main(const int argc, const char * argv[]) {
    // assumes both index and path are given by command line arguments

    // gets index integer, e.g. 123
    const auto index = std::stoi(argv[1]);

    // gets path string, e.g. a/b/c
    const auto path = std::string(argv[2]);

    // performs parsing with dynamic tag value replacements
    // tags are anything content that contains {xxx}, where xxx is the name of the tag
    // to be replaced
    const auto res = spdlog_setup::from_file_with_tag_replacement("log_conf.pre.toml",
        // replaces {index} with actual value in current variable index
        std::make_pair("index", index),
        // replaces {path} with actual value in current variable path
        std::make_pair("path", path));

    // Rust-like match expression syntax
    // able to type check for success and error safely
    // but unable to perform early return because of closure usage
    const auto value = res.match([](auto) {
        // success case
        // assumes that root logger has been initialized
        auto logger = spdlog::get("root");

        // ...
        return 0;

    }, [](const std::string &err_msg) {
        // error case
        // return value type must be similar to the success case
        return 127;
    });

    // (alternative) if-else syntax
    if (res.is_ok()) {
        // success case
        // ...

        return 0;
    } else {
        return 127;
    }
}
```

## Notes
- Make sure that the directory for the log files to reside in exists before using `spdlog`, unless the `create_parent_dir` flag is set to true for the sink.
- For the current set of unit tests, the working directory must be at the git root directory or in `build` directory so that the TOML configuration files in `config` directory can be found.
