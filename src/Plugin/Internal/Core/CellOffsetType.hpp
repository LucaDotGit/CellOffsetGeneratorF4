#pragma once

namespace Plugin::Internal::Core
{
	using CellOffsetType = std::uint32_t;

	void DeleteCellOffsetBuffer(CellOffsetType* a_ptr) noexcept;

	using CellOffsetBuffer = std::unique_ptr<CellOffsetType, decltype(&DeleteCellOffsetBuffer)>;

	auto CreateCellOffsetBuffer(std::size_t a_count) -> CellOffsetBuffer;
	auto CreateCellOffsetBufferForOverwrite(std::size_t a_count) -> CellOffsetBuffer;
}
