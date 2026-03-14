#include "OpenGLViewer.h"

#include <string>
#include <fstream>
#include <sstream>

#include "imgui.h"
#include "Console.h"
#include "FileMonitor.h"

#include <glm/gtc/type_ptr.hpp>

#include "Events.h"

// Standard functions for compiling and creating shaders
GLuint CompileShader(GLenum type, const std::string& source)
{
    GLuint shader = glCreateShader(type);
    const char* src = source.c_str();
    glShaderSource(shader, 1, &src, nullptr);
    glCompileShader(shader);

    // Check compile status
    GLint success;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        char infoLog[512];
        glGetShaderInfoLog(shader, sizeof(infoLog), nullptr, infoLog);
        std::cerr << "Shader compilation failed:\n" << infoLog << std::endl;
    }

    return shader;
}

GLuint CreateShaderProgram(const std::string& vertexSrc, const std::string& fragmentSrc)
{
    GLuint vertexShader = CompileShader(GL_VERTEX_SHADER, vertexSrc);
    if (!vertexShader) {
        std::cerr << "Vertex shader failed to compile" << std::endl;
        return 0;
    }

    GLuint fragmentShader = CompileShader(GL_FRAGMENT_SHADER, fragmentSrc);
    if (!fragmentShader) {
        std::cerr << "Fragment shader failed to compile" << std::endl;
        glDeleteShader(vertexShader);
        return 0;
    }

    GLuint program = glCreateProgram();
    glAttachShader(program, vertexShader);
    glAttachShader(program, fragmentShader);
    glLinkProgram(program);

    // Check link status
    GLint success;
    glGetProgramiv(program, GL_LINK_STATUS, &success);
    if (!success)
    {
        char infoLog[512];
        glGetProgramInfoLog(program, sizeof(infoLog), nullptr, infoLog);
        std::cerr << "Shader linking failed:\n" << infoLog << std::endl;

        glDeleteShader(vertexShader);
        glDeleteShader(fragmentShader);
        glDeleteProgram(program);
        return 0;
    }

    // Clean up
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    return program;
}

OpenGLViewer::OpenGLViewer(Container* cont) : Component(cont) {
    width = 720;
    height = 720;

    //default background color - hardcoded for now
    backgroundColor = glm::vec3(0.1f);

    //Get camera move events
    Subscribe<CameraMoveEvent>([this](const CameraMoveEvent& cm) {
        mvp = cm.mvp; //Update local mvp
    });
}

OpenGLViewer::~OpenGLViewer() {

}

void OpenGLViewer::init() {

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    //Fbo and texture setup
    {
        glGenFramebuffers(1, &renderFbo);
        glBindFramebuffer(GL_FRAMEBUFFER, renderFbo);

        glGenTextures(1, &renderTex);
        glBindTexture(GL_TEXTURE_2D, renderTex);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, width, height, 0, GL_RGBA, GL_FLOAT, nullptr);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, renderTex, 0);

        glGenRenderbuffers(1, &rbo);
        glBindRenderbuffer(GL_RENDERBUFFER, rbo);
        glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, width, height);
        glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rbo);

        // Check framebuffer status
        if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
            std::cout << "ERROR::FRAMEBUFFER:: Framebuffer is not complete!" << std::endl;
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }

    //Test render setup
    {
        //Bounding Cube
        {
            float cubeVertices[] = {
                // back face
                -0.5f,-0.5f,-0.5f,  0.5f,-0.5f,-0.5f,  0.5f, 0.5f,-0.5f,
                 0.5f, 0.5f,-0.5f, -0.5f, 0.5f,-0.5f, -0.5f,-0.5f,-0.5f,
                 // front face
                 -0.5f,-0.5f, 0.5f,  0.5f, 0.5f, 0.5f,  0.5f,-0.5f, 0.5f,
                  0.5f, 0.5f, 0.5f, -0.5f,-0.5f, 0.5f, -0.5f, 0.5f, 0.5f,
                  // left face
                  -0.5f,-0.5f,-0.5f, -0.5f, 0.5f, 0.5f, -0.5f,-0.5f, 0.5f,
                  -0.5f, 0.5f, 0.5f, -0.5f,-0.5f,-0.5f, -0.5f, 0.5f,-0.5f,
                  // right face
                   0.5f,-0.5f,-0.5f,  0.5f,-0.5f, 0.5f,  0.5f, 0.5f, 0.5f,
                   0.5f, 0.5f, 0.5f,  0.5f, 0.5f,-0.5f,  0.5f,-0.5f,-0.5f,
                   // top face
                   -0.5f, 0.5f,-0.5f,  0.5f, 0.5f, 0.5f, -0.5f, 0.5f, 0.5f,
                    0.5f, 0.5f, 0.5f, -0.5f, 0.5f,-0.5f,  0.5f, 0.5f,-0.5f,
                    // bottom face
                    -0.5f,-0.5f,-0.5f, -0.5f,-0.5f, 0.5f,  0.5f,-0.5f, 0.5f,
                     0.5f,-0.5f, 0.5f,  0.5f,-0.5f,-0.5f, -0.5f,-0.5f,-0.5f,
            };

            float cubeNormals[] = {
                // back face
                0,0,-1,  0,0,-1,  0,0,-1,
                0,0,-1,  0,0,-1,  0,0,-1,
                // front face
                0,0,1,   0,0,1,   0,0,1,
                0,0,1,   0,0,1,   0,0,1,
                // left face
                -1,0,0,  -1,0,0,  -1,0,0,
                -1,0,0,  -1,0,0,  -1,0,0,
                // right face
                1,0,0,   1,0,0,   1,0,0,
                1,0,0,   1,0,0,   1,0,0,
                // top face
                0,1,0,   0,1,0,   0,1,0,
                0,1,0,   0,1,0,   0,1,0,
                // bottom face
                0,-1,0,  0,-1,0,  0,-1,0,
                0,-1,0,  0,-1,0,  0,-1,0,
            };

            float cubeScalars[] = {
                // back face
                0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f,
                // front face
                1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f,
                // left face
                0.2f, 0.2f, 0.2f, 0.2f, 0.2f, 0.2f,
                // right face
                0.8f, 0.8f, 0.8f, 0.8f, 0.8f, 0.8f,
                // top face
                0.6f, 0.6f, 0.6f, 0.6f, 0.6f, 0.6f,
                // bottom face
                0.4f, 0.4f, 0.4f, 0.4f, 0.4f, 0.4f,
            };

            float cubeDisplacement[] = {
                // back face
                0,0,-1,  0,0,-1,  0,0,-1,
                0,0,-1,  0,0,-1,  0,0,-1,
                // front face
                0,0,1,   0,0,1,   0,0,1,
                0,0,1,   0,0,1,   0,0,1,
                // left face
                -1,0,0,  -1,0,0,  -1,0,0,
                -1,0,0,  -1,0,0,  -1,0,0,
                // right face
                1,0,0,   1,0,0,   1,0,0,
                1,0,0,   1,0,0,   1,0,0,
                // top face
                0,1,0,   0,1,0,   0,1,0,
                0,1,0,   0,1,0,   0,1,0,
                // bottom face
                0,-1,0,  0,-1,0,  0,-1,0,
                0,-1,0,  0,-1,0,  0,-1,0,
            };

            glGenVertexArrays(1, &vao);
            glGenBuffers(1, &vbo);
            glGenBuffers(1, &nVbo);
            glGenBuffers(1, &sVbo);
            glGenBuffers(1, &dVbo);

            glBindVertexArray(vao);

            glBindBuffer(GL_ARRAY_BUFFER, vbo);
            glBufferData(GL_ARRAY_BUFFER, sizeof(cubeVertices), cubeVertices, GL_DYNAMIC_DRAW);
            glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
            glEnableVertexAttribArray(0);

            glBindBuffer(GL_ARRAY_BUFFER, nVbo);
            glBufferData(GL_ARRAY_BUFFER, sizeof(cubeNormals), cubeNormals, GL_DYNAMIC_DRAW);
            glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
            glEnableVertexAttribArray(1);

            glBindBuffer(GL_ARRAY_BUFFER, sVbo);
            glBufferData(GL_ARRAY_BUFFER, sizeof(cubeScalars), cubeScalars, GL_DYNAMIC_DRAW);
            glVertexAttribPointer(2, 1, GL_FLOAT, GL_FALSE, 1 * sizeof(float), (void*)0);
            glEnableVertexAttribArray(2);

            glBindBuffer(GL_ARRAY_BUFFER, dVbo);
            glBufferData(GL_ARRAY_BUFFER, sizeof(cubeDisplacement), cubeDisplacement, GL_DYNAMIC_DRAW);
            glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
            glEnableVertexAttribArray(3);

            glBindVertexArray(0);

            numVertices = 36;
        }
    }

    updateShader();

    //Monitor Vert for hot reloading
    {
        std::string filePath = shaderDir + "/" + vertShaderName;
        std::wstring wFilePath = std::wstring(filePath.begin(), filePath.end());
        vertMonitor.beginMonitoring(wFilePath, [this]() {
            reloadShader = true;
        });
    }

    //Monitor Frag for hot reloading
    {
        std::string filePath = shaderDir + "/" + fragShaderName;
        std::wstring wFilePath = std::wstring(filePath.begin(), filePath.end());
        fragMonitor.beginMonitoring(wFilePath, [this]() {
            reloadShader = true;
        });
    }

    //Send back mesh data pointers
    Subscribe<GetMeshData>([this](GetMeshData& md) {
        md.posVbo = &vbo;
        md.normalVbo = &nVbo;
        md.scalarVbo = &sVbo;
        md.dispVbo = &dVbo;
    });

    //Subscribe to mesh data event and update buffer
    Subscribe<MeshData>([this](const MeshData& mD) {

        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        glBufferData(GL_ARRAY_BUFFER, mD.pos->size() * sizeof(glm::vec3), mD.pos->data(), GL_DYNAMIC_DRAW);

        glBindBuffer(GL_ARRAY_BUFFER, nVbo);
        glBufferData(GL_ARRAY_BUFFER, mD.normals->size() * sizeof(glm::vec3), mD.normals->data(), GL_DYNAMIC_DRAW);

        glBindBuffer(GL_ARRAY_BUFFER, sVbo);
        glBufferData(GL_ARRAY_BUFFER, mD.scalarField->size() * sizeof(float), mD.scalarField->data(), GL_DYNAMIC_DRAW);

        glBindBuffer(GL_ARRAY_BUFFER, dVbo);
        glBufferData(GL_ARRAY_BUFFER, mD.displacement->size() * sizeof(glm::vec3), mD.displacement->data(), GL_DYNAMIC_DRAW);

        numVertices = mD.pos->size();
    });
}

void OpenGLViewer::updateShader() {

    std::ifstream fragShader(shaderDir + "/" + fragShaderName);
    std::ifstream vertShader(shaderDir + "/" + vertShaderName);

    std::stringstream vertBuffer;
    vertBuffer << vertShader.rdbuf();

    std::stringstream fragBuffer;
    fragBuffer << fragShader.rdbuf();

    GLuint newShader = CreateShaderProgram(vertBuffer.str(), fragBuffer.str());
    if (newShader != 0) {
        glDeleteProgram(shader); // Only delete old if new is valid
        shader = newShader;
    }
    else {
        Console::Log("Shader reload failed, keeping previous shader", ERR);
    }

    reloadShader = false;

    mvpLocation = glGetUniformLocation(shader, "uMVP");
    displacementScaleLocation = glGetUniformLocation(shader, "uDisplacementScale");

    colorMapSelectorLocation = glGetUniformLocation(shader, "uColorMap");
    wireframeToggleLocation = glGetUniformLocation(shader, "uWireframe");
    rangeMinLocation = glGetUniformLocation(shader, "uRangeMin");
    rangeMaxLocation = glGetUniformLocation(shader, "uRangeMax");
    isolineBoolLocation = glGetUniformLocation(shader, "uIsoline");
    isolineValueLocation = glGetUniformLocation(shader, "uIsolineValue");
    
}

void OpenGLViewer::updateViewportSize(int w, int h) {

    //Only run function if update is necessary
    if (w == width && h == height)
        return;

    width = w;
    height = h;

    glBindTexture(GL_TEXTURE_2D, renderTex);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, width, height, 0, GL_RGBA, GL_FLOAT, nullptr);

    glBindRenderbuffer(GL_RENDERBUFFER, rbo);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, width, height);

    glBindFramebuffer(GL_FRAMEBUFFER, renderFbo);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, renderTex, 0);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rbo);

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        std::cout << "ERROR::FRAMEBUFFER:: Framebuffer is not complete!" << std::endl;

    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    Notify<ViewerResizeEvent>({ width, height });
}

static bool dragStartedInViewer = false;

void OpenGLViewer::update() {

    //View Window
    {
        ImGui::Begin("Viewer", NULL, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoBringToFrontOnFocus);

        ImVec2 contentSize = ImGui::GetContentRegionAvail();
        updateViewportSize((int)contentSize.x, (int)contentSize.y);

        //Mouse input handling

        { //Implemented scroll handling in dirty way - to prevent message queue from flooding
            float scrollY = ImGui::GetIO().MouseWheel;

            if (scrollY != lastScrollY) {
                Notify<MouseScrollEvent>(MouseScrollEvent(scrollY));
                lastScrollY = scrollY;
            }
        }

        if (ImGui::IsMouseClicked(ImGuiMouseButton_Left) ||
            ImGui::IsMouseClicked(ImGuiMouseButton_Right)) {
            dragStartedInViewer = ImGui::IsWindowHovered();
        }

        if (dragStartedInViewer) {
            if (ImGui::IsMouseDragging(ImGuiMouseButton_Left)) {
                ImVec2 mDel = ImGui::GetMouseDragDelta(ImGuiMouseButton_Left);
                Notify<LeftMouseDragEvent>(LeftMouseDragEvent(mDel.x, mDel.y));
                ImGui::ResetMouseDragDelta(ImGuiMouseButton_Left);
            }
            if (ImGui::IsMouseDragging(ImGuiMouseButton_Right)) {
                ImVec2 mDel = ImGui::GetMouseDragDelta(ImGuiMouseButton_Right);
                Notify<RightMouseDragEvent>(RightMouseDragEvent(mDel.x, mDel.y));
                ImGui::ResetMouseDragDelta(ImGuiMouseButton_Right);
            }
        }

        
        //ImVec2 contentSize = ImGui::GetContentRegionAvail();
        //ImVec2 renderSize = ImVec2(contentSize.x, contentSize.y);

        //updateViewportSize((int)renderSize.x, (int)renderSize.y);

        ImVec2 pos = ImGui::GetCursorScreenPos();
        ImGui::GetWindowDrawList()->AddImage(
            (ImTextureID)(getTextureID()),
            pos,
            ImVec2(pos.x + (float)width, pos.y + (float)height),
            ImVec2(0, 1),
            ImVec2(1, 0)
        );

        ImGui::PushStyleColor(ImGuiCol_ChildBg, ImVec4(0.08f, 0.08f, 0.08f, 1.0f));
        ImGui::BeginChild("##Transfer Function", ImVec2(contentSize.x, 38.0f), true);

        ImGui::SetNextItemWidth(80.0f);
        if (ImGui::BeginCombo("Visualization", transferFuncLabels[int(currentTransferFunc)].c_str())) {

            for (int i = 0; i < int(3); ++i) {
                bool is_selected = (currentTransferFunc == TransferFunctions(i));
                if (ImGui::Selectable(transferFuncLabels[i].c_str(), is_selected)) {

                    if (currentTransferFunc != TransferFunctions(i)) {

                    }

                    currentTransferFunc = TransferFunctions(i);
                }
                if (is_selected)
                    ImGui::SetItemDefaultFocus();
            }
            ImGui::EndCombo();
        }

        ImGui::SameLine(0.0f, 25.0f);
        ImGui::Text("|");
        ImGui::SameLine(0.0f, 25.0f);

        ImGui::SetNextItemWidth(240.0f);
        ImGui::DragFloatRange2("Range", &scalarRange[0], &scalarRange[1],
            0.005f,   // drag speed
            0.0f,     // min limit
            1.0f,     // max limit
            "Min: %.2f", "Max: %.2f");

        ImGui::SameLine(0.0f, 25.0f);
        ImGui::Text("|");
        ImGui::SameLine(0.0f, 25.0f);

        ImGui::Checkbox("Isoline", &showIsoline);
        if (showIsoline) {
            ImGui::SameLine();
            ImGui::SetNextItemWidth(100.0f);
            ImGui::SliderFloat("Value", &isolineValue, 0.0f, 1.0f, "%.2f");
        }

        ImGui::SameLine(0.0f, 25.0f);
        ImGui::Text("|");
        ImGui::SameLine(0.0f, 25.0f);

        ImGui::SetNextItemWidth(150.0f);
        ImGui::SliderFloat("Displacement Scale", &dispScale, 0.0f, 2.0f, "%.2f");

        ImGui::SameLine(0.0f, 25.0f);
        ImGui::Text("|");
        ImGui::SameLine(0.0f, 25.0f);

        ImGui::Checkbox("Wireframe", &wireframe);

        ImGui::EndChild(); //Transfer Function
        ImGui::PopStyleColor();

        ImGui::End(); //View Window
    }

    

}

void OpenGLViewer::render() {

    //Reload shader if update is required
    if (reloadShader)
        updateShader();

    glBindFramebuffer(GL_FRAMEBUFFER, renderFbo);
    glViewport(0, 0, width, height);
    glClearColor(backgroundColor.r, backgroundColor.g, backgroundColor.b, 1.0f);
    glEnable(GL_DEPTH_TEST);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glUseProgram(shader);

    glUniformMatrix4fv(mvpLocation, 1, GL_FALSE, glm::value_ptr(mvp));
    glUniform1i(colorMapSelectorLocation, currentTransferFunc);
    glUniform1i(wireframeToggleLocation, wireframe);
    glUniform1f(rangeMinLocation, scalarRange[0]);
    glUniform1f(rangeMaxLocation, scalarRange[1]);
    glUniform1i(isolineBoolLocation, showIsoline);
    glUniform1f(isolineValueLocation, isolineValue);
    glUniform1f(displacementScaleLocation, dispScale);

    glBindVertexArray(vao);
    glDrawArrays(GL_TRIANGLES, 0, numVertices);
    glBindVertexArray(0);

    glUseProgram(0);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

GLuint OpenGLViewer::getTextureID() {
    return renderTex;
}
