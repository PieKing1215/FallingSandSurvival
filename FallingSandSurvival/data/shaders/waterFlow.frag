#ifdef GL_ES
precision mediump float;
#endif

#extension GL_OES_standard_derivatives : enable

uniform sampler2D tex;
varying vec2 texCoord;

uniform vec2 resolution;

void main( void ) {
    //if(mod(time, 0.1) > 0.05) discard;

    vec2 worldPos = gl_FragCoord.xy / resolution;
    
    vec4 worldCol = texture2D(tex, vec2(worldPos.x, worldPos.y));
    if(worldCol.a > 0){
        
        vec4 flowCol = texture2D(tex, worldPos);
        
        float range = 16.0;
        float numSamples = 1.0;
        for(float xx = -range/2; xx <= range/2; xx += 1.0){
            for(float yy = -range/2; yy <= range/2; yy += 1.0){
                vec4 flowColScan = texture2D(tex, vec2(worldPos.x + xx/resolution.x, worldPos.y + yy/resolution.y));
                
                // invalid
                if(flowColScan.r == 0.0 && flowColScan.g == 0.0) continue;
                if(flowColScan.a == 0.0) continue;
                
                flowColScan.a = 0.0;
                
                //vec2 flow = vec2(flowColScan.r, flowColScan.g) - vec2(0.5);

                //speed += length(flow.xy);
                //angle += degrees(atan2(flow.y, flow.x)) - 90.0;
                vec2 flow2 = vec2(flowColScan.r, flowColScan.g) - vec2(0.5);
                float weight = flow2.x * flow2.x + flow2.y * flow2.y;
                flowCol += flowColScan * weight;
                numSamples += weight;
            }
        }
    
        flowCol /= numSamples;
        
        //angle /= (range * 2) * (range * 2);
        //speed /= (range * 2) * (range * 2);
        
        //flowCol = vec4(0.5, 0.0, 0.0, 0.0);
        //flowCol += vec4(0.5);
        
        vec2 flow = vec2(flowCol.r, flowCol.g) - vec2(0.5);
        //flowCol -= vec4(0.5);
        
        flowCol.r = flow.r + 0.5;
        flowCol.g = flow.g + 0.5;
        
        gl_FragColor = flowCol;
        
    }else{
        gl_FragColor = vec4(0.0);
    }

    //gl_FragColor = vec4(1, 0, 0, 1);
    
}