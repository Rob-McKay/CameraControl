#!/bin/zsh
setopt pushdsilent

script_path=${0:a:h}

if [ ! -d build-rel ]; then
    mkdir build-rel
fi

pushd build-rel
conan install .. --profile "$script_path/conan-profiles/macos-release" --build=missing || exit $?
cmake -G "Unix Makefiles" -DCMAKE_BUILD_TYPE=Release .. || exit $?
cmake --build .
# make
popd
