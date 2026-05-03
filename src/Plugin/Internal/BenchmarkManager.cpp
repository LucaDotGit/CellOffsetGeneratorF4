#include "Plugin/Internal/BenchmarkManager.hpp"

#include "Plugin/Internal/Benchmark/CellOffsetBenchmark.hpp"
#include "Plugin/Internal/PatchesManager.hpp"
#include "Plugin/Internal/SettingsManager.hpp"

namespace Plugin::Internal
{
	BenchmarkManager::BenchmarkManager(
		std::filesystem::path a_cellOffsetBenchmarksDirectoryPath,
		std::filesystem::path a_cellOffsetBenchmarkFileExtension,
		const REX::NotNull<std::shared_ptr<SettingsManager>>& a_settingsManager,
		const REX::NotNull<std::shared_ptr<PatchesManager>>& a_patchesManager)
		: _cellOffsetBenchmarksDirectoryPath(std::move(a_cellOffsetBenchmarksDirectoryPath)),
		  _cellOffsetBenchmarkFileExtension(std::move(a_cellOffsetBenchmarkFileExtension)),
		  _settingsManager(a_settingsManager),
		  _patchesManager(a_patchesManager),
		  _cellOffsetBenchmark(std::make_shared<Benchmark::CellOffsetBenchmark>())
	{
	}

	BenchmarkManager::~BenchmarkManager() noexcept = default;

	void BenchmarkManager::OnGameDataReady()
	{
		REX::LogDebug("{}"sv,
			REX::GetCurrentFunctionName());

		if (!_settingsManager->GetRunBenchmarkSetting()->GetValue()) {
			return;
		}

		LoadBenchmarkCasesFromSettings();
	}

	void BenchmarkManager::RunBenchmarks_Before()
	{
		if (!_settingsManager->GetRunBenchmarkSetting()->GetValue()) {
			return;
		}

		_patchesManager->Disable();

		RunBenchmarksImpl("Before"sv);

		_patchesManager->Enable();
	}

	void BenchmarkManager::RunBenchmarks_After()
	{
		if (!_settingsManager->GetRunBenchmarkSetting()->GetValue()) {
			return;
		}

		RunBenchmarksImpl("After"sv);
	}

	void BenchmarkManager::LoadBenchmarkCasesFromSettings()
	{
		constexpr auto WORLD_SPACE_KEY = "sWorldSpace"sv;
		constexpr auto CELL_X_KEY = "iCellX"sv;
		constexpr auto CELL_Y_KEY = "iCellY"sv;

		const auto benchmarkIterationCount = _settingsManager->GetBenchmarkIterationCountSetting()->GetValue();
		const auto benchmarkCasesArray = _settingsManager->GetBenchmarkCasesSetting()->GetValue();

		for (const auto& benchmarkCaseValue : benchmarkCasesArray) {
			if (!benchmarkCaseValue.is_table()) {
				continue;
			}

			constexpr auto MISSING_KEY_WARNING =
				R"(Missing or invalid key "{}" in benchmark case)"sv;

			const auto& benchmarkCaseTable = benchmarkCaseValue.as_table();

			const auto worldSpaceEditorIdIt = benchmarkCaseTable.find(std::string(WORLD_SPACE_KEY));
			if (worldSpaceEditorIdIt == benchmarkCaseTable.end() || !worldSpaceEditorIdIt->second.is_string()) [[unlikely]] {
				REX::LogWarning(MISSING_KEY_WARNING,
					WORLD_SPACE_KEY);
				continue;
			}

			const auto cellXIt = benchmarkCaseTable.find(std::string(CELL_X_KEY));
			if (cellXIt == benchmarkCaseTable.end() || !cellXIt->second.is_integer()) [[unlikely]] {
				REX::LogWarning(MISSING_KEY_WARNING,
					CELL_X_KEY);
				continue;
			}

			const auto cellYIt = benchmarkCaseTable.find(std::string(CELL_Y_KEY));
			if (cellYIt == benchmarkCaseTable.end() || !cellYIt->second.is_integer()) [[unlikely]] {
				REX::LogWarning(MISSING_KEY_WARNING,
					CELL_Y_KEY);
				continue;
			}

			const auto worldSpaceEditorId = RE::BSFixedString(worldSpaceEditorIdIt->second.as_string());
			const auto cellX = static_cast<std::int32_t>(cellXIt->second.as_integer());
			const auto cellY = static_cast<std::int32_t>(cellYIt->second.as_integer());

			_cellOffsetBenchmark->CreateAndAddBenchmarkCase(
				worldSpaceEditorId,
				cellX,
				cellY,
				benchmarkIterationCount);
		}
	}

	void BenchmarkManager::RunBenchmarksImpl(std::string_view a_extraData)
	{
		REX::LogDebug(R"(Running Cell Offset Benchmark ("{}"))"sv,
			a_extraData);

		_cellOffsetBenchmark->RunBenchmark();

		const auto saveError = _cellOffsetBenchmark->SaveBenchmarkResult(_cellOffsetBenchmarksDirectoryPath, _cellOffsetBenchmarkFileExtension, a_extraData);
		if (saveError.value() != 0) [[unlikely]] {
			REX::LogError("Failed to save benchmark results to file: {}"sv,
				saveError.message());
		}

		_cellOffsetBenchmark->ClearBenchmarkResult();
	}
}
