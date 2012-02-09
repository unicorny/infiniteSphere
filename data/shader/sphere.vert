uniform vec3 SphericalCenter;
uniform float theta;

uniform sampler2D texture;
varying vec3 v_texCoord3D;

void main()
{
	vec4 scaledVertex = vec4(gl_Vertex.xyz*sqrt(1.0-cos(theta)), 1.0);
// make it spherical
	vec4 dir  = scaledVertex-vec4(SphericalCenter, 1.0);
	float l = length(dir);

//calculate texture coordinates
	v_texCoord3D = vec4(gl_TextureMatrix[0] * normalize(dir)).xyz;
	vec4 transVertex = gl_TextureMatrix[0]* gl_Vertex;
	gl_TexCoord[0] = vec4(-transVertex.x*0.5+0.5, -transVertex.y*0.5+0.5, 0.0, 1.0);
	gl_TexCoord[1] = dir;
	
	float h = texture2D(texture, gl_TexCoord[0].xy).z;
	vec4 newVertex = scaledVertex + (dir / l) * ((1.0 - l)+((h*2.0-1.0)/6378.0*6.0));
	gl_Position    = gl_ModelViewProjectionMatrix * newVertex;

	gl_FrontColor  = gl_Color;
}
