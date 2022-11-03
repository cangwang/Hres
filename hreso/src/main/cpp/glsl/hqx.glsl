#version 300 es
precision highp float;
in highp vec2 v_TexCoordinate;
uniform sampler2D uTexture;
out vec4 glFragColor;

//upscaling multiplier amount
#define UPSCALE 10.

//image mipmap level, for base upscaling
#define ML 0

//equality threshold of 2 colors before forming lines
#define THRESHOLD .1

//line thickness
float LINE_THICKNESS;

//anti aliasing scaling, smaller value make lines more blurry
#define AA_SCALE (UPSCALE*1.)

bool diag(inout vec4 sum, vec2 uv, vec2 p1, vec2 p2) {
	vec4 v1 = texelFetch(uTexture,ivec2(uv+vec2(p1.x,p1.y)),ML),
	v2 = texelFetch(uTexture,ivec2(uv+vec2(p2.x,p2.y)),ML);
	if (length(v1-v2) < THRESHOLD) {
		vec2 dir = p2-p1,
		lp = uv-(floor(uv+p1)+.5);
		dir = normalize(vec2(dir.y,-dir.x));
		float l = clamp((LINE_THICKNESS-dot(lp,dir))*AA_SCALE,0.,1.);
		sum = mix(sum,v1,l);
		return true;
	}
	return false;
}

void main() {
	vec2 ip = v_TexCoordinate/UPSCALE;

	//start with nearest pixel as 'background'
	vec4 s = texelFetch(uTexture,ivec2(ip),ML);

	//draw anti aliased diagonal lines of surrounding pixels as 'foreground'
	LINE_THICKNESS = .4;
	if (diag(s,ip,vec2(-1, 0),vec2(0, 1))) {
		LINE_THICKNESS = .3;
		diag(s,ip,vec2(-1, 0),vec2(1, 1));
		diag(s,ip,vec2(-1, -1),vec2(0, 1));
	}
	LINE_THICKNESS = .4;
	if (diag(s,ip,vec2(0, 1),vec2(1, 0))) {
		LINE_THICKNESS = .3;
		diag(s,ip,vec2(0,1),vec2(1, -1));
		diag(s,ip,vec2(-1, 1),vec2(1, 0));
	}
	LINE_THICKNESS = .4;
	if (diag(s,ip,vec2(1, 0),vec2(0, -1))) {
		LINE_THICKNESS = .3;
		diag(s,ip,vec2(1, 0),vec2(-1, -1));
		diag(s,ip,vec2(1, 1),vec2(0, -1));
	}
	LINE_THICKNESS = .4;
	if (diag(s,ip,vec2(0, -1),vec2(-1, 0))) {
		LINE_THICKNESS = .3;
		diag(s,ip,vec2(0, -1),vec2(-1, 1));
		diag(s,ip,vec2(1, -1),vec2(-1, 0));
	}

	glFragColor = s;
}