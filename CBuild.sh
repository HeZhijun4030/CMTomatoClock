#!/bin/bash
cmake -S . -B build -G Ninja -DCMAKE_EXPORT_COMPILE_COMMANDS=ON --trace
# 将编译数据库复制到项目根目录（clangd 默认会在当前目录查找）
cp build/compile_commands.json .
cmake --build build --verbose
echo ===============
echo CodeManStuduo Finished
echo ===============
cd build
./bin/MyProject
