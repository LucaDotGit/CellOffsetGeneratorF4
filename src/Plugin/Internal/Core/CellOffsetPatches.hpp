#pragma once

namespace Plugin::Internal::Core::CellOffsetPatches
{
	void OnXseLoad(REL::HookStore& hookStore);
	void Enable();
	void Disable();
}
