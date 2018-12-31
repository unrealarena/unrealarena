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

/* reflection_CB_fp.glsl */

uniform samplerCube	u_ColorMap;
uniform sampler2D	u_NormalMap;
uniform vec3		u_ViewOrigin;
uniform mat4		u_ModelMatrix;

IN(smooth) vec3		var_Position;
IN(smooth) vec2		var_TexNormal;
IN(smooth) vec4		var_Tangent;
IN(smooth) vec4		var_Binormal;
IN(smooth) vec4		var_Normal;

DECLARE_OUTPUT(vec4)

void	main()
{
	// compute incident ray in world space
	vec3 I = normalize(var_Position - u_ViewOrigin);

	// compute normal in tangent space from normalmap
	vec3 N = texture2D(u_NormalMap, var_TexNormal.st).xyw;
	N.x *= N.z;
	N.xy = 2.0 * N.xy - 1.0;
	N.z = sqrt(1.0 - dot(N.xy, N.xy));
#if defined(r_NormalScale)
	N.z *= r_NormalScale;
	normalize(N);
#endif

	mat3 tangentToWorldMatrix = mat3(var_Tangent.xyz, var_Binormal.xyz, var_Normal.xyz);

	// transform normal into world space
	N = normalize(tangentToWorldMatrix * N);

	// compute reflection ray
	vec3 R = reflect(I, N);

	outputColor = textureCube(u_ColorMap, R).rgba;
	// outputColor = vec4(1.0, 0.0, 0.0, 1.0);
}
