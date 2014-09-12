attribute vec4 vPosition;
attribute vec4 vNormal;
attribute vec4 vTexCoord;
uniform vec4 uColor;

uniform	 mat4	 mvp_matrix;	 // model-view-projection matrix
uniform	 mat4	 normal_matrix;

varying vec4 fColor;
varying vec2 fTexCoord;
void main() {
  gl_Position = mvp_matrix * vPosition;
  float c;
  vec4 tNormal = normal_matrix * vNormal;
  tNormal = normalize(tNormal);
  if (tNormal.x > 0){
 	c = tNormal.x;
  }
  else{
  	c = 0;
  }
  fTexCoord = vTexCoord;
  //fTexCoord = vec2(vPosition.x, vPosition.y);
  fColor = vec4(c,c,c ,1);//uColor;
}