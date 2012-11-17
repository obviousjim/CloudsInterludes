uniform sampler2DRect tex0;
varying float useTexture;

void main() {
	//gl_FragColor = gl_Color * ((useTexture == 1) ? texture2DRect(tex0, gl_TexCoord[0].st) : vec4(1.0));

	gl_FragColor = gl_Color * ((colorOnly  > .5) ? vec4(1.0)) : texture2DRect(tex0, gl_TexCoord[0].st);

}
