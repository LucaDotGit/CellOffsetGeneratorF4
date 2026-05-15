#pragma once

namespace Plugin::Internal::Core
{
	class CellOffsetMenu final
		: public F4SE::Menus::MenuInfo
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

		CellOffsetMenu& operator=(const CellOffsetMenu&) = delete;
		CellOffsetMenu& operator=(CellOffsetMenu&&) = delete;

		auto CreateMenuInstance() const
			-> REX::NotNull<std::unique_ptr<RE::GameMenuBase>> override;

		void OnMenuLoadSuccess(const REX::NotNull<std::unique_ptr<RE::GameMenuBase>>& a_menuInstance) const override;
		void OnMenuLoadFailure(const REX::NotNull<std::unique_ptr<RE::GameMenuBase>>& a_menuInstance) const override;

		[[nodiscard]] std::uint32_t GetCurrentProgressCounter() const noexcept;
		void SetCurrentProgressCounter(std::uint32_t a_value) noexcept;

		[[nodiscard]] std::uint32_t GetMaxProgressCounter() const noexcept;
		void SetMaxProgressCounter(std::uint32_t a_value) noexcept;
		bool IncrementProgressCounter() noexcept;

	private:
		class CustomGameMenu final
			: public RE::GameMenuBase
		{
		public:
			CustomGameMenu(
				const REX::NotNull<std::shared_ptr<const CellOffsetMenu>>& a_cellOffsetMenu);

			~CustomGameMenu() override;

			CustomGameMenu(const CustomGameMenu&) = delete;
			CustomGameMenu(CustomGameMenu&&) = delete;

			CustomGameMenu& operator=(const CustomGameMenu&) = delete;
			CustomGameMenu& operator=(CustomGameMenu&&) = delete;

			void MapCodeObjectFunctions() override;

		private:
			REX::NotNull<std::shared_ptr<const CellOffsetMenu>> _cellOffsetMenu;
		};

		class getCurrentProgressCounter final
			: public ::Scaleform::GFx::FunctionHandler
		{
		public:
			getCurrentProgressCounter(
				const REX::NotNull<std::shared_ptr<const CellOffsetMenu>>& a_cellOffsetMenu);

			~getCurrentProgressCounter() override;

			getCurrentProgressCounter(const getCurrentProgressCounter&) = delete;
			getCurrentProgressCounter(getCurrentProgressCounter&&) = delete;

			getCurrentProgressCounter& operator=(const getCurrentProgressCounter&) = delete;
			getCurrentProgressCounter& operator=(getCurrentProgressCounter&&) = delete;

			void Call(const ::Scaleform::GFx::FunctionHandler::Params& a_params) override;

		private:
			REX::NotNull<std::shared_ptr<const CellOffsetMenu>> _cellOffsetMenu;
		};

		class getMaxProgressCounter final
			: public ::Scaleform::GFx::FunctionHandler
		{
		public:
			getMaxProgressCounter(
				const REX::NotNull<std::shared_ptr<const CellOffsetMenu>>& a_cellOffsetMenu);

			~getMaxProgressCounter() override;

			getMaxProgressCounter(const getMaxProgressCounter&) = delete;
			getMaxProgressCounter(getMaxProgressCounter&&) = delete;

			getMaxProgressCounter& operator=(const getMaxProgressCounter&) = delete;
			getMaxProgressCounter& operator=(getMaxProgressCounter&&) = delete;

			void Call(const ::Scaleform::GFx::FunctionHandler::Params& a_params) override;

		private:
			REX::NotNull<std::shared_ptr<const CellOffsetMenu>> _cellOffsetMenu;
		};

		std::atomic<std::uint32_t> _maxProgressCounter{ 0 };
		std::atomic<std::uint32_t> _currentProgressCounter{ 0 };
	};
}
