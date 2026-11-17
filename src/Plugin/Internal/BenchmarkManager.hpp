#pragma once

namespace Plugin::Internal
{
	class PatchesManager;
	class SettingsManager;
}

namespace Plugin::Internal::Benchmark
{
	class CellOffsetBenchmark;
	class CellOffsetBenchmarkCase;
}

namespace Plugin::Internal
{
	class BenchmarkManager final : public F4SE::Events::IOnGameDataReady
	{
	public:
		BenchmarkManager(
			const REX::NotNull<std::shared_ptr<REX::Logging::ILogger>>& a_logger,
			std::filesystem::path a_benchmarksDirectoryPath,
			std::filesystem::path a_benchmarkFileExtension,
			const REX::NotNull<std::shared_ptr<SettingsManager>>& a_settingsManager,
			const REX::NotNull<std::shared_ptr<PatchesManager>>& a_patchesManager);

		~BenchmarkManager() noexcept override;

		BenchmarkManager(const BenchmarkManager&) = delete;
		BenchmarkManager(BenchmarkManager&&) = delete;

		auto operator=(const BenchmarkManager&) -> BenchmarkManager& = delete;
		auto operator=(BenchmarkManager&&) -> BenchmarkManager& = delete;

		[[nodiscard]]
		auto GetBenchmarksDirectoryPath() const noexcept -> const auto&
		{
			return _benchmarksDirectoryPath;
		}

		[[nodiscard]]
		auto GetBenchmarkFileExtension() const noexcept -> const auto&
		{
			return _benchmarkFileExtension;
		}

		[[nodiscard]]
		auto GetCellOffsetBenchmark() const noexcept
		{
			return _cellOffsetBenchmark;
		}

		void OnGameDataReady(bool a_isReady) override;

		void RunBenchmarks_Before();
		void RunBenchmarks_After();

	private:
		[[nodiscard]]
		auto LoadBenchmarkCasesFromSettings()
			-> std::vector<REX::NotNull<std::shared_ptr<Benchmark::CellOffsetBenchmarkCase>>>;

		[[nodiscard]]
		auto RunBenchmarksImpl(std::string_view a_extraData) -> std::expected<void, std::string>;

		REX::NotNull<std::shared_ptr<REX::Logging::ILogger>> _logger;
		std::filesystem::path _benchmarksDirectoryPath;
		std::filesystem::path _benchmarkFileExtension;
		REX::NotNull<std::shared_ptr<SettingsManager>> _settingsManager;
		REX::NotNull<std::shared_ptr<PatchesManager>> _patchesManager;
		REX::NotNull<std::shared_ptr<Benchmark::CellOffsetBenchmark>> _cellOffsetBenchmark;
	};
}
