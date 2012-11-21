uniform float minSize;
uniform float maxDistance;
uniform float minDistance;

void main() {
	gl_TexCoord[0] = gl_MultiTexCoord0;
	vec4 eyeCoord = gl_ModelViewMatrix * gl_Vertex;
	gl_Position = gl_ProjectionMatrix * eyeCoord;
	float dist  = sqrt(eyeCoord.x*eyeCoord.x + eyeCoord.y*eyeCoord.y + eyeCoord.z*eyeCoord.z);
	float att	= 600. / dist;
	float nearMax = 25.0;
	float nearAttenuate = min(dist, nearMax)/nearMax;
	gl_PointSize = minSize * gl_Normal.x * att * nearAttenuate;
	gl_FrontColor = gl_Color*pow(nearAttenuate,1.0);
	
	//gl_PointSize = 100.0;
//	gl_FrontColor = vec4(1.0, 1.0, 1.0, 1.0);

}





