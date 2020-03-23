#!/bin/sh

find test include -type f -not -name catch.hpp -a \( -name "*.cc" -o -name "*.h" \) -print0 | xargs -0 clang-format-9 -i
git status --porcelain
git status --porcelain | xargs -I {} -0 test -z \"{}\"
