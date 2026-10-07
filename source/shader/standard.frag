#version 330 core


in vec2 texcoord;
in vec4 vertex_color;
out vec4 frag_color;


uniform int color_mode;

uniform bool desaturate_color;
uniform bool invert_color;
uniform bool multiply_color;

uniform float desaturation_factor;
uniform vec4 color;
uniform sampler2D texture2d;


void main()
{
  vec4 object_color;

  switch (color_mode) {
    case 0:
      object_color = vertex_color;
    break;
    case 1:
      object_color = color;
    break;
    case 2:
      object_color = texture(texture2d, texcoord);
    break;
    default:
      object_color = vec4(1.0, 0.0, 1.0, 1.0);
  }

  if (multiply_color) {
    object_color = object_color * color;
  }

  if (desaturate_color) {
    vec3 grayscale = vec3(dot(object_color.rgb, vec3(0.2126, 0.7152, 0.0722)));
    object_color = vec4(mix(object_color.rgb, grayscale, desaturation_factor), object_color.a);
  }

  if (invert_color) {
    object_color = vec4(vec3(1.0, 1.0, 1.0) - object_color.rgb, object_color.a);
  }

  frag_color = object_color;
}
