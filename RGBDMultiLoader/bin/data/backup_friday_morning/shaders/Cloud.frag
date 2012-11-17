uniform sampler2DRect tex0;
varying float colorOnly;

void main() {
	gl_FragColor = gl_Color * ((colorOnly  > .5) ? vec4(gl_Color.a) : texture2DRect(tex0, gl_TexCoord[0].st));

	//gl_FragColor = vec4(1.0,0.0,0.0,1.0);
}
