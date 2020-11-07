#pragma once

class GFxValue;
class VMValue;
class GFxMovieRoot;
class VirtualMachine;

namespace PlatformAdapter
{
	// Converts a Papyrus value to a Scaleform Value
	bool ConvertPapyrusValue(GFxValue * dest, VMValue * src, GFxMovieRoot * root);

	// Converts a Scaleform value to a Papyrus value
	bool ConvertScaleformValue(VMValue * dest, GFxValue * src, VirtualMachine * vm);
};
