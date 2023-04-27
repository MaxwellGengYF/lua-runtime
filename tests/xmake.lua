target("test_libc")
_config_project({
    project_kind = "shared"
})
add_files("test_libc.cpp")
add_deps("lua-lib")
target_end()