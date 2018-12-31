/*
===========================================================================
Copyright (C) 2006-2009 Robert Beckebans <trebor_7@users.sourceforge.net>

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

/* contrast_fp.glsl */

uniform sampler2D	u_ColorMap;

const vec4			LUMINANCE_VECTOR = vec4(0.2125, 0.7154, 0.0721, 0.0);

#if __VERSION__ > 120
out vec4 outputColor;
#else
#define outputColor gl_FragColor
#endif

// contrast adjustment function
vec4 f(vec4 color) {
	float L = dot(LUMINANCE_VECTOR, color);
	L = max(L - 0.71, 0.0) * (1.0 / (1.0 - 0.71));
	//L = pow(L, 8.0);
	return color * L;
}

void	main()
{
	vec2 scale = r_FBufScale;
	vec2 st = gl_FragCoord.st;

	// calculate the screen texcoord in the 0.0 to 1.0 range
	st *= r_FBufScale;

	// multiply with 4 because the FBO is only 1/4th of the screen resolution
	st *= vec2(4.0, 4.0);

	// perform a box filter for the downsample
	vec4 color = f(texture2D(u_ColorMap, st + vec2(-1.0, -1.0) * scale));
	color += f(texture2D(u_ColorMap, st + vec2(-1.0, 1.0) * scale));
	color += f(texture2D(u_ColorMap, st + vec2(1.0, -1.0) * scale));
	color += f(texture2D(u_ColorMap, st + vec2(1.0, 1.0) * scale));
	color *= 0.25;

	outputColor = color;
}
