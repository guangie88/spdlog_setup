# `spdlog_setup`

## Overview
spdlog file configuration setup for convenience in initializing spdlog. Requires at least `CMake 3.3` + `GCC 6.0` for Linux, or `MSVC2015` with `MSBuild` for Windows to support c++14 features. Not suitable for serious production use.

[![Build Status](https://travis-ci.org/guangie88/spdlog_setup.svg?branch=master)](https://travis-ci.org/guangie88/spdlog_setup)
[![Build status](https://ci.appveyor.com/api/projects/status/srek5xih80104eds/branch/master?svg=true)](https://ci.appveyor.com/project/guangie88/spdlog-setup/branch/master)
[![codecov](https://codecov.io/gh/guangie88/spdlog_setup/branch/master/graph/badge.svg)](https://codecov.io/gh/guangie88/spdlog_setup)

## Repository Checkout
Since this repository has other git-based dependencies as `git` submodules, use the command:
`git clone --recursive https://github.com/guangie88/spdlog_setup.git`
in order to clone all the submodule dependencies.

If the repository has already been cloned without the submodules, then instead run:
`git submodule update --init --recursive`
in order to clone all the submodule dependencies.

## Dependencies
This repository uses the following dependencies:
- [`Catch`](https://github.com/philsquared/Catch)
- [`cpptoml`](https://github.com/skystrife/cpptoml)
- [`fmt`](https://github.com/fmtlib/fmt)
- [`rustfp`](https://github.com/guangie88/rustfp)
- [`spdlog`](https://github.com/gabime/spdlog)
- [`tag_fmt`](https://github.com/guangie88/tag_fmt)

## How to Build
This guide prefers a `CMake` out-of-source build style.

### Linux (`GCC`)
In the root directory after `git` cloning:
- `mkdir build`
- `cd build`
- `cmake ..`
- `make`

Now the unit test executable should be compiled and residing in `build/bin/unit_test`.

### Windows (`MSVC2015`)
Ensure that `MSBuild` can be located in `PATH`.

In the root directory after `git` cloning:
- `mkdir build`
- `cd build`
- `cmake .. -G "Visual Studio 14 Win64"`
- `msbuild /p:Configuration=Debug spdlog_setup.sln`

Now the unit test executable should be compiled and residing in `build/bin/Debug/unit_test.exe`.

## Use Example

### Static Configuration File

```c++
#include "spdlog_setup.h"

#include <string>

int main() {
    const auto res = ::spdlog_setup::from_file("log_conf.toml");

    res.match_err([](const std::string &err_msg) {
        // error parsing the TOML config file
        // as the library follows a error code mechanism approach
        // you may choose to throw an exception with err_msg if preferred
    });

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
    const auto res = ::spdlog_setup::from_file_with_tag_replacement("log_conf.pre.toml",
        // replaces {index} with actual value in current variable index
        std::make_pair("index", index),
        // replaces {path} with actual value in current variable path
        std::make_pair("path", path));

    res.match_err([](const std::string &err_msg) {
        // error actions
    });

    return 0;
}
```

## Currently Supported Sinks
- `stdout_sink_st`
- `stdout_sink_mt`
- `color_stdout_sink_st`
- `color_stdout_sink_mt`
- `simple_file_sink_st`
- `simple_file_sink_mt`
- `rotating_file_sink_st`
- `rotating_file_sink_mt`

## TOML Configuration Example

### Static File Configuration

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
type = "color_stdout_sink_mt"

[[sink]]
name = "rotate_out"
type = "rotating_file_sink_mt"
base_filename = "log/spdlog_setup.log"
max_size = "1M"
max_files = 10
level = "info"
# optional flag to indicate the set-up to create the log dir first
create_parent_dir = true

[[sink]]
name = "rotate_err"
type = "rotating_file_sink_mt"
base_filename = "log/spdlog_setup_err.log"
max_size = "1M"
max_files = 10
level = "err"
# to show that create_parent_dir is indeed optional (defaults to false)

[[logger]]
name = "root"
sinks = ["console", "rotate_out", "rotate_err"]
level = "trace"

# unused, for demonstrating that multiple loggers can be created
[[logger]]
name = "console"
sinks = ["console"]
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

## Notes
- Make sure that the directory for the log files to reside in exists before using `spdlog`.
- For the current set of unit tests, the working directory must be at the git root directory so that the `config` and `log` directories can be found.
