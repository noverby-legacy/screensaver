#include "jwxyz-sdl.h"
#include "jwxyz.h"
#include "stdio.h"
#include "usleep.h"
#include <EGL/egl.h>

extern struct xscreensaver_function_table *xscreensaver_function_table;

const char *progname;  /* used by hacks in error messages */
const char *progclass; /* used by ../utils/resources.c */
Bool mono_p;           /* used by hacks */

struct event_queue {
  XEvent event;
  struct event_queue *next;
};

int CheckEGLErrors(const char *file, int line) {
  EGLenum error;
  const char *errortext = "unknown";
  error = eglGetError();
  switch (error) {
  case EGL_SUCCESS:
  case 0:
    return 0;
  case EGL_NOT_INITIALIZED:
    errortext = "EGL_NOT_INITIALIZED";
    break;
  case EGL_BAD_ACCESS:
    errortext = "EGL_BAD_ACCESS";
    break;
  case EGL_BAD_ALLOC:
    errortext = "EGL_BAD_ALLOC";
    break;
  case EGL_BAD_ATTRIBUTE:
    errortext = "EGL_BAD_ATTRIBUTE";
    break;
  case EGL_BAD_CONTEXT:
    errortext = "EGL_BAD_CONTEXT";
    break;
  case EGL_BAD_CONFIG:
    errortext = "EGL_BAD_CONFIG";
    break;
  case EGL_BAD_CURRENT_SURFACE:
    errortext = "EGL_BAD_CURRENT_SURFACE";
    break;
  case EGL_BAD_DISPLAY:
    errortext = "EGL_BAD_DISPLAY";
    break;
  case EGL_BAD_SURFACE:
    errortext = "EGL_BAD_SURFACE";
    break;
  case EGL_BAD_MATCH:
    errortext = "EGL_BAD_MATCH";
    break;
  case EGL_BAD_PARAMETER:
    errortext = "EGL_BAD_PARAMETER";
    break;
  case EGL_BAD_NATIVE_PIXMAP:
    errortext = "EGL_BAD_NATIVE_PIXMAP";
    break;
  case EGL_BAD_NATIVE_WINDOW:
    errortext = "EGL_BAD_NATIVE_WINDOW";
    break;
  default:
    errortext = "unknown";
    break;
  }
  printf("ERROR: EGL Error %s detected in file %s at line %d (0x%X)\n",
         errortext, file, line, error);
  return 1;
}

#define EGL_ERROR(str, check)                                                  \
  {                                                                            \
    if (check)                                                                 \
      CheckEGLErrors(__FILE__, __LINE__);                                      \
    printf("EGL ERROR: " str "\n");                                            \
    return False;                                                              \
  }

void jwxyz_logv(Bool error, const char *fmt, va_list args) {
  vprintf(fmt, args);
}

void jwxyz_abort(const char *fmt, ...) {
  if (!fmt || !*fmt)
    fmt = "abort";

  va_list args;
  va_start(args, fmt);
  jwxyz_logv(True, fmt, args);
  va_end(args);

  fflush(stdout);
  fflush(stderr);
  _exit(1);
}

void jwxyz_get_pos(Window w, XPoint *xvpos, XPoint *xp) {
  Assert(False, "get_pos stub");
}

const XRectangle *jwxyz_frame(Drawable d) { return &d->frame; }

float jwxyz_scale(Window main_window) { Assert(False, "scale stub"); }

void jwxyz_render_text(Display *dpy, void *native_font, const char *str,
                       size_t len, Bool utf8, Bool antialias_p, XCharStruct *cs,
                       char **pixmap_ret) {
  Assert(False, "render_text stub");
}

unsigned int jwxyz_drawable_depth(Drawable d) {
  return (d->type == WINDOW ? visual_depth(NULL, NULL) : d->pixmap.depth);
}

void *jwxyz_load_native_font(Window window, int traits_jwxyz, int mask_jwxyz,
                             const char *font_name_ptr, size_t font_name_length,
                             int font_name_type, float size,
                             char **family_name_ret, int *ascent_ret,
                             int *descent_ret) {
  Assert(False, "load_native_font stub");
}

void jwxyz_release_native_font(Display *dpy, void *native_font) {
  Assert(False, "release_native_font stub");
}

const char *jwxyz_default_font_family(int require) {
  Assert(False, "default_font_family stub");
}

Pixmap XCreatePixmap(Display *dpy, Drawable d, unsigned int width,
                     unsigned int height, unsigned int depth) {
  Assert(False, "XCreatePixmap stub");
}

Bool get_boolean_resource(Display *dpy, char *res_name, char *res_class) {
  char *tmp, buf[100];
  char *s = get_string_resource(dpy, res_name, res_class);
  char *os = s;
  if (!s)
    return 0;
  for (tmp = buf; *s; s++)
    *tmp++ = isupper(*s) ? _tolower(*s) : *s;
  *tmp = 0;
  free(os);

  while (*buf && (buf[strlen(buf) - 1] == ' ' || buf[strlen(buf) - 1] == '\t'))
    buf[strlen(buf) - 1] = 0;

  if (!strcmp(buf, "on") || !strcmp(buf, "true") || !strcmp(buf, "yes"))
    return 1;
  if (!strcmp(buf, "off") || !strcmp(buf, "false") || !strcmp(buf, "no"))
    return 0;
  fprintf(stderr, "%s: %s must be boolean, not %s.\n", progname, res_name, buf);
  return 0;
}

int get_integer_resource(Display *dpy, char *res_name, char *res_class) {
  int val;
  char c, *s = get_string_resource(dpy, res_name, res_class);
  char *ss = s;
  if (!s)
    return 0;

  while (*ss && *ss <= ' ')
    ss++; /* skip whitespace */

  if (ss[0] == '0' && (ss[1] == 'x' || ss[1] == 'X')) /* 0x: parse as hex */
  {
    if (1 == sscanf(ss + 2, "%x %c", (unsigned int *)&val, &c)) {
      free(s);
      return val;
    }
  } else /* else parse as dec */
  {
    /* Allow integer values to end in ".0". */
    int L = strlen(ss);
    if (L > 2 && ss[L - 2] == '.' && ss[L - 1] == '0')
      ss[L - 2] = 0;

    if (1 == sscanf(ss, "%d %c", &val, &c)) {
      free(s);
      return val;
    }
  }

  fprintf(stderr, "%s: %s must be an integer, not %s.\n", progname, res_name,
          s);
  free(s);
  return 0;
}

char *dict_get(dict_t *dict, char *key) {
  for (dict_t *ptr = dict; ptr != NULL; ptr = ptr->next) {
    if (strcmp(ptr->key, key) == 0) {
      return ptr->value;
    }
  }

  return NULL;
}

dict_t *dict_del(dict_t *dict, char *key) {
  dict_t *ptr, *prev, *res;
  for (ptr = dict, prev = NULL; ptr != NULL; prev = ptr, ptr = ptr->next) {
    if (strcmp(ptr->key, key) == 0) {
      if (ptr->next != NULL) {
        if (prev == NULL) {
          res = ptr->next;
        } else {
          prev->next = ptr->next;
        }
      } else if (prev != NULL) {
        prev->next = NULL;
      } else {
        res = NULL;
      }

      free(ptr->key);
      free(ptr->value);
      free(ptr);

      return res;
    }
  }
  return dict;
}

dict_t *dict_add(dict_t *dict, char *key, char *value) {
  dict = dict_del(dict, key);
  dict_t *d = malloc(sizeof(dict_t));
  d->key = strdup(key);
  d->value = strdup(value);
  d->next = dict;
  return d;
}

char *get_string_resource(Display *dpy, char *res_name, char *res_class) {
  Window window = RootWindow(dpy, 0);
  char *res = dict_get(window->window.rh->dict, res_name);
  return res ? strdup(res): res;
}

static char *get_string_resource_window(Window window, char *name) {
  return dict_get(window->window.rh->dict, name);
}

void check_gl_error(const char *type) {
  char buf[100];
  GLenum i;
  const char *e;
  switch ((i = glGetError())) {
  case GL_NO_ERROR:
    return;
  case GL_INVALID_ENUM:
    e = "invalid enum";
    break;
  case GL_INVALID_VALUE:
    e = "invalid value";
    break;
  case GL_INVALID_OPERATION:
    e = "invalid operation";
    break;
  case GL_STACK_OVERFLOW:
    e = "stack overflow";
    break;
  case GL_STACK_UNDERFLOW:
    e = "stack underflow";
    break;
  case GL_OUT_OF_MEMORY:
    e = "out of memory";
    break;
#ifdef GL_TABLE_TOO_LARGE_EXT
  case GL_TABLE_TOO_LARGE_EXT:
    e = "table too large";
    break;
#endif
#ifdef GL_TEXTURE_TOO_LARGE_EXT
  case GL_TEXTURE_TOO_LARGE_EXT:
    e = "texture too large";
    break;
#endif
  default:
    e = buf;
    sprintf(buf, "unknown GL error 0x%x", (int)i);
    break;
  }
  printf("%.50s: %.50s\n", type, e);
}

void jwxyz_assert_gl(void) { check_gl_error("jwxyz_assert_gl"); }

void jwxyz_assert_drawable(Window main_window, Drawable d) {
  check_gl_error("jwxyz_assert_drawable");
}

static void finish_bind_drawable(Display *dpy, Drawable dst) {
  jwxyz_assert_gl();

  glViewport(0, 0, dst->frame.width, dst->frame.height);
  jwxyz_set_matrices(dpy, dst->frame.width, dst->frame.height, False);
}

void jwxyz_bind_drawable(Display *dpy, Window w, Drawable d) {
  struct running_hack *rh = w->window.rh;
  if (rh->current_drawable != d) {
    eglMakeCurrent(rh->egl_display, d->egl_surface, d->egl_surface,
                   rh->egl_ctx);
    finish_bind_drawable(dpy, d);
    rh->current_drawable = d;
  }
}

void jwxyz_gl_copy_area(Display *dpy, Drawable src, Drawable dst, GC gc,
                        int src_x, int src_y, unsigned int width,
                        unsigned int height, int dst_x, int dst_y) {
  Assert(False, "jwxyz_gl_copy_area stub");
}

static void
screenhack_do_fps (Display *dpy, Window w, fps_state *fpst, void *closure)
{
  fps_compute (fpst, 0, -1);
  fps_draw (fpst);
}

static int send_event(struct running_hack *rh, XEvent *e) {
  // Assumes mutex is locked and context is prepared

  int *xP = 0, *yP = 0;
  switch (e->xany.type) {
  case ButtonPress:
  case ButtonRelease:
    xP = &e->xbutton.x;
    yP = &e->xbutton.y;
    break;
  case MotionNotify:
    xP = &e->xmotion.x;
    yP = &e->xmotion.y;
    break;
  }

  return (rh->xsft->event_cb
              ? rh->xsft->event_cb(rh->dpy, rh->window, rh->closure, e)
              : 0);
}

static void send_queued_events(struct running_hack *rh) {
  struct event_queue *event, *next;
  if (!rh->event_queue)
    return;
  for (event = rh->event_queue, next = event->next; event;
       event = next, next = (event ? event->next : 0)) {
    if (!send_event(rh, &event->event)) {
      // #### flash the screen or something
    }
    free(event);
  }
  rh->event_queue = 0;
}

static void prepare_context(struct running_hack *rh) {
  /* TODO: Adreno recommends against doing this every frame. */
  Assert(eglMakeCurrent(rh->egl_display, rh->egl_surface, rh->egl_surface,
                        rh->egl_ctx),
         "eglMakeCurrent failed");

  rh->current_drawable = NULL;

  if (rh->xsft->visual == GL_VISUAL)
    jwzgles_make_current(rh->gles_state);
}

void resize(struct running_hack *rh, int w, int h) 
{
  prepare_context(rh);

  glViewport(0, 0, w, h);

  Window wnd = rh->window;
  wnd->frame.x = 0;
  wnd->frame.y = 0;
  wnd->frame.width = w;
  wnd->frame.height = h;

  jwxyz_window_resized(rh->dpy);

  if (rh->initted_p)
    rh->xsft->reshape_cb(rh->dpy, rh->window, rh->closure, wnd->frame.width,
                         wnd->frame.height);
}

long draw(struct running_hack *rh) {
  unsigned long delay = 0;

  prepare_context(rh);

  if (!rh->initted_p) {

    void *(*init_cb)(Display *, Window, void *) =
        (void *(*)(Display *, Window, void *))rh->xsft->init_cb;

    
    if (rh->xsft->visual == DEFAULT_VISUAL) {
      unsigned int bg =
          get_pixel_resource(rh->dpy, 0, "background", "Background");
      XSetWindowBackground(rh->dpy, rh->window, bg);
      XClearWindow(rh->dpy, rh->window);
    }
    
    rh->closure = init_cb(rh->dpy, rh->window, rh->xsft->setup_arg);

    if (get_boolean_resource (rh->dpy, "doFPS", "DoFPS")) {
      rh->fpst = fps_init (rh->dpy, rh->window);
      if (! rh->xsft->fps_cb) rh->xsft->fps_cb = screenhack_do_fps;
    } else {
      rh->fpst = NULL;
    }

    rh->initted_p = True;
  }

  send_queued_events(rh);

  delay = rh->xsft->draw_cb(rh->dpy, rh->window, rh->closure);

  if (rh->fpst && rh->xsft->fps_cb)
    rh->xsft->fps_cb (rh->dpy, rh->window, rh->fpst, rh->closure);

  //glViewport(0, 0, rh->window->frame.height, rh->window->frame.width);
  //glClearColor(0.5f, 0.5f, 0.f, 1.f);
  //glClear(GL_COLOR_BUFFER_BIT);

  jwxyz_gl_flush(rh->dpy);

  if (!eglSwapBuffers(rh->egl_display, rh->egl_surface)) {
    check_gl_error("eglSwapBuffers failed");
  }

  do {
    unsigned long quantum = 33333;  /* 30 fps */
    if (quantum > delay) 
      quantum = delay;
    delay -= quantum;
    if (quantum > 0)
        usleep (quantum);
  } while (delay > 0);
  return delay;
}

const struct {
  const char *key, *val;
} default_defaults[] = {
    {"texFontCacheSize", "30"},
    {"textMode", "date"},
    {"textURL",
     "https://en.wikipedia.org/w/index.php?title=Special:NewPages&feed=rss"},
    {"grabDesktopImages", "True"},
    {"chooseRandomImages", "True"},
};


#define DEFAULT_WIDTH 800
#define DEFAULT_HEIGHT 600

void
get_egl_config (Window window,  EGLDisplay egl_display, EGLConfig *egl_config)
{
# define COMMON \
    EGL_SURFACE_TYPE,      EGL_WINDOW_BIT, \
    EGL_COLOR_BUFFER_TYPE, EGL_RGB_BUFFER
# define R EGL_RED_SIZE
# define G EGL_GREEN_SIZE
# define B EGL_BLUE_SIZE
# define A EGL_ALPHA_SIZE
# define D EGL_DEPTH_SIZE
# define I EGL_BUFFER_SIZE
# define ST EGL_STENCIL_SIZE
# define SB EGL_SAMPLE_BUFFERS
# define SM EGL_SAMPLES

  const EGLint templates[][40] = {
   { COMMON, R,8, G,8, B,8, A,8, D,8, ST,1, EGL_NONE }, /* rgba stencil */
   { COMMON, R,8, G,8, B,8,      D,8, ST,1, EGL_NONE }, /* rgb  stencil */
   { COMMON, R,4, G,4, B,4,      D,4, ST,1, EGL_NONE },
   { COMMON, R,2, G,2, B,2,      D,2, ST,1, EGL_NONE },
   { COMMON, R,8, G,8, B,8, A,8, D,8,       EGL_NONE }, /* rgba */
   { COMMON, R,8, G,8, B,8,      D,8,       EGL_NONE }, /* rgb  */
   { COMMON, R,4, G,4, B,4,      D,4,       EGL_NONE },
   { COMMON, R,2, G,2, B,2,      D,2,       EGL_NONE },
   { COMMON, R,1, G,1, B,1,      D,1,       EGL_NONE }  /* monochrome */
  };
  EGLint attrs[40];
  EGLint nconfig;
  int i, j, k, iter, pass;

  i = 0;

  char *glsls = get_string_resource_window(window, "prefersGLSL");
  Bool glslp = (glsls && !strcasecmp(glsls, "true"));
  iter = (glslp ? 2 : 1);

  *egl_config = 0;
  for (pass = 0; pass < iter; pass++)
    {
      for (; i < countof(templates); i++)
        {
          for (j = 0, k = 0; templates[i][j] != EGL_NONE; j += 2)
            {
              attrs[k++] = templates[i][j];
              attrs[k++] = templates[i][j+1];
            }

          attrs[k++] = EGL_RENDERABLE_TYPE;
# ifdef HAVE_GLES3
          if (glslp && pass == 0)
            attrs[k++] = EGL_OPENGL_ES3_BIT;
          else
            attrs[k++] = EGL_OPENGL_ES_BIT;
# elif defined(HAVE_JWZGLES)
          attrs[k++] = EGL_OPENGL_ES_BIT;
# else
          attrs[k++] = EGL_OPENGL_BIT;
# endif

          /*
          if (x11_visual_id != -1)
            {
              attrs[k++] = EGL_NATIVE_VISUAL_ID;
              attrs[k++] = x11_visual_id;
            }
          */

          attrs[k++] = EGL_NONE;

          nconfig = -1;
          if (eglChooseConfig (egl_display, attrs, egl_config, 1, &nconfig)
              && nconfig == 1)
            break;
        }
      if (i < countof(templates))
        break;
    }

  if (! *egl_config)
    {
      fprintf (stderr, "%s: eglChooseConfig: no configs found\n", progname);
      abort();
    }

  {
    int i;
    const struct {
      int hexp;
      EGLint i;
      const char *s;
    } fields[] = {
        {1, EGL_CONFIG_ID, "config ID:"},
        {1, EGL_CONFIG_CAVEAT, "caveat:"},
        {1, EGL_CONFORMANT, "conformant:"},
        {0, EGL_COLOR_BUFFER_TYPE, "buffer type:"},
        {0, EGL_RED_SIZE, "color size:"},
        {0, EGL_TRANSPARENT_RED_VALUE, "transparency:"},
        {0, EGL_BUFFER_SIZE, "buffer size:"},
        {0, EGL_DEPTH_SIZE, "depth size:"},
        {0, EGL_LUMINANCE_SIZE, "lum size:"},
        {0, EGL_STENCIL_SIZE, "stencil size:"},
        {0, EGL_ALPHA_MASK_SIZE, "alpha mask:"},
        {0, EGL_LEVEL, "level:"},
        {0, EGL_SAMPLES, "samples:"},
        {0, EGL_SAMPLE_BUFFERS, "sample bufs:"},
        {0, EGL_NATIVE_RENDERABLE, "native render:"},
        {1, EGL_NATIVE_VISUAL_TYPE, "native type:"},
        {1, EGL_RENDERABLE_TYPE, "render type:"},
        {0, EGL_SURFACE_TYPE, "surface type:"},
        {0, EGL_BIND_TO_TEXTURE_RGB, "bind RGB:"},
        {0, EGL_BIND_TO_TEXTURE_RGBA, "bind RGBA:"},
        {0, EGL_MAX_PBUFFER_WIDTH, "buffer width:"},
        {0, EGL_MAX_PBUFFER_HEIGHT, "buffer height:"},
        {0, EGL_MAX_PBUFFER_PIXELS, "buffer pixels:"},
        {0, EGL_MAX_SWAP_INTERVAL, "max swap:"},
        {0, EGL_MIN_SWAP_INTERVAL, "min swap:"},
    };
    EGLint r = 0, g = 0, b = 0, a = 0, tt = 0, tr = 0, tg = 0, tb = 0;
    eglGetConfigAttrib(egl_display, *egl_config, EGL_RED_SIZE, &r);
    eglGetConfigAttrib(egl_display, *egl_config, EGL_GREEN_SIZE, &g);
    eglGetConfigAttrib(egl_display, *egl_config, EGL_BLUE_SIZE, &b);
    eglGetConfigAttrib(egl_display, *egl_config, EGL_ALPHA_SIZE, &a);
    eglGetConfigAttrib(egl_display, *egl_config, EGL_TRANSPARENT_TYPE, &tt);
    eglGetConfigAttrib(egl_display, *egl_config, EGL_TRANSPARENT_RED_VALUE,
                       &tr);
    eglGetConfigAttrib(egl_display, *egl_config, EGL_TRANSPARENT_GREEN_VALUE,
                       &tg);
    eglGetConfigAttrib(egl_display, *egl_config, EGL_TRANSPARENT_BLUE_VALUE,
                       &tb);
    for (i = 0; i < countof(fields); i++) {
      EGLint v = 0;
      char s[100];
      eglGetConfigAttrib(egl_display, *egl_config, fields[i].i, &v);
      if (fields[i].i == EGL_RED_SIZE)
        sprintf(s, "%d, %d, %d, %d", r, g, b, a);
      else if (fields[i].i == EGL_TRANSPARENT_RED_VALUE && tt != EGL_NONE)
        sprintf(s, "%d, %d, %d", tr, tg, tb);
      else if (fields[i].i == EGL_CONFIG_CAVEAT)
        strcpy(s, (v == EGL_NONE          ? "none"
                   : v == EGL_SLOW_CONFIG ? "slow"
                   :
#ifdef EGL_NON_CONFORMANT
                   v == EGL_NON_CONFORMANT ? "non-conformant"
                                           :
#endif
                                           "???"));
      else if (fields[i].i == EGL_COLOR_BUFFER_TYPE)
        strcpy(s, (v == EGL_RGB_BUFFER         ? "RGB"
                   : v == EGL_LUMINANCE_BUFFER ? "luminance"
                                               : "???"));
      else if (fields[i].i == EGL_CONFORMANT ||
               fields[i].i == EGL_RENDERABLE_TYPE) {
        sprintf(s, "0x%02x", v);
        if (v & EGL_OPENGL_BIT)
          strcat(s, " OpenGL");
        if (v & EGL_OPENGL_ES_BIT)
          strcat(s, " GLES-1.x");
        if (v & EGL_OPENGL_ES2_BIT)
          strcat(s, " GLES-2.0");
#ifdef EGL_OPENGL_ES3_BIT
        if (v & EGL_OPENGL_ES3_BIT)
          strcat(s, " GLES-3.0");
#endif
        if (v & EGL_OPENVG_BIT)
          strcat(s, " OpenVG");
      } else if (fields[i].hexp)
        sprintf(s, "0x%02x", v);
      else if (v)
        sprintf(s, "%d", v);
      else
        *s = 0;

      if (*s)
        Log("EGL: %-14s %s\n", fields[i].s, s);
    }
  }

  check_gl_error("EGL config error!");
}

/*
static void get_egl_config(Window window, EGLDisplay *egl_display,
                           EGLConfig *egl_config) {
#define R EGL_RED_SIZE
#define G EGL_GREEN_SIZE
#define B EGL_BLUE_SIZE
#define A EGL_ALPHA_SIZE
#define D EGL_DEPTH_SIZE
#define I EGL_BUFFER_SIZE
#define ST EGL_STENCIL_SIZE
  EGLint templates[][40] = {
      {R, 8, G, 8, B, 8, A, 8, D, 8, ST, 1, EGL_NONE}, //* rgba stencil 
      {R, 8, G, 8, B, 8, D, 8, ST, 1, EGL_NONE},       /* rgb  stencil
      {R, 4, G, 4, B, 4, D, 4, ST, 1, EGL_NONE},
      {R, 2, G, 2, B, 2, D, 2, ST, 1, EGL_NONE},
      {R, 8, G, 8, B, 8, A, 8, D, 8, EGL_NONE}, //* rgba 
      {R, 8, G, 8, B, 8, D, 8, EGL_NONE},       //* rgb  
      {R, 4, G, 4, B, 4, D, 4, EGL_NONE},
      {R, 2, G, 2, B, 2, D, 2, EGL_NONE},
      {R, 1, G, 1, B, 1, D, 1, EGL_NONE} //* monochrome 
  };
  EGLint attrs[40];
  EGLint nconfig;
  int i, j, k, iter, pass;

  char *glsls = get_string_resource_window(window, "prefersGLSL");
  Bool glslp = (glsls && !strcasecmp(glsls, "true"));
  iter = (glslp ? 2 : 1);

  *egl_config = 0;
  for (pass = 0; pass < iter; pass++) {
    for (i = 0; i < countof(templates); i++) {
      for (j = 0, k = 0; templates[i][j] != EGL_NONE; j += 2) {
        attrs[k++] = templates[i][j];
        attrs[k++] = templates[i][j + 1];
      }

      attrs[k++] = EGL_RENDERABLE_TYPE;
#ifdef HAVE_GLES3
      if (glslp && pass == 0)
        attrs[k++] = EGL_OPENGL_ES3_BIT;
      else
        attrs[k++] = EGL_OPENGL_ES_BIT;
#else
      attrs[k++] = EGL_OPENGL_ES_BIT;
#endif

      attrs[k++] = EGL_NONE;

      nconfig = -1;
      if (eglChooseConfig(egl_display, attrs, egl_config, 1, &nconfig) &&
          nconfig == 1)
        break;
    }
    if (i < countof(templates))
      break;
  }
  Assert(*egl_config != 0, "no EGL config chosen");
#if 1
  {
    int i;
    const struct {
      int hexp;
      EGLint i;
      const char *s;
    } fields[] = {
        {1, EGL_CONFIG_ID, "config ID:"},
        {1, EGL_CONFIG_CAVEAT, "caveat:"},
        {1, EGL_CONFORMANT, "conformant:"},
        {0, EGL_COLOR_BUFFER_TYPE, "buffer type:"},
        {0, EGL_RED_SIZE, "color size:"},
        {0, EGL_TRANSPARENT_RED_VALUE, "transparency:"},
        {0, EGL_BUFFER_SIZE, "buffer size:"},
        {0, EGL_DEPTH_SIZE, "depth size:"},
        {0, EGL_LUMINANCE_SIZE, "lum size:"},
        {0, EGL_STENCIL_SIZE, "stencil size:"},
        {0, EGL_ALPHA_MASK_SIZE, "alpha mask:"},
        {0, EGL_LEVEL, "level:"},
        {0, EGL_SAMPLES, "samples:"},
        {0, EGL_SAMPLE_BUFFERS, "sample bufs:"},
        {0, EGL_NATIVE_RENDERABLE, "native render:"},
        {1, EGL_NATIVE_VISUAL_TYPE, "native type:"},
        {1, EGL_RENDERABLE_TYPE, "render type:"},
        {0, EGL_SURFACE_TYPE, "surface type:"},
        {0, EGL_BIND_TO_TEXTURE_RGB, "bind RGB:"},
        {0, EGL_BIND_TO_TEXTURE_RGBA, "bind RGBA:"},
        {0, EGL_MAX_PBUFFER_WIDTH, "buffer width:"},
        {0, EGL_MAX_PBUFFER_HEIGHT, "buffer height:"},
        {0, EGL_MAX_PBUFFER_PIXELS, "buffer pixels:"},
        {0, EGL_MAX_SWAP_INTERVAL, "max swap:"},
        {0, EGL_MIN_SWAP_INTERVAL, "min swap:"},
    };
    EGLint r = 0, g = 0, b = 0, a = 0, tt = 0, tr = 0, tg = 0, tb = 0;
    eglGetConfigAttrib(egl_display, *egl_config, EGL_RED_SIZE, &r);
    eglGetConfigAttrib(egl_display, *egl_config, EGL_GREEN_SIZE, &g);
    eglGetConfigAttrib(egl_display, *egl_config, EGL_BLUE_SIZE, &b);
    eglGetConfigAttrib(egl_display, *egl_config, EGL_ALPHA_SIZE, &a);
    eglGetConfigAttrib(egl_display, *egl_config, EGL_TRANSPARENT_TYPE, &tt);
    eglGetConfigAttrib(egl_display, *egl_config, EGL_TRANSPARENT_RED_VALUE,
                       &tr);
    eglGetConfigAttrib(egl_display, *egl_config, EGL_TRANSPARENT_GREEN_VALUE,
                       &tg);
    eglGetConfigAttrib(egl_display, *egl_config, EGL_TRANSPARENT_BLUE_VALUE,
                       &tb);
    for (i = 0; i < countof(fields); i++) {
      EGLint v = 0;
      char s[100];
      eglGetConfigAttrib(egl_display, *egl_config, fields[i].i, &v);
      if (fields[i].i == EGL_RED_SIZE)
        sprintf(s, "%d, %d, %d, %d", r, g, b, a);
      else if (fields[i].i == EGL_TRANSPARENT_RED_VALUE && tt != EGL_NONE)
        sprintf(s, "%d, %d, %d", tr, tg, tb);
      else if (fields[i].i == EGL_CONFIG_CAVEAT)
        strcpy(s, (v == EGL_NONE          ? "none"
                   : v == EGL_SLOW_CONFIG ? "slow"
                   :
#ifdef EGL_NON_CONFORMANT
                   v == EGL_NON_CONFORMANT ? "non-conformant"
                                           :
#endif
                                           "???"));
      else if (fields[i].i == EGL_COLOR_BUFFER_TYPE)
        strcpy(s, (v == EGL_RGB_BUFFER         ? "RGB"
                   : v == EGL_LUMINANCE_BUFFER ? "luminance"
                                               : "???"));
      else if (fields[i].i == EGL_CONFORMANT ||
               fields[i].i == EGL_RENDERABLE_TYPE) {
        sprintf(s, "0x%02x", v);
        if (v & EGL_OPENGL_BIT)
          strcat(s, " OpenGL");
        if (v & EGL_OPENGL_ES_BIT)
          strcat(s, " GLES-1.x");
        if (v & EGL_OPENGL_ES2_BIT)
          strcat(s, " GLES-2.0");
#ifdef EGL_OPENGL_ES3_BIT
        if (v & EGL_OPENGL_ES3_BIT)
          strcat(s, " GLES-3.0");
#endif
        if (v & EGL_OPENVG_BIT)
          strcat(s, " OpenVG");
      } else if (fields[i].hexp)
        sprintf(s, "0x%02x", v);
      else if (v)
        sprintf(s, "%d", v);
      else
        *s = 0;

      if (*s)
        Log("init:    EGL %-14s %s\n", fields[i].s, s);
    }
  }
#endif
  check_gl_error("EGL config error!");
}
*/

static void
get_egl_context(Window window, EGLDisplay egl_display,
                        EGLConfig egl_config, EGLContext *egl_context)
{
  EGLint context_attribs[][3] = {
    { EGL_CONTEXT_CLIENT_VERSION, 1, EGL_NONE },
    { EGL_CONTEXT_CLIENT_VERSION, 3, EGL_NONE }
  };
  EGLint *attrs;
  Bool glslp;
  int pass, iter;

# ifdef EGL_OPENGL_ES3_BIT
  char *glsls = get_string_resource_window (window, "prefersGLSL");
  glslp = (glsls && !strcasecmp(glsls, "true"));
  if (glslp)
    {
      EGLint renderable_type;
      eglGetConfigAttrib (egl_display, egl_config, EGL_RENDERABLE_TYPE,
                          &renderable_type);
      Bool gles3p = (renderable_type & EGL_OPENGL_ES3_BIT) != 0;
      glslp = glslp && gles3p;
    }
# else
  glslp = False;
# endif
  iter = (glslp ? 2 : 1);

  *egl_context = EGL_NO_CONTEXT;
  for (pass = 0; pass < iter; pass++)
    {
      if (glslp && pass == 0)
        attrs = context_attribs[1];
      else
        attrs = context_attribs[0];
      *egl_context = eglCreateContext (egl_display, egl_config,
                                       EGL_NO_CONTEXT, attrs);
      if (*egl_context != EGL_NO_CONTEXT)
        break;
    }

  Assert (*egl_context != EGL_NO_CONTEXT, "init: EGL_NO_CONTEXT");
  check_gl_error("EGL contex error!");
}


Bool init_egl(struct running_hack *rh) {
  EGLint egl_major = -1, egl_minor = -1;
  rh->native_display = get_native_display(rh->sdl_window);
  rh->native_window =
      get_native_window(rh->sdl_window, DEFAULT_WIDTH, DEFAULT_HEIGHT);
  if ((rh->egl_display = eglGetDisplay(rh->native_display)) == EGL_NO_DISPLAY) {
    EGL_ERROR("Unable to create EGL display!", True);
  }
  if (eglInitialize(rh->egl_display, &egl_major, &egl_minor) != EGL_TRUE) {
    eglTerminate(rh->egl_display);
    rh->egl_display = EGL_NO_DISPLAY;
    EGL_ERROR("Unable to initialize EGL display!", True);
  }
  get_egl_config(rh->window, rh->egl_display, &rh->egl_config);
  get_egl_context(rh->window, rh->egl_display, rh->egl_config, &rh->egl_ctx);
  if ((rh->egl_surface = eglCreateWindowSurface(rh->egl_display, rh->egl_config,
                                                rh->native_window, 0)) ==
      EGL_NO_SURFACE) {
    EGL_ERROR("Unable to create EGL surface!", True);
  }
  if (eglMakeCurrent(rh->egl_display, rh->egl_surface, rh->egl_surface,
                     rh->egl_ctx) != EGL_TRUE) {
    EGL_ERROR("Unable to make EGL context current!", True);
  }
  printf("Vendor: %s\nRenderer: %s\nVersion: %s\nEGL: %d.%d\n",
         glGetString(GL_VENDOR), glGetString(GL_RENDERER),
         glGetString(GL_VERSION), egl_major, egl_minor);
}

Bool init(struct running_hack *rh) {
  EGLint num_configs;
  struct xscreensaver_function_table *ft = xscreensaver_function_table;

  rh->initted_p = 0;
  rh->window = calloc(1, sizeof(*rh->window));
  rh->window->window.rh = rh;
  rh->window->frame.width = DEFAULT_WIDTH;
  rh->window->frame.height = DEFAULT_HEIGHT;
  rh->window->type = WINDOW;

  rh->dict = NULL;
  rh->event_queue = NULL;

  rh->xsft = &ft[0];

  rh->sdl_window = SDL_CreateWindow("xscreensaver", 0, 0, DEFAULT_WIDTH,
                                    DEFAULT_HEIGHT, SDL_WINDOW_OPENGL);
  SDL_SetWindowResizable(rh->sdl_window, True);

  if (!init_egl(rh))
    return False;

  for (int i = 0; i < countof(default_defaults); i++) {
    const char *key = default_defaults[i].key;
    const char *val = default_defaults[i].val;
    rh->dict = dict_add(rh->dict, key, val);
  }

  const char *const *defs = rh->xsft->defaults;
  while (*defs) {
    char *line = strdup(*defs);
    char *key, *val;
    key = line;
    while (*key == '.' || *key == '*' || *key == ' ' || *key == '\t')
      key++;
    val = key;
    while (*val && *val != ':')
      val++;
    if (*val != ':')
      abort();
    *val++ = 0;
    while (*val == ' ' || *val == '\t')
      val++;

    unsigned long L = strlen(val);
    while (L > 0 && (val[L - 1] == ' ' || val[L - 1] == '\t'))
      val[--L] = 0;

    rh->dict = dict_add(rh->dict, key, val);
    free(line);
    defs++;
  }

  prepare_context(rh);

  rh->window->egl_surface = rh->egl_surface;

  rh->dpy = jwxyz_gl_make_display(rh->window);

  if (rh->xsft->visual == GL_VISUAL)
    rh->gles_state = jwzgles_make_state();

  SDL_ShowWindow(rh->sdl_window);
  return True;
}

void cleanup(struct running_hack *rh) {
  eglDestroySurface(rh->egl_display, rh->egl_surface);
  eglDestroyContext(rh->egl_display, rh->egl_ctx);
  eglTerminate(rh->egl_display);
  SDL_DestroyWindow(rh->sdl_window);
}

static void
free_pixmap (struct running_hack *rh, Pixmap p)
{
  eglDestroySurface(rh->egl_display, p->egl_surface);
}


int
XFreePixmap (Display *d, Pixmap p)
{
  struct running_hack *rh = XRootWindow(d, 0)->window.rh;

  jwxyz_gl_flush (d);

  if (rh->current_drawable == p)
    rh->current_drawable = NULL;

  free_pixmap (rh, p);
  free (p);
  return 0;
}

