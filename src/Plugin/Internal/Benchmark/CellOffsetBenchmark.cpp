#include "Plugin/Internal/Benchmark/CellOffsetBenchmark.hpp"

#include "Plugin/Internal/Benchmark/CellOffsetBenchmarkCase.hpp"

namespace Plugin::Internal::Benchmark
{
	void CellOffsetBenchmark::AddBenchmarkCase(REX::NotNull<std::shared_ptr<CellOffsetBenchmarkCase>> a_benchmark)
	{
		_benchmarkCases.push_back(std::move(a_benchmark));
	}

	void CellOffsetBenchmark::AddBenchmarkCases(
		std::span<const REX::NotNull<std::shared_ptr<CellOffsetBenchmarkCase>>> a_benchmarks)
	{
		_benchmarkCases.insert(_benchmarkCases.end(), a_benchmarks.begin(), a_benchmarks.end());
	}

	void CellOffsetBenchmark::RunBenchmark(const RE::TESDataHandler& a_dataHandler)
	{
		for (const auto& benchmark : _benchmarkCases) {
			benchmark->RunBenchmark(a_dataHandler);
		}
	}

	auto CellOffsetBenchmark::FormatBenchmarkResult() const -> std::string
	{
		if (_benchmarkCases.empty()) {
			return {};
		}

		auto result = std::string();
		result += "Cell (X, Y);"sv;

		for (auto benchmarkIt = _benchmarkCases.begin(); benchmarkIt != _benchmarkCases.end(); benchmarkIt++) {
			result += REX::Format("({}, {})"sv, (*benchmarkIt)->GetCellX(), (*benchmarkIt)->GetCellY());

			if (std::next(benchmarkIt) != _benchmarkCases.end()) {
				result += ';';
			}
		}

		result += '\n';
		result += "Duration (ms);"sv;

		for (auto benchmarkIt = _benchmarkCases.begin(); benchmarkIt != _benchmarkCases.end(); benchmarkIt++) {
			using DurationType = std::chrono::duration<REX::Float64, std::chrono::milliseconds::period>;

			result += REX::Format(
				"{:.03f}"sv,
				std::chrono::duration_cast<DurationType>((*benchmarkIt)->GetElapsedTime()).count());
			if (std::next(benchmarkIt) != _benchmarkCases.end()) {
				result += ';';
			}
		}

		return result;
	}

	void CellOffsetBenchmark::ClearBenchmarkResult() noexcept
	{
		for (const auto& benchmark : _benchmarkCases) {
			benchmark->ResetBenchmark();
		}
	}

	auto CellOffsetBenchmark::FormatBenchmarkFilePath(
		const std::filesystem::path& a_directoryPath,
		const std::filesystem::path& a_fileExtension,
		std::string_view a_extraData) -> std::filesystem::path
	{
		const auto fileName = REX::Format(
			"Benchmark_{}{}{}"sv,
			_creationTimePoint.time_since_epoch().count(),
			a_extraData.empty() ? ""sv : REX::Format("_{}"sv, a_extraData),
			a_fileExtension.generic_string());

		return {a_directoryPath / fileName, std::filesystem::path::generic_format};
	}
}
