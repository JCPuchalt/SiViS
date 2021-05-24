varying vec2 tcoord;
uniform sampler2D tex;
uniform vec2 texelsize;

uniform vec2 pos;

void main(void) 
{
	vec4 col = texture2D(tex, tcoord);

	vec2 diff = tcoord/texelsize-pos;
	float norm2 = dot(diff,diff);
	if (norm2<=50.0) col = vec4(1.0, 1.0, 1.0, 1.0);

	gl_FragColor = clamp(col,vec4(0),vec4(1));
}

/*
		if ( tcoord.x/texelsize.x>(pos.x-1.0) && tcoord.x/texelsize.x<(pos.x+1.0)
	  && tcoord.y/texelsize.y>(pos.y-1.0) && tcoord.y/texelsize.y<(pos.y+1.0)) 
		col = vec4(1.0, 1.0, 1.0, 1.0);
*/
