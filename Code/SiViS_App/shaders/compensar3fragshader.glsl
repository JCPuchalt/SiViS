varying vec2 tcoord;

uniform sampler2D tex_im;
uniform vec2 texelsize_im; 

uniform sampler2D tex_pant;

uniform float fondo_ref;
uniform vec4 color_base;


void main(void) 
{
	vec4 col_pantalla = texture2D(tex_pant, tcoord);
	vec4 col = vec4(col_pantalla.r, col_pantalla.g, col_pantalla.b, 1.0 );  //color_base.g
	vec4 gris_im = texture2D(tex_im, tcoord);

	
	if( (0.3 - gris_im.r)<0.0 )
	{
		float color_compensado = 0.1*( 0.3 - gris_im.r) + col_pantalla.g;
		col = vec4(color_base.r, color_compensado, col_pantalla.b, 1.0);  //
	}
	
	if( (0.3 - gris_im.r)>0.0 )
	{
		float color_compensado = 0.9*( 0.3 - gris_im.r) + col_pantalla.b;
		col = vec4(color_base.r, col_pantalla.g, color_compensado, 1.0); //
	}

	gl_FragColor = clamp(col,vec4(0),vec4(1));
	
}

