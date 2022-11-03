#version 300 es
precision highp float;
in highp vec2 v_TexCoordinate;
uniform sampler2D uTexture;
out vec4 glFragColor;
uniform float w;
uniform float h;

void main() {
	/// Normalized pixel coordinates (from 0 to 1)
	vec2 uv = v_TexCoordinate;
	// Time varying pixel color
	vec3 col = texture(uTexture, uv).xyz;

	// CAS algorithm
	float max_g = col.y;
	float min_g = col.y;
	vec4 uvoff = vec4(1,0,1,-1)/vec4(w, w, h, h);
	vec3 colw;
	vec3 col1 = texture(uTexture, uv+uvoff.yw).xyz;
	max_g = max(max_g, col1.y);
	min_g = min(min_g, col1.y);
	colw = col1;
	col1 = texture(uTexture, uv+uvoff.xy).xyz;
	max_g = max(max_g, col1.y);
	min_g = min(min_g, col1.y);
	colw += col1;
	col1 = texture(uTexture, uv+uvoff.yz).xyz;
	max_g = max(max_g, col1.y);
	min_g = min(min_g, col1.y);
	colw += col1;
	col1 = texture(uTexture, uv-uvoff.xy).xyz;
	max_g = max(max_g, col1.y);
	min_g = min(min_g, col1.y);
	colw += col1;
	float d_min_g = min_g;
	float d_max_g = 1.-max_g;
//	float A;
//	if (d_max_g < d_min_g) {
//		A = d_max_g / max_g;
//	} else {
//		A = d_min_g / max_g;
//	}
	float A = step(d_min_g, d_max_g) / max_g;
	A = sqrt(A);
	A *= mix(-.125, -.2, 1.0);
	vec3 col_out = (col + colw * A) / (1.+4.*A);
	glFragColor = vec4(col_out,1);
}