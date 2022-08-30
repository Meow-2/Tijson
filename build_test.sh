#!/bin/sh

git submodule update --init \
    && ./vcpkg/bootstrap-vcpkg.sh \
    && cmake -B build \
    && cmake --build build \
