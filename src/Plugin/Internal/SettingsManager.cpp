#include "Plugin/Internal/SettingsManager.hpp"

namespace Plugin::Internal
{
	SettingsManager::SettingsManager(
		REX::NotNull<std::shared_ptr<REX::Logging::ILogger>> a_logger,
		std::filesystem::path a_mainFilePath,
		std::filesystem::path a_customFilePath) :
		_logger(std::move(a_logger)),
		_logMinLevelSetting(
			std::make_shared<REX::TOML::Setting<REX::Logging::Level>>(
				"LogMinLevel"s,
				REX::Logging::Level::Information)),
		_logRotationCountSetting(std::make_shared<REX::TOML::Setting<std::uint32_t>>("LogRotationCount"s, 0)),
		_logRotationByteSizeSetting(std::make_shared<REX::TOML::Setting<std::uint32_t>>("LogRotationByteSize"s, 0)),
		_enableMenuSetting(std::make_shared<REX::TOML::Setting<bool>>("EnableMenu"s, true)),
		_doTryDryRunSetting(std::make_shared<REX::TOML::Setting<bool>>("DoDryRun"s, false)),
		_threadCountSetting(std::make_shared<REX::TOML::Setting<std::uint32_t>>("ThreadCount"s, 0)),
		_runBenchmarkSetting(std::make_shared<REX::TOML::Setting<bool>>("RunBenchmark"s, false)),
		_benchmarkIterationCountSetting(
			std::make_shared<REX::TOML::Setting<std::uint32_t>>("BenchmarkIterationCount"s, 100)),
		_benchmarkCasesSetting(std::make_shared<REX::TOML::Setting<std::vector<BenchmarkCase>>>("BenchmarkCases"s)),
		_debugSettingGroup(std::make_shared<REX::TOML::SettingGroup>("Debug"s)),
		_generationSettingGroup(std::make_shared<REX::TOML::SettingGroup>("Generation"s)),
		_benchmarkSettingGroup(std::make_shared<REX::TOML::SettingGroup>("Benchmark"s)),
		_rootSettingList(std::make_shared<REX::TOML::SettingList>()),
		_mainSettingFile(std::make_shared<REX::TOML::SettingFile>(std::move(a_mainFilePath))),
		_customSettingFile(std::make_shared<REX::TOML::SettingFile>(std::move(a_customFilePath)))
	{
		_debugSettingGroup->Add(_logMinLevelSetting);
		_debugSettingGroup->Add(_logRotationCountSetting);
		_debugSettingGroup->Add(_logRotationByteSizeSetting);

		_generationSettingGroup->Add(_enableMenuSetting);
		_generationSettingGroup->Add(_doTryDryRunSetting);
		_generationSettingGroup->Add(_threadCountSetting);

		_benchmarkSettingGroup->Add(_runBenchmarkSetting);
		_benchmarkSettingGroup->Add(_benchmarkIterationCountSetting);
		_benchmarkSettingGroup->Add(_benchmarkCasesSetting);

		_rootSettingList->Add(_debugSettingGroup);
		_rootSettingList->Add(_generationSettingGroup);
		_rootSettingList->Add(_benchmarkSettingGroup);

		_mainSettingFile->Add(_rootSettingList);
		_customSettingFile->Add(_rootSettingList);
	}

	SettingsManager::~SettingsManager() noexcept = default;

	void SettingsManager::OnSettingsLoad()
	{
		_logger->LogDebug("{}"sv, REX::GetCurrentFunctionName());

		const auto mainLoadError = _mainSettingFile->Load();
		if (mainLoadError.value() != REX::ERROR_NUMBER_SUCCESS) [[unlikely]] {
			_logger->LogError(
				"Failed to load settings from file \"{}\": {}"sv,
				_mainSettingFile->GetFilePath().generic_string(),
				mainLoadError.message());
			return;
		}

		const auto customLoadError = _customSettingFile->Load();
		if (customLoadError.value() != REX::ERROR_NUMBER_SUCCESS &&
			customLoadError != REX::PosixErrorCode::no_such_file_or_directory) [[unlikely]] {
			_logger->LogError(
				"Failed to load settings from file \"{}\": {}"sv,
				_customSettingFile->GetFilePath().generic_string(),
				customLoadError.message());
			return;
		}
	}
}
