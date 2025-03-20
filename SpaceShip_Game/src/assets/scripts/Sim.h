#pragma once

#include "engine/FormatStr.h"

#if defined(TOAD_EDITOR) || !defined(NDEBUG)
	inline std::unordered_map<const char*, std::string> txt_to_draw = {};

	template <typename ...T>	
	inline void DrawText(std::string_view txt, T... args)
	{
		txt_to_draw[txt.data()] = Toad::format_str(txt, args...);
	}
#else 
template <typename ...T>	
inline void DrawText(std::string_view txt, T... args)
{}
#endif 

namespace fz
{
	class Sim;
}

class GAME_API Sim : public Toad::Script
{
public:
	SCRIPT_CONSTRUCT(Sim)
    
	void OnStart(Toad::Object* obj) override;
	void OnUpdate(Toad::Object* obj) override;
	void OnFixedUpdate(Toad::Object* obj) override;
	void OnRender(Toad::Object* obj, sf::RenderTarget& target) override;
	void ExposeVars() override;

	static fz::Sim& GetSim();

	bool pause_sim = false;
	inline static float d_y = 0;

#ifdef TOAD_EDITOR
	void OnEditorUI(Toad::Object* obj, ImGuiContext* ctx) override;
#endif
#if defined(TOAD_EDITOR) || !defined(NDEBUG)
	void OnImGui(Toad::Object* obj, ImGuiContext* ctx) override;
#endif 
};
