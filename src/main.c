/* standard headers */
#include <stdio.h>
#include <math.h>
#include <time.h>

/* library headers */
#include "SDL/SDL.h"
#include "GL/glew.h"
#include "GL/GL.h"
#include "cglm/cglm.h"

/* custom headers */
#include "glutils.h"
#include "osmloader.h"

/* Definitions for window parameters */
#include "headers/windowdefs.h"

/* Function declaration */
bool initSDL(SDL_Window**);
bool initOpenGL(SDL_Window*, SDL_GLContext*);
void sdlLoop(SDL_Window*);
void glRenderLoop(SDL_Window*);
void printShaderLog(GLuint);
void processKeys();

/* Global variables */
// OpenGL
GLuint vtx_buffer = 0;
GLint vtx_num_vertices = 8;
static const GLfloat vtx_data[] = {
   -1.0f,  1.0f, -1.0f,
   -1.0f, -1.0f, -1.0f,
    1.0f, -1.0f, -1.0f,
    1.0f,  1.0f, -1.0f,
   -1.0f,  1.0f,  1.0f,
   -1.0f, -1.0f,  1.0f,
    1.0f, -1.0f,  1.0f,
    1.0f,  1.0f,  1.0f
};

GLuint vtx_drawOrder = 0;
GLint vtx_num_elements = 18;
static const GLint vtx_orderData[] = {
    0, 1, 2, 2, 3, 0, // back
    0, 1, 4, 4, 5, 1,
    0, 4, 7, 7, 3, 0
};

GLuint glShaderProgram;
GLuint glUniformWave;
GLuint glUniformTranslateMat;
float 
    pos_x   = 0.0f,
    pos_y   = 0.0f,
    pos_z   = 1.0f,
    ang     = 0.0f;

// 
osm *mapData;
volatile bool renderThreadRun = true;
/* Global variables end*/

/* Program Entry */
int main(int argc, char** argv)
{
    // Load OSM data
    if(argc != 2 || argv[1] == NULL)
	{
        argv[1] = "map.osm";
    }
    printf("Loading map data...\n");
    osmLoad(argv[1], &mapData);
  
    // Init SDL
    SDL_Window* sdl_window = NULL;
    if(!initSDL(&sdl_window))
    {
        printf("Could not start SDL, the application will exit.\n");
        goto main_exit;
    }

    SDL_Thread* sdl_t = SDL_CreateThread((SDL_ThreadFunction) glRenderLoop, "sdlEvents", (void *) sdl_window);

    // Main loop
    SDL_Event ev;
    while (1)
    {
        SDL_PollEvent(&ev);
        switch(ev.type)
        {
            case SDL_QUIT:
                goto main_exit;
                break;
        }

        processKeys();
        
        SDL_Delay(5);
    }

main_exit:
    renderThreadRun = false;
    SDL_WaitThread(sdl_t, (void*) NULL);
    printf("Waiting for render thread to finish...\n");
    SDL_Quit();
    osmFree(&mapData);
    printf("Program end.\n");
    return 0;
}

bool initSDL(SDL_Window** wnd)
{
    // Initialise SDL
    if(SDL_Init(SDL_INIT_VIDEO) != 0)
    {
        // Error initializing SDL
        printf("SDL Error: %s\n", SDL_GetError());
        return false;
    }

    // Create a window
    *wnd = SDL_CreateWindow(W_PARAMS); // W_PARAMS is defined in windowdefs.h
    if(wnd != NULL)
        return true;

    printf("Error creating SDL window: %s\n", SDL_GetError());
    SDL_Quit();
    return false;
}

bool initOpenGL(SDL_Window* wnd, SDL_GLContext* ctx)
{
    // Create OpenGL context
    ctx = SDL_GL_CreateContext(wnd);
    printf("GL Version: %s\n", glGetString(GL_VERSION));
    
    if(ctx == NULL)
    {
        printf("Error creating OpenGL context: %s.\n", SDL_GetError());
        return false;
    }
    
    glewExperimental = GL_TRUE;
    glewInit();

    glGenBuffers(1, &vtx_buffer);
    glBindBuffer(GL_ARRAY_BUFFER, vtx_buffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vtx_data), &vtx_data, GL_STATIC_DRAW);

    glGenBuffers(1, &vtx_drawOrder);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vtx_drawOrder);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLuint) * vtx_num_elements, &vtx_orderData, GL_STATIC_DRAW);

    // TODO: Check if program is greater than zero
    glShaderProgram = mgl_compileProgram("vertex.glsl", "fragment.glsl");
    if(glShaderProgram == 0)
    {
        printf("Shader program compilation failed.\n");
        return false;
    }

    glUniformWave = glGetUniformLocation(glShaderProgram, "wave");
    glUniformTranslateMat = glGetUniformLocation(glShaderProgram, "translate");

    return true;
}

void processKeys()
{
    const Uint8* keys = SDL_GetKeyboardState(NULL);
    
    pos_x += (keys[SDL_SCANCODE_RIGHT] - keys[SDL_SCANCODE_LEFT]) * 0.01f;
    pos_y += (keys[SDL_SCANCODE_UP] - keys[SDL_SCANCODE_DOWN]) * 0.01f;
    pos_z += (keys[SDL_SCANCODE_X] - keys[SDL_SCANCODE_Z]) * 0.01f;
    ang += (keys[SDL_SCANCODE_E] - keys[SDL_SCANCODE_Q]) * 0.01f;
}

void glRenderLoop(SDL_Window* wnd)
{   
    clock_t CLOCKS_PER_MSEC = CLOCKS_PER_SEC / 1000;
    clock_t t_start, t_dif_ms;
    float t_delta;

    // Init OpenGL
    SDL_GLContext* sdl_glContext = NULL;
    if(!initOpenGL(wnd, sdl_glContext))
    {
        printf("Error initializing OpenGL.\n");
        return;
    }

    int w, h, 
        lw = 0, 
        lh = 0;
    
    float aspect;
    glTranslatef(pos_x, pos_y, 1);

    mat4 model, view, projection;
    glm_mat4_identity(projection);
    glm_mat4_identity(view);
    glm_mat4_identity(model);

    vec3 
        pos = {0.0f, 0.0f, 0.0f},
        axis = {0.0f, 1.0f, 0.0f};

    while (renderThreadRun)
    {
        t_start = clock();
        glUseProgram(glShaderProgram);
        
        SDL_GetWindowSize(wnd, &w, &h);
        if(lw != w || lh != h)
        {
            lw = w;
            lh = h;
            glViewport(0, 0, w, h);
            
            aspect = (float) w / (float) h;

            glClearColor(0, 0, 0, 1);
            glClear(GL_COLOR_BUFFER_BIT);

            way *wy = mapData->ways;
            node **nd;
            glColor4f(1,1,1,1);
            glBegin(GL_LINES);
            /*
            for(int n = 0; n < mapData->numNodes - 1; n++)
            {
                node * nd = &mapData->nodes[n];
                glVertex2d(nd->longitude, nd->latitude);
                nd = &mapData->nodes[n+1];
                glVertex2d(nd->longitude, nd->latitude);
            }
            */
            for(int nw = 0; nw < mapData->numWays; nw++)
            {
                nd = wy->nodes;
                // osmPrintWay(wy);
                int lim = (wy->numNodes % 2 == 0? wy->numNodes: wy->numNodes - 1);
                printf("%d -> %d\n", wy->numNodes, lim);
                if(lim < 2) continue;
                for(int nn = 0; nn < lim; nn++)
                {
                    glVertex2d((*nd)->longitude, (*nd)->latitude);
                    nd++;
                    glVertex2d((*nd)->longitude, (*nd)->latitude);
                }
                wy++;
            }
            
            glEnd();
            SDL_GL_SwapWindow(wnd);
        }

        t_dif_ms = t_start - clock() / CLOCKS_PER_MSEC;
        if(t_dif_ms < CLOCKS_PER_MSEC * 16)
        {
            SDL_Delay(16 - t_dif_ms);
        }
    }
    printf("Render thread ended.\n");
}