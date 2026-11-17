#pragma once

namespace Plugin::Internal
{
	class SettingsManager final : public F4SE::Events::IOnSettingsLoad
	{
	public:
		class BenchmarkCase final
		{
		public:
			// NOLINTBEGIN(readability-identifier-naming)
			std::string WorldSpace;
			std::int32_t CellX;
			std::int32_t CellY;
			// NOLINTEND(readability-identifier-naming)
		};

		SettingsManager(
			REX::NotNull<std::shared_ptr<REX::Logging::ILogger>> a_logger,
			std::filesystem::path a_mainFilePath,
			std::filesystem::path a_customFilePath);
		~SettingsManager() noexcept override;

		SettingsManager(const SettingsManager&) = delete;
		SettingsManager(SettingsManager&&) = delete;

		auto operator=(const SettingsManager&) -> SettingsManager& = delete;
		auto operator=(SettingsManager&&) -> SettingsManager& = delete;

		[[nodiscard]]
		auto GetLogMinLevelSetting() const noexcept
		{
			return _logMinLevelSetting;
		}

		[[nodiscard]]
		auto GetLogRotationCountSetting() const noexcept
		{
			return _logRotationCountSetting;
		}

		[[nodiscard]]
		auto GetLogRotationByteSizeSetting() const noexcept
		{
			return _logRotationByteSizeSetting;
		}

		[[nodiscard]]
		auto GetEnableMenuSetting() const noexcept
		{
			return _enableMenuSetting;
		}

		[[nodiscard]]
		auto GetDoTryDryRunSetting() const noexcept
		{
			return _doTryDryRunSetting;
		}

		[[nodiscard]]
		auto GetThreadCountSetting() const noexcept
		{
			return _threadCountSetting;
		}

		[[nodiscard]]
		auto GetRunBenchmarkSetting() const noexcept
		{
			return _runBenchmarkSetting;
		}

		[[nodiscard]]
		auto GetBenchmarkIterationCountSetting() const noexcept
		{
			return _benchmarkIterationCountSetting;
		}

		[[nodiscard]]
		auto GetBenchmarkCasesSetting() const noexcept
		{
			return _benchmarkCasesSetting;
		}

		[[nodiscard]]
		auto GetDebugSettingGroup() const noexcept
		{
			return _debugSettingGroup;
		}

		[[nodiscard]]
		auto GetGenerationSettingGroup() const noexcept
		{
			return _generationSettingGroup;
		}

		[[nodiscard]]
		auto GetBenchmarkSettingGroup() const noexcept
		{
			return _benchmarkSettingGroup;
		}

		[[nodiscard]]
		auto GetRootSettingList() const noexcept
		{
			return _rootSettingList;
		}

		[[nodiscard]]
		auto GetMainSettingFile() const noexcept
		{
			return _mainSettingFile;
		}

		[[nodiscard]]
		auto GetCustomSettingFile() const noexcept
		{
			return _customSettingFile;
		}

		void OnSettingsLoad() override;

	private:
		REX::NotNull<std::shared_ptr<REX::Logging::ILogger>> _logger;
		REX::NotNull<std::shared_ptr<REX::TOML::Setting<REX::Logging::Level>>> _logMinLevelSetting;
		REX::NotNull<std::shared_ptr<REX::TOML::Setting<std::uint32_t>>> _logRotationCountSetting;
		REX::NotNull<std::shared_ptr<REX::TOML::Setting<std::uint32_t>>> _logRotationByteSizeSetting;
		REX::NotNull<std::shared_ptr<REX::TOML::Setting<bool>>> _enableMenuSetting;
		REX::NotNull<std::shared_ptr<REX::TOML::Setting<bool>>> _doTryDryRunSetting;
		REX::NotNull<std::shared_ptr<REX::TOML::Setting<std::uint32_t>>> _threadCountSetting;
		REX::NotNull<std::shared_ptr<REX::TOML::Setting<bool>>> _runBenchmarkSetting;
		REX::NotNull<std::shared_ptr<REX::TOML::Setting<std::uint32_t>>> _benchmarkIterationCountSetting;
		REX::NotNull<std::shared_ptr<REX::TOML::Setting<std::vector<BenchmarkCase>>>> _benchmarkCasesSetting;
		REX::NotNull<std::shared_ptr<REX::TOML::SettingGroup>> _debugSettingGroup;
		REX::NotNull<std::shared_ptr<REX::TOML::SettingGroup>> _generationSettingGroup;
		REX::NotNull<std::shared_ptr<REX::TOML::SettingGroup>> _benchmarkSettingGroup;
		REX::NotNull<std::shared_ptr<REX::TOML::SettingList>> _rootSettingList;
		REX::NotNull<std::shared_ptr<REX::TOML::SettingFile>> _mainSettingFile;
		REX::NotNull<std::shared_ptr<REX::TOML::SettingFile>> _customSettingFile;
	};
}
