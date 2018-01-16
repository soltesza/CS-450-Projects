#version 330 compatibility

in vec3 vPosition;		// vertex position

uniform float uMaxY;	// maximum y position
uniform float uContourSpacing = 15.0;
uniform float uLineThickness = 1.0;


vec3 colors[5] = vec3[](
	vec3(1., 0., 0.),
	vec3(1., 1., 0.),
	vec3(0., 1., 0.),
	vec3(0., 1., 1.),
	vec3(0., 0., 1.)
);

void
main( )
{
	// draw colored bands
	float posNormalized = vPosition.y / uMaxY;  // Y position as a value between [0, 1]
	vec3 color1 = colors[int(posNormalized * (colors.length() - 1))]; 				// get color from array based on height
	vec3 color2 = colors[1 + int(posNormalized * (colors.length() - 1))];			// get next color from array
	vec3 myColor = mix(color1, color2, fract(posNormalized * (colors.length() - 1)));		// blend two colors

	// draw contour lines
	float f  = fract(vPosition.y * uContourSpacing);
	float df = fwidth(vPosition.y * uContourSpacing);
	float g = smoothstep(df, df * uLineThickness, f);
	vec3 contour = vec3(g, g, g);
	
	//gl_FragColor = vec4(myColor,  1. );
	gl_FragColor = vec4(myColor * contour,  1. );
}
