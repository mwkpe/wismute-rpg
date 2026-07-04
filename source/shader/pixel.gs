#version 330 core


layout (points) in;
layout (triangle_strip, max_vertices=4) out;

in vec4 pixel_color[];
in mat4 pixel_model[];
in vec2 pixel_stretch[];

flat out vec4 vertex_color;

uniform mat4 model;
uniform mat4 view_projection;

uniform float pixel_size;

void main()
{
  vec4 pos = gl_in[0].gl_Position;
  float offset_x = pixel_size * 0.5 * pixel_stretch[0].x;
  float offset_z = pixel_size * 0.5 * pixel_stretch[0].y;

  vec4 tl = pixel_model[0] * vec4(pos.x - offset_x, pos.y, pos.z - offset_z, 1.0);
  vec4 tr = pixel_model[0] * vec4(pos.x + offset_x, pos.y, pos.z - offset_z, 1.0);
  vec4 bl = pixel_model[0] * vec4(pos.x - offset_x, pos.y, pos.z + offset_z, 1.0);
  vec4 br = pixel_model[0] * vec4(pos.x + offset_x, pos.y, pos.z + offset_z, 1.0);

  vertex_color = pixel_color[0];

  gl_Position = view_projection * bl;
  EmitVertex();

  gl_Position = view_projection * br;
  EmitVertex();

  gl_Position = view_projection * tl;
  EmitVertex();

  gl_Position = view_projection * tr;
  EmitVertex();

  EndPrimitive();
}
