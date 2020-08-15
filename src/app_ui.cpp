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
    ImGui::Spacing(); ImGui::Spacing();

    float tr[4] = { 1.0f, 0.8, 0.7, 0.5 };


    if (UIOverlay.header("Scene"))
    {
        ImGui::Spacing();

        if (ImGui::TreeNodeEx("Light"))
        {
            static int e = 0;
            ImGui::RadioButton("Point", &e, 0); ImGui::SameLine(100);
            ImGui::RadioButton("Area", &e, 1);
            ImGui::DragFloat3("Position", &scene.ubo.lightPos.x);
            ImGui::TreePop();
        }

        ImGui::Spacing();

        if (ImGui::TreeNodeEx("Camera"))
        {
            ImGui::DragFloat3("Position", &(camera.position.x), 0.05);
            ImGui::TreePop();
        }

        ImGui::Spacing();


        ImGui::SetCursorPosX(ImGui::GetCursorPosX() + 10);
        ImGui::BeginChild("Sphere", ImVec2(100, 180), true, ImGuiWindowFlags_HorizontalScrollbar);

        auto nSpheres = scene.spheres.size();
        auto nPlanes = scene.planes.size();
        auto size = nSpheres + nPlanes;
        int n = nSpheres;

        char buf[24];

        for (int i = 0; i < nSpheres; i++)
        {
            sprintf(buf, "%02d | %s %d", i, "Sphere", i);
            if (ImGui::Selectable(buf, scene.selected == i))
            {
                scene.selected = i;
            }
        }

        ImGui::EndChild();

        ImGui::SameLine();

        ImGui::BeginChild("Plane", ImVec2(100, 180), true, ImGuiWindowFlags_HorizontalScrollbar);

        for (int i = 0; i < nPlanes; i++, n++)
        {
            sprintf(buf, "%02d | %s %d", n, "Plane ", i);
            if (ImGui::Selectable(buf, scene.selected == n))
            {
                scene.selected = n;
            }
        }

        ImGui::EndChild();

    }

    ImGui::Spacing(); ImGui::Spacing(); ImGui::Spacing();

    if (UIOverlay.header("Object Manager"))
    {
        ImGui::Spacing();

        if (ImGui::TreeNodeEx("Add Object", ImGuiTreeNodeFlags_DefaultOpen))
        {
            static int e = 0;
            ImGui::RadioButton("Cube", &e, 0); ImGui::SameLine(100);
            ImGui::RadioButton("Sphere", &e, 1); ImGui::SameLine(180);
            ImGui::RadioButton("Plane", &e, 2); ImGui::SameLine(260);
            if (ImGui::Button("Add"));
            ImGui::TreePop();
        }

        ImGui::Spacing();

        if (ImGui::TreeNodeEx("Transform", ImGuiTreeNodeFlags_DefaultOpen))
        {
            ImGui::DragFloat3("Translate", &controller.translate.x, 0.05);
            ImGui::DragFloat3("Rotate", &controller.rotate.x);
            ImGui::DragFloat3("Scale", &controller.scale.x, 0.05);
            ImGui::TreePop();
        }

        ImGui::Spacing();

        if (ImGui::TreeNodeEx("Material", ImGuiTreeNodeFlags_DefaultOpen))
        {
            static int e = 0;
            ImGui::RadioButton("Dielectric", &controller.isMetal, 0); ImGui::SameLine();
            ImGui::RadioButton("Metallic", &controller.isMetal, 1);
            // ImGui::SameLine();
            // ImGui::RadioButton("Diffuse", &controller.isMetal, 2);

            if (controller.isMetal == 0)
            {
                ImGui::ColorEdit3("Base Color", &controller.color.x);
            }
            else
            {
                const char* items[] = { "AAAA", "BBBB", "IIIIIII" };
                static int item_current = 0;
                ImGui::Combo("Texture", &item_current, items, IM_ARRAYSIZE(items));
            }

            ImGui::Spacing();

            static float m;

            ImGui::SliderFloat("Metallic", &m, 0, 1, "%.2f");
            ImGui::SliderFloat("Specular", &m, 0, 1, "%.2f");
            ImGui::SliderFloat("Roughness", &m, 0, 1, "%.2f");

            ImGui::TreePop();
        }
    }

    ImGui::Spacing(); ImGui::Spacing();

    ImGui::End();

    // ImGui::ShowDemoWindow();
}
