#pragma once

namespace Plugin::Internal::Core
{
	class CellOffsetPatches;
}

namespace Plugin::Internal
{
	class PatchesManager final : public F4SE::Events::IOnApiLoad
	{
	public:
		explicit PatchesManager(const REX::NotNull<std::shared_ptr<REX::Logging::ILogger>>& a_logger);
		~PatchesManager() noexcept override;

		PatchesManager(const PatchesManager&) = delete;
		PatchesManager(PatchesManager&&) = delete;

		auto operator=(const PatchesManager&) -> PatchesManager& = delete;
		auto operator=(PatchesManager&&) -> PatchesManager& = delete;

		[[nodiscard]]
		auto GetCellOffsetPatches() const noexcept
		{
			return _cellOffsetPatches;
		}

		void OnApiLoad(const REX::NotNull<std::shared_ptr<REL::HookList>>& a_hookList) override;

		void Enable() const;
		void Disable() const;

	private:
		REX::NotNull<std::shared_ptr<REX::Logging::ILogger>> _logger;
		REX::NotNull<std::shared_ptr<Core::CellOffsetPatches>> _cellOffsetPatches;
	};
}
