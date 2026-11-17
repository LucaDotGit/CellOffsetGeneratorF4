#pragma once

namespace Plugin::Internal::Core
{
	class CellOffsetPatches final : public F4SE::Events::IOnApiLoad
	{
	public:
		explicit CellOffsetPatches(REX::NotNull<std::shared_ptr<REX::Logging::ILogger>> a_logger);
		~CellOffsetPatches() noexcept override;

		CellOffsetPatches(const CellOffsetPatches&) = delete;
		CellOffsetPatches(CellOffsetPatches&&) = delete;

		auto operator=(const CellOffsetPatches&) -> CellOffsetPatches& = delete;
		auto operator=(CellOffsetPatches&&) -> CellOffsetPatches& = delete;

		void OnApiLoad(const REX::NotNull<std::shared_ptr<REL::HookList>>& a_hookList) override;

		void Enable() const;
		void Disable() const;

	private:
		REX::NotNull<std::shared_ptr<REX::Logging::ILogger>> _logger;
		std::vector<REX::NotNull<std::shared_ptr<REL::IHook>>> _hooks;
	};
}
