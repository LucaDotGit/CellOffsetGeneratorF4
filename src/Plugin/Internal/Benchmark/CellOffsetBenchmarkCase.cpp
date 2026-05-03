#include "Plugin/Internal/Benchmark/CellOffsetBenchmarkCase.hpp"

namespace Plugin::Internal::Benchmark
{
	CellOffsetBenchmarkCase::CellOffsetBenchmarkCase(
		const REX::NotNull<RE::TESWorldSpace*>& a_worldSpace,
		std::int32_t a_cellX,
		std::int32_t a_cellY,
		std::uint32_t a_iterationCount)
		: _worldSpace(a_worldSpace),
		  _cellX(a_cellX),
		  _cellY(a_cellY),
		  _iterationCount(a_iterationCount)
	{
	}

	void CellOffsetBenchmarkCase::RunBenchmark()
	{
		const auto* dataHandler = RE::TESDataHandler::GetSingleton();
		if (!dataHandler) [[unlikely]] {
			REX::Fail("Failed to get the game's data handler."sv);
		}

		_stopwatch.Start();

		for (auto i = 0ui32; i < _iterationCount; i++) {
			RunBenchmarkImpl(dataHandler);
		}

		_stopwatch.Stop();
	}

	void CellOffsetBenchmarkCase::ResetBenchmark() noexcept
	{
		_stopwatch.Reset();
	}

	void CellOffsetBenchmarkCase::RunBenchmarkImpl(const RE::TESDataHandler* a_dataHandler)
	{
		for (auto* worldSpace : a_dataHandler->GetFormArray<RE::TESWorldSpace>()) {
			if (!worldSpace) {
				continue;
			}

			if (worldSpace != _worldSpace.get()) {
				continue;
			}

			auto* worldSpaceFiles = worldSpace->sourceFiles.array;
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
