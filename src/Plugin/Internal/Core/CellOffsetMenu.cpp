#include "Plugin/Internal/Core/CellOffsetMenu.hpp"

namespace Plugin::Internal::Core
{
	CellOffsetMenu::CellOffsetMenu(
		std::string a_menuName,
		std::filesystem::path a_menuFilePath,
		std::string a_rootVarPath,
		RE::UI_MENU_FLAGS a_menuFlags,
		RE::UI_DEPTH_PRIORITY a_menuDepth) :
		F4SE::Menus::MenuInfo(
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
		return std::make_unique<CustomGameMenu>(std::static_pointer_cast<const CellOffsetMenu>(shared_from_this()));
	}

	auto CellOffsetMenu::GetCurrentProgressCounter() const noexcept -> std::uint32_t
	{
		return _currentProgressCounter.load(std::memory_order_acquire);
	}

	void CellOffsetMenu::SetCurrentProgressCounter(std::uint32_t a_value) noexcept
	{
		_currentProgressCounter.store(a_value, std::memory_order_release);
	}

	auto CellOffsetMenu::GetMaxProgressCounter() const noexcept -> std::uint32_t
	{
		return _maxProgressCounter.load(std::memory_order_acquire);
	}

	void CellOffsetMenu::SetMaxProgressCounter(std::uint32_t a_value) noexcept
	{
		_maxProgressCounter.store(a_value, std::memory_order_release);
	}

	auto CellOffsetMenu::IncrementProgressCounter() noexcept -> bool
	{
		auto oldValue = _currentProgressCounter.load(std::memory_order_acquire);
		while (oldValue < _maxProgressCounter.load(std::memory_order_acquire)) {
			if (_currentProgressCounter.compare_exchange_weak(
					oldValue,
					oldValue + 1,
					std::memory_order_acq_rel,
					std::memory_order_relaxed)) {
				return true;
			}
		}

		return false;
	}

	CellOffsetMenu::CustomGameMenu::CustomGameMenu(std::weak_ptr<const CellOffsetMenu> a_cellOffsetMenu) :
		_cellOffsetMenu(std::move(a_cellOffsetMenu))
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

		auto nativeCodeObject = GetNativeCodeObject();
		if (!nativeCodeObject.IsObject()) [[unlikely]] {
			REX::Assert(false);
			return;
		}

		SCALEFORM_REGISTER_FUNCTION_OR_FAIL(*uiMovie, nativeCodeObject, getCurrentProgressCounter, _cellOffsetMenu);
		SCALEFORM_REGISTER_FUNCTION_OR_FAIL(*uiMovie, nativeCodeObject, getMaxProgressCounter, _cellOffsetMenu);
	}

	CellOffsetMenu::getCurrentProgressCounter::getCurrentProgressCounter(
		std::weak_ptr<const CellOffsetMenu> a_cellOffsetMenu) :
		_cellOffsetMenu(std::move(a_cellOffsetMenu))
	{
	}

	CellOffsetMenu::getCurrentProgressCounter::~getCurrentProgressCounter() = default;

	void CellOffsetMenu::getCurrentProgressCounter::Call(const ::Scaleform::GFx::FunctionHandler::Params& a_params)
	{
		auto* paramResult = a_params.returnValue;
		if (!paramResult) [[unlikely]] {
			REX::Assert(false);
			return;
		}

		if (a_params.argCount != 0) {
			return;
		}

		const auto cellOffsetMenu = _cellOffsetMenu.lock();
		if (!cellOffsetMenu) {
			return;
		}

		paramResult->SetUInt(cellOffsetMenu->GetCurrentProgressCounter());
	}

	CellOffsetMenu::getMaxProgressCounter::getMaxProgressCounter(std::weak_ptr<const CellOffsetMenu> a_cellOffsetMenu) :
		_cellOffsetMenu(std::move(a_cellOffsetMenu))
	{
	}

	CellOffsetMenu::getMaxProgressCounter::~getMaxProgressCounter() = default;

	void CellOffsetMenu::getMaxProgressCounter::Call(const ::Scaleform::GFx::FunctionHandler::Params& a_params)
	{
		auto* paramResult = a_params.returnValue;
		if (!paramResult) [[unlikely]] {
			REX::Assert(false);
			return;
		}

		if (a_params.argCount != 0) {
			return;
		}

		const auto cellOffsetMenu = _cellOffsetMenu.lock();
		if (!cellOffsetMenu) {
			return;
		}

		paramResult->SetUInt(cellOffsetMenu->GetMaxProgressCounter());
	}
}
