#include "app.h"


void VulkanApp::OnUpdateUIOverlay()
{
    ImGui::SetNextWindowPos(ImVec2(2, 2));
    ImGui::SetNextWindowSize(ImVec2(0, 0), ImGuiCond_FirstUseEver);
    ImGui::Begin(
        "Control Panel",
        nullptr,
        ImGuiWindowFlags_NoResize
        | ImGuiWindowFlags_AlwaysAutoResize
        | ImGuiWindowFlags_NoMove
        );

    ImGui::TextUnformatted(deviceProperties.deviceName);
    ImGui::SameLine();
    ImGui::Text("  %.2f ms/fr (%.1d fps)", (1000.0f / lastFPS), lastFPS);
    ImGui::Spacing();

    auto nSpheres = scene.spheres.size();
    auto nPlanes = scene.planes.size();
    int n = nSpheres;

    if (ImGui::CollapsingHeader("Scene", ImGuiTreeNodeFlags_DefaultOpen))
    {
        ImGui::Spacing();

        if (ImGui::TreeNodeEx("Point Light", ImGuiTreeNodeFlags_DefaultOpen))
        {
            // ImGui::DragFloat3("Position", &scene.ubo.lightPos.x);
            ImGui::SliderFloat3("Color", &scene.ubo.lightColor.x, 0, 25, "%.0f");
            ImGui::SliderFloat("Ambient", &scene.ubo.ambient, 0, 4, "%.1f");
            ImGui::TreePop();
        }

        ImGui::Spacing();

        if (ImGui::TreeNodeEx("Camera", ImGuiTreeNodeFlags_DefaultOpen))
        {
            ImGui::SliderFloat("Radius", &camera.radius, 1, 4, "%.1f");
            if (ImGui::IsItemEdited())
                camera.updatePosition();
            ImGui::TreePop();
        }

        ImGui::Spacing();

        char buf[24];

        ImGui::SetCursorPosX(ImGui::GetCursorPosX());
    
        ImGui::BeginChild("Sphere", ImVec2(110, 120), true);
        for (int i = 0; i < nSpheres; i++)
        {
            sprintf(buf, "%02d | %s %d", i, "Sphere", i);
            if (ImGui::Selectable(buf, scene.selected == i))
                scene.selected = i;
        }
        ImGui::EndChild();

        ImGui::SameLine();

        ImGui::BeginChild("Plane", ImVec2(110, 120), true);
        for (int i = 0; i < nPlanes; i++, n++)
        {
            sprintf(buf, "%02d | %s %d", n, "Plane ", i);
            if (ImGui::Selectable(buf, scene.selected == n))
                scene.selected = n;
        }
        ImGui::EndChild();

        ImGui::SameLine();

        ImGui::BeginChild("Object", ImVec2(110, 120), true, ImGuiWindowFlags_HorizontalScrollbar);
        if (ImGui::Selectable("Diamond", scene.selected == n))
            scene.selected = n;
        ImGui::EndChild();
    }

    ImGui::Spacing(); ImGui::Spacing(); ImGui::Spacing();

    if (ImGui::CollapsingHeader("Object Manager", ImGuiTreeNodeFlags_DefaultOpen))
    {
        ImGui::Spacing();

        if (ImGui::TreeNodeEx("Add Object"))
        {
            static int e = 0;
            ImGui::RadioButton("Sphere", &e, 0); ImGui::SameLine(150);
            if (ImGui::Button("Add"))
            {
                scene.addSphere({0,0,0}, 0.5);
                scene.selected = nSpheres++;
            }
            ImGui::TreePop();
        }

        ImGui::Spacing();

        if (ImGui::TreeNodeEx("Transform", ImGuiTreeNodeFlags_DefaultOpen))
        {
            if (scene.selected < nSpheres)
            {
                auto& sphere = scene.spheres[scene.selected];
                ImGui::DragFloat3("Translate", &sphere.pos.x, 0.05, -4, 4, "%.2f");
                ImGui::DragFloat("Radius", &sphere.radius, 0.02, 0.2, 1.5, "%.2f");
            }
            // else if (scene.selected == n)
            // {
            //     ImGui::DragFloat3("Translate", &scene.translate.x, 0.05);
            // }
            ImGui::TreePop();
        }

        ImGui::Spacing();

        if (ImGui::TreeNodeEx("Material", ImGuiTreeNodeFlags_DefaultOpen))
        {
            Material* m;
            if (scene.selected < nSpheres)
            {
                m = &scene.spheres[scene.selected].material;
            }
            else if (scene.selected == n)
            {
                m = &scene.ubo.material;
            }
            else
            {
                m = &scene.planes[scene.selected - nSpheres].material;
            }
            
            ImGui::ColorEdit3("Base Color", &m->color.x);

            const char* items[] = {
                "none", "metal", "shiny", "diffuse", "minor", "plastic", "glass", "diamond"
            };
            static int item_current = 0;
            static int isMetal;
            isMetal = m->ior ? 0 : 1;

            ImGui::Combo("Material", &item_current, items, IM_ARRAYSIZE(items));
            auto oldColor = m->color;
            if (item_current)
            {
                *m = scene.store.get(items[item_current]);
                m->color = oldColor;
                item_current = 0;
            }
            ImGui::RadioButton("Dielectric", &isMetal, 0); ImGui::SameLine();
            ImGui::RadioButton("Metallic", &isMetal, 1);


            if (isMetal)
            {
                ImGui::SliderFloat("Ka", &m->ka, 0, 1, "%.2f");
                ImGui::SliderFloat("Kd", &m->kd, 0, 1, "%.2f");
                ImGui::SliderFloat("Ks", &m->ks, 0, 3, "%.2f");
                ImGui::SliderFloat("Shininess", &m->shininess, 0, 8, "%.2f");
                ImGui::SliderFloat("Reflect", &m->k, 0, 1, "%.2f");
            }
            else
            {
                ImGui::SliderFloat("Reflect", &m->k, 0, 1, "%.2f");
                ImGui::SliderFloat("IOR", &m->ior, 0.5, 3, "%.2f");
            }

            ImGui::TreePop();
        }
    }

    ImGui::Spacing();
    ImGui::End();
}
