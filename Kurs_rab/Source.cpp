#include "SDL.h"
#include <SDL_ttf.h>
#include <SDL_image.h>
#include <cmath>
#include <locale>
#include "SDL_mixer.h"
#include <string>
#include <fstream>
using namespace std;
const int SCREEN_WIDTH = 1060;
const int SCREEN_HEIGHT = 800;
const int AREA_WIDTH = 960;
const int AREA_HEIGHT = 700;
//Mix_Chunk* Sound = NULL;
Mix_Music* fon = NULL;
Mix_Chunk* Sound = NULL;
int BALL_COUNT=47,tries=3,count_score=0,first_tries=3,lines=0;
int x_settings[2]={ SCREEN_WIDTH / 2 ,SCREEN_WIDTH / 2 };



struct Rect
{
    SDL_Rect ball;
    int color,line,number,up_number,up_i,line_odd_or_even;
    bool connect;
    SDL_Texture* ballTexture;
};

struct point
{
    int x, y;
};

void sound(char* name) {
    
    Sound = Mix_LoadWAV(name);
    Mix_PlayChannel(-1, Sound, 0);
}

void loadmusic() {
    fon = Mix_LoadMUS("fon.wav");
    Mix_PlayMusic(fon, -1);
}

SDL_Texture* get_text_texture(SDL_Renderer*& renderer, char* text, TTF_Font* font)
{
    SDL_Surface* textSurface = NULL;
    SDL_Color fore_color = { 255,255,31 };
    textSurface = TTF_RenderText_Blended(font, text, fore_color);
    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, textSurface);
    SDL_FreeSurface(textSurface);
    return texture;

}

SDL_Texture* get_text_texture_result(SDL_Renderer*& renderer, char* text, TTF_Font* font)
{
    SDL_Surface* textSurface = NULL;
    SDL_Color fore_color = { 255,255,31 };
    textSurface = TTF_RenderText_Blended(font, text, fore_color);
    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, textSurface);
    SDL_FreeSurface(textSurface);
    return texture;

}

void draw_text(SDL_Renderer*& renderer, SDL_Texture* texture,int var,int x,int y,int h,int dop)
{
    SDL_Rect rect;
    if (var == 9)
    {
        int k = dop;
        int len = 0;
        if (k == 0) len = 1;
        while (k != 0) {
            k /= 10;
            len++;
        }
        rect = { x,y,40 * len,h };
    }
    if (var == 1 || var==3) 
    {
        int k = count_score;
        int len = 0;
        if (k == 0) len = 1;
        while (k != 0) {
            k /= 10;
            len++;
        }
        if (var == 1) rect = { SCREEN_WIDTH / 2 - 20 * len - 10,5, 20 * len, 50 };
        if (var == 3) rect = { SCREEN_WIDTH / 2 + 20 * len - 25,SCREEN_HEIGHT / 2 + 30, 20 * len, 89 };
    }
    if (var == 2 || var==4)
    {
        int k;
        if (var==2) k = tries;
        if (var == 4) k = first_tries - tries;
        int len = 0;
        if (k == 0) len = 1;
        while (k != 0) {
            k /= 10;
            len++;
        }
        if (var == 2)rect = { SCREEN_WIDTH / 2 - 20 * len - 10,SCREEN_HEIGHT - 50, 20 * len, 50 };
        if (var == 4) rect = { SCREEN_WIDTH / 3*2-70,SCREEN_HEIGHT / 2-64, 20 * len, 89 };
    }
    if (var == 5 || var==6||var==7 || var==8)
    {
        int k;
        if (var == 5) k = BALL_COUNT;
        if (var == 6) k = lines;
        if (var == 7) k = tries;
        if (var == 8) k = count_score;
        int len = 0;
        while (k != 0) {
            k /= 10;
            len++;
        }
        rect= { x,y,40 * len,h };
    }
    SDL_RenderCopy(renderer, texture, NULL, &rect);
}

void init_balls(Rect balls[],int level)
{
    int x=-10, y=50, w=60,rad=0;
    
    bool check;
    for (int i = 0; i < BALL_COUNT; i++)
    {
        srand(time(NULL) - i * i * 10000);
        if (level == 1) balls[i].color = rand() % 4 + 1;/////////
        if (level == 2) balls[i].color = rand() % 3 + 1;

        w = 60;
        x = x + w;
        if (x > AREA_WIDTH - w + 50)
        {

            rad++;
            if (rad % 2 == 1) x = w / 2 + 50;
            else x = 50;
            y += w;
        }
        balls[i].line = rad;
        if (rad % 2 == 0) { balls[i].number = x / w; balls[i].line_odd_or_even = 0; }
        if (rad % 2 == 1) { balls[i].number = (x - w / 2) / w;balls[i].line_odd_or_even = 1; }
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
    //cout << "\nFIRST BALL x_settings= "<<first.x_settings<<" y= "<<first.y;
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
    //cout << "\nBALLCOUNT= " << BALL_COUNT;
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
    int first_ball,first_ball_line;
};

int check_colors(Rect balls[],Rect main_b,int& k,int j,checking tmp)
{
    char delsound[13] = "delete.wav";
    int color = main_b.color,line= main_b.line, number=main_b.number, e_o_line=main_b.line_odd_or_even;
    if (k == 0) {
        tmp.first_ball = number; tmp.first_ball_line = line;
    }
    bool rule12 = 0, rule2 = 0, flag = 0, rule3 = 0,rule11=0,rule_centr=0;
    int tmp_i = 0, tmp_j = j;
    Rect BALL,ball_last,ball_second;
    for (int i = BALL_COUNT-1;i>=0;i--)
    {
        
        if (balls[i].ball.w == 0 || j==i ) continue;
        //balls[i].number == tmp.first_ball - 1 || balls[i].number == tmp.first_ball + 1
        rule11 = (balls[i].line == line && balls[i].number == number - 1);//стоит слева в ряду
        rule12 = (balls[i].line == line && balls[i].number == number + 1);//стоит справа в ряду
        rule_centr = (balls[i].line ==tmp.first_ball_line && balls[i].line == line && (balls[i].number == tmp.first_ball - 1 && balls[i].number == number - 2 || balls[i].number == tmp.first_ball + 1 && balls[i].number == number + 2));
        rule2 = (balls[i].line == line - 1 || balls[i].line == line + 1) && e_o_line == 1 && (balls[i].number == number || balls[i].number == number + 1);//попал в нечетный ряд
        rule3 = (balls[i].line == line - 1 || balls[i].line == line + 1) && e_o_line == 0 && (balls[i].number == number || balls[i].number == number - 1);//в четный 
        if (rule12 || rule2 || rule3 || rule11 || rule_centr)
        {
            if (balls[i].color == color)
            {
                
                if (balls[i].ball.w != 0) { k++;count_score++; }
                BALL = balls[i];

                tmp.ball_second = balls[i];
                tmp.tmp_i = i;
                tmp.ball_last = balls[j];
                tmp.tmp_j = j;

                balls[j].ball.w = balls[i].ball.w = 0;
                balls[j].ball.x = balls[j].ball.y = balls[i].ball.x = balls[i].ball.y = -1000;
                 balls[j].line = balls[j].number = balls[i].line = balls[i].number = -100;
                
                 if (k > 1) { flag = 1; }
                check_colors(balls, BALL, k, i,tmp);

                
            }
        }
    }
    
    if (flag == 1) {
        //count_score++;
        sound(delsound);
        
        return 1;
    }
    
    if (k == 1)//только два шарика
    {
        
        balls[tmp.tmp_i] = tmp.ball_second;
        balls[tmp.tmp_j] = tmp.ball_last;
    }
    
    return 0;
}

void up_balls(Rect balls[],Rect one_ball,int j)
{
    bool rule = 0,rule1=0;
    int line = one_ball.line, number = one_ball.number,e_o_line=one_ball.line_odd_or_even;
    for (int i =0;i<BALL_COUNT;i++)
    {
        rule = (balls[i].line == line - 1 && e_o_line == 1 && (balls[i].number == number || balls[i].number == number + 1));//есть шарик сверху справа или слева, нечетный ряд
        rule1= (balls[i].line == line - 1 && e_o_line == 0 && (balls[i].number == number || balls[i].number == number - 1));//есть шарик сверху справа или слева, четный ряд
        if (rule && e_o_line == 1) {
            balls[j].up_i = i;
            balls[j].up_number = balls[i].number;
            balls[j].connect = 1; return;
        }
        else {
            if (rule1 && e_o_line == 0) {
                balls[j].up_i = i;
                balls[j].up_number = balls[i].number;
                balls[j].connect = 1; return;
            }
            else {
                balls[j].connect = 0;balls[j].up_number = -1;
            }
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
        
        
    }
   
    for (int i = 0;i < BALL_COUNT;i++)
    {
        if (balls[i].ball.w == 0) continue;
        SDL_RenderCopy(renderer, balls[i].ballTexture, NULL, &balls[i].ball);
        
    }
    
    SDL_RenderPresent(renderer);
    SDL_Delay(40);

    for (int n = 0;n < point;n++)
    {
        num = numbers[n];
        count_score++;
        balls[num].line = balls[num].number = -100;
        balls[num].ball.w = 0;
        balls[num].ball.x = balls[num].ball.y = -1000;
       
    }
    SDL_DestroyTexture(ballTexture1);
}

int check_dissapear(Rect balls[], Rect cur_ball, int numbers[], int point, int num_ball, SDL_Renderer*& renderer)
{
    int neigh = -1,povtor=0,error=0;
    bool flag = 0;
    
    if (cur_ball.line == 0) {
        return 0;
    }
    if (cur_ball.ball.w==0) return 0;
    
    if (cur_ball.line == 1 && cur_ball.connect == 1) return 0;
    for (int i = 0;i < BALL_COUNT;i++)
    {
        if (balls[i].line == cur_ball.line && (balls[i].number == cur_ball.number - 1 || balls[i].number == cur_ball.number + 1) && povtor == 0)
        {
            neigh = i;
            for (int n = 0;n < point;n++)
            {
                if (neigh == numbers[n]) { povtor = 1;neigh = -1; break; }

            }
            if (povtor == 0)break;
        }
        else neigh = -1;
    }

    if (cur_ball.connect == 1)
    {
        if (num_ball != numbers[point - 1] && num_ball != numbers[point - 2])
        {
            numbers[point] = num_ball;
            point++;
        }
        check_dissapear(balls, balls[cur_ball.up_i], numbers, point, cur_ball.up_i, renderer);
    }

    if (cur_ball.connect == 0 && neigh != -1)
    {

        if (num_ball != numbers[point - 1] && num_ball != numbers[point - 2])
        {
            numbers[point] = num_ball;
            point++;
        }
        check_dissapear(balls, balls[neigh], numbers, point, neigh, renderer);
    }

    if (cur_ball.connect == 0 && neigh == -1)
    {
        if (num_ball != numbers[point - 1] && num_ball != numbers[point - 2])
        {
            numbers[point] = num_ball;
            point++;
            flag = 1;
        }
    }
    if (flag == 1)
    {
        //count_score ++;
        //cout << "Point = " << point << endl;
        dissapear(renderer, balls, point, numbers);
        return 1;
    }
}

bool is_button_hit(SDL_Rect button, int x, int y)
{
    bool hit=0;
    hit = (x > button.x && x<(button.x + button.w) && y>button.y && y < (button.y + button.h));
    return hit;
}

void DrawCircle(SDL_Renderer* renderer, int x0, int y0, int r) {
    double u = 3.14 / 180 * 0;
    
    for (int j = 0; j < r; j++) {
        SDL_SetRenderDrawColor(renderer, 255, 255, 31, 0);
        int x1 = int(x0 + cos(u) * j);
        int y1 = int(y0 - sin(u) * j);
        for (int i = 1; i <= 360; i++) {
            u = 3.14 / 180 * i;
            int x2 = int(x0 + cos(u) * j);
            int y2 = int(y0 - sin(u) * j);
            SDL_RenderDrawLine(renderer, x1, y1, x2, y2);
            x1 = x2;
            y1 = y2;
            
        }
        
    }
}

int settings(SDL_Renderer* renderer, TTF_Font* my_font) {

    SDL_Rect menu_rect = { 0,0, SCREEN_WIDTH, SCREEN_HEIGHT };
    SDL_Surface* menu = IMG_Load("settings.bmp");
    SDL_Texture* menuTexture = SDL_CreateTextureFromSurface(renderer, menu);
    SDL_FreeSurface(menu);//задний фон меню
    char buttonsound[13] = "button.wav";

    ////////////// ползунки
    SDL_Rect square[2];
    square[0] = { SCREEN_WIDTH / 4-25,SCREEN_HEIGHT / 4-20, SCREEN_WIDTH / 2+50, SCREEN_HEIGHT / 4 };
    square[1] = { SCREEN_WIDTH / 4-25,square[0].y+ square[0].h+ square[0].h/4, SCREEN_WIDTH / 2 + 50, SCREEN_HEIGHT / 4};

    SDL_Rect sliders[2];
    sliders[0] = { SCREEN_WIDTH / 4, square[0].y+ square[0].h/2, SCREEN_WIDTH / 2, 40};
    sliders[1] = { SCREEN_WIDTH / 4, square[1].y + square[1].h / 2, SCREEN_WIDTH / 2, 40 };

    SDL_Rect back = { SCREEN_WIDTH / 4 - 25, square[1].y + square[1].h + square[1].h / 4, SCREEN_WIDTH / 2 + 50, SCREEN_HEIGHT / 10 };//выход в меню
    SDL_Surface* backImage = IMG_Load("settings_exit.bmp");
    SDL_Texture* backTexture = SDL_CreateTextureFromSurface(renderer, backImage);
    SDL_FreeSurface(backImage);

    //создание текстур кнопок
    

    SDL_Surface* sliderImage = IMG_Load("line.bmp");
    SDL_Texture* sliderTexture = SDL_CreateTextureFromSurface(renderer, sliderImage);
    SDL_FreeSurface(sliderImage);

    SDL_Surface* squareImage = IMG_Load("settings_square_mus.bmp");
    SDL_Texture* squareTexture1 = SDL_CreateTextureFromSurface(renderer, squareImage);
    SDL_FreeSurface(squareImage);

    SDL_Surface* squareImage2 = IMG_Load("settings_square_sound.bmp");
    SDL_Texture* squareTexture2 = SDL_CreateTextureFromSurface(renderer, squareImage2);
    SDL_FreeSurface(squareImage2);
    ////////////////////

    SDL_Event event;
    bool quit = false;
    while (!quit)
    {
        while (SDL_PollEvent(&event))
        {
            if (event.type == SDL_QUIT) { quit = true; exit(1); }
            if (event.button.button == SDL_BUTTON_LEFT)
            {
                for (int i = 0; i < 2; i++) if (is_button_hit(sliders[i], event.button.x, event.button.y)) { x_settings[i] = event.button.x; }

                if (is_button_hit(back, event.button.x, event.button.y)) { sound(buttonsound); quit = true; return 0; }
            }
        }

        
        Mix_VolumeMusic((x_settings[0] - 10 - (SCREEN_WIDTH / 2 - (100 * 10) / 2)) / 8);
        Mix_Volume(-1, (x_settings[1] - 10 - (SCREEN_WIDTH / 2 - (100 * 10) / 2)) / 8);
        
        SDL_RenderClear(renderer);
        SDL_SetRenderDrawColor(renderer, 0, 21, 36, 0);
        SDL_RenderCopy(renderer, menuTexture, NULL, &menu_rect);

        SDL_RenderCopy(renderer, squareTexture1, NULL, &square[0]);
        SDL_RenderCopy(renderer, squareTexture2, NULL, &square[1]);

        SDL_RenderCopy(renderer, sliderTexture, NULL, &sliders[0]);
        SDL_RenderCopy(renderer, sliderTexture, NULL, &sliders[1]);
        DrawCircle(renderer, x_settings[0], sliders[0].y+20, 20);
        DrawCircle(renderer, x_settings[1], sliders[1].y+20, 20);
        SDL_RenderCopy(renderer, backTexture, NULL, &back);
        SDL_RenderPresent(renderer);
    }
}

int draw_result(SDL_Renderer*& renderer, bool quit, SDL_Event event, int level,int type);

int middle(SDL_Renderer*& renderer)
{
    BALL_COUNT = 47;
    tries = 30;
    first_tries = 30;
    count_score = 0;
    char hitsound[13] = "hit.wav";
    char delsound[13] = "delete.wav";
    char wallsound[13] = "wall.wav";
    //char bad_finalsound[13] = "zvyk.wav";
    char good_finalsound[20] = "good_final.wav";
    char buttonsound[13] = "button.wav";
    int x, y, x1, y1, r1 = 1, r2 = 1, co, location, flag1, flag_end = 0, wall = 0, func = 0,times=0;
    checking tmp{};
    SDL_Rect area = { (SCREEN_WIDTH - AREA_WIDTH) / 2, (SCREEN_HEIGHT - AREA_HEIGHT) / 2, AREA_WIDTH,AREA_HEIGHT };//поле для игры
    SDL_Surface* background_image = IMG_Load("mm.bmp");
    SDL_Texture* background = SDL_CreateTextureFromSurface(renderer, background_image);
    SDL_FreeSurface(background_image);
    SDL_Rect back_rect = { area.x,area.y,area.w,area.h };

    SDL_Rect settings_lvl = { SCREEN_WIDTH - 45,5,45 , 45 };
    SDL_Surface* settings_lvl_image = IMG_Load("pause.bmp");
    SDL_SetColorKey(settings_lvl_image, SDL_TRUE, SDL_MapRGB(settings_lvl_image->format, 255, 255, 255));
    SDL_Texture* settings_lvl_texture = SDL_CreateTextureFromSurface(renderer, settings_lvl_image);
    SDL_FreeSurface(settings_lvl_image);
    SDL_RenderCopy(renderer, settings_lvl_texture, NULL, &settings_lvl);//пауза

    Rect* balls = (Rect*)malloc(BALL_COUNT * sizeof(Rect));//все шары
    init_balls(balls,2);
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


    SDL_Rect first;//шарик который кидают
    first = { area.w / 2 - 60 + 50,area.h - 60 + 50,60,60 };
    int first_color = 1;
    SDL_Surface* ballImage = IMG_Load("1.bmp");
    SDL_SetColorKey(ballImage, SDL_TRUE, SDL_MapRGB(ballImage->format, 255, 255, 255));
    SDL_Texture* ballTexture1 = SDL_CreateTextureFromSurface(renderer, ballImage);
    SDL_FreeSurface(ballImage);

    int radius = first.w / 2;

    TTF_Init();
    TTF_Font* my_font = TTF_OpenFont("gogoia-deco.ttf", 100);
    SDL_Texture* textTexture;

    int k = 0;// счет
    char text[10];
    _itoa_s(count_score, text, 10);
    textTexture = get_text_texture(renderer, text, my_font);

    SDL_Texture* textTexture2;
    char text2[10];
    _itoa_s(tries, text2, 10);
    textTexture2 = get_text_texture(renderer, text2, my_font);

    SDL_Event event;
    bool level_quit = false;
    bool flag = false;
    while (!level_quit)
    {
        while (SDL_PollEvent(&event))
        {
            if (event.type == SDL_QUIT) exit(1); //закрыть игру
            flag1 = 1;
            if (event.type == SDL_MOUSEBUTTONDOWN &&
                event.button.button == SDL_BUTTON_LEFT && is_button_hit(settings_lvl, event.button.x, event.button.y))
            {
                settings(renderer, my_font); continue;
            }
            //нажали на мышь, шарик полетел
            if (event.type == SDL_MOUSEBUTTONDOWN &&
                event.button.button == SDL_BUTTON_LEFT)
            {
                
                wall = 0;
                SDL_GetMouseState(&x, &y);
                if (x > AREA_WIDTH + 50 || x<50 || y>AREA_HEIGHT || y < 50) break;
                //dir = { x_settings,y };
                if (flag1 == 1)x1 = first.x + first.w / 2; y1 = first.y + first.w / 2;
                if (flag1 != 1) x1 = first.x; y1 = first.y;
                flag1 = 2; flag_end = -10;
                bool flag_draw_delay = 0;
                int len_x = abs(x1 - x);
                int len_y = abs(y1 - y);
                r1 = len_x / 20;
                r2 = len_y / 20;

                if (x < first.x) { x += first.w / 2; y += first.w / 2; x -= r1 * 1000;y -= r2 * 1000; location = -1; }
                else { x -= first.w / 2;y += first.w / 2; x += r1 * 1000;y -= r2 * 1000;location = 1; }
                //int len_x = sqrt((x1 - x_settings) * (x1 - x_settings) + (y1 - y) * (y1 - y));

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
                    SDL_RenderCopy(renderer, background, NULL, &back_rect);
                    SDL_RenderCopy(renderer, settings_lvl_texture, NULL, &settings_lvl);
                    draw_balls(renderer, balls);
                    first_draw(renderer, first, ballTexture1);
                    textTexture = get_text_texture(renderer, text, my_font);
                    draw_text(renderer, textTexture, 1,0,0,0,0);
                    textTexture2 = get_text_texture(renderer, text2, my_font);
                    draw_text(renderer, textTexture2, 2,0,0,0, 0);
                    SDL_RenderPresent(renderer);
                    if (check_down(balls, first) == -10 && flag_draw_delay == 0) SDL_Delay(10);

                    if (x1 >= AREA_WIDTH - first.w + 50 || x1 <= 50)//отскок от стены
                    {
                        wall++;
                        if (wall>1) sound(wallsound);
                        if (wall<=1)sound(hitsound);
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
                        
                    }

                    if (check_down(balls, first) != -10 || y1 < first.w / 2) //долетел до шариков или верха поля
                    {
                        sound(hitsound);
                        tries--;
                        SDL_DestroyTexture(textTexture2);
                        _itoa_s(tries, text2, 10);
                        textTexture2 = get_text_texture(renderer, text2, my_font);
                        draw_text(renderer, textTexture2, 2,0,0,0, 0);

                        int nom = check_down(balls, first);
                        bool f = 0, f2 = 0;



                        if (check_down(balls, first) != -10)
                        {

                            if (x1 < balls[nom].ball.x + balls[nom].ball.w / 2 && y1 < balls[nom].ball.y + balls[nom].ball.w / 3 && balls[nom].ball.x - balls[nom].ball.w / 2 >= 50) { y1 = balls[nom].ball.y; x1 = balls[nom].ball.x - balls[nom].ball.w; f = 1; }
                            if (x1 >= balls[nom].ball.x + balls[nom].ball.w / 2 && y1 < balls[nom].ball.y + balls[nom].ball.w / 3 && balls[nom].ball.x + balls[nom].ball.w / 2 <= AREA_WIDTH + 50 - balls[nom].ball.w) { y1 = balls[nom].ball.y; x1 = balls[nom].ball.x + balls[nom].ball.w;f = 1; }


                            int tmp_x = x1, tmp_y = y1;
                            if (f != 1) y1 = balls[nom].ball.y + first.w;

                            //слева
                            if (x1 < balls[nom].ball.x + balls[nom].ball.w / 2 && balls[nom].ball.x - balls[nom].ball.w / 2 >= 50 && f != 1) x1 = balls[nom].ball.x - balls[nom].ball.w / 2;
                            if (x1 < balls[nom].ball.x + balls[nom].ball.w / 2 && balls[nom].ball.x - balls[nom].ball.w / 2 < 50 && f != 1) x1 = balls[nom].ball.x + balls[nom].ball.w / 2;

                            first.x = x1;first.y = y1;
                            if (check_near(balls, first) != -10 && f != 1)
                            {
                                if (balls[nom].ball.x + balls[nom].ball.w / 2 <= AREA_WIDTH + 50 - balls[nom].ball.w) {
                                    x1 = (balls[nom].ball.x + balls[nom].ball.w / 2); f2 = 1; first.x = x1;first.y = y1;
                                }
                            }

                            //справа
                            if (x1 >= balls[nom].ball.x + balls[nom].ball.w / 2 && balls[nom].ball.x + balls[nom].ball.w / 2 <= AREA_WIDTH + 50 - balls[nom].ball.w && f != 1 && f2 != 1) x1 = (balls[nom].ball.x + balls[nom].ball.w / 2);
                            if (x1 >= balls[nom].ball.x + balls[nom].ball.w / 2 && balls[nom].ball.x + balls[nom].ball.w / 2 > AREA_WIDTH + 50 - balls[nom].ball.w && f != 1 && f2 != 1) x1 = (balls[nom].ball.x - balls[nom].ball.w / 2);

                            first.x = x1;first.y = y1;
                            if (check_near(balls, first) != -10 && f != 1 && f2 != 1)
                            {

                                if (balls[nom].ball.x - balls[nom].ball.w / 2 >= 50) x1 = balls[nom].ball.x - balls[nom].ball.w / 2;
                            }
                            first.x = x1;first.y = y1;
                            flag_end = 1;
                        }


                        if (y1 < first.w / 2 + 50 && flag_end != 1)
                        {
                            flag_draw_delay = 1;
                            first.y = 50;
                            first.x = round(x1 / 60) * 60 + 50;
                            flag_end = 1;
                        }

                        BALL_COUNT++;
                        balls = (Rect*)realloc(balls, sizeof(Rect) * BALL_COUNT);

                        balls[BALL_COUNT - 1].ball.x = first.x;
                        balls[BALL_COUNT - 1].ball.w = balls[BALL_COUNT - 1].ball.h = first.w;
                        balls[BALL_COUNT - 1].ball.y = first.y;
                        balls[BALL_COUNT - 1].color = first_color;
                        balls[BALL_COUNT - 1].ballTexture = ballTexture1;
                        balls[BALL_COUNT - 1].line = first.y / first.w;
                        for (int i = 0;i < BALL_COUNT - 1;i++)
                        {
                            if (balls[i].line == balls[BALL_COUNT - 1].line) { balls[BALL_COUNT - 1].line_odd_or_even = balls[i].line_odd_or_even; break; }
                            if (balls[i].line == balls[BALL_COUNT - 1].line + 1 || balls[i].line == balls[BALL_COUNT - 1].line - 1)
                            {
                                if (balls[i].line_odd_or_even == 1) { balls[BALL_COUNT - 1].line_odd_or_even = 0;break; }
                                if (balls[i].line_odd_or_even == 0) { balls[BALL_COUNT - 1].line_odd_or_even = 1;break; }

                            }
                        }
                        if (balls[BALL_COUNT - 1].line_odd_or_even == 0) balls[BALL_COUNT - 1].number = first.x / first.w;
                        if (balls[BALL_COUNT - 1].line_odd_or_even == 1) balls[BALL_COUNT - 1].number = (first.x - first.w / 2) / first.w;

                        first = { area.w / 2 - 60 + 50,area.h - 60 + 50,60,60 };
                        first_color++;
                        if (first_color == 4) first_color = 1;

                        //SDL_Surface* ballImage = IMG_Load("1.bmp");
                        if (first_color == 1) ballImage = IMG_Load("1.bmp");
                        if (first_color == 2) ballImage = IMG_Load("2.bmp");
                        if (first_color == 3) ballImage = IMG_Load("3.bmp");
                        if (first_color == 4) ballImage = IMG_Load("4.bmp");
                        SDL_SetColorKey(ballImage, SDL_TRUE, SDL_MapRGB(ballImage->format, 255, 255, 255));


                        ballTexture1 = SDL_CreateTextureFromSurface(renderer, ballImage);
                        SDL_FreeSurface(ballImage);
                        SDL_SetRenderDrawColor(renderer, 0, 21, 36, 0);
                        SDL_RenderClear(renderer);
                        SDL_RenderCopy(renderer, background, NULL, &back_rect);
                        SDL_RenderCopy(renderer, settings_lvl_texture, NULL, &settings_lvl);

                        func = 0;

                        draw_balls(renderer, balls);
                        SDL_DestroyTexture(textTexture);
                        _itoa_s(count_score, text, 10);
                        textTexture = get_text_texture(renderer, text, my_font);
                        draw_text(renderer, textTexture, 1, 0, 0, 0, 0);
                        SDL_DestroyTexture(textTexture2);
                        _itoa_s(tries, text2, 10);
                        textTexture2 = get_text_texture(renderer, text2, my_font);
                        draw_text(renderer, textTexture2, 2, 0, 0, 0, 0);

                        SDL_RenderPresent(renderer);
                        SDL_Delay(10);

                        SDL_SetRenderDrawColor(renderer, 0, 21, 36, 0);
                        SDL_RenderClear(renderer);
                        SDL_RenderCopy(renderer, background, NULL, &back_rect);
                        SDL_RenderCopy(renderer, settings_lvl_texture, NULL, &settings_lvl);
                        check_colors(balls, balls[BALL_COUNT - 1], func, BALL_COUNT - 1, tmp);
                        if (func == 1) count_score--;


                        draw_balls(renderer, balls);
                        first_draw(renderer, first, ballTexture1);
                        SDL_DestroyTexture(textTexture);
                        //sound(hitsound);
                        _itoa_s(count_score, text, 10);
                        textTexture = get_text_texture(renderer, text, my_font);
                        draw_text(renderer, textTexture, 1, 0, 0, 0,0);

                        SDL_DestroyTexture(textTexture2);
                        _itoa_s(tries, text2, 10);
                        textTexture2 = get_text_texture(renderer, text2, my_font);
                        draw_text(renderer, textTexture2, 2, 0, 0, 0, 0);

                        //if (balls[BALL_COUNT - 1].ball.y >= AREA_HEIGHT + 50 - 120) { level_quit = true; flag_end = 1; }
                        for (int i = 0;i < BALL_COUNT;i++)
                        {
                            if (balls[i].ball.y >= AREA_HEIGHT + 50 - 120) { level_quit = true; flag_end = 1; }
                        }

                    }

                    if (wall > 1) //////////////////шарик улетел
                    {
                        tries--;
                        SDL_DestroyTexture(textTexture2);
                        _itoa_s(tries, text2, 10);
                        textTexture2 = get_text_texture(renderer, text2, my_font);
                        draw_text(renderer, textTexture2, 2, 0, 0, 0, 0);
                        first = { area.w / 2 - 60 + 50,area.h - 60 + 50,60,60 };
                        first_color++;
                        if (first_color == 5) first_color = 1;//////////////////////////////

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

                    if (flag_end == 1)//шарик уже использован
                    {
                        times++;
                        for (int i = 0;i < BALL_COUNT;i++)
                        {
                            up_balls(balls, balls[i], i);

                            //cout << " ball " << i << ")  connect = " << balls[i].connect;
                        }
                        int numbers[200];
                        int skip[200], l = 0, p;
                        bool f_check = 0;
                        for (int i = 0;i < BALL_COUNT;i++)
                        {
                            f_check = 0;
                            if (balls[i].ball.w == 0) continue;
                            if (balls[i].line == 0) continue;
                            for (int j = 0;j < l;j++)
                            {
                                if (skip[j] == i) f_check = 1;
                            }
                            if (f_check == 1) continue;
                            //check_dissapear(balls, balls[i], numbers, 0,i,renderer);|| balls[i].ball.w == 0
                            p = 0;int o = -1;
                            up_balls(balls, balls[i], i);
                            o = check_dissapear(balls, balls[i], numbers, p, i, renderer);
                            if (i == BALL_COUNT - 1)
                                int reko = 0;
                            if (o == 1) {
                                //cout << " CHEK_TRUE  " << i << endl;
                                skip[l] = i;
                                i = 0; l++;
                                int n[1];


                                SDL_SetRenderDrawColor(renderer, 0, 21, 36, 0);
                                SDL_RenderClear(renderer);
                                SDL_RenderCopy(renderer, background, NULL, &back_rect);
                                SDL_RenderCopy(renderer, settings_lvl_texture, NULL, &settings_lvl);
                                draw_balls(renderer, balls);
                                first_draw(renderer, first, ballTexture1);
                                SDL_DestroyTexture(textTexture);
                                //sound(hitsound);
                                _itoa_s(count_score, text, 10);
                                textTexture = get_text_texture(renderer, text, my_font);
                                draw_text(renderer, textTexture, 1, 0, 0, 0, 0);

                                SDL_DestroyTexture(textTexture2);
                                _itoa_s(tries, text2, 10);
                                textTexture2 = get_text_texture(renderer, text2, my_font);
                                draw_text(renderer, textTexture2, 2, 0, 0, 0, 0);
                                
                            }
                        }


                        if (times == 2) 
                        {
                            int even_or_odd = -1;
                            for (int i = 0;i < BALL_COUNT;i++)
                            {
                                if (balls[i].line == 0) even_or_odd = balls[i].line_odd_or_even;
                                balls[i].line = balls[i].line + 1;
                                balls[i].ball.y += 60;
                            }
                            int x = 0, y = 50, w = 60;
                            if (even_or_odd == 0)
                            {
                                x = 80 - w; BALL_COUNT += 15;
                                int k = BALL_COUNT - 15;
                                balls = (Rect*)realloc(balls, sizeof(Rect) * BALL_COUNT);
                                for (int i = k;i < BALL_COUNT;i++)
                                {
                                    srand(time(NULL) - i * i * 10000);
                                    balls[i].color = rand() % 3 + 1;/////////
                                    w = 60;
                                    x = x + w;
                                    balls[i].line = 0;
                                    balls[i].number = (x - w / 2) / w;
                                    balls[i].line_odd_or_even = 1;
                                    balls[i].ball = { x ,y ,w ,w };
                                    balls[i].up_i = NULL;
                                    balls[i].up_number = NULL;
                                    balls[i].connect = 1;
                                    SDL_Surface* ballImage = IMG_Load("1.bmp");;
                                    if (balls[i].color == 1) ballImage = IMG_Load("1.bmp");
                                    if (balls[i].color == 2) ballImage = IMG_Load("2.bmp");
                                    if (balls[i].color == 3) ballImage = IMG_Load("3.bmp");
                                    if (balls[i].color == 4) ballImage = IMG_Load("4.bmp");
                                    SDL_SetColorKey(ballImage, SDL_TRUE, SDL_MapRGB(ballImage->format, 255, 255, 255));
                                    balls[i].ballTexture = SDL_CreateTextureFromSurface(renderer, ballImage);
                                    SDL_FreeSurface(ballImage);

                                }

                            }
                            if (even_or_odd == 1)
                            {
                                x = -10;
                                BALL_COUNT += 16;
                                int n = BALL_COUNT - 16;
                                balls = (Rect*)realloc(balls, sizeof(Rect) * BALL_COUNT);
                                for (int i = n;i < BALL_COUNT;i++)
                                {
                                    srand(time(NULL) - i * i * 10000);
                                    balls[i].color = rand() % 3 + 1;/////////
                                    w = 60;
                                    x = x + w;
                                    balls[i].line = 0;
                                    balls[i].number = x / w;
                                    balls[i].line_odd_or_even = 0;
                                    balls[i].ball = { x ,y ,w ,w };

                                    SDL_Surface* ballImage = IMG_Load("1.bmp");;
                                    if (balls[i].color == 1) ballImage = IMG_Load("1.bmp");
                                    if (balls[i].color == 2) ballImage = IMG_Load("2.bmp");
                                    if (balls[i].color == 3) ballImage = IMG_Load("3.bmp");
                                    if (balls[i].color == 4) ballImage = IMG_Load("4.bmp");
                                    SDL_SetColorKey(ballImage, SDL_TRUE, SDL_MapRGB(ballImage->format, 255, 255, 255));
                                    balls[i].ballTexture = SDL_CreateTextureFromSurface(renderer, ballImage);
                                    SDL_FreeSurface(ballImage);
                                }
                            }
                            SDL_SetRenderDrawColor(renderer, 0, 21, 36, 0);
                            SDL_RenderClear(renderer);
                            SDL_RenderCopy(renderer, background, NULL, &back_rect);
                            SDL_RenderCopy(renderer, settings_lvl_texture, NULL, &settings_lvl);
                            draw_balls(renderer, balls);
                            first_draw(renderer, first, ballTexture1);
                            times = 0;
                        }
                    }

                } while (flag_end != 1);

            }

            SDL_SetRenderDrawColor(renderer, 0, 21, 36, 0);
            SDL_RenderClear(renderer);
            SDL_RenderCopy(renderer, background, NULL, &back_rect);
            SDL_RenderCopy(renderer, settings_lvl_texture, NULL, &settings_lvl);
            first_draw(renderer, first, ballTexture1);
            draw_balls(renderer, balls);
            SDL_DestroyTexture(textTexture);
            _itoa_s(count_score, text, 10);
            textTexture = get_text_texture(renderer, text, my_font);
            draw_text(renderer, textTexture, 1, 0, 0, 0, 0);
            SDL_DestroyTexture(textTexture2);
            _itoa_s(tries, text2, 10);
            textTexture2 = get_text_texture(renderer, text2, my_font);
            draw_text(renderer, textTexture2, 2, 0, 0, 0, 0);
            SDL_RenderPresent(renderer);

            for (int i = 0;i < BALL_COUNT;i++)
            {
                if (balls[i].ball.w != 0) flag = true;
            }
            if (!flag) level_quit = true;
            if (tries == 0) level_quit = true;

        }
    }
    sound(good_finalsound);
    bool quit = false;
    while (!quit)
    {
        while (SDL_PollEvent(&event))
        {
            if (event.type == SDL_QUIT)
                exit(1);

            if (draw_result(renderer, quit, event, 2,1) == -3)
            {
                quit = true;
                return 0;////////выход в меню
            }
        }

    }
    SDL_DestroyTexture(ballTexture1);
    for (int i = 0;i < BALL_COUNT;i++)
    {
        SDL_DestroyTexture(balls[i].ballTexture);
    }
    SDL_DestroyTexture(textTexture);
    SDL_DestroyTexture(textTexture2);
    SDL_DestroyTexture(ballTexture1);
    TTF_CloseFont(my_font);
    TTF_Quit();

    return 1;
}

int light(SDL_Renderer*& renderer);
int special(SDL_Renderer*& renderer);
int chose_kolvo(SDL_Renderer*& renderer);

int draw_result(SDL_Renderer*& renderer, bool quit, SDL_Event event, int level, int type)
{
    char light_record[4], middle_record[4], special_record[4];
    int light_record_file, middle_record_file, special_record_file;
    bool f_l = 0, f_m = 0, f_s = 0;

    if (level == 1)
    {
        ifstream light_rec("record_light.txt");
        light_rec.getline(light_record, 256);
        light_record_file = atoi(light_record);
        light_rec.close();
        if (light_record_file < count_score) 
        {
            ofstream light_rec2("record_light.txt");
            light_rec2 << count_score; f_l = 1;
            light_rec2.close();
        }
    }

    if (level == 2)
    {
        ifstream middle_rec("record_middle.txt");
        middle_rec.getline(middle_record, 256);
        middle_record_file = atoi(middle_record);
        middle_rec.close();
        if (middle_record_file < count_score)
        {
            ofstream middle_rec2("record_middle.txt");
            middle_rec2<< count_score; f_m = 1;
            middle_rec2.close();
        }
    }

    if (level == 3)
    {
        ifstream special_rec("record_special.txt");
        special_rec.getline(special_record, 256);
        special_record_file = atoi(special_record);
        special_rec.close();
        if (special_record_file < count_score) 
        { 
            ofstream special_rec2("record_special.txt");
            special_rec2 << count_score; f_s = 1;
            special_rec2.close();
        }
    }
    
    //текстуры кнопок и результатов
    SDL_RenderClear(renderer);
    TTF_Font* my_font = TTF_OpenFont("gogoia-deco.ttf", 100);
    char text2[10], text[10], buttonsound[15] = "button.wav";
    SDL_Rect result_rect = { 0,0, SCREEN_WIDTH, SCREEN_HEIGHT };
    SDL_Surface* end = IMG_Load("End.bmp");
    //SDL_SetColorKey(end, SDL_TRUE, SDL_MapRGB(end->format, 255, 255, 255));
    SDL_Texture* endTexture = SDL_CreateTextureFromSurface(renderer, end);
    SDL_FreeSurface(end);
    SDL_RenderCopy(renderer, endTexture, NULL, &result_rect);
    
    int rasn = first_tries - tries;
    _itoa_s(count_score, text, 10);
    _itoa_s(rasn, text2, 10);

    
    SDL_Texture* textTexture = get_text_texture_result(renderer, text, my_font);
    SDL_Texture* textTexture2 = get_text_texture_result(renderer, text2, my_font);
    //draw_message(renderer, textTexture, mush_counter);
    draw_text(renderer, textTexture, 3, 0, 0, 0, 0);
    draw_text(renderer, textTexture2, 4, 0, 0, 0, 0);


    int x0 = -1000, y0 = -1000, w0 = -1000, h0 = -1000;
    if (f_l == 1 || f_m == 1 || f_s == 1) { x0 = SCREEN_WIDTH / 4.5;y0 = SCREEN_HEIGHT / 7.5 * 1.8; w0 = SCREEN_WIDTH / 1.7;h0 = SCREEN_WIDTH / 1.8 / 6; }

    SDL_Rect new_record = { x0, y0,w0 , h0 };
    SDL_Surface* new_record_image = IMG_Load("new_record2.bmp");
    SDL_Texture* new_record_texture = SDL_CreateTextureFromSurface(renderer, new_record_image);
    SDL_FreeSurface(new_record_image);

    char text_record[10];
    _itoa_s(count_score, text_record, 10);
    SDL_Texture* textTexture5 = get_text_texture_result(renderer, text_record, my_font);
    int x = -1000, y =-1000, h = -1000;
    if (f_l == 1 || f_m == 1 || f_s == 1) { x = new_record.x + new_record.w - 185; y = SCREEN_HEIGHT / 7.5 * 1.5 + 35; h = SCREEN_WIDTH / 1.8 / 5.8; }
   

    SDL_RenderCopy(renderer, new_record_texture, NULL, &new_record);
    draw_text(renderer, textTexture5, 8, x, y, h, 0);

    SDL_Rect menu_res = { SCREEN_WIDTH / 8, SCREEN_HEIGHT - 1.5 * 235 / 2, 235, 235 / 2 };
    SDL_Surface* menu_res_image = IMG_Load("menu_res.bmp");
    SDL_Texture* menu_res_texture = SDL_CreateTextureFromSurface(renderer, menu_res_image);
    SDL_FreeSurface(menu_res_image);
    SDL_RenderCopy(renderer, menu_res_texture, NULL, &menu_res);

    SDL_Surface* lvl_res_image = IMG_Load("lvl_res.bmp");
    SDL_Rect lvl_res = { SCREEN_WIDTH / 8 + menu_res.w * 2.4, SCREEN_HEIGHT - 1.5 * 235 / 2, 235, 235 / 2 };
    if (type==1) lvl_res_image = IMG_Load("lvl_res.bmp");
    if (type==2) lvl_res_image = IMG_Load("first_lvl_res.bmp");
    SDL_Texture* lvl_res_texture = SDL_CreateTextureFromSurface(renderer, lvl_res_image);
    SDL_FreeSurface(lvl_res_image);
    SDL_RenderCopy(renderer, lvl_res_texture, NULL, &lvl_res);
    SDL_RenderPresent(renderer);

    while (!quit)
    {
        while (SDL_PollEvent(&event))
        {
            if (event.type == SDL_QUIT) exit(1);
            if (event.type == SDL_MOUSEBUTTONDOWN && event.button.button == SDL_BUTTON_LEFT)
            {
                if (is_button_hit(menu_res, event.button.x, event.button.y))
                {
                    SDL_DestroyTexture(textTexture);
                    SDL_DestroyTexture(textTexture2);
                    TTF_CloseFont(my_font);
                    quit = true;
                    sound(buttonsound);
                    return -3;////////выход в меню
                }
                if (is_button_hit(lvl_res, event.button.x, event.button.y))
                {
                    if (level == 1)
                    {
                        sound(buttonsound);
                        int var1 = middle(renderer);
                        if (var1 == 0) return -3;
                        if (var1 == -3) return-3;
                    }
                    if (level == 2)
                    {
                        sound(buttonsound);
                        if (chose_kolvo(renderer) == -1) return -1;
                        int var2 = special(renderer);
                        if (var2 == 0) return -3;
                        if (var2 == -3) return-3;
                    }
                    if (level == 3)
                    {
                        sound(buttonsound);
                        int var3 = light(renderer);
                        if (var3 == 0) return -3;
                        if (var3 == -3) return-3;
                    }
                }
                //переход на следующий уровень

            }
        }
    }
    if (quit==true) return -3;
}

int light(SDL_Renderer*& renderer)
{
    int x, y,x1,y1,r1=1,r2=1,co,location,flag1,flag_end=0,wall=0,func=0;
    BALL_COUNT = 47+16+15;
    tries = 40;first_tries = 40;
    
    count_score = 0;
    char hitsound[13] = "hit.wav";
    char delsound[13] = "delete.wav";
    char wallsound[13] = "wall.wav";
    char good_finalsound[20] = "good_final.wav";
    char buttonsound[13] = "button.wav";
    checking tmp{};
    SDL_Rect area = { (SCREEN_WIDTH-AREA_WIDTH)/2, (SCREEN_HEIGHT-AREA_HEIGHT)/2, AREA_WIDTH,AREA_HEIGHT };//поле для игры
    SDL_Surface* background_image = IMG_Load("mm.bmp");
    SDL_Texture* background = SDL_CreateTextureFromSurface(renderer, background_image);
    SDL_FreeSurface(background_image);
    SDL_Rect back_rect = {area.x,area.y,area.w,area.h};
    
    SDL_Rect settings_lvl = { SCREEN_WIDTH -45,5,45 , 45 };
    SDL_Surface* settings_lvl_image = IMG_Load("pause.bmp");
    SDL_SetColorKey(settings_lvl_image, SDL_TRUE, SDL_MapRGB(settings_lvl_image->format, 255, 255, 255));
    SDL_Texture* settings_lvl_texture = SDL_CreateTextureFromSurface(renderer, settings_lvl_image);
    SDL_FreeSurface(settings_lvl_image);
    SDL_RenderCopy(renderer, settings_lvl_texture, NULL, &settings_lvl);//пауза


    Rect* balls = (Rect*)malloc(BALL_COUNT * sizeof(Rect));//все шары
    init_balls(balls,1);
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

    
    SDL_Rect first;//шарик который кидают
    first = { area.w/2-60+50,area.h - 60+50,60,60 };
    int first_color = 1;
    SDL_Surface* ballImage = IMG_Load("1.bmp");
    SDL_SetColorKey(ballImage, SDL_TRUE, SDL_MapRGB(ballImage->format, 255, 255, 255));
    SDL_Texture* ballTexture1 = SDL_CreateTextureFromSurface(renderer, ballImage);
    SDL_FreeSurface(ballImage);
    
    int radius = first.w / 2;
    
    /*SDL_Rect ray;//указывающий луч
    ray = { area.w / 2 - radius,area.h - 5 * radius, radius,radius * 4 };
    SDL_Surface* rayImage = IMG_Load("ray.bmp");
    SDL_SetColorKey(rayImage, SDL_TRUE, SDL_MapRGB(rayImage->format, 255, 255, 255));
    SDL_Texture* rayTexture = SDL_CreateTextureFromSurface(renderer, rayImage);
    SDL_FreeSurface(rayImage);
    
    
    */
    TTF_Init();
    TTF_Font* my_font = TTF_OpenFont("gogoia-deco.ttf", 100);
    SDL_Texture* textTexture;

    int k = 0;// счет
    char text[10];
    _itoa_s(count_score, text, 10);
    textTexture = get_text_texture(renderer, text, my_font);

    SDL_Texture* textTexture2;
    char text2[10];
    _itoa_s(tries, text2, 10);
    textTexture2 = get_text_texture(renderer, text2, my_font);

    SDL_Event event;
    bool level_quit=false;
    bool flag = false;
    while (!level_quit)
    {
        while (SDL_PollEvent(&event))
        {
            if (event.type == SDL_QUIT) exit(1);; //закрыть игру
            flag1 = 1;

            if (event.type == SDL_MOUSEBUTTONDOWN &&
                event.button.button == SDL_BUTTON_LEFT && is_button_hit(settings_lvl, event.button.x, event.button.y))
            {
                settings(renderer, my_font); continue;
            }

            //нажали на мышь, шарик полетел
            if (event.type == SDL_MOUSEBUTTONDOWN &&
                event.button.button == SDL_BUTTON_LEFT)
            {

                wall = 0;
                SDL_GetMouseState(&x, &y);
                if (x > AREA_WIDTH + 50 || x<50 || y>AREA_HEIGHT || y < 50) break;
                //dir = { x_settings,y };
                if (flag1 == 1)x1 = first.x + first.w / 2; y1 = first.y + first.w / 2;
                if (flag1 != 1) x1 = first.x; y1 = first.y;
                flag1 = 2; flag_end = -10;
                bool flag_draw_delay = 0;
                int len_x = abs(x1 - x);
                int len_y = abs(y1 - y);
                r1 = len_x / 20;
                r2 = len_y / 20;

                if (x < first.x) { x += first.w / 2; y += first.w / 2; x -= r1 * 1000;y -= r2 * 1000; location = -1; }
                else { x -= first.w / 2;y += first.w / 2; x += r1 * 1000;y -= r2 * 1000;location = 1; }
                //int len_x = sqrt((x1 - x_settings) * (x1 - x_settings) + (y1 - y) * (y1 - y));

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
                    SDL_RenderCopy(renderer, background, NULL, &back_rect);
                    draw_balls(renderer, balls);
                    first_draw(renderer, first, ballTexture1);
                    textTexture = get_text_texture(renderer, text, my_font);
                    draw_text(renderer, textTexture, 1, 0, 0, 0, 0);
                    textTexture2 = get_text_texture(renderer, text2, my_font);
                    draw_text(renderer, textTexture2, 2, 0, 0, 0, 0);
                    SDL_RenderCopy(renderer, settings_lvl_texture, NULL, &settings_lvl);
                    SDL_RenderPresent(renderer);
                    if (check_down(balls, first) == -10 && flag_draw_delay == 0) SDL_Delay(10);

                    if (x1 >= AREA_WIDTH - first.w + 50 || x1 <= 50)//отскок от стены
                    {
                        wall++;
                        if (wall>1) sound(wallsound);
                        if (wall<=1) sound(hitsound);
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
                        
                    }

                    if (check_down(balls, first) != -10 || y1 < first.w / 2) //долетел до шариков или верха поля
                    {
                        tries--;
                        sound(hitsound);
                        SDL_DestroyTexture(textTexture2);
                        _itoa_s(tries, text2, 10);
                        textTexture2 = get_text_texture(renderer, text2, my_font);
                        draw_text(renderer, textTexture2, 2, 0, 0, 0, 0);

                        int nom = check_down(balls, first);
                        bool f = 0, f2 = 0;



                        if (check_down(balls, first) != -10)
                        {

                            if (x1 < balls[nom].ball.x + balls[nom].ball.w / 2 && y1 < balls[nom].ball.y + balls[nom].ball.w / 3 && balls[nom].ball.x - balls[nom].ball.w / 2 >= 50) { y1 = balls[nom].ball.y; x1 = balls[nom].ball.x - balls[nom].ball.w; f = 1; }
                            if (x1 >= balls[nom].ball.x + balls[nom].ball.w / 2 && y1 < balls[nom].ball.y + balls[nom].ball.w / 3 && balls[nom].ball.x + balls[nom].ball.w / 2 <= AREA_WIDTH + 50 - balls[nom].ball.w) { y1 = balls[nom].ball.y; x1 = balls[nom].ball.x + balls[nom].ball.w;f = 1; }


                            int tmp_x = x1, tmp_y = y1;
                            if (f != 1) y1 = balls[nom].ball.y + first.w;

                            //слева
                            if (x1 < balls[nom].ball.x + balls[nom].ball.w / 2 && balls[nom].ball.x - balls[nom].ball.w / 2 >= 50 && f != 1) x1 = balls[nom].ball.x - balls[nom].ball.w / 2;
                            if (x1 < balls[nom].ball.x + balls[nom].ball.w / 2 && balls[nom].ball.x - balls[nom].ball.w / 2 < 50 && f != 1) x1 = balls[nom].ball.x + balls[nom].ball.w / 2;

                            first.x = x1;first.y = y1;
                            if (check_near(balls, first) != -10 && f != 1)
                            {
                                if (balls[nom].ball.x + balls[nom].ball.w / 2 <= AREA_WIDTH + 50 - balls[nom].ball.w) {
                                    x1 = (balls[nom].ball.x + balls[nom].ball.w / 2); f2 = 1; first.x = x1;first.y = y1;
                                }
                            }

                            //справа
                            if (x1 >= balls[nom].ball.x + balls[nom].ball.w / 2 && balls[nom].ball.x + balls[nom].ball.w / 2 <= AREA_WIDTH + 50 - balls[nom].ball.w && f != 1 && f2 != 1) x1 = (balls[nom].ball.x + balls[nom].ball.w / 2);
                            if (x1 >= balls[nom].ball.x + balls[nom].ball.w / 2 && balls[nom].ball.x + balls[nom].ball.w / 2 > AREA_WIDTH + 50 - balls[nom].ball.w && f != 1 && f2 != 1) x1 = (balls[nom].ball.x - balls[nom].ball.w / 2);

                            first.x = x1;first.y = y1;
                            if (check_near(balls, first) != -10 && f != 1 && f2 != 1)
                            {

                                if (balls[nom].ball.x - balls[nom].ball.w / 2 >= 50) x1 = balls[nom].ball.x - balls[nom].ball.w / 2;
                            }
                            first.x = x1;first.y = y1;
                            flag_end = 1;
                        }


                        if (y1 < first.w / 2 + 50 && flag_end != 1)
                        {
                            flag_draw_delay = 1;
                            first.y = 50;
                            first.x = round(x1 / 60) * 60 + 50;
                            flag_end = 1;
                        }


                        BALL_COUNT++;
                        balls = (Rect*)realloc(balls, sizeof(Rect) * BALL_COUNT);

                        balls[BALL_COUNT - 1].ball.x = first.x;
                        balls[BALL_COUNT - 1].ball.w = balls[BALL_COUNT - 1].ball.h = first.w;
                        balls[BALL_COUNT - 1].ball.y = first.y;
                        balls[BALL_COUNT - 1].color = first_color;
                        balls[BALL_COUNT - 1].ballTexture = ballTexture1;
                        balls[BALL_COUNT - 1].line = first.y / first.w;
                        for (int i = 0;i < BALL_COUNT - 1;i++)
                        {
                            if (balls[i].line == balls[BALL_COUNT - 1].line) { balls[BALL_COUNT - 1].line_odd_or_even = balls[i].line_odd_or_even; break; }
                            if (balls[i].line == balls[BALL_COUNT - 1].line+1 || balls[i].line == balls[BALL_COUNT - 1].line - 1) 
                            {
                                if (balls[i].line_odd_or_even == 1) {balls[BALL_COUNT - 1].line_odd_or_even = 0;break;}
                                if (balls[i].line_odd_or_even == 0) {balls[BALL_COUNT - 1].line_odd_or_even = 1;break;}
                               
                            }
                        }
                        if (balls[BALL_COUNT - 1].line_odd_or_even == 0) balls[BALL_COUNT - 1].number = (first.x) / first.w;
                        if (balls[BALL_COUNT - 1].line_odd_or_even == 1) balls[BALL_COUNT - 1].number = (first.x - first.w / 2) / first.w;

                        first = { area.w / 2 - 60 + 50,area.h - 60 + 50,60,60 };
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
                        SDL_SetRenderDrawColor(renderer, 0, 21, 36, 0);
                        SDL_RenderClear(renderer);
                        SDL_RenderCopy(renderer, background, NULL, &back_rect);
                        SDL_RenderCopy(renderer, settings_lvl_texture, NULL, &settings_lvl);

                        func = 0;

                        draw_balls(renderer, balls);
                        SDL_DestroyTexture(textTexture);
                        _itoa_s(count_score, text, 10);
                        textTexture = get_text_texture(renderer, text, my_font);
                        draw_text(renderer, textTexture, 1, 0, 0, 0, 0);
                        SDL_DestroyTexture(textTexture2);
                        _itoa_s(tries, text2, 10);
                        textTexture2 = get_text_texture(renderer, text2, my_font);
                        draw_text(renderer, textTexture2, 2, 0, 0, 0, 0);

                        SDL_RenderPresent(renderer);
                        SDL_Delay(10);

                        SDL_SetRenderDrawColor(renderer, 0, 21, 36, 0);
                        SDL_RenderClear(renderer);
                        SDL_RenderCopy(renderer, background, NULL, &back_rect);
                        SDL_RenderCopy(renderer, settings_lvl_texture, NULL, &settings_lvl);
                        check_colors(balls, balls[BALL_COUNT - 1], func, BALL_COUNT - 1, tmp);
                        if (func == 1) count_score--;


                        draw_balls(renderer, balls);
                        first_draw(renderer, first, ballTexture1);
                        SDL_DestroyTexture(textTexture);
                        //sound(hitsound);
                        _itoa_s(count_score, text, 10);
                        textTexture = get_text_texture(renderer, text, my_font);
                        draw_text(renderer, textTexture, 1, 0, 0, 0, 0);

                        SDL_DestroyTexture(textTexture2);
                        _itoa_s(tries, text2, 10);
                        textTexture2 = get_text_texture(renderer, text2, my_font);
                        draw_text(renderer, textTexture2, 2, 0, 0, 0, 0);

                        if (balls[BALL_COUNT - 1].ball.y >= AREA_HEIGHT + 50 - 120) { level_quit = true; flag_end = 1; }

                    }

                    if (wall > 1) //////////////////шарик улетел
                    {
                        tries--;
                        SDL_DestroyTexture(textTexture2);
                        _itoa_s(tries, text2, 10);
                        textTexture2 = get_text_texture(renderer, text2, my_font);
                        draw_text(renderer, textTexture2, 2, 0, 0, 0, 0);
                        first = { area.w / 2 - 60 + 50,area.h - 60 + 50,60,60 };
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
                        int skip[200], l = 0, p;
                        bool f_check = 0;
                        for (int i = 0;i < BALL_COUNT;i++)
                        {
                            f_check = 0;
                            if (balls[i].ball.w == 0) continue;
                            if (balls[i].line == 0) continue;
                            for (int j = 0;j < l;j++)
                            {
                                if (skip[j] == i) f_check = 1;
                            }
                            if (f_check == 1) continue;
                            //check_dissapear(balls, balls[i], numbers, 0,i,renderer);|| balls[i].ball.w == 0
                            p = 0;int o = -1;
                            up_balls(balls, balls[i], i);
                            o = check_dissapear(balls, balls[i], numbers, p, i, renderer);
                            if (i == BALL_COUNT - 1)
                                int reko = 0;
                            if (o == 1) {
                                //cout << " CHEK_TRUE  " << i << endl;
                                skip[l] = i;
                                i = 0; l++;
                                int n[1];


                                SDL_SetRenderDrawColor(renderer, 0, 21, 36, 0);
                                SDL_RenderClear(renderer);
                                SDL_RenderCopy(renderer, background, NULL, &back_rect);
                                SDL_RenderCopy(renderer, settings_lvl_texture, NULL, &settings_lvl);
                                draw_balls(renderer, balls);
                                first_draw(renderer, first, ballTexture1);
                                SDL_DestroyTexture(textTexture);
                                //sound(hitsound);
                                _itoa_s(count_score, text, 10);
                                textTexture = get_text_texture(renderer, text, my_font);
                                draw_text(renderer, textTexture, 1, 0, 0, 0, 0);

                                SDL_DestroyTexture(textTexture2);
                                _itoa_s(tries, text2, 10);
                                textTexture2 = get_text_texture(renderer, text2, my_font);
                                draw_text(renderer, textTexture2, 2, 0, 0, 0, 0);
                                //SDL_RenderPresent(renderer);
                                //continue;
                            }



                        }
                    }

                } while (flag_end != 1);

            }

            SDL_SetRenderDrawColor(renderer, 0, 21, 36, 0);
            SDL_RenderClear(renderer);
            SDL_RenderCopy(renderer, background, NULL, &back_rect);
            first_draw(renderer, first, ballTexture1);
            SDL_RenderCopy(renderer, settings_lvl_texture, NULL, &settings_lvl);
            draw_balls(renderer, balls);
            SDL_DestroyTexture(textTexture);
            _itoa_s(count_score, text, 10);
            textTexture = get_text_texture(renderer, text, my_font);
            draw_text(renderer, textTexture, 1, 0, 0, 0, 0);
            SDL_DestroyTexture(textTexture2);
            _itoa_s(tries, text2, 10);
            textTexture2 = get_text_texture(renderer, text2, my_font);
            draw_text(renderer, textTexture2, 2, 0, 0, 0, 0);
            SDL_RenderPresent(renderer);
            //}////////////////pole ivent

            for (int i = 0;i < BALL_COUNT;i++)
            {
                if (balls[i].ball.w != 0) flag = true;
            }
            if (!flag) level_quit = true;
            if (tries == 0) level_quit = true;

        }
        //SDL_Delay(15000);
        //break;
    }
    sound(good_finalsound);
    bool quit = false;
    SDL_Event event1;
    while (!quit)
    {
        while (SDL_PollEvent(&event1))
        {
            if (event1.type == SDL_QUIT)
                exit(1);;

            if (draw_result(renderer,quit,event1,1,1)==-3) 
            {
                quit = true;
                return 0;////////выход в меню
            }
        }

    }
    SDL_DestroyTexture(ballTexture1);
    for (int i = 0;i < BALL_COUNT;i++)
    {
        SDL_DestroyTexture(balls[i].ballTexture);
    }
    SDL_DestroyTexture(textTexture);
    SDL_DestroyTexture(textTexture2);
    SDL_DestroyTexture(ballTexture1);
    TTF_CloseFont(my_font);
    TTF_Quit();
    //return 1;//закрыть уровень
}

int level_menu(SDL_Renderer*& renderer)
{
    SDL_Rect menu_rect = { 0,0, SCREEN_WIDTH, SCREEN_HEIGHT };
    SDL_Surface* menu = IMG_Load("menu.bmp");
    SDL_Texture* menuTexture = SDL_CreateTextureFromSurface(renderer, menu);
    SDL_FreeSurface(menu);
    SDL_RenderCopy(renderer, menuTexture, NULL, &menu_rect);//задний фон меню


    SDL_Rect start_menu = { (SCREEN_WIDTH - SCREEN_WIDTH / 1.8) / 2,SCREEN_HEIGHT / 7.5 * 1.5,SCREEN_WIDTH / 1.8 , SCREEN_WIDTH / 1.8 / 5.5 };
    SDL_Surface* start_menu_image = IMG_Load("light_menu.bmp");
    SDL_Texture* start_menu_texture = SDL_CreateTextureFromSurface(renderer, start_menu_image);
    SDL_FreeSurface(start_menu_image);
    SDL_RenderCopy(renderer, start_menu_texture, NULL, &start_menu);//легкий уровень


    SDL_Rect set_menu = { (SCREEN_WIDTH - SCREEN_WIDTH / 1.8) / 2, SCREEN_HEIGHT / 7.5 * 3,SCREEN_WIDTH / 1.8 , SCREEN_WIDTH / 1.8 / 5.5 };
    SDL_Surface* set_menu_image = IMG_Load("middle_menu.bmp");
    SDL_Texture* set_menu_texture = SDL_CreateTextureFromSurface(renderer, set_menu_image);
    SDL_FreeSurface(set_menu_image);
    SDL_RenderCopy(renderer, set_menu_texture, NULL, &set_menu);//средний уровень

    SDL_Rect record_menu = { (SCREEN_WIDTH - SCREEN_WIDTH / 1.8) / 2, SCREEN_HEIGHT / 7.5 * 4.5,SCREEN_WIDTH / 1.8 , SCREEN_WIDTH / 1.8 / 5.5 };
    SDL_Surface* record_menu_image = IMG_Load("special_menu.bmp");
    SDL_Texture* record_menu_texture = SDL_CreateTextureFromSurface(renderer, record_menu_image);
    SDL_FreeSurface(record_menu_image);
    SDL_RenderCopy(renderer, record_menu_texture, NULL, &record_menu);//специальный уровень

    SDL_Rect exit_menu = { (SCREEN_WIDTH - SCREEN_WIDTH / 1.8) / 2, SCREEN_HEIGHT / 7.5 * 6,SCREEN_WIDTH / 1.8 , SCREEN_WIDTH / 1.8 / 5.5 };
    SDL_Surface* exit_menu_image = IMG_Load("exit_menu.bmp");
    SDL_Texture* exit_menu_texture = SDL_CreateTextureFromSurface(renderer, exit_menu_image);
    SDL_FreeSurface(exit_menu_image);
    SDL_RenderCopy(renderer, exit_menu_texture, NULL, &exit_menu);//выход


    SDL_RenderPresent(renderer);
    SDL_Event event;
    bool quit = false;
    while (!quit)
    {
        while (SDL_PollEvent(&event))
        {
            if (event.type == SDL_QUIT) exit(1);
            if (event.type == SDL_MOUSEBUTTONDOWN && event.button.button == SDL_BUTTON_LEFT)
            {

                if (is_button_hit(start_menu, event.button.x, event.button.y))
                {
                    return 1;////////выход в выбор уровней
                }
                if (is_button_hit(set_menu, event.button.x, event.button.y))
                {
                    return 2;//////настройки
                }
                if (is_button_hit(record_menu, event.button.x, event.button.y))
                {
                    return 3;//////рекорды
                }
                if (is_button_hit(exit_menu, event.button.x, event.button.y))
                {
                    return 4;//////выход
                }
            }
        }
    }
    if (quit == true) return -1;
}

int chose_kolvo(SDL_Renderer*& renderer)
{
    char text_lines[10], text_ball[10], text_tries[10], buttonsound[15] = "button.wav";
    bool flag = false;
    SDL_RenderClear(renderer);
    SDL_Rect menu_rect = { 0,0, SCREEN_WIDTH, SCREEN_HEIGHT };
    SDL_Surface* menu = IMG_Load("chose_kolvo.bmp");
    SDL_Texture* menuTexture = SDL_CreateTextureFromSurface(renderer, menu);
    SDL_FreeSurface(menu);
    SDL_RenderCopy(renderer, menuTexture, NULL, &menu_rect);//задний фон меню

    TTF_Font* my_font = TTF_OpenFont("gogoia-deco.ttf", 100);
    SDL_Rect result_rect = { 0,0, SCREEN_WIDTH, SCREEN_HEIGHT };
    SDL_Surface* chose = IMG_Load("chose.bmp");
    SDL_Texture* choseTexture = SDL_CreateTextureFromSurface(renderer, chose);
    SDL_FreeSurface(chose);
    SDL_RenderCopy(renderer, choseTexture, NULL, &result_rect);
    lines = 1;
    BALL_COUNT = 1;
    tries = 1; first_tries = 0;
    


    SDL_Rect chose_ball = { (SCREEN_WIDTH - SCREEN_WIDTH / 1.8) / 2,SCREEN_HEIGHT / 7.5 * 1.5,SCREEN_WIDTH / 3.5 , SCREEN_WIDTH / 1.8 / 5.5 };
    SDL_Surface* ball_menu_image = IMG_Load("chose_ball.bmp");
    SDL_Texture* ball_menu_texture = SDL_CreateTextureFromSurface(renderer, ball_menu_image);
    SDL_FreeSurface(ball_menu_image);
    SDL_RenderCopy(renderer, ball_menu_texture, NULL, &chose_ball);//кол-во шаров

    SDL_Rect chose_lines = { (SCREEN_WIDTH - SCREEN_WIDTH / 1.8) / 2, SCREEN_HEIGHT / 7.5 * 3,SCREEN_WIDTH / 3.5 , SCREEN_WIDTH / 1.8 / 5.5 };
    SDL_Surface* lines_menu_image = IMG_Load("chose_lines.bmp");
    SDL_Texture* lines_menu_texture = SDL_CreateTextureFromSurface(renderer, lines_menu_image);
    SDL_FreeSurface(lines_menu_image);
    SDL_RenderCopy(renderer, lines_menu_texture, NULL, &chose_lines);//кол-во рядов

    SDL_Rect chose_tries = { (SCREEN_WIDTH - SCREEN_WIDTH / 1.8) / 2, SCREEN_HEIGHT / 7.5 * 4.5,SCREEN_WIDTH /  3.5, SCREEN_WIDTH / 1.8 / 5.5 };
    SDL_Surface* tries_menu_image = IMG_Load("chose_tries.bmp");
    SDL_Texture* tries_menu_texture = SDL_CreateTextureFromSurface(renderer, tries_menu_image);
    SDL_FreeSurface(tries_menu_image);
    SDL_RenderCopy(renderer, tries_menu_texture, NULL, &chose_tries);//кол-во попыток

    SDL_Rect done = { (SCREEN_WIDTH - SCREEN_WIDTH / 1.8) / 2, SCREEN_HEIGHT / 7.5 * 6,SCREEN_WIDTH / 1.8 , SCREEN_WIDTH / 1.8 / 5.5 };
    SDL_Surface* done_menu_image = IMG_Load("done.bmp");
    SDL_Texture* done_menu_texture = SDL_CreateTextureFromSurface(renderer, done_menu_image);
    SDL_FreeSurface(done_menu_image);
    SDL_RenderCopy(renderer, done_menu_texture, NULL, &done);//применить
    int w = chose_ball.h / 2 - 1;
    SDL_Rect plus1 = { done.x + done.w - w, chose_ball.y,w,w };
    SDL_Rect plus2 = { done.x + done.w - w,chose_lines.y,w,w };
    SDL_Rect plus3 = { done.x + done.w - w,chose_tries.y,w,w };
    SDL_Surface* plus1_menu_image = IMG_Load("plus.bmp");
    SDL_Texture* plus1_menu_texture = SDL_CreateTextureFromSurface(renderer, plus1_menu_image);
    SDL_FreeSurface(plus1_menu_image);
    SDL_RenderCopy(renderer, plus1_menu_texture, NULL, &plus1);
    SDL_RenderCopy(renderer, plus1_menu_texture, NULL, &plus2);
    SDL_RenderCopy(renderer, plus1_menu_texture, NULL, &plus3);
    //плюсы

    SDL_Rect minus1 = { done.x + done.w - w, chose_ball.y + plus1.w + 1,w,w };
    SDL_Rect minus2 = { done.x + done.w - w,chose_lines.y + plus2.w + 1,w,w };
    SDL_Rect minus3 = { done.x + done.w - w,chose_tries.y + plus3.w + 1,w,w };
    SDL_Surface* minus1_menu_image = IMG_Load("minus.bmp");
    SDL_Texture* minus1_menu_texture = SDL_CreateTextureFromSurface(renderer, minus1_menu_image);
    SDL_FreeSurface(minus1_menu_image);
    SDL_RenderCopy(renderer, minus1_menu_texture, NULL, &minus1);
    SDL_RenderCopy(renderer, minus1_menu_texture, NULL, &minus2);
    SDL_RenderCopy(renderer, minus1_menu_texture, NULL, &minus3);//минусы


    _itoa_s(BALL_COUNT, text_ball, 10);
    SDL_Texture* textTexture_ball = get_text_texture(renderer, text_ball, my_font);
    _itoa_s(lines, text_lines, 10);
    SDL_Texture* textTexture_lines = get_text_texture(renderer, text_lines, my_font);
    _itoa_s(tries, text_tries, 10);
    SDL_Texture* textTexture_tries = get_text_texture(renderer, text_tries, my_font);

    draw_text(renderer, textTexture_tries, 7, chose_tries.x + chose_tries.w * 1.3, chose_tries.y, chose_tries.h, 0);
    draw_text(renderer, textTexture_lines, 6, chose_lines.x + chose_lines.w * 1.3, chose_lines.y, chose_lines.h, 0);
    draw_text(renderer, textTexture_ball, 5, chose_ball.x + chose_ball.w * 1.3, chose_ball.y, chose_ball.h, 0);
    SDL_RenderPresent(renderer);
    SDL_Event event;
    while (flag == false)
    {
        while (SDL_PollEvent(&event))
        {
            if (event.type == SDL_QUIT) exit(1);
            if (event.type == SDL_MOUSEBUTTONDOWN && event.button.button == SDL_BUTTON_LEFT)
            {

                SDL_RenderClear(renderer);
                SDL_RenderCopy(renderer, menuTexture, NULL, &menu_rect);
                SDL_RenderCopy(renderer, ball_menu_texture, NULL, &chose_ball);
                SDL_RenderCopy(renderer, lines_menu_texture, NULL, &chose_lines);
                SDL_RenderCopy(renderer, tries_menu_texture, NULL, &chose_tries);
                SDL_RenderCopy(renderer, done_menu_texture, NULL, &done);
                SDL_RenderCopy(renderer, plus1_menu_texture, NULL, &plus1);
                SDL_RenderCopy(renderer, plus1_menu_texture, NULL, &plus2);
                SDL_RenderCopy(renderer, plus1_menu_texture, NULL, &plus3);
                SDL_RenderCopy(renderer, minus1_menu_texture, NULL, &minus1);
                SDL_RenderCopy(renderer, minus1_menu_texture, NULL, &minus2);
                SDL_RenderCopy(renderer, minus1_menu_texture, NULL, &minus3);
                
                if (is_button_hit(plus1, event.button.x, event.button.y) == 1)
                {
                    if (BALL_COUNT < 155)
                    {
                        BALL_COUNT+=4;
                        SDL_DestroyTexture(textTexture_ball);
                        _itoa_s(BALL_COUNT, text_ball, 10);
                        textTexture_ball = get_text_texture(renderer, text_ball, my_font);
                    }
                }
                
                
                if (is_button_hit(minus1, event.button.x, event.button.y) == 1)
                {
                    if (BALL_COUNT > 2) 
                    {
                        BALL_COUNT--;
                        SDL_DestroyTexture(textTexture_ball);
                        _itoa_s(BALL_COUNT, text_ball, 10);
                        textTexture_ball = get_text_texture(renderer, text_ball, my_font);
                    }
                }
                if (is_button_hit(plus3, event.button.x, event.button.y) == 1)
                {
                    tries++;
                    SDL_DestroyTexture(textTexture_tries);
                    _itoa_s(tries, text_tries, 10);
                    textTexture_tries = get_text_texture(renderer, text_tries, my_font);
                }
                if (is_button_hit(minus3, event.button.x, event.button.y) == 1)
                {
                    if (tries > 1)
                    {
                        tries--;
                        SDL_DestroyTexture(textTexture_tries);
                        _itoa_s(tries, text_tries, 10);
                        textTexture_tries = get_text_texture(renderer, text_tries, my_font);
                    }
                }
                if (is_button_hit(plus2, event.button.x, event.button.y) == 1)
                {
                    if (lines < 3)
                    {
                        lines++;
                        SDL_DestroyTexture(textTexture_lines);
                        _itoa_s(lines, text_lines, 10);
                        textTexture_lines = get_text_texture(renderer, text_lines, my_font);
                    }
                    
                }
                if (is_button_hit(minus2, event.button.x, event.button.y) == 1)
                {
                    if (lines > 0)
                    {
                        lines--;
                        SDL_DestroyTexture(textTexture_lines);
                        _itoa_s(lines, text_lines, 10);
                        textTexture_lines = get_text_texture(renderer, text_lines, my_font);
                    }
                }


                draw_text(renderer, textTexture_tries, 7, chose_tries.x + chose_tries.w * 1.3, chose_tries.y, chose_tries.h, 0);
                draw_text(renderer, textTexture_lines, 6, chose_lines.x + chose_lines.w * 1.3, chose_lines.y, chose_lines.h, 0);
                draw_text(renderer, textTexture_ball, 5, chose_ball.x + chose_ball.w * 1.3, chose_ball.y, chose_ball.h, 0);
                SDL_RenderPresent(renderer);
                if (is_button_hit(done, event.button.x, event.button.y) == 1) flag = true;

            }
        }
    }
    first_tries = tries;
    return 0;
}

int special(SDL_Renderer*& renderer)

{
    count_score = 0;
    char hitsound[13] = "hit.wav";
    char delsound[13] = "delete.wav";

    //char bad_finalsound[13] = "zvyk.wav";
    char good_finalsound[20] = "good_final.wav";
    char buttonsound[13] = "button.wav";
    char wallsound[13] = "wall.wav";
    int x, y, x1, y1, r1 = 1, r2 = 1, co, location, flag1, flag_end = 0, wall = 0, func = 0;
    checking tmp{};
    SDL_Rect area = { (SCREEN_WIDTH - AREA_WIDTH) / 2, (SCREEN_HEIGHT - AREA_HEIGHT) / 2, AREA_WIDTH,AREA_HEIGHT };//поле для игры
    SDL_Surface* background_image = IMG_Load("mm.bmp");
    SDL_Texture* background = SDL_CreateTextureFromSurface(renderer, background_image);
    SDL_FreeSurface(background_image);
    SDL_Rect back_rect = { area.x,area.y,area.w,area.h };

    SDL_Rect settings_lvl = { SCREEN_WIDTH - 45,5,45 , 45 };
    SDL_Surface* settings_lvl_image = IMG_Load("pause.bmp");
    SDL_SetColorKey(settings_lvl_image, SDL_TRUE, SDL_MapRGB(settings_lvl_image->format, 255, 255, 255));
    SDL_Texture* settings_lvl_texture = SDL_CreateTextureFromSurface(renderer, settings_lvl_image);
    SDL_FreeSurface(settings_lvl_image);

    Rect* balls = (Rect*)malloc(BALL_COUNT * sizeof(Rect));//все шары
    init_balls(balls, 2);
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


    SDL_Rect first;//шарик который кидают
    first = { area.w / 2 - 60 + 50,area.h - 60 + 50,60,60 };
    int first_color = 1;
    SDL_Surface* ballImage = IMG_Load("1.bmp");
    SDL_SetColorKey(ballImage, SDL_TRUE, SDL_MapRGB(ballImage->format, 255, 255, 255));
    SDL_Texture* ballTexture1 = SDL_CreateTextureFromSurface(renderer, ballImage);
    SDL_FreeSurface(ballImage);

    int radius = first.w / 2;

    TTF_Init();
    TTF_Font* my_font = TTF_OpenFont("gogoia-deco.ttf", 100);
    SDL_Texture* textTexture;

    int k = 0;// счет
    char text[10];
    _itoa_s(count_score, text, 10);
    textTexture = get_text_texture(renderer, text, my_font);

    SDL_Texture* textTexture2;
    char text2[10];
    _itoa_s(tries, text2, 10);
    textTexture2 = get_text_texture(renderer, text2, my_font);

    SDL_Event event;
    bool level_quit = false;
    bool flag = false;
    while (!level_quit)
    {
        while (SDL_PollEvent(&event))
        {
            if (event.type == SDL_QUIT) exit(1); //закрыть игру
            flag1 = 1;
            if (event.type == SDL_MOUSEBUTTONDOWN &&
                event.button.button == SDL_BUTTON_LEFT && is_button_hit(settings_lvl, event.button.x, event.button.y))
            {
                settings(renderer, my_font); continue;
            }
            //нажали на мышь, шарик полетел
            if (event.type == SDL_MOUSEBUTTONDOWN &&
                event.button.button == SDL_BUTTON_LEFT)
            {
                wall = 0;
                SDL_GetMouseState(&x, &y);
                if (x > AREA_WIDTH + 50 || x<50 || y>AREA_HEIGHT || y < 50) break;
                //dir = { x_settings,y };
                if (flag1 == 1)x1 = first.x + first.w / 2; y1 = first.y + first.w / 2;
                if (flag1 != 1) x1 = first.x; y1 = first.y;
                flag1 = 2; flag_end = -10;
                bool flag_draw_delay = 0;
                int len_x = abs(x1 - x);
                int len_y = abs(y1 - y);
                r1 = len_x / 20;
                r2 = len_y / 20;

                if (x < first.x) { x += first.w / 2; y += first.w / 2; x -= r1 * 1000;y -= r2 * 1000; location = -1; }
                else { x -= first.w / 2;y += first.w / 2; x += r1 * 1000;y -= r2 * 1000;location = 1; }
                //int len_x = sqrt((x1 - x_settings) * (x1 - x_settings) + (y1 - y) * (y1 - y));

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
                    SDL_RenderCopy(renderer, background, NULL, &back_rect);
                    SDL_RenderCopy(renderer, settings_lvl_texture, NULL, &settings_lvl);
                    draw_balls(renderer, balls);
                    first_draw(renderer, first, ballTexture1);
                    textTexture = get_text_texture(renderer, text, my_font);
                    draw_text(renderer, textTexture, 1, 0, 0, 0, 0);
                    textTexture2 = get_text_texture(renderer, text2, my_font);
                    draw_text(renderer, textTexture2, 2, 0, 0, 0, 0);
                    SDL_RenderPresent(renderer);
                    if (check_down(balls, first) == -10 && flag_draw_delay == 0) SDL_Delay(10);

                    if (x1 >= AREA_WIDTH - first.w + 50 || x1 <= 50)//отскок от стены
                    {
                        wall++;
                        if (wall > 1) sound(wallsound);
                        if (wall <= 1) sound(hitsound);
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
                    }

                    if (check_down(balls, first) != -10 || y1 < first.w / 2) //долетел до шариков или верха поля
                    {
                        sound(hitsound);
                        tries--;
                        SDL_DestroyTexture(textTexture2);
                        _itoa_s(tries, text2, 10);
                        textTexture2 = get_text_texture(renderer, text2, my_font);
                        draw_text(renderer, textTexture2, 2, 0, 0, 0, 0);

                        int nom = check_down(balls, first);
                        bool f = 0, f2 = 0;



                        if (check_down(balls, first) != -10)
                        {

                            if (x1 < balls[nom].ball.x + balls[nom].ball.w / 2 && y1 < balls[nom].ball.y + balls[nom].ball.w / 3 && balls[nom].ball.x - balls[nom].ball.w / 2 >= 50) { y1 = balls[nom].ball.y; x1 = balls[nom].ball.x - balls[nom].ball.w; f = 1; }
                            if (x1 >= balls[nom].ball.x + balls[nom].ball.w / 2 && y1 < balls[nom].ball.y + balls[nom].ball.w / 3 && balls[nom].ball.x + balls[nom].ball.w / 2 <= AREA_WIDTH + 50 - balls[nom].ball.w) { y1 = balls[nom].ball.y; x1 = balls[nom].ball.x + balls[nom].ball.w;f = 1; }


                            int tmp_x = x1, tmp_y = y1;
                            if (f != 1) y1 = balls[nom].ball.y + first.w;

                            //слева
                            if (x1 < balls[nom].ball.x + balls[nom].ball.w / 2 && balls[nom].ball.x - balls[nom].ball.w / 2 >= 50 && f != 1) x1 = balls[nom].ball.x - balls[nom].ball.w / 2;
                            if (x1 < balls[nom].ball.x + balls[nom].ball.w / 2 && balls[nom].ball.x - balls[nom].ball.w / 2 < 50 && f != 1) x1 = balls[nom].ball.x + balls[nom].ball.w / 2;

                            first.x = x1;first.y = y1;
                            if (check_near(balls, first) != -10 && f != 1)
                            {
                                if (balls[nom].ball.x + balls[nom].ball.w / 2 <= AREA_WIDTH + 50 - balls[nom].ball.w) {
                                    x1 = (balls[nom].ball.x + balls[nom].ball.w / 2); f2 = 1; first.x = x1;first.y = y1;
                                }
                            }

                            //справа
                            if (x1 >= balls[nom].ball.x + balls[nom].ball.w / 2 && balls[nom].ball.x + balls[nom].ball.w / 2 <= AREA_WIDTH + 50 - balls[nom].ball.w && f != 1 && f2 != 1) x1 = (balls[nom].ball.x + balls[nom].ball.w / 2);
                            if (x1 >= balls[nom].ball.x + balls[nom].ball.w / 2 && balls[nom].ball.x + balls[nom].ball.w / 2 > AREA_WIDTH + 50 - balls[nom].ball.w && f != 1 && f2 != 1) x1 = (balls[nom].ball.x - balls[nom].ball.w / 2);

                            first.x = x1;first.y = y1;
                            if (check_near(balls, first) != -10 && f != 1 && f2 != 1)
                            {

                                if (balls[nom].ball.x - balls[nom].ball.w / 2 >= 50) x1 = balls[nom].ball.x - balls[nom].ball.w / 2;
                            }
                            first.x = x1;first.y = y1;
                            flag_end = 1;
                        }


                        if (y1 < first.w / 2 + 50 && flag_end != 1)
                        {
                            flag_draw_delay = 1;
                            first.y = 50;
                            first.x = round(x1 / 60) * 60 + 50;
                            flag_end = 1;
                        }

                        BALL_COUNT++;
                        balls = (Rect*)realloc(balls, sizeof(Rect) * BALL_COUNT);

                        balls[BALL_COUNT - 1].ball.x = first.x;
                        balls[BALL_COUNT - 1].ball.w = balls[BALL_COUNT - 1].ball.h = first.w;
                        balls[BALL_COUNT - 1].ball.y = first.y;
                        balls[BALL_COUNT - 1].color = first_color;
                        balls[BALL_COUNT - 1].ballTexture = ballTexture1;
                        balls[BALL_COUNT - 1].line = first.y / first.w;
                        for (int i = 0;i < BALL_COUNT - 1;i++)
                        {
                            if (balls[i].line == balls[BALL_COUNT - 1].line) { balls[BALL_COUNT - 1].line_odd_or_even = balls[i].line_odd_or_even; break; }
                            if (balls[i].line == balls[BALL_COUNT - 1].line + 1 || balls[i].line == balls[BALL_COUNT - 1].line - 1)
                            {
                                if (balls[i].line_odd_or_even == 1) { balls[BALL_COUNT - 1].line_odd_or_even = 0;break; }
                                if (balls[i].line_odd_or_even == 0) { balls[BALL_COUNT - 1].line_odd_or_even = 1;break; }

                            }
                        }
                        if (balls[BALL_COUNT - 1].line_odd_or_even == 0) balls[BALL_COUNT - 1].number = first.x / first.w;
                        if (balls[BALL_COUNT - 1].line_odd_or_even == 1) balls[BALL_COUNT - 1].number = (first.x - first.w / 2) / first.w;

                        first = { area.w / 2 - 60 + 50,area.h - 60 + 50,60,60 };
                        first_color++;
                        if (first_color == 4) first_color = 1;

                        //SDL_Surface* ballImage = IMG_Load("1.bmp");
                        if (first_color == 1) ballImage = IMG_Load("1.bmp");
                        if (first_color == 2) ballImage = IMG_Load("2.bmp");
                        if (first_color == 3) ballImage = IMG_Load("3.bmp");
                        if (first_color == 4) ballImage = IMG_Load("4.bmp");
                        SDL_SetColorKey(ballImage, SDL_TRUE, SDL_MapRGB(ballImage->format, 255, 255, 255));


                        ballTexture1 = SDL_CreateTextureFromSurface(renderer, ballImage);
                        SDL_FreeSurface(ballImage);
                        SDL_SetRenderDrawColor(renderer, 0, 21, 36, 0);
                        SDL_RenderClear(renderer);
                        SDL_RenderCopy(renderer, background, NULL, &back_rect);
                        SDL_RenderCopy(renderer, settings_lvl_texture, NULL, &settings_lvl);

                        func = 0;

                        draw_balls(renderer, balls);
                        SDL_DestroyTexture(textTexture);
                        _itoa_s(count_score, text, 10);
                        textTexture = get_text_texture(renderer, text, my_font);
                        draw_text(renderer, textTexture, 1, 0, 0, 0, 0);
                        SDL_DestroyTexture(textTexture2);
                        _itoa_s(tries, text2, 10);
                        textTexture2 = get_text_texture(renderer, text2, my_font);
                        draw_text(renderer, textTexture2, 2, 0, 0, 0, 0);

                        SDL_RenderPresent(renderer);
                        SDL_Delay(10);

                        SDL_SetRenderDrawColor(renderer, 0, 21, 36, 0);
                        SDL_RenderClear(renderer);
                        SDL_RenderCopy(renderer, background, NULL, &back_rect);
                        SDL_RenderCopy(renderer, settings_lvl_texture, NULL, &settings_lvl);
                        check_colors(balls, balls[BALL_COUNT - 1], func, BALL_COUNT - 1, tmp);
                        if (func == 1) count_score--;


                        draw_balls(renderer, balls);
                        first_draw(renderer, first, ballTexture1);
                        SDL_DestroyTexture(textTexture);
                        //sound(hitsound);
                        _itoa_s(count_score, text, 10);
                        textTexture = get_text_texture(renderer, text, my_font);
                        draw_text(renderer, textTexture, 1, 0, 0, 0, 0);

                        SDL_DestroyTexture(textTexture2);
                        _itoa_s(tries, text2, 10);
                        textTexture2 = get_text_texture(renderer, text2, my_font);
                        draw_text(renderer, textTexture2, 2, 0, 0, 0, 0);

                        //if (balls[BALL_COUNT - 1].ball.y >= AREA_HEIGHT + 50 - 120) { level_quit = true; flag_end = 1; }
                        for (int i = 0;i < BALL_COUNT;i++)
                        {
                            if (balls[i].ball.y >= AREA_HEIGHT + 50 - 120) { level_quit = true; flag_end = 1; }
                        }

                    }

                    if (wall > 1) //////////////////шарик улетел
                    {
                        tries--;
                        SDL_DestroyTexture(textTexture2);
                        _itoa_s(tries, text2, 10);
                        textTexture2 = get_text_texture(renderer, text2, my_font);
                        draw_text(renderer, textTexture2, 2, 0, 0, 0, 0);
                        first = { area.w / 2 - 60 + 50,area.h - 60 + 50,60,60 };
                        first_color++;
                        if (first_color == 5) first_color = 1;//////////////////////////////

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

                    if (flag_end == 1)//шарик уже использован
                    {

                        for (int i = 0;i < BALL_COUNT;i++)
                        {
                            up_balls(balls, balls[i], i);

                            //cout << " ball " << i << ")  connect = " << balls[i].connect;
                        }
                        int numbers[200];
                        int skip[200], l = 0, p;
                        bool f_check = 0;
                        for (int i = 0;i < BALL_COUNT;i++)
                        {
                            f_check = 0;
                            if (balls[i].ball.w == 0) continue;
                            if (balls[i].line == 0) continue;
                            for (int j = 0;j < l;j++)
                            {
                                if (skip[j] == i) f_check = 1;
                            }
                            if (f_check == 1) continue;
                            //check_dissapear(balls, balls[i], numbers, 0,i,renderer);|| balls[i].ball.w == 0
                            p = 0;int o = -1;
                            up_balls(balls, balls[i], i);
                            o = check_dissapear(balls, balls[i], numbers, p, i, renderer);
                            if (i == BALL_COUNT - 1)
                                int reko = 0;
                            if (o == 1) {
                                //cout << " CHEK_TRUE  " << i << endl;
                                skip[l] = i;
                                i = 0; l++;
                                int n[1];


                                SDL_SetRenderDrawColor(renderer, 0, 21, 36, 0);
                                SDL_RenderClear(renderer);
                                SDL_RenderCopy(renderer, background, NULL, &back_rect);
                                SDL_RenderCopy(renderer, settings_lvl_texture, NULL, &settings_lvl);
                                draw_balls(renderer, balls);
                                first_draw(renderer, first, ballTexture1);
                                SDL_DestroyTexture(textTexture);
                                //sound(hitsound);
                                _itoa_s(count_score, text, 10);
                                textTexture = get_text_texture(renderer, text, my_font);
                                draw_text(renderer, textTexture, 1, 0, 0, 0, 0);

                                SDL_DestroyTexture(textTexture2);
                                _itoa_s(tries, text2, 10);
                                textTexture2 = get_text_texture(renderer, text2, my_font);
                                draw_text(renderer, textTexture2, 2, 0, 0, 0, 0);

                            }
                        }
                        for (int b = 0;b < lines;b++) {
                            int even_or_odd = -1;
                            for (int i = 0;i < BALL_COUNT;i++)
                            {
                                if (balls[i].line == 0) even_or_odd = balls[i].line_odd_or_even;
                                balls[i].line = balls[i].line + 1;
                                balls[i].ball.y += 60;
                            }
                            int x = 0, y = 50, w = 60;
                            if (even_or_odd == 0)
                            {
                                x = 80 - w; BALL_COUNT += 15;
                                int k = BALL_COUNT - 15;
                                balls = (Rect*)realloc(balls, sizeof(Rect) * BALL_COUNT);
                                for (int i = k;i < BALL_COUNT;i++)
                                {
                                    srand(time(NULL) - i * i * 10000);
                                    balls[i].color = rand() % 3 + 1;/////////
                                    w = 60;
                                    x = x + w;
                                    balls[i].line = 0;
                                    balls[i].number = (x - w / 2) / w;
                                    balls[i].line_odd_or_even = 1;
                                    balls[i].ball = { x ,y ,w ,w };
                                    balls[i].up_i = NULL;
                                    balls[i].up_number = NULL;
                                    balls[i].connect = 1;
                                    SDL_Surface* ballImage = IMG_Load("1.bmp");;
                                    if (balls[i].color == 1) ballImage = IMG_Load("1.bmp");
                                    if (balls[i].color == 2) ballImage = IMG_Load("2.bmp");
                                    if (balls[i].color == 3) ballImage = IMG_Load("3.bmp");
                                    if (balls[i].color == 4) ballImage = IMG_Load("4.bmp");
                                    SDL_SetColorKey(ballImage, SDL_TRUE, SDL_MapRGB(ballImage->format, 255, 255, 255));
                                    balls[i].ballTexture = SDL_CreateTextureFromSurface(renderer, ballImage);
                                    SDL_FreeSurface(ballImage);

                                }

                            }
                            if (even_or_odd == 1)
                            {
                                x = -10;
                                BALL_COUNT += 16;
                                int n = BALL_COUNT - 16;
                                balls = (Rect*)realloc(balls, sizeof(Rect) * BALL_COUNT);
                                for (int i = n;i < BALL_COUNT;i++)
                                {
                                    srand(time(NULL) - i * i * 10000);
                                    balls[i].color = rand() % 3 + 1;/////////
                                    w = 60;
                                    x = x + w;
                                    balls[i].line = 0;
                                    balls[i].number = x / w;
                                    balls[i].line_odd_or_even = 0;
                                    balls[i].ball = { x ,y ,w ,w };

                                    SDL_Surface* ballImage = IMG_Load("1.bmp");;
                                    if (balls[i].color == 1) ballImage = IMG_Load("1.bmp");
                                    if (balls[i].color == 2) ballImage = IMG_Load("2.bmp");
                                    if (balls[i].color == 3) ballImage = IMG_Load("3.bmp");
                                    if (balls[i].color == 4) ballImage = IMG_Load("4.bmp");
                                    SDL_SetColorKey(ballImage, SDL_TRUE, SDL_MapRGB(ballImage->format, 255, 255, 255));
                                    balls[i].ballTexture = SDL_CreateTextureFromSurface(renderer, ballImage);
                                    SDL_FreeSurface(ballImage);
                                }
                            }
                            SDL_SetRenderDrawColor(renderer, 0, 21, 36, 0);
                            SDL_RenderClear(renderer);
                            SDL_RenderCopy(renderer, background, NULL, &back_rect);
                            SDL_RenderCopy(renderer, settings_lvl_texture, NULL, &settings_lvl);
                            draw_balls(renderer, balls);
                            first_draw(renderer, first, ballTexture1);
                        }
                    }

                } while (flag_end != 1);

            }

            SDL_SetRenderDrawColor(renderer, 0, 21, 36, 0);
            SDL_RenderClear(renderer);
            SDL_RenderCopy(renderer, background, NULL, &back_rect);
            SDL_RenderCopy(renderer, settings_lvl_texture, NULL, &settings_lvl);
            first_draw(renderer, first, ballTexture1);
            draw_balls(renderer, balls);
            SDL_DestroyTexture(textTexture);
            _itoa_s(count_score, text, 10);
            textTexture = get_text_texture(renderer, text, my_font);
            draw_text(renderer, textTexture, 1, 0, 0, 0, 0);
            SDL_DestroyTexture(textTexture2);
            _itoa_s(tries, text2, 10);
            textTexture2 = get_text_texture(renderer, text2, my_font);
            draw_text(renderer, textTexture2, 2, 0, 0, 0, 0);
            SDL_RenderPresent(renderer);

            for (int i = 0;i < BALL_COUNT;i++)
            {
                if (balls[i].ball.w != 0) flag = true;
            }
            if (!flag) level_quit = true;
            if (tries == 0) level_quit = true;

        }
    }
    sound(good_finalsound);
    bool quit = false;
    while (!quit)
    {
        while (SDL_PollEvent(&event))
        {
            if (event.type == SDL_QUIT)
                exit(1);

            if (draw_result(renderer, quit, event, 3,2) == -3)
            {
                quit = true;
                return 0;////////выход в меню
            }
        }

    }
    SDL_DestroyTexture(ballTexture1);
    for (int i = 0;i < BALL_COUNT;i++)
    {
        SDL_DestroyTexture(balls[i].ballTexture);
    }
    SDL_DestroyTexture(textTexture);
    SDL_DestroyTexture(textTexture2);
    SDL_DestroyTexture(ballTexture1);
    TTF_CloseFont(my_font);
    TTF_Quit();

    return 1;
}

void record_window(SDL_Renderer*& renderer)
{
    TTF_Font* my_font = TTF_OpenFont("gogoia-deco.ttf", 100);
    char light_record[4], middle_record[4], special_record[4], text1[10], text2[10], text3[10], buttonsound[15] = "button.wav";
    int light_record_file, middle_record_file, special_record_file;

    ifstream light_rec("record_light.txt");
    light_rec.getline(light_record, 256);
    light_record_file = atoi(light_record);
    light_rec.close();

    ifstream middle_rec("record_middle.txt");
    middle_rec.getline(middle_record, 256);
    middle_record_file = atoi(middle_record);
    middle_rec.close();

    ifstream special_rec("record_special.txt");
    special_rec.getline(special_record, 256);
    special_record_file = atoi(special_record);
    special_rec.close();

    SDL_RenderClear(renderer);
    SDL_Rect menu_rect = { 0,0, SCREEN_WIDTH, SCREEN_HEIGHT };
    SDL_Surface* menu = IMG_Load("menu_record.bmp");
    SDL_Texture* menuTexture = SDL_CreateTextureFromSurface(renderer, menu);
    SDL_FreeSurface(menu);
    SDL_RenderCopy(renderer, menuTexture, NULL, &menu_rect);

    SDL_Rect exit_menu = { (SCREEN_WIDTH - SCREEN_WIDTH / 1.8) / 2, SCREEN_HEIGHT / 7.5 * 6,SCREEN_WIDTH / 1.8 , SCREEN_WIDTH / 1.8 / 5.5 };
    SDL_Surface* exit_menu_image = IMG_Load("exit_menu.bmp");
    SDL_Texture* exit_menu_texture = SDL_CreateTextureFromSurface(renderer, exit_menu_image);
    SDL_FreeSurface(exit_menu_image);
    SDL_RenderCopy(renderer, exit_menu_texture, NULL, &exit_menu);//выход

    _itoa_s(light_record_file, text1, 10);
    _itoa_s(middle_record_file, text2, 10);
    _itoa_s(special_record_file, text3, 10);

    SDL_Texture* textTexture1 = get_text_texture_result(renderer, text1, my_font);
    SDL_Texture* textTexture2 = get_text_texture_result(renderer, text2, my_font);
    SDL_Texture* textTexture3 = get_text_texture_result(renderer, text3, my_font);

    draw_text(renderer, textTexture1, 9, SCREEN_WIDTH/2+SCREEN_WIDTH/6-40, SCREEN_HEIGHT/4, 80, light_record_file);
    draw_text(renderer, textTexture2, 9, SCREEN_WIDTH / 2 + SCREEN_WIDTH / 6-40, SCREEN_HEIGHT / 4 + 50+SCREEN_HEIGHT/8, 80, middle_record_file);
    draw_text(renderer, textTexture3, 9, SCREEN_WIDTH / 2 + SCREEN_WIDTH / 6-40, SCREEN_HEIGHT / 4 + 45+SCREEN_HEIGHT/3, 80, special_record_file);

    SDL_RenderPresent(renderer);
    SDL_Event event;
    bool quit = false;
    while (!quit) {
        while (SDL_PollEvent(&event))
        {
            if (event.type == SDL_QUIT) exit(1);
            if (event.type == SDL_MOUSEBUTTONDOWN && event.button.button == SDL_BUTTON_LEFT)
            {

                if (is_button_hit(exit_menu, event.button.x, event.button.y))
                {
                    sound(buttonsound);
                    return;////////выход в меню
                }
            }
        }
    }
}

int menu(SDL_Renderer*& renderer, SDL_Event event)
{
    SDL_Rect menu_rect = { 0,0, SCREEN_WIDTH, SCREEN_HEIGHT };
    SDL_Surface* menu = IMG_Load("menu.bmp");
    SDL_Texture* menuTexture = SDL_CreateTextureFromSurface(renderer, menu);
    SDL_FreeSurface(menu);
    SDL_RenderCopy(renderer, menuTexture, NULL, &menu_rect);//задний фон меню
    
    
    SDL_Rect start_menu = { (SCREEN_WIDTH - SCREEN_WIDTH / 1.8) / 2,SCREEN_HEIGHT / 7.5 * 1.5,SCREEN_WIDTH / 1.8 , SCREEN_WIDTH / 1.8 / 5.5 };
    SDL_Surface* start_menu_image = IMG_Load("start_menu.bmp");
    SDL_Texture* start_menu_texture = SDL_CreateTextureFromSurface(renderer, start_menu_image);
    SDL_FreeSurface(start_menu_image);
    SDL_RenderCopy(renderer, start_menu_texture, NULL, &start_menu);//начать игру

    
    SDL_Rect set_menu = { (SCREEN_WIDTH - SCREEN_WIDTH / 1.8) / 2, SCREEN_HEIGHT / 7.5 * 3,SCREEN_WIDTH / 1.8 , SCREEN_WIDTH / 1.8 / 5.5 };
    SDL_Surface* set_menu_image = IMG_Load("settings_menu.bmp");
    SDL_Texture* set_menu_texture = SDL_CreateTextureFromSurface(renderer, set_menu_image);
    SDL_FreeSurface(set_menu_image);
    SDL_RenderCopy(renderer, set_menu_texture, NULL, &set_menu);//настройки

    SDL_Rect record_menu = { (SCREEN_WIDTH - SCREEN_WIDTH / 1.8) / 2, SCREEN_HEIGHT / 7.5 * 4.5,SCREEN_WIDTH / 1.8 , SCREEN_WIDTH / 1.8 / 5.5 };
    SDL_Surface* record_menu_image = IMG_Load("record_menu.bmp");
    SDL_Texture* record_menu_texture = SDL_CreateTextureFromSurface(renderer, record_menu_image);
    SDL_FreeSurface(record_menu_image);
    SDL_RenderCopy(renderer, record_menu_texture, NULL, &record_menu);//рекорды

    SDL_Rect exit_menu = { (SCREEN_WIDTH - SCREEN_WIDTH / 1.8) / 2, SCREEN_HEIGHT / 7.5 * 6,SCREEN_WIDTH / 1.8 , SCREEN_WIDTH / 1.8 / 5.5 };
    SDL_Surface* exit_menu_image = IMG_Load("exit_menu.bmp");
    SDL_Texture* exit_menu_texture = SDL_CreateTextureFromSurface(renderer, exit_menu_image);
    SDL_FreeSurface(exit_menu_image);
    SDL_RenderCopy(renderer, exit_menu_texture, NULL, &exit_menu);//выход


    SDL_RenderPresent(renderer);

    if (event.type == SDL_MOUSEBUTTONDOWN && event.button.button == SDL_BUTTON_LEFT)
    {

        if (is_button_hit(start_menu, event.button.x, event.button.y))
        {
            return 1;////////выход в выбор уровней
        }
        if (is_button_hit(set_menu, event.button.x, event.button.y))
        {
            return 2;//////настройки
        }
        if (is_button_hit(record_menu, event.button.x, event.button.y))
        {
            return 3;//////рекорды
        }
        if (is_button_hit(exit_menu, event.button.x, event.button.y))
        {
            return 4;//////выход
        }
    }
}

int main(int argc, char** argv)
{
    int x[2];
    x[0] = x[1] = SCREEN_WIDTH / 2;
    Mix_Init(0);
    Mix_OpenAudio(22050, MIX_DEFAULT_FORMAT, 2, 1024);
    loadmusic();
    int menu_chose = -1;
    char buttonsound[17] = "button.wav";
    
    TTF_Init();
    TTF_Font* my_font = TTF_OpenFont("gogoia-deco.ttf", 100);

    SDL_Init(SDL_INIT_EVERYTHING);
    SDL_Window* window = SDL_CreateWindow("Click the balls",
        SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
        SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, 0);
    SDL_SetRenderDrawColor(renderer, 0, 21, 36, 0);

    SDL_Event event;
    bool quit = false;
    while (!quit)
    {

        while (SDL_PollEvent(&event))
        {
            if (event.type == SDL_QUIT) exit(1);
            menu_chose = menu(renderer, event);
            if (menu_chose == 1)
            {
                sound(buttonsound);
                int level_chose = level_menu(renderer);
                if (level_chose == 1)
                {
                    sound(buttonsound);
                    int res = light(renderer);
                    if (res == -1) return 0;
                    if (res == 0)  break;
                }
                if (level_chose == 2)
                {
                    sound(buttonsound);
                    int res2 = middle(renderer);
                    if (res2 == -1) return 0;
                    if (res2 == 0)  break;
                }
                if (level_chose == 3)
                {
                    sound(buttonsound);
                    int chose_k = chose_kolvo(renderer);
                    if (chose_k == -1) return 0;
                    if (chose_k!=-1)
                    {
                        int res3 = special(renderer);
                        if (res3 == -1) return 0;
                        if (res3 == 0)  break;
                    }
                }
                if (level_chose == 4) { sound(buttonsound); break;}
                if (level_chose == -1) { sound(buttonsound);return 0; }
            }
            if (menu_chose == 2) { sound(buttonsound); if (settings(renderer, my_font) == -3) return 0; }
            if (menu_chose == 3) { sound(buttonsound); record_window(renderer); }
            if (menu_chose == 4) { sound(buttonsound);return 0; }
        }
    }
    /*if (menu_chose == 1)
    {
        int res = light(renderer);
        if (res == 1) return 0;
        if (res == 0)  break;
    }
    if (menu_chose == 2)
    {
        int res2 = middle(renderer);
        if (res2 == 1) return 0;
        if (res2 == 0)  break;
    }
    if (menu_chose == 3);
    if (menu_chose == 4) return 0;*/

    TTF_Quit();
    Mix_FreeChunk(Sound);
    Mix_CloseAudio();
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}