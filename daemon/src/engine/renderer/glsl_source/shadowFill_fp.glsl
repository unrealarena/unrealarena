/*
===========================================================================
Copyright (C) 2006-2011 Robert Beckebans <trebor_7@users.sourceforge.net>

This file is part of XreaL source code.

XreaL source code is free software; you can redistribute it
and/or modify it under the terms of the GNU General Public License as
published by the Free Software Foundation; either version 2 of the License,
or (at your option) any later version.

XreaL source code is distributed in the hope that it will be
useful, but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with XreaL source code; if not, write to the Free Software
Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
===========================================================================
*/

/* shadowFill_fp.glsl */

uniform sampler2D	u_ColorMap;
uniform float		u_AlphaThreshold;
uniform vec3		u_LightOrigin;
uniform float		u_LightRadius;

IN(smooth) vec3		var_Position;
IN(smooth) vec2		var_Tex;
IN(smooth) vec4		var_Color;

DECLARE_OUTPUT(vec4)

#ifdef TEXTURE_RG
#  define SWIZ1 r
#  define SWIZ2 rg
#else
#  define SWIZ1 a
#  define SWIZ2 ar
#endif

#if defined(EVSM)

vec2 WarpDepth(float depth)
{
    // rescale depth into [-1, 1]
    depth = 2.0 * depth - 1.0;
    float pos =  exp( r_EVSMExponents.x * depth);
    float neg = -exp(-r_EVSMExponents.y * depth);

    return vec2(pos, neg);
}

vec4 ShadowDepthToEVSM(float depth)
{
	vec2 warpedDepth = WarpDepth(depth);
	return vec4(warpedDepth.x, warpedDepth.x * warpedDepth.x, warpedDepth.y, warpedDepth.y * warpedDepth.y);
}

#endif // #if defined(EVSM)

void	main()
{
	vec4 color = texture2D(u_ColorMap, var_Tex);

	if( abs(color.a + u_AlphaThreshold) <= 1.0 )
	{
		discard;
		return;
	}


#if defined(VSM)

	float distance;

#if defined(LIGHT_DIRECTIONAL)
	distance = gl_FragCoord.z;
#else
	distance = length(var_Position - u_LightOrigin) / u_LightRadius;
#endif

	float distanceSquared = distance * distance;

	// shadowmap can be float RGBA or luminance alpha so store distanceSquared into alpha

#if defined(VSM_CLAMP)
	// convert to [0,1] color space
	outputColor.SWIZ2 = vec2(distance, distanceSquared) * 0.5 + 0.5;
#else
	outputColor.SWIZ2 = vec2(distance, distanceSquared);
#endif

#elif defined(EVSM) || defined(ESM)

	float distance;
#if defined(LIGHT_DIRECTIONAL)
	{
		distance = gl_FragCoord.z;// * r_ShadowMapDepthScale;
		//distance /= gl_FragCoord.w;
		//distance = var_Position.z / var_Position.w;
		//distance = var_Position.z;
	}
#else
	{
		distance = (length(var_Position - u_LightOrigin) / u_LightRadius); // * r_ShadowMapDepthScale;
	}
#endif

#if defined(EVSM)
#if !defined(r_EVSMPostProcess)
	outputColor = ShadowDepthToEVSM(distance);
#else
	outputColor.SWIZ1 = distance;
#endif
#else
	outputColor.SWIZ1 = distance;
#endif // defined(EVSM)

#else
	outputColor = vec4(0.0, 0.0, 0.0, 0.0);
#endif
}
