#ifdef TOAD_EDITOR 
#include "framework/Framework.h"
#include "Fizzix/FZSim.h"
#include "SimEnvironments/CarEnvironment.h"
#include "scripts/Sim.h"

namespace UI
{
    void FizzixMenu(fz::Sim& sim, char* source, bool& env_car_loaded, bool& pause_sim)
    { 
        using namespace Toad;

        ImGui::Begin("[Sim] fizzix menu");

        if (ImGui::Button("LoadCarScene"))
        {
            env_car_loaded = true;
            CarEnvironmentLoad();
            DrawingCanvas::ClearVertices();

            // copy default script to source 
            strncpy(source, car_controller_script, strlen(car_controller_script) + 1);

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
}
#endif 