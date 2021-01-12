#!/bin/zsh
setopt pushdsilent

script_path=${0:a:h}

if [ ! -d build ]; then
    mkdir build
fi

pushd build
conan install .. --profile "$script_path/conan-profiles/macos-debug" --build=missing
cmake -G "Unix Makefiles" -DCMAKE_BUILD_TYPE=Debug ..
cmake --build . --clean-first -v
# make
popd