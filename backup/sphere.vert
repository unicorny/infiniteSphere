uniform vec3 SphericalCenter;
uniform vec3 CameraIntersectionPoint;
uniform float sphereH;
uniform float theta;
uniform vec2 camUV; //[0;PI], [-PI/2;PI/2]
uniform vec3 sektorID;

varying vec3 v_texCoord3D;
varying vec2 v_texCoord2D;

const float PI = 3.14159265;
const float PIhalf = PI/2.0;

mat4 rotationAxis(vec3 v, float f)
{
    vec3 axis = v;
    if(length(v) != 1.0)
    {
        axis = normalize(v);
        //LOG_WARNING("parameter v isn't normalized!");
        //printf("\r [DRMatrix::rotationAxis] parameter v isn't normalized!");
    }
    float _cos = cos(f);
    float _sin = sin(f);
    float oneMinusCos = 1.0 - _cos;

    return mat4(axis.x*axis.x * oneMinusCos + _cos,
                axis.x*axis.y * oneMinusCos - axis.z * _sin,
                axis.x*axis.z * oneMinusCos + axis.y * _sin, 
                0.0,
                axis.y*axis.x * oneMinusCos + axis.z * _sin,
                axis.y*axis.y * oneMinusCos + _cos,
                axis.y*axis.z * oneMinusCos - axis.x * _sin,
                0.0,
                axis.x*axis.z * oneMinusCos - axis.y * _sin,
                axis.y*axis.z * oneMinusCos + axis.x * _sin,
                axis.z*axis.z * oneMinusCos + _cos,
                0.0,
                0.0, 0.0, 0.0, 1.0);
}

vec3 transformCoords(vec3 v, mat4 m)
{
    // Vektor mit Matrix multiplizieren
	vec3 vResult = vec3(v.x * m[0][0] + v.y * m[1][0] + v.z * m[2][0] + m[3][0],
						v.x * m[0][1] + v.y * m[1][1] + v.z * m[2][1] + m[3][1],
						v.x * m[0][2] + v.y * m[1][2] + v.z * m[2][2] + m[3][2]);

	// Vierte Koordinate (w) berechnen. Wenn diese ungleich eins
	// ist, müssen die anderen Vektorelemente durch sie geteilt
	// werden.
	float w = v.x * m[0][3] + v.y * m[1][3] + v.z * m[2][3] + m[3][3];
	if(w != 1.0) vResult /= w;

	return vResult;
}

void main()
{
		
	vec4 scaledVertex = vec4(gl_Vertex.xyz*sqrt(1.0-cos(theta)), 1.0);
// make it spherical
	vec4 dir = scaledVertex-vec4(SphericalCenter, 1.0);
	float l = length(dir);
    vec4 newVertex = scaledVertex + (dir / l) * (1.0 - l);
	gl_Position    = gl_ModelViewProjectionMatrix * newVertex;

//calculate texture coordinates

	vec4 thetaDir = dir;
//	vec3 v = normalize(thetaDir.xyz+CameraIntersectionPoint);
//	gl_TexCoord[0] = vec4(asin(v.xy)/PI+0.5, v.z, 1.0);	
	vec3 pos = thetaDir.xyz;
//	vec2 tx = vec2(atan(pos.x, pos.z)/ (2.0*PI)*0.5,
//  				   asin(pos.y)/PI+0.5);
//	vec3 cam = CameraIntersectionPoint;
//	if(cam.z < 0.0) cam = -CameraIntersectionPoint;
//	if(cam.x > 0.0) cam.z = -CameraIntersectionPoint.z;
//    vec2 uv = camUV;
//	uv.x /= (2.0*PI);
//	if(CameraIntersectionPoint.z < 0.0)
//		uv.x -= 0.5;

	vec2 tx = vec3((normalize(pos))).xy+camUV;//vec2(pos.x / length(pos),
  				//   pos.y / length(pos));
	
	gl_TexCoord[0] = vec4(tx.x, tx.y, 0.0, 1.0);
	gl_TexCoord[1] = gl_Vertex*(1.0-cos(theta))*0.5;

	//v_texCoord3D = vec3(tx, 99);
	
	if(gl_Vertex == vec4(0.0, 0.0, 0.0, 1.0))
	{
		v_texCoord3D = normalize(CameraIntersectionPoint);
	}
	else
	{
		float angle = acos(dot(normalize(thetaDir.xyz), vec3(0.0,0.0,1.0)));
		vec3 axis = cross((thetaDir.xyz), CameraIntersectionPoint);
		mat4 m = rotationAxis(axis, -angle);
//	v_texCoord3D = normalize(normalize(CameraIntersectionPoint) + normalize(gl_Vertex));
		v_texCoord3D = m * vec4(normalize(CameraIntersectionPoint).xyz, 1.0);
		//v_texCoord3D = transformCoords(normalize(CameraIntersectionPoint), m);
		v_texCoord3D = normalize(normalize(thetaDir) - vec3(0.0, 0.0, 1.0) + CameraIntersectionPoint);
	}
	v_texCoord3D = normalize(thetaDir) - vec3(0.0, 0.0, 1.0) + CameraIntersectionPoint;
	v_texCoord3D = gl_TextureMatrix[0] * normalize(gl_Vertex -(vec4(0.0, 0.0, 1.0, 1.0)));
	v_texCoord3D = gl_TextureMatrix[0] * normalize(thetaDir);
	
	vec4 vert = gl_TextureMatrix[0] * gl_Vertex*(1.0-cos(theta));
	if(sektorID.x) 
	{
		if(sektorID.x > 0.0)
			v_texCoord3D = vec3(sektorID.x, vert.y + CameraIntersectionPoint.y,
											vert.x - CameraIntersectionPoint.z);
		else
			v_texCoord3D = vec3(sektorID.x, vert.yx + CameraIntersectionPoint.yz);
	}
	else if(sektorID.y)
	{
		float f  = 1.0;
		if(CameraIntersectionPoint.z < 0.0)
			f = -1.0;
		if(sektorID.y < 0.0)
			v_texCoord3D = vec3(CameraIntersectionPoint.x - vert.x, sektorID.y, 
								CameraIntersectionPoint.z + f*vert.y);
		else
			v_texCoord3D = vec3(CameraIntersectionPoint.x + vert.x, sektorID.y, 
								CameraIntersectionPoint.z - f*vert.y);
		
	}
	else if(sektorID.z)
	{
		if(sektorID.z > 0.0)
			v_texCoord3D = vec3(CameraIntersectionPoint.xy + vert.xy, sektorID.z);
		else
			v_texCoord3D = vec3(CameraIntersectionPoint.x - vert.x,
								CameraIntersectionPoint.y + vert.y, sektorID.z);
			
		//if(CameraIntersectionPoint.x > 0.0 && sektorID.z < 0.0)
			//v_texCoord3D.y *= -1.0;
	}
	//v_texCoord3D = ci + gl_Vertex; 
	if(v_texCoord3D.x > 1.0 || v_texCoord3D.x < -1.0) 
		v_texCoord3D = vec3(0.0);
	if(v_texCoord3D.y > 1.0 || v_texCoord3D.y < -1.0) 
		v_texCoord3D = vec3(0.0);
	if(v_texCoord3D.z > 1.0 || v_texCoord3D.z < -1.0) 
		v_texCoord3D = vec3(0.0);
		
	v_texCoord3D = gl_TextureMatrix[0] * normalize(thetaDir);
//	*/	

//	CameraIntersectionPoint = normalize(CameraIntersectionPoint);
//	gl_Position    = gl_ModelViewProjectionMatrix * dir;
	gl_FrontColor  = gl_Color;
//    gl_FrontColor = gl_Vertex;
//	v_texCoord3D = normalize(dir.xyz+CameraIntersectionPoint);
//	v_texCoord3D = vec4(rotateMatrix * vec4(0.0, 0.0, 1.0, 1.0)).xyz;
//	v_texCoord3D = vec3(gl_Vertex.xy*theta, 17);
//	gl_TexCoord[0] = dir*theta;
}
