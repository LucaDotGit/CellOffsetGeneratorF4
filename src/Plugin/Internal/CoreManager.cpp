#include "Plugin/Internal/CoreManager.hpp"

#include "Plugin/Internal/Core/CellOffsetCreator.hpp"
#include "Plugin/Internal/Core/CellOffsetMenu.hpp"
#include "Plugin/Internal/Core/CellOffsetTranslation.hpp"

namespace Plugin::Internal
{
	CoreManager::CoreManager(
		const REX::NotNull<std::shared_ptr<REX::Logging::ILogger>>& a_logger,
		const REX::NotNull<std::shared_ptr<SettingsManager>>& a_settingsManager,
		std::string a_translationName,
		std::string a_menuName,
		std::filesystem::path a_menuFilePath,
		std::string a_rootVarPath,
		RE::UI_MENU_FLAGS a_menuFlags,
		RE::UI_DEPTH_PRIORITY a_menuDepth,
		std::filesystem::path a_cellOffsetsDirectoryPath,
		std::filesystem::path a_cellOffsetFileExtension) :
		_logger(a_logger),
		_cellOffsetTranslation(std::make_shared<Core::CellOffsetTranslation>(a_logger, std::move(a_translationName))),
		_cellOffsetMenu(
			std::make_shared<Core::CellOffsetMenu>(
				std::move(a_menuName),
				std::move(a_menuFilePath),
				std::move(a_rootVarPath),
				a_menuFlags,
				a_menuDepth)),
		_cellOffsetCreator(
			std::make_shared<Core::CellOffsetCreator>(
				a_logger,
				a_settingsManager,
				_cellOffsetMenu,
				std::move(a_cellOffsetsDirectoryPath),
				std::move(a_cellOffsetFileExtension)))
	{
	}

	CoreManager::~CoreManager() noexcept = default;

	void CoreManager::OnGameDataReady(bool a_isReady)
	{
		_logger->LogDebug("{}"sv, REX::GetCurrentFunctionName());

		_cellOffsetTranslation->OnGameDataReady(a_isReady);
		_cellOffsetCreator->OnGameDataReady(a_isReady);
	}
}
