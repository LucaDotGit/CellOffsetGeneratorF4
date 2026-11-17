#pragma once

namespace Plugin::Internal::Core
{
	class CellOffsetTranslation final : public F4SE::Events::IOnGameDataReady
	{
	public:
		CellOffsetTranslation(REX::NotNull<std::shared_ptr<REX::Logging::ILogger>> a_logger, std::string a_name);
		~CellOffsetTranslation() noexcept override = default;

		CellOffsetTranslation(const CellOffsetTranslation&) = default;
		CellOffsetTranslation(CellOffsetTranslation&&) noexcept = default;

		auto operator=(const CellOffsetTranslation&) -> CellOffsetTranslation& = default;
		auto operator=(CellOffsetTranslation&&) noexcept -> CellOffsetTranslation& = default;

		[[nodiscard]]
		auto GetName() const noexcept
		{
			return std::string_view(_name);
		}

		void OnGameDataReady(bool a_isReady) override;

	private:
		REX::NotNull<std::shared_ptr<REX::Logging::ILogger>> _logger;
		std::string _name;
	};
}
