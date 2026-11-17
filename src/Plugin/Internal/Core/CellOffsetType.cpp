#include "Plugin/Internal/Core/CellOffsetType.hpp"

namespace Plugin::Internal::Core
{
	void DeleteCellOffsetBuffer(CellOffsetType* a_ptr) noexcept
	{
		RE::free(a_ptr);
	}

	auto CreateCellOffsetBuffer(std::size_t a_count) -> CellOffsetBuffer
	{
		if (a_count == 0) {
			return {nullptr, nullptr};
		}

		auto* ptr = RE::calloc<CellOffsetType>(a_count);
		if (!ptr) [[unlikely]] {
			throw std::bad_alloc();
		}

		return {ptr, DeleteCellOffsetBuffer};
	}

	auto CreateCellOffsetBufferForOverwrite(std::size_t a_count) -> CellOffsetBuffer
	{
		if (a_count == 0) {
			return {nullptr, nullptr};
		}

		auto* ptr = RE::malloc<CellOffsetType>(a_count);
		if (!ptr) [[unlikely]] {
			throw std::bad_alloc();
		}

		return {ptr, DeleteCellOffsetBuffer};
	}
}
