attribute vec4 vertex;
uniform vec2 offset;
uniform vec2 scale;
varying vec2 tcoord;

uniform vec2 texelsize_im;
uniform vec2 texelsize_llum;


mat3 H = mat3(vec3(  0.17674,   0.00103, 0.00000),
              vec3(  0.00036,  -0.17580, 0.00000),
	      vec3( 67.51878, 388.65081,   1.0)  );
	  	
   	   
/*
mat3 H = mat3(vec3(  0.17674707419906441,   0.0010339296528549668, 0.00000276972673978386),
              vec3(  0.00036932829405254001, -0.17580414261702260, 0.00000030826302805233484),
	      vec3( 67.518783706291842,        388.65081552075532,       1.0)  );
*/

/*
mat3 H = mat3(vec3(  0.1761,    0.0006,  0.0000),
              vec3(  0.0008,   -0.1749,  0.0000),
	      vec3( 66.6345,  389.8664,  1.0)  );

mat3 H = mat3(vec3(  0.1761301683425,    0.0006201940930,  0.0000017226125),
              vec3(  0.0008251333594,   -0.1749109431131,  0.0000026510845),
	      vec3( 66.6345630048912,  389.8664084929169,  1.0)  );
*/



void main(void) 
{
	vec4 pos = vertex;
	tcoord.xy = pos.xy;


	pos.xyz = H*vec3(pos.x/texelsize_im.x, pos.y/texelsize_im.y, 1.0);
	pos.xy  = vec2((pos.x*texelsize_llum.x/pos.z) , (pos.y*texelsize_llum.y/pos.z))*scale +offset; 


	gl_Position = pos;

}




/*
	if( pos.x==0.0 && pos.y==1.0 )
		pos.xy = vec2 (60.2311*texelsize_llum.x,  67.6716*texelsize_llum.y )*scale +offset;
	if( pos.x==1.0 && pos.y==1.0 )
		pos.xy = vec2 (396.7375*texelsize_llum.x,  56.2020*texelsize_llum.y )*scale +offset;
	if( pos.x==0.0 && pos.y==0.0 )
		pos.xy = vec2 (76.9495*texelsize_llum.x, 411.5652*texelsize_llum.y )*scale +offset;
	if( pos.x==1.0 && pos.y==0.0 )
		pos.xy = vec2 (413.4559*texelsize_llum.x, 400.0956*texelsize_llum.y )*scale +offset;

*/