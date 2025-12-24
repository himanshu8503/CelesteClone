#version 430 core

//input 
layout (location = 0) in vec2 textureCoodsIn;

//output 
layout (location = 0) out vec4 fragColor;

// binding
layout (location = 0) uniform sampler2D textureAtlas;

void main()
{
    vec4 textureColor = texelFetch(textureAtlas, ivec2(textureCoodsIn), 0);

    if(textureColor.a == 0.0)
    {
        discard;
    }

    fragColor = textureColor;
}