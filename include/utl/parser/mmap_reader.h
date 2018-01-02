#pragma once

#ifdef _WIN32
#include "utl/parser/mmap_reader_msvc.h"
#else
#include "utl/parser/mmap_reader_unix.h"
#endif
