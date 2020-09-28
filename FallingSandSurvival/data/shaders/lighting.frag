// Fragment
precision mediump float;

float transmit=0.995;// light transmition coeficient <0,1>
const int txrsiz=1200;     // max texture size [pixels]
uniform sampler2D txrmap;   // texture unit for light map
uniform vec2 t0;            // texture start point (mouse position) <0,1>
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
    vec2 t, dt;
    vec4 c0,c1;
    dt = normalize(texCoord - t0) / float(txrsiz);
    c0 = vec4(1.0, 1.0, 1.0, 1.0); // light ray strength
    t = t0;
    if (dot(texCoord - t, dt) > 0.0){
        for (int i = 0; i < txrsiz; i++){
            c1 = texture2D(txrmap, t);
            float aa = c1.a;
            if(aa > 0.05) aa = 0.05;
            c0.rgb *= ((aa) * (c1.rgb)) + ((1.0 - aa) * transmit);
            if (dot(texCoord - t, dt) <= 0.000) break;
            if (c0.r + c0.g + c0.b <= 0.001) break;
            t += dt;
        }
    }
    vec4 col = texture2D(txrmap, texCoord);
    if(col.a == 0.0){
        float alph = 1.0 - rgb2hsv(c0.rgb).b;
        gl_FragColor = vec4(0, 0, 0, alph);
    }else{
        gl_FragColor = 0.9 * (c0 * col) + 0.1 * col; // render with ambient light
    }
    //gl_FragColor = c0; // render without ambient light
    //gl_FragColor = texture2D(txrmap, texCoord + vec2(texCoord.y * 0.1, 0));
    //gl_FragColor = vec4(1, 1, 1, 1);
}

