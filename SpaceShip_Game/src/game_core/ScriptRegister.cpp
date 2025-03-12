#include "ScriptRegister.h"
#include "engine/Engine.h"

#include "scripts/TestScript.h"

// tools 
#include "engine/default_scripts/AnimationController.h"
#include "assets/scripts/Thruster.h"
#include "assets/scripts/Sim.h"

namespace Toad
{

static std::vector<ScriptBuffer> scripts{};

void register_scripts()
{
	scripts.clear();

	SCRIPT_REGISTER(TestScript);
	SCRIPT_REGISTER(AnimationController);
	SCRIPT_REGISTER(Thruster);
	SCRIPT_REGISTER(Sim);
}

void register_script(void* instance, size_t size)
{
	scripts.emplace_back(instance, size);
}

Scripts get_registered_scripts()
{
	return {scripts.data(), scripts.size()};
}

}
