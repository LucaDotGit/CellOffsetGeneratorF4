#pragma once

namespace Plugin::Internal
{
	class PatchesManager final
	{
	public:
		PatchesManager() = default;
		~PatchesManager() noexcept = default;

		PatchesManager(const PatchesManager&) = delete;
		PatchesManager(PatchesManager&&) = delete;

		PatchesManager& operator=(const PatchesManager&) = delete;
		PatchesManager& operator=(PatchesManager&&) = delete;

		void OnXseLoad();
		void Enable();
		void Disable();
	};
}
