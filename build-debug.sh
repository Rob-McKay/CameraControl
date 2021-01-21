#!/bin/zsh
setopt pushdsilent

script_path=${0:a:h}

if [ ! -d debug ]; then
    mkdir debug
fi

pushd debug
conan install .. --profile "$script_path/conan-profiles/macos-debug" --build=missing || exit $?
cmake -G "Unix Makefiles" -DCMAKE_BUILD_TYPE=Debug .. || exit $?
cmake --build . || exit $?
ctest -V|| exit $?

popd