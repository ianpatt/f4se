![testbuild](https://github.com/ianpatt/f4se/workflows/testbuild/badge.svg)

## Building

```
git clone https://github.com/ianpatt/common
git clone https://github.com/ianpatt/f4se
cmake -B common/build -S common -DCMAKE_INSTALL_PREFIX=extern common
cmake --build common/build --config Release --target install
cmake -B f4se/build -S f4se -DCMAKE_INSTALL_PREFIX=extern f4se
cmake --build f4se/build --config Release
```
Solution will be generated at f4se/build/umbrella.sln.
