precision mediump float;
uniform sampler2D g_Tex0;
uniform vec4 u_color;       //rgb,strength
uniform vec4 u_blurInfo;   //blurw,blurh,texw,texh
uniform vec4 u_blur_off;    //blurx,blury,offx,offy
uniform float g_Alpha;
varying vec2 texCoord;

#define PI 3.141593

float sigma=u_blurInfo.x/3.0;//3������Ӱ���С��������=1��ʱ�򣬰뾶Ϊ3
float sig2 = sigma*sigma;
float _2sig2 = 2.0*sig2;
//return 1.0/(2*PI*sig2)*exp(-(x*x+y*y)/_2sig2)
float gauss1 = 1.0/(2.0*PI*sig2);

float getGaussian(float x, float y){
    return gauss1*exp(-(x*x+y*y)/_2sig2);
}

vec4 blur(){
    const float blurw = 9.0;
    vec4 vec4Color = vec4(0.0,0.0,0.0,0.0);
    vec2 halfsz=vec2(blurw,blurw)/2.0/u_blurInfo.zw;    
    vec2 startpos=texCoord-halfsz-u_blur_off.zw;
    vec2 ctexcoord = startpos;
    vec2 step = 1.0/u_blurInfo.zw;  //ÿ������      
    
    for(float y = 0.0;y<=blurw; ++y){
        ctexcoord.x=startpos.x;
        for(float x = 0.0;x<=blurw; ++x){
            //TODO ��������Ĺ̶�ƫ��Ӧ����vs�д���
            vec4Color += texture2D(g_Tex0, ctexcoord)*getGaussian(x-blurw/2.0,y-blurw/2.0);
            ctexcoord.x+=step.x;
        }
        ctexcoord.y+=step.y;
    }
    return vec4Color;
}

void main(){
   gl_FragColor = blur();
   gl_FragColor.w*=g_Alpha;
}

