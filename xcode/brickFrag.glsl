#version 120

varying float       LightIntensity;
uniform sampler2D   tex;
uniform sampler2D   tex2;
varying vec2        TexCoord;

void main() {
    
    vec3 color = texture2D(tex, TexCoord).rgb;
    gl_FragColor = vec4(color,1.0);
    
    vec3 color2 = texture2D(tex2, TexCoord).rgb;
    //color *= LightIntensity;
    
    vec3 finalColor = mix(color, color2, LightIntensity);
    gl_FragColor = vec4(finalColor,1.0);
    
    vec4 colorA = texture2D(tex, TexCoord);
    vec4 color2A = texture2D(tex2, TexCoord);
    
    vec4 finalColorA = mix(colorA, color2A, LightIntensity);
    gl_FragColor = vec4(finalColorA);
    gl_FragColor = vec4(colorA);
    
    
}