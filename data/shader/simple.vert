// simple vertex shader

uniform mat4x4 projection;

void main()
{
	gl_Position    = gl_ModelViewProjectionMatrix * projection * gl_Vertex;
	gl_FrontColor  = gl_Color;
	gl_TexCoord[0] = gl_Vertex;
}
