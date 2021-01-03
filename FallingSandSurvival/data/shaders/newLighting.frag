// Fragment
#ifdef GL_ES
precision mediump float;
#endif

uniform bool simpleOnly = false;
uniform bool emission = true;
uniform bool dithering = false;
uniform float lightingQuality = 0.5;
uniform float inside = 0.0;

uniform float minX = 0.0;
uniform float minY = 0.0;
uniform float maxX = 0.0;
uniform float maxY = 0.0;


uniform sampler2D txrmap;   // texture unit for light map
uniform sampler2D emitmap;
uniform vec2 texSize;
varying vec2 texCoord;
uniform vec2 t0;

float light(vec4 col){
    return 1.0 - col.a;
}

vec4 light2(vec2 coord){
    vec4 col = texture2D(txrmap, coord);
    return vec4(1.0 - vec3(col.a), 1.0);
}

vec4 lightEmit(vec2 coord){
    if(!emission) return vec4(0.0);
    vec4 emit = texture2D(emitmap, coord);
    return emit * emit.a;
}

float brightnessContrast(float value, float brightness, float contrast){
    return (value - 0.5) * contrast + 0.5 + brightness;
}

vec4 brightnessContrast2(vec4 value, float brightness, float contrast){
    return vec4((value.rgb - vec3(0.5)) * contrast + 0.5 + brightness, 1.0);
}

const float DITHER_NOISE = 0.004;

// based on https://shader-tutorial.dev/advanced/color-banding-dithering/ (MIT license)
float random(vec2 coords) {
   return fract(sin(dot(coords.xy, vec2(12.9898,78.233))) * 43758.5453);
}

void main(){
    if(texCoord.x < (minX / texSize.x) || texCoord.x > (maxX / texSize.x) || texCoord.y < (minY / texSize.y) || texCoord.y > (maxY / texSize.y)){
        // only basic lighting outside visible area
        float dst2 = distance(texCoord * texSize * vec2(0.75, 1.0), t0 * texSize * vec2(0.75, 1.0)) / 3000.0;
        
        float dark = clamp(1.0 - dst2 * 3.5 * inside, 0.0, 1.0);
        // gl_FragColor = vec4(0.0, 0.0, 0.0, 1.0 - dark);
        gl_FragColor = vec4(vec3(dark), 1.0);
    }else{
        float dst = distance(texCoord * texSize * vec2(0.75, 1.0), t0 * texSize * vec2(0.75, 1.0)) / 3000.0;
        float distBr = 1.0 - dst * 3.5 * inside;
        if(distBr <= -1.0){
            gl_FragColor = vec4(0.5, 0.0, 0.0, 1.0);
        }else{
            distBr = clamp(distBr, 0.0, 1.0);
            vec4 olcol = texture2D(txrmap, texCoord);
            float distNr = 1.0 - clamp(dst * 10.0, 0.0, 1.0);
            distNr *= 1.0;
            if(!simpleOnly && olcol.a > 0){
                // actual tile
            
                vec4 lcol = light2(texCoord);
                vec4 ecol = lightEmit(texCoord);
                vec4 emitCol = vec4(0.0);
                
                float nDirs = 10.0 + int(16.0 * lightingQuality);
                float nSteps = 5.0 + int(10.0 * lightingQuality);
                vec2 rad = 64.0 / texSize.xy;
                
                for(float deg = 0.0; deg < 3.1415927 * 2; deg += (3.1415927 * 2) / nDirs){
                    for(float i = 1.0 / nSteps; i <= 1.0; i += 1.0 / nSteps){
                        lcol += light2(texCoord + vec2(cos(deg), sin(deg)) * rad * i);	
                        if(emission) ecol += lightEmit(texCoord + vec2(cos(deg), sin(deg)) * rad * i);	
                    }
                }
                
                lcol.rgb /= nSteps * nDirs - 15.0;
                if(emission) ecol.rgb /= nSteps * nDirs - 15.0;
                
                vec4 c = brightnessContrast2(lcol, 0.2, 0.9) * distBr;
                if(dithering){
                    c += vec4(mix(-DITHER_NOISE * 5, DITHER_NOISE * 5, random(gl_FragCoord.xy / texSize))) * c * (1.0 - lightingQuality * 0.8);
                }
                vec4 brr = clamp(c, 0.0, 1.0);
                brr = mix(brr, vec4(1.0), distNr);
                
                if(emission) brr += pow(ecol, vec4(0.6));
                //gl_FragColor = mix(vec4(vec3(0.0), 1.0), olcol, brr); // mix orig color
                gl_FragColor = brr; // b/w
                //gl_FragColor = vec4(vec3(0.0), 1.0 - brr); // transparent black
            }else{
                // no tile (background)
            
                //float c = clamp(mix(distBr, 1.0, distNr), 0.0, 1.0);
                float c = clamp(distBr, 0.0, 1.0);
                if(dithering){
                    c += mix(-DITHER_NOISE, DITHER_NOISE, random(gl_FragCoord.xy / texSize));
                }
                //c = floor(c * 100.0) / 100.0;
                vec4 col = vec4(vec3(c), 1.0);
            
                if(!simpleOnly && emission){
                    vec4 ecol = lightEmit(texCoord);
                    vec4 emitCol = vec4(0.0);
                    
                    float nDirs = 10.0 + int(16.0 * lightingQuality);
                    float nSteps = 5.0 + int(10.0 * lightingQuality);
                    vec2 rad = 64.0 / texSize.xy;
                    
                    for(float deg = 0.0; deg < 3.1415927 * 2; deg += (3.1415927 * 2) / nDirs){
                        for(float i = 1.0 / nSteps; i <= 1.0; i += 1.0 / nSteps){
                            if(emission) ecol += lightEmit(texCoord + vec2(cos(deg), sin(deg)) * rad * i);	
                        }
                    }
                    ecol.rgb /= nSteps * nDirs - 15.0;
                    
                    col += pow(ecol, vec4(0.6));
                }
            
                //gl_FragColor = vec4(0.0, 0.0, 0.0, 1.0 - clamp(mix(distBr, 1.0, distNr), 0.0, 1.0));
                gl_FragColor = col;
                //gl_FragColor = vec4(vec3(0.0), 1.0 - clamp(mix(distBr, 1.0, distNr), 0.0, 1.0));
            }
        }
    }
    gl_FragColor.a = 1.0;
}

