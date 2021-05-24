varying vec2 tcoord;
uniform sampler2D tex;
uniform vec2 texelsize;

uniform vec2 pos;

void main(void) 
{
	//vec4 col = vec4(0.0, 0.0, 0.0, 1.0);
	vec4 col = texture2D(tex, tcoord);

	vec2 diff = tcoord/texelsize-pos;
	float norm2 = dot(diff,diff);
	if (norm2<=944784.0) col = vec4(1.0, 1.0, 1.0, 1.0);  //944784.0
 
	gl_FragColor = clamp(col,vec4(0),vec4(1));
	
}
