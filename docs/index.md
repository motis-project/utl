# MOTIS utl module documentation
MOTIS is an open-source software platform for efficient planning and routing in multi-modal transportation systems.
GitHub main repository: https://github.com/motis-project/motis

This is the documentation for the **utl** (utility) module.

:::{toctree}
:maxdepth: 2
:caption: Contents:
:::

## Logging
The simplest way to produce log lines is to use the `utl:log()` function,
or the wrapping functions for the various log levels,
`utl::log_debug()`, `utl::log_info()` and `utl::log_error()`:
```c++
#include "utl/logging.h"

utl::log_info("MyCtx", "Simple message");
```

The first parameter is the **context**, that provides an information of the origin of the log line inside MOTIS code.

The following log levels are supported:

debug
: Messages that contain information only useful when debugging MOTIS

info
: Important information about a normal behavior of the program

error
: Details on an abnormal behavior of the application

### Advanced usage
You can insert variables in the message by using `{}` and passing them as extra arguments
(formatting is performed by the [fmt](https://fmt.dev>) library):
```c++
utl::log_info("MyCtx", "String={} Int={}", "Hello", 42);
```

You can specify **metadata** using `.attrs()`:
```c++
utl::log_info("MyCtx", "Message").attrs({{"key1", "value1"}, {"key2", "value2"}});
```

### API details
:::{doxygenstruct} utl::log
:no-link:
:members:
:::

:::{doxygenstruct} utl::log_debug
:no-link:
:members:
:::

:::{doxygenstruct} utl::log_info
:no-link:
:members:
:::

:::{doxygenstruct} utl::log_error
:no-link:
:members:
:::

:::{note}
Those logging function are an exception to the rule that, in MOTIS,
we use [Aggregate Initialization](https://en.cppreference.com/w/cpp/language/aggregate_initialization) wherever possible,
but here we do not want to use `utl::log_info{...}`.
:::
