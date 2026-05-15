#include "Plugin/Internal/Benchmark/CellOffsetBenchmark.hpp"

#include "Plugin/Internal/Benchmark/CellOffsetBenchmarkCase.hpp"

namespace Plugin::Internal::Benchmark
{
	bool CellOffsetBenchmark::CreateAndAddBenchmarkCase(
		std::string_view a_worldSpaceEditorId,
		std::int32_t a_x,
		std::int32_t a_y,
		std::uint32_t a_iterationCount)
	{
		const auto* dataHandler = RE::TESDataHandler::GetSingleton();
		if (!dataHandler) [[unlikely]] {
			REX::Fail("Failed to get the game's data handler."sv);
		}

		auto* worldSpace = RE::TESForm::FindFormByEditorID<RE::TESWorldSpace>(RE::BSFixedString(a_worldSpaceEditorId));
		if (!worldSpace) {
			REX::LogError(R"(Failed to find world space with editor ID "{}")"sv,
				a_worldSpaceEditorId);
			return false;
		}

		auto benchmarkCase = REX::NotNull(std::make_shared<CellOffsetBenchmarkCase>(worldSpace, a_x, a_y, a_iterationCount));
		AddBenchmarkCase(std::move(benchmarkCase));
		return true;
	}

	void CellOffsetBenchmark::AddBenchmarkCase(REX::NotNull<std::shared_ptr<CellOffsetBenchmarkCase>> a_benchmark)
	{
		_benchmarkCases.push_back(std::move(a_benchmark));
	}

	void CellOffsetBenchmark::RunBenchmark()
	{
		for (const auto& benchmark : _benchmarkCases) {
			benchmark->RunBenchmark();
		}
	}

	std::string CellOffsetBenchmark::FormatBenchmarkResult() const
	{
		if (_benchmarkCases.empty()) {
			return {};
		}

		auto result = std::string();
		result += "Cell (X, Y);"sv;

		for (auto benchmarkIt = _benchmarkCases.begin(); benchmarkIt != _benchmarkCases.end(); benchmarkIt++) {
			result += REX::Format("({}, {})"sv,
				(*benchmarkIt)->GetCellX(), (*benchmarkIt)->GetCellY());

			if (std::next(benchmarkIt) != _benchmarkCases.end()) {
				result += ';';
			}
		}

		result += '\n';
		result += "Duration (ms);"sv;

		for (auto benchmarkIt = _benchmarkCases.begin(); benchmarkIt != _benchmarkCases.end(); benchmarkIt++) {
			using MillisType = std::chrono::duration<REX::Float64, std::chrono::milliseconds::period>;

			result += REX::Format("{:.03f}"sv, std::chrono::duration_cast<MillisType>((*benchmarkIt)->GetElapsedTime()).count());
			if (std::next(benchmarkIt) != _benchmarkCases.end()) {
				result += ';';
			}
		}

		return result;
	}

	REX::SystemError CellOffsetBenchmark::SaveBenchmarkResult(const std::filesystem::path& a_filePath) const
	{
		auto createDirsError = REX::SystemError();
		if (!REX::CreateParentDirectories(a_filePath, createDirsError) && createDirsError.value() != REX::ERROR_NUMBER_SUCCESS) [[unlikely]] {
			return createDirsError;
		}

		auto fileStream = std::fstream(a_filePath, std::ios::out | std::ios::trunc);
		if (!fileStream.is_open()) {
			return REX::CreateSystemError(REX::PosixErrorCode::io_error);
		}

		auto content = FormatBenchmarkResult();

		fileStream << content;
		if (!fileStream.good()) {
			return REX::CreateSystemError(REX::PosixErrorCode::io_error);
		}

		return REX::CreateSystemError(REX::ERROR_NUMBER_SUCCESS);
	}

	void CellOffsetBenchmark::ClearBenchmarkResult() noexcept
	{
		for (const auto& benchmark : _benchmarkCases) {
			benchmark->ResetBenchmark();
		}
	}

	std::filesystem::path CellOffsetBenchmark::FormatBenchmarkFilePath(
		const std::filesystem::path& a_directoryPath,
		const std::filesystem::path& a_fileExtension,
		std::string_view a_extraData)
	{
		const auto fileName = REX::Format("Benchmark_{}{}{}"sv,
			_creationTimePoint.time_since_epoch().count(),
			a_extraData.empty() ? ""sv : REX::Format("_{}"sv, a_extraData),
			a_fileExtension.generic_string());

		return {
			a_directoryPath / fileName,
			std::filesystem::path::generic_format
		};
	}
}
