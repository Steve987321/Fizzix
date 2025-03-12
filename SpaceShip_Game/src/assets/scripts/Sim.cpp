#include "framework/Framework.h"
#include "Sim.h"
#include "Fizzix/FZSim.h"

#include "engine/Engine.h"

using namespace Toad;

static fz::Sim sim;

void Sim::OnStart(Object* obj)
{
	Script::OnStart(obj);

#if defined(TOAD_EDITOR) || !defined(NDEBUG)
	txt_to_draw.clear();
#endif 

	sim = fz::Sim();

	std::vector<Toad::Vec2f> square1 = { {-1.f, -1.f}, {1.f, -1.f}, {1.f, 1.f}};
	// std::vector<Toad::Vec2f> square2 = square1;
	std::vector<Toad::Vec2f> floor_v = {{-10.f, 2.f}, {5.f, 7.f}, {10.f, 2.f}};
	// for (auto& v : square2)
	// {
	// 	v.x += 5.f;
	// 	v.y += 5.f;
	// }

	fz::Polygon p1(square1);
	p1.rb.velocity = Vec2f{0, 10.f};
	// fz::Polygon p2(square2);
	fz::Polygon floor(floor_v);
	// floor.rb.mass = FLT_MAX;
	floor.rb.is_static = true;
	// sim.gravity = {0, 0};
	// p1.rb.velocity = {0, 5.f};

	sim.polygons.emplace_back(p1);
	// sim.polygons.emplace_back(p2);
	sim.polygons.emplace_back(floor);

	Toad::DrawingCanvas::AddVertexArray(square1.size());
	// Toad::DrawingCanvas::AddVertexArray(square2.size());
	Toad::DrawingCanvas::AddVertexArray(floor_v.size());
}

void Sim::OnUpdate(Object* obj)
{
	Script::OnUpdate(obj);

	for (int i = 0; i < sim.polygons.size(); i++)
	{
		for (int j = 0; j < sim.polygons[i].vertices.size(); j++)
		{
			sf::Vertex v;
			v.position = sim.polygons[i].vertices[j];
			v.color = sf::Color::White;
			Toad::DrawingCanvas::ModifyVertex(i, j, v);
		}
	}
}


void Sim::OnFixedUpdate(Toad::Object* obj)
{
	if (!pause_sim)
		sim.Update(Time::GetFixedDeltaTime());
}

void Sim::OnRender(Object* obj, sf::RenderTarget& target) 
{
	Toad::DrawingCanvas::DrawVertices(target, sf::Triangles);
}

void Sim::ExposeVars()
{
	Script::ExposeVars();

}

#ifdef TOAD_EDITOR
void Sim::OnEditorUI(Toad::Object *obj, ImGuiContext *ctx)
{
	ImGui::SetCurrentContext(ctx);
}
#endif

#if defined(TOAD_EDITOR) || !defined(NDEBUG)
void Sim::OnImGui(Toad::Object* obj, ImGuiContext* ctx)
{
	ImGui::SetCurrentContext(ctx);

	ImGui::Begin("[Sim] fizzix menu");
    if (!sim.polygons.empty())
    {
        if (ImGui::Button("FORCE"))
        {
            sim.polygons[0].rb.velocity += Vec2f{0, -100.f} * Time::GetDeltaTime();
        }
        if (ImGui::Button("ANGULARA"))
        {
            sim.polygons[0].rb.angular_velocity += 10.f * Time::GetDeltaTime();
        }if (ImGui::Button("ANGULARB"))
        {
            sim.polygons[0].rb.angular_velocity -= 10.f * Time::GetDeltaTime();
        }
    }

	static float angle = 0;
	static float scale = 1.f;
	ImGui::DragFloat("Angle", &angle);
	ImGui::Checkbox("Pause", &pause_sim);
	ImGui::DragFloat("Time scale", &scale, 0.05f);

	float grav_edit[2] = {sim.gravity.x, sim.gravity.y};
	if (ImGui::DragFloat2("Grav", grav_edit, 0.1f))
	{	
		sim.gravity.x = grav_edit[0];
		sim.gravity.y = grav_edit[1];
	}
	if (ImGui::Button("Set time scale"))
		Time::SetTimeScale(scale);

	for (int i = 0; i < sim.polygons.size(); i++)
	{
		fz::Rigidbody& rb = sim.polygons[i].rb;
		
		ImGui::PushID(i);

		ImGui::DragFloat("Restitution", &rb.restitution);
		
		if (ImGui::DragFloat("Mass", &rb.mass))
			if (rb.mass < 0.1f) 
				rb.mass = 0.1f;
		
		ImGui::Text("Center(%.2f, %.2f) Resting(%d)", rb.center.x, rb.center.y, rb.resting);
		ImGui::Text("Angular(%.2f) AngularAvg(%.2f)", rb.angular_velocity, rb.angular_velocity_average);
		ImGui::Text("Vel(%.2f, %.2f) VelLength(%.3f) AbsVelAvg(%.2f, %.2f)", rb.velocity.x, rb.velocity.y, rb.velocity.Length(), rb.velocity_average.x, rb.velocity_average.y);
		for (auto& f : sim.polygons[i].vertices)
		{
			ImGui::Text("%.1f, %.1f", f.x, f.y);
		}
		if (ImGui::Button("Rotate"))
		{
			sim.polygons[i].Rotate(angle);
		}
		ImGui::PopID();
	}

	ImGui::End();

	auto draw = ImGui::GetForegroundDrawList();

	draw->AddText({50.f, 20.f}, IM_COL32(255, 255, 0, 255), std::to_string(txt_to_draw.size()).c_str());
	int i = 2;
	for (const auto& [k, v] : txt_to_draw)
	{	
		draw->AddText({50.f, (float)i * 20.f}, IM_COL32_WHITE, v.c_str());
		i++;
	}

	if (!txt_to_draw.empty())
		txt_to_draw.clear();
}
#endif 
