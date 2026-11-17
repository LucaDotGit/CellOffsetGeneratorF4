#include "Plugin/Internal/Benchmark/CellOffsetBenchmarkCase.hpp"

namespace Plugin::Internal::Benchmark
{
	CellOffsetBenchmarkCase::CellOffsetBenchmarkCase(
		REX::NotNull<RE::TESWorldSpace*> a_worldSpace,
		std::int32_t a_cellX,
		std::int32_t a_cellY,
		std::uint32_t a_iterationCount) :
		_worldSpace(a_worldSpace),
		_cellX(a_cellX),
		_cellY(a_cellY),
		_iterationCount(a_iterationCount)
	{
	}

	void CellOffsetBenchmarkCase::RunBenchmark(const RE::TESDataHandler& a_dataHandler)
	{
		_stopwatch.Start();

		for (auto i = static_cast<decltype(_iterationCount)>(0); i < _iterationCount; i++) {
			RunBenchmarkImpl(a_dataHandler);
		}

		_stopwatch.Stop();
	}

	void CellOffsetBenchmarkCase::ResetBenchmark() noexcept
	{
		_stopwatch.Reset();
	}

	auto CellOffsetBenchmarkCase::GetWorldSpaceByEditorId(const RE::BSFixedString& a_editorId) -> RE::TESWorldSpace*
	{
		return RE::TESForm::FindFormByEditorID<RE::TESWorldSpace>(a_editorId);
	}

	void CellOffsetBenchmarkCase::RunBenchmarkImpl(const RE::TESDataHandler& a_dataHandler)
	{
		for (auto* worldSpace : a_dataHandler.GetFormArray<RE::TESWorldSpace>()) {
			if (!worldSpace || worldSpace != _worldSpace.get()) {
				continue;
			}

			const auto* worldSpaceFiles = worldSpace->sourceFiles.array;
			if (!worldSpaceFiles) {
				continue;
			}

			for (auto* file : *worldSpaceFiles) {
				if (!file) {
					continue;
				}

				std::ignore = worldSpace->FindCellInFile(file, _cellX, _cellY);
			}
		}
	}
}
