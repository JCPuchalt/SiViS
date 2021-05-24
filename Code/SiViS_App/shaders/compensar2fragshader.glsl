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
	//if (norm2<=944784.0)
	{
		float error = 0.9*(0.19 - gris_im.r);  //fondo_ref
	
	
		float col_global= col_pantalla.r + col_pantalla.g + col_pantalla.b + error;
			
		col = vec4(col_global, col_global-1.0, col_global-2.0, 1.0); //col_global, col_global-1.0
			
		if (col_global-2.0<0.0) col.b=0.0;
		if (col_global-1.0<0.0) col.g=0.0;
		if (col_global<0.0)     col.r=0.0;
		if (col_global-2.0>1.0) col.b=1.0;
		if (col_global-1.0>1.0) col.g=1.0;
		if (col_global>1.0)     col.r=1.0;
			
		if(tcoord.x==0.0 && tcoord.y==0.0) col.a=10.0;
	}

	gl_FragColor = clamp(col,vec4(0),vec4(1));
	
}