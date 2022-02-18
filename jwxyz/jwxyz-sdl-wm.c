
#include <SDL2/SDL_syswm.h>

void *get_native_window(SDL_Window *window, int width, int height) {
  SDL_SysWMinfo sysInfo;
  SDL_VERSION(&sysInfo.version);
  SDL_GetWindowWMInfo(window, &sysInfo);

  switch (sysInfo.subsystem) {
      case SDL_SYSWM_WAYLAND:
          printf("Window: wayland\n");
		      return  wl_egl_window_create(sysInfo.info.wl.surface, width, height);
      case SDL_SYSWM_X11:
          printf("Window: x11\n");
		      return sysInfo.info.x11.window;
      default:
          printf("Not supported: %i\n", sysInfo.subsystem);
          return NULL;
  }
};

void *get_native_display(SDL_Window *window) {
  SDL_SysWMinfo sysInfo;
  SDL_VERSION(&sysInfo.version);
  SDL_GetWindowWMInfo(window, &sysInfo);

  switch (sysInfo.subsystem) {
      case SDL_SYSWM_WAYLAND:
          printf("Display: wayland\n");
          return sysInfo.info.wl.display;
          break;
      case SDL_SYSWM_X11:
          printf("Display: x11\n");
          return sysInfo.info.x11.display;
      default:
          printf("Not supported: %i\n", sysInfo.subsystem);
          return NULL;
  }
};