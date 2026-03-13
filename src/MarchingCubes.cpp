#include "MarchingCubes.h"

#include "imgui.h"
#include <iostream>
#include "Console.h"

MarchingCubes::MarchingCubes() {

    generatorLabels[0] = "Torus";
    generatorLabels[1] = "Gyroid";

    generators.insert({ Torus, std::make_unique<TorusGenerator>() });
    generators.insert({ Gyroid, std::make_unique<GyroidGenerator>() });

    selectedGenerator = generators.at(Torus).get();
}

MarchingCubes::~MarchingCubes() {

}

void MarchingCubes::renderGUI() {

    ImGui::Text("Object");

    ImGui::SetNextItemWidth(-FLT_MIN);
    if (ImGui::BeginCombo("##Select Procedural Object", generatorLabels[int(currentGenerator)].c_str())) {

        for (int i = 0; i < int(2); ++i) {
            bool is_selected = (currentGenerator == Generators(i));
            if (ImGui::Selectable(generatorLabels[i].c_str(), is_selected)) {

                if (currentGenerator != Generators(i)) {
                    selectedGenerator = generators.at(Generators(i)).get();
                }

                currentGenerator = Generators(i);
            }
            if (is_selected)
                ImGui::SetItemDefaultFocus();
        }
        ImGui::EndCombo();
    }

	ImGui::SliderFloat("Cell Size", &cellSize, 0.1f, 5.0f, "%.1f");

    if (ImGui::Button("Generate Mesh")) {
        Console::Log("Generating Mesh!");
    }
}