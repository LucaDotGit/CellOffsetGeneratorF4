#include "Plugin/Internal/Core/CellOffsetTranslation.hpp"

namespace Plugin::Internal::Core
{
	CellOffsetTranslation::CellOffsetTranslation(
		REX::NotNull<std::shared_ptr<REX::Logging::ILogger>> a_logger,
		std::string a_name) :
		_logger(std::move(a_logger)),
		_name(std::move(a_name))
	{
	}

	void CellOffsetTranslation::OnGameDataReady([[maybe_unused]] bool a_isReady)
	{
		const auto translator = F4SE::GetOrCreateTranslator();
		if (!translator.has_value()) [[unlikely]] {
			REX::Fail("Failed to get or create the game's translator."sv);
		}

		const auto loadError = translator->LoadFromFile(_name);
		if (loadError.value() != 0) {
			_logger->LogError(R"(Failed to load translation file "{}": {})"sv, _name, loadError.message());
		}
	}
}
