# ==============================================================================
#  Project Settings
# ==============================================================================

project('Unreal Arena', 'c', 'cpp', default_options : ['c_std=gnu11', 'cpp_std=c++11'], version : '0.1')

add_project_arguments('-DUNREALARENA', language : 'cpp')

cc = meson.get_compiler('c')
cxx = meson.get_compiler('cpp')

cpp_warnings = [
  '-Wno-class-memaccess',
  '-Wno-format-truncation',
  '-Wno-implicit-fallthrough',
  '-Wno-null-pointer-arithmetic',
  '-Wno-sign-compare',
  '-Wno-unused-parameter',
  ]

c_warnings = [
  '-Wno-unused-parameter',
  ]

add_project_arguments(
  cxx.get_supported_arguments(cpp_warnings),
  language : 'cpp'
  )

add_project_arguments(
  cc.get_supported_arguments(c_warnings),
  language : 'c'
  )

# FIXME Skip this for releases
revision_h = vcs_tag(
  input : 'daemon/src/common/Revision.h.in',
  output : 'revision.h',
  command : ['git', 'rev-parse', '--short', 'HEAD'],
  fallback : 'unknown',
  )

# ==============================================================================
#  Engine
# ==============================================================================

commonlist_srcs = files(
  'daemon/src/common/Color.cpp',
  'daemon/src/common/Command.cpp',
  'daemon/src/common/Cvar.cpp',
  'daemon/src/common/Debugger.cpp',
  'daemon/src/common/FileSystem.cpp',
  'daemon/src/common/IPC/CommandBuffer.cpp',
  'daemon/src/common/IPC/Primitives.cpp',
  'daemon/src/common/KeyIdentification.cpp',
  'daemon/src/common/LineEditData.cpp',
  'daemon/src/common/Log.cpp',
  'daemon/src/common/String.cpp',
  'daemon/src/common/System.cpp',

  'daemon/src/common/cm/cm_load.cpp',
  'daemon/src/common/cm/cm_patch.cpp',
  'daemon/src/common/cm/cm_plane.cpp',
  'daemon/src/common/cm/cm_polylib.cpp',
  'daemon/src/common/cm/cm_test.cpp',
  'daemon/src/common/cm/cm_trace.cpp',
  'daemon/src/common/cm/cm_trisoup.cpp',

  'daemon/src/engine/qcommon/q_math.cpp',
  'daemon/src/engine/qcommon/q_shared.cpp',
  'daemon/src/engine/qcommon/q_unicode.cpp',
  )

qcommonlist_srcs = files(
  'daemon/src/engine/qcommon/cmd.cpp',
  'daemon/src/engine/qcommon/common.cpp',
  'daemon/src/engine/qcommon/crypto.cpp',
  'daemon/src/engine/qcommon/cvar.cpp',
  'daemon/src/engine/qcommon/files.cpp',
  'daemon/src/engine/qcommon/huffman.cpp',
  'daemon/src/engine/qcommon/msg.cpp',
  'daemon/src/engine/qcommon/net_chan.cpp',
  'daemon/src/engine/qcommon/net_ip.cpp',
  'daemon/src/engine/qcommon/parse.cpp',
  'daemon/src/engine/qcommon/translation.cpp',
  )

serverlist_srcs = files(
  'daemon/src/engine/botlib/bot_convert.cpp',
  'daemon/src/engine/botlib/bot_load.cpp',
  'daemon/src/engine/botlib/bot_local.cpp',
  'daemon/src/engine/botlib/bot_nav.cpp',

  'daemon/src/engine/server/CryptoChallenge.cpp',

  'daemon/src/engine/server/sv_bot.cpp',
  'daemon/src/engine/server/sv_ccmds.cpp',
  'daemon/src/engine/server/sv_client.cpp',
  'daemon/src/engine/server/sv_init.cpp',
  'daemon/src/engine/server/sv_main.cpp',
  'daemon/src/engine/server/sv_net_chan.cpp',
  'daemon/src/engine/server/sv_sgame.cpp',
  'daemon/src/engine/server/sv_snapshot.cpp',
  )

clientbaselist_srcs = files(
  'daemon/src/engine/botlib/bot_debug.cpp',
  'daemon/src/engine/botlib/bot_nav_edit.cpp',

  'daemon/src/engine/client/ClientApplication.cpp',

  'daemon/src/engine/client/cin_ogm.cpp',
  'daemon/src/engine/client/cl_avi.cpp',
  'daemon/src/engine/client/cl_cgame.cpp',
  'daemon/src/engine/client/cl_cin.cpp',
  'daemon/src/engine/client/cl_console.cpp',
  'daemon/src/engine/client/cl_input.cpp',
  'daemon/src/engine/client/cl_irc.cpp',
  'daemon/src/engine/client/cl_main.cpp',
  'daemon/src/engine/client/cl_parse.cpp',
  'daemon/src/engine/client/cl_scrn.cpp',
  'daemon/src/engine/client/dl_main.cpp',
  )

clientlist_srcs = files(
  'daemon/src/engine/audio/ALObjects.cpp',
  'daemon/src/engine/audio/Audio.cpp',
  'daemon/src/engine/audio/Emitter.cpp',
  'daemon/src/engine/audio/OggCodec.cpp',
  'daemon/src/engine/audio/OpusCodec.cpp',
  'daemon/src/engine/audio/Sample.cpp',
  'daemon/src/engine/audio/SoundCodec.cpp',
  'daemon/src/engine/audio/Sound.cpp',
  'daemon/src/engine/audio/WavCodec.cpp',

  'daemon/src/engine/client/cl_keys.cpp',
  'daemon/src/engine/client/key_binding.cpp',
  'daemon/src/engine/client/key_identification.cpp',

  'daemon/src/engine/sys/sdl_input.cpp',

  'daemon/src/engine/renderer/gl_shader.cpp',
  'daemon/src/engine/renderer/shaders.cpp',

  'daemon/src/engine/renderer/tr_animation.cpp',
  'daemon/src/engine/renderer/tr_backend.cpp',
  'daemon/src/engine/renderer/tr_bsp.cpp',
  'daemon/src/engine/renderer/tr_cmds.cpp',
  'daemon/src/engine/renderer/tr_curve.cpp',
  'daemon/src/engine/renderer/tr_decals.cpp',
  'daemon/src/engine/renderer/tr_fbo.cpp',
  'daemon/src/engine/renderer/tr_flares.cpp',
  'daemon/src/engine/renderer/tr_font.cpp',
  'daemon/src/engine/renderer/tr_image.cpp',
  'daemon/src/engine/renderer/tr_image_crn.cpp',
  'daemon/src/engine/renderer/tr_image_dds.cpp',
  'daemon/src/engine/renderer/tr_image_exr.cpp',
  'daemon/src/engine/renderer/tr_image_jpg.cpp',
  'daemon/src/engine/renderer/tr_image_ktx.cpp',
  'daemon/src/engine/renderer/tr_image_png.cpp',
  'daemon/src/engine/renderer/tr_image_tga.cpp',
  'daemon/src/engine/renderer/tr_image_webp.cpp',
  'daemon/src/engine/renderer/tr_init.cpp',
  'daemon/src/engine/renderer/tr_light.cpp',
  'daemon/src/engine/renderer/tr_main.cpp',
  'daemon/src/engine/renderer/tr_marks.cpp',
  'daemon/src/engine/renderer/tr_mesh.cpp',
  'daemon/src/engine/renderer/tr_model.cpp',
  'daemon/src/engine/renderer/tr_model_iqm.cpp',
  'daemon/src/engine/renderer/tr_model_md3.cpp',
  'daemon/src/engine/renderer/tr_model_md5.cpp',
  'daemon/src/engine/renderer/tr_model_skel.cpp',
  'daemon/src/engine/renderer/tr_noise.cpp',
  'daemon/src/engine/renderer/tr_scene.cpp',
  'daemon/src/engine/renderer/tr_shade_calc.cpp',
  'daemon/src/engine/renderer/tr_shade.cpp',
  'daemon/src/engine/renderer/tr_shader.cpp',
  'daemon/src/engine/renderer/tr_skin.cpp',
  'daemon/src/engine/renderer/tr_sky.cpp',
  'daemon/src/engine/renderer/tr_surface.cpp',
  'daemon/src/engine/renderer/tr_vbo.cpp',
  'daemon/src/engine/renderer/tr_world.cpp',

  'daemon/src/engine/sys/sdl_glimp.cpp',
)

dedserverlist_srcs = files(
  'daemon/src/engine/null/null_client.cpp',
  'daemon/src/engine/null/null_input.cpp',

  'daemon/src/engine/null/NullKeyboard.cpp',

  'daemon/src/engine/server/ServerApplication.cpp',
  )

daemon_incl = include_directories(
  'daemon/libs/',
  'daemon/src/',
  'daemon/src/engine/',
  )

# ------------------------------------------------------------------------------
#  External Libraries
# ------------------------------------------------------------------------------

dl_lib = cxx.find_library('dl')
m_lib = cxx.find_library('m')
rt_lib = cxx.find_library('rt')

threads_dep = dependency('threads')

# FIXME Build it in unrealarena-deps
zlib_dep = dependency('zlib')

deps_dir = 'daemon/deps/linux-' + get_option('deps_version')

depslib_dir = join_paths(meson.source_root(), deps_dir, 'lib/')
depslib64_dir = join_paths(meson.source_root(), deps_dir, 'lib64/')
depsinclude_dir = join_paths(deps_dir, 'include/')

# freetype2 --------------------------------------------------------------------

freetype2_lib = cxx.find_library(
  'freetype',
  dirs : [depslib_dir],
  )

freetype2_incl = include_directories(
  join_paths(depsinclude_dir, 'freetype2/'),
  )

freetype2_dep = declare_dependency(
  include_directories : [freetype2_incl],
  dependencies : [freetype2_lib],
  )

# geoip ------------------------------------------------------------------------

geoip_lib = cxx.find_library(
  'GeoIP',
  dirs : [depslib_dir],
  )

geoip_incl = include_directories(
  depsinclude_dir,
  )

geoip_dep = declare_dependency(
  include_directories : [geoip_incl],
  dependencies : [geoip_lib],
  compile_args : ['-DHAVE_GEOIP'],
  )

# glew -------------------------------------------------------------------------

glew_lib = cxx.find_library(
  'GLEW',
  dirs : [depslib64_dir],
  )

glew_incl = include_directories(
  depsinclude_dir,
  )

glu_dep = dependency('glu')

glew_dep = declare_dependency(
  include_directories : [glew_incl],
  dependencies : [glew_lib, glu_dep],
  )

# gmp --------------------------------------------------------------------------

gmp_lib = cxx.find_library(
  'gmp',
  dirs : [depslib_dir],
  )

gmp_incl = include_directories(
  depsinclude_dir,
  )

gmp_dep = declare_dependency(
  include_directories : [gmp_incl],
  dependencies : [gmp_lib],
  )

# libcurl ----------------------------------------------------------------------

libcurl_lib = cxx.find_library(
  'curl',
  dirs : [depslib_dir],
  )

libcurl_incl = include_directories(
  depsinclude_dir,
  )

libcurl_dep = declare_dependency(
  include_directories : [libcurl_incl],
  dependencies : [libcurl_lib],
  compile_args : ['-DCURL_STATICLIB'],
  )

# libjpeg ----------------------------------------------------------------------

libjpeg_lib = cxx.find_library(
  'libjpeg',
  dirs : [depslib_dir],
  )

libjpeg_incl = include_directories(
  depsinclude_dir,
  )

libjpeg_dep = declare_dependency(
  include_directories : [libjpeg_incl],
  dependencies : [libjpeg_lib],
  )

# libpng -----------------------------------------------------------------------

libpng_lib = cxx.find_library(
  'libpng',
  dirs : [depslib_dir],
  )

libpng_incl = include_directories(
  join_paths(depsinclude_dir, 'libpng16/'),
  )

libpng_dep = declare_dependency(
  include_directories : [libpng_incl],
  dependencies : [libpng_lib, zlib_dep, m_lib],
  )

# lua --------------------------------------------------------------------------

lua_lib = cxx.find_library(
  'lua',
  dirs : [depslib_dir],
  )

lua_incl = include_directories(
  depsinclude_dir,
  )

lua_dep = declare_dependency(
  include_directories : [lua_incl],
  dependencies : [lua_lib],
  )

# nettle -----------------------------------------------------------------------

nettle_lib = cxx.find_library(
  'nettle',
  dirs : [depslib_dir],
  )

nettle_incl = include_directories(
  depsinclude_dir,
  )

nettle_dep = declare_dependency(
  include_directories : [nettle_incl],
  dependencies : [nettle_lib],
  )

hogweed_lib = cxx.find_library(
  'hogweed',
  dirs : [depslib_dir],
  )

hogweed_incl = include_directories(
  depsinclude_dir,
  )

hogweed_dep = declare_dependency(
  include_directories : [hogweed_incl],
  dependencies : [hogweed_lib, gmp_dep, nettle_dep],
  )

# ogg --------------------------------------------------------------------------

ogg_lib = cxx.find_library(
  'ogg',
  dirs : [depslib_dir],
  )

ogg_incl = include_directories(
  depsinclude_dir,
  )

ogg_dep = declare_dependency(
  include_directories : [ogg_incl],
  dependencies : [ogg_lib],
  )

# openal -----------------------------------------------------------------------

openal_lib = cxx.find_library(
  'openal',
  dirs : [depslib_dir],
  )

openal_incl = include_directories(
  join_paths(depsinclude_dir, 'AL/'),
  )

openal_dep = declare_dependency(
  include_directories : [openal_incl],
  dependencies : [openal_lib],
  )

# opus -------------------------------------------------------------------------

opus_lib = cxx.find_library(
  'opus',
  dirs : [depslib_dir],
  )

opus_incl = include_directories(
  join_paths(depsinclude_dir, 'opus/'),
  )

opus_dep = declare_dependency(
  include_directories : [opus_incl],
  dependencies : [opus_lib, m_lib],
  )

opusfile_lib = cxx.find_library(
  'opusfile',
  dirs : [depslib_dir],
  )

opusfile_incl = include_directories(
  join_paths(depsinclude_dir, 'opus/'),
  )

opusfile_dep = declare_dependency(
  include_directories : [opusfile_incl],
  dependencies : [opusfile_lib, ogg_dep, opus_dep],
  )

# sdl2 -------------------------------------------------------------------------

sdl2_lib = cxx.find_library(
  'SDL2',
  dirs : [depslib_dir],
  )

sdl2_incl = include_directories(
  join_paths(depsinclude_dir, 'SDL2/'),
  )

sdl2_dep = declare_dependency(
  include_directories : [sdl2_incl],
  dependencies : [sdl2_lib, threads_dep],
  compile_args : ['-D_REENTRANT'],
  )

# theora -----------------------------------------------------------------------

theora_lib = cxx.find_library(
  'theora',
  dirs : [depslib_dir],
  )

theora_incl = include_directories(
  depsinclude_dir,
  )

theora_dep = declare_dependency(
  include_directories : [theora_incl],
  dependencies : [theora_lib, ogg_dep],
  )

# vorbis -----------------------------------------------------------------------

vorbis_lib = cxx.find_library(
  'vorbis',
  dirs : [depslib_dir],
  )

vorbis_incl = include_directories(
  depsinclude_dir,
  )

vorbis_dep = declare_dependency(
  include_directories : [vorbis_incl],
  dependencies : [vorbis_lib, m_lib, ogg_dep],
  )

vorbisfile_lib = cxx.find_library(
  'vorbisfile',
  dirs : [depslib_dir],
  )

vorbisfile_incl = include_directories(
  depsinclude_dir,
  )

vorbisfile_dep = declare_dependency(
  include_directories : [vorbisfile_incl],
  dependencies : [vorbisfile_lib, vorbis_dep],
  )

# libwebp ----------------------------------------------------------------------

libwebp_lib = cxx.find_library(
  'libwebp',
  dirs : [depslib_dir],
  )

libwebp_incl = include_directories(
  depsinclude_dir,
  )

libwebp_dep = declare_dependency(
  include_directories : [libwebp_incl],
  dependencies : [libwebp_lib, m_lib],
  )

# ------------------------------------------------------------------------------
#  Internal Libraries
# ------------------------------------------------------------------------------

# detour -----------------------------------------------------------------------

detour_srcs = files(
  'daemon/libs/recastnavigation/DebugUtils/Source/DebugDraw.cpp',
  'daemon/libs/recastnavigation/DebugUtils/Source/DetourDebugDraw.cpp',
  'daemon/libs/recastnavigation/DetourCrowd/Source/DetourPathCorridor.cpp',
  'daemon/libs/recastnavigation/Detour/Source/DetourAlloc.cpp',
  'daemon/libs/recastnavigation/Detour/Source/DetourCommon.cpp',
  'daemon/libs/recastnavigation/Detour/Source/DetourNavMeshBuilder.cpp',
  'daemon/libs/recastnavigation/Detour/Source/DetourNavMesh.cpp',
  'daemon/libs/recastnavigation/Detour/Source/DetourNavMeshQuery.cpp',
  'daemon/libs/recastnavigation/Detour/Source/DetourNode.cpp',
  'daemon/libs/recastnavigation/DetourTileCache/Source/DetourTileCacheBuilder.cpp',
  'daemon/libs/recastnavigation/DetourTileCache/Source/DetourTileCache.cpp',
  )

detour_incl = include_directories(
  'daemon/libs/recastnavigation/DebugUtils/Include/',
  'daemon/libs/recastnavigation/DetourCrowd/Include/',
  'daemon/libs/recastnavigation/Detour/Include/',
  'daemon/libs/recastnavigation/DetourTileCache/Include/',
  )

detour_lib = static_library(
  'detour',
  name_prefix : '',
  sources : [detour_srcs],
  include_directories : [daemon_incl, detour_incl],
  pic : true,
  build_by_default : false,
  )

detour_dep = declare_dependency(
  include_directories : [detour_incl],
  link_with : [detour_lib],
  )

# fastlz -----------------------------------------------------------------------

fastlz_srcs = files(
  'daemon/libs/fastlz/fastlz.c',
  )

fastlz_lib = static_library(
  'fastlz',
  name_prefix : '',
  sources : [fastlz_srcs],
  pic : true,
  build_by_default : false,
  )

fastlz_dep = declare_dependency(
  link_with : [fastlz_lib],
  )

# nacl -----------------------------------------------------------------------

nacl_srcs = files(
  'daemon/libs/nacl/native_client/src/shared/imc/nacl_imc_common.cc',

  'daemon/libs/nacl/native_client/src/shared/imc/linux/nacl_imc.cc',  # LINUX ONLY
  'daemon/libs/nacl/native_client/src/shared/imc/posix/nacl_imc_posix.cc',  # LINUX ONLY
  )

nacl_incl = include_directories(
  'daemon/libs/nacl/',
  )

nacl_defs = [  # LINUX ONLY
  '-DNACL_ANDROID=0',
  '-DNACL_LINUX=1',
  '-DNACL_OSX=0',
  '-DNACL_WINDOWS=0',
  '-DNACL_BUILD_ARCH=x86',
  '-DNACL_BUILD_SUBARCH=64',
  ]

nacl_lib = static_library(
  'nacl',
  name_prefix : '',
  sources : [nacl_srcs],
  include_directories : [nacl_incl],
  cpp_args : [nacl_defs],
  pic : true,
  build_by_default : false,
  )

nacl_dep = declare_dependency(
  include_directories : [nacl_incl],
  link_with : [nacl_lib],
  compile_args : nacl_defs,
  )

# openexr -----------------------------------------------------------------------

openexr_srcs = files(
  'daemon/libs/openexr/half.cpp',
  )

openexr_lib = static_library(
  'openexr',
  name_prefix : '',
  sources : [openexr_srcs],
  pic : true,
  build_by_default : false,
  )

openexr_dep = declare_dependency(
  link_with : [openexr_lib],
  )

# minizip -----------------------------------------------------------------------

minizip_srcs = files(
  'daemon/libs/minizip/ioapi.c',
  'daemon/libs/minizip/unzip.c',
  )

minizip_lib = static_library(
  'minizip',
  name_prefix : '',
  sources : [minizip_srcs],
  pic : true,
  build_by_default : false,
  )

minizip_dep = declare_dependency(
  link_with : [minizip_lib],
  )

# mumblelink -------------------------------------------------------------------

mumblelink_srcs = files(
  'daemon/libs/mumblelink/libmumblelink.cpp',
  )

mumblelink_lib = static_library(
  'mumblelink',
  name_prefix : '',
  sources : [mumblelink_srcs],
  pic : true,
  build_by_default : false,
  )

mumblelink_dep = declare_dependency(
  link_with : [mumblelink_lib],
  dependencies : [rt_lib],
  )

# tinygettext ------------------------------------------------------------------

tinygettext_srcs = files(
  'daemon/libs/findlocale/findlocale.c',

  'daemon/libs/tinygettext/dictionary.cpp',
  'daemon/libs/tinygettext/dictionary_manager.cpp',
  'daemon/libs/tinygettext/iconv.cpp',
  'daemon/libs/tinygettext/language.cpp',
  'daemon/libs/tinygettext/log.cpp',
  'daemon/libs/tinygettext/plural_forms.cpp',
  'daemon/libs/tinygettext/po_parser.cpp',
  'daemon/libs/tinygettext/tinygettext.cpp',
  )

tinygettext_lib = static_library(
  'tinygettext',
  name_prefix : '',
  sources : [tinygettext_srcs],
  pic : true,
  build_by_default : false,
  )

tinygettext_dep = declare_dependency(
  link_with : [tinygettext_lib],
  )

# engine -----------------------------------------------------------------------

enginelist_srcs = files(
  'daemon/src/engine/framework/Application.cpp',
  'daemon/src/engine/framework/BaseCommands.cpp',
  'daemon/src/engine/framework/CommandBufferHost.cpp',
  'daemon/src/engine/framework/CommandSystem.cpp',
  'daemon/src/engine/framework/CommonVMServices.cpp',
  'daemon/src/engine/framework/ConsoleField.cpp',
  'daemon/src/engine/framework/ConsoleHistory.cpp',
  'daemon/src/engine/framework/CrashDump.cpp',
  'daemon/src/engine/framework/Crypto.cpp',
  'daemon/src/engine/framework/CvarSystem.cpp',
  'daemon/src/engine/framework/LogSystem.cpp',
  'daemon/src/engine/framework/Network.cpp',
  'daemon/src/engine/framework/Rcon.cpp',
  'daemon/src/engine/framework/Resource.cpp',
  'daemon/src/engine/framework/System.cpp',
  'daemon/src/engine/framework/VirtualMachine.cpp',

  'daemon/src/engine/qcommon/md5.cpp',

  'daemon/src/engine/sys/con_common.cpp',

  # 'daemon/src/engine/sys/con_curses.cpp',  # ncurses

  'daemon/src/engine/sys/con_tty.cpp',  # LINUX ONLY
  )

engine_srcs = [
  commonlist_srcs,
  enginelist_srcs,
  ]

engine_deps = [
  nacl_dep,
  m_lib,
  rt_lib,
  dl_lib,
  threads_dep,
  minizip_dep,
  zlib_dep,
  # ncurses [-DUSE_CURSES]
  ]

engine_lib = static_library(
  'engine',
  name_prefix : '',
  sources : [engine_srcs, revision_h],
  include_directories : [daemon_incl],
  dependencies : [engine_deps],
  cpp_args : ['-DBUILD_ENGINE'],
  pic : true,
  build_by_default : false,
  )

engine_dep = declare_dependency(
  include_directories : [daemon_incl],
  link_with : [engine_lib],
  dependencies : [threads_dep],
  )

# ------------------------------------------------------------------------------
#  GLSL
# ------------------------------------------------------------------------------

include_bin_srcs = files(
  'daemon/libs/include-bin/main.cpp',
  )

include_bin_exe = executable(
  'include-bin',
  sources : [include_bin_srcs],
  build_by_default : false,
  )

glsl_srcs = files(
  'daemon/src/engine/renderer/glsl_source/blurX_fp.glsl',
  'daemon/src/engine/renderer/glsl_source/blurX_vp.glsl',
  'daemon/src/engine/renderer/glsl_source/blurY_fp.glsl',
  'daemon/src/engine/renderer/glsl_source/blurY_vp.glsl',
  'daemon/src/engine/renderer/glsl_source/cameraEffects_fp.glsl',
  'daemon/src/engine/renderer/glsl_source/cameraEffects_vp.glsl',
  'daemon/src/engine/renderer/glsl_source/contrast_fp.glsl',
  'daemon/src/engine/renderer/glsl_source/contrast_vp.glsl',
  'daemon/src/engine/renderer/glsl_source/debugShadowMap_fp.glsl',
  'daemon/src/engine/renderer/glsl_source/debugShadowMap_vp.glsl',
  'daemon/src/engine/renderer/glsl_source/deformVertexes_vp.glsl',
  'daemon/src/engine/renderer/glsl_source/depthtile1_fp.glsl',
  'daemon/src/engine/renderer/glsl_source/depthtile1_vp.glsl',
  'daemon/src/engine/renderer/glsl_source/depthtile2_fp.glsl',
  'daemon/src/engine/renderer/glsl_source/depthtile2_vp.glsl',
  'daemon/src/engine/renderer/glsl_source/depthToColor_fp.glsl',
  'daemon/src/engine/renderer/glsl_source/depthToColor_vp.glsl',
  'daemon/src/engine/renderer/glsl_source/dispersion_C_fp.glsl',
  'daemon/src/engine/renderer/glsl_source/dispersion_C_vp.glsl',
  'daemon/src/engine/renderer/glsl_source/fogGlobal_fp.glsl',
  'daemon/src/engine/renderer/glsl_source/fogGlobal_vp.glsl',
  'daemon/src/engine/renderer/glsl_source/fogQuake3_fp.glsl',
  'daemon/src/engine/renderer/glsl_source/fogQuake3_vp.glsl',
  'daemon/src/engine/renderer/glsl_source/forwardLighting_fp.glsl',
  'daemon/src/engine/renderer/glsl_source/forwardLighting_vp.glsl',
  'daemon/src/engine/renderer/glsl_source/fxaa3_11_fp.glsl',
  'daemon/src/engine/renderer/glsl_source/fxaa_fp.glsl',
  'daemon/src/engine/renderer/glsl_source/fxaa_vp.glsl',
  'daemon/src/engine/renderer/glsl_source/generic_fp.glsl',
  'daemon/src/engine/renderer/glsl_source/generic_vp.glsl',
  'daemon/src/engine/renderer/glsl_source/heatHaze_fp.glsl',
  'daemon/src/engine/renderer/glsl_source/heatHaze_vp.glsl',
  'daemon/src/engine/renderer/glsl_source/lightMapping_fp.glsl',
  'daemon/src/engine/renderer/glsl_source/lightMapping_vp.glsl',
  'daemon/src/engine/renderer/glsl_source/lighttile_fp.glsl',
  'daemon/src/engine/renderer/glsl_source/lighttile_vp.glsl',
  'daemon/src/engine/renderer/glsl_source/lightVolume_omni_fp.glsl',
  'daemon/src/engine/renderer/glsl_source/lightVolume_omni_vp.glsl',
  'daemon/src/engine/renderer/glsl_source/liquid_fp.glsl',
  'daemon/src/engine/renderer/glsl_source/liquid_vp.glsl',
  'daemon/src/engine/renderer/glsl_source/motionblur_fp.glsl',
  'daemon/src/engine/renderer/glsl_source/motionblur_vp.glsl',
  'daemon/src/engine/renderer/glsl_source/portal_fp.glsl',
  'daemon/src/engine/renderer/glsl_source/portal_vp.glsl',
  'daemon/src/engine/renderer/glsl_source/reflection_CB_fp.glsl',
  'daemon/src/engine/renderer/glsl_source/reflection_CB_vp.glsl',
  'daemon/src/engine/renderer/glsl_source/refraction_C_fp.glsl',
  'daemon/src/engine/renderer/glsl_source/refraction_C_vp.glsl',
  'daemon/src/engine/renderer/glsl_source/reliefMapping_fp.glsl',
  'daemon/src/engine/renderer/glsl_source/screen_fp.glsl',
  'daemon/src/engine/renderer/glsl_source/screen_vp.glsl',
  'daemon/src/engine/renderer/glsl_source/shadowFill_fp.glsl',
  'daemon/src/engine/renderer/glsl_source/shadowFill_vp.glsl',
  'daemon/src/engine/renderer/glsl_source/skybox_fp.glsl',
  'daemon/src/engine/renderer/glsl_source/skybox_vp.glsl',
  'daemon/src/engine/renderer/glsl_source/ssao_fp.glsl',
  'daemon/src/engine/renderer/glsl_source/ssao_vp.glsl',
  'daemon/src/engine/renderer/glsl_source/vertexAnimation_vp.glsl',
  'daemon/src/engine/renderer/glsl_source/vertexLighting_DBS_entity_fp.glsl',
  'daemon/src/engine/renderer/glsl_source/vertexLighting_DBS_entity_vp.glsl',
  'daemon/src/engine/renderer/glsl_source/vertexLighting_DBS_world_fp.glsl',
  'daemon/src/engine/renderer/glsl_source/vertexLighting_DBS_world_vp.glsl',
  'daemon/src/engine/renderer/glsl_source/vertexSimple_vp.glsl',
  'daemon/src/engine/renderer/glsl_source/vertexSkinning_vp.glsl',
  'daemon/src/engine/renderer/glsl_source/vertexSprite_vp.glsl',
  'daemon/src/engine/renderer/glsl_source/volumetricFog_fp.glsl',
  'daemon/src/engine/renderer/glsl_source/volumetricFog_vp.glsl',
  )

glsl_h = []

foreach glsl_src : glsl_srcs
  glsl_h += custom_target(
    '@0@'.format(glsl_src).underscorify(),
    input : glsl_src,
    output : '@BASENAME@.h',
    command : [include_bin_exe, '@INPUT@', '@OUTPUT@'],
    )
endforeach

# ------------------------------------------------------------------------------
#  Executables
# ------------------------------------------------------------------------------

daemon_srcs = [
  qcommonlist_srcs,
  serverlist_srcs,
  clientbaselist_srcs,
  clientlist_srcs,
  ]

daemon_deps = [
  engine_dep,

  tinygettext_dep,
  fastlz_dep,
  detour_dep,
  geoip_dep,
  nettle_dep,
  hogweed_dep,
  gmp_dep,

  sdl2_dep,
  openexr_dep,
  ogg_dep,
  vorbis_dep,
  vorbisfile_dep,
  opus_dep,
  opusfile_dep,
  theora_dep,
  libwebp_dep,
  libjpeg_dep,
  libpng_dep,
  freetype2_dep,
  glew_dep,
  openal_dep,
  libcurl_dep,
  mumblelink_dep,
  ]

daemon_exe = executable(
  'daemon',
  sources : [daemon_srcs, revision_h, glsl_h],
  include_directories : [daemon_incl],
  dependencies : [daemon_deps],
  cpp_args : ['-DBUILD_ENGINE', '-DBUILD_GRAPHICAL_CLIENT', '-DUSE_SMP'],
  )

daemonded_srcs = [
  qcommonlist_srcs,
  serverlist_srcs,
  dedserverlist_srcs,
  ]

daemonded_deps = [
  engine_dep,

  tinygettext_dep,
  fastlz_dep,
  detour_dep,
  geoip_dep,
  nettle_dep,
  hogweed_dep,
  gmp_dep,
  ]

daemonded_exe = executable(
  'daemonded',
  sources : [daemonded_srcs],
  include_directories : [daemon_incl],
  dependencies : [daemonded_deps],
  cpp_args : ['-DBUILD_ENGINE', '-DBUILD_SERVER'],
  )

# ==============================================================================
#  Game Modules
# ==============================================================================

sharedlist_srcs = files(
  'daemon/src/shared/CommandBufferClient.cpp',
  'daemon/src/shared/CommonProxies.cpp',
  'daemon/src/shared/VMMain.cpp',
  )

gamesharedlist_srcs = files(
  'src/shared/bg_alloc.cpp',
  'src/shared/bg_misc.cpp',
  'src/shared/bg_parse.cpp',
  'src/shared/bg_pmove.cpp',
  'src/shared/bg_slidemove.cpp',
  'src/shared/bg_utilities.cpp',
  'src/shared/bg_voice.cpp',
  )

cgamelist_srcs = files(
  'src/cgame/cg_animation.cpp',
  'src/cgame/cg_animdelta.cpp',
  'src/cgame/cg_animmapobj.cpp',
  'src/cgame/cg_api.cpp',
  'src/cgame/cg_attachment.cpp',
  'src/cgame/cg_beacon.cpp',
  'src/cgame/cg_consolecmds.cpp',
  'src/cgame/cg_draw.cpp',
  'src/cgame/cg_drawtools.cpp',
  'src/cgame/cg_ents.cpp',
  'src/cgame/cg_event.cpp',
  'src/cgame/cg_gameinfo.cpp',
  'src/cgame/cg_key_name.cpp',
  'src/cgame/cg_main.cpp',
  'src/cgame/cg_marks.cpp',
  'src/cgame/cg_minimap.cpp',
  'src/cgame/cg_parseutils.cpp',
  'src/cgame/cg_particles.cpp',
  'src/cgame/cg_players.cpp',
  'src/cgame/cg_playerstate.cpp',
  'src/cgame/cg_predict.cpp',
  'src/cgame/cg_rocket.cpp',
  'src/cgame/cg_rocket_dataformatter.cpp',
  'src/cgame/cg_rocket_datasource.cpp',
  'src/cgame/cg_rocket_draw.cpp',
  'src/cgame/cg_rocket_events.cpp',
  'src/cgame/cg_rocket_progressbar.cpp',
  'src/cgame/cg_segmented_skeleton.cpp',
  'src/cgame/cg_servercmds.cpp',
  'src/cgame/cg_snapshot.cpp',
  'src/cgame/cg_trails.cpp',
  'src/cgame/cg_tutorial.cpp',
  'src/cgame/cg_utils.cpp',
  'src/cgame/cg_view.cpp',
  'src/cgame/cg_weapons.cpp',

  'src/cgame/CombatFeedback.cpp',

  'src/cgame/rocket/rocket.cpp',
  'src/cgame/rocket/rocket_dataformatter.cpp',
  'src/cgame/rocket/rocket_datagrid.cpp',
  'src/cgame/rocket/rocket_documents.cpp',
  'src/cgame/rocket/rocket_element.cpp',
  'src/cgame/rocket/rocket_events.cpp',
  'src/cgame/rocket/rocket_hud.cpp',
  'src/cgame/rocket/rocket_keys.cpp',

  'src/cgame/rocket/lua/Cmd.cpp',
  'src/cgame/rocket/lua/Cvar.cpp',
  'src/cgame/rocket/lua/Events.cpp',
  'src/cgame/rocket/lua/Timer.cpp',
  )

sharedlistcgame_srcs = files(
  'daemon/src/shared/client/cg_api.cpp',
  )

sgamelist_srcs = files(
  'src/sgame/Beacon.cpp',
  'src/sgame/CombatFeedback.cpp',
  'src/sgame/Entities.cpp',

  'src/sgame/sg_active.cpp',
  'src/sgame/sg_admin.cpp',
  'src/sgame/sg_api.cpp',
  'src/sgame/sg_bot_ai.cpp',
  'src/sgame/sg_bot.cpp',
  'src/sgame/sg_bot_nav.cpp',
  'src/sgame/sg_bot_parse.cpp',
  'src/sgame/sg_bot_util.cpp',
  'src/sgame/sg_client.cpp',
  'src/sgame/sg_cmds.cpp',
  'src/sgame/sg_cm_world.cpp',
  'src/sgame/sg_combat.cpp',
  'src/sgame/sg_entities.cpp',
  'src/sgame/sg_main.cpp',
  'src/sgame/sg_maprotation.cpp',
  'src/sgame/sg_missile.cpp',
  'src/sgame/sg_namelog.cpp',
  'src/sgame/sg_physics.cpp',
  'src/sgame/sg_session.cpp',
  'src/sgame/sg_spawn_afx.cpp',
  'src/sgame/sg_spawn.cpp',
  'src/sgame/sg_spawn_ctrl.cpp',
  'src/sgame/sg_spawn_fx.cpp',
  'src/sgame/sg_spawn_game.cpp',
  'src/sgame/sg_spawn_generic.cpp',
  'src/sgame/sg_spawn_gfx.cpp',
  'src/sgame/sg_spawn_mover.cpp',
  'src/sgame/sg_spawn_position.cpp',
  'src/sgame/sg_spawn_sensor.cpp',
  'src/sgame/sg_spawn_shared.cpp',
  'src/sgame/sg_svcmds.cpp',
  'src/sgame/sg_team.cpp',
  'src/sgame/sg_utils.cpp',
  'src/sgame/sg_weapon.cpp',

  'src/sgame/backend/CBSEBackend.cpp',

  'src/sgame/components/ClientComponent.cpp',
  'src/sgame/components/HealthComponent.cpp',
  'src/sgame/components/KnockbackComponent.cpp',
  'src/sgame/components/SpectatorComponent.cpp',
  'src/sgame/components/TeamComponent.cpp',
  )

sharedlistsgame_srcs = files(
  'daemon/src/shared/server/sg_api.cpp',
  )

# ------------------------------------------------------------------------------
#  Internal Libraries
# ------------------------------------------------------------------------------

# librocket --------------------------------------------------------------------

librocket_srcs = files(
  'libs/libRocket/Source/Core/BaseXMLParser.cpp',
  'libs/libRocket/Source/Core/Box.cpp',
  'libs/libRocket/Source/Core/Clock.cpp',
  'libs/libRocket/Source/Core/Context.cpp',
  'libs/libRocket/Source/Core/ContextInstancer.cpp',
  'libs/libRocket/Source/Core/ContextInstancerDefault.cpp',
  'libs/libRocket/Source/Core/ConvolutionFilter.cpp',
  'libs/libRocket/Source/Core/Core.cpp',
  'libs/libRocket/Source/Core/Decorator.cpp',
  'libs/libRocket/Source/Core/DecoratorInstancer.cpp',
  'libs/libRocket/Source/Core/DecoratorNone.cpp',
  'libs/libRocket/Source/Core/DecoratorNoneInstancer.cpp',
  'libs/libRocket/Source/Core/DecoratorTiledBox.cpp',
  'libs/libRocket/Source/Core/DecoratorTiledBoxInstancer.cpp',
  'libs/libRocket/Source/Core/DecoratorTiled.cpp',
  'libs/libRocket/Source/Core/DecoratorTiledHorizontal.cpp',
  'libs/libRocket/Source/Core/DecoratorTiledHorizontalInstancer.cpp',
  'libs/libRocket/Source/Core/DecoratorTiledImage.cpp',
  'libs/libRocket/Source/Core/DecoratorTiledImageInstancer.cpp',
  'libs/libRocket/Source/Core/DecoratorTiledInstancer.cpp',
  'libs/libRocket/Source/Core/DecoratorTiledVertical.cpp',
  'libs/libRocket/Source/Core/DecoratorTiledVerticalInstancer.cpp',
  'libs/libRocket/Source/Core/Dictionary.cpp',
  'libs/libRocket/Source/Core/DocumentHeader.cpp',
  'libs/libRocket/Source/Core/ElementBackground.cpp',
  'libs/libRocket/Source/Core/ElementBorder.cpp',
  'libs/libRocket/Source/Core/Element.cpp',
  'libs/libRocket/Source/Core/ElementDecoration.cpp',
  'libs/libRocket/Source/Core/ElementDefinition.cpp',
  'libs/libRocket/Source/Core/ElementDocument.cpp',
  'libs/libRocket/Source/Core/ElementHandle.cpp',
  'libs/libRocket/Source/Core/ElementImage.cpp',
  'libs/libRocket/Source/Core/ElementInstancer.cpp',
  'libs/libRocket/Source/Core/ElementReference.cpp',
  'libs/libRocket/Source/Core/ElementScroll.cpp',
  'libs/libRocket/Source/Core/ElementStyleCache.cpp',
  'libs/libRocket/Source/Core/ElementStyle.cpp',
  'libs/libRocket/Source/Core/ElementText.cpp',
  'libs/libRocket/Source/Core/ElementTextDefault.cpp',
  'libs/libRocket/Source/Core/ElementUtilities.cpp',
  'libs/libRocket/Source/Core/Event.cpp',
  'libs/libRocket/Source/Core/EventDispatcher.cpp',
  'libs/libRocket/Source/Core/EventInstancer.cpp',
  'libs/libRocket/Source/Core/EventInstancerDefault.cpp',
  'libs/libRocket/Source/Core/EventListenerInstancer.cpp',
  'libs/libRocket/Source/Core/Factory.cpp',
  'libs/libRocket/Source/Core/FileInterface.cpp',
  'libs/libRocket/Source/Core/FileInterfaceDefault.cpp',
  'libs/libRocket/Source/Core/FontDatabase.cpp',
  'libs/libRocket/Source/Core/FontEffect.cpp',
  'libs/libRocket/Source/Core/FontEffectInstancer.cpp',
  'libs/libRocket/Source/Core/FontEffectNone.cpp',
  'libs/libRocket/Source/Core/FontEffectNoneInstancer.cpp',
  'libs/libRocket/Source/Core/FontEffectOutline.cpp',
  'libs/libRocket/Source/Core/FontEffectOutlineInstancer.cpp',
  'libs/libRocket/Source/Core/FontEffectShadow.cpp',
  'libs/libRocket/Source/Core/FontEffectShadowInstancer.cpp',
  'libs/libRocket/Source/Core/FontFace.cpp',
  'libs/libRocket/Source/Core/FontFaceHandle.cpp',
  'libs/libRocket/Source/Core/FontFaceLayer.cpp',
  'libs/libRocket/Source/Core/FontFamily.cpp',
  'libs/libRocket/Source/Core/Geometry.cpp',
  'libs/libRocket/Source/Core/GeometryDatabase.cpp',
  'libs/libRocket/Source/Core/GeometryUtilities.cpp',
  'libs/libRocket/Source/Core/LayoutBlockBox.cpp',
  'libs/libRocket/Source/Core/LayoutBlockBoxSpace.cpp',
  'libs/libRocket/Source/Core/LayoutEngine.cpp',
  'libs/libRocket/Source/Core/LayoutInlineBox.cpp',
  'libs/libRocket/Source/Core/LayoutInlineBoxText.cpp',
  'libs/libRocket/Source/Core/LayoutLineBox.cpp',
  'libs/libRocket/Source/Core/Log.cpp',
  'libs/libRocket/Source/Core/Math.cpp',
  'libs/libRocket/Source/Core/Plugin.cpp',
  'libs/libRocket/Source/Core/PluginRegistry.cpp',
  'libs/libRocket/Source/Core/precompiled.cpp',
  'libs/libRocket/Source/Core/Property.cpp',
  'libs/libRocket/Source/Core/PropertyDefinition.cpp',
  'libs/libRocket/Source/Core/PropertyDictionary.cpp',
  'libs/libRocket/Source/Core/PropertyParserColour.cpp',
  'libs/libRocket/Source/Core/PropertyParserKeyword.cpp',
  'libs/libRocket/Source/Core/PropertyParserNumber.cpp',
  'libs/libRocket/Source/Core/PropertyParserString.cpp',
  'libs/libRocket/Source/Core/PropertySpecification.cpp',
  'libs/libRocket/Source/Core/ReferenceCountable.cpp',
  'libs/libRocket/Source/Core/RenderInterface.cpp',
  'libs/libRocket/Source/Core/Stream.cpp',
  'libs/libRocket/Source/Core/StreamFile.cpp',
  'libs/libRocket/Source/Core/StreamMemory.cpp',
  'libs/libRocket/Source/Core/StringCache.cpp',
  'libs/libRocket/Source/Core/String.cpp',
  'libs/libRocket/Source/Core/StringUtilities.cpp',
  'libs/libRocket/Source/Core/StyleSheet.cpp',
  'libs/libRocket/Source/Core/StyleSheetFactory.cpp',
  'libs/libRocket/Source/Core/StyleSheetNode.cpp',
  'libs/libRocket/Source/Core/StyleSheetNodeSelector.cpp',
  'libs/libRocket/Source/Core/StyleSheetNodeSelectorEmpty.cpp',
  'libs/libRocket/Source/Core/StyleSheetNodeSelectorFirstChild.cpp',
  'libs/libRocket/Source/Core/StyleSheetNodeSelectorFirstOfType.cpp',
  'libs/libRocket/Source/Core/StyleSheetNodeSelectorLastChild.cpp',
  'libs/libRocket/Source/Core/StyleSheetNodeSelectorLastOfType.cpp',
  'libs/libRocket/Source/Core/StyleSheetNodeSelectorNthChild.cpp',
  'libs/libRocket/Source/Core/StyleSheetNodeSelectorNthLastChild.cpp',
  'libs/libRocket/Source/Core/StyleSheetNodeSelectorNthLastOfType.cpp',
  'libs/libRocket/Source/Core/StyleSheetNodeSelectorNthOfType.cpp',
  'libs/libRocket/Source/Core/StyleSheetNodeSelectorOnlyChild.cpp',
  'libs/libRocket/Source/Core/StyleSheetNodeSelectorOnlyOfType.cpp',
  'libs/libRocket/Source/Core/StyleSheetParser.cpp',
  'libs/libRocket/Source/Core/StyleSheetSpecification.cpp',
  'libs/libRocket/Source/Core/SystemInterface.cpp',
  'libs/libRocket/Source/Core/TemplateCache.cpp',
  'libs/libRocket/Source/Core/Template.cpp',
  'libs/libRocket/Source/Core/Texture.cpp',
  'libs/libRocket/Source/Core/TextureDatabase.cpp',
  'libs/libRocket/Source/Core/TextureLayout.cpp',
  'libs/libRocket/Source/Core/TextureLayoutRectangle.cpp',
  'libs/libRocket/Source/Core/TextureLayoutRow.cpp',
  'libs/libRocket/Source/Core/TextureLayoutTexture.cpp',
  'libs/libRocket/Source/Core/TextureResource.cpp',
  'libs/libRocket/Source/Core/UnicodeRange.cpp',
  'libs/libRocket/Source/Core/URL.cpp',
  'libs/libRocket/Source/Core/Variant.cpp',
  'libs/libRocket/Source/Core/WidgetSlider.cpp',
  'libs/libRocket/Source/Core/WidgetSliderScroll.cpp',
  'libs/libRocket/Source/Core/WString.cpp',
  'libs/libRocket/Source/Core/XMLNodeHandlerBody.cpp',
  'libs/libRocket/Source/Core/XMLNodeHandler.cpp',
  'libs/libRocket/Source/Core/XMLNodeHandlerDefault.cpp',
  'libs/libRocket/Source/Core/XMLNodeHandlerHead.cpp',
  'libs/libRocket/Source/Core/XMLNodeHandlerTemplate.cpp',
  'libs/libRocket/Source/Core/XMLParser.cpp',
  'libs/libRocket/Source/Core/XMLParseTools.cpp',

  'libs/libRocket/Source/Core/Lua/Colourb.cpp',
  'libs/libRocket/Source/Core/Lua/Colourf.cpp',
  'libs/libRocket/Source/Core/Lua/Context.cpp',
  'libs/libRocket/Source/Core/Lua/ContextDocumentsProxy.cpp',
  'libs/libRocket/Source/Core/Lua/Document.cpp',
  'libs/libRocket/Source/Core/Lua/ElementAttributesProxy.cpp',
  'libs/libRocket/Source/Core/Lua/ElementChildNodesProxy.cpp',
  'libs/libRocket/Source/Core/Lua/Element.cpp',
  'libs/libRocket/Source/Core/Lua/ElementInstancer.cpp',
  'libs/libRocket/Source/Core/Lua/ElementStyleProxy.cpp',
  'libs/libRocket/Source/Core/Lua/ElementText.cpp',
  'libs/libRocket/Source/Core/Lua/Event.cpp',
  'libs/libRocket/Source/Core/Lua/EventParametersProxy.cpp',
  'libs/libRocket/Source/Core/Lua/GlobalLuaFunctions.cpp',
  'libs/libRocket/Source/Core/Lua/Interpreter.cpp',
  'libs/libRocket/Source/Core/Lua/Log.cpp',
  'libs/libRocket/Source/Core/Lua/LuaDocument.cpp',
  'libs/libRocket/Source/Core/Lua/LuaDocumentElementInstancer.cpp',
  'libs/libRocket/Source/Core/Lua/LuaElementInstancer.cpp',
  'libs/libRocket/Source/Core/Lua/LuaEventListener.cpp',
  'libs/libRocket/Source/Core/Lua/LuaEventListenerInstancer.cpp',
  'libs/libRocket/Source/Core/Lua/precompiled.cpp',
  'libs/libRocket/Source/Core/Lua/RocketContextsProxy.cpp',
  'libs/libRocket/Source/Core/Lua/Rocket.cpp',
  'libs/libRocket/Source/Core/Lua/Utilities.cpp',
  'libs/libRocket/Source/Core/Lua/Vector2f.cpp',
  'libs/libRocket/Source/Core/Lua/Vector2i.cpp',

  'libs/libRocket/Source/Controls/Clipboard.cpp',
  'libs/libRocket/Source/Controls/Controls.cpp',
  'libs/libRocket/Source/Controls/DataFormatter.cpp',
  'libs/libRocket/Source/Controls/DataQuery.cpp',
  'libs/libRocket/Source/Controls/DataSource.cpp',
  'libs/libRocket/Source/Controls/DataSourceListener.cpp',
  'libs/libRocket/Source/Controls/ElementDataGridCell.cpp',
  'libs/libRocket/Source/Controls/ElementDataGrid.cpp',
  'libs/libRocket/Source/Controls/ElementDataGridExpandButton.cpp',
  'libs/libRocket/Source/Controls/ElementDataGridRow.cpp',
  'libs/libRocket/Source/Controls/ElementFormControl.cpp',
  'libs/libRocket/Source/Controls/ElementFormControlDataSelect.cpp',
  'libs/libRocket/Source/Controls/ElementFormControlInput.cpp',
  'libs/libRocket/Source/Controls/ElementFormControlSelect.cpp',
  'libs/libRocket/Source/Controls/ElementFormControlTextArea.cpp',
  'libs/libRocket/Source/Controls/ElementForm.cpp',
  'libs/libRocket/Source/Controls/ElementTabSet.cpp',
  'libs/libRocket/Source/Controls/ElementTextSelection.cpp',
  'libs/libRocket/Source/Controls/InputTypeButton.cpp',
  'libs/libRocket/Source/Controls/InputTypeCheckbox.cpp',
  'libs/libRocket/Source/Controls/InputType.cpp',
  'libs/libRocket/Source/Controls/InputTypeRadio.cpp',
  'libs/libRocket/Source/Controls/InputTypeRange.cpp',
  'libs/libRocket/Source/Controls/InputTypeSubmit.cpp',
  'libs/libRocket/Source/Controls/InputTypeText.cpp',
  'libs/libRocket/Source/Controls/SelectOption.cpp',
  'libs/libRocket/Source/Controls/WidgetDropDown.cpp',
  'libs/libRocket/Source/Controls/WidgetSlider.cpp',
  'libs/libRocket/Source/Controls/WidgetSliderInput.cpp',
  'libs/libRocket/Source/Controls/WidgetTextInput.cpp',
  'libs/libRocket/Source/Controls/WidgetTextInputMultiLine.cpp',
  'libs/libRocket/Source/Controls/WidgetTextInputSingleLine.cpp',
  'libs/libRocket/Source/Controls/WidgetTextInputSingleLinePassword.cpp',
  'libs/libRocket/Source/Controls/XMLNodeHandlerDataGrid.cpp',
  'libs/libRocket/Source/Controls/XMLNodeHandlerTabSet.cpp',
  'libs/libRocket/Source/Controls/XMLNodeHandlerTextArea.cpp',

  'libs/libRocket/Source/Controls/Lua/Controls.cpp',
  'libs/libRocket/Source/Controls/Lua/DataFormatter.cpp',
  'libs/libRocket/Source/Controls/Lua/DataSource.cpp',
  'libs/libRocket/Source/Controls/Lua/ElementDataGrid.cpp',
  'libs/libRocket/Source/Controls/Lua/ElementDataGridRow.cpp',
  'libs/libRocket/Source/Controls/Lua/ElementFormControl.cpp',
  'libs/libRocket/Source/Controls/Lua/ElementFormControlDataSelect.cpp',
  'libs/libRocket/Source/Controls/Lua/ElementFormControlInput.cpp',
  'libs/libRocket/Source/Controls/Lua/ElementFormControlSelect.cpp',
  'libs/libRocket/Source/Controls/Lua/ElementFormControlTextArea.cpp',
  'libs/libRocket/Source/Controls/Lua/ElementForm.cpp',
  'libs/libRocket/Source/Controls/Lua/ElementTabSet.cpp',
  'libs/libRocket/Source/Controls/Lua/LuaDataFormatter.cpp',
  'libs/libRocket/Source/Controls/Lua/LuaDataSource.cpp',
  'libs/libRocket/Source/Controls/Lua/SelectOptionsProxy.cpp',

  'libs/libRocket/Source/Debugger/Debugger.cpp',
  'libs/libRocket/Source/Debugger/ElementContextHook.cpp',
  'libs/libRocket/Source/Debugger/ElementInfo.cpp',
  'libs/libRocket/Source/Debugger/ElementLog.cpp',
  'libs/libRocket/Source/Debugger/Geometry.cpp',
  'libs/libRocket/Source/Debugger/Plugin.cpp',
  'libs/libRocket/Source/Debugger/SystemInterface.cpp',
  )

librocket_incl = include_directories(
  'libs/libRocket/Include/',
  )

librocket_lib = static_library(
  'librocket',
  name_prefix : '',
  sources : [librocket_srcs],
  include_directories : [librocket_incl],
  dependencies : [freetype2_dep, lua_dep],
  cpp_args : ['-DROCKET_STATIC_LIB', '-DLUA_COMPAT_APIINTCASTS'],
  pic : true,
  build_by_default : false,
  )

librocket_dep = declare_dependency(
  include_directories : [librocket_incl],
  link_with : [librocket_lib],
  dependencies : [freetype2_dep, lua_dep],
  )

# ------------------------------------------------------------------------------
#  Modules
# ------------------------------------------------------------------------------

cgame_srcs = [
  cgamelist_srcs,
  gamesharedlist_srcs,
  sharedlist_srcs,
  sharedlistcgame_srcs,
  commonlist_srcs,
  ]

cgame_incl = include_directories(
  'src/',
  'daemon/libs/',
  'daemon/src/',
  )

cgame_deps = [
  librocket_dep,

  nacl_dep,
  m_lib,
  rt_lib,
  dl_lib,
  threads_dep,
  minizip_dep,
  zlib_dep,
  ]

cgame_native_dll_lib = shared_module(
  'cgame-native-dll',
  name_prefix : '',
  sources : [cgame_srcs, revision_h],
  include_directories : [cgame_incl],
  dependencies : [cgame_deps],
  cpp_args : ['-DVM_NAME=cgame', '-DBUILD_CGAME', '-DROCKET_STATIC_LIB', '-DBUILD_VM', '-DBUILD_VM_IN_PROCESS'],
  )

sgame_srcs = [
  sgamelist_srcs,
  gamesharedlist_srcs,
  sharedlist_srcs,
  sharedlistsgame_srcs,
  commonlist_srcs,
  ]

sgame_incl = include_directories(
  'src/',
  'daemon/libs/',
  'daemon/src/',
  )

sgame_deps = [
  nacl_dep,
  m_lib,
  rt_lib,
  dl_lib,
  threads_dep,
  minizip_dep,
  zlib_dep,
  ]

sgame_native_dll_lib = shared_module(
  'sgame-native-dll',
  name_prefix : '',
  sources : [sgame_srcs, revision_h],
  include_directories : [sgame_incl],
  dependencies : [sgame_deps],
  cpp_args : ['-DVM_NAME=sgame', '-DBUILD_SGAME', '-DBUILD_VM', '-DBUILD_VM_IN_PROCESS'],
  )
