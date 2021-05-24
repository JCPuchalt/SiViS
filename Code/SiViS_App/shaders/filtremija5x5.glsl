varying vec2 tcoord;
uniform sampler2D tex;
uniform vec2 texelsize;
uniform int AxA;

void main(void) 
{
	float i = 0.0;
	vec4 col = vec4(0.0, 0.0, 0.0, 1.0);
	
	for(int xoffset = -2; xoffset <= 2; xoffset++)
	{
		for(int yoffset = -2; yoffset <= 2; yoffset++)
		{
			vec2 offset = vec2(xoffset,yoffset);
			col.rgb += texture2D(tex,tcoord+offset*texelsize).rgb;
			i++;
		}
	}
	
	col.rgb = col.rgb/i;


    gl_FragColor = clamp(col,vec4(0),vec4(1));
}
