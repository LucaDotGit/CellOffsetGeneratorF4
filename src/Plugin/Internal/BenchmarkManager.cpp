#include "Plugin/Internal/BenchmarkManager.hpp"

#include "Plugin/Internal/Benchmark/CellOffsetBenchmark.hpp"
#include "Plugin/Internal/Benchmark/CellOffsetBenchmarkCase.hpp"
#include "Plugin/Internal/PatchesManager.hpp"
#include "Plugin/Internal/SettingsManager.hpp"

namespace Plugin::Internal
{
	BenchmarkManager::BenchmarkManager(
		const REX::NotNull<std::shared_ptr<REX::Logging::ILogger>>& a_logger,
		std::filesystem::path a_benchmarksDirectoryPath,
		std::filesystem::path a_benchmarkFileExtension,
		const REX::NotNull<std::shared_ptr<SettingsManager>>& a_settingsManager,
		const REX::NotNull<std::shared_ptr<PatchesManager>>& a_patchesManager) :
		_logger(a_logger),
		_benchmarksDirectoryPath(std::move(a_benchmarksDirectoryPath)),
		_benchmarkFileExtension(std::move(a_benchmarkFileExtension)),
		_settingsManager(a_settingsManager),
		_patchesManager(a_patchesManager),
		_cellOffsetBenchmark(std::make_shared<Benchmark::CellOffsetBenchmark>())
	{
	}

	BenchmarkManager::~BenchmarkManager() noexcept = default;

	void BenchmarkManager::OnGameDataReady([[maybe_unused]] bool a_isReady)
	{
		_logger->LogDebug("{}"sv, REX::GetCurrentFunctionName());

		if (!_settingsManager->GetRunBenchmarkSetting()->GetValue()) {
			return;
		}

		const auto benchmarkCases = LoadBenchmarkCasesFromSettings();
		_cellOffsetBenchmark->AddBenchmarkCases(benchmarkCases);
	}

	void BenchmarkManager::RunBenchmarks_Before()
	{
		constexpr auto BENCHMARK_EXTRA_DATA = "Before"sv;

		if (!_settingsManager->GetRunBenchmarkSetting()->GetValue()) {
			return;
		}

		_patchesManager->Disable();

		const auto runBenchmarksResult = RunBenchmarksImpl(BENCHMARK_EXTRA_DATA);
		if (!runBenchmarksResult.has_value()) [[unlikely]] {
			_logger->LogError(
				R"(Failed to run benchmarks "{}": {})"sv,
				BENCHMARK_EXTRA_DATA,
				runBenchmarksResult.error());
		}

		_patchesManager->Enable();
	}

	void BenchmarkManager::RunBenchmarks_After()
	{
		constexpr auto BENCHMARK_EXTRA_DATA = "After"sv;

		if (!_settingsManager->GetRunBenchmarkSetting()->GetValue()) {
			return;
		}

		const auto runBenchmarksResult = RunBenchmarksImpl(BENCHMARK_EXTRA_DATA);
		if (!runBenchmarksResult.has_value()) [[unlikely]] {
			_logger->LogError(
				R"(Failed to run benchmarks "{}": {})"sv,
				BENCHMARK_EXTRA_DATA,
				runBenchmarksResult.error());
		}
	}

	auto BenchmarkManager::LoadBenchmarkCasesFromSettings()
		-> std::vector<REX::NotNull<std::shared_ptr<Benchmark::CellOffsetBenchmarkCase>>>
	{
		const auto benchmarkIterationCount = _settingsManager->GetBenchmarkIterationCountSetting()->GetValue();

		auto result = std::vector<REX::NotNull<std::shared_ptr<Benchmark::CellOffsetBenchmarkCase>>>();

		for (const auto& benchmarkCase : _settingsManager->GetBenchmarkCasesSetting()->GetValue()) {
			auto* worldSpace = Benchmark::CellOffsetBenchmarkCase::GetWorldSpaceByEditorId(
				RE::BSFixedString(benchmarkCase.WorldSpace));
			if (!worldSpace) [[unlikely]] {
				continue;
			}

			result.emplace_back(
				std::make_shared<Benchmark::CellOffsetBenchmarkCase>(
					worldSpace,
					benchmarkCase.CellX,
					benchmarkCase.CellY,
					benchmarkIterationCount));
		}

		return result;
	}

	auto BenchmarkManager::RunBenchmarksImpl(std::string_view a_extraData) -> std::expected<void, std::string>
	{
		const auto* dataHandler = RE::TESDataHandler::GetSingleton();
		if (!dataHandler) [[unlikely]] {
			REX::Fail("Failed to get the game's data handler."sv);
		}

		_cellOffsetBenchmark->RunBenchmark(*dataHandler);

		const auto filePath = _cellOffsetBenchmark->FormatBenchmarkFilePath(
			_benchmarksDirectoryPath,
			_benchmarkFileExtension,
			a_extraData);

		const auto fileContent = _cellOffsetBenchmark->FormatBenchmarkResult();

		const auto saveError = REX::WriteAllTextToFile(filePath, std::string_view(fileContent));

		_cellOffsetBenchmark->ClearBenchmarkResult();

		if (saveError.value() != REX::ERROR_NUMBER_SUCCESS) [[unlikely]] {
			return std::unexpected(saveError.message());
		}

		return {};
	}
}
