#include "Model.h"
#include "imgui.h"

#include "MarchingCubes.h"
#include "FileModelLoader.h"
#include "GPUAdvection.h"

#include <iostream>

#include "Events.h"

Model::Model(Container* cont) : Component(cont) {

    loaders.insert({ Marching_Cubes, std::make_unique<MarchingCubes>(cont) });
    //loaders.insert({ File, std::make_unique<FileModelLoader>(cont) });
    loaders.insert({ GPUAdvec, std::make_unique<GPUAdvection>(cont) });

    currentModelLoader = loaders.at(Marching_Cubes).get(); //Default is marching cubes
}

Model::~Model() {

}

//Initialize all models
void Model::init() {
    loaders.at(Marching_Cubes).get()->init();
    //loaders.at(File).get()->init(); //Nothing to initialize here
    loaders.at(GPUAdvec).get()->init();
}

void Model::guiRender() {

	if (ImGui::Begin("Model")) {

        ImGui::Text("Select Model Type");

        ImGui::SetNextItemWidth(-FLT_MIN);
        if (ImGui::BeginCombo("##Model Loader", meshTypeLabels[int(selectedMeshType)].c_str())) {

            for (int i = 0; i < int(2); ++i) {
                bool is_selected = (selectedMeshType == MeshType(i));
                if (ImGui::Selectable(meshTypeLabels[i].c_str(), is_selected)) {

                    if (selectedMeshType != MeshType(i)) {
                        currentModelLoader = loaders.at(MeshType(i)).get();
                        Notify<ChangedLoader>({ MeshType(i)});
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

        ImGui::Text("Model Stats");

        std::string vertDisp = "Vertices: " + std::to_string(currentModelLoader->_NumVertices);
        ImGui::Text(vertDisp.c_str());

        std::string faceDisp = "Faces: " + std::to_string(currentModelLoader->_NumVertices * 3);
        ImGui::Text(faceDisp.c_str());

        ImGui::Spacing();
        ImGui::Separator();
        ImGui::Spacing();

        //Model Loader specific UI
        if(currentModelLoader != nullptr)
            currentModelLoader->renderGUI();
	}

    ImGui::End();
}