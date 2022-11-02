#version 300 es
precision highp float;
in highp vec2 fragCoord;
in vec2 wh;

uniform sampler2D iChannel0;
out vec4 glFragColor;

void main() {
	/// Normalized pixel coordinates (from 0 to 1)
	vec2 uv = fragCoord/wh.xy;

	// Time varying pixel color
	vec3 col = texture(iChannel0, uv).xyz;

	// CAS algorithm
	float max_g = col.y;
	float min_g = col.y;
	vec4 uvoff = vec4(1,0,1,-1)/vec4(wh.x, wh.x, wh.y, wh.y);
	vec3 colw;
	vec3 col1 = texture(iChannel0, uv+uvoff.yw).xyz;
	max_g = max(max_g, col1.y);
	min_g = min(min_g, col1.y);
	colw = col1;
	col1 = texture(iChannel0, uv+uvoff.xy).xyz;
	max_g = max(max_g, col1.y);
	min_g = min(min_g, col1.y);
	colw += col1;
	col1 = texture(iChannel0, uv+uvoff.yz).xyz;
	max_g = max(max_g, col1.y);
	min_g = min(min_g, col1.y);
	colw += col1;
	col1 = texture(iChannel0, uv-uvoff.xy).xyz;
	max_g = max(max_g, col1.y);
	min_g = min(min_g, col1.y);
	colw += col1;
	float d_min_g = min_g;
	float d_max_g = 1.-max_g;
	float A;
	if (d_max_g < d_min_g) {
		A = d_max_g / max_g;
	} else {
		A = d_min_g / max_g;
	}
	A = sqrt(A);
	A *= mix(-.125, -.2, 1.0);
	vec3 col_out = (col + colw * A) / (1.+4.*A);
	glFragColor = vec4(col_out,1);
}