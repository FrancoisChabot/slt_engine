
//it's assumed that this will be in screenspace
in vec2 in_vertex;
in vec2 in_uv;

out vec2 uv;

void main() {
  uv = in_uv;
  gl_Position = in_vertex;

}
