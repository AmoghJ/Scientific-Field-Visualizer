#include "OpenGLViewer.h"

#include <string>
#include <fstream>
#include <sstream>

#include "imgui.h"
#include "Console.h"
#include "FileMonitor.h"

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
}

OpenGLViewer::~OpenGLViewer() {

}

void OpenGLViewer::init() {

    glEnable(GL_BLEND);
    glBlendFunc(GL_ONE, GL_ONE);  // Additive blending

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
        float quadVertices[] = {
        -1.0f, -1.0f, 0.0f, 0.0f, // bottom-left
         1.0f, -1.0f, 1.0f, 0.0f, // bottom-right
        -1.0f,  1.0f, 0.0f, 1.0f, // top-left
         1.0f,  1.0f, 1.0f, 1.0f  // top-right
        };

        unsigned int indices[] = {
            0, 1, 2,
            2, 1, 3
        };

        glGenVertexArrays(1, &vao);
        glGenBuffers(1, &vbo);
        glGenBuffers(1, &ebo);

        glBindVertexArray(vao);
        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), quadVertices, GL_STATIC_DRAW);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);

        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));

        glBindVertexArray(0);
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
}

void OpenGLViewer::update() {

    //View Window
    {
        ImGui::Begin("Viewer", NULL);//, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoBringToFrontOnFocus);
        ImVec2 pos = ImGui::GetCursorScreenPos();
        float winHeight = ImGui::GetWindowSize().y;
        float imgRatio = float(width) / float(height);
        float scaledImageWidth = imgRatio * winHeight;

        // Center image in content region
        float startPos = pos.x + (ImGui::GetContentRegionAvail().x - scaledImageWidth) * 0.5f;

        ImVec2 minRect = ImVec2(startPos, pos.y);
        ImVec2 maxRect = ImVec2(startPos + scaledImageWidth, pos.y + winHeight);

        // Draw image
        ImGui::GetWindowDrawList()->AddImage(
            (ImTextureID)(getTextureID()),
            minRect,
            maxRect,
            ImVec2(0, 1),
            ImVec2(1, 0)
        );

        ImGui::End();
    }

}

void OpenGLViewer::render() {

    //Reload shader if update is required
    if (reloadShader)
        updateShader();

    glBindFramebuffer(GL_FRAMEBUFFER, renderFbo);
    glViewport(0, 0, width, height);
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glUseProgram(shader);

    glBindVertexArray(vao);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);

    glUseProgram(0);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

GLuint OpenGLViewer::getTextureID() {
    return renderTex;
}
