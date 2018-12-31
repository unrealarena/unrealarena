/*
 * Daemon GPL Source Code
 * Copyright (C) 2018  Unreal Arena
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */


#include <string>
#include <unordered_map>
#ifdef UNREALARENA
#include "skybox_vp.h"
#include "ssao_fp.h"
#include "ssao_vp.h"
#include "vertexAnimation_vp.h"
#include "vertexLighting_DBS_entity_fp.h"
#include "vertexLighting_DBS_entity_vp.h"
#include "vertexLighting_DBS_world_fp.h"
#include "vertexLighting_DBS_world_vp.h"
#include "vertexSimple_vp.h"
#include "vertexSkinning_vp.h"
#include "vertexSprite_vp.h"
#include "volumetricFog_fp.h"
#include "volumetricFog_vp.h"
#include "blurX_fp.h"
#include "blurX_vp.h"
#include "blurY_fp.h"
#include "blurY_vp.h"
#include "cameraEffects_fp.h"
#include "cameraEffects_vp.h"
#include "contrast_fp.h"
#include "contrast_vp.h"
#include "debugShadowMap_fp.h"
#include "debugShadowMap_vp.h"
#include "deformVertexes_vp.h"
#include "depthtile1_fp.h"
#include "depthtile1_vp.h"
#include "depthtile2_fp.h"
#include "depthtile2_vp.h"
#include "depthToColor_fp.h"
#include "depthToColor_vp.h"
#include "dispersion_C_fp.h"
#include "dispersion_C_vp.h"
#include "fogGlobal_fp.h"
#include "fogGlobal_vp.h"
#include "fogQuake3_fp.h"
#include "fogQuake3_vp.h"
#include "forwardLighting_fp.h"
#include "forwardLighting_vp.h"
#include "fxaa_fp.h"
#include "fxaa_vp.h"
#include "fxaa3_11_fp.h"
#include "generic_fp.h"
#include "generic_vp.h"
#include "heatHaze_fp.h"
#include "heatHaze_vp.h"
#include "lightMapping_fp.h"
#include "lightMapping_vp.h"
#include "lighttile_fp.h"
#include "lighttile_vp.h"
#include "lightVolume_omni_fp.h"
#include "lightVolume_omni_vp.h"
#include "liquid_fp.h"
#include "liquid_vp.h"
#include "motionblur_fp.h"
#include "motionblur_vp.h"
#include "portal_fp.h"
#include "portal_vp.h"
#include "reflection_CB_fp.h"
#include "reflection_CB_vp.h"
#include "refraction_C_fp.h"
#include "refraction_C_vp.h"
#include "reliefMapping_fp.h"
#include "screen_fp.h"
#include "screen_vp.h"
#include "shadowFill_fp.h"
#include "shadowFill_vp.h"
#include "skybox_fp.h"
#else
#include "glsl/skybox_vp.h"
#include "glsl/ssao_fp.h"
#include "glsl/ssao_vp.h"
#include "glsl/vertexAnimation_vp.h"
#include "glsl/vertexLighting_DBS_entity_fp.h"
#include "glsl/vertexLighting_DBS_entity_vp.h"
#include "glsl/vertexLighting_DBS_world_fp.h"
#include "glsl/vertexLighting_DBS_world_vp.h"
#include "glsl/vertexSimple_vp.h"
#include "glsl/vertexSkinning_vp.h"
#include "glsl/vertexSprite_vp.h"
#include "glsl/volumetricFog_fp.h"
#include "glsl/volumetricFog_vp.h"
#include "glsl/blurX_fp.h"
#include "glsl/blurX_vp.h"
#include "glsl/blurY_fp.h"
#include "glsl/blurY_vp.h"
#include "glsl/cameraEffects_fp.h"
#include "glsl/cameraEffects_vp.h"
#include "glsl/contrast_fp.h"
#include "glsl/contrast_vp.h"
#include "glsl/debugShadowMap_fp.h"
#include "glsl/debugShadowMap_vp.h"
#include "glsl/deformVertexes_vp.h"
#include "glsl/depthtile1_fp.h"
#include "glsl/depthtile1_vp.h"
#include "glsl/depthtile2_fp.h"
#include "glsl/depthtile2_vp.h"
#include "glsl/depthToColor_fp.h"
#include "glsl/depthToColor_vp.h"
#include "glsl/dispersion_C_fp.h"
#include "glsl/dispersion_C_vp.h"
#include "glsl/fogGlobal_fp.h"
#include "glsl/fogGlobal_vp.h"
#include "glsl/fogQuake3_fp.h"
#include "glsl/fogQuake3_vp.h"
#include "glsl/forwardLighting_fp.h"
#include "glsl/forwardLighting_vp.h"
#include "glsl/fxaa_fp.h"
#include "glsl/fxaa_vp.h"
#include "glsl/fxaa3_11_fp.h"
#include "glsl/generic_fp.h"
#include "glsl/generic_vp.h"
#include "glsl/heatHaze_fp.h"
#include "glsl/heatHaze_vp.h"
#include "glsl/lightMapping_fp.h"
#include "glsl/lightMapping_vp.h"
#include "glsl/lighttile_fp.h"
#include "glsl/lighttile_vp.h"
#include "glsl/lightVolume_omni_fp.h"
#include "glsl/lightVolume_omni_vp.h"
#include "glsl/liquid_fp.h"
#include "glsl/liquid_vp.h"
#include "glsl/motionblur_fp.h"
#include "glsl/motionblur_vp.h"
#include "glsl/portal_fp.h"
#include "glsl/portal_vp.h"
#include "glsl/reflection_CB_fp.h"
#include "glsl/reflection_CB_vp.h"
#include "glsl/refraction_C_fp.h"
#include "glsl/refraction_C_vp.h"
#include "glsl/reliefMapping_fp.h"
#include "glsl/screen_fp.h"
#include "glsl/screen_vp.h"
#include "glsl/shadowFill_fp.h"
#include "glsl/shadowFill_vp.h"
#include "glsl/skybox_fp.h"
#endif

std::unordered_map<std::string, std::string> shadermap({
	{ "glsl/blurX_fp.glsl", std::string((char*)blurX_fp_glsl, blurX_fp_glsl_len) },
	{ "glsl/blurX_vp.glsl", std::string((char*)blurX_vp_glsl, blurX_vp_glsl_len) },
	{ "glsl/blurY_fp.glsl", std::string((char*)blurY_fp_glsl, blurY_fp_glsl_len) },
	{ "glsl/blurY_vp.glsl", std::string((char*)blurY_vp_glsl, blurY_vp_glsl_len) },
	{ "glsl/cameraEffects_fp.glsl", std::string((char*)cameraEffects_fp_glsl, cameraEffects_fp_glsl_len) },
	{ "glsl/cameraEffects_vp.glsl", std::string((char*)cameraEffects_vp_glsl, cameraEffects_vp_glsl_len) },
	{ "glsl/contrast_fp.glsl", std::string((char*)contrast_fp_glsl, contrast_fp_glsl_len) },
	{ "glsl/contrast_vp.glsl", std::string((char*)contrast_vp_glsl, contrast_vp_glsl_len) },
	{ "glsl/debugShadowMap_fp.glsl", std::string((char*)debugShadowMap_fp_glsl, debugShadowMap_fp_glsl_len) },
	{ "glsl/debugShadowMap_vp.glsl", std::string((char*)debugShadowMap_vp_glsl, debugShadowMap_vp_glsl_len) },
	{ "glsl/deformVertexes_vp.glsl", std::string((char*)deformVertexes_vp_glsl, deformVertexes_vp_glsl_len) },
	{ "glsl/depthToColor_fp.glsl", std::string((char*)depthToColor_fp_glsl, depthToColor_fp_glsl_len) },
	{ "glsl/depthToColor_vp.glsl", std::string((char*)depthToColor_vp_glsl, depthToColor_vp_glsl_len) },
	{ "glsl/depthtile1_fp.glsl", std::string((char*)depthtile1_fp_glsl, depthtile1_fp_glsl_len) },
	{ "glsl/depthtile1_vp.glsl", std::string((char*)depthtile1_vp_glsl, depthtile1_vp_glsl_len) },
	{ "glsl/depthtile2_fp.glsl", std::string((char*)depthtile2_fp_glsl, depthtile2_fp_glsl_len) },
	{ "glsl/depthtile2_vp.glsl", std::string((char*)depthtile2_vp_glsl, depthtile2_vp_glsl_len) },
	{ "glsl/dispersion_C_fp.glsl", std::string((char*)dispersion_C_fp_glsl, dispersion_C_fp_glsl_len) },
	{ "glsl/dispersion_C_vp.glsl", std::string((char*)dispersion_C_vp_glsl, dispersion_C_vp_glsl_len) },
	{ "glsl/fogGlobal_fp.glsl", std::string((char*)fogGlobal_fp_glsl, fogGlobal_fp_glsl_len) },
	{ "glsl/fogGlobal_vp.glsl", std::string((char*)fogGlobal_vp_glsl, fogGlobal_vp_glsl_len) },
	{ "glsl/fogQuake3_fp.glsl", std::string((char*)fogQuake3_fp_glsl, fogQuake3_fp_glsl_len) },
	{ "glsl/fogQuake3_vp.glsl", std::string((char*)(char*)fogQuake3_vp_glsl, fogQuake3_vp_glsl_len) },
	{ "glsl/forwardLighting_fp.glsl", std::string((char*)forwardLighting_fp_glsl, forwardLighting_fp_glsl_len) },
	{ "glsl/forwardLighting_vp.glsl", std::string((char*)forwardLighting_vp_glsl, forwardLighting_vp_glsl_len) },
	{ "glsl/fxaa3_11_fp.glsl", std::string((char*)fxaa3_11_fp_glsl, fxaa3_11_fp_glsl_len) },
	{ "glsl/fxaa_fp.glsl", std::string((char*)fxaa_fp_glsl, fxaa_fp_glsl_len) },
	{ "glsl/fxaa_vp.glsl", std::string((char*)fxaa_vp_glsl, fxaa_vp_glsl_len) },
	{ "glsl/generic_fp.glsl", std::string((char*)generic_fp_glsl, generic_fp_glsl_len) },
	{ "glsl/generic_vp.glsl", std::string((char*)generic_vp_glsl, generic_vp_glsl_len) },
	{ "glsl/heatHaze_fp.glsl", std::string((char*)heatHaze_fp_glsl, heatHaze_fp_glsl_len) },
	{ "glsl/heatHaze_vp.glsl", std::string((char*)heatHaze_vp_glsl, heatHaze_vp_glsl_len) },
	{ "glsl/lightMapping_fp.glsl", std::string((char*)lightMapping_fp_glsl, lightMapping_fp_glsl_len) },
	{ "glsl/lightMapping_vp.glsl", std::string((char*)lightMapping_vp_glsl, lightMapping_vp_glsl_len) },
	{ "glsl/lightVolume_omni_fp.glsl", std::string((char*)lightVolume_omni_fp_glsl, lightVolume_omni_fp_glsl_len) },
	{ "glsl/lightVolume_omni_vp.glsl", std::string((char*)lightVolume_omni_vp_glsl, lightVolume_omni_vp_glsl_len) },
	{ "glsl/lighttile_fp.glsl", std::string((char*)lighttile_fp_glsl, lighttile_fp_glsl_len) },
	{ "glsl/lighttile_vp.glsl", std::string((char*)lighttile_vp_glsl, lighttile_vp_glsl_len) },
	{ "glsl/liquid_fp.glsl", std::string((char*)liquid_fp_glsl, liquid_fp_glsl_len) },
	{ "glsl/liquid_vp.glsl", std::string((char*)liquid_vp_glsl, liquid_vp_glsl_len) },
	{ "glsl/motionblur_fp.glsl", std::string((char*)motionblur_fp_glsl, motionblur_fp_glsl_len) },
	{ "glsl/motionblur_vp.glsl", std::string((char*)motionblur_vp_glsl, motionblur_vp_glsl_len) },
	{ "glsl/portal_fp.glsl", std::string((char*)portal_fp_glsl, portal_fp_glsl_len) },
	{ "glsl/portal_vp.glsl", std::string((char*)portal_vp_glsl, portal_vp_glsl_len) },
	{ "glsl/reflection_CB_fp.glsl", std::string((char*)reflection_CB_fp_glsl, reflection_CB_fp_glsl_len) },
	{ "glsl/reflection_CB_vp.glsl", std::string((char*)reflection_CB_vp_glsl, reflection_CB_vp_glsl_len) },
	{ "glsl/refraction_C_fp.glsl", std::string((char*)refraction_C_fp_glsl, refraction_C_fp_glsl_len) },
	{ "glsl/refraction_C_vp.glsl", std::string((char*)refraction_C_vp_glsl, refraction_C_vp_glsl_len) },
	{ "glsl/reliefMapping_fp.glsl", std::string((char*)reliefMapping_fp_glsl, reliefMapping_fp_glsl_len) },
	{ "glsl/screen_fp.glsl", std::string((char*)screen_fp_glsl, screen_fp_glsl_len) },
	{ "glsl/screen_vp.glsl", std::string((char*)screen_vp_glsl, screen_vp_glsl_len) },
	{ "glsl/shadowFill_fp.glsl", std::string((char*)shadowFill_fp_glsl, shadowFill_fp_glsl_len) },
	{ "glsl/shadowFill_vp.glsl", std::string((char*)shadowFill_vp_glsl, shadowFill_vp_glsl_len) },
	{ "glsl/skybox_fp.glsl", std::string((char*)skybox_fp_glsl, skybox_fp_glsl_len) },
	{ "glsl/skybox_vp.glsl", std::string((char*)skybox_vp_glsl, skybox_vp_glsl_len) },
	{ "glsl/ssao_fp.glsl", std::string((char*)ssao_fp_glsl, ssao_fp_glsl_len) },
	{ "glsl/ssao_vp.glsl", std::string((char*)ssao_vp_glsl, ssao_vp_glsl_len) },
	{ "glsl/vertexAnimation_vp.glsl", std::string((char*)vertexAnimation_vp_glsl, vertexAnimation_vp_glsl_len) },
	{ "glsl/vertexLighting_DBS_entity_fp.glsl", std::string((char*)vertexLighting_DBS_entity_fp_glsl, vertexLighting_DBS_entity_fp_glsl_len) },
	{ "glsl/vertexLighting_DBS_entity_vp.glsl", std::string((char*)vertexLighting_DBS_entity_vp_glsl, vertexLighting_DBS_entity_vp_glsl_len) },
	{ "glsl/vertexLighting_DBS_world_fp.glsl", std::string((char*)vertexLighting_DBS_world_fp_glsl, vertexLighting_DBS_world_fp_glsl_len) },
	{ "glsl/vertexLighting_DBS_world_vp.glsl", std::string((char*)vertexLighting_DBS_world_vp_glsl, vertexLighting_DBS_world_vp_glsl_len) },
	{ "glsl/vertexSimple_vp.glsl", std::string((char*)vertexSimple_vp_glsl, vertexSimple_vp_glsl_len) },
	{ "glsl/vertexSkinning_vp.glsl", std::string((char*)vertexSkinning_vp_glsl, vertexSkinning_vp_glsl_len) },
	{ "glsl/vertexSprite_vp.glsl", std::string((char*)vertexSprite_vp_glsl, vertexSprite_vp_glsl_len) },
	{ "glsl/volumetricFog_fp.glsl", std::string((char*)volumetricFog_fp_glsl, volumetricFog_fp_glsl_len) },
	{ "glsl/volumetricFog_vp.glsl", std::string((char*)volumetricFog_vp_glsl, volumetricFog_vp_glsl_len) },
	});
