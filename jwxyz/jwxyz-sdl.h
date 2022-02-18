#include "jwxyz.h"

#include "screenhackI.h"
#include "jwxyzI.h"

#include <SDL2/SDL.h>
#include <EGL/egl.h>
#include <GLES/gl.h>
#include <GLES/glext.h>
#include <SDL2/SDL_video.h>
#include "jwzglesI.h"

#define DEFAULT_VISUAL	-1
#define GL_VISUAL	-6

typedef struct dict_t {
    char *key;
    char *value;
    struct dict_t *next;
} dict_t;

dict_t *dict_add(dict_t *dict, char *key, char *value);
dict_t *dict_del(dict_t *dict, char *key);
char *dict_get(dict_t *dict, char *key);

struct jwxyz_Drawable {
  enum { WINDOW, PIXMAP } type;
  XRectangle frame;
  union {
    /* JWXYZ_GL */
    EGLSurface egl_surface;
    GLuint texture; /* If this is 0, it's the default framebuffer. */

    /* JWXYZ_IMAGE */
    void *image_data;
  };
  union {
    struct {
      struct running_hack *rh;
      int last_mouse_x, last_mouse_y;
    } window;
    struct {
      int depth;
    } pixmap;
  };
};

struct running_hack {
  struct xscreensaver_function_table *xsft;
  fps_state *fpst;

  Display *dpy;
  Window window;
  void *closure;
  Bool initted_p;

  SDL_Window *sdl_window;

  EGLContext egl_ctx;
  EGLSurface egl_surface;
  EGLDisplay egl_display;
  EGLConfig egl_config;
  GLint fb_default;

  EGLNativeDisplayType native_display;
  EGLNativeWindowType native_window;
  
  EGLint num_configs;

  dict_t *dict;
  
  struct jwxyz_Drawable frontbuffer;
  GC copy_gc;
  GLuint fb_pixmap;

  Drawable current_drawable;

  jwzgles_state *gles_state;

  struct event_queue *event_queue;
};

void resize(struct running_hack *rh, int w, int h);
long draw (struct running_hack *rh);
void *get_native_window(SDL_Window *window, int width, int height);
void *get_native_display(SDL_Window *window);