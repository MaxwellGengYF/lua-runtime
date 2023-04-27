# README:
## Build:
xmake version later or equal than 2.7.8
run command to gen options.lua
```bash
xmake lua scripts/write_options.lua
```
different toolchain supported(default llvm):
```bash
xmake lua scripts/write_options.lua msvc
xmake lua scripts/write_options.lua gcc
```

config project:
```bash
xmake f -c
```

build project:
```bash
xmake
```

## Run test
* Copy tests/*.lua to binary's directory.
* run command to execute text directly:
```bash
lua test_xxx.lua
```
* compile and execute is also OK:
```bash
luac -o test_xxx.out test_xxx.lua
lua test_xxx.out
```