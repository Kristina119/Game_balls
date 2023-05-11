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
const int AREA_WIDTH = 600;
const int AREA_HEIGHT = 600;
Mix_Chunk* Sound = NULL;
Mix_Music* fon = NULL;
int BALL_COUNT=10;

struct Rect
{
    SDL_Rect ball;
    int color,line,number,up_number,up_i;
    bool connect,dis;
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
    int x=-60, y=0, w=60,rad=0;
    
    bool check;
    for (int i = 0; i < BALL_COUNT; i++)
    {
        srand(time(NULL)-i*i*10000);
        balls[i].color = rand()%2+1;/////////
        
        w = 60;
        x = x + w;
        if (x > SCREEN_WIDTH - w)
        {

            rad++;
            if (rad % 2 == 1) x = w / 2;
            else x = -w;
            y += w;
        }
        balls[i].line = rad;
        if (rad % 2 == 0 )balls[i].number = x / w;
        if (rad % 2 == 1)balls[i].number = (x-w/2) / w;
        balls[i].ball = { x ,y ,w ,w };
        srand(time(NULL));
        balls[i].dis = 1;
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
            return i;
    }
    return -10;
}

int check_near(Rect balls[], SDL_Rect first)
{
    for (int i = 0; i < BALL_COUNT; i++)
    {
        if (balls[i].ball.x==first.x && balls[i].ball.y==first.y)
          return i;
    }
    return -10;
}

void delete_ball(Rect balls[], int i)
{
   
    Rect* balls2 = (Rect*)malloc((BALL_COUNT-1) * sizeof(Rect));
    for (int j = 0;j < i;j++)
    {
        balls2[j] = balls[j];
        //cout << "  " << j;
    }
    for (int j = i+1;j < BALL_COUNT;j++)
    {
        balls2[j-1] = balls[j];
        //cout << "  " << j;
    }
    cout << "\nBALLCOUNT= " << BALL_COUNT;
    BALL_COUNT--;
    free(balls);
    balls = (Rect*)malloc(sizeof(Rect) * BALL_COUNT);
    for (int j = 0;j < BALL_COUNT;j++)
    {
        balls[j] = balls2[j];
    }
}

struct checking
{
    int tmp_i, tmp_j;
    Rect ball_last, ball_second;
};

int check_colors(Rect balls[],Rect main_b,int& k,int j,checking tmp)
{
    int color = main_b.color,line= main_b.line, number=main_b.number;
    
    bool rule1 = 0, rule2 = 0, flag = 0, rule3 = 0;
    int tmp_i = 0, tmp_j = j;
    Rect BALL,ball_last,ball_second;
    for (int i = BALL_COUNT-1;i>=0;i--)
    {
        
        if (balls[i].ball.w == 0 || j==i ) continue;
        rule1 = (balls[i].line == line && (balls[i].number == number - 1 || balls[i].number == number + 1));//стоит рядом в ряду
        rule2 = (balls[i].line == line - 1 || balls[i].line == line + 1) && line % 2 == 1 && (balls[i].number == number || balls[i].number == number + 1);//попал в нечетный ряд
        rule3 = (balls[i].line == line - 1 || balls[i].line == line + 1) && line % 2 == 0 && (balls[i].number == number || balls[i].number == number - 1);//в четный 
        if (rule1 || rule2 || rule3)
        {
            if (balls[i].color == color)
            {
                
                if (balls[i].ball.w != 0) k++;
                BALL = balls[i];

                tmp.ball_second = balls[i];
                tmp.tmp_i = i;
                tmp.ball_last = balls[j];
                tmp.tmp_j = j;

                balls[j].ball.w = balls[i].ball.w = 0;
                balls[j].ball.x = balls[j].ball.y = balls[i].ball.x = balls[i].ball.y = -1000;
                 balls[j].line = balls[j].number = balls[i].line = balls[i].number = -100;
                if (k>1) flag = 1;
                check_colors(balls, BALL, k, i,tmp);

                
            }
        }
    }
    
    if (flag==1) return 1;
    if (k == 1)//только два шарика
    {
        balls[tmp.tmp_i] = tmp.ball_second;
        balls[tmp.tmp_j] = tmp.ball_last;
    }
    return 0;
}

void new_ball(SDL_Renderer*& renderer, Rect balls[],SDL_Rect first,int first_color,SDL_Texture * ballTexture1,SDL_Surface *ballImage, SDL_Rect area)
{
    BALL_COUNT++;
    balls = (Rect*)realloc(balls, sizeof(Rect) * BALL_COUNT);

    balls[BALL_COUNT - 1].ball.x = first.x;
    balls[BALL_COUNT - 1].ball.w = balls[BALL_COUNT - 1].ball.h = first.w;
    balls[BALL_COUNT - 1].ball.y = first.y;
    balls[BALL_COUNT - 1].color = first_color;
    balls[BALL_COUNT - 1].ballTexture = ballTexture1;
    balls[BALL_COUNT - 1].line = first.y / first.w;
    if (balls[BALL_COUNT - 1].line % 2 == 0) balls[BALL_COUNT - 1].number = first.x / first.w;
    if (balls[BALL_COUNT - 1].line % 2 == 1) balls[BALL_COUNT - 1].number = (first.x - first.w / 2) / first.w;

    first = { area.w / 2 - 60,area.h - 60,60,60 };
    first_color++;
    if (first_color == 5) first_color = 1;

    //SDL_Surface* ballImage = IMG_Load("1.bmp");
    if (first_color == 1) ballImage = IMG_Load("1.bmp");
    if (first_color == 2) ballImage = IMG_Load("2.bmp");
    if (first_color == 3) ballImage = IMG_Load("3.bmp");
    if (first_color == 4) ballImage = IMG_Load("4.bmp");
    SDL_SetColorKey(ballImage, SDL_TRUE, SDL_MapRGB(ballImage->format, 255, 255, 255));

    ballTexture1 = SDL_CreateTextureFromSurface(renderer, ballImage);
    SDL_FreeSurface(ballImage);
    SDL_SetRenderDrawColor(renderer, 255, 248, 220, 0);
    SDL_RenderClear(renderer);
}

void up_balls(Rect balls[],Rect one_ball,int j)
{
    bool rule = 0,rule1=0;
    int line = one_ball.line, number = one_ball.number;
    for (int i =0;i<BALL_COUNT;i++)
    {
        rule = (balls[i].line == line - 1 && line % 2 == 1 && (balls[i].number == number || balls[i].number == number + 1));//есть шарик сверху справа или слева, нечетный ряд
        rule1= (balls[i].line == line - 1 && line % 2 == 0 && (balls[i].number == number || balls[i].number == number - 1));//есть шарик сверху справа или слева, четный ряд
        if (rule && line % 2 == 1) {
            balls[j].up_i = i;
            balls[j].up_number = balls[i].number;
            balls[j].connect = 1; return;
        }
        else {
            if (rule1 && line % 2 == 0) {
                balls[j].up_i = i;
                balls[j].up_number = balls[i].number;
                balls[j].connect = 1; return;
            }
            else balls[j].connect = 0;
        }
        
    }
    
}

void dissapear(SDL_Renderer*& renderer, Rect balls[], int point, int numbers[])
{
    int num;
    SDL_Surface* ballImage = IMG_Load("dissapear.bmp");
    
    SDL_SetColorKey(ballImage, SDL_TRUE, SDL_MapRGB(ballImage->format, 255, 255, 255));
    SDL_Texture* ballTexture1 = SDL_CreateTextureFromSurface(renderer, ballImage);
    SDL_FreeSurface(ballImage);
    
    for (int n = 0;n < point;n++)
    {
        num = numbers[n];
        balls[num].ballTexture = ballTexture1;
        SDL_DestroyTexture(ballTexture1);
        
    }
    for (int i = 0;i < BALL_COUNT;i++)
    {
        if (balls[i].ball.w == 0) continue;
        SDL_RenderCopy(renderer, balls[i].ballTexture, NULL, &balls[i].ball);
        
    }
    SDL_RenderPresent(renderer);
    SDL_Delay(200);

    for (int n = 0;n < point;n++)
    {
        num = numbers[n];
        balls[num].line = balls[num].number = -100;
        balls[num].ball.w = 0;
        balls[num].ball.x = balls[num].ball.y = -1000;
       
    }
}

int check_dissapear(Rect balls[], Rect cur_ball, int numbers[], int point, int num_ball, SDL_Renderer*& renderer)
{
    int neigh = -1,povtor=0,error=0;
    bool flag = 0;
    if (cur_ball.ball.w==0) return 0;
    if (cur_ball.line == 0) return 0;
    if (cur_ball.line == 1 && cur_ball.connect == 1) return 0;
    for (int i = 0;i < BALL_COUNT;i++)
    {
      if (balls[i].line == cur_ball.line && (balls[i].number == cur_ball.number - 1 || balls[i].number == cur_ball.number + 1) && povtor == 0) { neigh = i; }
      for (int n = 0;n < point;n++)
      {
          if (neigh == numbers[n]) povtor = 1;
          
      }
    }

    

    for (int i = 0;i < BALL_COUNT;i++)
    {
        

        if (cur_ball.connect==1) 
        {
            if (num_ball!= numbers[point-1] && num_ball != numbers[point - 2])
            {
                numbers[point] = num_ball;
                point++;
            }
            check_dissapear(balls, balls[cur_ball.up_i], numbers, point, cur_ball.up_i, renderer);
        }

        if (cur_ball.connect == 0 && neigh != -1)
        {
            
            if (num_ball != numbers[point-1] && num_ball != numbers[point - 2])
            {
                numbers[point] = num_ball;
                point++;
            }
            check_dissapear(balls, balls[neigh], numbers, point, neigh, renderer);
        }

        if (cur_ball.connect == 0 && neigh == -1)
        {
            if (num_ball != numbers[point-1] && num_ball != numbers[point - 2])
            {
                numbers[point] = num_ball;
                point++;
                flag = 1;
            }
            
        }
    }

    if (flag == 1)
    {
        cout << "Point = " << point << endl;
        dissapear(renderer, balls, point, numbers);
        /*for (int n = 0;n < point;n++)
        {
            int num = numbers[n];
            
            cout<<numbers[n] << "  ";
        }*/

    }
    return 1;
}

/*int check_dissapear(Rect balls[], Rect ball, int numbers[], int point, int num_ball, SDL_Renderer*& renderer)
{
    int flag = 0,tmp=-1;
    bool rule = 0,povtor=0;
    
    if (ball.line != 0)
    {
            for (int j = 0;j < BALL_COUNT;j++)
            {
                if (balls[j].ball.w==0) continue;
                if (balls[j].line == 0) continue;
                for (int n = 0;n < point;n++)
                {
                    if (j == numbers[n]) povtor = 1;
                }

                for (int j = 0;j < BALL_COUNT;j++)
                {
                    rule = (balls[j].number == ball.number - 1 || balls[j].number == ball.number + 1) && balls[j].line == ball.line && povtor == 0;//рядом в ряду есть шарик
                    if (rule == 1) {
                        tmp = j; rule = 1; break;
                    }
                    
                }
               
                
                
                if (ball.connect == 1)//есть и сверху
                {
                    if (balls[ball.up_number].line != 0) { numbers[point] = num_ball;point++; check_dissapear(balls, balls[ball.up_number], numbers, point, ball.up_number,renderer); }
                    else {
                        point = 0; break;
                    }
                }
                //есть только сбоку
                if (ball.connect == 0 && rule)
                {
                    if (balls[balls[j].up_number].line == 0) { point = 0; break; }
                    numbers[point] = num_ball;point++; check_dissapear(balls, balls[j], numbers, point, j, renderer);
                }
                //есть только сверху
                if (ball.connect == 1 && !rule && povtor==0) 
                {
                    if(balls[ball.up_number].line==0) { point = 0; break; }
                    numbers[point] = num_ball;point++; check_dissapear(balls, balls[ball.up_number], numbers, point, ball.up_number, renderer); 
                }
                if (ball.connect == 0 && !rule && povtor == 0) { flag = -1; numbers[point] = num_ball;point++; break; }//нет шаров вокруг
            }
    }
    else {
        point = 0; return -1;
    }
    if (flag == -1)
    {
        cout << "point = " << point<<endl;
        for (int i = 0;i < point;i++)
        {
            int num = numbers[i];
            
            balls[num].dis = 0;
        }
        int l = 0;
        for (int i = 0;i < BALL_COUNT;i++)
        {
            
            if (balls[i].dis == 0) {
                numbers[l] = i;
                l++;
            }
        }

        dissapear(renderer, balls, l, numbers);
        
        //for (int n = 0;n < point;n++)
        
            /*int num = numbers[n];
            balls[num].ball.w = 0;
            balls[num].ball.x = balls[num].ball.y = -1000;
            balls[num].line = balls[num].number = -100;//


        
    }
    point = 0;
    return 0;
    
}*/


int light(SDL_Renderer*& renderer)
{
    int x, y,x1,y1,r1=1,r2=1,co,radius,location,score_x,score_y,flag1,flag_end=0,wall=0,func=0;
    checking tmp{};
    Rect* balls = (Rect*)malloc(BALL_COUNT * sizeof(Rect));
    init_balls(balls);
    int first_color = 1;
    SDL_Rect area = { 0, 0, 900, 800 };//поле для игры

    SDL_Surface* ballImage = IMG_Load("1.bmp");
    SDL_SetColorKey(ballImage, SDL_TRUE, SDL_MapRGB(ballImage->format, 255, 255, 255));
    SDL_Texture* ballTexture1 = SDL_CreateTextureFromSurface(renderer, ballImage);
    SDL_FreeSurface(ballImage);

    SDL_Rect first;
    int angle = 30;
    int speed = 10;
    first = { area.w / 2 - 60,area.h - 60,60,60 };
    first_draw(renderer, first, ballTexture1);
    SDL_RenderPresent(renderer);
    radius = first.w / 2;
    
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
            flag1 = 1;
            //нажали на мышь, шарик полетел
            if (event.type == SDL_MOUSEBUTTONDOWN &&
                event.button.button == SDL_BUTTON_LEFT)
                            
            {
                wall = 0;
                SDL_GetMouseState(&x, &y);
                //dir = { x,y };
                if (flag1==1)x1 = first.x + first.w / 2; y1 = first.y + first.w / 2;
                if (flag1!=1) x1 = first.x; y1 = first.y;
                flag1 = 2; flag_end = -10;
                int len_x = abs(x1 - x);
                int len_y = abs(y1 - y);
                r1= len_x / 20;
                r2= len_y / 20;
                
                if (x < first.x) { x += first.w / 2; y += first.w / 2; x -= r1 * 1000;y -= r2 * 1000; location = -1; }
                else { x -= first.w / 2;y += first.w / 2; x += r1 * 1000;y -= r2 * 1000;location = 1; }
                //int len_x = sqrt((x1 - x) * (x1 - x) + (y1 - y) * (y1 - y));
                
                do { //летит в заданном направлении 
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
                    if (check_down(balls, first) == -10) SDL_Delay(10);

                    if (x1 >= SCREEN_WIDTH - first.w || x1 <= 0)//отскок от стены
                    {
                        
                        if (location == 1)
                        {
                            x -= r1 * 1000;
                            x = x - 2 * len_x;
                            x -= r1 * 1000;
                            y -= r2 * 1000;

                        }
                        if (location == -1)
                        {
                            x += r1 * 1000;
                            x += 2 * len_x;
                            x += r1 * 1000;
                            y -= r2 * 1000;
                        }
                        wall ++;
                    }

                 

                    if (check_down(balls, first) != -10 || y1 < first.w/2) //долетел до шариков или верха поля
                    {
                        int nom = check_down(balls, first);
                        bool f = 0,f2=0;
                        if (check_down(balls, first) != -10)
                        {
                            
                            if (x1 < balls[nom].ball.x + balls[nom].ball.w / 2 && y1 < balls[nom].ball.y + balls[nom].ball.w / 3 && balls[nom].ball.x - balls[nom].ball.w / 2 >= 0) { y1 = balls[nom].ball.y; x1 = balls[nom].ball.x - balls[nom].ball.w; f = 1; }
                            if (x1 >= balls[nom].ball.x + balls[nom].ball.w / 2 && y1 < balls[nom].ball.y + balls[nom].ball.w / 3 && balls[nom].ball.x + balls[nom].ball.w / 2 <= SCREEN_WIDTH - balls[nom].ball.w) { y1 = balls[nom].ball.y; x1 = balls[nom].ball.x + balls[nom].ball.w;f = 1; }


                            int tmp_x = x1, tmp_y = y1;
                            if (f != 1) y1 = balls[nom].ball.y + first.w;//////

                            //слева
                            if (x1 < balls[nom].ball.x + balls[nom].ball.w / 2 && balls[nom].ball.x - balls[nom].ball.w / 2 >= 0 && f != 1) x1 = balls[nom].ball.x - balls[nom].ball.w / 2;
                            if (x1 < balls[nom].ball.x + balls[nom].ball.w / 2 && balls[nom].ball.x - balls[nom].ball.w / 2 < 0 && f != 1) x1 = balls[nom].ball.x + balls[nom].ball.w / 2;

                            first.x = x1;first.y = y1;
                            if (check_near(balls, first) != -10 && f != 1)
                            {
                                if (balls[nom].ball.x + balls[nom].ball.w / 2 <= SCREEN_WIDTH - balls[nom].ball.w) { x1 = (balls[nom].ball.x + balls[nom].ball.w / 2); f2 = 1; first.x = x1;first.y = y1;
                                }
                            }
                            
                            //справа
                            if (x1 >= balls[nom].ball.x + balls[nom].ball.w / 2 && balls[nom].ball.x + balls[nom].ball.w / 2 <= SCREEN_WIDTH - balls[nom].ball.w && f != 1 && f2!=1) x1 = (balls[nom].ball.x + balls[nom].ball.w / 2);
                            if (x1 >= balls[nom].ball.x + balls[nom].ball.w / 2 && balls[nom].ball.x + balls[nom].ball.w / 2 > SCREEN_WIDTH - balls[nom].ball.w && f != 1 && f2 != 1) x1 = (balls[nom].ball.x - balls[nom].ball.w / 2);

                            first.x = x1;first.y = y1;
                            if (check_near(balls, first) != -10 && f != 1 && f2 != 1)
                            {
                                
                                if (balls[nom].ball.x - balls[nom].ball.w / 2 >= 0) x1 = balls[nom].ball.x - balls[nom].ball.w / 2;
                            }
                            first.x = x1;first.y = y1;
                            flag_end = 1;
                        }


                        if (y1 < first.w / 2 && flag_end!=1)
                        {
                            first.y = 0;
                            first.x = round(x1 / 60) * 60;
                            flag_end = 1;
                        }

                        BALL_COUNT++;
                        balls = (Rect*)realloc(balls, sizeof(Rect) *BALL_COUNT);
                       
                        balls[BALL_COUNT-1].ball.x = first.x;
                        balls[BALL_COUNT-1].ball.w = balls[BALL_COUNT-1].ball.h=first.w;
                        balls[BALL_COUNT-1].ball.y = first.y;
                        balls[BALL_COUNT-1].color = first_color;
                        balls[BALL_COUNT - 1].ballTexture = ballTexture1;
                        balls[BALL_COUNT - 1].line = first.y / first.w;
                        if (balls[BALL_COUNT - 1].line % 2 == 0) balls[BALL_COUNT - 1].number = first.x / first.w;
                        if (balls[BALL_COUNT - 1].line % 2 == 1) balls[BALL_COUNT - 1].number = (first.x-first.w/2) / first.w;
                        
                        first = { area.w / 2 - 60,area.h - 60,60,60 };
                        first_color++;
                        if (first_color == 5) first_color = 1;

                        //SDL_Surface* ballImage = IMG_Load("1.bmp");
                        if (first_color == 1) ballImage = IMG_Load("1.bmp");
                        if (first_color == 2) ballImage = IMG_Load("2.bmp");
                        if (first_color == 3) ballImage = IMG_Load("3.bmp");
                        if (first_color == 4) ballImage = IMG_Load("4.bmp");
                        SDL_SetColorKey(ballImage, SDL_TRUE, SDL_MapRGB(ballImage->format, 255, 255, 255));
                        
                        ballTexture1 = SDL_CreateTextureFromSurface(renderer, ballImage);
                        SDL_FreeSurface(ballImage);
                        SDL_SetRenderDrawColor(renderer, 255, 248, 220, 0);
                        SDL_RenderClear(renderer);

                        
                        func = 0;
                        
                        draw_balls(renderer, balls);
                        SDL_RenderPresent(renderer);
                        SDL_Delay(10);

                        SDL_SetRenderDrawColor(renderer, 255, 248, 220, 0);
                        SDL_RenderClear(renderer);

                        check_colors(balls, balls[BALL_COUNT - 1], func, BALL_COUNT - 1, tmp);
                        draw_balls(renderer, balls);
                        first_draw(renderer, first, ballTexture1);
                        SDL_RenderPresent(renderer);
                        
                        
                    }

                    if (wall > 1) //////////////////шарик улетел
                    {
                        first = { area.w / 2 - 60,area.h - 60,60,60 };
                        first_color++;
                        if (first_color == 5) first_color = 1;

                        //SDL_Surface* ballImage = IMG_Load("1.bmp");
                        if (first_color == 1) ballImage = IMG_Load("1.bmp");
                        if (first_color == 2) ballImage = IMG_Load("2.bmp");
                        if (first_color == 3) ballImage = IMG_Load("3.bmp");
                        if (first_color == 4) ballImage = IMG_Load("4.bmp");
                        SDL_SetColorKey(ballImage, SDL_TRUE, SDL_MapRGB(ballImage->format, 255, 255, 255));

                        ballTexture1 = SDL_CreateTextureFromSurface(renderer, ballImage);
                        SDL_FreeSurface(ballImage);
                        flag_end = 1;
                    }
                    
                    if (flag_end == 1)
                    {
                        
                        for (int i = 0;i < BALL_COUNT;i++)
                        {
                            up_balls(balls, balls[i], i);
                            
                            //cout << " ball " << i << ")  connect = " << balls[i].connect;
                        }
                        int numbers[200];
                        int skip[200],l=0;
                        for (int i = 0;i < BALL_COUNT;i++)
                        {
                            
                            if (balls[i].ball.w == 0) continue;
                            cout << " CHEK  " << i << endl;
                            check_dissapear(balls, balls[i], numbers, 0,i,renderer);
                            if (int p= check_dissapear(balls, balls[i], numbers, 0, i, renderer) == 1) {
                                skip[l] = i;  i = 0;
                            }
                            
                                SDL_SetRenderDrawColor(renderer, 255, 248, 220, 0);
                                SDL_RenderClear(renderer);
                                draw_balls(renderer, balls);
                                first_draw(renderer, first, ballTexture1);
                                SDL_RenderPresent(renderer);
                           
                        }
                    }

                } while (flag_end!=1 );
                

                    
                
                    /*if (first_color == 3) first_color = 0;
                    else first_color++;

                    SDL_SetRenderDrawColor(renderer, 255, 248, 220, 0);
                    SDL_RenderClear(renderer);
                    //first_draw(renderer, first, texture_first[first_color]);

                    first_draw(renderer, first, ballTexture1);
                    SDL_RenderPresent(renderer);*/

                
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


    for (int i = 0;i < BALL_COUNT;i++)
    {
        SDL_DestroyTexture(balls[i].ballTexture);
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