uniform vec3 edge1; //left/top
uniform vec3 edge2; //right/top
uniform vec3 edge3; //left/bottom
uniform vec3 edge4; //right/bottom

// simple vertex shader
void main()
{
	gl_Position    = gl_ModelViewProjectionMatrix * gl_Vertex;
	gl_FrontColor  = gl_Color;

	vec2 xDir = edge2.xy-edge1.xy;
	vec2 yDir = edge3.xy-edge1.xy;
	vec2 startPos1 = vec2(-1.0,-1.0);
	vec2 i =(-startPos1 + gl_Vertex.xy) / vec2(2.0, 2.0);

	gl_TexCoord[0] = vec4(edge1.xy + i*vec2(xDir.x, yDir.y), 0, 0);
}
