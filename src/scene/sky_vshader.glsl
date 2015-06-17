#version 330 core
in vec3 vertex_pos;
in vec2 tex_pos;

out vec2 uv;

uniform mat4 mvp;

void main()
{
    gl_Position =  mvp * vec4(vertex_pos, 1.0);  
    uv = tex_pos;
}