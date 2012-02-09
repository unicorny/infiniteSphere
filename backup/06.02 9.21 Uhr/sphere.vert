uniform vec3 SphericalCenter;
uniform float theta;
uniform vec2 rotation;

uniform sampler2D texture;
varying vec3 v_texCoord3D;


vec3 sphereToKartesian(vec3 spherical)
{
	vec3 cartesian;

	//x = p cos (0) sin(oI)
	cartesian.x = spherical.x*cos(spherical.y)*sin(spherical.z);

	//z = p sin (0) sin(oI )
	cartesian.z = spherical.x*sin(spherical.y)*sin(spherical.z);

	//y = p cos (oI )
	cartesian.y = spherical.x*cos(spherical.z);

	return cartesian;
}

vec3 cartesianToSphere(vec3 cartesian)
{
	vec3 spherical;

	//p = Wurzel(x^2 + y^2 + z^2)
	spherical.x = length(cartesian);

	//O = tan^-1(z/x)
	spherical.y = atan(cartesian.z, cartesian.x);

	//oI = cos^-1(y/p)
	spherical.z = acos(cartesian.y/spherical.x);

	return spherical;
}

void main()
{
	vec4 scaledVertex = vec4(gl_Vertex.xyz*sqrt(1.0-cos(theta)), 1.0);
// make it spherical
	vec4 dir  = scaledVertex-vec4(SphericalCenter, 1.0);
	
	//vec3 spherical = cartesianToSphere(dir.xyz);
	//spherical -= vec3(0.0, rotation);
	//dir = vec4(sphereToKartesian(spherical), dir.w);
	
	float l = length(dir);
		
//calculate texture coordinates
	v_texCoord3D = vec4(gl_TextureMatrix[0] * normalize(dir)).xyz;
	vec4 transVertex = gl_TextureMatrix[0]* (vec4(gl_Vertex.xy, 0.0, 1.0));
	gl_TexCoord[0] = vec4(-transVertex.x*0.5+0.5, -transVertex.y*0.5+0.5, 0.0, 1.0);
	gl_TexCoord[1] = dir;
	
	float h = texture2D(texture, gl_TexCoord[0].xy).z;
	vec4 newVertex = scaledVertex + (dir / l) * ((1.0 - l)+((h*2.0-1.0)/6378.0*6.0));
		
	gl_Position    = gl_ModelViewProjectionMatrix * newVertex;

	gl_FrontColor  = gl_Color;
}
