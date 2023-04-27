#include <lua.hpp>
#include <math.h>
#include <string>
#ifdef _MSC_VER
#define LIBC_EXTERN extern "C" __declspec(dllexport)
#else
#define LIBC_EXTERN extern "C"
#endif

static int c_swap(lua_State* L) {
	//check and fetch the arguments
	double arg1 = luaL_checknumber(L, 1);
	double arg2 = luaL_checknumber(L, 2);

	//push the results
	lua_pushnumber(L, arg2);
	lua_pushnumber(L, arg1);

	//return number of results
	return 2;
}

static int my_sin(lua_State* L) {
	double arg = luaL_checknumber(L, 1);
	lua_pushnumber(L, sin(arg));
	return 1;
}

static int test_string(lua_State* L) {
	std::string str{luaL_checkstring(L, 1)};
	str += " from c function";
	lua_pushstring(L, str.c_str());
	return 1;
}

//library to be registered
static const luaL_Reg mylib[] = {
	{"c_swap", c_swap},
	{"mysin", my_sin}, /* names can be different */
	{"test_string", test_string},
	{nullptr, nullptr} /* sentinel */
};

//name of this function is not flexible
LIBC_EXTERN int luaopen_test_libc(lua_State* L) {
	luaL_newlib(L, mylib);
	return 1;
}
