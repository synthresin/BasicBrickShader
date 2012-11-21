#version 120
uniform sampler2D tex0;

void main() {
    vec3 color = texture2D(tex0, gl_TexCoord[0].st).xyz;
    gl_FragColor = vec4(color, 1.0);
}