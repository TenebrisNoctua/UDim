#pragma once

#include "ILuauModule.h"
#include "ILuauHost.h"
#include "lua.h"

// Two modules share UDimCore: udim for UDim, udim2 for UDim2. Each is built as its own library.
namespace Luwow::UDims {
    using ILuauModule = Luwow::Engine::ILuauModule;
    using ILuauHost = Luwow::Engine::ILuauHost;
    using RunMode = Luwow::Engine::RunMode;

    class UDimLibrary : public ILuauModule {
    public:
        ILuauModule* initialize(ILuauHost* host) override;
        RunMode getRunMode() const override;

        const char* getModuleName() const override;
        const char* getModuleAlias() const override;
        const LuauExport* getExports() const override;
    };

    class UDim2Library : public ILuauModule {
    public:
        ILuauModule* initialize(ILuauHost* host) override;
        RunMode getRunMode() const override;

        const char* getModuleName() const override;
        const char* getModuleAlias() const override;
        const LuauExport* getExports() const override;
    };
} // namespace Luwow::UDims
