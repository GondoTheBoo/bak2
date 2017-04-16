#version 330 core

layout ( location = 0 ) out vec4 fragmentColor;

uniform sampler2D tex_rsm_depth;
uniform sampler2D tex_rsm_normal;
uniform sampler2D tex_rsm_worldPos;
uniform sampler2D tex_rsm_intensity;


void main()
{	
	vec4 rsmDepth = texelFetch(tex_rsm_depth, ivec2(gl_FragCoord.xy),0);
	vec4 rsmNormal = texelFetch(tex_rsm_normal, ivec2(gl_FragCoord.xy),0);
	vec4 rsmWorldPos = texelFetch(tex_rsm_worldPos, ivec2(gl_FragCoord.xy),0);
	vec4 rsmIntensity = texelFetch(tex_rsm_intensity, ivec2(gl_FragCoord.xy),0);
	
	vec3 N = 2*rsmNormal.rgb - vec3( 1.0 );
	//fragmentColor = vec4(rsmWorldPos.rgb,1);
	//fragmentColor = vec4(rsmDepth.rgb, 1.0);
	//fragmentColor = vec4(N, 1.0);
	fragmentColor = vec4(rsmIntensity.rgb, 1.0);
}