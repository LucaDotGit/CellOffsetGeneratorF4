#pragma once

namespace Plugin::Internal::Benchmark
{
	class CellOffsetBenchmarkCase final
	{
	public:
		CellOffsetBenchmarkCase(
			REX::NotNull<RE::TESWorldSpace*> a_worldSpace,
			std::int32_t a_cellX,
			std::int32_t a_cellY,
			std::uint32_t a_iterationCount);

		~CellOffsetBenchmarkCase() noexcept = default;

		CellOffsetBenchmarkCase(const CellOffsetBenchmarkCase&) = delete;
		CellOffsetBenchmarkCase(CellOffsetBenchmarkCase&&) noexcept = default;

		auto operator=(const CellOffsetBenchmarkCase&) -> CellOffsetBenchmarkCase& = delete;
		auto operator=(CellOffsetBenchmarkCase&&) noexcept -> CellOffsetBenchmarkCase& = default;

		[[nodiscard]]
		auto GetWorldSpace() const noexcept
		{
			return _worldSpace;
		}

		[[nodiscard]]
		auto GetCellX() const noexcept
		{
			return _cellX;
		}

		[[nodiscard]]
		auto GetCellY() const noexcept
		{
			return _cellY;
		}

		[[nodiscard]]
		auto GetIterationCount() const noexcept
		{
			return _iterationCount;
		}

		[[nodiscard]]
		auto GetElapsedTime() const noexcept
		{
			return _stopwatch.GetElapsedTime() / _iterationCount;
		}

		void RunBenchmark(const RE::TESDataHandler& a_dataHandler);
		void ResetBenchmark() noexcept;

		[[nodiscard]]
		static auto GetWorldSpaceByEditorId(const RE::BSFixedString& a_editorId) -> RE::TESWorldSpace*;

	private:
		void RunBenchmarkImpl(const RE::TESDataHandler& a_dataHandler);

		REX::NotNull<RE::TESWorldSpace*> _worldSpace;
		std::int32_t _cellX;
		std::int32_t _cellY;
		std::uint32_t _iterationCount;
		REX::Stopwatch _stopwatch;
	};
}
