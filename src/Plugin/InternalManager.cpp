#include "Plugin/InternalManager.hpp"

#include "Plugin/Internal/BenchmarkManager.hpp"
#include "Plugin/Internal/Core/CellOffsetCreator.hpp"
#include "Plugin/Internal/CoreManager.hpp"
#include "Plugin/Internal/PatchesManager.hpp"
#include "Plugin/Internal/SettingsManager.hpp"

namespace Plugin::InternalManager_Impl
{
	namespace
	{
		constexpr auto MENU_ROOT_VAR_PATH = "root1"sv;
		constexpr auto MENU_FLAGS = REX::EnumSet(RE::UI_MENU_FLAGS::UsesCursor);
		constexpr auto MENU_DEPTH = REX::Enum(RE::UI_DEPTH_PRIORITY::Message);

		constexpr auto CELL_OFFSET_CACHE_DIRECTORY_NAME = "CellOffsetCache"sv;
		constexpr auto CELL_OFFSET_CACHE_FILE_EXTENSION = ".bin"sv;

		constexpr auto CELL_OFFSET_BENCHMARKS_DIRECTORY_NAME = "CellOffsetBenchmarks"sv;
		constexpr auto CELL_OFFSET_BENCHMARKS_FILE_EXTENSION = ".csv"sv;
	}
}

namespace Plugin
{
	InternalManager::InternalManager() :
		_pluginName(F4SE::PluginVersionData::GetSingleton().GetPluginName()),
		_dataDirectoryPath(F4SE::DATA_DIRECTORY_PATH, std::filesystem::path::generic_format),
		_pluginsDirectoryPath(F4SE::PLUGINS_DIRECTORY_PATH, std::filesystem::path::generic_format),
		_logger(std::make_shared<REX::Logging::Logger>(std::string(_pluginName))),
		_settingsManager(
			std::make_shared<Internal::SettingsManager>(
				_logger,
				std::filesystem::path(
					_pluginsDirectoryPath / REX::Format("{}.toml"sv, _pluginName),
					std::filesystem::path::generic_format),
				std::filesystem::path(
					_pluginsDirectoryPath / REX::Format("{}_Custom.toml"sv, _pluginName),
					std::filesystem::path::generic_format))),
		_coreManager(
			std::make_shared<Internal::CoreManager>(
				_logger,
				_settingsManager,
				std::string(_pluginName),
				std::string(_pluginName),
				std::filesystem::path(REX::Format("{}.swf"sv, _pluginName), std::filesystem::path::generic_format),
				std::string(InternalManager_Impl::MENU_ROOT_VAR_PATH),
				InternalManager_Impl::MENU_FLAGS.get(),
				InternalManager_Impl::MENU_DEPTH.get(),
				std::filesystem::path(
					_dataDirectoryPath / InternalManager_Impl::CELL_OFFSET_CACHE_DIRECTORY_NAME,
					std::filesystem::path::generic_format),
				std::filesystem::path(
					InternalManager_Impl::CELL_OFFSET_CACHE_FILE_EXTENSION,
					std::filesystem::path::generic_format))),
		_patchesManager(std::make_shared<Internal::PatchesManager>(_logger)),
		_benchmarkManager(
			std::make_shared<Internal::BenchmarkManager>(
				_logger,
				std::filesystem::path(
					_dataDirectoryPath / InternalManager_Impl::CELL_OFFSET_BENCHMARKS_DIRECTORY_NAME,
					std::filesystem::path::generic_format),
				std::filesystem::path(
					InternalManager_Impl::CELL_OFFSET_BENCHMARKS_FILE_EXTENSION,
					std::filesystem::path::generic_format),
				_settingsManager,
				_patchesManager))
	{
	}

	InternalManager::~InternalManager() noexcept = default;

	void InternalManager::OnApiLoad(const REX::NotNull<std::shared_ptr<REL::HookList>>& a_hookList)
	{
		_logger->LogDebug("{}"sv, REX::GetCurrentFunctionName());

		_patchesManager->OnApiLoad(a_hookList);
	}

	void InternalManager::OnGameDataReady(bool a_isReady)
	{
		_logger->LogDebug("{} with ready state {}"sv, REX::GetCurrentFunctionName(), a_isReady);

		_benchmarkManager->OnGameDataReady(a_isReady);
		_benchmarkManager->RunBenchmarks_Before();

		_coreManager->OnGameDataReady(a_isReady);
		_benchmarkManager->RunBenchmarks_After();
	}
}
