#include "Plugin/PluginManager.hpp"

F4SE_PLUGIN_LOAD(const F4SE::LoadInterface* a_interface)
{
	return Plugin::PluginManager::Setup(a_interface);
}
