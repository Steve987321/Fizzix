// #define TOAD_EDITOR
#ifdef TOAD_EDITOR 
#include "framework/Framework.h"
#include "Fizzix/FZSim.h"
#include "SimEnvironments/CarEnvironment.h"
#include "SimEnvironments/StressTestEnvironment.h"
#include "scripts/Sim.h"

namespace UI
{
    
    void FizzixMenu(Sim& sim_script, char* source)
    { 
        using namespace Toad;

        ImGui::Begin("[Sim] fizzix menu");

        fz::Sim& sim = sim_script.GetSim();

        if (ImGui::Button("Load CarScene"))
        {
            sim_script.env_car_loaded = true;
            sim_script.env_stress_test_loaded = false;
            SimEnvironments::CarEnvironmentLoad();
            DrawingCanvas::ClearVertices();

            // copy default script to source 
            strncpy(source, SimEnvironments::car_controller_script, strlen(SimEnvironments::car_controller_script) + 1);

            for (fz::Polygon& p : sim.polygons)
                DrawingCanvas::AddVertexArray(p.vertices.size());
        }
        if (ImGui::Button("Load StressTest"))
        {
            sim_script.env_car_loaded = false;
            sim_script.env_stress_test_loaded = true;
            SimEnvironments::StressTestEnvironmentLoad(0);

            DrawingCanvas::ClearVertices();
            for (fz::Polygon& p : sim.polygons)
                DrawingCanvas::AddVertexArray(p.vertices.size());
        }

        if (ImGui::Button("Clear"))
        {
            sim.polygons.clear();
            sim.springs.clear();
            DrawingCanvas::ClearVertices();
        }
        if (!sim.polygons.empty())
        {
            if (ImGui::Button("FORCE"))
            {
                sim.polygons[0].rb.velocity += Vec2f{0, -10.f};
            }
            if (ImGui::Button("ANGULARA"))
            {
                // Sim::env_car_gas = -10.f;
                sim.polygons[0].rb.angular_velocity += 2.f;
                // sim.polygons[1].rb.angular_velocity += 2.f;
            }if (ImGui::Button("ANGULARB"))
            {
                // Sim::env_car_gas = 10.f;
                sim.polygons[0].rb.angular_velocity -= 2.f;
                // sim.polygons[1].rb.angular_velocity -= 2.f;
            }

        }

        static float angle = 0;
        static float scale = 1.f;
        static float fdt = Time::GetFixedDeltaTime();
        ImGui::DragFloat("Angle", &angle);
        ImGui::Checkbox("Pause", &sim_script.pause_sim);
        ImGui::DragFloat("Time scale", &scale, 0.05f);
        ImGui::DragFloat("Fixed time step", &fdt, 0.01f);
        ImGui::Checkbox("Show AABB", &sim_script.show_aabb);
        ImGui::Checkbox("Show Velocities", &sim_script.show_velocities);
        
        float grav_edit[2] = {sim.gravity.x, sim.gravity.y};
        if (ImGui::DragFloat2("Grav", grav_edit, 0.1f))
        {	
            sim.gravity.x = grav_edit[0];
            sim.gravity.y = grav_edit[1];
        }
        if (ImGui::Button("Set time scale"))
            Time::SetTimeScale(scale);
        if (ImGui::Button("Set fixed DT"))
            Time::SetFixedDeltaTime(fdt);

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
                
                if (ImGui::DragFloat("InvMass", &rb.inv_mass, 0.01f))
                    if (rb.inv_mass <= 0.0001f) 
                        rb.inv_mass = 0.0001f;
                        
                ImGui::SliderFloat("Friction", &rb.friction, 0.0f, 1.f);
                
                ImGui::Text("Center(%.2f, %.2f) Sleeping(%d) Slide(%.2f)", rb.center.x, rb.center.y, rb.is_sleeping, rb.slide);
                ImGui::Text("Angular(%.2f)", rb.angular_velocity);
                ImGui::Text("Vel(%.2f, %.2f) VelLength(%.3f)", rb.velocity.x, rb.velocity.y, rb.velocity.Length());
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
        Camera* cam = Camera::GetActiveCamera(); 
        if (cam)
            for (const auto& [min, max] : rect_to_draw)
            {
                Vec2f min_screen = Screen::WorldToScreen(min, *cam);
                Vec2f max_screen = Screen::WorldToScreen(max, *cam);
                draw->AddRect({min_screen.x, min_screen.y}, {max_screen.x, max_screen.y}, IM_COL32_WHITE, 0, 0, 2.f);
            }

        if (!txt_to_draw.empty())
            txt_to_draw.clear();
        if (!rect_to_draw.empty())
            rect_to_draw.clear();
            
    }
}
#endif 