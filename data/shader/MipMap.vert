uniform vec3 edge1; //left/top
uniform vec3 edge2; //right/top
uniform vec3 edge3; //left/bottom
uniform vec3 edge4; //right/bottom

uniform mat4x4 projection;
uniform mat4x4 texturRotation;

// simple vertex shader
void main()
{
	gl_Position    = gl_ModelViewProjectionMatrix * projection * gl_Vertex;
	gl_FrontColor  = gl_Color;

	vec3 minv = vec3(-1.0,-1.0, 0.0);
	vec3 maxv = vec3(1.0, 1.0, 0.0);
	edge1 = clamp(edge1, minv, maxv);
	edge2 = clamp(edge2, minv, maxv);
	edge3 = clamp(edge3, minv, maxv);
	edge4 = clamp(edge4, minv, maxv);

	vec2 xDir = edge2.xy-edge1.xy;
	vec2 yDir = edge3.xy-edge1.xy;
	vec2 dir  = edge4.xy-edge1.xy;
	vec2 startPos1 = vec2(-1.0,-1.0);
	vec2 i =(-startPos1 + gl_Vertex.xy) / vec2(2.0, 2.0);

	gl_TexCoord[0] = vec4(edge1.xy + i*dir, 0, 0);
//	gl_TexCoord[0] = gl_Vertex;
}
