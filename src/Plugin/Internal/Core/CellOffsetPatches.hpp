#pragma once

namespace Plugin::Internal::Core
{
	class CellOffsetPatches final
	{
	public:
		CellOffsetPatches() = default;
		~CellOffsetPatches() noexcept = default;

		CellOffsetPatches(const CellOffsetPatches&) = delete;
		CellOffsetPatches(CellOffsetPatches&&) = delete;

		CellOffsetPatches& operator=(const CellOffsetPatches&) = delete;
		CellOffsetPatches& operator=(CellOffsetPatches&&) = delete;

		void OnXseLoad(REL::HookStore& hookStore);
		void Enable();
		void Disable();

	private:
		std::vector<REX::NotNull<std::shared_ptr<REL::IHook>>> _hooks;
	};
}
