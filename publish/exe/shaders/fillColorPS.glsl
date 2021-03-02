precision mediump float;
uniform float g_Alpha;
varying vec4 oColor;
void main()
{
   gl_FragColor=vec4(oColor.xyz,g_Alpha);
}

