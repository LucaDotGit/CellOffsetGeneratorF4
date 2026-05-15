#pragma once

namespace Plugin::Internal::Core
{
	class CellOffsetPatches;
}

namespace Plugin::Internal
{
	class PatchesManager final
	{
	public:
		PatchesManager();
		~PatchesManager() noexcept;

		PatchesManager(const PatchesManager&) = delete;
		PatchesManager(PatchesManager&&) = delete;

		PatchesManager& operator=(const PatchesManager&) = delete;
		PatchesManager& operator=(PatchesManager&&) = delete;

		[[nodiscard]] auto GetCellOffsetPatches() const noexcept { return _cellOffsetPatches; }

		void OnXseLoad();
		void Enable();
		void Disable();

	private:
		REX::NotNull<std::shared_ptr<Core::CellOffsetPatches>> _cellOffsetPatches;
	};
}
