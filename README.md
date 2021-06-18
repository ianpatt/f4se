![testbuild](https://github.com/ianpatt/f4se/workflows/testbuild/badge.svg)

## Building

```
git clone https://github.com/ianpatt/common
git clone https://github.com/ianpatt/f4se
cmake -B common/build -S common -DCMAKE_INSTALL_PREFIX=extern -DCMAKE_BUILD_TYPE=Release common
cmake --build common/build --target install
cmake -B f4se/build -S f4se -DCMAKE_INSTALL_PREFIX=extern -DCMAKE_BUILD_TYPE=Release -DBUILD_SHARED_LIBS=ON f4se
cmake --build f4se/build
```

## Build Options

* `BUILD_SHARED_LIBS`: Set to `ON` to build f4se as a dll, or `OFF` to build is as a static lib. See [cmake documentation](https://cmake.org/cmake/help/latest/variable/BUILD_SHARED_LIBS.html) for more info.
* `CMAKE_MSVC_RUNTIME_LIBRARY`: See [cmake documentation](https://cmake.org/cmake/help/latest/variable/CMAKE_MSVC_RUNTIME_LIBRARY.html) for more info.
* `F4SE_COPY_OUTPUT`: Set to `ON` to copy built files to the Fallout 4 directory as a post-build step.
* `Fallout4Path`: Set to the directory containing `Fallout4.exe`. Needed for `F4SE_COPY_OUTPUT` / papyrus compilation targets.
