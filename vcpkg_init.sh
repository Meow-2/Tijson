#!/bin/sh

# vcpkg export command
# vcpkg list | awk -F ':' '{print $1}' | head -n -2 >! .vcpkg_packages.txt
./vcpkg/bootstrap-vcpkg.sh \
    && ./vcpkg/vcpkg install "@.vcpkg_packages.txt"
