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

/* generic_fp.glsl */

uniform sampler2D	u_ColorMap;
uniform float		u_AlphaThreshold;

IN(smooth) vec2		var_Tex;
IN(smooth) vec4		var_Color;

#if defined(USE_DEPTH_FADE) || defined(USE_VERTEX_SPRITE)
IN(smooth) vec2         var_FadeDepth;
uniform sampler2D       u_DepthMap;
#endif

DECLARE_OUTPUT(vec4)

void	main()
{
	vec4 color = texture2D(u_ColorMap, var_Tex);

#if defined(USE_ALPHA_TESTING)
	if( abs(color.a + u_AlphaThreshold) <= 1.0 )
	{
		discard;
		return;
	}
#endif

#if defined(USE_DEPTH_FADE) || defined(USE_VERTEX_SPRITE)
	float depth = texture2D(u_DepthMap, gl_FragCoord.xy * r_FBufScale).x;
	float fadeDepth = 0.5 * var_FadeDepth.x / var_FadeDepth.y + 0.5;
	color.a *= smoothstep(gl_FragCoord.z, fadeDepth, depth);
#endif

	color *= var_Color;
	outputColor = color;
}
