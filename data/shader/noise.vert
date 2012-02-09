<<<<<<< HEAD

varying vec3 v_texCoord3D;
varying vec2 v_texCoord2D;
varying vec4 c_color;

uniform vec3 SphericalCenter;
uniform float theta;



// simple vertex shader

void main()
{
	vec4 scaledVertex = vec4(gl_Vertex.xyz*sqrt(1.0-cos(theta)), 1.0);
// make it spherical
	vec4 dir = scaledVertex-vec4(SphericalCenter, 1.0);
	float l = length(dir);
    vec4 newVertex = scaledVertex + (dir / l) * (1.0 - l);
	gl_Position    = gl_ModelViewProjectionMatrix * gl_Vertex;

//calculate texture coordinates
	v_texCoord3D = gl_TextureMatrix[0] * (dir);
	
	gl_FrontColor  = gl_Color;	
}
=======

varying vec3 v_texCoord3D;
varying vec2 v_texCoord2D;
varying vec4 c_color;

uniform vec3 SphericalCenter;
uniform float theta;
const float PI = 3.1415926535;


// simple vertex shader

void main()
{
	gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex;
	float lon =  -PI      + (gl_Vertex.x*0.5+0.5)*(2.0*PI);
	float lat = (-PI*0.5) + (gl_Vertex.y*0.5+0.5)*     PI ;
    float r = cos(lat);
	//if(lon >= PI) lon -= 0.0001;
	//if(lat >= PI) lon -= 0.0001;
	v_texCoord3D = vec3(r*cos(lon), sin(lat), r*sin(lon));
	
	gl_FrontColor  = gl_Color;	
}
>>>>>>> 8d2f560ed5612c81fd65ab7efae5c3ce9ace1f37
