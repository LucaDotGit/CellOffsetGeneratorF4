#pragma once

#include "Plugin/Internal/Core/CellOffsetHasher.hpp"
#include "Plugin/Internal/Core/CellOffsetType.hpp"

namespace Plugin::Internal::Core
{
	class CellOffsetFile final
	{
	public:
		inline static constexpr auto HEADER_ID = REX::HashCrc32("CellOffsetFile"sv);
		inline static constexpr auto HEADER_VERSION = 1ui32;

		class HeaderInfo final
		{
		public:
			HeaderInfo() = default;
			~HeaderInfo() noexcept = default;

			HeaderInfo(const HeaderInfo&) = delete;
			HeaderInfo(HeaderInfo&&) noexcept = default;

			HeaderInfo& operator=(const HeaderInfo&) = delete;
			HeaderInfo& operator=(HeaderInfo&&) noexcept = default;

			std::uint32_t headerId{ HEADER_ID };
			std::uint32_t headerVersion{ HEADER_VERSION };
			Core::HashType fileHash{ 0 };
		};

		class OffsetInfo final
		{
		public:
			OffsetInfo() = default;
			~OffsetInfo() noexcept = default;

			OffsetInfo(const OffsetInfo&) = delete;
			OffsetInfo(OffsetInfo&&) noexcept = default;

			OffsetInfo& operator=(const OffsetInfo&) = delete;
			OffsetInfo& operator=(OffsetInfo&&) noexcept = default;

			std::int32_t maxOffsetX{ 0 };
			std::int32_t maxOffsetY{ 0 };
			Core::HashType offsetHash{ 0 };
			std::uint32_t offsetCount{ 0 };
			Core::CellOffsetBuffer offsetBuffer{ nullptr, Core::DeleteCellOffsetBuffer };
		};

		CellOffsetFile() = default;
		~CellOffsetFile() noexcept = default;

		CellOffsetFile(const CellOffsetFile&) = delete;
		CellOffsetFile(CellOffsetFile&&) noexcept = default;

		CellOffsetFile& operator=(const CellOffsetFile&) = delete;
		CellOffsetFile& operator=(CellOffsetFile&&) noexcept = default;

		[[nodiscard]] static auto LoadFromFile(const std::filesystem::path& a_path)
			-> std::expected<CellOffsetFile, REX::SystemError>;
		REX::SystemError SaveToFile(const std::filesystem::path& a_path);

		static REX::SystemError MarkFileAsReadOnly(const std::filesystem::path& a_path) noexcept;

		HeaderInfo headerInfo;
		OffsetInfo offsetInfo;
	};
}
