uniform sampler2D tex;

void main (void) {
	vec4 color = texture2D(tex, gl_TexCoord[0].st);
	gl_FragColor = texture2D(tex, gl_TexCoord[0].st) * gl_Color * color.a;
}
