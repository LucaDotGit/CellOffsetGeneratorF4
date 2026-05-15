#include "Plugin/MessagingManager.hpp"

#include "Plugin/InternalManager.hpp"

namespace Plugin::MessagingManager
{
	void Setup(REX::NotNull<const F4SE::MessagingInterface*> a_messaging)
	{
		static constexpr auto FUNCTION_NAME = REX::GetCurrentFunctionName();

		static constinit auto OnceFlag = std::once_flag();
		std::call_once(OnceFlag, [a_messaging]() -> void {
			REX::LogDebug("{}"sv,
				FUNCTION_NAME);

			a_messaging->RegisterListener(Callback);
		});
	}

	void Callback(F4SE::MessagingInterface::Message* a_message)
	{
		if (!a_message) [[unlikely]] {
			REX::Assert(false);
			return;
		}

		REX::LogDebug(R"(Received message of type "{}")"sv,
			a_message->GetType());

		using MessageType = F4SE::MessagingInterface::MessageType;

		switch (a_message->GetType()) {
			case MessageType::kGameDataReady: {
				OnGameDataReady();
				break;
			}
			default: {
				break;
			}
		}
	}

	void OnGameDataReady()
	{
		REX::LogDebug("{}"sv,
			REX::GetCurrentFunctionName());

		Plugin::GetInternalManager()->OnGameDataReady();
	}
}
