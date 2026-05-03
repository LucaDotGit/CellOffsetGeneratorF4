#include "Plugin/Internal/PatchesManager.hpp"

#include "Plugin/Internal/Core/CellOffsetPatches.hpp"

namespace Plugin::Internal
{
	// NOLINTNEXTLINE(readability-convert-member-functions-to-static)
	void PatchesManager::OnXseLoad()
	{
		static constexpr auto FUNCTION_NAME = REX::GetCurrentFunctionName();

		static constinit auto OnceFlag = std::once_flag();
		std::call_once(OnceFlag, []() noexcept {
			REX::LogDebug("{}"sv,
				FUNCTION_NAME);

			const auto& hookStore = REL::GetHookStore();

			Core::CellOffsetPatches::OnXseLoad(*(hookStore.get()));
		});
	}

	// NOLINTNEXTLINE(readability-convert-member-functions-to-static)
	void PatchesManager::Enable()
	{
		REX::LogDebug("{}"sv,
			REX::GetCurrentFunctionName());

		Core::CellOffsetPatches::Enable();
	}

	// NOLINTNEXTLINE(readability-convert-member-functions-to-static)
	void PatchesManager::Disable()
	{
		REX::LogDebug("{}"sv,
			REX::GetCurrentFunctionName());

		Core::CellOffsetPatches::Disable();
	}
}
