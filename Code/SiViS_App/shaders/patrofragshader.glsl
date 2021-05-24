varying vec2 tcoord;
uniform sampler2D tex;
uniform vec2 texelsize;
uniform vec2 incr;
uniform vec4 rgb;

void main(void) 
{
	vec4 col = vec4 (0.0, 0.0, 0.0, 1.0);
	for(float i = 1.0; i <=7.0; i++)
	{
		for(float j = 1.0; j <=7.0; j++)
		{
			if (			//(j==1.0 && i==2.0) || (j==1.0 && i==4.0) || (j==1.0 && i==6.0) ||
			    (j==2.0 && i==1.0) || (j==2.0 && i==3.0) || (j==2.0 && i==5.0) ||
			    (j==3.0 && i==2.0) || (j==3.0 && i==4.0) || (j==3.0 && i==6.0) ||
			    (j==4.0 && i==1.0) || (j==4.0 && i==3.0) || (j==4.0 && i==5.0) ||
			    (j==5.0 && i==2.0) || (j==5.0 && i==4.0) || (j==5.0 && i==6.0) ||
			    (j==6.0 && i==1.0) || (j==6.0 && i==3.0) || (j==6.0 && i==5.0) ||
			    (j==7.0 && i==2.0) || (j==7.0 && i==4.0) || (j==7.0 && i==6.0) ||
			    (j==2.0 && i==7.0) || (j==4.0 && i==7.0) || (j==6.0 && i==7.0)
					   ) 
						 //||(j==2.0 && i==7.0) || (j==4.0 && i==7.0)
			{
				vec2 centre = vec2(42.0*1.07597765363*i+60.0, 42.0*j+50.0); //+46.8995 //+15.0  // +incr
				vec2 diff = tcoord/texelsize-centre;
				float norm2 = dot(diff,diff);
				if (norm2<=100.0) col = vec4(1.0, 1.0, 1.0, 1.0);
			}
		}
	}
	gl_FragColor = clamp(col,vec4(0),vec4(1));
	
}

/*

*/

/*
	vec4 col = vec4(0.0, 0.0, 0.0, 1.0);

	vec2 centre = vec2(215.19553, 240.0);
	vec2 diff = tcoord/texelsize-centre;
	float norm2 = dot(diff,diff);
	if (norm2<=40000.0) col = vec4(rgb.r, rgb.g, rgb.b, 1.0); //56186.0
	if(tcoord.x/texelsize.x>=215.19553 && tcoord.x/texelsize.x<=216.19553) col = vec4(0.0, 1.0, 0.0, 1.0);
	if(tcoord.y/texelsize.y>=240.0 && tcoord.y/texelsize.y<=241.0) col = vec4(0.0, 1.0, 0.0, 1.0);


	gl_FragColor = clamp(col,vec4(0),vec4(1));

*/