#!/bin/zsh
setopt pushdsilent

script_path=${0:a:h}

if [ ! -d release ]; then
    mkdir release
fi

pushd release
conan install .. --profile "$script_path/conan-profiles/macos-release" --build=missing || exit $?
cmake -G "Unix Makefiles" -DCMAKE_BUILD_TYPE=Release .. || exit $?
cmake --build . || exit $?
ctest || exit $?

popd
