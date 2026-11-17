#pragma once

namespace Plugin::Internal::Benchmark
{
	class CellOffsetBenchmarkCase;

	class CellOffsetBenchmark final
	{
	public:
		CellOffsetBenchmark() = default;
		~CellOffsetBenchmark() noexcept = default;

		CellOffsetBenchmark(const CellOffsetBenchmark&) = delete;
		CellOffsetBenchmark(CellOffsetBenchmark&&) noexcept = default;

		auto operator=(const CellOffsetBenchmark&) -> CellOffsetBenchmark& = delete;
		auto operator=(CellOffsetBenchmark&&) noexcept -> CellOffsetBenchmark& = default;

		[[nodiscard]]
		auto GetCreationTimePoint() const noexcept
		{
			return _creationTimePoint;
		}

		void AddBenchmarkCase(REX::NotNull<std::shared_ptr<CellOffsetBenchmarkCase>> a_benchmark);

		void AddBenchmarkCases(std::span<const REX::NotNull<std::shared_ptr<CellOffsetBenchmarkCase>>> a_benchmarks);

		void RunBenchmark(const RE::TESDataHandler& a_dataHandler);

		[[nodiscard]]
		auto FormatBenchmarkResult() const -> std::string;

		void ClearBenchmarkResult() noexcept;

		[[nodiscard]]
		auto FormatBenchmarkFilePath(
			const std::filesystem::path& a_directoryPath,
			const std::filesystem::path& a_fileExtension,
			std::string_view a_extraData = {}) -> std::filesystem::path;

	private:
		std::chrono::system_clock::time_point _creationTimePoint{std::chrono::system_clock::now()};
		std::vector<REX::NotNull<std::shared_ptr<CellOffsetBenchmarkCase>>> _benchmarkCases;
	};
}
