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

cmake .

make

mkdir ../../Builds

mkdir ../../Builds/Flumen

cp Battle ../../Builds/Flumen --update

cp Shaders ../../Builds/Flumen --update --recursive

cp Textures ../../Builds/Flumen --update --recursive

cp Fonts ../../Builds/Flumen --update --recursive

cp Sounds ../../Builds/Flumen --update --recursive

if "$execute"; then
    ../../Builds/Flumen/Battle
fi