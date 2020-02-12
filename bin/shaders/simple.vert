#version 330

uniform mat4 mvp;
uniform int cat_id;
layout (location = 0) in vec4 pos;
layout (location = 1) in float cat;

out vec4 out_col;

// All components are in the range [0…1], including hue.
vec3 hsv2rgb(vec3 c) {
    vec4 K = vec4(1.0, 2.0 / 3.0, 1.0 / 3.0, 3.0);
    vec3 p = abs(fract(c.xxx + K.xyz) * 6.0 - K.www);
    return c.z * mix(K.xxx, clamp(p - K.xxx, 0.0, 1.0), c.y);
}


void main() {
    
    int i = 1;
    if(cat<0.0) {
        out_col = vec4(0.2,0.2,0.2,0.2);
    } else {
        out_col = vec4(hsv2rgb(vec3(cat/150.0,0.75,1.0)),0.5);
    }

    gl_Position = mvp*vec4(pos.xyz, 1.0);
}
