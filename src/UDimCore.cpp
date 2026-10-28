#include "UDimCore.h"
#include "lua.h"
#include "lualib.h"

#include <cstdio>
#include <cstring>
#include <new>

namespace Luwow::UDims {
    static constexpr const char* LockedMessage = "The metatable is locked";

    // Formatting

    static std::string formatNumber(float value) {
        if (value == 0) value = 0; // -0 prints as 0
        char buffer[32];
        snprintf(buffer, sizeof(buffer), "%g", static_cast<double>(value));
        return buffer;
    }

    std::string formatUDim(const UDim& udim) {
        return formatNumber(udim.scale) + ", " + formatNumber(udim.offset);
    }

    std::string formatUDim2(const UDim2& udim2) {
        return "{" + formatUDim(udim2.x) + "}, {" + formatUDim(udim2.y) + "}";
    }

    // Luau values

    bool isUDim(lua_State* L, int index) {
        return lua_touserdatatagged(L, index, UDimTag) != nullptr;
    }

    bool isUDim2(lua_State* L, int index) {
        return lua_touserdatatagged(L, index, UDim2Tag) != nullptr;
    }

    UDim checkUDim(lua_State* L, int index) {
        auto* udim = static_cast<UDim*>(lua_touserdatatagged(L, index, UDimTag));
        if (!udim) luaL_typeerror(L, index, "UDim");
        return *udim;
    }

    UDim2 checkUDim2(lua_State* L, int index) {
        auto* udim2 = static_cast<UDim2*>(lua_touserdatatagged(L, index, UDim2Tag));
        if (!udim2) luaL_typeerror(L, index, "UDim2");
        return *udim2;
    }

    void pushUDim(lua_State* L, const UDim& udim) {
        new (lua_newuserdatataggedwithmetatable(L, sizeof(UDim), UDimTag)) UDim(udim);
    }

    void pushUDim2(lua_State* L, const UDim2& udim2) {
        new (lua_newuserdatataggedwithmetatable(L, sizeof(UDim2), UDim2Tag)) UDim2(udim2);
    }

    UDim checkUDimLike(lua_State* L, int index) {
        if (isUDim(L, index)) return checkUDim(L, index);
        if (lua_type(L, index) == LUA_TNUMBER) return UDim{ 0, static_cast<float>(lua_tonumber(L, index)) };
        luaL_typeerror(L, index, "UDim or number");
    }

    UDim2 checkUDim2Like(lua_State* L, int index) {
        if (isUDim2(L, index)) return checkUDim2(L, index);
        if (lua_isvector(L, index)) {
            const float* v = lua_tovector(L, index);
            return UDim2::fromOffset(v[0], v[1]);
        }
        luaL_typeerror(L, index, "UDim2 or vector");
    }

    // Arithmetic shared by both types: T + T, T - T, -T, T * number, number * T, T / number

    template <typename T, T (*Check)(lua_State*, int), void (*Push)(lua_State*, const T&)>
    struct Arithmetic {
        static int add(lua_State* L) { Push(L, Check(L, 1) + Check(L, 2)); return 1; }
        static int sub(lua_State* L) { Push(L, Check(L, 1) - Check(L, 2)); return 1; }
        static int unm(lua_State* L) { Push(L, -Check(L, 1)); return 1; }

        static int mul(lua_State* L) {
            if (lua_type(L, 1) == LUA_TNUMBER) Push(L, Check(L, 2) * static_cast<float>(lua_tonumber(L, 1)));
            else Push(L, Check(L, 1) * static_cast<float>(luaL_checknumber(L, 2)));
            return 1;
        }

        static int div(lua_State* L) {
            float divisor = static_cast<float>(luaL_checknumber(L, 2));
            if (divisor == 0) luaL_error(L, "division by zero");
            Push(L, Check(L, 1) / divisor);
            return 1;
        }

        static int eq(lua_State* L) {
            lua_pushboolean(L, Check(L, 1) == Check(L, 2));
            return 1;
        }
    };

    // UDim

    // udim:Resolve(length) -> number, the length in pixels within a parent of the given length
    static int udim_resolve(lua_State* L) {
        lua_pushnumber(L, checkUDim(L, 1).resolve(static_cast<float>(luaL_checknumber(L, 2))));
        return 1;
    }

    static lua_CFunction findUDimMethod(const char* name) {
        if (std::strcmp(name, "Resolve") == 0) return udim_resolve;
        return nullptr;
    }

    static int udim_index(lua_State* L) {
        UDim udim = checkUDim(L, 1);
        const char* key = luaL_checkstring(L, 2);

        if (std::strcmp(key, "Scale") == 0) lua_pushnumber(L, udim.scale);
        else if (std::strcmp(key, "Offset") == 0) lua_pushnumber(L, udim.offset);
        else if (lua_CFunction method = findUDimMethod(key)) lua_pushcfunction(L, method, key);
        else luaL_error(L, "%s is not a valid member of UDim", key);
        return 1;
    }

    static int udim_namecall(lua_State* L) {
        const char* name = lua_namecallatom(L, nullptr);
        lua_CFunction method = name ? findUDimMethod(name) : nullptr;
        if (!method) luaL_error(L, "%s is not a valid member of UDim", name ? name : "?");
        return method(L);
    }

    static int udim_tostring(lua_State* L) {
        lua_pushstring(L, formatUDim(checkUDim(L, 1)).c_str());
        return 1;
    }

    // UDim2

    // udim2:Lerp(goal, alpha) -> UDim2
    static int udim2_lerp(lua_State* L) {
        pushUDim2(L, checkUDim2(L, 1).lerp(checkUDim2(L, 2), static_cast<float>(luaL_checknumber(L, 3))));
        return 1;
    }

    // udim2:Resolve(size: vector) -> vector, the pixels within a parent of the given size
    static int udim2_resolve(lua_State* L) {
        UDim2 udim2 = checkUDim2(L, 1);
        const float* size = luaL_checkvector(L, 2);
        lua_pushvector(L, udim2.x.resolve(size[0]), udim2.y.resolve(size[1]), 0.0f);
        return 1;
    }

    static lua_CFunction findUDim2Method(const char* name) {
        if (std::strcmp(name, "Lerp") == 0) return udim2_lerp;
        if (std::strcmp(name, "Resolve") == 0) return udim2_resolve;
        return nullptr;
    }

    static int udim2_index(lua_State* L) {
        UDim2 udim2 = checkUDim2(L, 1);
        const char* key = luaL_checkstring(L, 2);

        if (std::strcmp(key, "X") == 0 || std::strcmp(key, "Width") == 0) pushUDim(L, udim2.x);
        else if (std::strcmp(key, "Y") == 0 || std::strcmp(key, "Height") == 0) pushUDim(L, udim2.y);
        else if (lua_CFunction method = findUDim2Method(key)) lua_pushcfunction(L, method, key);
        else luaL_error(L, "%s is not a valid member of UDim2", key);
        return 1;
    }

    static int udim2_namecall(lua_State* L) {
        const char* name = lua_namecallatom(L, nullptr);
        lua_CFunction method = name ? findUDim2Method(name) : nullptr;
        if (!method) luaL_error(L, "%s is not a valid member of UDim2", name ? name : "?");
        return method(L);
    }

    static int udim2_tostring(lua_State* L) {
        lua_pushstring(L, formatUDim2(checkUDim2(L, 1)).c_str());
        return 1;
    }

    static int immutable_newindex(lua_State* L) {
        luaL_error(L, "%s cannot be assigned to, the value is immutable", luaL_checkstring(L, 2));
        return 0;
    }

    template <typename T, T (*Check)(lua_State*, int), void (*Push)(lua_State*, const T&)>
    static void registerType(lua_State* L, int tag, const char* name, lua_CFunction index, lua_CFunction namecall, lua_CFunction tostring) {
        lua_getuserdatametatable(L, tag);
        bool registered = !lua_isnil(L, -1);
        lua_pop(L, 1);
        if (registered) return;

        using Ops = Arithmetic<T, Check, Push>;
        const luaL_Reg metamethods[] = {
            { "__index", index },
            { "__namecall", namecall },
            { "__newindex", immutable_newindex },
            { "__add", Ops::add },
            { "__sub", Ops::sub },
            { "__unm", Ops::unm },
            { "__mul", Ops::mul },
            { "__div", Ops::div },
            { "__eq", Ops::eq },
            { "__tostring", tostring },
        };

        // luaL_newmetatable keeps the table in the registry
        luaL_newmetatable(L, (std::string("Luwow.") + name).c_str());
        for (const luaL_Reg& metamethod : metamethods) {
            lua_pushcfunction(L, metamethod.func, metamethod.name);
            lua_setfield(L, -2, metamethod.name);
        }
        lua_pushstring(L, name);
        lua_setfield(L, -2, "__type");
        lua_pushstring(L, LockedMessage);
        lua_setfield(L, -2, "__metatable");
        lua_setreadonly(L, -1, true);
        lua_setuserdatametatable(L, tag);
    }

    void registerUDims(lua_State* L) {
        registerType<UDim, checkUDim, pushUDim>(L, UDimTag, "UDim", udim_index, udim_namecall, udim_tostring);
        registerType<UDim2, checkUDim2, pushUDim2>(L, UDim2Tag, "UDim2", udim2_index, udim2_namecall, udim2_tostring);
    }
}
