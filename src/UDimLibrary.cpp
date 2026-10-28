#include "Library.h"
#include "UDimCore.h"
#include "lualib.h"

namespace Luwow::UDims {
    ILuauModule* UDimLibrary::initialize(ILuauHost* host) {
        registerUDims(host->getMainState());
        return new UDimLibrary();
    }

    Luwow::Engine::RunMode UDimLibrary::getRunMode() const {
        return LUWOW_MODULE_RUN_MODE;
    }

    // UDim.new(scale, offset), both defaulting to 0
    static int udim_new(lua_State* L) {
        pushUDim(L, UDim{ static_cast<float>(luaL_optnumber(L, 1, 0)), static_cast<float>(luaL_optnumber(L, 2, 0)) });
        return 1;
    }

    // UDim.fromScale(scale), a fraction of the parent
    static int udim_fromScale(lua_State* L) {
        pushUDim(L, UDim{ static_cast<float>(luaL_checknumber(L, 1)), 0 });
        return 1;
    }

    // UDim.fromOffset(pixels)
    static int udim_fromOffset(lua_State* L) {
        pushUDim(L, UDim{ 0, static_cast<float>(luaL_checknumber(L, 1)) });
        return 1;
    }

    const char* UDimLibrary::getModuleName() const {
        return "udim";
    }

    const char* UDimLibrary::getModuleAlias() const {
        return "Luwow";
    }

    static LuauExport exports[] = {
        { "new", udim_new },
        { "fromScale", udim_fromScale },
        { "fromOffset", udim_fromOffset },
        { nullptr, nullptr }
    };

    const LuauExport* UDimLibrary::getExports() const {
        return exports;
    }
} // namespace Luwow::UDims

LUWOW_REGISTER_MODULE(Luwow::UDims::UDimLibrary)
