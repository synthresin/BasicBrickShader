#version 120

uniform vec3 BrickColor, MortarColor;
uniform vec2 BrickSize;
uniform vec2 BrickPct;
varying float   LightIntensity;
varying vec2    MCposition;

void main() {
    
    vec3 color;
    vec2 position, useBrick;
    
    /* 
    각 프래그먼트의 모델링좌표상의 x 를 벽돌의 가로 사이즈로,
    각 프래그먼트의 모델링좌표상의 y 를 벽돌의 세로 사이즈로 나눈다.
    이 결과는, 벽돌의 열 순번 ( position.x )와 벽돌의 행 순번 ( position.y )을 반환한다.
    */
    position = MCposition / BrickSize;
    
    /*
    벽돌의 행 순번에 반을 곱해서 그거의 분수부분이 0.5 보다 크다면, 벽돌 가로를 반칸 오른쪽으로 옮긴다. (even/odd row determination.)
    */
    if ( fract( position.y * 0.5 ) > 0.5 ) {
        position.x += 0.5;
    }
    
    /*
    하나의 논리적인 벽돌안에서의 프래그먼트의 위치를 알 수 있다. 
    */
    position = fract( position );
    
    /*
    브릭 퍼센티지 보다 높으면 모르타르가, 그렇지 않으면 브릭이 쓰인다.
    */
    useBrick = step(position, BrickPct);
    
    color = mix(MortarColor, BrickColor, useBrick.x * useBrick.y);
    color *= LightIntensity;
    gl_FragColor = vec4(color,1.0);
    
    
    
//    color = vec3(0.6,0.9,0.3);
//    color *= LightIntensity;
//    gl_FragColor = vec4(color,1.0);
    
    
}