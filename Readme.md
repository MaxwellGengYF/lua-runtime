# Build:
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
