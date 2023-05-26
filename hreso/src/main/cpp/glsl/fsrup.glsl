#version 300 es
precision highp float;
in highp vec2 v_TexCoordinate;
uniform sampler2D uTexture;
out vec4 glFragColor;
uniform float iW;
uniform float iH;
uniform float w;
uniform float h;

void main() {
	vec4 scale = vec4(1. / vec2(iW, iH), vec2(iW, iH) / vec2(w,h));

	// Source position in fractions of a texel
	vec2 src_pos = scale.zw * v_TexCoordinate;
	// Source bottom left texel centre
	vec2 src_centre = floor(src_pos - .5) + .5;
	// f is position. f.x runs left to right, y bottom to top, z right to left, w top to bottom
	vec4 f; f.zw = 1. - (f.xy = src_pos - src_centre);
	// Calculate weights in x and y in parallel.
	// These polynomials are piecewise approximation of Lanczos kernel
	// Calculator here: https://gist.github.com/going-digital/752271db735a07da7617079482394543
	vec4 l2_w0_o3 = ((1.5672 * f - 2.6445) * f + 0.0837) * f + 0.9976;
	vec4 l2_w1_o3 = ((-0.7389 * f + 1.3652) * f - 0.6295) * f - 0.0004;

	vec4 w1_2 = l2_w0_o3;
	vec2 w12 = w1_2.xy + w1_2.zw;
	vec4 wedge = l2_w1_o3.xyzw * w12.yxyx;
	// Calculate texture read positions. tc12 uses bilinear interpolation to do 4 reads in 1.
	vec2 tc12 = scale.xy * (src_centre + w1_2.zw / w12);
	vec2 tc0 = scale.xy * (src_centre - 1.);
	vec2 tc3 = scale.xy * (src_centre + 2.);
	// Sharpening adjustment
	// Thanks mad_gooze for the normalization fix.
	float sum = wedge.x + wedge.y + wedge.z + wedge.w + w12.x * w12.y;
	wedge /= sum;
	vec3 col = vec3(
		texture(uTexture, vec2(tc12.x, tc0.y)).rgb * wedge.y +
		texture(uTexture, vec2(tc0.x, tc12.y)).rgb * wedge.x +
		texture(uTexture, tc12.xy).rgb * (w12.x * w12.y) +
		texture(uTexture, vec2(tc3.x, tc12.y)).rgb * wedge.z +
		texture(uTexture, vec2(tc12.x, tc3.y)).rgb * wedge.w
	);

	glFragColor = vec4(col,1);
}