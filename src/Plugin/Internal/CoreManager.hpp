#pragma once

namespace Plugin::Internal::Core
{
	class CellOffsetCreator;
	class CellOffsetMenu;
	class CellOffsetTranslation;
}

namespace Plugin::Internal
{
	class SettingsManager;

	class CoreManager final : public F4SE::Events::IOnGameDataReady
	{
	public:
		CoreManager(
			const REX::NotNull<std::shared_ptr<REX::Logging::ILogger>>& a_logger,
			const REX::NotNull<std::shared_ptr<SettingsManager>>& a_settingsManager,
			std::string a_translationName,
			std::string a_menuName,
			std::filesystem::path a_menuFilePath,
			std::string a_rootVarPath,
			RE::UI_MENU_FLAGS a_menuFlags,
			RE::UI_DEPTH_PRIORITY a_menuDepth,
			std::filesystem::path a_cellOffsetsDirectoryPath,
			std::filesystem::path a_cellOffsetFileExtension);

		~CoreManager() noexcept override;

		CoreManager(const CoreManager&) = delete;
		CoreManager(CoreManager&&) = delete;

		auto operator=(const CoreManager&) -> CoreManager& = delete;
		auto operator=(CoreManager&&) -> CoreManager& = delete;

		[[nodiscard]]
		auto GetCellOffsetTranslation() const noexcept
		{
			return _cellOffsetTranslation;
		}

		[[nodiscard]]
		auto GetCellOffsetMenu() const noexcept
		{
			return _cellOffsetMenu;
		}

		[[nodiscard]]
		auto GetCellOffsetCreator() const noexcept
		{
			return _cellOffsetCreator;
		}

		void OnGameDataReady(bool a_isReady) override;

	private:
		REX::NotNull<std::shared_ptr<REX::Logging::ILogger>> _logger;
		REX::NotNull<std::shared_ptr<Core::CellOffsetTranslation>> _cellOffsetTranslation;
		REX::NotNull<std::shared_ptr<Core::CellOffsetMenu>> _cellOffsetMenu;
		REX::NotNull<std::shared_ptr<Core::CellOffsetCreator>> _cellOffsetCreator;
	};
}
