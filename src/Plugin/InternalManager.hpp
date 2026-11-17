#pragma once

namespace Plugin::Internal
{
	class BenchmarkManager;
	class CoreManager;
	class PatchesManager;
	class SettingsManager;
}

namespace Plugin
{
	class InternalManager final :
		public REX::Singleton<InternalManager>,
		public F4SE::Events::IOnApiLoad,
		public F4SE::Events::IOnGameDataReady
	{
	public:
		InternalManager();
		~InternalManager() noexcept override;

		InternalManager(const InternalManager&) = delete;
		InternalManager(InternalManager&&) = delete;

		auto operator=(const InternalManager&) -> InternalManager& = delete;
		auto operator=(InternalManager&&) -> InternalManager& = delete;

		[[nodiscard]]
		auto GetPluginName() const noexcept
		{
			return _pluginName;
		}

		[[nodiscard]]
		auto GetDataDirectoryPath() const noexcept -> const auto&
		{
			return _dataDirectoryPath;
		}

		[[nodiscard]]
		auto GetPluginsDirectoryPath() const noexcept -> const auto&
		{
			return _pluginsDirectoryPath;
		}

		[[nodiscard]]
		auto GetLogger() const noexcept
		{
			return _logger;
		}

		[[nodiscard]]
		auto GetSettingsManager() const noexcept
		{
			return _settingsManager;
		}

		[[nodiscard]]
		auto GetCoreManager() const noexcept
		{
			return _coreManager;
		}

		[[nodiscard]]
		auto GetPatchesManager() const noexcept
		{
			return _patchesManager;
		}

		[[nodiscard]]
		auto GetBenchmarkManager() const noexcept
		{
			return _benchmarkManager;
		}

		void OnApiLoad(const REX::NotNull<std::shared_ptr<REL::HookList>>& a_hookList) override;
		void OnGameDataReady(bool a_isReady) override;

	private:
		std::string_view _pluginName;
		std::filesystem::path _dataDirectoryPath;
		std::filesystem::path _pluginsDirectoryPath;
		REX::NotNull<std::shared_ptr<REX::Logging::ILogger>> _logger;
		REX::NotNull<std::shared_ptr<Internal::SettingsManager>> _settingsManager;
		REX::NotNull<std::shared_ptr<Internal::CoreManager>> _coreManager;
		REX::NotNull<std::shared_ptr<Internal::PatchesManager>> _patchesManager;
		REX::NotNull<std::shared_ptr<Internal::BenchmarkManager>> _benchmarkManager;
	};
}
