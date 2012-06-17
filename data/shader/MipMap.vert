uniform vec3 edge[4];
/*uniform vec3 edge1; //left/top (-1,-1)
uniform vec3 edge2; //left/bottom (-1,1)
uniform vec3 edge3; //right/bottom (1,1)
uniform vec3 edge4; //right/top (1,-1)
*/

/*
geometrie vertices

1/1     -1/1
   _____
   |   |
   |   |
   -----
1/-1     -1/-1
*/

uniform mat4x4 projection;
uniform mat4x4 texturRotation;

// simple vertex shader
void main()
{
	gl_Position    = gl_ModelViewProjectionMatrix * projection * gl_Vertex;
	gl_FrontColor  = gl_Color;

	vec2 startPos1 = vec2(1.0,1.0);
	vec2 i = (startPos1 - gl_Vertex.xy) / vec2(2.0, 2.0);
	vec2 dir = vec2(edge[3].x-edge[0].x+i.y*(edge[2].x-edge[3].x-edge[1].x+edge[0].x),
					edge[1].y-edge[0].y+i.x*(edge[2].y-edge[1].y-edge[3].y+edge[0].y));

	gl_TexCoord[0] = vec4(edge[0]+i*dir, 0, 0);
//	gl_TexCoord[0] = gl_Vertex;
}
