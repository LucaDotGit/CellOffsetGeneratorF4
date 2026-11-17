#include "Plugin/PluginManager.hpp"

#include "Plugin/Internal/SettingsManager.hpp"
#include "Plugin/InternalManager.hpp"
#include "Plugin/MessagingManager.hpp"

namespace Plugin::PluginManager
{
	auto Setup(REX::NotNull<const F4SE::LoadInterface*> a_interface) -> bool
	{
		static constinit auto OnceFlag = std::once_flag();
		std::call_once(OnceFlag, [a_interface]() -> void {
			auto& internalManager = Plugin::InternalManager::GetSingleton();

			const auto settingsManager = internalManager.GetSettingsManager();
			settingsManager->OnSettingsLoad();

			const auto logMinLevel = settingsManager->GetLogMinLevelSetting()->GetValue();
			const auto logRotationCount = settingsManager->GetLogRotationCountSetting()->GetValue();
			const auto logRotationByteSize = settingsManager->GetLogRotationByteSizeSetting()->GetValue();

			const auto logger = internalManager.GetLogger();
			logger->SetMinLevel(logMinLevel);
			logger->SetFlushLevel(logMinLevel);

			auto setupOptions = F4SE::SetupOptions{
				.logger = logger.get(),
				.logRotationCount = logRotationCount,
				.logRotationByteSize = logRotationByteSize,
				.useHooks = true,
				.useTrampoline = true
			};

			F4SE::RegisterForOnLoad(
				[&internalManager](const REX::NotNull<std::shared_ptr<REL::HookList>>& a_hookList) -> void {
					internalManager.OnApiLoad(a_hookList);
				});

			F4SE::Setup(a_interface, std::move(setupOptions));

			Plugin::MessagingManager::Setup(F4SE::GetMessagingInterface());
		});

		return true;
	}
}
