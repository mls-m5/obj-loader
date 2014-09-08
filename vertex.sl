//attribute vec4 vPosition;
//attribute vec4 vNormal;
//uniform vec4 uColor;


uniform mat4 ProjectionModelviewMatrix;
uniform mat4 ModelviewMatrix;

attribute vec4 InVertex;  //w will be set to 1.0 automatically
attribute vec2 InTexCoord0;
attribute vec3 InNormal;
uniform	 mat4	 mvp_matrix;	 // model-view-projection matrix

varying vec4 fColor;
void main() {
  //  gl_Position = ProjectionModelviewMatrix * InVertex;
  gl_Position = vec4(0,0,0,0);
    vec3 normal = vec3(ModelviewMatrix * vec4(InNormal, 0.0));
    fColor = vec4(normal * vec3(1, 0, 0), 1);
  //gl_Position = mvp_matrix * vPosition;
  //fColor = vec4(0,1,0,1);//uColor;
}