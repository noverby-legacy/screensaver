
fps_lib = static_library('fps', ['fps.c'], c_args: global_args, dependencies: [jwxyz_dep, utils_dep])

emscripten = false

args = global_args
libs = [fps_lib]
deps = [jwxyz_dep, utils_dep, m_dep, wayland_dep, egl_dep, gles1_dep, gles2_dep]

if emscripten
	#name_suffix = 'js'
	#args += ['-s', 'USE_SDL=2']
	#deps += []
	## Configure index.html
	#conf_data = configuration_data()
	#conf_data.set('exe_name', 'hello-sdl')
	#configure_file(input : 'index.html.in',
	#               output : 'index.html',
	#               configuration : conf_data)
else
	name_suffix = []
  args += []
	deps += [dependency('sdl2')]
endif

lib_name = 'greynetic'
greynetic_lib = static_library(lib_name, 'greynetic.c', name_suffix: name_suffix, c_args: args, link_args: args, dependencies: deps, link_with: libs)

hacks = [greynetic_lib]


executable('screenhack', 'screenhack-sdl.c', name_suffix: name_suffix, c_args: args, link_args: args, dependencies: deps, link_with: hacks)

#args = ['-s', 'USE_SDL=2', '-DHAVE_JWXYZ=1', '-DHAVE_EMSCRIPTEN']
#executable('greynetic', 'greynetic.c', name_suffix:'js', c_args:args,  link_args:args, include_directories: [jwxyz_inc, utils_inc])


