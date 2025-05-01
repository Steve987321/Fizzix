#include "framework/Framework.h"
#include "Sim.h"
#include "Fizzix/FZSim.h"
#include "Fizzix/FZMath.h"

#include "Toot/Compiler/Compiler.h"
#include "Toot/TVM/TVM.h"
#include "Toot/Compiler/Parser.h" // op codes to string 
#include "Toot/TVM/Lib/SimLib.h"
#include "Toot/TVM/Lib/IO.h"

#include "engine/Engine.h"
#include "engine/PlaySession.h"

#include "UI/FizzixMenu.h"
#include "UI/TVMMenu.h"

#include "SimEnvironments/CarEnvironment.h"
#include "SimEnvironments/StressTestEnvironment.h"

using namespace Toad;

static VM vm;
static fz::Sim sim;
static bool run_vm = false;

static bool lmouse_released = false;
static bool lmouse_pressed = false;
static bool rmouse_released = false;
static bool rmouse_pressed = false;

static bool add_potential_spring = false;
static bool add_potential_square = false;

static void OnMousePress(sf::Mouse::Button mouse)
{
	if (mouse == sf::Mouse::Left)
		lmouse_pressed = true; 
	else if (mouse == sf::Mouse::Right)
		rmouse_pressed = true; 
}

static void OnMouseRelease(sf::Mouse::Button mouse)
{
	if (mouse == sf::Mouse::Left)
		lmouse_released = true;
	else if (mouse == sf::Mouse::Right)
		rmouse_released = true; 
}

fz::Sim &Sim::GetSim()
{
    return sim;
}

void Sim::SetDefaultScene(fz::Sim &sim)
{
	DrawingCanvas::ClearVertices();

	std::array<Vec2f, 6> player_vertices = fz::CreateSquare(30, 50);
	std::array<Vec2f, 6> floor_vertices = fz::CreateSquare(100000, 500);

	fz::Polygon sim_player({player_vertices.begin(), player_vertices.end()});
	sim_player.Translate({10, -50});
	sim_player.rb.angular_damping = 1.f;
	sim_player.rb.inv_mass = 1.f / 20.f;
	sim_player.rb.restitution = 0.5f;
	sim_player.rb.friction = 1.f;

	fz::Polygon floor({floor_vertices.begin(), floor_vertices.end()});
	floor.Translate({-50, 0});
	floor.rb.is_static = true;
	
	sim.AddPolygon(sim_player);
	sim.AddPolygon(floor);

	DrawingCanvas::AddVertexArray(sim_player.vertices.size());
	DrawingCanvas::AddVertexArray(floor.vertices.size());
}

void Sim::OnStart(Object* obj)
{
	Script::OnStart(obj);

	Time::SetFixedDeltaTime(0.01f);

#if defined(TOAD_EDITOR) || !defined(NDEBUG)
	txt_to_draw.clear();
#endif 

	Mouse::capture_mouse = true;
	Mouse::SetVisible(true);

	sim = fz::Sim();
	vm = VM();

	// use the sim lib library 
	vm.RegisterLib(SimLib::GetSimLib());
	vm.RegisterLib(IO::GetIOLib());

	lmouse_released = false;
	lmouse_pressed = false;
	rmouse_released = false;
	rmouse_pressed = false;
	add_potential_spring = false;
	add_potential_square = false;
	run_vm = false;
	
	SetDefaultScene(sim);

	Input::AddMousePressCallback(OnMousePress);
	Input::AddMouseReleaseCallback(OnMouseRelease);
}

void Sim::OnUpdate(Object* obj)
{
	Script::OnUpdate(obj);

	Camera* cam = Camera::GetActiveCamera();
	if (cam)
		cam->SetPosition(sim.polygons[0].rb.center);

	Vec2f world_mouse = Screen::ScreenToWorld(Mouse::GetPosition(), *Camera::GetActiveCamera());
	
	static Vec2f potential_square_pos;

	// spring previous spring polygon index
	static int i_prev = 0;
	static Vec2f rel_prev {};

	for (int i = 0; i < sim.polygons.size(); i++)
	{
		fz::Polygon& curr_polygon = sim.polygons[i];

		if (interact_with_mouse)
		{
			if (curr_polygon.ContainsPoint(world_mouse))
			{
				// do some 
				if (lmouse_pressed)
				{
					add_potential_spring = true;
					rel_prev = world_mouse - curr_polygon.rb.center;
					i_prev = i;
				}
				else if (add_potential_spring && lmouse_released)
				{
					// don't add spring to same rb 
					if (i_prev != i)
					{
						add_potential_spring = false;
						fz::Polygon& start_polygon = sim.polygons[i_prev];
						fz::Polygon& end_polygon = curr_polygon;
						Vec2f end_rel = world_mouse - end_polygon.rb.center;
	
						fz::Spring& spr = sim.AddSpring(start_polygon, end_polygon, rel_prev, end_rel);
						spr.stiffness = 1.f;
					}
				}
			}
			else
			{
				// add polygon using rmb 
	
				if (rmouse_pressed)
				{
					add_potential_square = true; 
					potential_square_pos = world_mouse;
				}
				else if (add_potential_square && rmouse_released)
				{
					add_potential_square = false; 
					Vec2f square_size = world_mouse - potential_square_pos;
					std::array<Vec2f, 6> square_vertices = fz::CreateSquare(square_size.x, square_size.y);
					fz::Polygon p({square_vertices.begin(), square_vertices.end()});
					p.Translate(potential_square_pos);
					sim.AddPolygon(p);
					
					DrawingCanvas::AddVertexArray(square_vertices.size());
				}
			}
		}
		
		if (show_velocities)
			DrawingCanvas::DrawArrow(curr_polygon.rb.center, curr_polygon.rb.velocity, 1.f);
		
		if (show_aabb)
			DrawingCanvas::DrawRect(curr_polygon.aabb.min, curr_polygon.aabb.max);

		float mass_col = (uint8_t)curr_polygon.rb.inv_mass * 100;
		Color color(255, mass_col, mass_col, 255);
		for (int j = 0; j < curr_polygon.vertices.size(); j++)
		{
			sf::Vertex v;
			v.position = curr_polygon.vertices[j];
			v.color = color;
			DrawingCanvas::ModifyVertex(i, j, v);
		}
	}

	// int i = 0;
	for (const fz::Spring& spr : sim.springs)
	{
		Vec2f a = spr.start_rb->center + spr.start_rel;
		Vec2f b = spr.end_rb->center + spr.end_rel;
		DrawingCanvas::DrawArrow(a, (b - a), 1.f);
	}

	if (add_potential_spring && lmouse_released)
		add_potential_spring = false;
	if (add_potential_square && rmouse_released)
		add_potential_square = false;

	lmouse_pressed = false;
	lmouse_released = false;
	rmouse_pressed = false;
	rmouse_released = false;

	if (env_car_loaded)
		SimEnvironments::CarEnvironmentUpdate(env_car_gas * Time::GetDeltaTime());
}

void Sim::OnFixedUpdate(Object* obj)
{
	if (!pause_sim)
		sim.Update(Time::GetFixedDeltaTime());

	if (run_vm)
	{
		vm.Run(); 
		vm.instruction_pointer = 0;
	}
}

void Sim::OnRender(Object* obj, sf::RenderTarget& target) 
{
	DrawingCanvas::DrawVertices(target, sf::TrianglesStrip);
}

void Sim::ExposeVars()
{
	Script::ExposeVars();

}

#ifdef TOAD_EDITOR
void Sim::OnEditorUI(Object *obj, ImGuiContext *ctx)
{
	ImGui::SetCurrentContext(ctx);
}
#endif

// #define TOAD_EDITOR // for intellisense in vscode 

#if defined(TOAD_EDITOR) || !defined(NDEBUG)
void Sim::OnImGui(Object* obj, ImGuiContext* ctx)
{
	ImGui::SetCurrentContext(ctx);
	static char source[1024];

    UI::FizzixMenu(*this, source);
    UI::TVMMenu(vm, run_vm, source);

	if (env_stress_test_loaded)
		SimEnvironments::StressTestImGui(*this);
}
#endif 
