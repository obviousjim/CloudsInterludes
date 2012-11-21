uniform sampler2DRect tex0;


void main() {
	gl_FragColor = gl_Color*gl_Color.a;
	//gl_FragColor = vec4(1.0,0.0,0.0,1.0);
}
