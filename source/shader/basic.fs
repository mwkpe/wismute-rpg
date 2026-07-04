#version 330 core


uniform int color_mode;
uniform bool colorize;
uniform bool invert_color;
uniform bool desaturate;
uniform float desaturation_factor;
uniform vec4 color;
uniform sampler2D texture2d;

in vec2 texcoord;
in vec4 vertex_color;
out vec4 frag_color;


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

  if (colorize) {
    object_color = object_color * color;
  }

  if (desaturate) {
    vec3 grayscale = vec3(dot(object_color.rgb, vec3(0.2126, 0.7152, 0.0722)));
    object_color = vec4(mix(object_color.rgb, grayscale, desaturation_factor), object_color.a);
  }

  if (invert_color) {
    object_color = vec4(vec3(1.0, 1.0, 1.0) - object_color.rgb, object_color.a);
  }

  frag_color = object_color;
}
