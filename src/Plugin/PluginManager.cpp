#include "Plugin/PluginManager.hpp"

#include "Plugin/Internal/SettingsManager.hpp"
#include "Plugin/InternalManager.hpp"
#include "Plugin/MessagingManager.hpp"

namespace Plugin::PluginManager
{
	bool Setup(REX::NotNull<const F4SE::LoadInterface*> a_f4se)
	{
		static constinit auto OnceFlag = std::once_flag();
		std::call_once(OnceFlag, [a_f4se]() {
			F4SE::RegisterForOnLoad([]() {
				const auto& internalManager = Plugin::GetInternalManager();
				internalManager->OnXseLoad();
			});

			const auto& internalManager = Plugin::GetInternalManager();

			const auto settingsManager = internalManager->GetSettingsManager();
			settingsManager->LoadSettings();

			const auto logLevel = static_cast<REX::LogLevel>(settingsManager->GetLogLevelSetting()->GetValue());
			const auto logFileCount = static_cast<std::size_t>(settingsManager->GetLogFileCountSetting()->GetValue());
			const auto initInfo = F4SE::InitInfo{
				.logLevel = logLevel,
				.logFileCount = logFileCount,
				.useHooks = true
			};

			F4SE::Init(a_f4se, initInfo);

			Plugin::MessagingManager::Setup(F4SE::GetMessagingInterface());
		});

		return true;
	}
}
