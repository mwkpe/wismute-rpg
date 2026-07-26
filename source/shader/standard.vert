#version 330 core


layout (location = 0) in vec3 a_position;
layout (location = 1) in vec3 a_normal;
layout (location = 2) in vec2 a_texcoord;
layout (location = 3) in vec4 a_color;

uniform int render_mode;

// Screen
uniform vec3 scale;
uniform vec3 translation;
uniform mat4 projection;

// World
uniform mat4 model;
uniform mat4 view_projection;

out vec2 texcoord;
out vec4 vertex_color;


void main()
{
  if (render_mode == 0) {
    vec4 world_position = model * vec4(a_position, 1.0);
    gl_Position = view_projection * world_position;
  }
  else {
    vec3 screen_pos = a_position * scale + translation;
    gl_Position = projection * vec4(screen_pos, 1.0);
  }

  texcoord = a_texcoord;
  vertex_color = a_color;
}
