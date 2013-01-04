# Overview
qconnectlint is a [Clang tool](http://clang.llvm.org/docs/ClangTools.html) for verifying the consistency of [signal and slot](http://doc-snapshot.qt-project.org/4.8/signalsandslots.html) connections made with Qt 4's QObject::connect.

# Building
A recent compiler toolchain supporting C++11 is required. GCC 4.7 or Clang 3.1 should be sufficient. 

1. Download the source for [LLVM and Clang](http://clang.llvm.org) 3.2
2. Check out qconnectlint as `tools/clang/tools/qconnectlint`
3. Run `./configure --enable-cxx11` in the root LLVM directory
 * `--enable-libcpp` is required on Mac OS X due to the distributed libstdc++ not supporting C++11
 * `--disable-assertions --enable-optimized` is recommended for performance reasons. These flags are typically used when building a distribution's version of Clang.
 * `--prefix` can be used to install qconnectlint's copy of LLVM and Clang to a non-default location. 
4. Run the following from LLVM's root directory

 ```console
$ make install && cd tools/clang/tools/qconnectlint && make install
 ```

# Usage

If a [Clang compilation database](http://clang.llvm.org/docs/JSONCompilationDatabase.html) exists for the project `qconnectlint` can be invoked with a list of source files to lint. 

If no database is present the list of source files must be followed with a delimiting `--` and a list of compiler flags exactly as they would be passed to Clang. For example, the following woudl check the files `test1.cpp` and `test2.cpp` with some typical compiler flags:

```console
$ qconnectlint test1.cpp test2.cpp -- -std=c++11 -Wall -I/usr/include/qt4
```
