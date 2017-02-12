precision mediump float;

varying vec2 v_TexCoord;
uniform sampler2D u_Sampler;

void main() {
    vec3 tex = texture2D(u_Sampler, v_TexCoord).rgb;
    gl_FragColor = vec4(tex, 1.0);
}
