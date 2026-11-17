#include "Plugin/MessagingManager.hpp"

#include "Plugin/InternalManager.hpp"

namespace Plugin::MessagingManager
{
	void Setup(REX::NotNull<const F4SE::MessagingInterface*> a_interface)
	{
		static constexpr auto FUNCTION_NAME = REX::GetCurrentFunctionName();

		static constinit auto OnceFlag = std::once_flag();
		std::call_once(OnceFlag, [a_interface]() -> void {
			const auto& internalManager = Plugin::InternalManager::GetSingleton();
			internalManager.GetLogger()->LogDebug("{}"sv, FUNCTION_NAME);

			a_interface->RegisterListener(Callback);
		});
	}

	void Callback(const F4SE::MessagingInterface::Message* a_message)
	{
		if (!a_message) [[unlikely]] {
			REX::Assert(false);
			return;
		}

		const auto& internalManager = Plugin::InternalManager::GetSingleton();
		internalManager.GetLogger()->LogDebug(R"(Received message of type "{}")"sv, a_message->GetType());

		using MessageType = F4SE::MessagingInterface::MessageType;

		// NOLINTNEXTLINE(readability-trivial-switch)
		switch (a_message->GetType()) {
			case MessageType::GameDataReady: {
				const auto isReady = a_message->GetRawValue<bool>();
				if (!isReady.has_value()) [[unlikely]] {
					REX::Assert(false);
					break;
				}

				Plugin::InternalManager::GetSingleton().OnGameDataReady(isReady.value());
				break;
			}
			default: {
				break;
			}
		}
	}
}
