#!/usr/bin/env bash
HOST=$1

TOOLCHAIN=$(pwd)/../mips-gcc472-glibc216-64bit/bin
CROSS_COMPILE=$TOOLCHAIN/mips-linux-gnu-
export CROSS_COMPILE=${CROSS_COMPILE}
export CC=${CROSS_COMPILE}gcc
export CPLUSPLUS=${CROSS_COMPILE}gcc
export LD=${CROSS_COMPILE}ld
export CFLAGS="-muclibc -O2 -DDEBUG_TRACE -DFAKE_ROOT "
export CPPFLAGS="-muclibc -O2"
export LDFLAGS="-muclibc -O2"
make clean
make

if [[ ! -z "${HOST}" && $? == 0 ]]; then
  echo "Uploading setconf to ${HOST}"
  ftp-upload -h ${HOST} -u root --password ismart12 -d /system/sdcard/bin/ setconf
  echo "Uploading getimage to ${HOST}"
  ftp-upload -h ${HOST} -u root --password ismart12 -d /system/sdcard/bin/ getimage
fi 
