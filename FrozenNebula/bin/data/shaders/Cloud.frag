uniform sampler2DRect tex0;
void main() {
	gl_FragColor = gl_Color * texture2DRect(tex0, gl_TexCoord[0].st) * gl_Color.a;

	//gl_FragColor = vec4(gl_Color.a);
}
