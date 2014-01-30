#version 130

in vec3 in_position;

in vec4 in_color;
out vec4 pass_color;

void main(){
  gl_Position=vec4(in_position,1.0); 
  pass_color=in_color;
}
