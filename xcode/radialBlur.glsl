#version 120

uniform vec2 resolution;
uniform float time;
uniform sampler2D tex0;


void main() {
    //vec3 color = texture2D(tex0, gl_TexCoord[0].st).xyz;
    
    
    vec2 p =  -1.0 + 2.0 * (gl_FragCoord.xy / resolution.xy);
    
    
    //vec3 color = texture2D( tex0, vec2(p.x, p.y) ).xyz;
    //vec3 color = texture2D(tex0, gl_TexCoord[0].st).xyz;
    
    vec3 color = vec3(0.0, 0.0, 0.0);
    // 오른쪽 위
    if (p.x > 0.0 && p.y > 0.0) {
        color = texture2D(tex0, gl_TexCoord[0].st).xyz;
    } else if (p.x > 0.0 && p.y < 0.0) { // 오른쪽 아래
        color = texture2D(tex0, gl_TexCoord[0].st + vec2(0.0, 0.5)).xyz;
    } else if (p.x < 0.0 && p.y > 0.0) { // 왼쪽 위
        color = texture2D(tex0, gl_TexCoord[0].st + vec2(0.5, 0.0)).xyz;
    } else if (p.x < 0.0 && p.y < 0.0) { // 왼쪽 아래
        color = texture2D(tex0, gl_TexCoord[0].st + vec2(0.5, 0.5)).xyz;
    }

    gl_FragColor = vec4(color, 1.0);
}