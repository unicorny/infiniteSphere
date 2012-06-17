// simple fragment shader

// 'time' contains seconds since the program was linked.
uniform float time;

uniform sampler2D tex1;
uniform sampler2D tex2;
uniform sampler2D tex3;

void main()
{
	vec4 color = tex2D(tex1, gl_TexCoord[0].xy);
	if(color.xyz == vec3(0.0,0.0,0.0))
		color += tex2D(tex2, gl_TexCoord[0].xy);
	if(color.xyz == vec3(0.0,0.0,0.0))
		color += tex2D(tex3, gl_TexCoord[0].xy);
	gl_FragColor = color;
}
