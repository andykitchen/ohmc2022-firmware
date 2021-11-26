#!/bin/bash

set -e

SRC=tools/dot-vscode-example
RISCV_BIN=`which riscv64-unknown-elf-gcc`
RISCV_TOOLCHAIN_DIR=`dirname "$RISCV_BIN"`
VSCODE_PATH="$RISCV_TOOLCHAIN_DIR:\${workspaceFolder}/venv/bin:/usr/local/bin:/usr/bin:/bin"

mkdir .vscode
cp -v $SRC/settings.json .vscode/settings.json
jq ".configurations[].compilerPath=\"$RISCV_BIN\"" $SRC/c_cpp_properties.json > .vscode/c_cpp_properties.json
jq ".tasks[].options?.env?.PATH?=\"$VSCODE_PATH\"" $SRC/tasks.json > .vscode/tasks.json
jq ".configurations[].env?.PATH?=\"$VSCODE_PATH\"" $SRC/launch.json > .vscode/launch.json
