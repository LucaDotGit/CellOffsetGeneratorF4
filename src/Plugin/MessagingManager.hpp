#pragma once

namespace Plugin::MessagingManager
{
	void Setup(REX::NotNull<const F4SE::MessagingInterface*> a_messaging);
	void Callback(F4SE::MessagingInterface::Message* a_message);
	void OnGameDataReady();
}
