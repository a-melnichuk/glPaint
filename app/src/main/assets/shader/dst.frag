precision mediump float;

varying vec2 v_TexCoord;
varying vec2 v_Touch;
varying vec2 v_Pos;

uniform sampler2D u_Mask;
uniform sampler2D u_Sampler;
uniform vec3 u_Color;

void main() {
    vec3 mask = texture2D(u_Mask, v_TexCoord).rgb;
    vec3 tex = texture2D(u_Sampler, v_TexCoord).rgb;

    if (length(v_Pos - v_Touch) < 0.05) {
        vec3 paint = vec3(1.0, 0.0, 0.0);
        gl_FragColor = vec4(u_Color * mask, 1.0);
        return;
    }

    gl_FragColor = vec4(tex, 1.0);
}
