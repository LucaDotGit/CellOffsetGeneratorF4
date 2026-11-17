#include "Plugin/Internal/PatchesManager.hpp"

#include "Plugin/Internal/Core/CellOffsetPatches.hpp"

namespace Plugin::Internal
{
	PatchesManager::PatchesManager(const REX::NotNull<std::shared_ptr<REX::Logging::ILogger>>& a_logger) :
		_logger(a_logger),
		_cellOffsetPatches(std::make_shared<Core::CellOffsetPatches>(a_logger))
	{
	}

	PatchesManager::~PatchesManager() noexcept = default;

	void PatchesManager::OnApiLoad(const REX::NotNull<std::shared_ptr<REL::HookList>>& a_hookList)
	{
		_logger->LogDebug("{}"sv, REX::GetCurrentFunctionName());

		_cellOffsetPatches->OnApiLoad(a_hookList);
	}

	void PatchesManager::Enable() const
	{
		_logger->LogDebug("{}"sv, REX::GetCurrentFunctionName());

		_cellOffsetPatches->Enable();
	}

	void PatchesManager::Disable() const
	{
		_logger->LogDebug("{}"sv, REX::GetCurrentFunctionName());

		_cellOffsetPatches->Disable();
	}
}
