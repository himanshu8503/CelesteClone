#pragma once
#include "schnitzel_lib.h"
#include "gl_renderer.h"
#include <iostream>


// ########################################################
//                          OpenGL Structs
// ########################################################

struct GLContext
{
    GLuint ProgramID;
};


// ########################################################
//                          OpenGL Globels
// ########################################################

static GLContext glContext;


// ########################################################
//                          OpenGL Function 
// ########################################################
static void APIENTRY gl_debug_callback(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar* message, const void* user)
{
    if(severity == GL_DEBUG_SEVERITY_LOW ||
        severity == GL_DEBUG_SEVERITY_MEDIUM ||
        severity == GL_DEBUG_SEVERITY_HIGH
    )
    {
        SM_ASSERT(false,"OpenGL Error : %s", message);
    }
    else
    {
        SM_TRACE((char*)message);
    }
}

// this is the opengllear.com debug logging fuction
void APIENTRY glDebugOutput(GLenum source, 
                            GLenum type, 
                            unsigned int id, 
                            GLenum severity, 
                            GLsizei length, 
                            const char *message, 
                            const void *userParam)
{
    // ignore non-significant error/warning codes
    if(id == 131169 || id == 131185 || id == 131218 || id == 131204) return; 

    std::cout << "---------------" << std::endl;
    std::cout << "Debug message (" << id << "): " <<  message << std::endl;

    switch (source)
    {
        case GL_DEBUG_SOURCE_API:             std::cout << "Source: API"; break;
        case GL_DEBUG_SOURCE_WINDOW_SYSTEM:   std::cout << "Source: Window System"; break;
        case GL_DEBUG_SOURCE_SHADER_COMPILER: std::cout << "Source: Shader Compiler"; break;
        case GL_DEBUG_SOURCE_THIRD_PARTY:     std::cout << "Source: Third Party"; break;
        case GL_DEBUG_SOURCE_APPLICATION:     std::cout << "Source: Application"; break;
        case GL_DEBUG_SOURCE_OTHER:           std::cout << "Source: Other"; break;
    } std::cout << std::endl;

    switch (type)
    {
        case GL_DEBUG_TYPE_ERROR:               std::cout << "Type: Error"; break;
        case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR: std::cout << "Type: Deprecated Behaviour"; break;
        case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR:  std::cout << "Type: Undefined Behaviour"; break; 
        case GL_DEBUG_TYPE_PORTABILITY:         std::cout << "Type: Portability"; break;
        case GL_DEBUG_TYPE_PERFORMANCE:         std::cout << "Type: Performance"; break;
        case GL_DEBUG_TYPE_MARKER:              std::cout << "Type: Marker"; break;
        case GL_DEBUG_TYPE_PUSH_GROUP:          std::cout << "Type: Push Group"; break;
        case GL_DEBUG_TYPE_POP_GROUP:           std::cout << "Type: Pop Group"; break;
        case GL_DEBUG_TYPE_OTHER:               std::cout << "Type: Other"; break;
    } std::cout << std::endl;
    
    switch (severity)
    {
        case GL_DEBUG_SEVERITY_HIGH:         std::cout << "Severity: high"; break;
        case GL_DEBUG_SEVERITY_MEDIUM:       std::cout << "Severity: medium"; break;
        case GL_DEBUG_SEVERITY_LOW:          std::cout << "Severity: low"; break;
        case GL_DEBUG_SEVERITY_NOTIFICATION: std::cout << "Severity: notification"; break;
    } std::cout << std::endl;
    std::cout << std::endl;
}

bool gl_Init(BumpAllocator* transientStorage)
{
    gl_load_function();

    glDebugMessageCallback(&glDebugOutput,nullptr);
    glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
    glEnable(GL_DEBUG_OUTPUT);



    GLuint VertexShaderID = glCreateShader(GL_VERTEX_SHADER);
    GLuint FragmentShaderID = glCreateShader(GL_FRAGMENT_SHADER);

    int fileSize = 0;
    char* vertShader = read_file("assets/shaders/quad.vert",&fileSize,transientStorage);
    char* fragShader = read_file("assets/shaders/quad.frag",&fileSize,transientStorage);
    

    if(!vertShader || !fragShader)
    {
        SM_ASSERT(false,"Failed to load shaders");
        return false;
    }

    glShaderSource(VertexShaderID,1,&vertShader,0);
    glShaderSource(FragmentShaderID,1,&fragShader,0);

    glCompileShader(VertexShaderID);
    glCompileShader(FragmentShaderID);

    // test is vertex shader compiled successfully
    {
        int success;
        char shaderLog[2048] = {};

        glGetShaderiv(VertexShaderID,GL_COMPILE_STATUS,&success);
        if(!success)
        {
            glGetShaderInfoLog(VertexShaderID,2048,0,shaderLog);
            SM_ASSERT(false,"Failed to Compile Vertex Shader, Error : %s",shaderLog);
        }
    }

    
    // test is fragment shader compiled successfully
    {
        int success;
        char shaderLog[2048] = {};

        glGetShaderiv(FragmentShaderID,GL_COMPILE_STATUS,&success);
        if(!success)
        {
            glGetShaderInfoLog(FragmentShaderID,2048,0,shaderLog);
            SM_ASSERT(false,"Failed to Compile Vertex Shader, Error : %s",shaderLog);
        }
    }

    glContext.ProgramID = glCreateProgram();
    glAttachShader(glContext.ProgramID,VertexShaderID);
    glAttachShader(glContext.ProgramID,FragmentShaderID);
    glLinkProgram(glContext.ProgramID);

    glDetachShader(glContext.ProgramID,VertexShaderID);
    glDetachShader(glContext.ProgramID,FragmentShaderID);
    glDeleteShader(VertexShaderID);
    glDeleteShader(FragmentShaderID);

 
    GLuint VAO;
    glGenVertexArrays(1,&VAO);
    glBindVertexArray(VAO);

    // gl depth testing
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_GREATER);

    // Use Program
    glUseProgram(glContext.ProgramID);

    return true;
}

void gl_render()
{
    glClearColor(119.0f/255.0f, 33.0f / 255.0f, 111.0f / 255.0f, 1.0f);
    glClearDepth(0.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glViewport(0,0,input.ScreenSizeX,input.ScreenSizeY);

    glDrawArrays(GL_TRIANGLES, 0, 6);
}


