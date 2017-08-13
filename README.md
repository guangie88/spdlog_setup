# `spdlog_setup`

## Overview
spdlog file configuration setup for convenience in initializing spdlog. Requires `GCC` 6.0 / `MSVC2015` and above to support c++14 features. For testing purposes only.

[![Build Status](https://travis-ci.org/guangie88/spdlog_setup.svg?branch=master)](https://travis-ci.org/guangie88/spdlog_setup)
[![codecov](https://codecov.io/gh/guangie88/spdlog_setup/branch/master/graph/badge.svg)](https://codecov.io/gh/guangie88/spdlog_setup)

## Use Example

```c++
#include "spdlog_setup.h"

#include <string>

int main() {
    const auto res = ::spdlog_setup::from_file("log_conf.toml");

    res.match_err([](const std::string &err_msg) {
        // error parsing the TOML config file
        // ...
    });

    return 0;
}
```

## TOML Configuration Example
Currently only works for `color_stdout_sink_mt` and `rotating_file_sink_mt` sink types.

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
base_filename = "../log/spdlog_setup.log"
max_size = "1M"
max_files = 10
level = "info"

[[sink]]
name = "rotate_err"
type = "rotating_file_sink_mt"
base_filename = "../log/spdlog_setup_err.log"
max_size = "1M"
max_files = 10
level = "err"

[[logger]]
name = "root"
sinks = ["console", "rotate_out", "rotate_err"]
level = "trace"

[[logger]]
name = "console"
sinks = ["console"]
```

## Notes
Make sure that the directory for the log files to reside in exists before using `spdlog`.
