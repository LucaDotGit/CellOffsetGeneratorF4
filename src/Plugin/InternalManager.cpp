#include "Plugin/InternalManager.hpp"

#include "Plugin/Internal/BenchmarkManager.hpp"
#include "Plugin/Internal/Core/CellOffsetCreator.hpp"
#include "Plugin/Internal/CoreManager.hpp"
#include "Plugin/Internal/PatchesManager.hpp"
#include "Plugin/Internal/SettingsManager.hpp"

namespace Plugin
{
	static constexpr auto TRANSLATION_NAME = "CellOffsetGenerator"sv;

	static constexpr auto MENU_NAME = "CellOffsetGenerator"sv;
	static constexpr auto MENU_FILE_PATH = "CellOffsetGenerator.swf"sv;
	static constexpr auto MENU_ROOT_VAR_PATH = "root1"sv;

	static constexpr auto MENU_FLAGS = REX::EnumSet{
		RE::UI_MENU_FLAGS::kPausesGame,
		RE::UI_MENU_FLAGS::kUsesCursor,
		RE::UI_MENU_FLAGS::kUsesMenuContext,
		RE::UI_MENU_FLAGS::kDisablePauseMenu,
		RE::UI_MENU_FLAGS::kAdvancesUnderPauseMenu
	};

	static constexpr auto MENU_DEPTH = REX::Enum(RE::UI_DEPTH_PRIORITY::kMessage);

	static constexpr auto CELL_OFFSET_CACHE_DIRECTORY_NAME = "CellOffsetCache"sv;
	static constexpr auto CELL_OFFSET_CACHE_FILE_EXTENSION = ".bin"sv;

	static constexpr auto CELL_OFFSET_BENCHMARKS_DIRECTORY_NAME = "CellOffsetBenchmarks"sv;
	static constexpr auto CELL_OFFSET_BENCHMARKS_FILE_EXTENSION = ".csv"sv;

	InternalManager::InternalManager()
		: _pluginName(F4SE::PluginVersionData::GetSingleton()->GetPluginName()),
		  _dataDirectoryPath(F4SE::DATA_DIRECTORY_PATH, std::filesystem::path::generic_format),
		  _pluginsDirectoryPath(F4SE::PLUGINS_DIRECTORY_PATH, std::filesystem::path::generic_format),
		  _settingsManager(std::make_shared<Internal::SettingsManager>(
			  std::filesystem::path(_pluginsDirectoryPath / REX::Format("{}.toml"sv, _pluginName), std::filesystem::path::generic_format),
			  std::filesystem::path(_pluginsDirectoryPath / REX::Format("{}_Custom.toml"sv, _pluginName), std::filesystem::path::generic_format))),
		  _coreManager(std::make_shared<Internal::CoreManager>(
			  _settingsManager,
			  std::string(TRANSLATION_NAME),
			  std::string(MENU_NAME),
			  std::filesystem::path(MENU_FILE_PATH, std::filesystem::path::generic_format),
			  std::string(MENU_ROOT_VAR_PATH),
			  MENU_FLAGS.get(),
			  MENU_DEPTH.get(),
			  std::filesystem::path(_dataDirectoryPath / CELL_OFFSET_CACHE_DIRECTORY_NAME, std::filesystem::path::generic_format),
			  std::filesystem::path(CELL_OFFSET_CACHE_FILE_EXTENSION, std::filesystem::path::generic_format))),
		  _patchesManager(std::make_shared<Internal::PatchesManager>()),
		  _benchmarkManager(std::make_shared<Internal::BenchmarkManager>(
			  std::filesystem::path(_dataDirectoryPath / CELL_OFFSET_BENCHMARKS_DIRECTORY_NAME, std::filesystem::path::generic_format),
			  std::filesystem::path(CELL_OFFSET_BENCHMARKS_FILE_EXTENSION, std::filesystem::path::generic_format),
			  _settingsManager,
			  _patchesManager))
	{
	}

	InternalManager::~InternalManager() noexcept = default;

	void InternalManager::OnXseLoad()
	{
		REX::LogDebug("{}"sv,
			REX::GetCurrentFunctionName());

		_patchesManager->OnXseLoad();
	}

	void InternalManager::OnGameDataReady()
	{
		REX::LogDebug("{}"sv,
			REX::GetCurrentFunctionName());

		_benchmarkManager->OnGameDataReady();
		_benchmarkManager->RunBenchmarks_Before();

		_coreManager->OnGameDataReady();
		_benchmarkManager->RunBenchmarks_After();
	}

	auto GetInternalManager() -> const REX::NotNull<std::unique_ptr<InternalManager>>&
	{
		static const auto INSTANCE = REX::NotNull(std::make_unique<InternalManager>());
		return INSTANCE;
	}
}
