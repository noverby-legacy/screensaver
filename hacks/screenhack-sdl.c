/* xscreensaver, Copyright (c) 1992-2021 Jamie Zawinski <jwz@jwz.org>
 *
 * Permission to use, copy, modify, distribute, and sell this software and its
 * documentation for any purpose is hereby granted without fee, provided that
 * the above copyright notice appear in all copies and that both that
 * copyright notice and this permission notice appear in supporting
 * documentation.  No representations are made about the suitability of this
 * software for any purpose.  It is provided "as is" without express or
 * implied warranty.
 *
 * And remember: X Windows is to graphics hacking as roman numerals are to
 * the square root of pi.
 */

/* This file contains simple code to open a window or draw on the root.
   The idea being that, when writing a graphics hack, you can just link
   with this .o to get all of the uninteresting junk out of the way.

*/

#include <stdio.h>

#include "jwxyz-sdl.h"
#include "jwxyzI.h"
#include "screenhackI.h"
#include "version.h"

int main(int argc, char **argv) {
  int loop = 1;
  SDL_Event event;

  /*
  const char *hack = "Greynetic";
  int chosen = 0;
  for (;;) {
    if (!strcmp(ft[chosen].progclass, hack))
      break;
    chosen++;
  }
  */

  struct running_hack rh;

  if (!init(&rh))
    return 1;

  while (loop) {
    while (SDL_PollEvent(&event)) {
      switch (event.type) {
      case SDL_QUIT: {
        loop = 0;
        break;
      }
      case SDL_WINDOWEVENT: {
        switch (event.window.event) {
        case SDL_WINDOWEVENT_SIZE_CHANGED: {
          resize(&rh, event.window.data1, event.window.data2);
          break;
        }
        }
      }
      }
    }
    draw(&rh);
  }

  cleanup(&rh);
}
