#pragma once
#include "schnitzel_lib.h"
#include "gl_renderer.h"
#include <iostream>

// ########################################################
//                          OpenGL Constant
// ########################################################
const char* TEXTURE_PATH = "assets/textures/Textue_atlas.png";

// ########################################################
//                          OpenGL Structs
// ########################################################

struct GLContext
{
    GLuint ProgramID;
    GLuint TextureID;
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

    // texture loading using stbi
    {
        int width, height, channels;
        char* data = (char*)stbi_load(TEXTURE_PATH,&width,&height,&channels,4);

        if(!data)
        {
            SM_ASSERT(false,"Failed to load texture");
            return false;
        }

        glGenTextures(1,&glContext.TextureID);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D,glContext.TextureID);

        // set the texture wrapping/filtering options (on the currently bound texture object)
        glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S,GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T,GL_CLAMP_TO_EDGE);
        // This setting only matters when using the GLSL texture() function
        // When you use texelFetch() this setting has no effect,
        // because texelFetch is designed for this purpose
        // See: https://interactiveimmersive.io/blog/glsl/glsl-data-tricks/
        glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);

        glTexImage2D(GL_TEXTURE_2D,0,GL_SRGB8_ALPHA8,width,height,0,GL_RGBA,GL_UNSIGNED_BYTE,data);
        stbi_image_free(data);

    }

    // sRGB output (even if input texture is non-sRGB -> don't rely on texture used)
    // Your font is not using sRGB, for example (not that it matters there, because no actual color is sampled from it)
    // But this could prevent some future bug when you start mixing different types of textures
    // Of course, you still need to correctly set the image file source format when using glTexImage2D()    
    glEnable(GL_FRAMEBUFFER_SRGB);
    glDisable(0x809D); // disable multisampling

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


