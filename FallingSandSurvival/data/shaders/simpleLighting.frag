// Fragment
precision mediump float;

float transmit=0.98;// light transmition coeficient <0,1>
const int txrsiz=1200;     // max texture size [pixels]
uniform sampler2D txrmap;   // texture unit for light map
uniform vec2 texSize;
uniform vec2 lightPoints[10];
uniform int nLights = 0;
varying vec2 texCoord;

vec3 rgb2hsv(vec3 c){
    vec4 K = vec4(0.0, -1.0 / 3.0, 2.0 / 3.0, -1.0);
    vec4 p = mix(vec4(c.bg, K.wz), vec4(c.gb, K.xy), step(c.b, c.g));
    vec4 q = mix(vec4(p.xyw, c.r), vec4(c.r, p.yzx), step(p.x, c.r));

    float d = q.x - min(q.w, q.y);
    float e = 1.0e-10;
    return vec3(abs(q.z + (q.w - q.y) / (6.0 * d + e)), d / (q.x + e), q.x);
}

void main(){
    vec4 col = texture2D(txrmap, texCoord);
    //if(col.a == 0){
    //    gl_FragColor = vec4(1, 1, 1, 1);
    //}else{
    //    gl_FragColor = vec4(0, 0, 0, 1);
    //}
    
    float shadow = 1;
    for(int i = 0; i < nLights; i++){
        
        //float dit = distance(texCoord * texSize, lightPoints[i]);
        //if(dit < 80){
        //    float lig = (dit / 80.0);
        //    shadow = min(shadow, lig);
        //}
        
        vec2 fromPos = texCoord;
        vec2 toPos = lightPoints[i] / texSize;
        
        vec2 t, dt;
        float c0, c1;
        dt = normalize(fromPos - toPos) / float(txrsiz);
        c0 = 3.0; // light ray strength
        t = toPos;
        if (dot(fromPos - t, dt) > 0.0){
            for (int j = 0; j < txrsiz; j++){
                c1 = texture2D(txrmap, t).a;
                float aa = c1;
                if(aa > 0) {
                    c0 = 0;
                    break;
                }
                if(aa > 0.05) aa = 0.05;
                c0 *= (1 - aa) * 0.98;
                if (dot(fromPos - t, dt) <= 0.000) break;
                if (c0 <= 0.001) break;
                t += dt;
            }
        }
        
        if(c0 > 1) c0 = 1;
        shadow = min(shadow, 1 - c0);
        
    }
    
    /*
    if(col.a != 0){
        int r = 40;
        int wh = 0;
        int ct = 0;
        for(int xx = -r; xx <= r; xx++){
            for(int yy = -r; yy <= r; yy++){
                vec4 pxCol = texture2D(txrmap, texCoord + vec2(xx / texSize.x, yy / texSize.y));
                ct++;
                if(pxCol.a == 0){
                    wh++;
                }
            }
        }
        float val = float(wh) / ct;
        shadow = 1 - val;
    }else{
        shadow = 0;
    }
    */
    
    float r = (1 - shadow);
    float g = (1 - shadow);
    float b = (1 - shadow);
    float a = 1;
    gl_FragColor = vec4(r, g, b, a);
    
    //gl_FragColor = col;
}

