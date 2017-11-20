#!/bin/bash

hdr=src/resources.hpp
src=src/resources.cpp

echo "#ifndef RESOURCES_HPP_" >> $hdr
echo "#define RESOURCES_HPP_" >> $hdr
echo >> $hdr

for file in "$@"
do
    echo $file

    tmp=${file%.cl}
    name=${tmp##*/}_source

    echo "extern const char $name[];" >> $hdr
    echo >> $hdr

    ./make_resource $name $file >> $src
done

echo "#endif // RESOURCES_HPP_" >> $hdr
