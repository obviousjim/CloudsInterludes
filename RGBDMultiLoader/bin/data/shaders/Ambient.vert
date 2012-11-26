
uniform float maxSize;
uniform float maxDisance;

const float PI = 3.14159265;
//const float maxSize = 32.;

void main() {
	//get the homogeneous 2d position
  	//gl_Position = gl_ModelViewProjectionMatrix * (gl_Vertex + ;
	// use the distance from the camera and aperture to determine the radius
	// the +1 is because point sizes <1 are rendered differently than those >1
  	//gl_PointSize = min(maxSize, abs(gl_Position.z - focalDistance) * aperture + 1.);
    
    //gl_Position = ftransform();
    //float scale = min(1.0, abs(gl_Position.z - focalDistance) / focalRange);
    //gl_PointSize = scale * (maxSize - minSize) + minSize;
	//gl_FrontColor = gl_Color;
    //gl_FrontColor.a = 1.0 - (gl_PointSize - minSize) / (maxSize - minSize);
	//gl_TexCoord[0] = gl_MultiTexCoord0;

	gl_Position = gl_ModelViewProjectionMatrix * (gl_Vertex + vec4(-pow(gl_Color.r,1.5)*15., pow(gl_Color.r,1.5)*25.,pow(gl_Color.r,2.0)*15., 0.0) );

	//gl_Position = ftransform();
//	gl_Position.y += gl_Color.r*10.;
	gl_TexCoord[0] = gl_MultiTexCoord0;
	gl_FrontColor = gl_Color;
	float fade = max(maxDisance - gl_Position.z, 0.) / maxDisance;
	fade *= max(gl_Position.z - 100., 0.0) / 100.0;
	float maxSize = 17.0;
	float minSize = 0.0;
	
	//gl_PointSize = 3.0 + fade * max(gl_Color.r - .2, 0.0)*(maxSize-minSize);
	gl_PointSize = minSize + gl_Color.r*(maxSize-minSize ) * fade;
	//gl_PointSize = maxSize;
//	float shrinkFade = min(gl_PointSize, maxSize*.2) / maxSize*.2;
	float growthFade = 1. - pow(max(gl_PointSize, maxSize*.1) / maxSize*.1, 2.0);
	gl_FrontColor *= growthFade * fade;// * max(gl_Color.r-.5, 0.); // * max(1.-gl_PointSize/2.0, 0.0);
	
//	if(gl_PointSize > 2.){
//		gl_FrontColor.a *= max(gl_PointSize-1., 0.);
//	}
	
	//gl_PointSize = gl_Color.a*1.0;
    //float scale = min(1.0, abs(gl_Position.z - focalDistance) / focalRange);
    //gl_PointSize = scale * (maxSize - minSize) + minSize;
//	gl_FrontColor = gl_Color;
    //gl_FrontColor.a = 1.0 - (gl_PointSize - minSize) / (maxSize - minSize);
	//gl_TexCoord[0] = gl_MultiTexCoord0;

	//gl_FrontColor = texture2DRect(src_tex_unit0, gl_	MultiTexCoord0.xy);
	// divide the color alpha by the area
  	//gl_FrontColor.a *= focalRange;
  	//gl_FrontColor.a /= PI * radius * radius;
    //gl_PointSize  = 4.0;
    //gl_FrontColor = vec4(1.0);
}
