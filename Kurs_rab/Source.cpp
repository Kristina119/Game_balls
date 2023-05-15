#include "SDL.h"
#include <SDL_ttf.h>
#include <SDL_image.h>
#include <cmath>
#include <locale>
#include "SDL_mixer.h"
#include <iostream>
using namespace std;
const int SCREEN_WIDTH = 1060;
const int SCREEN_HEIGHT = 800;
const int AREA_WIDTH = 960;
const int AREA_HEIGHT = 700;
Mix_Chunk* Sound = NULL;
Mix_Music* fon = NULL;
int BALL_COUNT=47,tries=3,count_score=0,first_tries=3;

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

SDL_Texture* get_text_texture(SDL_Renderer*& renderer, char* text, TTF_Font* font)
{
    SDL_Surface* textSurface = NULL;
    SDL_Color fore_color = { 180,100,55 };
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

void draw_text(SDL_Renderer*& renderer, SDL_Texture* texture,int var)
{
    SDL_Rect rect;
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
        if (var == 3) rect = { SCREEN_WIDTH / 2 + 20 * len-25,SCREEN_HEIGHT/2+30, 20 * len, 89 };
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
    SDL_RenderCopy(renderer, texture, NULL, &rect);
}

void init_balls(Rect balls[])
{
    int x=-10, y=50, w=60,rad=0;
    
    bool check;
    for (int i = 0; i < BALL_COUNT; i++)
    {
        srand(time(NULL) - i * i * 10000);
        balls[i].color = rand() % 4 + 1;/////////

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
    int first_ball,first_ball_line;
};

int check_colors(Rect balls[],Rect main_b,int& k,int j,checking tmp)
{
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
        rule11 = (balls[i].line == line && balls[i].number == number - 1);//����� ����� � ����
        rule12 = (balls[i].line == line && balls[i].number == number + 1);//����� ������ � ����
        rule_centr = (balls[i].line ==tmp.first_ball_line && balls[i].line == line && (balls[i].number == tmp.first_ball - 1 && balls[i].number == number - 2 || balls[i].number == tmp.first_ball + 1 && balls[i].number == number + 2));
        rule2 = (balls[i].line == line - 1 || balls[i].line == line + 1) && e_o_line == 1 && (balls[i].number == number || balls[i].number == number + 1);//����� � �������� ���
        rule3 = (balls[i].line == line - 1 || balls[i].line == line + 1) && e_o_line == 0 && (balls[i].number == number || balls[i].number == number - 1);//� ������ 
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
        return 1;
    }
    
    if (k == 1)//������ ��� ������
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
        rule = (balls[i].line == line - 1 && e_o_line == 1 && (balls[i].number == number || balls[i].number == number + 1));//���� ����� ������ ������ ��� �����, �������� ���
        rule1= (balls[i].line == line - 1 && e_o_line == 0 && (balls[i].number == number || balls[i].number == number - 1));//���� ����� ������ ������ ��� �����, ������ ���
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
    bool hit;
    hit = (x > button.x && x<button.x + button.w && y>button.y && y < button.y + button.h);
    return hit;
}

int draw_result(SDL_Renderer*& renderer, bool quit, SDL_Event event, int level);

int middle(SDL_Renderer*& renderer)
{
    BALL_COUNT = 47;
    tries = 30;
    first_tries = 30;
    count_score = 0;
    int x, y, x1, y1, r1 = 1, r2 = 1, co, location, flag1, flag_end = 0, wall = 0, func = 0;
    checking tmp{};
    SDL_Rect area = { (SCREEN_WIDTH - AREA_WIDTH) / 2, (SCREEN_HEIGHT - AREA_HEIGHT) / 2, AREA_WIDTH,AREA_HEIGHT };//���� ��� ����
    SDL_Surface* background_image = IMG_Load("mm.bmp");
    SDL_Texture* background = SDL_CreateTextureFromSurface(renderer, background_image);
    SDL_FreeSurface(background_image);
    SDL_Rect back_rect = { area.x,area.y,area.w,area.h };

    Rect* balls = (Rect*)malloc(BALL_COUNT * sizeof(Rect));//��� ����
    init_balls(balls);
    for (int i = 0;i < BALL_COUNT;i++)//������ �������� ��� �������
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


    SDL_Rect first;//����� ������� ������
    first = { area.w / 2 - 60 + 50,area.h - 60 + 50,60,60 };
    int first_color = 1;
    SDL_Surface* ballImage = IMG_Load("1.bmp");
    SDL_SetColorKey(ballImage, SDL_TRUE, SDL_MapRGB(ballImage->format, 255, 255, 255));
    SDL_Texture* ballTexture1 = SDL_CreateTextureFromSurface(renderer, ballImage);
    SDL_FreeSurface(ballImage);

    int radius = first.w / 2;

    /*SDL_Rect ray;//����������� ���
    ray = { area.w / 2 - radius,area.h - 5 * radius, radius,radius * 4 };
    SDL_Surface* rayImage = IMG_Load("ray.bmp");
    SDL_SetColorKey(rayImage, SDL_TRUE, SDL_MapRGB(rayImage->format, 255, 255, 255));
    SDL_Texture* rayTexture = SDL_CreateTextureFromSurface(renderer, rayImage);
    SDL_FreeSurface(rayImage);


    */
    TTF_Init();
    TTF_Font* my_font = TTF_OpenFont("gogoia-deco.ttf", 100);
    SDL_Texture* textTexture;

    int k = 0;// ����
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
            if (event.type == SDL_QUIT) return 1; //������� ����
            flag1 = 1;

            //������ �� ����, ����� �������
            if (event.type == SDL_MOUSEBUTTONDOWN &&
                event.button.button == SDL_BUTTON_LEFT)
            {
                wall = 0;
                SDL_GetMouseState(&x, &y);
                //dir = { x,y };
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
                //int len_x = sqrt((x1 - x) * (x1 - x) + (y1 - y) * (y1 - y));

                do { //����� � �������� ����������� 
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
                    draw_text(renderer, textTexture, 1);
                    textTexture2 = get_text_texture(renderer, text2, my_font);
                    draw_text(renderer, textTexture2, 2);
                    SDL_RenderPresent(renderer);
                    if (check_down(balls, first) == -10 && flag_draw_delay == 0) SDL_Delay(10);

                    if (x1 >= AREA_WIDTH - first.w + 50 || x1 <= 50)//������ �� �����
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
                        wall++;
                    }

                    if (check_down(balls, first) != -10 || y1 < first.w / 2) //������� �� ������� ��� ����� ����
                    {
                        tries--;
                        SDL_DestroyTexture(textTexture2);
                        _itoa_s(tries, text2, 10);
                        textTexture2 = get_text_texture(renderer, text2, my_font);
                        draw_text(renderer, textTexture2, 2);

                        int nom = check_down(balls, first);
                        bool f = 0, f2 = 0;



                        if (check_down(balls, first) != -10)
                        {

                            if (x1 < balls[nom].ball.x + balls[nom].ball.w / 2 && y1 < balls[nom].ball.y + balls[nom].ball.w / 3 && balls[nom].ball.x - balls[nom].ball.w / 2 >= 50) { y1 = balls[nom].ball.y; x1 = balls[nom].ball.x - balls[nom].ball.w; f = 1; }
                            if (x1 >= balls[nom].ball.x + balls[nom].ball.w / 2 && y1 < balls[nom].ball.y + balls[nom].ball.w / 3 && balls[nom].ball.x + balls[nom].ball.w / 2 <= AREA_WIDTH + 50 - balls[nom].ball.w) { y1 = balls[nom].ball.y; x1 = balls[nom].ball.x + balls[nom].ball.w;f = 1; }


                            int tmp_x = x1, tmp_y = y1;
                            if (f != 1) y1 = balls[nom].ball.y + first.w;

                            //�����
                            if (x1 < balls[nom].ball.x + balls[nom].ball.w / 2 && balls[nom].ball.x - balls[nom].ball.w / 2 >= 50 && f != 1) x1 = balls[nom].ball.x - balls[nom].ball.w / 2;
                            if (x1 < balls[nom].ball.x + balls[nom].ball.w / 2 && balls[nom].ball.x - balls[nom].ball.w / 2 < 50 && f != 1) x1 = balls[nom].ball.x + balls[nom].ball.w / 2;

                            first.x = x1;first.y = y1;
                            if (check_near(balls, first) != -10 && f != 1)
                            {
                                if (balls[nom].ball.x + balls[nom].ball.w / 2 <= AREA_WIDTH + 50 - balls[nom].ball.w) {
                                    x1 = (balls[nom].ball.x + balls[nom].ball.w / 2); f2 = 1; first.x = x1;first.y = y1;
                                }
                            }

                            //������
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


                        func = 0;

                        draw_balls(renderer, balls);
                        SDL_DestroyTexture(textTexture);
                        _itoa_s(count_score, text, 10);
                        textTexture = get_text_texture(renderer, text, my_font);
                        draw_text(renderer, textTexture, 1);
                        SDL_DestroyTexture(textTexture2);
                        _itoa_s(tries, text2, 10);
                        textTexture2 = get_text_texture(renderer, text2, my_font);
                        draw_text(renderer, textTexture2, 2);

                        SDL_RenderPresent(renderer);
                        SDL_Delay(10);

                        SDL_SetRenderDrawColor(renderer, 0, 21, 36, 0);
                        SDL_RenderClear(renderer);
                        SDL_RenderCopy(renderer, background, NULL, &back_rect);
                        check_colors(balls, balls[BALL_COUNT - 1], func, BALL_COUNT - 1, tmp);
                        if (func == 1) count_score--;


                        draw_balls(renderer, balls);
                        first_draw(renderer, first, ballTexture1);
                        SDL_DestroyTexture(textTexture);
                        //sound(hitsound);
                        _itoa_s(count_score, text, 10);
                        textTexture = get_text_texture(renderer, text, my_font);
                        draw_text(renderer, textTexture, 1);

                        SDL_DestroyTexture(textTexture2);
                        _itoa_s(tries, text2, 10);
                        textTexture2 = get_text_texture(renderer, text2, my_font);
                        draw_text(renderer, textTexture2, 2);

                        if (balls[BALL_COUNT - 1].ball.y >= AREA_HEIGHT + 50 - 120) { level_quit = true; flag_end = 1; }

                    }

                    if (wall > 1) //////////////////����� ������
                    {
                        tries--;
                        SDL_DestroyTexture(textTexture2);
                        _itoa_s(tries, text2, 10);
                        textTexture2 = get_text_texture(renderer, text2, my_font);
                        draw_text(renderer, textTexture2, 2);
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



                    if (flag_end == 1)//����� ��� �����������
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
                                draw_balls(renderer, balls);
                                first_draw(renderer, first, ballTexture1);
                                SDL_DestroyTexture(textTexture);
                                //sound(hitsound);
                                _itoa_s(count_score, text, 10);
                                textTexture = get_text_texture(renderer, text, my_font);
                                draw_text(renderer, textTexture, 1);

                                SDL_DestroyTexture(textTexture2);
                                _itoa_s(tries, text2, 10);
                                textTexture2 = get_text_texture(renderer, text2, my_font);
                                draw_text(renderer, textTexture2, 2);


                                //SDL_RenderPresent(renderer);
                                //continue;
                            }
                        }

                        int even_or_odd = -1;
                        for (int i = 0;i < BALL_COUNT;i++)
                        {
                            if (balls[i].line == 0) even_or_odd = balls[i].line_odd_or_even;
                            balls[i].line = balls[i].line + 1;
                            balls[i].ball.y += 60;
                        }
                        setlocale(LC_ALL, "rus");

                        //new_line(renderer, balls, even_or_odd);






                        int x = 0, y = 50, w = 60;
                        if (even_or_odd == 0)
                        {
                            x = 80 - w; BALL_COUNT += 15;
                            int k = BALL_COUNT - 15;
                            balls = (Rect*)realloc(balls, sizeof(Rect) * BALL_COUNT);
                            for (int i = k;i < BALL_COUNT;i++)
                            {
                                srand(time(NULL) - i * i * 10000);
                                balls[i].color = rand() % 4 + 1;/////////
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
                                balls[i].color = rand() % 4 + 1;/////////
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
                        draw_balls(renderer, balls);
                        first_draw(renderer, first, ballTexture1);
                    }

                } while (flag_end != 1);

            }

            SDL_SetRenderDrawColor(renderer, 0, 21, 36, 0);
            SDL_RenderClear(renderer);
            SDL_RenderCopy(renderer, background, NULL, &back_rect);
            first_draw(renderer, first, ballTexture1);
            draw_balls(renderer, balls);
            SDL_DestroyTexture(textTexture);
            _itoa_s(count_score, text, 10);
            textTexture = get_text_texture(renderer, text, my_font);
            draw_text(renderer, textTexture, 1);
            SDL_DestroyTexture(textTexture2);
            _itoa_s(tries, text2, 10);
            textTexture2 = get_text_texture(renderer, text2, my_font);
            draw_text(renderer, textTexture2, 2);
            SDL_RenderPresent(renderer);

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
    bool quit = false;
    while (!quit)
    {
        while (SDL_PollEvent(&event))
        {
            if (event.type == SDL_QUIT)
                return 1;

            if (draw_result(renderer, quit, event, 2) == 0)
            {
                quit = true;
                return 0;////////����� � ����
            }
            /* //�������� ������ � �����������
                 SDL_Rect result_rect = { 0,0, SCREEN_WIDTH, SCREEN_HEIGHT };
                 SDL_Surface* end = IMG_Load("End.bmp");
                 //SDL_SetColorKey(end, SDL_TRUE, SDL_MapRGB(end->format, 255, 255, 255));
                 SDL_Texture* endTexture = SDL_CreateTextureFromSurface(renderer, end);
                 SDL_FreeSurface(end);
                 SDL_RenderCopy(renderer, endTexture, NULL, &result_rect);
                 int rasn = first_tries - tries;
                 _itoa_s(count_score, text, 10);
                 _itoa_s(rasn, text2, 10);

                 SDL_DestroyTexture(textTexture);
                 SDL_DestroyTexture(textTexture2);
                 textTexture = get_text_texture_result(renderer, text, my_font);
                 textTexture2 = get_text_texture_result(renderer, text2, my_font);
                 //draw_message(renderer, textTexture, mush_counter);
                 draw_text(renderer, textTexture, 3);
                 draw_text(renderer, textTexture2, 4);

                 SDL_Rect menu_res = { SCREEN_WIDTH / 8, SCREEN_HEIGHT - 1.5 * 235 / 2, 235, 235 / 2 };
                 SDL_Surface* menu_res_image = IMG_Load("menu_res.bmp");
                 SDL_Texture* menu_res_texture = SDL_CreateTextureFromSurface(renderer, menu_res_image);
                 SDL_FreeSurface(menu_res_image);
                 SDL_RenderCopy(renderer, menu_res_texture, NULL, &menu_res);

                 SDL_Rect lvl_res = { SCREEN_WIDTH / 8 + menu_res.w * 2.4, SCREEN_HEIGHT - 1.5 * 235 / 2, 235, 235 / 2 };
                 SDL_Surface* lvl_res_image = IMG_Load("lvl_res.bmp");
                 SDL_Texture* lvl_res_texture = SDL_CreateTextureFromSurface(renderer, lvl_res_image);
                 SDL_FreeSurface(lvl_res_image);
                 SDL_RenderCopy(renderer, lvl_res_texture, NULL, &lvl_res);

             SDL_RenderPresent(renderer);

             if (event.type == SDL_MOUSEBUTTONDOWN && event.button.button == SDL_BUTTON_LEFT)
             {

                 if (is_button_hit(menu_res, event.button.x, event.button.y))
                 {
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
                     quit = true;
                     return 1;////////����� � ����
                 }
                 if (is_button_hit(lvl_res, event.button.x, event.button.y)) return -1;//middle(renderer);////////������� �� ��������� �������
             }*/
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

int draw_result(SDL_Renderer*& renderer,bool quit, SDL_Event event,int level)
{
    //�������� ������ � �����������
    char text2[10], text[10];
    SDL_Rect result_rect = { 0,0, SCREEN_WIDTH, SCREEN_HEIGHT };
    SDL_Surface* end = IMG_Load("End.bmp");
    //SDL_SetColorKey(end, SDL_TRUE, SDL_MapRGB(end->format, 255, 255, 255));
    SDL_Texture* endTexture = SDL_CreateTextureFromSurface(renderer, end);
    SDL_FreeSurface(end);
    SDL_RenderCopy(renderer, endTexture, NULL, &result_rect);
    int rasn = first_tries - tries;
    _itoa_s(count_score, text, 10);
    _itoa_s(rasn, text2, 10);

    TTF_Font* my_font = TTF_OpenFont("gogoia-deco.ttf", 100);
    SDL_Texture* textTexture = get_text_texture_result(renderer, text, my_font);
    SDL_Texture* textTexture2 = get_text_texture_result(renderer, text2, my_font);
    //draw_message(renderer, textTexture, mush_counter);
    draw_text(renderer, textTexture, 3);
    draw_text(renderer, textTexture2, 4);

    SDL_Rect menu_res = { SCREEN_WIDTH / 8, SCREEN_HEIGHT - 1.5 * 235 / 2, 235, 235 / 2 };
    SDL_Surface* menu_res_image = IMG_Load("menu_res.bmp");
    SDL_Texture* menu_res_texture = SDL_CreateTextureFromSurface(renderer, menu_res_image);
    SDL_FreeSurface(menu_res_image);
    SDL_RenderCopy(renderer, menu_res_texture, NULL, &menu_res);

    SDL_Rect lvl_res = { SCREEN_WIDTH / 8 + menu_res.w * 2.4, SCREEN_HEIGHT - 1.5 * 235 / 2, 235, 235 / 2 };
    SDL_Surface* lvl_res_image = IMG_Load("lvl_res.bmp");
    SDL_Texture* lvl_res_texture = SDL_CreateTextureFromSurface(renderer, lvl_res_image);
    SDL_FreeSurface(lvl_res_image);
    SDL_RenderCopy(renderer, lvl_res_texture, NULL, &lvl_res);

    SDL_RenderPresent(renderer);

    if (event.type == SDL_MOUSEBUTTONDOWN && event.button.button == SDL_BUTTON_LEFT)
    {

        if (is_button_hit(menu_res, event.button.x, event.button.y))
        {
            SDL_DestroyTexture(textTexture);
            SDL_DestroyTexture(textTexture2);
            TTF_CloseFont(my_font);
            quit = true;
            return 0;////////����� � ����
        }
        if (is_button_hit(lvl_res, event.button.x, event.button.y))
        {
            if (level==1) middle(renderer);
            if (level==2) return 0;//hard();
        }
         //������� �� ��������� �������
    }
}

int light(SDL_Renderer*& renderer)
{
    int x, y,x1,y1,r1=1,r2=1,co,location,flag1,flag_end=0,wall=0,func=0;
    BALL_COUNT = 47;
    tries = 20;first_tries = 20;
    count_score = 0;
    checking tmp{};
    SDL_Rect area = { (SCREEN_WIDTH-AREA_WIDTH)/2, (SCREEN_HEIGHT-AREA_HEIGHT)/2, AREA_WIDTH,AREA_HEIGHT };//���� ��� ����
    SDL_Surface* background_image = IMG_Load("mm.bmp");
    SDL_Texture* background = SDL_CreateTextureFromSurface(renderer, background_image);
    SDL_FreeSurface(background_image);
    SDL_Rect back_rect = {area.x,area.y,area.w,area.h};
    
    Rect* balls = (Rect*)malloc(BALL_COUNT * sizeof(Rect));//��� ����
    init_balls(balls);
    for (int i = 0;i < BALL_COUNT;i++)//������ �������� ��� �������
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

    
    SDL_Rect first;//����� ������� ������
    first = { area.w/2-60+50,area.h - 60+50,60,60 };
    int first_color = 1;
    SDL_Surface* ballImage = IMG_Load("1.bmp");
    SDL_SetColorKey(ballImage, SDL_TRUE, SDL_MapRGB(ballImage->format, 255, 255, 255));
    SDL_Texture* ballTexture1 = SDL_CreateTextureFromSurface(renderer, ballImage);
    SDL_FreeSurface(ballImage);
    
    int radius = first.w / 2;
    
    /*SDL_Rect ray;//����������� ���
    ray = { area.w / 2 - radius,area.h - 5 * radius, radius,radius * 4 };
    SDL_Surface* rayImage = IMG_Load("ray.bmp");
    SDL_SetColorKey(rayImage, SDL_TRUE, SDL_MapRGB(rayImage->format, 255, 255, 255));
    SDL_Texture* rayTexture = SDL_CreateTextureFromSurface(renderer, rayImage);
    SDL_FreeSurface(rayImage);
    
    
    */
    TTF_Init();
    TTF_Font* my_font = TTF_OpenFont("gogoia-deco.ttf", 100);
    SDL_Texture* textTexture;

    int k = 0;// ����
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
            if (event.type == SDL_QUIT) return 1; //������� ����
            flag1 = 1;

            //������ �� ����, ����� �������
            if (event.type == SDL_MOUSEBUTTONDOWN &&
                event.button.button == SDL_BUTTON_LEFT)
            {
                wall = 0;
                SDL_GetMouseState(&x, &y);
                //dir = { x,y };
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
                //int len_x = sqrt((x1 - x) * (x1 - x) + (y1 - y) * (y1 - y));

                do { //����� � �������� ����������� 
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
                    draw_text(renderer, textTexture, 1);
                    textTexture2 = get_text_texture(renderer, text2, my_font);
                    draw_text(renderer, textTexture2, 2);
                    SDL_RenderPresent(renderer);
                    if (check_down(balls, first) == -10 && flag_draw_delay == 0) SDL_Delay(10);

                    if (x1 >= AREA_WIDTH - first.w + 50 || x1 <= 50)//������ �� �����
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
                        wall++;
                    }

                    if (check_down(balls, first) != -10 || y1 < first.w / 2) //������� �� ������� ��� ����� ����
                    {
                        tries--;
                        SDL_DestroyTexture(textTexture2);
                        _itoa_s(tries, text2, 10);
                        textTexture2 = get_text_texture(renderer, text2, my_font);
                        draw_text(renderer, textTexture2, 2);

                        int nom = check_down(balls, first);
                        bool f = 0, f2 = 0;



                        if (check_down(balls, first) != -10)
                        {

                            if (x1 < balls[nom].ball.x + balls[nom].ball.w / 2 && y1 < balls[nom].ball.y + balls[nom].ball.w / 3 && balls[nom].ball.x - balls[nom].ball.w / 2 >= 50) { y1 = balls[nom].ball.y; x1 = balls[nom].ball.x - balls[nom].ball.w; f = 1; }
                            if (x1 >= balls[nom].ball.x + balls[nom].ball.w / 2 && y1 < balls[nom].ball.y + balls[nom].ball.w / 3 && balls[nom].ball.x + balls[nom].ball.w / 2 <= AREA_WIDTH + 50 - balls[nom].ball.w) { y1 = balls[nom].ball.y; x1 = balls[nom].ball.x + balls[nom].ball.w;f = 1; }


                            int tmp_x = x1, tmp_y = y1;
                            if (f != 1) y1 = balls[nom].ball.y + first.w;

                            //�����
                            if (x1 < balls[nom].ball.x + balls[nom].ball.w / 2 && balls[nom].ball.x - balls[nom].ball.w / 2 >= 50 && f != 1) x1 = balls[nom].ball.x - balls[nom].ball.w / 2;
                            if (x1 < balls[nom].ball.x + balls[nom].ball.w / 2 && balls[nom].ball.x - balls[nom].ball.w / 2 < 50 && f != 1) x1 = balls[nom].ball.x + balls[nom].ball.w / 2;

                            first.x = x1;first.y = y1;
                            if (check_near(balls, first) != -10 && f != 1)
                            {
                                if (balls[nom].ball.x + balls[nom].ball.w / 2 <= AREA_WIDTH + 50 - balls[nom].ball.w) {
                                    x1 = (balls[nom].ball.x + balls[nom].ball.w / 2); f2 = 1; first.x = x1;first.y = y1;
                                }
                            }

                            //������
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


                        func = 0;

                        draw_balls(renderer, balls);
                        SDL_DestroyTexture(textTexture);
                        _itoa_s(count_score, text, 10);
                        textTexture = get_text_texture(renderer, text, my_font);
                        draw_text(renderer, textTexture, 1);
                        SDL_DestroyTexture(textTexture2);
                        _itoa_s(tries, text2, 10);
                        textTexture2 = get_text_texture(renderer, text2, my_font);
                        draw_text(renderer, textTexture2, 2);

                        SDL_RenderPresent(renderer);
                        SDL_Delay(10);

                        SDL_SetRenderDrawColor(renderer, 0, 21, 36, 0);
                        SDL_RenderClear(renderer);
                        SDL_RenderCopy(renderer, background, NULL, &back_rect);
                        check_colors(balls, balls[BALL_COUNT - 1], func, BALL_COUNT - 1, tmp);
                        if (func == 1) count_score--;


                        draw_balls(renderer, balls);
                        first_draw(renderer, first, ballTexture1);
                        SDL_DestroyTexture(textTexture);
                        //sound(hitsound);
                        _itoa_s(count_score, text, 10);
                        textTexture = get_text_texture(renderer, text, my_font);
                        draw_text(renderer, textTexture, 1);

                        SDL_DestroyTexture(textTexture2);
                        _itoa_s(tries, text2, 10);
                        textTexture2 = get_text_texture(renderer, text2, my_font);
                        draw_text(renderer, textTexture2, 2);

                        if (balls[BALL_COUNT - 1].ball.y >= AREA_HEIGHT + 50 - 120) { level_quit = true; flag_end = 1; }

                    }

                    if (wall > 1) //////////////////����� ������
                    {
                        tries--;
                        SDL_DestroyTexture(textTexture2);
                        _itoa_s(tries, text2, 10);
                        textTexture2 = get_text_texture(renderer, text2, my_font);
                        draw_text(renderer, textTexture2, 2);
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
                                draw_balls(renderer, balls);
                                first_draw(renderer, first, ballTexture1);
                                SDL_DestroyTexture(textTexture);
                                //sound(hitsound);
                                _itoa_s(count_score, text, 10);
                                textTexture = get_text_texture(renderer, text, my_font);
                                draw_text(renderer, textTexture, 1);

                                SDL_DestroyTexture(textTexture2);
                                _itoa_s(tries, text2, 10);
                                textTexture2 = get_text_texture(renderer, text2, my_font);
                                draw_text(renderer, textTexture2, 2);
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
            draw_balls(renderer, balls);
            SDL_DestroyTexture(textTexture);
            _itoa_s(count_score, text, 10);
            textTexture = get_text_texture(renderer, text, my_font);
            draw_text(renderer, textTexture, 1);
            SDL_DestroyTexture(textTexture2);
            _itoa_s(tries, text2, 10);
            textTexture2 = get_text_texture(renderer, text2, my_font);
            draw_text(renderer, textTexture2, 2);
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
    bool quit = false;
    SDL_Event event1;
    while (!quit)
    {
        while (SDL_PollEvent(&event1))
        {
            if (event1.type == SDL_QUIT)
                return 1;

            if (draw_result(renderer,quit,event1,1)==0) 
            {
                quit = true;
                return 0;////////����� � ����
            }
           /* //�������� ������ � �����������
                SDL_Rect result_rect = { 0,0, SCREEN_WIDTH, SCREEN_HEIGHT };
                SDL_Surface* end = IMG_Load("End.bmp");
                //SDL_SetColorKey(end, SDL_TRUE, SDL_MapRGB(end->format, 255, 255, 255));
                SDL_Texture* endTexture = SDL_CreateTextureFromSurface(renderer, end);
                SDL_FreeSurface(end);
                SDL_RenderCopy(renderer, endTexture, NULL, &result_rect);
                int rasn = first_tries - tries;
                _itoa_s(count_score, text, 10);
                _itoa_s(rasn, text2, 10);

                SDL_DestroyTexture(textTexture);
                SDL_DestroyTexture(textTexture2);
                textTexture = get_text_texture_result(renderer, text, my_font);
                textTexture2 = get_text_texture_result(renderer, text2, my_font);
                //draw_message(renderer, textTexture, mush_counter);
                draw_text(renderer, textTexture, 3);
                draw_text(renderer, textTexture2, 4);

                SDL_Rect menu_res = { SCREEN_WIDTH / 8, SCREEN_HEIGHT - 1.5 * 235 / 2, 235, 235 / 2 };
                SDL_Surface* menu_res_image = IMG_Load("menu_res.bmp");
                SDL_Texture* menu_res_texture = SDL_CreateTextureFromSurface(renderer, menu_res_image);
                SDL_FreeSurface(menu_res_image);
                SDL_RenderCopy(renderer, menu_res_texture, NULL, &menu_res);

                SDL_Rect lvl_res = { SCREEN_WIDTH / 8 + menu_res.w * 2.4, SCREEN_HEIGHT - 1.5 * 235 / 2, 235, 235 / 2 };
                SDL_Surface* lvl_res_image = IMG_Load("lvl_res.bmp");
                SDL_Texture* lvl_res_texture = SDL_CreateTextureFromSurface(renderer, lvl_res_image);
                SDL_FreeSurface(lvl_res_image);
                SDL_RenderCopy(renderer, lvl_res_texture, NULL, &lvl_res);
            
            SDL_RenderPresent(renderer);
            
            if (event.type == SDL_MOUSEBUTTONDOWN && event.button.button == SDL_BUTTON_LEFT)
            {

                if (is_button_hit(menu_res, event.button.x, event.button.y))
                {
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
                    quit = true;
                    return 1;////////����� � ����
                }
                if (is_button_hit(lvl_res, event.button.x, event.button.y)) return -1;//middle(renderer);////////������� �� ��������� �������
            }*/
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
    //return 1;//������� �������
}

int level_menu(SDL_Renderer*& renderer)
{
    SDL_Rect menu_rect = { 0,0, SCREEN_WIDTH, SCREEN_HEIGHT };
    SDL_Surface* menu = IMG_Load("menu.bmp");
    SDL_Texture* menuTexture = SDL_CreateTextureFromSurface(renderer, menu);
    SDL_FreeSurface(menu);
    SDL_RenderCopy(renderer, menuTexture, NULL, &menu_rect);//������ ��� ����


    SDL_Rect start_menu = { (SCREEN_WIDTH - SCREEN_WIDTH / 1.8) / 2,SCREEN_HEIGHT / 7.5 * 1.5,SCREEN_WIDTH / 1.8 , SCREEN_WIDTH / 1.8 / 5.5 };
    SDL_Surface* start_menu_image = IMG_Load("light_menu.bmp");
    SDL_Texture* start_menu_texture = SDL_CreateTextureFromSurface(renderer, start_menu_image);
    SDL_FreeSurface(start_menu_image);
    SDL_RenderCopy(renderer, start_menu_texture, NULL, &start_menu);//������ �������


    SDL_Rect set_menu = { (SCREEN_WIDTH - SCREEN_WIDTH / 1.8) / 2, SCREEN_HEIGHT / 7.5 * 3,SCREEN_WIDTH / 1.8 , SCREEN_WIDTH / 1.8 / 5.5 };
    SDL_Surface* set_menu_image = IMG_Load("middle_menu.bmp");
    SDL_Texture* set_menu_texture = SDL_CreateTextureFromSurface(renderer, set_menu_image);
    SDL_FreeSurface(set_menu_image);
    SDL_RenderCopy(renderer, set_menu_texture, NULL, &set_menu);//������� �������

    SDL_Rect record_menu = { (SCREEN_WIDTH - SCREEN_WIDTH / 1.8) / 2, SCREEN_HEIGHT / 7.5 * 4.5,SCREEN_WIDTH / 1.8 , SCREEN_WIDTH / 1.8 / 5.5 };
    SDL_Surface* record_menu_image = IMG_Load("special_menu.bmp");
    SDL_Texture* record_menu_texture = SDL_CreateTextureFromSurface(renderer, record_menu_image);
    SDL_FreeSurface(record_menu_image);
    SDL_RenderCopy(renderer, record_menu_texture, NULL, &record_menu);//����������� �������

    SDL_Rect exit_menu = { (SCREEN_WIDTH - SCREEN_WIDTH / 1.8) / 2, SCREEN_HEIGHT / 7.5 * 6,SCREEN_WIDTH / 1.8 , SCREEN_WIDTH / 1.8 / 5.5 };
    SDL_Surface* exit_menu_image = IMG_Load("exit_menu.bmp");
    SDL_Texture* exit_menu_texture = SDL_CreateTextureFromSurface(renderer, exit_menu_image);
    SDL_FreeSurface(exit_menu_image);
    SDL_RenderCopy(renderer, exit_menu_texture, NULL, &exit_menu);//�����


    SDL_RenderPresent(renderer);
    SDL_Event event;
    bool quit = false;
    while (!quit)
    {
        while (SDL_PollEvent(&event))
        {
            if (event.type == SDL_QUIT) return -1;
            if (event.type == SDL_MOUSEBUTTONDOWN && event.button.button == SDL_BUTTON_LEFT)
            {

                if (is_button_hit(start_menu, event.button.x, event.button.y))
                {
                    return 1;////////����� � ����� �������
                }
                if (is_button_hit(set_menu, event.button.x, event.button.y))
                {
                    return 2;//////���������
                }
                if (is_button_hit(record_menu, event.button.x, event.button.y))
                {
                    return 3;//////�������
                }
                if (is_button_hit(exit_menu, event.button.x, event.button.y))
                {
                    return 4;//////�����
                }
            }
        }
    }
    if (quit == true) return -1;
}

int menu(SDL_Renderer*& renderer, SDL_Event event)
{
    SDL_Rect menu_rect = { 0,0, SCREEN_WIDTH, SCREEN_HEIGHT };
    SDL_Surface* menu = IMG_Load("menu.bmp");
    SDL_Texture* menuTexture = SDL_CreateTextureFromSurface(renderer, menu);
    SDL_FreeSurface(menu);
    SDL_RenderCopy(renderer, menuTexture, NULL, &menu_rect);//������ ��� ����
    
    
    SDL_Rect start_menu = { (SCREEN_WIDTH - SCREEN_WIDTH / 1.8) / 2,SCREEN_HEIGHT / 7.5 * 1.5,SCREEN_WIDTH / 1.8 , SCREEN_WIDTH / 1.8 / 5.5 };
    SDL_Surface* start_menu_image = IMG_Load("start_menu.bmp");
    SDL_Texture* start_menu_texture = SDL_CreateTextureFromSurface(renderer, start_menu_image);
    SDL_FreeSurface(start_menu_image);
    SDL_RenderCopy(renderer, start_menu_texture, NULL, &start_menu);//������ ����

    
    SDL_Rect set_menu = { (SCREEN_WIDTH - SCREEN_WIDTH / 1.8) / 2, SCREEN_HEIGHT / 7.5 * 3,SCREEN_WIDTH / 1.8 , SCREEN_WIDTH / 1.8 / 5.5 };
    SDL_Surface* set_menu_image = IMG_Load("settings_menu.bmp");
    SDL_Texture* set_menu_texture = SDL_CreateTextureFromSurface(renderer, set_menu_image);
    SDL_FreeSurface(set_menu_image);
    SDL_RenderCopy(renderer, set_menu_texture, NULL, &set_menu);//���������

    SDL_Rect record_menu = { (SCREEN_WIDTH - SCREEN_WIDTH / 1.8) / 2, SCREEN_HEIGHT / 7.5 * 4.5,SCREEN_WIDTH / 1.8 , SCREEN_WIDTH / 1.8 / 5.5 };
    SDL_Surface* record_menu_image = IMG_Load("record_menu.bmp");
    SDL_Texture* record_menu_texture = SDL_CreateTextureFromSurface(renderer, record_menu_image);
    SDL_FreeSurface(record_menu_image);
    SDL_RenderCopy(renderer, record_menu_texture, NULL, &record_menu);//�������

    SDL_Rect exit_menu = { (SCREEN_WIDTH - SCREEN_WIDTH / 1.8) / 2, SCREEN_HEIGHT / 7.5 * 6,SCREEN_WIDTH / 1.8 , SCREEN_WIDTH / 1.8 / 5.5 };
    SDL_Surface* exit_menu_image = IMG_Load("exit_menu.bmp");
    SDL_Texture* exit_menu_texture = SDL_CreateTextureFromSurface(renderer, exit_menu_image);
    SDL_FreeSurface(exit_menu_image);
    SDL_RenderCopy(renderer, exit_menu_texture, NULL, &exit_menu);//�����


    SDL_RenderPresent(renderer);

    if (event.type == SDL_MOUSEBUTTONDOWN && event.button.button == SDL_BUTTON_LEFT)
    {

        if (is_button_hit(start_menu, event.button.x, event.button.y))
        {
            return 1;////////����� � ����� �������
        }
        if (is_button_hit(set_menu, event.button.x, event.button.y))
        {
            return 2;//////���������
        }
        if (is_button_hit(record_menu, event.button.x, event.button.y))
        {
            return 3;//////�������
        }
        if (is_button_hit(exit_menu, event.button.x, event.button.y))
        {
            return 4;//////�����
        }
    }
}

int main(int argc, char** argv)
{
    int level = -1;
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
            if (event.type == SDL_QUIT) return 0;
            level = menu(renderer, event);
            if (level == 1)
            {
                int chose = level_menu(renderer);
                if (chose == 1)
                {
                    int res = light(renderer);
                    if (res == 1) return 0;
                    if (res == 0)  break;
                }
                if (chose == 2)
                {
                    int res2 = middle(renderer);
                    if (res2 == 1) return 0;
                    if (res2 == 0)  break;
                }
                if (chose == 3);
                if (chose == 4) break;
                if (chose == -1) return 0;
            }


            if (level == 2);
            if (level == 3);
            if (level == 4) return 0;
        }
    }
    /*if (level == 1)
    {
        int res = light(renderer);
        if (res == 1) return 0;
        if (res == 0)  break;
    }
    if (level == 2)
    {
        int res2 = middle(renderer);
        if (res2 == 1) return 0;
        if (res2 == 0)  break;
    }
    if (level == 3);
    if (level == 4) return 0;*/

    TTF_Quit();
    Mix_FreeChunk(Sound);
    Mix_CloseAudio();
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}