#version 300 es
precision highp float;
in highp vec2 v_TexCoordinate;
uniform sampler2D uTexture;
out vec4 glFragColor;

uniform float w;
uniform float h;
float c_textureSize = 0.0;

float c_onePixel = 0.0;
float c_twoPixels = 0.0;

float c_x0 = -1.0;
float c_x1 =  0.0;
float c_x2 =  1.0;
float c_x3 =  2.0;

vec3 CubicLagrange (vec3 A, vec3 B, vec3 C, vec3 D, float t) {
	return
	A * (
	(t - c_x1) / (c_x0 - c_x1) *
	(t - c_x2) / (c_x0 - c_x2) *
	(t - c_x3) / (c_x0 - c_x3)) +
	B * (
	(t - c_x0) / (c_x1 - c_x0) *
	(t - c_x2) / (c_x1 - c_x2) *
	(t - c_x3) / (c_x1 - c_x3)) +
	C * (
	(t - c_x0) / (c_x2 - c_x0) *
	(t - c_x1) / (c_x2 - c_x1) *
	(t - c_x3) / (c_x2 - c_x3)) +
	D * (
	(t - c_x0) / (c_x3 - c_x0) *
	(t - c_x1) / (c_x3 - c_x1) *
	(t - c_x2) / (c_x3 - c_x2)
	);
}

//=======================================================================================
vec3 BicubicLagrangeTextureSample (vec2 P) {
	vec2 pixel = P * c_textureSize + 0.5;

	vec2 frac = fract(pixel);
	pixel = floor(pixel) / c_textureSize - vec2(c_onePixel/2.0);

	vec3 C00 = texture(uTexture, pixel + vec2(-c_onePixel ,-c_onePixel)).rgb;
	vec3 C10 = texture(uTexture, pixel + vec2( 0.0        ,-c_onePixel)).rgb;
	vec3 C20 = texture(uTexture, pixel + vec2( c_onePixel ,-c_onePixel)).rgb;
	vec3 C30 = texture(uTexture, pixel + vec2( c_twoPixels,-c_onePixel)).rgb;

	vec3 C01 = texture(uTexture, pixel + vec2(-c_onePixel , 0.0)).rgb;
	vec3 C11 = texture(uTexture, pixel + vec2( 0.0        , 0.0)).rgb;
	vec3 C21 = texture(uTexture, pixel + vec2( c_onePixel , 0.0)).rgb;
	vec3 C31 = texture(uTexture, pixel + vec2( c_twoPixels, 0.0)).rgb;

	vec3 C02 = texture(uTexture, pixel + vec2(-c_onePixel , c_onePixel)).rgb;
	vec3 C12 = texture(uTexture, pixel + vec2( 0.0        , c_onePixel)).rgb;
	vec3 C22 = texture(uTexture, pixel + vec2( c_onePixel , c_onePixel)).rgb;
	vec3 C32 = texture(uTexture, pixel + vec2( c_twoPixels, c_onePixel)).rgb;

	vec3 C03 = texture(uTexture, pixel + vec2(-c_onePixel , c_twoPixels)).rgb;
	vec3 C13 = texture(uTexture, pixel + vec2( 0.0        , c_twoPixels)).rgb;
	vec3 C23 = texture(uTexture, pixel + vec2( c_onePixel , c_twoPixels)).rgb;
	vec3 C33 = texture(uTexture, pixel + vec2( c_twoPixels, c_twoPixels)).rgb;

	vec3 CP0X = CubicLagrange(C00, C10, C20, C30, frac.x);
	vec3 CP1X = CubicLagrange(C01, C11, C21, C31, frac.x);
	vec3 CP2X = CubicLagrange(C02, C12, C22, C32, frac.x);
	vec3 CP3X = CubicLagrange(C03, C13, C23, C33, frac.x);

	return CubicLagrange(CP0X, CP1X, CP2X, CP3X, frac.y);
}


void main() {
	c_textureSize = sqrt(w*h);
	c_onePixel = (1.0 / c_textureSize);
	c_twoPixels = (2.0 / c_textureSize);
	glFragColor = vec4(BicubicLagrangeTextureSample(v_TexCoordinate), 1.0);
}