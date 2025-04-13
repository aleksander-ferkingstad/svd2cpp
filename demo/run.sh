#!/bin/bash

# Validate input
if [[ $# -ne 1 ]] || [[ ! $1 =~ ^(debug|release|size)$ ]]; then
    echo "Usage: $0 <build_type>"
    echo "Valid types: debug, release, size"
    exit 1
fi

# Configure build parameters
case $1 in
    debug)
        meson_flags="-Dstrip=false -Doptimization=g -Db_ndebug=false"
        ;;
    release)
        meson_flags="-Dstrip=true -Doptimization=s -Db_ndebug=true"
        ;;
    size)
        meson_flags="-Dstrip=false -Doptimization=s -Db_ndebug=false -Ddebug=true"
        ;;
esac

# Execute build
meson setup build --cross-file cross/stm32f767zi.ini $meson_flags --reconfigure --wipe
ninja -C build
arm-none-eabi-objcopy -O ihex build/demo.elf build/demo.hex
st-flash --reset --format=ihex write build/demo.hex
