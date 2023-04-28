target("test_libc")
_config_project({
    project_kind = "shared"
})
add_files("test_libc.cpp")
add_deps("lua-lib")
target_end()

target("test_embeding")
_config_project({
    project_kind = "binary"
})
add_files("test_embeding.cpp")
add_deps("lua-lib")
target_end()