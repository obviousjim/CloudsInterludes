
uniform float focalDistance;
uniform float focalRange;
uniform float maxSize;
uniform float minSize;

const float PI = 3.14159265;
//const float maxSize = 32.;
varying float colorOnly;
void main() {
	//get the homogeneous 2d position
  	//gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex;
	// use the distance from the camera and aperture to determine the radius
	// the +1 is because point sizes <1 are rendered differently than those >1
  	//gl_PointSize = min(maxSize, abs(gl_Position.z - focalDistance) * aperture + 1.);
    
    //gl_Position = ftransform();
    //float scale = min(1.0, abs(gl_Position.z - focalDistance) / focalRange);
    //gl_PointSize = scale * (maxSize - minSize) + minSize;
	//gl_FrontColor = gl_Color;
    //gl_FrontColor.a = 1.0 - (gl_PointSize - minSize) / (maxSize - minSize);
	//gl_TexCoord[0] = gl_MultiTexCoord0;

	gl_Position = ftransform();
	gl_TexCoord[0] = gl_MultiTexCoord0;
	colorOnly = gl_Normal.x;
	gl_FrontColor = gl_Color;
	gl_PointSize = gl_Normal.y;
	
    //float scale = min(1.0, abs(gl_Position.z - focalDistance) / focalRange);
    //gl_PointSize = scale * (maxSize - minSize) + minSize;
	gl_FrontColor = gl_Color;
    //gl_FrontColor.a = 1.0 - (gl_PointSize - minSize) / (maxSize - minSize);
	//gl_TexCoord[0] = gl_MultiTexCoord0;

	//gl_FrontColor = texture2DRect(src_tex_unit0, gl_MultiTexCoord0.xy);
	// divide the color alpha by the area
  	//gl_FrontColor.a *= focalRange;
  	//gl_FrontColor.a /= PI * radius * radius;
    //gl_PointSize  = 4.0;
    //gl_FrontColor = vec4(1.0);
}
