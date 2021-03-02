precision mediump float;
uniform sampler2D g_Tex0;
uniform mat4 g_colorMat;
uniform vec4 g_colorAlpha;
uniform float g_Alpha;
varying vec2 texCoord;
void main(){
   vec4 color = texture2D(g_Tex0, texCoord.xy);
   gl_FragColor =  color * g_colorMat + g_colorAlpha/255.0;
   gl_FragColor.w *= g_Alpha;
}