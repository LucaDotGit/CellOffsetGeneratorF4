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

		CellOffsetBenchmark& operator=(const CellOffsetBenchmark&) = delete;
		CellOffsetBenchmark& operator=(CellOffsetBenchmark&&) noexcept = default;

		[[nodiscard]] auto GetCreationTimePoint() const noexcept { return _creationTimePoint; }

		bool CreateAndAddBenchmarkCase(
			std::string_view a_worldSpaceEditorId,
			std::int32_t a_x,
			std::int32_t a_y,
			std::uint32_t a_iterationCount);

		void AddBenchmarkCase(REX::NotNull<std::shared_ptr<CellOffsetBenchmarkCase>> a_benchmark);

		void RunBenchmark();

		[[nodiscard]] std::string FormatBenchmarkResult() const;

		REX::SystemError SaveBenchmarkResult(const std::filesystem::path& a_filePath) const;

		void ClearBenchmarkResult() noexcept;

		[[nodiscard]] std::filesystem::path FormatBenchmarkFilePath(
			const std::filesystem::path& a_directoryPath,
			const std::filesystem::path& a_fileExtension,
			std::string_view a_extraData = {});

	private:
		std::chrono::system_clock::time_point _creationTimePoint{ std::chrono::system_clock::now() };
		std::vector<REX::NotNull<std::shared_ptr<CellOffsetBenchmarkCase>>> _benchmarkCases;
	};
}
