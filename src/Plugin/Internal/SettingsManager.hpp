#pragma once

namespace Plugin::Internal
{
	class SettingsManager final
	{
	public:
		SettingsManager(std::filesystem::path a_filePath, std::filesystem::path a_customFilePath);
		~SettingsManager() noexcept;

		SettingsManager(const SettingsManager&) = delete;
		SettingsManager(SettingsManager&&) = delete;

		SettingsManager& operator=(const SettingsManager&) = delete;
		SettingsManager& operator=(SettingsManager&&) = delete;

		[[nodiscard]] const auto& GetFilePath() const noexcept { return _settings.GetMainPath(); }
		[[nodiscard]] const auto& GetCustomFilePath() const noexcept { return _settings.GetCustomPath(); }

		[[nodiscard]] auto GetLogLevelSetting() const noexcept { return _logLevelSetting; }
		[[nodiscard]] auto GetLogFileCountSetting() const noexcept { return _logFileCountSetting; }
		[[nodiscard]] auto GetEnableMenuSetting() const noexcept { return _enableMenuSetting; }
		[[nodiscard]] auto GetDoTryDryRunSetting() const noexcept { return _doTryDryRunSetting; }
		[[nodiscard]] auto GetMarkCreatedFilesAsReadOnlySetting() const noexcept { return _markCreatedFilesAsReadOnlySetting; }
		[[nodiscard]] auto GetRunBenchmarkSetting() const noexcept { return _runBenchmarkSetting; }
		[[nodiscard]] auto GetBenchmarkIterationCountSetting() const noexcept { return _benchmarkIterationCountSetting; }
		[[nodiscard]] auto GetBenchmarkCasesSetting() const noexcept { return _benchmarkCasesSetting; }

		void LoadSettings();

	private:
		REX::NotNull<std::shared_ptr<REX::Toml::Int32>> _logLevelSetting;
		REX::NotNull<std::shared_ptr<REX::Toml::Int32>> _logFileCountSetting;
		REX::NotNull<std::shared_ptr<REX::Toml::Bool>> _enableMenuSetting;
		REX::NotNull<std::shared_ptr<REX::Toml::Bool>> _doTryDryRunSetting;
		REX::NotNull<std::shared_ptr<REX::Toml::Bool>> _markCreatedFilesAsReadOnlySetting;
		REX::NotNull<std::shared_ptr<REX::Toml::Bool>> _runBenchmarkSetting;
		REX::NotNull<std::shared_ptr<REX::Toml::UInt32>> _benchmarkIterationCountSetting;
		REX::NotNull<std::shared_ptr<REX::Toml::ValueArray>> _benchmarkCasesSetting;
		REX::Toml::SettingStore _settings;
	};
}
