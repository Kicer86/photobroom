
#!/bin/bash

if [ $# != 1 ]; then
    echo "Run script with new version as argument."
    echo "Example: $0 2.3.4"
    exit 1
fi

parent_dir=$(dirname $0)/..
top_dir=$(readlink -f $parent_dir)
version=$1

sed -i -e "s/project(PhotoBroom VERSION .*)/project(PhotoBroom VERSION $version)/" $top_dir/CMakeLists.txt
sed -i -e "s/version: .*{build}/version: $version.{build}/" $top_dir/appveyor.yml
sed -i -e "s/path: build\/PhotoBroom-.*exe/path: build\/PhotoBroom-$version-*.exe/" $top_dir/appveyor.yml
