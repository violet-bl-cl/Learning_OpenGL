#version 460 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aColor;
out vec4 vertexColor;
out vec3 ourColor;
void main()
{
gl_Position = vec4(aPos, 1.0); 
ourColor = aColor;
vertexColor = vec4(0.5f,0.0f,0.0f,1.0f);
}