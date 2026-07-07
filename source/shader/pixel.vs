#version 330 core


layout (location = 0) in uint a_index;
layout (location = 1) in uint a_color;
layout (location = 2) in uint a_material;


out vec4 pixel_color;
out mat4 pixel_model;
out vec2 pixel_stretch;


uniform mat4 model;

// Game constants
uniform float pixel_size;
uniform uint tile_size;
uniform vec4 colors[53];

// Generic variables
uniform float time;
uniform uvec2 tile_position;

// Post process variables
uniform bool blending_enabled;
uniform bool desaturation_enabled;
uniform bool tile_tilt_enabled;
uniform bool pixel_displacement_enabled;
uniform bool breathe_enabled;

uniform float blending_alpha;
uniform float desaturation_factor;
uniform float tile_tilt;
uniform float pixel_displacement;
uniform float breathe_amplitude;
uniform float breathe_speed;
uniform float breathe_phase;


uint pcg_hash(uint v)
{
  uint state = v * 747796405u + 2891336453u;
  uint word = ((state >> ((state >> 28u) + 4u)) ^ state) * 277803737u;
  return (word >> 22u) ^ word;
}


// Returns a float in [0, 1)
float hash_to_float(uint h)
{
  return float(h) * (1.0 / 4294967296.0);
}


float wave_offset(float col, float row, float count, float amplitude, float speed, float tilt)
{
  const float tau = 6.283185;

  float coord = (col + tilt * row) / float(tile_size);
  float phase = coord * tau * count - time * speed;

  return sin(phase) * amplitude;
}


float wave_offset(float col, float row, float count, float amplitude, float sharpness, float speed,
    float tilt)
{
  const float tau = 6.283185;

  float coord = (col + tilt * row) / float(tile_size);
  float phase = coord * tau * count - time * speed;

  return exp(sharpness * (cos(phase) - 1.0)) * amplitude;
}


void apply_material(inout vec3 pixel_position)
{
  uint tile_col = a_index % tile_size;
  uint tile_row = a_index / tile_size;

  float global_col = float(tile_position.x * tile_size + tile_col);
  float global_row = float(tile_position.y * tile_size + tile_row);

  // Water
  if (a_material == 1u) {
    float offset = wave_offset(global_col, global_row, 4.0, 0.06, 2.5, 0.2) +
        wave_offset(global_col, global_row, 3.0, 0.04, 1.0, 0.6);

    const uint trough_color = 14u;
    float trough_threshold = pixel_size * -0.3;

    if (offset < trough_threshold) {
      pixel_color = colors[trough_color];
    }

    pixel_position.z -= offset;
  }

  // Foliage
  if (a_material == 2u) {
    uint seed = a_index + tile_position.x * 7919u + tile_position.y * 104729u;
    uint h1 = pcg_hash(seed);
    uint h2 = pcg_hash(h1);
    uint h3 = pcg_hash(h2);

    float wind_x = wave_offset(global_col, global_row, 5.0, hash_to_float(h1) * 0.1, 10.0, 0.45);
    float wind_z = wave_offset(global_col, global_row, 4.0, hash_to_float(h2) * 0.1, 10.0, 0.65);

    float jitter_strength = pixel_size * 0.3;
    const float wind_strength = 0.1;

    pixel_position.x += (hash_to_float(h1) * 2.0 - 1.0) * jitter_strength + wind_x * wind_strength;
    pixel_position.z += (hash_to_float(h2) * 2.0 - 1.0) * jitter_strength + wind_z * wind_strength;
    pixel_position.y += 0.01 + hash_to_float(h3) * jitter_strength * 0.2;
  }
}


void post_process(inout vec3 pixel_position, float row)
{
  pixel_model = model;

  if (breathe_enabled) {
    float s = 1.0 + breathe_amplitude * sin(time * breathe_speed + breathe_phase);
    float sx = 1.0 / sqrt(s);

    // Anchor sprite to ground
    float anchor = float(tile_size) * pixel_size * 0.5;

    pixel_position.z = anchor + (pixel_position.z - anchor) * s;
    pixel_position.x *= sx;

    pixel_stretch = vec2(sx, s);
  }

  if (tile_tilt_enabled) {
    pixel_position.x += (tile_size * pixel_size - row) * 0.1 * tile_tilt;
  }

  if (pixel_displacement_enabled) {
    vec3 direction = normalize(pixel_position);
    pixel_position += direction * pixel_displacement;
  }

  if (blending_enabled) {
    pixel_color.a = blending_alpha;
  }

  if (desaturation_enabled) {
    vec3 grayscale = vec3(dot(pixel_color.rgb, vec3(0.2126, 0.7152, 0.0722)));
    pixel_color = vec4(mix(pixel_color.rgb, grayscale, desaturation_factor), pixel_color.a);
  }
}


void main()
{
  float col = float(a_index % tile_size) * pixel_size;
  float row = float(a_index / tile_size) * pixel_size;

  // Tile and pixel origin is center
  vec3 offset;
  offset.x = tile_size * pixel_size * 0.5 - pixel_size * 0.5;
  offset.y = 0.0;
  offset.z = tile_size * pixel_size * 0.5 - pixel_size * 0.5;

  vec3 pixel_position = vec3(col, 0.0, row) - offset;
  pixel_color = colors[a_color];
  pixel_stretch = vec2(1.0);

  apply_material(pixel_position);
  post_process(pixel_position, row);

  gl_Position = vec4(pixel_position, 1.0);
}
