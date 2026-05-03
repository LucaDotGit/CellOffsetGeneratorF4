#pragma once

namespace Plugin::Internal
{
	class SettingsManager;
	class PatchesManager;
}

namespace Plugin::Internal::Benchmark
{
	class CellOffsetBenchmark;
}

namespace Plugin::Internal
{
	class BenchmarkManager final
	{
	public:
		BenchmarkManager(
			std::filesystem::path a_cellOffsetBenchmarksDirectoryPath,
			std::filesystem::path a_cellOffsetBenchmarkFileExtension,
			const REX::NotNull<std::shared_ptr<SettingsManager>>& a_settingsManager,
			const REX::NotNull<std::shared_ptr<PatchesManager>>& a_patchesManager);

		~BenchmarkManager() noexcept;

		BenchmarkManager(const BenchmarkManager&) = delete;
		BenchmarkManager(BenchmarkManager&&) = delete;

		BenchmarkManager& operator=(const BenchmarkManager&) = delete;
		BenchmarkManager& operator=(BenchmarkManager&&) = delete;

		[[nodiscard]] const auto& GetCellOffsetBenchmarksDirectoryPath() const noexcept { return _cellOffsetBenchmarksDirectoryPath; }
		[[nodiscard]] const auto& GetCellOffsetBenchmarkFileExtension() const noexcept { return _cellOffsetBenchmarkFileExtension; }

		[[nodiscard]] auto GetCellOffsetBenchmark() const noexcept { return _cellOffsetBenchmark; }

		void OnGameDataReady();

		void RunBenchmarks_Before();
		void RunBenchmarks_After();

	private:
		void LoadBenchmarkCasesFromSettings();
		void RunBenchmarksImpl(std::string_view a_extraData);

		std::filesystem::path _cellOffsetBenchmarksDirectoryPath;
		std::filesystem::path _cellOffsetBenchmarkFileExtension;
		REX::NotNull<std::shared_ptr<SettingsManager>> _settingsManager;
		REX::NotNull<std::shared_ptr<PatchesManager>> _patchesManager;
		REX::NotNull<std::shared_ptr<Benchmark::CellOffsetBenchmark>> _cellOffsetBenchmark;
	};
}
