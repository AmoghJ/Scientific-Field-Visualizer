#include "ComputeShader.h"

GLuint compileComputeShader(const std::string& src) {
    GLuint shader = glCreateShader(GL_COMPUTE_SHADER);
    const char* c = src.c_str();
    glShaderSource(shader, 1, &c, nullptr);
    glCompileShader(shader);

    GLint success;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success) {
        char log[512];
        glGetShaderInfoLog(shader, 512, nullptr, log);
        std::cerr << "Compute shader compile error:\n" << log << std::endl;
        return 0;
    }

    GLuint program = glCreateProgram();
    glAttachShader(program, shader);
    glLinkProgram(program);

    glGetProgramiv(program, GL_LINK_STATUS, &success);
    if (!success) {
        char log[512];
        glGetProgramInfoLog(program, 512, nullptr, log);
        std::cerr << "Compute shader link error:\n" << log << std::endl;
        glDeleteShader(shader);
        glDeleteProgram(program);
        return 0;
    }

    glDeleteShader(shader);
    return program;
}