#include "Library.h"
#include "UDimCore.h"
#include "lualib.h"

namespace Luwow::UDims {
    ILuauModule* UDim2Library::initialize(ILuauHost* host) {
        registerUDims(host->getMainState());
        return new UDim2Library();
    }

    Luwow::Engine::RunMode UDim2Library::getRunMode() const {
        return LUWOW_MODULE_RUN_MODE;
    }

    static float optNumber(lua_State* L, int index) {
        return static_cast<float>(luaL_optnumber(L, index, 0));
    }

    // UDim2.new(xScale, xOffset, yScale, yOffset), or UDim2.new(x: UDim, y: UDim)
    static int udim2_new(lua_State* L) {
        if (isUDim(L, 1)) {
            pushUDim2(L, UDim2{ checkUDim(L, 1), checkUDim(L, 2) });
        } else {
            pushUDim2(L, UDim2{ UDim{ optNumber(L, 1), optNumber(L, 2) }, UDim{ optNumber(L, 3), optNumber(L, 4) } });
        }
        return 1;
    }

    // UDim2.fromScale(x, y)
    static int udim2_fromScale(lua_State* L) {
        pushUDim2(L, UDim2::fromScale(optNumber(L, 1), optNumber(L, 2)));
        return 1;
    }

    // UDim2.fromOffset(x, y), or UDim2.fromOffset(pixels: vector)
    static int udim2_fromOffset(lua_State* L) {
        if (lua_isvector(L, 1)) {
            const float* v = lua_tovector(L, 1);
            pushUDim2(L, UDim2::fromOffset(v[0], v[1]));
        } else {
            pushUDim2(L, UDim2::fromOffset(optNumber(L, 1), optNumber(L, 2)));
        }
        return 1;
    }

    const char* UDim2Library::getModuleName() const {
        return "udim2";
    }

    const char* UDim2Library::getModuleAlias() const {
        return "Luwow";
    }

    static LuauExport exports[] = {
        { "new", udim2_new },
        { "fromScale", udim2_fromScale },
        { "fromOffset", udim2_fromOffset },
        { nullptr, nullptr }
    };

    const LuauExport* UDim2Library::getExports() const {
        return exports;
    }
} // namespace Luwow::UDims

LUWOW_REGISTER_MODULE(Luwow::UDims::UDim2Library)
