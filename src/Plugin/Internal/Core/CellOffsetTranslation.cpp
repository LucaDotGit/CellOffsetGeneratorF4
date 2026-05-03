#include "Plugin/Internal/Core/CellOffsetTranslation.hpp"

namespace Plugin::Internal::Core
{
	CellOffsetTranslation::CellOffsetTranslation(
		std::string a_name)
		: _name(std::move(a_name))
	{
	}

	void CellOffsetTranslation::OnGameDataReady()
	{
		const auto loadError = F4SE::Translations::GetTranslator()->LoadFromFile(_name);
		if (loadError.value() != 0) {
			REX::LogError(R"(Failed to load translation file "{}": {})"sv,
				_name, loadError.message());
		}
	}
}
