#version 430 core
// structs
struct Transform
{
    ivec2 atlasOffset;
    ivec2 spriteSize;
    vec2 pos;
    vec2 size;
};

// input 
layout(std430, binding = 0) buffer TransformSBO
{
    Transform transforms[];
};

// uniforms
uniform vec2 screenSize;

// output
layout (location = 0 ) out vec2 textureCoodsOut;


void main()
{
    Transform transform = transforms[gl_InstanceID];

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
        transform.pos,                                          //top left
        vec2(transform.pos + vec2(0.0,transform.size.y)),       //bottom left
        vec2(transform.pos + vec2(transform.size.x , 0.0)),     //top right
        vec2(transform.pos + vec2(transform.size.x , 0.0)),     //top right
        vec2(transform.pos + vec2(0.0,transform.size.y)),       //bottom left
        transform.pos + transform.size                          //bottom right
    };

    float left = transform.atlasOffset.x;
    float top = transform.atlasOffset.y;
    float right = transform.atlasOffset.x + transform.spriteSize.x;
    float bottom = transform.atlasOffset.y + transform.spriteSize.y;

    vec2 textureCoods[6] = 
    {
        vec2(left,top),
        vec2(left,bottom),
        vec2(right,top),
        vec2(right,top),
        vec2(left,bottom),
        vec2(right,bottom) 
    };


    // normalize Position
    {
        vec2 VertexPos = vertices[gl_VertexID];
        VertexPos.y = (-VertexPos.y) + screenSize.y;
        VertexPos = 2.0 * (VertexPos / screenSize) - 1.0;
        gl_Position = vec4(VertexPos, 0.0, 1.0);
    }

    // gl_Position = vec4(vertices[gl_VertexID], 0.0, 1.0);
    textureCoodsOut = textureCoods[gl_VertexID];
    
}