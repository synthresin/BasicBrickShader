#version 120

varying float   LightIntensity;
uniform sampler2D EarthTexture;

void main() {
    
    vec3 color = vec3(texture2D(EarthTexture, gl_TexCoord[0].st));
    gl_FragColor = vec4( color * LightIntensity, 1.0);
    
    
    
//    color = vec3(0.6,0.9,0.3);
//    color *= LightIntensity;
//    gl_FragColor = vec4(color,1.0);
    
    
}