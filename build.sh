#!/bin/sh

# vcpkg export command
# vcpkg list | awk -F ':' '{print $1}' | head -n -2 >! .vcpkg_packages.txt
git submodule update --init \
    && ./vcpkg/bootstrap-vcpkg.sh \
    && cmake -B build -GNinja -DCMAKE_BUILD_TYPE=Debug -DCMAKE_EXPORT_COMPILE_COMMANDS=Enable \
    && cmake --build build -j16
