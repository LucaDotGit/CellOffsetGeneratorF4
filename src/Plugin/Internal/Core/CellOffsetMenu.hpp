#pragma once

namespace Plugin::Internal::Core
{
	class CellOffsetMenu final : public F4SE::Menus::MenuInfo
	{
	public:
		CellOffsetMenu(
			std::string a_menuName,
			std::filesystem::path a_menuFilePath,
			std::string a_rootVarPath,
			RE::UI_MENU_FLAGS a_menuFlags,
			RE::UI_DEPTH_PRIORITY a_menuDepth);

		~CellOffsetMenu() noexcept override;

		CellOffsetMenu(const CellOffsetMenu&) = delete;
		CellOffsetMenu(CellOffsetMenu&&) = delete;

		auto operator=(const CellOffsetMenu&) -> CellOffsetMenu& = delete;
		auto operator=(CellOffsetMenu&&) -> CellOffsetMenu& = delete;

		auto CreateMenuInstance() const -> REX::NotNull<std::unique_ptr<RE::GameMenuBase>> override;

		[[nodiscard]]
		auto GetCurrentProgressCounter() const noexcept -> std::uint32_t;
		void SetCurrentProgressCounter(std::uint32_t a_value) noexcept;

		[[nodiscard]]
		auto GetMaxProgressCounter() const noexcept -> std::uint32_t;
		void SetMaxProgressCounter(std::uint32_t a_value) noexcept;
		auto IncrementProgressCounter() noexcept -> bool;

	private:
		class CustomGameMenu final : public RE::GameMenuBase
		{
		public:
			explicit CustomGameMenu(std::weak_ptr<const CellOffsetMenu> a_cellOffsetMenu);
			~CustomGameMenu() override;

			CustomGameMenu(const CustomGameMenu&) = delete;
			CustomGameMenu(CustomGameMenu&&) = delete;

			auto operator=(const CustomGameMenu&) -> CustomGameMenu& = delete;
			auto operator=(CustomGameMenu&&) -> CustomGameMenu& = delete;

			void MapCodeObjectFunctions() override;

		private:
			std::weak_ptr<const CellOffsetMenu> _cellOffsetMenu;
		};

		class getCurrentProgressCounter final : public ::Scaleform::GFx::FunctionHandler
		{
		public:
			explicit getCurrentProgressCounter(std::weak_ptr<const CellOffsetMenu> a_cellOffsetMenu);
			~getCurrentProgressCounter() override;

			getCurrentProgressCounter(const getCurrentProgressCounter&) = delete;
			getCurrentProgressCounter(getCurrentProgressCounter&&) = delete;

			auto operator=(const getCurrentProgressCounter&) -> getCurrentProgressCounter& = delete;
			auto operator=(getCurrentProgressCounter&&) -> getCurrentProgressCounter& = delete;

			void Call(const ::Scaleform::GFx::FunctionHandler::Params& a_params) override;

		private:
			std::weak_ptr<const CellOffsetMenu> _cellOffsetMenu;
		};

		class getMaxProgressCounter final : public ::Scaleform::GFx::FunctionHandler
		{
		public:
			explicit getMaxProgressCounter(std::weak_ptr<const CellOffsetMenu> a_cellOffsetMenu);
			~getMaxProgressCounter() override;

			getMaxProgressCounter(const getMaxProgressCounter&) = delete;
			getMaxProgressCounter(getMaxProgressCounter&&) = delete;

			auto operator=(const getMaxProgressCounter&) -> getMaxProgressCounter& = delete;
			auto operator=(getMaxProgressCounter&&) -> getMaxProgressCounter& = delete;

			void Call(const ::Scaleform::GFx::FunctionHandler::Params& a_params) override;

		private:
			std::weak_ptr<const CellOffsetMenu> _cellOffsetMenu;
		};

		std::atomic<std::uint32_t> _maxProgressCounter{0};
		std::atomic<std::uint32_t> _currentProgressCounter{0};
	};
}
