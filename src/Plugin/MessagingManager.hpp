#pragma once

namespace Plugin::MessagingManager
{
	void Setup(REX::NotNull<const F4SE::MessagingInterface*> a_interface);
	void Callback(const F4SE::MessagingInterface::Message* a_message);
}
