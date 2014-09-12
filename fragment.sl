varying vec4 fColor;
varying vec2 fTexCoord;
uniform sampler2D sTexture;


void main() {
  //float t = mod(fTexCoord.x * 100, 1);
  //gl_FragColor = fColor;
  vec2 v1 = fTexCoord + vec2(.001, .001);
  vec2 v2 = fTexCoord + vec2(.001, -.001);
  vec2 v3 = fTexCoord + vec2(-.001, .001);
  vec2 v4 = fTexCoord + vec2(-.001, -.001);
  gl_FragColor = fColor * texture2D(sTexture, fTexCoord);
  //gl_FragColor = fColor * (texture2D(sTexture, fTexCoord) +
  //		texture2D(sTexture, v1) + 
  //		texture2D(sTexture, v2) + 
  //		texture2D(sTexture, v3) + 
  //		texture2D(sTexture, v4)) / 5.;
}