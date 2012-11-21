uniform float focalPlane;
uniform float focalRange;
uniform float lineFade;

void main() {
	
	vec4 eyeCoord = gl_ModelViewMatrix * gl_Vertex;
	gl_Position = gl_ProjectionMatrix * eyeCoord;
	float dist = sqrt(eyeCoord.x*eyeCoord.x + eyeCoord.y*eyeCoord.y + eyeCoord.z*eyeCoord.z);
	float attenuation = 1.0 - min(abs(gl_Position.z - focalPlane)/focalRange, 1.0);
	
	gl_FrontColor = gl_Color * attenuation * lineFade;

}





