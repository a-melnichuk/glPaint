attribute vec4 a_Pos;
attribute vec2 a_TexCoord;

uniform vec2 u_Touch;
varying vec2 v_TexCoord;
varying vec2 v_Pos;
varying vec2 v_Touch;

void main() {
    gl_Position = a_Pos;
    v_Pos = a_Pos.xy;
    v_TexCoord = a_TexCoord;
    v_Touch = u_Touch;
}
