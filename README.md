# UDim

A Roblox compatible UDim and UDim2 value libraries for Luwow-Project.

This repository is not stand-alone. You must build it externally from within another
repository or library.

## Project Overview

This project contains the sources for the Luwow `udim` and `udim2` libraries.

A UDim is one length relative to a parent, like Roblox's: `Scale` times the parent's length, plus `Offset` pixels. A UDim2 is a UDim for each axis. They are immutable, and support arithmetic operators.

The implementation lives in `UDimCore`, a small static library other libraries link to create and resolve UDims from C++. It builds two modules, `udim` and `udim2`.

## Library Documentation

```luau
local UDim = require("@Luwow/udim")
local UDim2 = require("@Luwow/udim2")
```

### `UDim.new(scale: number?, offset: number?) -> UDim`

Both parts default to 0. Also `UDim.fromScale(scale)` and `UDim.fromOffset(pixels)`.

### `udim.Scale`, `udim.Offset: number` / `udim:Resolve(length: number) -> number`

The parts, and the length in pixels within a parent of the given length.

### `UDim2.new(xScale, xOffset, yScale, yOffset) -> UDim2` / `UDim2.new(x: UDim, y: UDim) -> UDim2`

Also `UDim2.fromScale(x, y)` and `UDim2.fromOffset(x, y)` (or a vector).

### `udim2.X`, `udim2.Y: UDim`

The axes, also available as `Width` and `Height`.

### `udim2:Lerp(goal: UDim2, alpha: number) -> UDim2` / `udim2:Resolve(size: vector) -> vector`

The value between this one and goal, and the pixels within a parent of the given size.

## C++

```cpp
#include "UDimCore.h"

using namespace Luwow::UDims;

registerUDims(L);                                   // once, in the module's initialize
pushUDim2(L, UDim2{ UDim{ 1, -20 }, UDim{ 0, 40 } });

UDim2 size = checkUDim2Like(L, 1);                  // a UDim2, or a vector of pixels
float width = size.x.resolve(parentWidth);
```

UDim and UDim2 use the userdata tags 124 and 125.
