#include "Plugin/Internal/Core/CellOffsetHasher.hpp"

namespace Plugin::Internal::Core
{
	void DeleteHashState(HashStateType* a_hashState) noexcept
	{
		if (XXH3_freeState(a_hashState) != XXH_OK) [[unlikely]] {
			REX::Assert(false);
		}
	}

	auto CreateHashState() -> REX::NotNull<HashStateBuffer>
	{
		auto* ptr = XXH3_createState();
		if (!ptr) [[unlikely]] {
			REX::AllocationFail("Failed to allocate hash state."sv);
		}

		return HashStateBuffer{
			ptr,
			DeleteHashState
		};
	}
}

namespace Plugin::Internal::Core::CellOffsetHasher
{
	auto CreateHashForFile(
		REX::NotNull<RE::TESFile*> a_file,
		const REX::NotNull<Core::HashStateBuffer>& a_hashState) -> std::expected<HashType, REX::SystemError>
	{
		auto* fileImpl = a_file->file;
		if (!fileImpl) [[unlikely]] {
			REX::Assert(false);
			return std::unexpected(REX::CreateSystemError(REX::PosixErrorCode::bad_file_descriptor));
		}

		a_file->TESRewind();

		auto bufferSize = a_file->GetBufferSize();
		if (bufferSize == 0) {
			bufferSize = RE::TESFile::DEFAULT_BUFFER_SIZE;
			a_file->ChangeBufferSize(bufferSize);
		}

		auto buffer = std::vector<std::byte>();
		buffer.resize(bufferSize);

		if (XXH3_64bits_reset(a_hashState.get().get()) != XXH_OK) [[unlikely]] {
			REX::Assert(false);
			return std::unexpected(REX::CreateSystemError(REX::PosixErrorCode::state_not_recoverable));
		}

		while (true) {
			const auto readBytes = fileImpl->BinaryRead(buffer.data(), buffer.size());
			if (readBytes == 0) {
				break;
			}

			if (XXH3_64bits_update(a_hashState.get().get(), buffer.data(), readBytes) != XXH_OK) [[unlikely]] {
				REX::Assert(false);
				return std::unexpected(REX::CreateSystemError(REX::PosixErrorCode::state_not_recoverable));
			}
		}

		return XXH3_64bits_digest(a_hashState.get().get());
	}

	auto CreateHashForOffsets(
		const Core::CellOffsetType* a_offsets,
		std::size_t a_offsetCount,
		const REX::NotNull<Core::HashStateBuffer>& a_hashState) -> std::expected<HashType, REX::SystemError>
	{
		if (!a_offsets || a_offsetCount == 0) {
			return std::unexpected(REX::CreateSystemError(REX::PosixErrorCode::invalid_argument));
		}

		if (XXH3_64bits_reset(a_hashState.get().get()) != XXH_OK) [[unlikely]] {
			REX::Assert(false);
			return std::unexpected(REX::CreateSystemError(REX::PosixErrorCode::state_not_recoverable));
		}

		if (XXH3_64bits_update(a_hashState.get().get(), a_offsets, sizeof(Core::CellOffsetType) * a_offsetCount) != XXH_OK) [[unlikely]] {
			REX::Assert(false);
			return std::unexpected(REX::CreateSystemError(REX::PosixErrorCode::state_not_recoverable));
		}

		return XXH3_64bits_digest(a_hashState.get().get());
	}
}
