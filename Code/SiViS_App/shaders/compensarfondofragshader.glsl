varying vec2 tcoord;

uniform sampler2D tex_im;
uniform vec2 texelsize_im; 

uniform sampler2D tex_pant;

uniform float fondo_ref;
uniform vec4 color_base;


void main(void) 
{
	vec4 col = vec4(color_base.r, color_base.g, 0.0, 1.0 );
	vec4 gris_im = texture2D(tex_im, tcoord);
	vec4 col_pantalla = texture2D(tex_pant, tcoord);


	float color_compensado = 6.0*( 0.3 - gris_im.r); // + col_pantalla.b //fondo_ref
	col = vec4(color_base.r, color_base.g, color_compensado, 1.0);


	gl_FragColor = clamp(col,vec4(0),vec4(1));
	
}
