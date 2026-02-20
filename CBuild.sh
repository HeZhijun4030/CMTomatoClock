#!/bin/bash
cmake -S . -B build -G Ninja -DCMAKE_EXPORT_COMPILE_COMMANDS=ON --trace

cp build/compile_commands.json .
cmake --build build --verbose
echo ===============
echo CodeManStuduo Finished
echo the clock has been build at build/bin/
echo ===============
cd build
./bin/CMTomatoClock.cms
