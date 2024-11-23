#!/bin/bash

execute=false
debug=false

while getopts 'e' opt
do
    case $opt in
        e) execute=true;;
        d) debug=true;;
    esac
done

#export CC=/home/andrei/Documents/Code/Utilities/clang+llvm-17.0.6/bin/clang
#export CXX=/home/andrei/Documents/Code/Utilities/clang+llvm-17.0.6/bin/clang++

export CXX=/usr/bin/g++

#../../Utilities/cmake-3.28.0/bin/cmake . -DCMAKE_CXX_COMPILER=/home/andrei/Documents/Code/Utilities/clang+llvm-17.0.6/bin/clang++
../../Utilities/cmake-3.28.0/bin/cmake . -DCMAKE_CXX_COMPILER=/usr/bin/g++

make -j 4

mkdir -p ../../Builds

mkdir -p ../../Builds/Flumen

cp Battle ../../Builds/Flumen --update

cp Shaders ../../Builds/Flumen --update --recursive

cp Textures ../../Builds/Flumen --update --recursive

cp Fonts ../../Builds/Flumen --update --recursive

cp Sounds ../../Builds/Flumen --update --recursive

cp Config.json ../../Builds/Flumen --update --recursive

cp PartyPreset.json ../../Builds/Flumen --update --recursive

if "$execute"; then
    ../../Builds/Flumen/Battle
fi