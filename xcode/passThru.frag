#version 120
uniform sampler2D Tex0;

void main() {
    vec3 color = texture2D(Tex0, gl_TexCoord[0].st).rgb;
    gl_FragColor = vec4(color, rgb);
}