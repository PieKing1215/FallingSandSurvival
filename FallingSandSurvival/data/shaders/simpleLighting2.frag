// Fragment
precision mediump float;

float transmit=0.98;// light transmition coeficient <0,1>
const int txrsiz=1200;     // max texture size [pixels]
uniform sampler2D txrmap;   // texture unit for light map
uniform sampler2D lightmap;   // texture unit for light map
uniform vec2 texSize;
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
    vec4 lcol = texture2D(lightmap, texCoord);
    
    float shadow = 1.0;
    
    if(lcol.r == 0){
        int r = 40;
        float wh = 0;
        int ct = -1;
        for(int xx = -r; xx <= r; xx++){
            for(int yy = -r; yy <= r; yy++){
                vec4 pxCol = texture2D(lightmap, texCoord + vec2(xx / texSize.x, yy / texSize.y));
                ct++;
                if(pxCol.r > 0){
                    wh += pxCol.r;
                }
            }
        }
        float val = float(wh) / ct;
        shadow = 1 - val;
    }else{
        float val = lcol.r;
        shadow = 1 - val;
    }
    
    vec4 col = texture2D(txrmap, texCoord);
    //gl_FragColor = col;
    //shadow = 0;
    float r = col.r * (1 - shadow);
    float g = col.g * (1 - shadow);
    float b = col.b * (1 - shadow);
    float a = col.a * (1 - shadow) + (1 * shadow);
    gl_FragColor = vec4(r, g, b, a);
    
    //gl_FragColor = col;
}

