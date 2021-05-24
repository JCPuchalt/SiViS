varying vec2 tcoord;

uniform sampler2D tex_im;
uniform vec2 texelsize_im; 

uniform sampler2D tex_pant;

uniform float fondo_ref;
uniform vec4 color_base;


void main(void)
{
	vec4 col = vec4(0.0, 0.0, 0.0, 1.0);
	vec4 gris_im      = texture2D(tex_im, tcoord);
	vec4 col_pantalla = texture2D(tex_pant, tcoord);
	

	vec2 diff = tcoord/texelsize_im-vec2(972.0, 972.0);
	float norm2 = dot(diff,diff);
	if (norm2<=944784.0)
	{
		float error = 0.12 - gris_im.r;
		float corregit = 0.3*error+col_pantalla.r;  //fondo_ref
		if(error<0.0) col = vec4(corregit, abs(error), 0.0, 1.0);
		if(error>0.0) col = vec4(corregit, 0.0, abs(error), 1.0);
	}

	gl_FragColor = clamp(col,vec4(0),vec4(1));
	
}
