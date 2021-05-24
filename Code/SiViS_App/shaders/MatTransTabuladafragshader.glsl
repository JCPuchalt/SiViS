varying vec2 tcoord;

uniform sampler2D tex_im;
uniform vec2 texelsize_im; 

uniform sampler2D tex_calibracion;

uniform vec2 texelsize_target;


void main(void)
{
	vec4 col = vec4(0.0, 0.0, 0.0, 1.0);
	vec4 col_image    = texture2D(tex_im, tcoord);
	vec4 coord_transf = texture2D(tex_calibracion, tcoord);
	vec4 col_pantalla = texture2D(tex_pant, tcoord);
	

	if (coord_transf.x!=0 && coord_transf.y!=0)
	{
		
	}

	gl_FragColor = clamp(col_pantalla,vec4(0),vec4(1));
	
}