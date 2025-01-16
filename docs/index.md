# MOTIS utl module documentation
MOTIS is an open-source software platform for efficient planning and routing in multi-modal transportation systems.
GitHub main repository: https://github.com/motis-project/motis

This is the documentation for the **utl** (utility) module.

```{toctree}
:maxdepth: 2
:caption: Contents:
```

## Logging
The simplest way to produce log lines is to use the `utl:log()` function,
or the wrapping functions for the various log levels,
`utl::debug()`, `utl::info()` and `utl::error()`:
```c++
#include "utl/logging.h"

utl::info("Simple message");
```

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
utl::info("String={} Int={}", "Hello", 42);
```

You can specify a **context** string:
```c++
utl::info("Message").ctx("http.get.resource");
```

You can specify **metadata**:
```c++
utl::info("Message").metadata("key", "value");
```

### API details
```{doxygenfunction} utl::log
```
