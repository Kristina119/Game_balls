#include "SDL.h"
#include <SDL_ttf.h>
#include <SDL_image.h>
#include <cmath>
#include <locale>
#include "SDL_mixer.h"
#include <iostream>
using namespace std;
const int SCREEN_WIDTH = 900;
const int SCREEN_HEIGHT = 800;
Mix_Chunk* Sound = NULL;
Mix_Music* fon = NULL;
#define BALL_COUNT 20

struct Rect
{
    SDL_Rect ball;
    int color;
    SDL_Texture* ballTexture;
};

struct point
{
    int x, y;
};

SDL_Texture* get_text_texture(SDL_Renderer*& renderer, char* text, TTF_Font* font)
{
    SDL_Surface* textSurface = NULL;
    SDL_Color fore_color = { 126,80,1 };
    textSurface = TTF_RenderText_Blended(font, text, fore_color);
    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, textSurface);
    SDL_FreeSurface(textSurface);
    return texture;

}

void init_balls(Rect balls[])
{
    int x=-60, y=0, w=60;
    srand(time(NULL));
    bool check;
    for (int i = 0; i < BALL_COUNT; i++)
    {
        balls[i].color = (i%2+1)+rand()%2+1;/////////
        
        w = 60;
        x = x + w;
        if (x > SCREEN_WIDTH - w)
        {
            x = w/2; y += w;
        }
        
        balls[i].ball = { x ,y ,w ,w };
        srand(time(NULL));
        
    }
}

void draw_balls(SDL_Renderer*& renderer, Rect balls[])
{
    for (int i = 0; i < BALL_COUNT; i++)
    {
        if (balls[i].ball.w == 0) continue;
        SDL_RenderCopy(renderer, balls[i].ballTexture, NULL, &balls[i].ball);
    }
}

void first_draw(SDL_Renderer*& renderer, SDL_Rect first, SDL_Texture* texture)
{
    //cout << "\nFIRST BALL x= "<<first.x<<" y= "<<first.y;
    SDL_RenderCopy(renderer, texture, NULL, &first);
}

int check_down(Rect balls[], SDL_Rect first)
{
    for (int i = 0; i < BALL_COUNT; i++)
    {
        if (sqrt((first.x + first.w / 2 - balls[i].ball.x - balls[i].ball.w / 2) * (first.x + first.w / 2 - balls[i].ball.x - balls[i].ball.w / 2) + (first.y + first.w / 2 - balls[i].ball.y - balls[i].ball.w / 2) * (first.y + first.w / 2 - balls[i].ball.y - balls[i].ball.w / 2)) <= first.w / 2 + balls[i].ball.w / 2)
            return 1;
    }
    return 0;
}

int light(SDL_Renderer*& renderer)
{
    int x, y,x1,y1,r1=1,r2=1,co,radius;
    Rect balls[BALL_COUNT];
    init_balls(balls);
    int first_color = 0;
    SDL_Rect area = { 0, 0, 900, 800 };//поле для игры
    /*SDL_Surface* colors_first[4] = {IMG_Load("1.bmp"), IMG_Load("2.bmp"), IMG_Load("3.bmp"), IMG_Load("4.bmp")};
    SDL_Texture* texture_first[4];
    for (int i = 0;i < 4;i++)
    {
        SDL_SetColorKey(colors_first[i], SDL_TRUE, SDL_MapRGB(colors_first[i]->format, 255, 255, 255));
        texture_first[i] = SDL_CreateTextureFromSurface(renderer, colors_first[i]);
        SDL_FreeSurface(colors_first[i]);
    }*/
    SDL_Surface* ballImage1 = IMG_Load("1.bmp");
    SDL_SetColorKey(ballImage1, SDL_TRUE, SDL_MapRGB(ballImage1->format, 255, 255, 255));
    SDL_Texture* ballTexture1 = SDL_CreateTextureFromSurface(renderer, ballImage1);
    SDL_FreeSurface(ballImage1);

    SDL_Rect first;
    //SDL_Rect first = { 200,400,60,60 };
    int angle = 30;
    int speed = 10;
    first = { area.w / 2 - 60,area.h - 60,60,60 };
    first_draw(renderer, first, ballTexture1);
    SDL_RenderPresent(renderer);
    point dir = { (int)(speed * cos(angle * M_PI / 180.0f)), (int)(-speed * sin(angle * M_PI / 180.0f)) };
    radius = first.w / 2;
    //radius = 90;
    
    for (int i = 0;i < BALL_COUNT;i++)//четыре текстуры для шариков
    {
        SDL_Surface* ballImage = IMG_Load("1.bmp");;
        if (balls[i].color == 1) ballImage = IMG_Load("1.bmp");
        if (balls[i].color == 2) ballImage = IMG_Load("2.bmp");
        if (balls[i].color == 3) ballImage = IMG_Load("3.bmp");
        if (balls[i].color == 4) ballImage = IMG_Load("4.bmp");
        SDL_SetColorKey(ballImage, SDL_TRUE, SDL_MapRGB(ballImage->format, 255, 255, 255));
        balls[i].ballTexture = SDL_CreateTextureFromSurface(renderer, ballImage);
        SDL_FreeSurface(ballImage);
    }

    TTF_Init();
    TTF_Font* my_font = TTF_OpenFont("gogoia-deco.ttf", 100);
    SDL_Texture* textTexture;

    int k = 0;// счет
    char text[10];
    _itoa_s(k, text, 10);
    textTexture = get_text_texture(renderer, text, my_font);

    int tries = 10;// попытки
    char text2[10];
    _itoa_s(tries, text2, 10);
    textTexture = get_text_texture(renderer, text2, my_font);

    SDL_Event event;
    bool level_quit=false;
    bool flag = false;
    while (!level_quit)
    {
        while (SDL_PollEvent(&event))
        {
            if (event.type == SDL_QUIT) return 0; //закрыть игру

            //нажали на мышь, шарик полетел
            if (event.type == SDL_MOUSEBUTTONDOWN &&
                event.button.button == SDL_BUTTON_LEFT)
            {
                SDL_GetMouseState(&x, &y);
                //dir = { x,y };
                x1 = first.x; y1 = first.y;
                int len_x = abs(x1 - x);
                int len_y = abs(y1 - y);
                r1= len_x / 20;
                r2= len_y / 20;
                
                if (x < first.x) { x -= r1 * 20;y -= r2 * 20; }
                else { x += r1 * 50;y -= r2 * 50; }
                //int len_x = sqrt((x1 - x) * (x1 - x) + (y1 - y) * (y1 - y));
                do {
                    x1 = first.x; y1 = first.y;
                    co = 0;
                    //x1 -= r1; y1 -= r2;
                    if (((x > x1) && (y > y1)) && (co == 0)) { x1 += r1; y1 += r2; co += 1; }
                    if (((x > x1) && (y < y1)) && (co == 0)) { x1 += r1; y1 -= r2; co += 1; }
                    if (((x < x1) && (y > y1)) && (co == 0)) { x1 -= r1; y1 += r2; co += 1; }
                    if (((x < x1) && (y < y1)) && (co == 0)) { x1 -= r1; y1 -= r2; co += 1; }
                    if ((x < x1) && (co == 0)) { x1 -= r1;  co += 1; }
                    if ((x > x1) && (co == 0)) { x1 += r1; co += 1; }
                    if ((y > y1) && (co == 0)) { y1 += r2; co += 1; }
                    if ((y < y1) && (co == 0)) { y1 -= r2;  co += 1; }
                    
                    first.x = x1;first.y = y1;
                    SDL_RenderClear(renderer);
                    draw_balls(renderer, balls);
                    first_draw(renderer, first, ballTexture1);
                    SDL_RenderPresent(renderer);
                    SDL_Delay(10);
                } while (x1 < SCREEN_WIDTH - first.w && y1 < SCREEN_HEIGHT - first.w && check_down(balls, first) == 0 && x1>0);
                
                if (x1 >= SCREEN_WIDTH - first.w || y1 >= SCREEN_HEIGHT - first.w || x1 <= 0)
                {
                    
                }



                cout << "DA\n";
                    
                    /*first.x += dir.x;
                    first.y += dir.y;
                    if (first.x - radius < area.x) {
                        first.x = area.x + radius;
                        dir.x *= -1;
                    }
                    if (first.x + radius > area.x + area.w) {
                        first.x = area.x + area.w - radius;
                        dir.x *= -1;
                    }
                    if (first.y - radius < area.y) {
                        first.y = area.y + radius;
                        dir.y *= -1;
                    }
                    if (first.y + radius > area.y + area.h) {
                        first.y = area.y + area.h - radius;
                        dir.y *= -1;
                    }*/

                /*
                    if (first_color == 3) first_color = 0;
                    else first_color++;

                    SDL_SetRenderDrawColor(renderer, 255, 248, 220, 0);
                    SDL_RenderClear(renderer);
                    //first_draw(renderer, first, texture_first[first_color]);

                    first_draw(renderer, first, ballTexture1);
                    SDL_RenderPresent(renderer);

                } while (check_down(balls, first) == 0);*/
            }

            SDL_SetRenderDrawColor(renderer, 255, 248, 220, 0);
            SDL_RenderClear(renderer);

            first_draw(renderer, first, ballTexture1);
            draw_balls(renderer, balls);
            SDL_RenderPresent(renderer);

        }
        
        for (int i = 0;i < BALL_COUNT;i++)
        {
            if (balls[i].ball.w != 0) flag = true;
        }
        if (!flag) level_quit = true;
        if (tries == 0) level_quit = true;
    }



    return 1;//закрыть уровень
}




int main(int argc, char** argv)
{
    int level = 1;
    SDL_Init(SDL_INIT_EVERYTHING);
    SDL_Window* window = SDL_CreateWindow("Click the balls",
        SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
        SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, 0);
    SDL_SetRenderDrawColor(renderer, 255, 248, 220, 0);
    if (level == 1) light(renderer);


    
    TTF_Quit();
    Mix_FreeChunk(Sound);
    Mix_CloseAudio();
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}