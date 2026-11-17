#pragma once

#include "Plugin/Internal/Core/CellOffsetHasher.hpp"
#include "Plugin/Internal/Core/CellOffsetType.hpp"

namespace Plugin::Internal::Core
{
	class CellOffsetFile final
	{
	public:
		class HeaderInfo final
		{
		public:
			inline static constexpr auto HEADER_HASH = REX::HashCrc32("CellOffsetFile"sv);
			inline static constexpr auto HEADER_VERSION = static_cast<std::uint32_t>(1);

			explicit HeaderInfo(Core::HashType a_fileHash) noexcept :
				_fileHash(a_fileHash)
			{
			}

			~HeaderInfo() noexcept = default;

			HeaderInfo(const HeaderInfo&) = delete;
			HeaderInfo(HeaderInfo&&) noexcept = default;

			auto operator=(const HeaderInfo&) -> HeaderInfo& = delete;
			auto operator=(HeaderInfo&&) noexcept -> HeaderInfo& = default;

			[[nodiscard]]
			auto GetHeaderHash() const noexcept
			{
				return _headerHash;
			}

			[[nodiscard]]
			auto GetHeaderVersion() const noexcept
			{
				return _headerVersion;
			}

			[[nodiscard]]
			auto GetFileHash() const noexcept
			{
				return _fileHash;
			}

		private:
			std::uint32_t _headerHash{HEADER_HASH};
			std::uint32_t _headerVersion{HEADER_VERSION};
			Core::HashType _fileHash;
		};

		class CellInfo final
		{
		public:
			explicit CellInfo(
				std::int32_t a_maxOffsetX,
				std::int32_t a_maxOffsetY,
				Core::HashType a_offsetHash,
				std::uint32_t a_offsetCount) noexcept :
				_maxOffsetX(a_maxOffsetX),
				_maxOffsetY(a_maxOffsetY),
				_offsetHash(a_offsetHash),
				_offsetCount(a_offsetCount)
			{
			}

			explicit CellInfo(
				std::int32_t a_maxOffsetX,
				std::int32_t a_maxOffsetY,
				Core::HashType a_offsetHash,
				std::uint32_t a_offsetCount,
				Core::CellOffsetBuffer a_offsetBuffer) noexcept :
				_maxOffsetX(a_maxOffsetX),
				_maxOffsetY(a_maxOffsetY),
				_offsetHash(a_offsetHash),
				_offsetCount(a_offsetCount),
				_offsetBuffer(std::move(a_offsetBuffer))
			{
			}

			~CellInfo() noexcept = default;

			CellInfo(const CellInfo&) = delete;
			CellInfo(CellInfo&&) noexcept = default;

			auto operator=(const CellInfo&) -> CellInfo& = delete;
			auto operator=(CellInfo&&) noexcept -> CellInfo& = default;

			[[nodiscard]]
			auto GetMaxOffsetX() const noexcept
			{
				return _maxOffsetX;
			}

			[[nodiscard]]
			auto GetMaxOffsetY() const noexcept
			{
				return _maxOffsetY;
			}

			[[nodiscard]]
			auto GetOffsetHash() const noexcept
			{
				return _offsetHash;
			}

			[[nodiscard]]
			auto GetOffsetCount() const noexcept
			{
				return _offsetCount;
			}

			[[nodiscard]]
			auto GetOffsetBuffer() & noexcept -> auto&
			{
				return _offsetBuffer;
			}

			[[nodiscard]]
			auto GetOffsetBuffer() && noexcept -> auto&&
			{
				return std::move(_offsetBuffer);
			}

			[[nodiscard]]
			auto GetOffsetBuffer() const& noexcept -> const auto&
			{
				return _offsetBuffer;
			}

			[[nodiscard]]
			auto GetOffsetBuffer() const&& noexcept -> const auto&&
			{
				return std::move(_offsetBuffer);
			}

		private:
			std::int32_t _maxOffsetX;
			std::int32_t _maxOffsetY;
			Core::HashType _offsetHash;
			std::uint32_t _offsetCount;
			Core::CellOffsetBuffer _offsetBuffer{nullptr, nullptr};
		};

		explicit CellOffsetFile(HeaderInfo a_headerInfo, CellInfo a_cellInfo) noexcept :
			_headerInfo(std::move(a_headerInfo)),
			_cellInfo(std::move(a_cellInfo))
		{
		}

		~CellOffsetFile() noexcept = default;

		CellOffsetFile(const CellOffsetFile&) = delete;
		CellOffsetFile(CellOffsetFile&&) noexcept = default;

		auto operator=(const CellOffsetFile&) -> CellOffsetFile& = delete;
		auto operator=(CellOffsetFile&&) noexcept -> CellOffsetFile& = default;

		[[nodiscard]]
		auto GetHeaderInfo() const noexcept -> const auto&
		{
			return _headerInfo;
		}

		[[nodiscard]]
		auto GetCellInfo() noexcept -> auto&
		{
			return _cellInfo;
		}

		[[nodiscard]]
		auto GetCellInfo() const noexcept -> const auto&
		{
			return _cellInfo;
		}

		[[nodiscard]]
		static auto LoadFromFile(std::filesystem::path a_path) noexcept
			-> std::expected<CellOffsetFile, REX::SystemError>;
		auto SaveToFile(std::filesystem::path a_path) const noexcept -> REX::SystemError;

		static auto ChangeFileWritePermissions(
			const std::filesystem::path& a_path,
			std::filesystem::perm_options a_options) noexcept -> REX::SystemError;
		static auto AddFileReadOnlyFlag(const std::filesystem::path& a_path) noexcept -> REX::SystemError;
		static auto RemoveFileReadOnlyFlag(const std::filesystem::path& a_path) noexcept -> REX::SystemError;

	private:
		HeaderInfo _headerInfo;
		CellInfo _cellInfo;
	};
}
