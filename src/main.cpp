#include <SDL2/SDL.h>
#include <stdlib.h>
#include <iostream>
#include <string>
#include <vector>
#include "glad.h"
#include <random>
#include "../include/shader.hpp"
#include "../include/camera.hpp"
#include  "../glm/glm.hpp"
#include "../glm/gtc/type_ptr.hpp"
#include "../glm/gtc/matrix_transform.hpp"
using namespace std;

#define RC_INTERSPACE 0.02 //en pixel
#define RC_OFFSET_CUBE (1.0f+RC_INTERSPACE)
#define RC_ROTATION_SPEED 120.0f

enum rc_face{
    RC_FRONT = 0,
    RC_REAR = 1,
    RC_LEFT = 2,
    RC_RIGHT = 3,
    RC_BOTTOM = 4,
    RC_TOP = 5
};

// 3*3*6 = 54 faces 54*4 = 216 points
float vertices[] = {
    // avant
    -0.5f, -0.5f, 0.5f,    0.0f, 0.0f, 1.0f,
     0.5f, -0.5f, 0.5f,    0.0f, 0.0f, 1.0f,
     0.5f,  0.5f, 0.5f,    0.0f, 0.0f, 1.0f,
    -0.5f,  0.5f, 0.5f,    0.0f, 0.0f, 1.0f,
    // arriere
    -0.5f, -0.5f, -0.5f,    0.0f, 1.0f, 0.0f,
     0.5f, -0.5f, -0.5f,    0.0f, 1.0f, 0.0f,
     0.5f,  0.5f, -0.5f,    0.0f, 1.0f, 0.0f,
    -0.5f,  0.5f, -0.5f,    0.0f, 1.0f, 0.0f,
    //gauche
    -0.5f,  0.5f, -0.5f,    1.0f, 0.0f, 0.0f,
    -0.5f,  0.5f,  0.5f,    1.0f, 0.0f, 0.0f,
    -0.5f, -0.5f,  0.5f,    1.0f, 0.0f, 0.0f,
    -0.5f, -0.5f, -0.5f,    1.0f, 0.0f, 0.0f,
    //droite
     0.5f,  0.5f, -0.5f,    0.0f, 0.0f, 1.0f,
     0.5f,  0.5f,  0.5f,    0.0f, 0.0f, 1.0f,
     0.5f, -0.5f,  0.5f,    0.0f, 0.0f, 1.0f,
     0.5f, -0.5f, -0.5f,    0.0f, 0.0f, 1.0f,
    //dessous
    -0.5f, -0.5f,  0.5f,    0.0f, 1.0f, 0.0f,
     0.5f, -0.5f,  0.5f,    0.0f, 1.0f, 0.0f,
     0.5f, -0.5f, -0.5f,    0.0f, 1.0f, 0.0f,
    -0.5f, -0.5f, -0.5f,    0.0f, 1.0f, 0.0f,
    //dessus
    -0.5f,  0.5f,  0.5f,    1.0f, 0.0f, 0.0f,
     0.5f,  0.5f,  0.5f,    1.0f, 0.0f, 0.0f,
     0.5f,  0.5f, -0.5f,    1.0f, 0.0f, 0.0f,
    -0.5f,  0.5f, -0.5f,    1.0f, 0.0f, 0.0f,
};

GLuint indices[] = {
    //avant
    0, 1, 2, 0, 2, 3,
    //arriere
    4, 5, 6, 4, 6, 7,
    //gauche
    8, 9, 10, 8, 10, 11,
    //droite
    12, 13, 14, 12, 14, 15,
    //dessous
    16, 17, 18, 16, 18, 19,
    //arriere
    20, 21, 22, 20, 22, 23,
    //dessus
    24, 25, 26, 24, 26, 27
};

uint32_t black = 0x00000000;
uint32_t white = 0xFFFFFF00;
uint32_t red = 0xFF000000;
uint32_t blue = 0x0000FF00;
uint32_t green = 0x00FF0000;
uint32_t yellow = 0xFFFF0000;
uint32_t orange = 0xFFA50000;

//derriere devant gauche droite dessous dessus
uint32_t colors[27][6] = {{white, black, blue, black, red, black},
                          {white, black, black, black, red, black},
                          {white, black, black, green, red, black},
                          {white, black, blue, black, black, black},
                          {white, black, black, black, black, black},
                          {white, black, black, green, black, black},
                          {white, black, blue, black, black, orange},
                          {white, black, black, black, black, orange},
                          {white, black, black, green, black, orange},
                          {black, black, blue, black, red, black},
                          {black, black, black, black, red, black},
                          {black, black, black, green, red, black},
                          {black, black, blue, black, black, black},
                          {black, black, black, black, black, black},
                          {black, black, black, green, black, black},
                          {black, black, blue, black, black, orange},
                          {black, black, black, black, black, orange},
                          {black, black, black, green, black, orange},
                          {black, yellow, blue, black, red, black},
                          {black, yellow, black, black, red, black},
                          {black, yellow, black, green, red, black},
                          {black, yellow, blue, black, black, black},
                          {black, yellow, black, black, black, black},
                          {black, yellow, black, green, black, black},
                          {black, yellow, blue, black, black, orange},
                          {black, yellow, black, black, black, orange},
                          {black, yellow, black, green, black, orange}};

glm::vec3 cube_position[] = {
    glm::vec3(-RC_OFFSET_CUBE, -RC_OFFSET_CUBE, RC_OFFSET_CUBE), glm::vec3(0.0f, -RC_OFFSET_CUBE, RC_OFFSET_CUBE), glm::vec3(RC_OFFSET_CUBE, -RC_OFFSET_CUBE, RC_OFFSET_CUBE),//tranche devant
    glm::vec3(-RC_OFFSET_CUBE, 0.0, RC_OFFSET_CUBE),             glm::vec3(0.0f, 0.0, RC_OFFSET_CUBE),             glm::vec3(RC_OFFSET_CUBE, 0.0, RC_OFFSET_CUBE),
    glm::vec3(-RC_OFFSET_CUBE, RC_OFFSET_CUBE, RC_OFFSET_CUBE),  glm::vec3(0.0f, RC_OFFSET_CUBE, RC_OFFSET_CUBE),  glm::vec3(RC_OFFSET_CUBE, RC_OFFSET_CUBE, RC_OFFSET_CUBE),
    glm::vec3(-RC_OFFSET_CUBE, -RC_OFFSET_CUBE, 0.0f),           glm::vec3(0.0f, -RC_OFFSET_CUBE, 0.0f),           glm::vec3(RC_OFFSET_CUBE, -RC_OFFSET_CUBE, 0.0f),//tranche milieu
    glm::vec3(-RC_OFFSET_CUBE, 0.0, 0.0f),                       glm::vec3(0.0f, 0.0, 0.0f),                       glm::vec3(RC_OFFSET_CUBE, 0.0, 0.0f),
    glm::vec3(-RC_OFFSET_CUBE, RC_OFFSET_CUBE, 0.0f),            glm::vec3(0.0f, RC_OFFSET_CUBE, 0.0f),            glm::vec3(RC_OFFSET_CUBE, RC_OFFSET_CUBE, 0.0f),
    glm::vec3(-RC_OFFSET_CUBE, -RC_OFFSET_CUBE, -RC_OFFSET_CUBE),glm::vec3(0.0f, -RC_OFFSET_CUBE, -RC_OFFSET_CUBE),glm::vec3(RC_OFFSET_CUBE, -RC_OFFSET_CUBE, -RC_OFFSET_CUBE),//tranche deriiere
    glm::vec3(-RC_OFFSET_CUBE, 0.0, -RC_OFFSET_CUBE),            glm::vec3(0.0f, 0.0, -RC_OFFSET_CUBE),            glm::vec3(RC_OFFSET_CUBE, 0.0, -RC_OFFSET_CUBE),
    glm::vec3(-RC_OFFSET_CUBE, RC_OFFSET_CUBE, -RC_OFFSET_CUBE), glm::vec3(0.0f, RC_OFFSET_CUBE, -RC_OFFSET_CUBE), glm::vec3(RC_OFFSET_CUBE, RC_OFFSET_CUBE, -RC_OFFSET_CUBE) 

};

void rc_change_face_color(float vertices[], rc_face face, uint32_t color)
{
    for(int i=0;i<4;i++)
    {
        vertices[24*face+3+6*i] = (float)((uint8_t)(color >>24))/255.0;
        vertices[24*face+3+6*i+1] = (float)((uint8_t)(color >>16))/255.0;
        vertices[24*face+3+6*i+2] = (float)((uint8_t)(color >>8))/255.0;
    }
}

void rc_bind_cube_color(float vertices[], uint32_t color[6])
{
    for(int i = 0;i<6;i++)
    {
        rc_change_face_color(vertices, (rc_face)i, color[i]);
    }
}

void rc_handle_rotation(uint32_t color[27][6], rc_face face, int32_t trigo)
{
    uint32_t offset=0;
    uint32_t permut[20][2];

    if(face == RC_REAR)
        offset = 18;
    else if(face == RC_RIGHT)
        offset = 2;
    else if(face == RC_TOP)
        offset = 6;
    
    if((face == RC_FRONT || face == RC_REAR) && trigo == 1)
    {
        //Les points sont dans l'ordre inverse de deplacement
        uint32_t tab[20][2] = {{1+offset, RC_BOTTOM}, {3+offset, RC_LEFT}, {7+offset, RC_TOP},   {5+offset, RC_RIGHT},//milieu de l'arete
                               {0+offset, RC_BOTTOM}, {6+offset, RC_LEFT}, {8+offset, RC_TOP},   {2+offset, RC_RIGHT},
                               {0+offset, RC_LEFT},   {6+offset, RC_TOP},  {8+offset, RC_RIGHT}, {2+offset, RC_BOTTOM},
                               {0+offset, face},      {6+offset, face},    {8+offset, face},     {2+offset, face},//face
                               {1+offset, face},      {3+offset, face},    {7+offset, face},     {5+offset, face}};
        copy(&tab[0][0], &tab[19][2], &permut[0][0]);
    }
    else if((face == RC_FRONT || face == RC_REAR) && trigo == -1)
    {
        uint32_t tab[20][2] = {{1+offset, RC_BOTTOM}, {5+offset, RC_RIGHT},  {7+offset, RC_TOP},   {3+offset, RC_LEFT}, 
                               {0+offset, RC_BOTTOM}, {2+offset, RC_RIGHT},  {8+offset, RC_TOP},   {6+offset, RC_LEFT},
                               {0+offset, RC_LEFT},   {2+offset, RC_BOTTOM}, {8+offset, RC_RIGHT}, {6+offset, RC_TOP},
                               {0+offset, face},      {2+offset, face},      {8+offset, face},     {6+offset, face},//face
                               {1+offset, face},      {5+offset, face},      {7+offset, face},     {3+offset, face}};
        copy(&tab[0][0], &tab[19][2], &permut[0][0]);
    }
    else if((face == RC_LEFT || face == RC_RIGHT) && trigo == 1)
    {
        uint32_t tab[20][2] = {{3+offset, RC_FRONT},  {15+offset, RC_TOP},  {21+offset, RC_REAR}, {9+offset, RC_BOTTOM}, 
                               {0+offset, RC_FRONT},  {6+offset, RC_TOP},   {24+offset, RC_REAR}, {18+offset, RC_BOTTOM},
                               {0+offset, RC_BOTTOM}, {6+offset, RC_FRONT}, {24+offset, RC_TOP},  {18+offset, RC_REAR},
                               {0+offset, face},      {6+offset, face},     {24+offset, face},    {18+offset, face},//face
                               {3+offset, face},      {15+offset, face},    {21+offset, face},    {9+offset, face}};
        copy(&tab[0][0], &tab[19][2], &permut[0][0]);
    }
    else if((face == RC_LEFT || face == RC_RIGHT) && trigo == -1)
    {
        uint32_t tab[20][2] = {{3+offset, RC_FRONT},  {9+offset, RC_BOTTOM},  {21+offset, RC_REAR}, {15+offset, RC_TOP}, 
                               {0+offset, RC_FRONT},  {18+offset, RC_BOTTOM}, {24+offset, RC_REAR}, {6+offset, RC_TOP},
                               {0+offset, RC_BOTTOM}, {18+offset, RC_REAR},   {24+offset, RC_TOP},  {6+offset, RC_FRONT},
                               {0+offset, face},      {18+offset, face},      {24+offset, face},    {6+offset, face},//face
                               {3+offset, face},      {9+offset, face},       {21+offset, face},    {15+offset, face}};
        copy(&tab[0][0], &tab[19][2], &permut[0][0]);
    }
    else if((face == RC_BOTTOM || RC_TOP) && trigo == 1)
    {
        uint32_t tab[20][2] = {{1+offset, RC_FRONT}, {9+offset, RC_LEFT},  {19+offset, RC_REAR},  {11+offset, RC_RIGHT}, 
                               {0+offset, RC_FRONT}, {18+offset, RC_LEFT}, {20+offset, RC_REAR},  {2+offset, RC_RIGHT},
                               {0+offset, RC_LEFT},  {18+offset, RC_REAR}, {20+offset, RC_RIGHT}, {2+offset, RC_FRONT},
                               {0+offset, face},     {18+offset, face},    {20+offset, face},     {2+offset, face},//face
                               {1+offset, face},     {9+offset, face},     {19+offset, face},     {11+offset, face}};
        copy(&tab[0][0], &tab[19][2], &permut[0][0]);
    }
    else if((face == RC_BOTTOM || RC_TOP) && trigo == -1)
    {
        uint32_t tab[20][2] = {{1+offset, RC_FRONT}, {11+offset, RC_RIGHT}, {19+offset, RC_REAR},  {9+offset, RC_LEFT}, 
                               {0+offset, RC_FRONT}, {2+offset, RC_RIGHT},  {20+offset, RC_REAR},  {18+offset, RC_LEFT},
                               {0+offset, RC_LEFT},  {2+offset, RC_FRONT},  {20+offset, RC_RIGHT}, {18+offset, RC_REAR},
                               {0+offset, face},     {2+offset, face},      {20+offset, face},     {18+offset, face} ,//face
                               {1+offset, face},     {11+offset, face},     {19+offset, face},     {9+offset, face}};
        copy(&tab[0][0], &tab[19][2], &permut[0][0]);
    }


    for(int i(0);i<20;i+=4)
    {
        uint32_t temp = color[permut[i][0]][permut[i][1]];
        
        color[permut[i][0]][permut[i][1]]     = color[permut[i+1][0]][permut[i+1][1]];
        color[permut[i+1][0]][permut[i+1][1]] = color[permut[i+2][0]][permut[i+2][1]];
        color[permut[i+2][0]][permut[i+2][1]] = color[permut[i+3][0]][permut[i+3][1]];
        color[permut[i+3][0]][permut[i+3][1]] = temp;
    }
}

void rc_shuffle_cube(uint32_t nbshuffle)
{
    for(int i(0);i<nbshuffle;i++)
    {
        rc_handle_rotation(colors, (rc_face)(rand()%6), 1);
    }
}

int main(int argc, char **argv)
{
    
    uint32_t deltaTime = 0.0f; // Time between current frame and last frame
    uint32_t lastFrame = 0.0f; // Time of last frame
    int last_x;
    int last_y;
    bool firstmouse = true;
    
    //init
    if(SDL_Init(SDL_INIT_VIDEO) < 0)
    {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "[debug] > %s", SDL_GetError());
        return 1;
    }
    
    // SDL
    SDL_Window *win; SDL_Renderer *re;
    SDL_CreateWindowAndRenderer(800, 800, 0, &win, &re);
    if(win == NULL || re == NULL)
    {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "[debug] > %s", SDL_GetError());
        SDL_Quit();
        return 1;
    }
    SDL_SetWindowTitle(win, "Rubic's Cube");

    if(!gladLoadGLLoader(SDL_GL_GetProcAddress))
    {
        printf("Error initialise Glad\n");
        return 1;
    }
   
    //GL
    glEnable(GL_DEPTH_TEST);
    glViewport(0, 0, 800, 800);
    glPolygonMode(GL_FRONT_AND_BACK, GL_TRIANGLE_STRIP_ADJACENCY);

    //VAO
    unsigned int VAO;
    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);
    // cree un buffer dans le GPU
    unsigned int VBO;
    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), &vertices[0], GL_STATIC_DRAW);
    //EBO
    unsigned int EBO;
    glGenBuffers(1,&EBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices),&indices[0],GL_STATIC_DRAW);
    //shader
    Shader program("shader/vertex.glsl", "shader/fragment.glsl");
    program.use();
    //format des données d'entrées
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6*sizeof(GL_FLOAT), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6*sizeof(GL_FLOAT), (void*)(3*sizeof(GL_FLOAT)));
    glEnableVertexAttribArray(1);
    
    //camera
    Camera camera(glm::vec3(2, 2, 4), glm::vec3(0.0, 1.0, 0.0), -117.0, -21.0);

    glm::mat4 projection;
    projection = glm::perspective(glm::radians(45.0f), (float)800/(float)800, 0.1f, 100.0f);
    glUniformMatrix4fv(glGetUniformLocation(program.ID, "projection"), 1, GL_FALSE, &projection[0][0]);
    srand(SDL_GetTicks());
    rc_shuffle_cube(100);

    SDL_bool run = SDL_TRUE;
    SDL_Event events;
    bool ismoving = false;
    int32_t trigo;
    rc_face mouvement;
    uint32_t mouvement_first_frame;
    float mv_total_angle;
    while(run)
    {
        uint32_t currentFrame = SDL_GetTicks();
        deltaTime = (currentFrame - lastFrame);
        lastFrame = currentFrame;
        
        while(SDL_PollEvent(&events))
        {
            switch (events.type)
            {
            case SDL_QUIT:
                run = SDL_FALSE;
            break;
            case SDL_KEYDOWN:
                switch(events.key.keysym.sym)
                {
                case SDLK_ESCAPE:
                    run = SDL_FALSE;
                break;
                case SDLK_d:
                    camera.ProcessKeyboard(RIGHT, (float)deltaTime/100);
                break;
                case SDLK_z:
                    camera.ProcessKeyboard(FORWARD, (float)deltaTime/100);
                break;
                case SDLK_q:
                    camera.ProcessKeyboard(LEFT, (float)deltaTime/100);
                break;
                case SDLK_s:
                    camera.ProcessKeyboard(BACKWARD, (float)deltaTime/100);
                break;
                case SDLK_r:
                    if(!ismoving)
                    {
                        ismoving = true;
                        mouvement_first_frame = currentFrame;
                        mouvement = RC_FRONT;
                        trigo = 1;
                    }
                break;
                case SDLK_t:
                    if(!ismoving)
                    {
                        ismoving = true;
                        mouvement_first_frame = currentFrame;
                        mouvement = RC_FRONT;
                        trigo = -1;
                    }
                break;
                case SDLK_y:
                    if(!ismoving)
                    {
                        ismoving = true;
                        mouvement_first_frame = currentFrame;
                        mouvement = RC_REAR;
                        trigo = 1;
                    }
                break;
                case SDLK_u:
                    if(!ismoving)
                    {
                        ismoving = true;
                        mouvement_first_frame = currentFrame;
                        mouvement = RC_REAR;
                        trigo = -1;
                    }
                break;
                case SDLK_i:
                    if(!ismoving)
                    {
                        ismoving = true;
                        mouvement_first_frame = currentFrame;
                        mouvement = RC_LEFT;
                        trigo = 1;
                    }
                break;
                case SDLK_o:
                    if(!ismoving)
                    {
                        ismoving = true;
                        mouvement_first_frame = currentFrame;
                        mouvement = RC_LEFT;
                        trigo = -1;
                    }
                break;
                case SDLK_f:
                    if(!ismoving)
                    {
                        ismoving = true;
                        mouvement_first_frame = currentFrame;
                        mouvement = RC_RIGHT;
                        trigo = 1;
                    }
                break;
                case SDLK_g:
                    if(!ismoving)
                    {
                        ismoving = true;
                        mouvement_first_frame = currentFrame;
                        mouvement = RC_RIGHT;
                        trigo = -1;
                    }
                break;
                case SDLK_h:
                    if(!ismoving)
                    {
                        ismoving = true;
                        mouvement_first_frame = currentFrame;
                        mouvement = RC_BOTTOM;
                        trigo = 1;
                    }
                break;
                case SDLK_j:
                    if(!ismoving)
                    {
                        ismoving = true;
                        mouvement_first_frame = currentFrame;
                        mouvement = RC_BOTTOM;
                        trigo = -1;
                    }
                break;
                case SDLK_k:
                    if(!ismoving)
                    {
                        ismoving = true;
                        mouvement_first_frame = currentFrame;
                        mouvement = RC_TOP;
                        trigo = 1;
                    }
                break;
                case SDLK_l:
                    if(!ismoving)
                    {
                        ismoving = true;
                        mouvement_first_frame = currentFrame;
                        mouvement = RC_TOP;
                        trigo = -1;
                    }
                break;
                }
            break;
            case SDL_MOUSEMOTION:
                if(firstmouse)
                {
                    last_x = events.motion.x;
                    last_y = events.motion.y;
                    firstmouse = false;
                }
                else
                {
                    camera.ProcessMouseMovement(events.motion.x - last_x, last_y - events.motion.y, GL_TRUE);
                    last_x = events.motion.x;
                    last_y = events.motion.y;
                }
            break;
            }
            glUniformMatrix4fv(glGetUniformLocation(program.ID, "view"), 1, GL_FALSE, &(camera.GetViewMatrix())[0][0]);
        }

        glClearColor(0, 0, 0, 1);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        
        
        
        
        glm::mat4 model;
        mv_total_angle = RC_ROTATION_SPEED*trigo*((float)(currentFrame-mouvement_first_frame))/1000;
        if((mv_total_angle > 90.0f || mv_total_angle < -90.0f) && ismoving)
        {
            mv_total_angle = 0;
            ismoving = false;
            rc_handle_rotation(colors, mouvement, trigo);
        }

        for(int i(0);i<sizeof(cube_position)/sizeof(glm::vec3);i++)
        {
            model = glm::mat4(1.0f);
            model = glm::scale(model, glm::vec3((float)1/3, (float)1/3, (float)1/3));

            if(ismoving)
            {
                switch (mouvement)
                {
                case RC_FRONT:
                    if(i<9) //les points du front
                        model = glm::rotate(model, glm::radians(mv_total_angle), glm::vec3(0.0, 0.0, 1.0));
                break;
                case RC_REAR:
                    if(i>17)
                        model = glm::rotate(model, glm::radians(mv_total_angle), glm::vec3(0.0, 0.0, 1.0));
                break;
                case RC_LEFT:
                    if(i%3 == 0)
                        model = glm::rotate(model, glm::radians(mv_total_angle), glm::vec3(1.0, 0.0, 0.0));
                break;
                case RC_RIGHT:
                    if(i%3 == 2)
                        model = glm::rotate(model, glm::radians(mv_total_angle), glm::vec3(1.0, 0.0, 0.0));
                break;
                case RC_BOTTOM:
                    if(i%9 < 3)
                        model = glm::rotate(model, glm::radians(mv_total_angle), glm::vec3(0.0, 1.0, 0.0));
                break;
                case RC_TOP:
                    if(i%9 > 5)
                        model = glm::rotate(model, glm::radians(mv_total_angle), glm::vec3(0.0, 1.0, 0.0));
                break;
                }
            }
            model = glm::translate(model, cube_position[i]);
    
            //couleurs des cubes
            rc_bind_cube_color(vertices, colors[i]);
            glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), &vertices[0], GL_STATIC_DRAW);
            glUniformMatrix4fv(glGetUniformLocation(program.ID, "model"), 1, GL_FALSE, &model[0][0]);
            glDrawElements(GL_TRIANGLES, sizeof(indices), GL_UNSIGNED_INT, 0);
            
        }
        
        SDL_GL_SwapWindow(win);
    }
    
    

    SDL_DestroyRenderer(re);
    SDL_DestroyWindow(win);
    return 0;
}

