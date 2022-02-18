args = global_args
utils_lib = static_library('jwxyz', ['resources.c', 'yarandom.c', 'pow2.c', 'utf8wc.c', 'xft.c', 'font-retry.c', 'usleep.c', 'colors.c', 'hsv.c'], c_args: args, include_directories: jwxyz_inc)
utils_inc = include_directories('.')
utils_dep = declare_dependency(link_with: utils_lib, include_directories: utils_inc)