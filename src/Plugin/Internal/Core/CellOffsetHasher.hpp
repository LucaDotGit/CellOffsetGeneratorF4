#pragma once

#include "Plugin/Internal/Core/CellOffsetType.hpp"

namespace Plugin::Internal::Core
{
	using HashType = XXH64_hash_t;

	using HashStateType = XXH3_state_t;

	void DeleteHashState(HashStateType* a_hashState) noexcept;

	using HashStateBuffer = std::unique_ptr<HashStateType, decltype(&DeleteHashState)>;

	auto CreateHashState() -> REX::NotNull<HashStateBuffer>;
}

namespace Plugin::Internal::Core::CellOffsetHasher
{
	[[nodiscard]] auto CreateHashForFile(
		REX::NotNull<RE::TESFile*> a_file,
		const REX::NotNull<Core::HashStateBuffer>& a_hashState) -> std::expected<HashType, REX::SystemError>;

	[[nodiscard]] auto CreateHashForOffsets(
		const CellOffsetType* a_offsets,
		std::size_t a_offsetCount,
		const REX::NotNull<Core::HashStateBuffer>& a_hashState) -> std::expected<HashType, REX::SystemError>;
}
