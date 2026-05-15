#include "Plugin/Internal/SettingsManager.hpp"

namespace Plugin::Internal
{
	SettingsManager::SettingsManager(std::filesystem::path a_filePath, std::filesystem::path a_customFilePath)
		: _logLevelSetting(std::make_shared<REX::Toml::Int32>("Debug"s, "iLogLevel"s, static_cast<std::int32_t>(REX::LogLevel::kInformation))),
		  _logFileCountSetting(std::make_shared<REX::Toml::Int32>("Debug"s, "iLogFileCount"s, -1)),
		  _enableMenuSetting(std::make_shared<REX::Toml::Bool>("General"s, "bEnableMenu"s, true)),
		  _doTryDryRunSetting(std::make_shared<REX::Toml::Bool>("General"s, "bDoDryRun"s, false)),
		  _createReadOnlyFilesSetting(std::make_shared<REX::Toml::Bool>("General"s, "bCreateReadOnlyFiles"s, true)),
		  _runBenchmarkSetting(std::make_shared<REX::Toml::Bool>("Benchmark"s, "bRunBenchmark"s, false)),
		  _benchmarkIterationCountSetting(std::make_shared<REX::Toml::UInt32>("Benchmark"s, "uiBenchmarkIterationCount"s, 100)),
		  _benchmarkCasesSetting(std::make_shared<REX::Toml::ValueArray>("Benchmark"s, "kBenchmarkCases"s)),
		  _settings(std::move(a_filePath), std::move(a_customFilePath))
	{
		const auto settings = std::initializer_list<const REX::NotNull<std::shared_ptr<REX::ISetting>>>{
			_logLevelSetting,
			_logFileCountSetting,
			_enableMenuSetting,
			_doTryDryRunSetting,
			_createReadOnlyFilesSetting,
			_runBenchmarkSetting,
			_benchmarkIterationCountSetting,
			_benchmarkCasesSetting
		};

		_settings.AddRange(settings);
	}

	SettingsManager::~SettingsManager() noexcept = default;

	void SettingsManager::LoadSettings()
	{
		REX::LogDebug("{}"sv,
			REX::GetCurrentFunctionName());

		_settings.Load();
	}
}
