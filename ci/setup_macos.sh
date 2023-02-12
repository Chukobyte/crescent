#!/bin/sh

# Current director
PROJECT_DIR=$(pwd)

BUILD_NUMBER=custom

# Version of packages that will be compiled by this meta-package
# PYTHON_VERSION is the full version number (e.g., 3.10.0b3)
# PYTHON_MICRO_VERSION is the full version number, without any alpha/beta/rc suffix. (e.g., 3.10.0)
# PYTHON_VER is the major/minor version (e.g., 3.10)
PYTHON_VERSION=3.10.5
PYTHON_MICRO_VERSION=$(echo $PYTHON_VERSION | grep -Po "\d+\.\d+\.\d+")
PYTHON_VER=$(echo $PYTHON_VERSION | awk -F . '{print $1 "." $2}')

ARCH=$(uname -m)

all() {
  echo "***** Building $PYTHON_VERSION $ARCH build $BUILD_NUMBER *****"
  rm -rf build
  echo "Docker build not supported on macOS. Please install the dependencies manually and run the following commands in the project directory:"
  echo "./configure --prefix=$PROJECT_DIR/build/python --enable-ipv6 --enable-shared --without-ensurepip"
  echo "make"
  echo "make install"
  echo "patchelf --set-rpath "\$\$ORIGIN/../lib" build/python/bin/python$PYTHON_VER"
  echo "patchelf --set-rpath "\$\$ORIGIN" build/python/lib/libpython3.so"
}

# Clean all builds
clean() {
  rm -rf build dist
}

# Full clean - includes all downloaded products
distclean() {
  clean
  rm -rf downloads
}

dependencies() {
  /bin/bash -c "$(curl -fsSL https://raw.githubusercontent.com/Homebrew/install/master/install.sh)"
  brew install dpkg -y
  dpkg -l > downloads/system.versions
}

downloads() {
  dependencies
  if [ ! -e downloads/Python-$PYTHON_VERSION.tgz ]; then \
    curl --fail -L https://www.python.org/ftp/python/$PYTHON_MICRO_VERSION/Python-$PYTHON_VERSION.tgz \
      -o downloads/Python-$PYTHON_VERSION.tgz; \
  fi
}

build() {
  mkdir build
}

dist() {
  mkdir dist
}

echo "***** Building $PYTHON_VERSION $ARCH build $BUILD_NUMBER *****"
downloads
mkdir -p downloads
tar zxf downloads/Python-$PYTHON_VERSION.tgz -C build
cd build/Python-$PYTHON_VERSION
./configure --prefix=$PROJECT_DIR/build/python --enable-ipv6 --enable-shared --without-ensurepip 2>&1 | tee -a ../python-$PYTHON_VERSION.
