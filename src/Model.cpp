#include "Model.h"
#include "imgui.h"

#include "MarchingCubes.h"
#include "FileModelLoader.h"

Model::Model(Container* cont) : Component(cont) {

    meshTypeLabels[0] = "Marching Cubes (procedural)";
    meshTypeLabels[1] = "Import from file";

    loaders.insert({ Marching_Cubes, std::make_unique<MarchingCubes>(cont) });
    loaders.insert({ File, std::make_unique<FileModelLoader>(cont) });

    currentModelLoader = loaders.at(Marching_Cubes).get(); //Default is marching cubes
}

Model::~Model() {

}

//TODO: Add Mesh Statistics
void Model::guiRender() {

	if (ImGui::Begin("Model")) {

        ImGui::Text("Select Model Type");

        ImGui::SetNextItemWidth(-FLT_MIN);
        if (ImGui::BeginCombo("##Model Loader", meshTypeLabels[int(selectedMeshType)].c_str())) {

            for (int i = 0; i < int(2); ++i) {
                bool is_selected = (selectedMeshType == MeshType(i));
                if (ImGui::Selectable(meshTypeLabels[i].c_str(), is_selected)) {

                    if (selectedMeshType != MeshType(i)) { //TODO: Notify change of meshLoader
                        currentModelLoader = loaders.at(MeshType(i)).get();
                    }

                    selectedMeshType = MeshType(i);
                }
                if (is_selected)
                    ImGui::SetItemDefaultFocus();
            }
            ImGui::EndCombo();
        }

        ImGui::Spacing();
        ImGui::Separator();
        ImGui::Spacing();
        //Model Loader specific UI
        if(currentModelLoader != nullptr)
            currentModelLoader->renderGUI();
	}

    ImGui::End();
}