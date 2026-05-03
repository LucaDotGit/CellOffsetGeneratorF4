#pragma once

namespace Plugin::Internal::Core
{
	class CellOffsetTranslation final
	{
	public:
		CellOffsetTranslation(
			std::string a_name);

		~CellOffsetTranslation() noexcept = default;

		CellOffsetTranslation(const CellOffsetTranslation&) = default;
		CellOffsetTranslation(CellOffsetTranslation&&) noexcept = default;

		CellOffsetTranslation& operator=(const CellOffsetTranslation&) = default;
		CellOffsetTranslation& operator=(CellOffsetTranslation&&) noexcept = default;

		[[nodiscard]] const auto& GetName() const noexcept { return _name; }

		void OnGameDataReady();

	private:
		std::string _name;
	};
}
