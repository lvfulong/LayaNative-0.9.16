precision mediump float;
uniform sampler2D g_Tex0;
uniform vec4 u_color;       //rgb,strength
uniform vec4 u_blurInfo;   //blurw,blurh,texw,texh
uniform vec4 u_blur_off;    //blurx,blury,offx,offy
uniform float g_Alpha;
varying vec2 texCoord;

vec4 blur(){
    const float blurw = 10.0;
    const float floatIterationTotalTime = 100.0;
    vec4 vec4Color = vec4(0.0,0.0,0.0,0.0);
    vec2 halfsz=vec2(blurw,blurw)/u_blurInfo.zw;    //这里x2了，其实原来是/2
    vec2 startpos=texCoord-halfsz-u_blur_off.zw;
    vec2 ctexcoord = startpos;
    vec2 step = 2.0/u_blurInfo.zw;  //每个像素      //这里x2了
    
    for(float i = 0.0;i<=blurw; ++i){
        ctexcoord.x=startpos.x;
        for(float j = 0.0;j<=blurw; ++j){
            //TODO 纹理坐标的固定偏移应该在vs中处理
            vec4Color += texture2D(g_Tex0, ctexcoord)/floatIterationTotalTime;
            ctexcoord.x+=step.x;
        }
        ctexcoord.y+=step.y;
    }
    return vec4Color;
}

void main(){
   vec4 col = blur();
   gl_FragColor=vec4(u_color.rgb,col.a*u_color.w);
   gl_FragColor.w*=g_Alpha;
}

