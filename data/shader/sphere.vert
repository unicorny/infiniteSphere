#version 120
uniform vec3 SphericalCenter;
uniform float theta;
uniform float theta2;
uniform vec2 rotation;

uniform sampler2D texture;

varying vec3 v_texCoord3D;


void main()
{
	vec4 scaledVertex = vec4(gl_Vertex.xy*sqrt(1.0-cos(theta))*1.0, 0.0, 1.0);
	
// make it spherical
	vec4 dir  = vec4(scaledVertex.xyz+SphericalCenter, 0.0);
	
	float l = length(dir);
	
		
//calculate texture coordinates
//	v_texCoord3D = normalize(vec4(gl_TextureMatrix[0] * normalize(nonScaledDir)).xyz)*lNS;
	gl_TexCoord[0] = vec4(-gl_Vertex.xyz*0.5+0.5, 1.0);//vec4(((-v_texCoord3D.xyz)*0.5+0.5), 1.0);//vec4(-transVertex.x*0.5+0.5, -transVertex.y*0.5+0.5, 0.0, 1.0);
	
	float h = texture2D(texture, gl_TexCoord[1].xy).z;
	h = 0.0;
	vec4 newVertex = scaledVertex + (dir / l) * ((1.0 - l));//+((h*2.0-1.0)/6378.0*6.0));
		
	gl_Position    = gl_ModelViewProjectionMatrix * newVertex;

	gl_FrontColor  = gl_Color;
}
