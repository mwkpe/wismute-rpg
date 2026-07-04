#!/bin/bash

set -e

if [ -z "$1" ]; then
  echo "Usage: $0 <build-dir-path>"
  exit 1
fi

if [ ! -d "$1" ]; then
  echo "Directory $1 does not exist"
  exit 1
fi

BUILD_DIR="$1"

rm -rf $BUILD_DIR/assets
rm -rf $BUILD_DIR/shader

mkdir -p $BUILD_DIR/assets
mkdir -p $BUILD_DIR/shader

cp -vu ../assets/*.png $BUILD_DIR/assets
cp -vu ../source/shader/*.vs $BUILD_DIR/shader
cp -vu ../source/shader/*.gs $BUILD_DIR/shader
cp -vu ../source/shader/*.fs $BUILD_DIR/shader
cp -vu ../apeiron/apeiron/opengl/shader/*.vs $BUILD_DIR/shader
cp -vu ../apeiron/apeiron/opengl/shader/*.fs $BUILD_DIR/shader
