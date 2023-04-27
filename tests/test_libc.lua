local libc = require("test_libc")
print("input: 3, 6")
local a, b = libc.c_swap(3,6)
print("output: " .. tostring(a) .. ", " .. tostring(b))
print("sin(0.5): " .. tostring(libc.mysin(0.5)))
local str = "hello world"
print('Origin string: "' .. str .. '"')
str = libc.test_string(str)
print('New string: "' .. str .. '"')