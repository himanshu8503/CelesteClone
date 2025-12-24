#version 430 core

// input 

// output
layout (location = 0 ) out vec2 textureCoodsOut;

void main()
{
    // genreting vertices on the GPU
    // mostly because we have a 2D Engine

    // opengl Coordinates
    //-1/1----------------------1/1
    //  |                        |
    //  |                        |
    //  |                        |
    //  |                        |
    //  |                        |
    //-1/-1---------------------1/-1


    vec2 vertices[6] = 
    {
        //top left
        vec2(-0.5,0.5),

        //bottom left
        vec2(-0.5,-0.5),

        //top right
        vec2(0.5,0.5),

        //top right
        vec2(0.5,0.5),

        //bottom left
        vec2(-0.5,-0.5),

        //bottom right
        vec2(0.5,-0.5)
    };

    float left = 0.0;
    float top = 0.0;
    float right = 16.0;
    float bottom = 16.0;

    vec2 textureCoods[6] = 
    {
        vec2(left,top),
        vec2(left,bottom),
        vec2(right,top),
        vec2(right,top),
        vec2(left,bottom),
        vec2(right,bottom) 
    };

    gl_Position = vec4(vertices[gl_VertexID], 1.0, 1.0);
    textureCoodsOut = textureCoods[gl_VertexID];
    
}