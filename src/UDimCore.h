#pragma once

#include <string>

struct lua_State;


/*
    UDim and UDim2 values, usable from Luau and C++ alike.

    A UDim is one length relative to a parent, Scale times the parent's length,
    plus Offset pixels. A UDim2 is a UDim for each axis.
*/
namespace Luwow::UDims {
    // Userdata tags reserved for UDim and UDim2, below Luau's limit of 128
    inline constexpr int UDimTag = 124;
    inline constexpr int UDim2Tag = 125;

    struct UDim {
        float scale = 0;
        float offset = 0;

        // The length in pixels, within a parent of the given length
        float resolve(float length) const { return scale * length + offset; }

        UDim operator+(const UDim& other) const { return UDim{ scale + other.scale, offset + other.offset }; }
        UDim operator-(const UDim& other) const { return UDim{ scale - other.scale, offset - other.offset }; }
        UDim operator-() const { return UDim{ -scale, -offset }; }
        UDim operator*(float factor) const { return UDim{ scale * factor, offset * factor }; }
        UDim operator/(float divisor) const { return UDim{ scale / divisor, offset / divisor }; }
        bool operator==(const UDim& other) const = default;
    };

    struct UDim2 {
        UDim x;
        UDim y;

        static UDim2 fromScale(float x, float y) { return UDim2{ UDim{ x, 0 }, UDim{ y, 0 } }; }
        static UDim2 fromOffset(float x, float y) { return UDim2{ UDim{ 0, x }, UDim{ 0, y } }; }

        UDim2 lerp(const UDim2& goal, float alpha) const { return *this + (goal - *this) * alpha; }

        UDim2 operator+(const UDim2& other) const { return UDim2{ x + other.x, y + other.y }; }
        UDim2 operator-(const UDim2& other) const { return UDim2{ x - other.x, y - other.y }; }
        UDim2 operator-() const { return UDim2{ -x, -y }; }
        UDim2 operator*(float factor) const { return UDim2{ x * factor, y * factor }; }
        UDim2 operator/(float divisor) const { return UDim2{ x / divisor, y / divisor }; }
        bool operator==(const UDim2& other) const = default;
    };

    // "0.5, 10" and "{1, -20}, {0, 40}"
    std::string formatUDim(const UDim& udim);
    std::string formatUDim2(const UDim2& udim2);

    // Registers both metatables once per Luau state, calling it again does nothing
    void registerUDims(lua_State* L);

    void pushUDim(lua_State* L, const UDim& udim);
    void pushUDim2(lua_State* L, const UDim2& udim2);

    bool isUDim(lua_State* L, int index);
    bool isUDim2(lua_State* L, int index);

    // The value at index, raising a type error for anything else
    UDim checkUDim(lua_State* L, int index);
    UDim2 checkUDim2(lua_State* L, int index);

    // What properties taking a UDim accept: a UDim, or a number of pixels
    UDim checkUDimLike(lua_State* L, int index);
    // What properties taking a UDim2 accept: a UDim2, or a vector of pixels
    UDim2 checkUDim2Like(lua_State* L, int index);
}
