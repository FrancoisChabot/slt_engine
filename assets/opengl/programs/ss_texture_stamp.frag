layout(binding = 0) uniform sampler2D tex;

in vec2 uv;
out vec4 result_color; 

void main() {
  result_color = texture(tex, uv);
}