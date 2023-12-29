#version 430

in vec3 vPosition;
in vec4 vColor;
out vec4 fColor;

void main()
{
	fColor = vColor;
	gl_Position = vec4(vPosition, 1.0);
}