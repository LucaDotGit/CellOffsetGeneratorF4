#pragma once

namespace Plugin::Internal
{
	class SettingsManager;
	class CoreManager;
	class PatchesManager;
	class BenchmarkManager;
}

namespace Plugin
{
	class InternalManager final
	{
	public:
		InternalManager();
		~InternalManager() noexcept;

		InternalManager(const InternalManager&) = delete;
		InternalManager(InternalManager&&) = delete;

		InternalManager& operator=(const InternalManager&) = delete;
		InternalManager& operator=(InternalManager&&) = delete;

		[[nodiscard]] auto GetSettingsManager() const noexcept { return _settingsManager; }
		[[nodiscard]] auto GetCoreManager() const noexcept { return _coreManager; }
		[[nodiscard]] auto GetPatchesManager() const noexcept { return _patchesManager; }
		[[nodiscard]] auto GetBenchmarkManager() const noexcept { return _benchmarkManager; }

		void OnXseLoad();
		void OnGameDataReady();

	private:
		std::string_view _pluginName;
		std::filesystem::path _dataDirectoryPath;
		std::filesystem::path _pluginsDirectoryPath;
		REX::NotNull<std::shared_ptr<Internal::SettingsManager>> _settingsManager;
		REX::NotNull<std::shared_ptr<Internal::CoreManager>> _coreManager;
		REX::NotNull<std::shared_ptr<Internal::PatchesManager>> _patchesManager;
		REX::NotNull<std::shared_ptr<Internal::BenchmarkManager>> _benchmarkManager;
	};

	[[nodiscard]] auto GetInternalManager()
		-> const REX::NotNull<std::unique_ptr<InternalManager>>&;
}
