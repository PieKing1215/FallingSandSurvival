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

uniform sampler2D flowTex;

uniform int overlay = 0;
uniform bool showFlow = true;
uniform bool pixelated = false;

//==================================================================
//https://github.com/stegu/webgl-noise/blob/master/src/classicnoise2D.glsl

//
// GLSL textureless classic 2D noise "cnoise",
// with an RSL-style periodic variant "pnoise".
// Author:  Stefan Gustavson (stefan.gustavson@liu.se)
// Version: 2011-08-22
//
// Many thanks to Ian McEwan of Ashima Arts for the
// ideas for permutation and gradient selection.
//
// Copyright (c) 2011 Stefan Gustavson. All rights reserved.
// Distributed under the MIT license. See LICENSE file.
// https://github.com/stegu/webgl-noise
//

vec4 mod289(vec4 x)
{
  return x - floor(x * (1.0 / 289.0)) * 289.0;
}

vec4 permute(vec4 x)
{
  return mod289(((x*34.0)+1.0)*x);
}

vec4 taylorInvSqrt(vec4 r)
{
  return 1.79284291400159 - 0.85373472095314 * r;
}

vec2 fade(vec2 t) {
  return t*t*t*(t*(t*6.0-15.0)+10.0);
}

// Classic Perlin noise
float cnoise(vec2 P)
{
  vec4 Pi = floor(P.xyxy) + vec4(0.0, 0.0, 1.0, 1.0);
  vec4 Pf = fract(P.xyxy) - vec4(0.0, 0.0, 1.0, 1.0);
  Pi = mod289(Pi); // To avoid truncation effects in permutation
  vec4 ix = Pi.xzxz;
  vec4 iy = Pi.yyww;
  vec4 fx = Pf.xzxz;
  vec4 fy = Pf.yyww;

  vec4 i = permute(permute(ix) + iy);

  vec4 gx = fract(i * (1.0 / 41.0)) * 2.0 - 1.0 ;
  vec4 gy = abs(gx) - 0.5 ;
  vec4 tx = floor(gx + 0.5);
  gx = gx - tx;

  vec2 g00 = vec2(gx.x,gy.x);
  vec2 g10 = vec2(gx.y,gy.y);
  vec2 g01 = vec2(gx.z,gy.z);
  vec2 g11 = vec2(gx.w,gy.w);

  vec4 norm = taylorInvSqrt(vec4(dot(g00, g00), dot(g01, g01), dot(g10, g10), dot(g11, g11)));
  g00 *= norm.x;  
  g01 *= norm.y;  
  g10 *= norm.z;  
  g11 *= norm.w;  

  float n00 = dot(g00, vec2(fx.x, fy.x));
  float n10 = dot(g10, vec2(fx.y, fy.y));
  float n01 = dot(g01, vec2(fx.z, fy.z));
  float n11 = dot(g11, vec2(fx.w, fy.w));

  vec2 fade_xy = fade(Pf.xy);
  vec2 n_x = mix(vec2(n00, n01), vec2(n10, n11), fade_xy.x);
  float n_xy = mix(n_x.x, n_x.y, fade_xy.y);
  return 2.3 * n_xy;
}
//==================================================================

void main( void ) {
    // this shader is applied to the background layer when it is drawn to the main buffer
    // gl_FragCoord is the pixel on screen
    // worldPos is the coordinate of the material in world space (from 0 to 1) (can be fractional since this shader is in screen space)
    // clworldPos (clipped worldPos) is worldPos but floored to a pixel material coordinate in world space (from 0 to 1)
    
    vec2 worldPos = (vec2(gl_FragCoord.x, (resolution.y / scale) - gl_FragCoord.y) - vec2(maskPos.x, maskPos.y)) / maskSize;
    
    // early exit if not in world
    if(worldPos.x < 0.0 || worldPos.y < 0.0 || worldPos.x >= 1.0 || worldPos.y >= 1.0){
        gl_FragColor = texture2D(tex, texCoord);
        return;
    }
    
    vec4 worldCol = texture2D(mask, vec2(worldPos.x, worldPos.y));
    
    // early exit if air
    if(worldCol.a == 0.0){
        gl_FragColor = texture2D(tex, texCoord);
        return;
    }
    
    // early exit if fully opaque
    if(worldCol.a > 0.99){
        return;
    }
    
    
    // apply the shader
    
    vec2 clworldPos = floor(worldPos * (maskSize/scale)) / (maskSize/scale);
    
    if(!pixelated) {
        clworldPos = worldPos;
    }
    
    // calculate flow
    
    vec4 flowCol = texture2D(flowTex, worldPos);
    
    float angle = 0;
    float speed = 0.0;

    if(showFlow){
        vec2 flow = vec2(flowCol.r, flowCol.g) - vec2(0.5);
        
        // clamp flow to only a few values so we get larger "chunks" with the same value
        // (this helps since the borders between values are not continuous)
        flow.r = int(flow.r * 10.0) / 10.0;
        flow.g = int(flow.g * 10.0) / 10.0;
        
        flowCol.r = flow.r + 0.5;
        flowCol.g = flow.g + 0.5;
        
        speed = length(flow.xy);
        angle = degrees(atan(flow.y, flow.x)) - 90.0;
    }
    
    // calculate positions for large and small layer
    vec2 position1 = vec2(clworldPos.x, -clworldPos.y);
    vec2 position2 = vec2(clworldPos.x, -clworldPos.y);
    
    position1 += time * 0.05 * speed * vec2(sin(radians(angle)), cos(radians(angle)));
    position2 += time * 0.1 * speed * vec2(sin(radians(angle)), cos(radians(angle)));

    // calculate noise position
    float noiseScale = pixelated ? 1.25 : 1.0;

    vec2 noisecoord1 = position1.xy * 300000.0 / noiseScale / resolution.x;
    vec2 noisecoord2 = position2.xy * 450000.0 / noiseScale / resolution.y + 4.0;
    
    vec2 motion1 = vec2(time * 0.15, time * -0.4);
    vec2 motion2 = vec2(time * -0.25, time * 0.25);
    
    // calculate noise layers
    vec2 distort1 = vec2(cnoise(noisecoord1 + motion1), cnoise(noisecoord2 + motion1));
    vec2 distort2 = vec2(cnoise(noisecoord1 + motion2), cnoise(noisecoord2 + motion2));
    
    vec2 distort_sum = (distort1 + distort2 * (speed + 0.25) * 5.0) / ((3.0 / scale) * 180.0);
    distort_sum /= 3.0;
    vec2 dstPlace = texCoord;
    
    //vec2 distortion = vec2(sin(time*2.0 + (clworldPos.y * maskSize.y / scale) / 5.0) * 4.0, 0.0);
    vec2 distortion = distort_sum;
    vec2 distorted_pos = clworldPos + distortion;
    vec2 dp = (distorted_pos * maskSize + maskPos) * scale / resolution;

    if(overlay == 1) {
        // flow map
        gl_FragColor = vec4(flowCol.rgb, 1.0);
    }else if(overlay == 2){
        // distortion
        gl_FragColor = vec4(length(distort_sum) * 150.0);
    }else{
        // normal output
        gl_FragColor = texture2D(tex, dp) * (1.0 + length(distort_sum) * (10.0 + speed * 15.0));
    }
}