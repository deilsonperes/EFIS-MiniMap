#ifndef __EFIS_WINDOW__
#define __EFIS_WINDOW__
#include <SDL/SDL.h>

#define W_PARAMS "EFIS MiniMap", \
                    SDL_WINDOWPOS_CENTERED, /* window x coord */\
                    SDL_WINDOWPOS_CENTERED, /* window y coord */\
                    300,                   /* window width */  \
                    200,                    /* window height */ \
                    SDL_WINDOW_OPENGL|SDL_WINDOW_RESIZABLE /* flags */
#endif//__EFIS_WINDOW__
