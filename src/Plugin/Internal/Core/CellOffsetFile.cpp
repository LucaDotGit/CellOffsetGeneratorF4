#include "Plugin/Internal/Core/CellOffsetFile.hpp"

namespace Plugin::Internal::Core
{
	auto CellOffsetFile::LoadFromFile(std::filesystem::path a_path) noexcept
		-> std::expected<CellOffsetFile, REX::SystemError>
	{
		auto fileStream = REX::FileStream::Open(
			std::move(a_path),
			REX::FileMode::Open,
			REX::FileAccess::Read,
			REX::FileShare::Read,
			REX::FileOptions::SequentialScan);
		if (!fileStream.has_value()) [[unlikely]] {
			return std::unexpected(fileStream.error());
		}

		const auto readHeaderHash = fileStream->Read<std::uint32_t>();
		if (!readHeaderHash.has_value()) [[unlikely]] {
			return std::unexpected(readHeaderHash.error());
		}

		const auto readHeaderVersion = fileStream->Read<std::uint32_t>();
		if (!readHeaderVersion.has_value()) [[unlikely]] {
			return std::unexpected(readHeaderVersion.error());
		}

		if (readHeaderHash.value() != HeaderInfo::HEADER_HASH ||
			readHeaderVersion.value() != HeaderInfo::HEADER_VERSION) [[unlikely]] {
			return std::unexpected(REX::CreateSystemError(REX::PosixErrorCode::bad_message));
		}

		const auto readHeaderFileHash = fileStream->Read<Core::HashType>();
		if (!readHeaderFileHash.has_value()) [[unlikely]] {
			return std::unexpected(readHeaderFileHash.error());
		}

		const auto readOffsetMaxX = fileStream->Read<std::int32_t>();
		if (!readOffsetMaxX.has_value()) [[unlikely]] {
			return std::unexpected(readOffsetMaxX.error());
		}

		const auto readOffsetMaxY = fileStream->Read<std::int32_t>();
		if (!readOffsetMaxY.has_value()) [[unlikely]] {
			return std::unexpected(readOffsetMaxY.error());
		}

		const auto readOffsetHash = fileStream->Read<Core::HashType>();
		if (!readOffsetHash.has_value()) [[unlikely]] {
			return std::unexpected(readOffsetHash.error());
		}

		const auto readOffsetCount = fileStream->Read<std::uint32_t>();
		if (!readOffsetCount.has_value()) [[unlikely]] {
			return std::unexpected(readOffsetCount.error());
		}

		auto offsetBuffer = Core::CreateCellOffsetBufferForOverwrite(readOffsetCount.value());

		const auto readOffsetBufferError = fileStream->Read(offsetBuffer.get(), readOffsetCount.value());
		if (readOffsetBufferError.value() != REX::ERROR_NUMBER_SUCCESS) [[unlikely]] {
			return std::unexpected(readOffsetBufferError);
		}

		const auto endOfFileResult = fileStream->Peek();
		if (endOfFileResult.has_value()) {
			return std::unexpected(REX::CreateSystemError(REX::PosixErrorCode::message_size));
		}

		return CellOffsetFile(
			HeaderInfo(readHeaderFileHash.value()),
			CellInfo(
				readOffsetMaxX.value(),
				readOffsetMaxY.value(),
				readOffsetHash.value(),
				readOffsetCount.value(),
				std::move(offsetBuffer)));
	}

	auto CellOffsetFile::SaveToFile(std::filesystem::path a_path) const noexcept -> REX::SystemError
	{
		const auto createDirsResult = REX::CreateParentDirectories(a_path);
		if (!createDirsResult.has_value() && createDirsResult.error().value() != REX::ERROR_NUMBER_SUCCESS)
			[[unlikely]] {
			return createDirsResult.error();
		}

		{
			std::ignore = RemoveFileReadOnlyFlag(a_path);
		}

		auto fileStream = REX::FileStream::Open(
			std::move(a_path),
			REX::FileMode::Create,
			REX::FileAccess::Write,
			REX::FileShare::Read,
			REX::FileOptions::SequentialScan);
		if (!fileStream.has_value()) [[unlikely]] {
			return fileStream.error();
		}

		const auto& headerInfo = GetHeaderInfo();

		const auto writeHeaderHashError = fileStream->Write(headerInfo.GetHeaderHash());
		if (writeHeaderHashError.value() != REX::ERROR_NUMBER_SUCCESS) [[unlikely]] {
			return writeHeaderHashError;
		}

		const auto writeHeaderVersionError = fileStream->Write(headerInfo.GetHeaderVersion());
		if (writeHeaderVersionError.value() != REX::ERROR_NUMBER_SUCCESS) [[unlikely]] {
			return writeHeaderVersionError;
		}

		const auto writeHeaderFileHashError = fileStream->Write(headerInfo.GetFileHash());
		if (writeHeaderFileHashError.value() != REX::ERROR_NUMBER_SUCCESS) [[unlikely]] {
			return writeHeaderFileHashError;
		}

		const auto& cellInfo = GetCellInfo();

		const auto writeCellMaxXError = fileStream->Write(cellInfo.GetMaxOffsetX());
		if (writeCellMaxXError.value() != REX::ERROR_NUMBER_SUCCESS) [[unlikely]] {
			return writeCellMaxXError;
		}

		const auto writeCellMaxYError = fileStream->Write(cellInfo.GetMaxOffsetY());
		if (writeCellMaxYError.value() != REX::ERROR_NUMBER_SUCCESS) [[unlikely]] {
			return writeCellMaxYError;
		}

		const auto writeCellHashError = fileStream->Write(cellInfo.GetOffsetHash());
		if (writeCellHashError.value() != REX::ERROR_NUMBER_SUCCESS) [[unlikely]] {
			return writeCellHashError;
		}

		const auto writeCellCountError = fileStream->Write(cellInfo.GetOffsetCount());
		if (writeCellCountError.value() != REX::ERROR_NUMBER_SUCCESS) [[unlikely]] {
			return writeCellCountError;
		}

		const auto& offsetBuffer = cellInfo.GetOffsetBuffer();
		if (!offsetBuffer) {
			return REX::CreateSystemError(REX::ERROR_NUMBER_SUCCESS);
		}

		const auto writeOffsetBufferError = fileStream->Write(offsetBuffer.get(), cellInfo.GetOffsetCount());
		if (writeOffsetBufferError.value() != REX::ERROR_NUMBER_SUCCESS) [[unlikely]] {
			return writeOffsetBufferError;
		}

		return REX::CreateSystemError(REX::ERROR_NUMBER_SUCCESS);
	}

	auto CellOffsetFile::ChangeFileWritePermissions(
		const std::filesystem::path& a_path,
		std::filesystem::perm_options a_options) noexcept -> REX::SystemError
	{
		constexpr auto ALL_WRITE_FLAGS = std::filesystem::perms::owner_write |
										 std::filesystem::perms::group_write |
										 std::filesystem::perms::others_write;

		auto permissionsError = REX::SystemError();
		std::filesystem::permissions(a_path, ALL_WRITE_FLAGS, a_options, permissionsError);

		return permissionsError;
	}

	auto CellOffsetFile::AddFileReadOnlyFlag(const std::filesystem::path& a_path) noexcept -> REX::SystemError
	{
		return ChangeFileWritePermissions(a_path, std::filesystem::perm_options::remove);
	}

	auto CellOffsetFile::RemoveFileReadOnlyFlag(const std::filesystem::path& a_path) noexcept -> REX::SystemError
	{
		return ChangeFileWritePermissions(a_path, std::filesystem::perm_options::add);
	}
}
