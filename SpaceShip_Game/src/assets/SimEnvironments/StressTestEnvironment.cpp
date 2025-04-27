#include "framework/Framework.h"
#include "StressTestEnvironment.h"

#include "scripts/Sim.h"
#include "Fizzix/FZSim.h"
#include "Fizzix/FZMath.h"

namespace SimEnvironments
{
    static float time = 0;
    static std::array<Toad::Vec2f, 6> square_vertices;
    static std::array<float, 50> fps_data;

    void StressTestEnvironmentLoad(size_t count)
    {
        fz::Sim& sim = Sim::GetSim();

        time = 0;
        square_vertices = fz::CreateSquare(25, 25);

        sim.polygons.clear();
        sim.springs.clear();
        
        std::array<Toad::Vec2f, 6> floor_vertices = fz::CreateSquare(1050, 50);
        std::array<Toad::Vec2f, 6> wall_vertices = fz::CreateSquare(50, 1000);

        fz::Polygon floor({floor_vertices.begin(), floor_vertices.end()});
        floor.rb.is_static = true;
        floor.Translate({-500, 0});

        fz::Polygon wall_left({wall_vertices.begin(), wall_vertices.end()});
        wall_left.rb.is_static = true;
        wall_left.Translate({-500, -1000});

        fz::Polygon wall_right({wall_vertices.begin(), wall_vertices.end()});
        wall_right.rb.is_static = true;
        wall_right.Translate({500, -1000});

        sim.AddPolygon(floor);
        sim.AddPolygon(wall_right);
        sim.AddPolygon(wall_left);
    }

    void StressTestImGui(Sim &sim_script)
    {
        fz::Sim& s = sim_script.GetSim();
        ImGui::Begin("[sim] stress test menu");
        static bool shoot = false;
        ImGui::Checkbox("shoot", &shoot);
        float dt = Toad::Time::GetDeltaTime();
        float fps = 1.f / dt;
        static size_t i = 0;
        fps_data[i] = fps;
        i = (i + 1) % fps_data.size();
        fps = 0;
        for (float v : fps_data)
            fps += v;
        fps /= fps_data.size();

        // float min_fps = *std::min(fps_data.begin(), fps_data.end());
        const float fps_threshold = 400.f;
        float g = std::min(1.f, fps / fps_threshold);
        ImGui::TextColored({1.f - g, g, 0, 1}, "FPS %.1f", fps);
        // ImGui::TextColored({0, std::max(1.f, min_fps / fps_threshold), 0, 1}, "FPS min %.1f", min_fps);
        static size_t polygon_add_counter = 0;
        ImGui::Text("%lu", polygon_add_counter);

        static float mass = 1.f;
        static float friction = 1.f;
        static float shoot_delay = 0.1f;
        if (ImGui::Button("Clear"))
        {
            Toad::DrawingCanvas::ClearVertices();
            s.polygons.clear();
        }
        
        ImGui::DragFloat("mass", &mass);
        ImGui::SliderFloat("friction", &friction, 0, 1);
        ImGui::DragFloat("shoot delay", &shoot_delay, 0.05f);

        if (shoot)
        {
            time += dt;
            if (time > shoot_delay)
            {
                fz::Polygon p({square_vertices.begin(), square_vertices.end()});
                p.Translate({0, -100});
                float r = ImSin(ImGui::GetTime() * 1.5f);
                p.rb.velocity.y = 50.f;
                p.rb.velocity.x = r * 100.f;
                p.rb.inv_mass = 1.f / mass;
                p.rb.friction = friction;
                s.AddPolygon(p);
				Toad::DrawingCanvas::AddVertexArray(square_vertices.size());
                time = 0;
                polygon_add_counter++;
            }

            if (polygon_add_counter > 50)
            {
                shoot = false;
                time = 0;
                polygon_add_counter = 0;
            }
            // if (fps < fps_threshold)
            //     shoot = false;
        }

        ImGui::End();
    }
}