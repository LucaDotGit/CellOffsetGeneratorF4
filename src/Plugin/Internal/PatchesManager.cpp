#include "Plugin/Internal/PatchesManager.hpp"

#include "Plugin/Internal/Core/CellOffsetPatches.hpp"

namespace Plugin::Internal
{
	PatchesManager::PatchesManager()
		: _cellOffsetPatches(std::make_shared<Core::CellOffsetPatches>())
	{
	}

	PatchesManager::~PatchesManager() noexcept = default;

	void PatchesManager::OnXseLoad()
	{
		static constexpr auto FUNCTION_NAME = REX::GetCurrentFunctionName();

		static constinit auto OnceFlag = std::once_flag();
		std::call_once(OnceFlag, [this]() -> void {
			REX::LogDebug("{}"sv,
				FUNCTION_NAME);

			const auto& hookStore = REL::GetHookStore();

			_cellOffsetPatches->OnXseLoad(*(hookStore.get()));
		});
	}

	void PatchesManager::Enable()
	{
		REX::LogDebug("{}"sv,
			REX::GetCurrentFunctionName());

		_cellOffsetPatches->Enable();
	}

	void PatchesManager::Disable()
	{
		REX::LogDebug("{}"sv,
			REX::GetCurrentFunctionName());

		_cellOffsetPatches->Disable();
	}
}
