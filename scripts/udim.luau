-- This is the type system for the UDim API as it exists in C++ code.

--[=[
	One length relative to a parent, Scale times the parent's length, plus Offset
	pixels, printed as "0.5, 10".
]=]
export type UDim = typeof(setmetatable(
	{} :: {
		read Scale: number,
		read Offset: number,

		--[=[
			The length in pixels within a parent of the given length.
		]=]
		Resolve: (self: UDim, length: number) -> number,
	},
	{} :: {
		__add: (UDim, UDim) -> UDim,
		__sub: (UDim, UDim) -> UDim,
		__unm: (UDim) -> UDim,
		__mul: (UDim, number) -> UDim,
		__div: (UDim, number) -> UDim,
	}
))

--[=[
	What properties taking a UDim accept: a UDim, or a number of pixels.
]=]
export type UDimLike = UDim | number

export type Library = {
	--[=[
		Creates a UDim, both parts defaulting to 0.
	]=]
	new: (scale: number?, offset: number?) -> UDim,

	--[=[
		A fraction of the parent: fromScale(0.5) is half of it.
	]=]
	fromScale: (scale: number) -> UDim,

	--[=[
		A number of pixels.
	]=]
	fromOffset: (offset: number) -> UDim
}

return table.freeze({} :: Library)
