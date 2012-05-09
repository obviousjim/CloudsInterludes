uniform sampler2DRect tex0;
uniform int useTexture;
void main() {
	gl_FragColor = gl_Color * ((useTexture == 1) ? texture2DRect(tex0, gl_TexCoord[0].st) : vec4(1.0));
}
