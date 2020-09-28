#ifdef GL_ES
precision mediump float;
#endif

#extension GL_OES_standard_derivatives : enable

uniform sampler2D tex;
varying vec2 texCoord;

uniform float time;
uniform vec2 resolution;

uniform sampler2D mask;
uniform vec2 maskPos;
uniform vec2 maskSize;

uniform float scale;

float rand(vec2 coord){
    return fract(sin(dot(coord, vec2(12.9898, 78.233)))* 43758.5453123);
}

float noise(vec2 coord){
	vec2 i = floor(coord);
	vec2 f = fract(coord);

	// 4 corners of a rectangle surrounding our point
	float a = rand(i);
	float b = rand(i + vec2(1.0, 0.0));
	float c = rand(i + vec2(0.0, 1.0));
	float d = rand(i + vec2(1.0, 1.0));

	vec2 cubic = f * f * (3.0 - 2.0 * f);

	return mix(a, b, cubic.x) + (c - a) * cubic.y * (1.0 - cubic.x) + (d - b) * cubic.x * cubic.y;
}

void main( void ) {

    float scX = 1.0 / (resolution.x / scale) * scale;
    float scY = 1.0 / (resolution.y / scale) * scale;
    vec2 sc = vec2(scX, scY);
    
	vec2 position = ( gl_FragCoord.xy / resolution.xy );
    //position = floor(position / (sc / scale)) * (sc / scale);

    vec2 noisecoord1 = position.xy * 100000.0 / resolution.x;
	vec2 noisecoord2 = position.xy * 100000.0 / resolution.y + 4.0;
	
	vec2 motion1 = vec2(time * 0.3, time * -0.4);
	vec2 motion2 = vec2(time * 0.1, time * 0.5);
	
	vec2 distort1 = vec2(noise(noisecoord1 + motion1), noise(noisecoord2 + motion1)) - vec2(0.5);
	vec2 distort2 = vec2(noise(noisecoord1 + motion2), noise(noisecoord2 + motion2)) - vec2(0.5);
	
	vec2 distort_sum = (distort1 + distort2) / 240.0;
    vec2 dstPlace = texCoord;
    //dstPlace = floor(dstPlace / sc) * sc;
    vec2 dp = dstPlace + distort_sum;
    //vec2 dp = dstPlace + vec2(0, sin((time + (position.x*resolution.x)/10.0)/10.0)/5.0);
    
    vec2 worldPos = (gl_FragCoord.xy - vec2(maskPos.x, maskPos.y)) / maskSize;
    
    if(worldPos.x > 0.0 && worldPos.y < 0.0 && worldPos.x < 1.0 && worldPos.y > -1.0){
        vec4 worldCol = texture2D(mask, vec2(worldPos.x, -worldPos.y));
        if(worldCol.a > 0){
            gl_FragColor = texture2D(tex, dp);
        }else{
            gl_FragColor = texture2D(tex, texCoord);
        }
    }else{
        gl_FragColor = texture2D(tex, texCoord);
    }
    
    //gl_FragColor = vec4(1, 0, 0, 1);
    
}