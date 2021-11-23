#shader vertex
#version 330 core

in vec3 position;
in vec3 color_in;
out vec3 color_out;

void main()
{
   gl_Position = vec4(position, 1);
   color_out = color_in;
};


#shader fragment
#version 330 core

in vec3 color_out;
out vec4 new_c;

void main()
{
   new_c = vec4(color_out, 1);
};