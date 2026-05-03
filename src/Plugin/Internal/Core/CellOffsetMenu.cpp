#include "Plugin/Internal/Core/CellOffsetMenu.hpp"

namespace Plugin::Internal::Core
{
	CellOffsetMenu::CellOffsetMenu(
		std::string a_menuName,
		std::filesystem::path a_menuFilePath,
		std::string a_rootVarPath,
		RE::UI_MENU_FLAGS a_menuFlags,
		RE::UI_DEPTH_PRIORITY a_menuDepth)
		: F4SE::Menus::MenuInfo(
			  std::move(a_menuName),
			  std::move(a_menuFilePath),
			  std::move(a_rootVarPath),
			  a_menuFlags,
			  a_menuDepth)
	{
	}

	CellOffsetMenu::~CellOffsetMenu() noexcept = default;

	auto CellOffsetMenu::CreateMenuInstance() const -> REX::NotNull<std::unique_ptr<RE::GameMenuBase>>
	{
		const auto sharedMenu = shared_from_this();
		return std::make_unique<CustomGameMenu>(std::static_pointer_cast<const CellOffsetMenu>(sharedMenu));
	}

	void CellOffsetMenu::OnMenuLoadSuccess([[maybe_unused]] const REX::NotNull<std::unique_ptr<RE::GameMenuBase>>& a_menuInstance) const
	{
	}

	void CellOffsetMenu::OnMenuLoadFailure([[maybe_unused]] const REX::NotNull<std::unique_ptr<RE::GameMenuBase>>& a_menuInstance) const
	{
		REX::LogError("Failed to load the cell offset menu"sv);
	}

	std::uint32_t CellOffsetMenu::GetCurrentProgressCounter() const noexcept
	{
		return _currentProgressCounter.load(std::memory_order_acquire);
	}

	void CellOffsetMenu::SetCurrentProgressCounter(std::uint32_t a_value) noexcept
	{
		_currentProgressCounter.store(a_value, std::memory_order_release);
	}

	std::uint32_t CellOffsetMenu::GetMaxProgressCounter() const noexcept
	{
		return _maxProgressCounter.load(std::memory_order_acquire);
	}

	void CellOffsetMenu::SetMaxProgressCounter(std::uint32_t a_value) noexcept
	{
		_maxProgressCounter.store(a_value, std::memory_order_release);
	}

	bool CellOffsetMenu::IncrementProgressCounter() noexcept
	{
		auto oldValue = _currentProgressCounter.load(std::memory_order_acquire);
		while (oldValue < _maxProgressCounter.load(std::memory_order_acquire)) {
			if (_currentProgressCounter.compare_exchange_weak(oldValue, oldValue + 1,
					std::memory_order_acq_rel, std::memory_order_relaxed)) {
				return true;
			}
		}

		return false;
	}

	CellOffsetMenu::CustomGameMenu::CustomGameMenu(
		const REX::NotNull<std::shared_ptr<const CellOffsetMenu>>& a_cellOffsetMenu)
		: _cellOffsetMenu(a_cellOffsetMenu)
	{
	}

	CellOffsetMenu::CustomGameMenu::~CustomGameMenu() = default;

	void CellOffsetMenu::CustomGameMenu::MapCodeObjectFunctions()
	{
		RE::GameMenuBase::MapCodeObjectFunctions();

		if (!uiMovie) [[unlikely]] {
			REX::Assert(false);
			return;
		}

		auto menuRoot = uiMovie->asMovieRoot;
		if (!menuRoot) [[unlikely]] {
			REX::Assert(false);
			return;
		}

		auto nativeCodeObject = GetNativeCodeObject();
		if (!nativeCodeObject.IsObject()) [[unlikely]] {
			REX::Assert(false);
			return;
		}

		RE_REGISTER_SF_FUNCTION(menuRoot, std::addressof(nativeCodeObject), getCurrentProgressCounter, _cellOffsetMenu);
		RE_REGISTER_SF_FUNCTION(menuRoot, std::addressof(nativeCodeObject), getMaxProgressCounter, _cellOffsetMenu);
	}

	CellOffsetMenu::getCurrentProgressCounter::getCurrentProgressCounter(
		const REX::NotNull<std::shared_ptr<const CellOffsetMenu>>& a_cellOffsetMenu)
		: _cellOffsetMenu(a_cellOffsetMenu)
	{
	}

	CellOffsetMenu::getCurrentProgressCounter::~getCurrentProgressCounter() = default;

	void CellOffsetMenu::getCurrentProgressCounter::Call(
		const ::Scaleform::GFx::FunctionHandler::Params& a_params)
	{
		auto* paramResult = a_params.returnValue;
		if (!paramResult) [[unlikely]] {
			REX::Assert(false);
			return;
		}

		if (a_params.argCount != 0) {
			return;
		}

		paramResult->SetUInt(_cellOffsetMenu->GetCurrentProgressCounter());
	}

	CellOffsetMenu::getMaxProgressCounter::getMaxProgressCounter(
		const REX::NotNull<std::shared_ptr<const CellOffsetMenu>>& a_cellOffsetMenu)
		: _cellOffsetMenu(a_cellOffsetMenu)
	{
	}

	CellOffsetMenu::getMaxProgressCounter::~getMaxProgressCounter() = default;

	void CellOffsetMenu::getMaxProgressCounter::Call(
		const ::Scaleform::GFx::FunctionHandler::Params& a_params)
	{
		auto* paramResult = a_params.returnValue;
		if (!paramResult) [[unlikely]] {
			REX::Assert(false);
			return;
		}

		if (a_params.argCount != 0) {
			return;
		}

		paramResult->SetUInt(_cellOffsetMenu->GetMaxProgressCounter());
	}
}
