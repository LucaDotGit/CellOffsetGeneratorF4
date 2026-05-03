#include "Plugin/PluginManager.hpp"

F4SE_PLUGIN_LOAD(const F4SE::LoadInterface* a_f4se)
{
	return Plugin::PluginManager::Setup(a_f4se);
}
