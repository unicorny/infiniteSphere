// simple fragment shader

// 'time' contains seconds since the program was linked.
uniform float time;
uniform sampler2D texture;

void main()
{
	gl_FragColor = gl_Color;
	gl_FragColor = tex2D(texture, gl_TexCoord[0].xy*0.5+0.5);
}
