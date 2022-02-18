project('samples', 'c')

global_args = ['-DGETTIMEOFDAY_TWO_ARGS', '-DHAVE_JWXYZ', '-DJWXYZ_GL', '-DHAVE_GL', '-DHAVE_JWZGLES', '-DHAVE_GLES', '-DHAVE_SDL', '-DHAVE_EGL']

jwxyz_inc = include_directories('jwxyz')
hacks_inc = include_directories('hacks')
cc = meson.get_compiler('c')
m_dep = cc.find_library('m')

wayland_dep = dependency('wayland-egl')
gl_dep = dependency('gl')
egl_dep = dependency('egl')
gles1_dep = dependency('glesv1_cm')
gles2_dep = dependency('glesv2')

subdir('utils')
subdir('jwxyz')

subdir('hacks')