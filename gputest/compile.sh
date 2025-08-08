#!/bin/bash

 export PATH=/usr/local/google/home/aumrao/Android/Sdk/ndk/29.0.13846066/toolchains/llvm/prebuilt/linux-x86_64/bin/:$PATH

 make 





#NDK = /usr/local/google/home/aumrao/Android/Sdk/ndk/29.0.13846066/
#export TOOLCHAIN=$NDK/toolchains/llvm/prebuilt/darwin-x86_64
#export TOOLCHAIN=$NDK/toolchains/llvm/prebuilt/linux-x86_64
#export TARGET=aarch64-linux-android
#export API=21
# Configure and build.
#export AR=$TOOLCHAIN/bin/llvm-ar
#export CC="$TOOLCHAIN/bin/clang --target=$TARGET$API"
#export AS=$CC
#export CXX="$TOOLCHAIN/bin/clang++ --target=$TARGET$API"
#export LD=$TOOLCHAIN/bin/ld
#export RANLIB=$TOOLCHAIN/bin/llvm-ranlib
#export STRIP=$TOOLCHAIN/bin/llvm-strip