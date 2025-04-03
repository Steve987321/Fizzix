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

#include "SimEnvironments/CarEnvironment.h"

using namespace Toad;

static VM vm;
static fz::Sim sim;
static Vec2f player_vel;
static bool run_vm = false;

static bool lmouse_released = false;
static bool lmouse_pressed = false;
static bool rmouse_released = false;
static bool rmouse_pressed = false;

static bool add_potential_spring = false;
static fz::Spring potential_spring {};

static bool add_potential_square = false;

static bool env_car_loaded = false;

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

static void AddLibsToParserCtx()
{
	Compiler::AddLibToParserCtx(SimLib::GetSimLib());
	Compiler::AddLibToParserCtx(IO::GetIOLib());
}

void Sim::OnStart(Object* obj)
{
	Script::OnStart(obj);

#if defined(TOAD_EDITOR) || !defined(NDEBUG)
	txt_to_draw.clear();
#endif 

	Mouse::capture_mouse = true;
	Mouse::SetVisible(true);

	sim = fz::Sim();
	vm = VM();

	// use the sim lib library 
	SimLib::RegisterToVM(vm);
	IO::RegisterToVM(vm);

	lmouse_released = false;
	lmouse_pressed = false;
	rmouse_released = false;
	rmouse_pressed = false;
	add_potential_spring = false;
	add_potential_square = false;
	run_vm = false;
	d_y = 0;

	// #todo use something else for sim objects 
	Toad::DrawingCanvas::ClearVertices();

	std::vector<Toad::Vec2f> player = {{-50, -30}, {-30, -30}, {-30, 0}, {-50, -30}, {-30, 0},  {-50, 0}};
	std::array<Toad::Vec2f, 6> floor_v = fz::CreateSquare(100000, 500);

	fz::Polygon sim_player(player);
	sim_player.Translate({10, -50});
	sim_player.rb.angular_damping = 1.f;
	sim_player.rb.mass = 20.f;
	sim_player.rb.restitution = 0.5f;
	sim_player.rb.friction = 1.f;

	fz::Polygon floor({floor_v.begin(), floor_v.end()});
	floor.Translate({-50, 0});
	floor.rb.is_static = true;

	sim.polygons.emplace_back(sim_player);
	// sim_player.Translate({60, 0});
	// sim.polygons.emplace_back(sim_player);
	sim.polygons.emplace_back(floor);
	sim.springs.reserve(10);

	Toad::DrawingCanvas::AddVertexArray(player.size());
	Toad::DrawingCanvas::AddVertexArray(sim_player.vertices.size());
	Toad::DrawingCanvas::AddVertexArray(floor_v.size());

	Input::AddMousePressCallback(OnMousePress);
	Input::AddMouseReleaseCallback(OnMouseRelease);
}

void Sim::OnUpdate(Object* obj)
{
	Script::OnUpdate(obj);

	Camera* cam = Camera::GetActiveCamera();
	if (cam)
		cam->SetPosition(sim.polygons[0].rb.center);

	// sim.polygons[0].rb.velocity += player_vel * Time::GetDeltaTime();
	Vec2f world_mouse = Screen::ScreenToWorld(Mouse::GetPosition(), *Camera::GetActiveCamera());
	
	static Vec2f potential_square_pos;

	static int i_prev = 0;
	for (int i = 0; i < sim.polygons.size(); i++)
	{
		if (sim.polygons[i].ContainsPoint(world_mouse))
		{
			// do some 
			if (lmouse_pressed)
			{
				add_potential_spring = true;
				potential_spring.start_rb = &sim.polygons[i].rb;
				potential_spring.start_rel = world_mouse - sim.polygons[i].rb.center;
				i_prev = i;
			}
			else if (add_potential_spring && lmouse_released)
			{
				if (potential_spring.start_rb != &sim.polygons[i].rb)
				{
					add_potential_spring = false;
					potential_spring.stiffness = 1.f;
					potential_spring.end_rb = &sim.polygons[i].rb;
					potential_spring.end_rel = world_mouse - sim.polygons[i].rb.center;
					potential_spring.target_len = fz::dist(world_mouse, potential_spring.start_rb->center + potential_spring.start_rel);
					potential_spring.min_len = potential_spring.target_len / 3.f;
					sim.springs.push_back(potential_spring);
					LOGDEBUGF("{} {}", i, i_prev);
					sim.polygons[i].extra_points.emplace_back(&sim.springs.back().end_rel);
					sim.polygons[i_prev].extra_points.emplace_back(&sim.springs.back().start_rel);
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
				sim.polygons.emplace_back(p);
				
				Toad::DrawingCanvas::AddVertexArray(square_vertices.size());
			}
		}

		DrawingCanvas::DrawArrow(sim.polygons[i].rb.center, sim.polygons[i].rb.velocity, 1.f);
		
		for (int j = 0; j < sim.polygons[i].vertices.size(); j++)
		{
			sf::Vertex v;
			v.position = sim.polygons[i].vertices[j];
			v.color = sf::Color::White;
			DrawingCanvas::ModifyVertex(i, j, v);
		}
	}

	int i = 0;
	for (const fz::Spring& spr : sim.springs)
	{
		Vec2f a = spr.start_rb->center + spr.start_rel;
		Vec2f b = spr.end_rb->center + spr.end_rel;
		DrawingCanvas::DrawArrow(a, (b - a), 1.f);
		std::ostringstream ss;
		ss << i;
		DrawingCanvas::DrawText((a + b) / 2.f, ss.str(), 7.f);
		i++;
	}

	if (Input::IsKeyDown(Keyboard::D))
	{
		if (sim.polygons[0].rb.angular_velocity < 10.f)
			sim.polygons[0].rb.angular_velocity += 10.f * Time::GetDeltaTime();
	}
	if (Input::IsKeyDown(Keyboard::A))
	{
		if (sim.polygons[0].rb.angular_velocity > -10.f)
			sim.polygons[0].rb.angular_velocity -= 10.f * Time::GetDeltaTime();
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
		CarEnvironmentUpdate(env_car_gas * Time::GetDeltaTime());
}

void Sim::OnFixedUpdate(Toad::Object* obj)
{
	d_y = sim.polygons[0].rb.center.y;
	if (!pause_sim)
		sim.Update(Time::GetFixedDeltaTime());
	d_y = sim.polygons[0].rb.center.y - d_y;

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

fz::Sim &Sim::GetSim()
{
    return sim;
}
#ifdef TOAD_EDITOR
void Sim::OnEditorUI(Toad::Object *obj, ImGuiContext *ctx)
{
	ImGui::SetCurrentContext(ctx);
}
#endif

// #define TOAD_EDITOR // for intellisense in vscode 

// from parser.cpp 
static std::string InstructionToStr(VM::Instruction instr)
{
	std::string s = op_code_names[instr.op] + ' ';
	instr.args.emplace_back(instr.reserved);
	for (const VMRegister& arg : instr.args)
	{
		switch (arg.type)
		{
		case VMRegisterType::FLOAT:
			s += std::to_string(arg.value.flt);
			break;
		case VMRegisterType::INT:
			s += std::to_string(arg.value.num);
			break;
		case VMRegisterType::STRING:
			s += arg.value.str;
			break;
		case VMRegisterType::REGISTER:
			s += 'r' + std::to_string(arg.value.num);
			break;
		default:
			break;
		}
		s += ' ';
	}

	return s;
	// std::cout << s << std::endl;
}

#if defined(TOAD_EDITOR) || !defined(NDEBUG)
void Sim::OnImGui(Toad::Object* obj, ImGuiContext* ctx)
{
	ImGui::SetCurrentContext(ctx);

	ImGui::Begin("[Sim] command menu");

	//bool ImGui::InputTextMultiline(const char* label, char* buf, size_t buf_size, const ImVec2& size, ImGuiInputTextFlags flags, ImGuiInputTextCallback callback, void* user_data)

	static char source[1024];
	static std::vector<VM::Instruction> bytecodes;

	ImVec2 command_menu_window_size = ImGui::GetWindowSize();
	ImGui::BeginChild("Text Edit", {command_menu_window_size.x / 2.f - 10.f, command_menu_window_size.y});
	ImGui::InputTextMultiline("source", source, 1024, {command_menu_window_size.x, command_menu_window_size.y - 20.f});
	ImGui::EndChild();

	ImGui::SameLine();

	ImGui::BeginChild("VM options", {command_menu_window_size.x / 2.f - 10.f, command_menu_window_size.y});

	ImGui::BeginDisabled(!begin_play);
	if (ImGui::Button("Compile & Run"))
	{
		bytecodes.clear();
		if (Compiler::CompileString(source, bytecodes, AddLibsToParserCtx) != Compiler::CompileResult::ERR)
		{
			vm.instructions = bytecodes;
			vm.Init();
			run_vm = true;
			vm.Run();
		}
	}
	ImGui::SameLine();
	if (ImGui::Button("Stop"))
	{
		run_vm = false;
	}
	ImGui::EndDisabled();
	if (ImGui::TreeNode("Compiled Bytecodes View1"))
	{
		int i = 0;
		for (const VM::Instruction& ins : bytecodes)
		{
			ImGui::TextColored({1, 1, 1, 0.5f}, "[%d]", i);
			ImGui::SameLine();
			ImGui::Text(" %s", InstructionToStr(ins).c_str());
			i++;
		}

		ImGui::TreePop();
	}

	if (ImGui::TreeNode("Compiled Bytecodes View2"))
	{
		for (const VM::Instruction& ins : bytecodes)
		{
			ImGui::Text("%d", (int)ins.op);
			for (const VMRegister& arg : ins.args)
			{
				ImGui::SameLine();

				switch (arg.type)
				{
					case VMRegisterType::FLOAT:
						ImGui::Text("%.2f", arg.value.flt);
						break;
					case VMRegisterType::INT:
					case VMRegisterType::REGISTER:
						ImGui::Text("%d", arg.value.num);
						break;
					default: 
						ImGui::Text("?");
						break;
				}
			}
			if (ins.reserved.type != VMRegisterType::INVALID)
			{
				ImGui::SameLine();

				switch (ins.reserved.type)
				{
					case VMRegisterType::FLOAT:
						ImGui::Text("%.2f", ins.reserved.value.flt);
						break;
					case VMRegisterType::INT:
					case VMRegisterType::REGISTER:
						ImGui::Text("%d", ins.reserved.value.num);
						break;
					default: 
						ImGui::Text("?");
						break;
				}
			}
		}

		ImGui::TreePop();
	}
	if (ImGui::TreeNode("Errors", "Errors (%lu)", Compiler::error_msgs.size()))
	{
		for (const std::string& err : Compiler::error_msgs)
		{
			ImGui::TextColored({0.5f, 0, 0, 1}, "%s", err.c_str());
		}

		ImGui::TreePop();
	}
	if (ImGui::TreeNode("VM Registers", "VM Registers (%lu)", vm.registers.size()))
	{
		for (size_t i = 0; i < vm.registers.size(); i++)
		{
			const VMRegister& reg = vm.registers[i];

			if (reg.type == VMRegisterType::INVALID)
				break;

			switch(reg.type)
			{
				case VMRegisterType::REGISTER:
					ImGui::TextColored({1, 1, 1, 0.6f}, "Reg [%lu]", i);
					ImGui::SameLine();
					
					ImGui::TextColored({1,1,0,1}, "[Register]");
					ImGui::SameLine();
					ImGui::Text("%d", reg.value.num);
					break;
				case VMRegisterType::INT:
					ImGui::TextColored({1, 1, 1, 0.6f}, "Reg [%lu]", i);
					ImGui::SameLine();
					
					ImGui::TextColored({0.2f,0.2f,1,1}, "[Int]");
					ImGui::SameLine();
					ImGui::Text("%d", reg.value.num);
					break;
				case VMRegisterType::FLOAT:
					ImGui::TextColored({1, 1, 1, 0.6f}, "Reg [%lu]", i);
					ImGui::SameLine();
					
					ImGui::TextColored({0.2f,1,0.2f,1}, "[Float]");
					ImGui::SameLine();
					ImGui::Text("%f", reg.value.flt);
				break;

				default:
					break;
			}
		}
		ImGui::TreePop();

	}

	ImGui::EndChild();

	ImGui::End();

	ImGui::Begin("[Sim] fizzix menu");

	if (ImGui::Button("LoadCarScene"))
	{
		env_car_loaded = true;
		CarEnvironmentLoad();
		Toad::DrawingCanvas::ClearVertices();

		// copy default script to source 
		strncpy(source, CarControllerScript, strlen(CarControllerScript) + 1);

		for (fz::Polygon& p : sim.polygons)
			Toad::DrawingCanvas::AddVertexArray(p.vertices.size());
	}
	if (ImGui::Button("Clear"))
	{
		sim.polygons.clear();
		sim.springs.clear();
		Toad::DrawingCanvas::ClearVertices();
	}
    if (!sim.polygons.empty())
    {
        if (ImGui::Button("FORCE"))
        {
            sim.polygons[0].rb.velocity += Vec2f{0, -10.f};
        }
        if (ImGui::Button("ANGULARA"))
        {
            sim.polygons[0].rb.angular_velocity += 2.f;
            sim.polygons[1].rb.angular_velocity += 2.f;
        }if (ImGui::Button("ANGULARB"))
        {
            sim.polygons[0].rb.angular_velocity -= 2.f;
            sim.polygons[1].rb.angular_velocity -= 2.f;
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

	for (int i = 0; i < sim.springs.size(); i++)
	{
		ImGui::PushID(i);

		if (ImGui::TreeNode("Spring", "Spring %d", i))
		{
			fz::Spring& spr = sim.springs[i];

			ImGui::Text("attached A: (%.2f %.2f) B: (%.2f %.2f)", spr.start_rb->center.x,  spr.start_rb->center.y, spr.end_rb->center.x, spr.end_rb->center.y);
			
			ImGui::DragFloat("Stiffness", &spr.stiffness, 0.05f);
			ImGui::DragFloat("Rotational force factor", &spr.rotation_force_factor, 0.1f);
			ImGui::DragFloat("Rebound damping", &spr.rebound_damping, 0.05f);
			ImGui::DragFloat("Compression damping", &spr.compression_damping, 0.05f);
			ImGui::DragFloat("Target len", &spr.target_len);
			ImGui::DragFloat("Min len", &spr.min_len);
			
			ImGui::TreePop();
		}

		ImGui::PopID();
	}
	for (int i = 0; i < sim.polygons.size(); i++)
	{
		ImGui::PushID(i);

		if (ImGui::TreeNode("Object", "Object %d", i))
		{
			fz::Rigidbody& rb = sim.polygons[i].rb;

			ImGui::DragFloat("Moment of inertia", &rb.moment_of_inertia);
			ImGui::SliderFloat("Restitution", &rb.restitution, 0.0f, 1.f);
			
			if (ImGui::DragFloat("Mass", &rb.mass))
				if (rb.mass < 0.1f) 
					rb.mass = 0.1f;
			ImGui::SliderFloat("Friction", &rb.friction, 0.0f, 1.f);
			
			ImGui::Text("Center(%.2f, %.2f) Resting(%d) Slide(%.2f)", rb.center.x, rb.center.y, rb.resting, rb.slide);
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

			ImGui::TreePop();
		}

		ImGui::PopID();
	}

	ImGui::End();

	auto draw = ImGui::GetForegroundDrawList();

	draw->AddText({50.f, 120.f}, IM_COL32(255, 255, 0, 255), std::to_string(txt_to_draw.size()).c_str());
	int i = 10;
	for (const auto& [k, v] : txt_to_draw)
	{	
		draw->AddText({50.f, (float)i * 20.f}, IM_COL32(255, 255, 0, 255), v.c_str());
		i++;
	}

	if (!txt_to_draw.empty())
		txt_to_draw.clear();
}
#endif 
