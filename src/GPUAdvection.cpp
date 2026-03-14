#include "GPUAdvection.h"

#include <fstream>
#include <sstream>
#include "imgui.h"

#include "ComputeShader.h"

#include "Events.h"

GPUAdvection::GPUAdvection(Container* cont) : IModelData(cont) {

}

GPUAdvection::~GPUAdvection() {

}

void GPUAdvection::init() {

    //Compile compute shader
    std::ifstream file("Shaders/GPUAdvection.comp");
    std::stringstream buffer;
    buffer << file.rdbuf();
    computeProgram = compileComputeShader(buffer.str());

    _NumVertices = 15000 * 8;

    timeLocation = glGetUniformLocation(computeProgram, "uTime");
    deltaTimeLocation = glGetUniformLocation(computeProgram, "uDeltaTime");
    numParticlesLocation = glGetUniformLocation(computeProgram, "uNumParticles");
    maxAgeLocation = glGetUniformLocation(computeProgram, "uMaxAge");
    boundsSizeLocation = glGetUniformLocation(computeProgram, "uBoundsSize");
    resetLocation = glGetUniformLocation(computeProgram, "uReset");
    trailLengthLocation = glGetUniformLocation(computeProgram, "uTrailLength");

    GetAdvectionData mD;
    Notify<GetAdvectionData>(mD);

    glBindBuffer(GL_SHADER_STORAGE_BUFFER, mD.posVbo);
    glBufferData(GL_SHADER_STORAGE_BUFFER, _NumVertices * sizeof(glm::vec4), nullptr, GL_DYNAMIC_COPY);

    glBindBuffer(GL_SHADER_STORAGE_BUFFER, mD.velVbo);
    glBufferData(GL_SHADER_STORAGE_BUFFER, _NumVertices * sizeof(glm::vec4), nullptr, GL_DYNAMIC_COPY);

    glBindBuffer(GL_SHADER_STORAGE_BUFFER, mD.ageVbo);
    glBufferData(GL_SHADER_STORAGE_BUFFER, _NumVertices * sizeof(float), nullptr, GL_DYNAMIC_COPY);

    glBindBuffer(GL_SHADER_STORAGE_BUFFER, mD.trailVbo);
    glBufferData(GL_SHADER_STORAGE_BUFFER, _NumVertices * mD.trailLength * sizeof(glm::vec4), nullptr, GL_DYNAMIC_COPY);

    glBindBuffer(GL_SHADER_STORAGE_BUFFER, mD.headVbo);
    glBufferData(GL_SHADER_STORAGE_BUFFER, _NumVertices * sizeof(int), nullptr, GL_DYNAMIC_COPY);

    //Initialize with default settings
    resetAdvection = true;
    runAdvectionPass();
}

void GPUAdvection::runAdvectionPass() {

    GetAdvectionData mD;
    Notify<GetAdvectionData>(mD);

    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, mD.posVbo);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, mD.velVbo);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 2, mD.ageVbo);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 3, mD.trailVbo);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 4, mD.headVbo);

    glUseProgram(computeProgram);

    glUniform1f(timeLocation, time);
    glUniform1f(deltaTimeLocation, deltaTime);
    glUniform1i(numParticlesLocation, _NumVertices);
    glUniform1f(maxAgeLocation, maxAge);
    glUniform1f(boundsSizeLocation, 32.0f);
    glUniform1i(resetLocation, resetAdvection);
    glUniform1i(trailLengthLocation, mD.trailLength);

    glDispatchCompute(_NumVertices /8, 1, 1);  // 8x8x8 thread groups
    glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT | GL_VERTEX_ATTRIB_ARRAY_BARRIER_BIT);

    if (resetAdvection)
        resetAdvection = false;
}

void GPUAdvection::renderGUI() {

    //timing
    auto now = std::chrono::high_resolution_clock::now();
    if (!timerInitialized) {
        lastTime = now;
        timerInitialized = true;
    }

    if (advectionRunning) {
        deltaTime = std::chrono::duration<float>(now - lastTime).count();
        deltaTime = glm::clamp(deltaTime, 0.0f, 0.05f);  // clamp to avoid spiral on lag spikes
        time += deltaTime;
    }
    lastTime = now;

    bool pushedRun = false;
    if (!advectionRunning) {
        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.8f, 0.4f, 0.0f, 1.0f));
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.9f, 0.5f, 0.1f, 1.0f));
        ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.7f, 0.3f, 0.0f, 1.0f));
        pushedRun = true;
    }

    if (ImGui::Button("Run Advection", ImVec2(ImGui::GetContentRegionAvail().x, 0))) {
        advectionRunning = true;
    }

    if (pushedRun)
        ImGui::PopStyleColor(3);

    bool pushedStop = false;
    if (advectionRunning) {
        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.8f, 0.4f, 0.0f, 1.0f));
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.9f, 0.5f, 0.1f, 1.0f));
        ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.7f, 0.3f, 0.0f, 1.0f));
        pushedStop = true;
    }

    if (ImGui::Button("Pause Advection", ImVec2(ImGui::GetContentRegionAvail().x, 0))) {
        advectionRunning = false;
        deltaTime = 0.0f;
        timerInitialized = false;
    }

    if (pushedStop)
        ImGui::PopStyleColor(3);

    if (ImGui::Checkbox("Reset Advection", &resetAdvection))
        runAdvectionPass(); //To reset immediately

    ImGui::SliderFloat("Max Age", &maxAge, 1.0f, 15.0f, "%.2f");

    if (advectionRunning) {
        runAdvectionPass(); //Run every frame
    }
}