#!/bin/bash

INCLUDE_DIR=./include
BUILD_DIR=./build
TEST_DIR=./test

mkdir -p "$BUILD_DIR"

SRC_FILES=(
    src/environment_systems/buzzer.c
    src/environment_systems/air_quality.c
    src/environment_systems/temperature.c
    src/light_system/growing_light.c
    $TEST_DIR/buzzer_test.c
    $TEST_DIR/air_qual_test.c
    $TEST_DIR/light_test.c
    $TEST_DIR/temp_test.c
)

for src in "${SRC_FILES[@]}"; do
    obj="$BUILD_DIR/$(basename "${src%.*}").o"
    gcc -DSOFTWARE_DEBUG -DDEBUG -I "$INCLUDE_DIR" -c "$src" -o "$obj"
done

gcc -DSOFTWARE_DEBUG -DDEBUG "$TEST_DIR/test_all.c" -o "$BUILD_DIR/tests" \
    "$BUILD_DIR/air_qual_test.o" "$BUILD_DIR/growing_light.o" \
    "$BUILD_DIR/light_test.o" "$BUILD_DIR/temp_test.o" \
    "$BUILD_DIR/buzzer_test.o" "$BUILD_DIR/buzzer.o" \
    "$BUILD_DIR/temperature.o" "$BUILD_DIR/air_quality.o"

"$BUILD_DIR/tests"

rm -rf "$BUILD_DIR"