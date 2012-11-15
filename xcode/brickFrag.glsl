#version 120

varying float       LightIntensity;
uniform sampler2D   tex;

void main() {
    
    vec3 color;

    
    color = texture2D(tex, TexCoord).rgb;
    color *= LightIntensity;
    gl_FragColor = vec4(color,1.0);
    
    
}