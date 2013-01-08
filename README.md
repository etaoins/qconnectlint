# Overview
qconnectlint is a [Clang tool](http://clang.llvm.org/docs/ClangTools.html) for statically verifying the consistency of [signal and slot](http://doc-snapshot.qt-project.org/4.8/signalsandslots.html) connections made with Qt's `QObject::connect`. It can detect the following problems:

* Signals or slots not existing with the expected signature
* Incompatible signal and slot signatures due to types or argument count
* Slots being used as signals 

# Building
A recent compiler toolchain supporting C++11 is required. GCC 4.7 or Clang 3.1 should be sufficient. 

1. Download the source for [LLVM and Clang](http://clang.llvm.org/get_started.html) 3.2
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

If no database is present the list of source files must be followed with a delimiting `--` and a list of compiler flags exactly as they would be passed to Clang. For example, the following would check the files `test1.cpp` and `test2.cpp` with some typical compiler flags:

```console
$ qconnectlint test1.cpp test2.cpp -- -std=c++11 -Wall -I/usr/include/qt4
```

qconnectlint's output mimics Clang's format. This means tools intended to parse the output of Clang should easily handle lint reports. An example Vim compiler plugin is included in the `vim` directory.

# Limitations
qconnectlint is limited to static analysis using information available at compile time. This has a number of consequences:

* Signals and slots must be referenced using inline `SIGNAL()` and `SLOT()` macros when calling `connect()`. For example, passing a signal/slot to a function that indirectly calls `connect()` itself won't be checked
* Passing a derived class pointer upcast to a base class (typically QObject*) will falsely trigger warnings if the base class doesn't define the referenced signal/slot.
* Signal and slot annotations are lost at the preprocessor stage. As a result qconnectlint can't determine if a given class member has been defined as a signal or slot; it simply checks for the existence of a member with a matching signature.

