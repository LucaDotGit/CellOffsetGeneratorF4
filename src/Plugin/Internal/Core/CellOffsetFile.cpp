#include "Plugin/Internal/Core/CellOffsetFile.hpp"

namespace Plugin::Internal::Core
{
	auto CellOffsetFile::LoadFromFile(const std::filesystem::path& a_path) -> std::expected<CellOffsetFile, REX::SystemError>
	{
		auto fileStream = std::fstream(a_path, std::ios::in | std::ios::binary);
		if (!fileStream.is_open()) {
			return std::unexpected(REX::CreateSystemError(REX::PosixErrorCode::no_such_file_or_directory));
		}

		auto cellOffsetFile = CellOffsetFile();
		auto& headerInfo = cellOffsetFile.headerInfo;
		auto& offsetInfo = cellOffsetFile.offsetInfo;

		fileStream.read(reinterpret_cast<char*>(std::addressof(headerInfo.headerId)), sizeof(decltype(headerInfo.headerId)));
		if (!fileStream.good()) {
			return std::unexpected(REX::CreateSystemError(REX::PosixErrorCode::io_error));
		}

		fileStream.read(reinterpret_cast<char*>(std::addressof(headerInfo.headerVersion)), sizeof(decltype(headerInfo.headerVersion)));
		if (!fileStream.good()) {
			return std::unexpected(REX::CreateSystemError(REX::PosixErrorCode::io_error));
		}

		if (headerInfo.headerId != HEADER_ID || headerInfo.headerVersion != HEADER_VERSION) {
			return std::unexpected(REX::CreateSystemError(REX::PosixErrorCode::illegal_byte_sequence));
		}

		fileStream.read(reinterpret_cast<char*>(std::addressof(headerInfo.fileHash)), sizeof(decltype(headerInfo.fileHash)));
		if (!fileStream.good()) {
			return std::unexpected(REX::CreateSystemError(REX::PosixErrorCode::io_error));
		}

		fileStream.read(reinterpret_cast<char*>(std::addressof(offsetInfo.maxOffsetX)), sizeof(decltype(offsetInfo.maxOffsetX)));
		if (!fileStream.good()) {
			return std::unexpected(REX::CreateSystemError(REX::PosixErrorCode::io_error));
		}

		fileStream.read(reinterpret_cast<char*>(std::addressof(offsetInfo.maxOffsetY)), sizeof(decltype(offsetInfo.maxOffsetY)));
		if (!fileStream.good()) {
			return std::unexpected(REX::CreateSystemError(REX::PosixErrorCode::io_error));
		}

		fileStream.read(reinterpret_cast<char*>(std::addressof(offsetInfo.offsetHash)), sizeof(decltype(offsetInfo.offsetHash)));
		if (!fileStream.good()) {
			return std::unexpected(REX::CreateSystemError(REX::PosixErrorCode::io_error));
		}

		auto offsetBufferSize = static_cast<decltype(offsetInfo.offsetCount)>(0);
		fileStream.read(reinterpret_cast<char*>(std::addressof(offsetBufferSize)), sizeof(decltype(offsetBufferSize)));

		if (!fileStream.good()) {
			return std::unexpected(REX::CreateSystemError(REX::PosixErrorCode::io_error));
		}

		if (offsetBufferSize == 0) {
			if (fileStream.peek() != std::char_traits<char>::eof()) {
				return std::unexpected(REX::CreateSystemError(REX::PosixErrorCode::illegal_byte_sequence));
			}

			offsetInfo.offsetCount = 0;
			offsetInfo.offsetBuffer.reset();
			return std::move(cellOffsetFile);
		}

		auto offsetBuffer = Core::CreateCellOffsetBuffer(offsetBufferSize);
		fileStream.read(reinterpret_cast<char*>(offsetBuffer.get()), static_cast<std::streamsize>(sizeof(Core::CellOffsetType) * offsetBufferSize));

		if (!fileStream.good()) {
			return std::unexpected(REX::CreateSystemError(REX::PosixErrorCode::io_error));
		}

		offsetInfo.offsetCount = offsetBufferSize;
		offsetInfo.offsetBuffer = std::move(offsetBuffer);

		return std::move(cellOffsetFile);
	}

	REX::SystemError CellOffsetFile::SaveToFile(const std::filesystem::path& a_path)
	{
		auto fileStream = std::fstream(a_path, std::ios::out | std::ios::trunc | std::ios::binary);
		if (!fileStream.is_open()) {
			return REX::CreateSystemError(REX::PosixErrorCode::io_error);
		}

		fileStream.write(reinterpret_cast<const char*>(std::addressof(headerInfo.headerId)), sizeof(decltype(headerInfo.headerId)));
		if (!fileStream.good()) {
			return REX::CreateSystemError(REX::PosixErrorCode::io_error);
		}

		fileStream.write(reinterpret_cast<const char*>(std::addressof(headerInfo.headerVersion)), sizeof(decltype(headerInfo.headerVersion)));
		if (!fileStream.good()) {
			return REX::CreateSystemError(REX::PosixErrorCode::io_error);
		}

		fileStream.write(reinterpret_cast<const char*>(std::addressof(headerInfo.fileHash)), sizeof(decltype(headerInfo.fileHash)));
		if (!fileStream.good()) {
			return REX::CreateSystemError(REX::PosixErrorCode::io_error);
		}

		fileStream.write(reinterpret_cast<const char*>(std::addressof(offsetInfo.maxOffsetX)), sizeof(decltype(offsetInfo.maxOffsetX)));
		if (!fileStream.good()) {
			return REX::CreateSystemError(REX::PosixErrorCode::io_error);
		}

		fileStream.write(reinterpret_cast<const char*>(std::addressof(offsetInfo.maxOffsetY)), sizeof(decltype(offsetInfo.maxOffsetY)));
		if (!fileStream.good()) {
			return REX::CreateSystemError(REX::PosixErrorCode::io_error);
		}

		fileStream.write(reinterpret_cast<const char*>(std::addressof(offsetInfo.offsetHash)), sizeof(decltype(offsetInfo.offsetHash)));
		if (!fileStream.good()) {
			return REX::CreateSystemError(REX::PosixErrorCode::io_error);
		}

		const auto offsetCount = offsetInfo.offsetCount;

		fileStream.write(reinterpret_cast<const char*>(std::addressof(offsetCount)), sizeof(decltype(offsetCount)));
		if (!fileStream.good()) {
			return REX::CreateSystemError(REX::PosixErrorCode::io_error);
		}

		const auto& offsetBuffer = offsetInfo.offsetBuffer;
		if (offsetCount == 0 || !offsetBuffer) {
			return REX::CreateSystemError(REX::ERROR_NUMBER_SUCCESS);
		}

		fileStream.write(reinterpret_cast<const char*>(offsetBuffer.get()), static_cast<std::streamsize>(sizeof(Core::CellOffsetType) * offsetCount));
		if (!fileStream.good()) {
			return REX::CreateSystemError(REX::PosixErrorCode::io_error);
		}

		fileStream.flush();
		if (!fileStream.good()) {
			return REX::CreateSystemError(REX::PosixErrorCode::io_error);
		}

		fileStream.close();
		return REX::CreateSystemError(REX::ERROR_NUMBER_SUCCESS);
	}

	REX::SystemError CellOffsetFile::MarkFileAsReadOnly(const std::filesystem::path& a_path) noexcept
	{
		constexpr auto READONLY_REMOVAL_FLAGS =
			std::filesystem::perms::owner_write |
			std::filesystem::perms::group_write |
			std::filesystem::perms::others_write;

		auto permissionsError = REX::SystemError();
		std::filesystem::permissions(a_path,
			READONLY_REMOVAL_FLAGS,
			std::filesystem::perm_options::remove,
			permissionsError);

		return permissionsError;
	}
}
