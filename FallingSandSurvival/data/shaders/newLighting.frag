// Fragment
precision mediump float;

uniform float lightingQuality = 0.5;
uniform float inside = 0.0;

uniform float minX = 0.0;
uniform float minY = 0.0;
uniform float maxX = 0.0;
uniform float maxY = 0.0;


uniform sampler2D txrmap;   // texture unit for light map
uniform vec2 texSize;
varying vec2 texCoord;
uniform vec2 t0;

float light(vec4 col){
    return 1.0 - col.a;
}

float brightnessContrast(float value, float brightness, float contrast){
    return (value - 0.5) * contrast + 0.5 + brightness;
}

void main(){
    if(texCoord.x < (minX / texSize.x) || texCoord.x > (maxX / texSize.x) || texCoord.y < (minY / texSize.y) || texCoord.y > (maxY / texSize.y)){
        // only basic lighting outside visible area
        float dst2 = distance(vec2(texCoord.x, texCoord.y), t0);
        
        float dark = clamp(1.0 - dst2 * 3.5 * inside, 0.0, 1.0);
        gl_FragColor = vec4(0.0, 0.0, 0.0, 1.0 - dark);
    }else{
        float dst = distance(texCoord, t0);
        float distBr = clamp(1.0 - dst * 3.5 * inside, 0.0, 1.0);
        if(distBr == 0.0){
            gl_FragColor = vec4(0.0, 0.0, 0.0, 1.0);
        }else{
            vec4 olcol = texture2D(txrmap, texCoord);
            float distNr = 1.0 - clamp(dst * 10.0, 0.0, 1.0);
            distNr *= 1.0;
            if(olcol.a > 0){
                // actual tile
            
                float lcol = light(texture2D(txrmap, texCoord));
                
                float nDirs = 10.0 + int(16.0 * lightingQuality);
                float nSteps = 5.0 + int(10.0 * lightingQuality);
                vec2 rad = 64.0 / texSize.xy;
                
                for(float deg = 0.0; deg < 3.1415927 * 2; deg += (3.1415927 * 2) / nDirs){
                    for(float i = 1.0 / nSteps; i <= 1.0; i += 1.0 / nSteps){
                        lcol += light(texture2D(txrmap, texCoord + vec2(cos(deg), sin(deg)) * rad * i));		
                    }
                }
                
                lcol /= nSteps * nDirs - 15.0;
                
                float brr = clamp(brightnessContrast(lcol, 0.2, 0.9) * distBr, 0.0, 1.0);
                brr = mix(brr, 1.0, distNr);
                //gl_FragColor = mix(vec4(vec3(0.0), 1.0), olcol, brr); // mix orig color
                //gl_FragColor = vec4(vec3(brr), 1.0); // b/w
                gl_FragColor = vec4(vec3(0.0), 1.0 - brr); // transparent black
            }else{
                // no tile (background)
            
                //gl_FragColor = vec4(0.0, 0.0, 0.0, 1.0 - clamp(mix(distBr, 1.0, distNr), 0.0, 1.0));
                //gl_FragColor = vec4(vec3(clamp(mix(distBr, 1.0, distNr), 0.0, 1.0)), 1.0);
                gl_FragColor = vec4(vec3(0.0), 1.0 - clamp(mix(distBr, 1.0, distNr), 0.0, 1.0));
            }
        }
    }
}

