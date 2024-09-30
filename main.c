#include "raylib.h"
#include "raymath.h"
#include<stdio.h>
#include<stdlib.h>
#include<math.h>

const int WIDTH = 1600;
const int HEIGHT = 900;

float deltaTime;
int score = 0;
int scene = 0;

Sound collectSound;
Sound deathSound;

Music song;

//OBJECTS ESSENTIALLY
typedef struct Player
{
    Texture2D image;
    Rectangle sprite;
    int frame;
    float frameCounter;

    Vector2 position;
    Vector2 velocity;

    float topSpeed;
    float acceleration;

    Vector2 size;
    Rectangle collider;
}Player;

typedef struct BlackHole{
    Texture2D image;

    Vector2 position;
    Vector2 velocity;

    float topSpeed;
    float acceleration;

    Vector2 size;
    Rectangle collider;
}BlackHole;

typedef struct Berry
{
    Texture2D image;

    Vector2 position;
    Rectangle collider;
}Berry;


Player player;
BlackHole blackHole;
Berry berry;

Texture sky;

//OBJECT CONSTRUCTORS
Player NewPlayer(float posX, float posY, float topSpeed, float acceleration){

    Player player = {
        .position.x = posX,
        .position.y = posY,
        .velocity.x = 0,
        .velocity.y = 0,
        .topSpeed = topSpeed,
        .acceleration = acceleration,
        .image = LoadTexture("sprites/bear.png"),
        .size.x = (float)player.image.width/8.0f,
        .size.y = player.image.height,
        .sprite = {0.0f,0.0f, (float)player.image.width/8.0f, player.image.height},
        .collider = {0.0f,0.0f, (float)player.image.width/8.0f, player.image.height},
        .frame = 0,
        .frameCounter = 0,
    };
    return player;
}

BlackHole NewBlackHole(float posX, float posY, float topSpeed, float acceleration){
    BlackHole blackHole = {
        .position.x = posX,
        .position.y = posY,
        .velocity.x = 0,
        .velocity.y = 0,
        .topSpeed = topSpeed,
        .acceleration = acceleration,
        .image = LoadTexture("sprites/blackhole.png"),
        .size.x = blackHole.image.width,
        .size.y = blackHole.image.height,
        .collider = {0.0f,0.0f, blackHole.image.width * 0.7f, blackHole.image.height * 0.7f},
    };

    return blackHole;
}

Berry NewBerry(float posX, float posY){
    Berry berry = {
        .position.x = posX,
        .position.y = posY,

        .image = LoadTexture("sprites/berry.png"),
        .collider = {0.0f,0.0f, berry.image.width, berry.image.height},
    };
    return berry;
}

void UpdatePlayer(Player *player);
void DrawPlayer(Player *player);

void UpdateBlackHole(BlackHole *blackHole, Vector2 targetPos);
void DrawBlackHole(BlackHole *blackHole);

void UpdateBerry(Berry *berry);
void DrawBerry(Berry *berry);
void ChangeBerryPosition(Berry *berry);

void UpdateMenu();
void DrawMenu();

void UpdateEnd();
void DrawEnd();

void Update();
void UpdateDraw();

int main(){
    SetTargetFPS(60);

    InitWindow(WIDTH, HEIGHT, "C Game Demo");
    InitAudioDevice();

    song = LoadMusicStream("sound/Music.ogg");

    PlayMusicStream(song);

    song.looping = true;

    deathSound = LoadSound("sound/Death_Sound.wav");
    collectSound = LoadSound("sound/Collect_Sound.wav");

    score = 0;
    player =  NewPlayer(0,0,400,800);
    blackHole = NewBlackHole(1000,500, 500,200);
    berry = NewBerry(GetRandomValue(0 + berry.collider.width, GetScreenWidth() - berry.collider.width), GetRandomValue(0 + berry.collider.height, GetScreenHeight() - berry.collider.height));
    sky = LoadTexture("sprites/sky.png");

    while (!WindowShouldClose())
    {
        UpdateMusicStream(song);
        deltaTime = GetFrameTime();

        Update(deltaTime);

        BeginDrawing();
        ClearBackground(WHITE);
        UpdateDraw();
        EndDrawing();
        
    }
    
    UnloadTexture(player.image);
    UnloadTexture(blackHole.image);
    UnloadTexture(berry.image);
    UnloadTexture(sky);

    UnloadSound(collectSound);
    UnloadSound(deathSound);

    UnloadMusicStream(song);

    CloseAudioDevice();
    CloseWindow();
    return 0;
}

//UPDATES THE GAME
void Update(){
    //DECIDES WHICH SCENE IS LOADED
    switch (scene)
    {
    case 1:
        UpdatePlayer(&player);
        UpdateBlackHole(&blackHole, player.position);
        UpdateBerry(&berry);

        //COLLISION CHECKS
        if(CheckCollisionRecs(player.collider, berry.collider)){
            ChangeBerryPosition(&berry);
            PlaySound(collectSound);
            score ++;
        }

        if(CheckCollisionRecs(player.collider, blackHole.collider)){
            PlaySound(deathSound);
            scene = 2;
        }
        break;
    case 2:
        UpdateEnd();
        break;
    
    default:
    UpdateMenu();
        break;
    }
}

//DRAWS EVERY UPDATE
void UpdateDraw(){
    DrawTexture(sky,0,0,WHITE);

    switch (scene)
    {
    case 1:
        DrawPlayer(&player);
        DrawBlackHole(&blackHole);
        DrawBerry(&berry);
        DrawText(TextFormat("SCORE: %i", score), 10, 10, 40, RED);
        break;
    case 2:
        DrawEnd();
        break;
    
    default:
        DrawMenu();
        break;
    }

}

//PLAYER HANDLING 
void UpdatePlayer(Player *player){
    
    //HANDLES PLAYER MOVEMENT
    if(IsKeyDown(KEY_LEFT)){
       player->velocity.x -= player->acceleration * deltaTime; 
    }
    else if(IsKeyDown(KEY_RIGHT)){
       player->velocity.x += player->acceleration * deltaTime; 
    }else{
        if(player->velocity.x > 0){
            player->velocity.x -= fmin(fabs(player->velocity.x), player->acceleration) * 1 * deltaTime;
        }
        if(player->velocity.x < 0){
            player->velocity.x -= fmin(fabs(player->velocity.x), player->acceleration) * -1 * deltaTime;
        }
    }

    if(IsKeyDown(KEY_DOWN)){
       player->velocity.y += player->acceleration * deltaTime; 
    }
    else if(IsKeyDown(KEY_UP)){
       player->velocity.y -= player->acceleration * deltaTime; 
    }else{
        if(player->velocity.y > 0){
            player->velocity.y -= fmin(fabs(player->velocity.y), player->acceleration) * 1 * deltaTime;
        }
        if(player->velocity.y < 0){
            player->velocity.y -= fmin(fabs(player->velocity.y), player->acceleration) * -1 * deltaTime;
        }
    }

    //CAPS MOVEMENT SPEED
    if(player->velocity.x > player->topSpeed){
        player->velocity.x = player->topSpeed;
    }else if(player->velocity.x < -player->topSpeed){
        player->velocity.x = - player->topSpeed;
    }

    if(player->velocity.y > player->topSpeed){
        player->velocity.y = player->topSpeed;
    }else if(player->velocity.y < -player->topSpeed){
        player->velocity.y = - player->topSpeed;
    }

    player->collider.x = player->position.x;
    player->collider.y = player->position.y;

    //KEEP PLAYER ON SCREEN
    if(player->position.x + player->collider.width > GetScreenWidth() && player->velocity.x > 0){
        player->velocity.x = 0;
    }else if(player->position.x < 0 && player->velocity.x < 0){
        player->velocity.x = 0;
    }

    if(player->position.y + player->collider.height > GetScreenHeight() && player->velocity.y > 0){
        player->velocity.y = 0;
    }else if(player->position.y< 0 && player->velocity.y < 0){
        player->velocity.y = 0;
    }

    player->position.x += player->velocity.x * deltaTime;
    player->position.y += player->velocity.y * deltaTime;
}

void DrawPlayer(Player *player){
    player->frameCounter += deltaTime;

    //ANIMATION
    if(player->frameCounter > 0.08f){
        player->frame++;
        player->frameCounter = 0;
        player->sprite.x = player->frame * player->image.width/ 8;

        if(player->frame > 7){
            player->frame = 0;
        }
    }
    DrawTextureRec(player->image, player->sprite, player->position,WHITE);
}

void UpdateBlackHole(BlackHole *blackHole, Vector2 targetPos){
    //FOLLOW TARGET
    if(blackHole->position.x > targetPos.x){
        blackHole->velocity.x -= blackHole->acceleration * deltaTime; 
    }else if(blackHole->position.x < targetPos.x){
        blackHole->velocity.x += blackHole->acceleration * deltaTime; 
    }

    if(blackHole->position.y > targetPos.y){
        blackHole->velocity.y -= blackHole->acceleration * deltaTime; 
    }else if(blackHole->position.y < targetPos.y){
        blackHole->velocity.y += blackHole->acceleration * deltaTime; 
    }


    //CAP SPEED
    if(blackHole->velocity.x > blackHole->topSpeed){
        blackHole->velocity.x = blackHole->topSpeed;
    }else if(blackHole->velocity.x < -blackHole->topSpeed){
        blackHole->velocity.x = - blackHole->topSpeed;
    }

    if(blackHole->velocity.y > blackHole->topSpeed){
        blackHole->velocity.y = blackHole->topSpeed;
    }else if(blackHole->velocity.y < -blackHole->topSpeed){
        blackHole->velocity.y = - blackHole->topSpeed;
    }

    blackHole->collider.x = blackHole->position.x;
    blackHole->collider.y = blackHole->position.y;

    if(blackHole->position.x + blackHole->collider.width > GetScreenWidth() && blackHole->velocity.x > 0){
        blackHole->velocity.x = 0;
    }else if(blackHole->position.x < 0 && blackHole->velocity.x < 0){
        blackHole->velocity.x = 0;
    }

    if(blackHole->position.y + blackHole->collider.height > GetScreenHeight() && blackHole->velocity.y > 0){
        blackHole->velocity.y = 0;
    }else if(blackHole->position.y< 0 && blackHole->velocity.y < 0){
        blackHole->velocity.y = 0;
    }

    blackHole->position.x += blackHole->velocity.x * deltaTime;
    blackHole->position.y += blackHole->velocity.y * deltaTime;
}

void DrawBlackHole(BlackHole *blackHole){
    DrawTexture(blackHole->image, blackHole->position.x, blackHole->position.y, WHITE);
}

void UpdateBerry(Berry *berry){
    berry->collider.x = berry->position.x;
    berry->collider.y = berry->position.y;
}

void ChangeBerryPosition(Berry *berry){
    berry->position.x = GetRandomValue(0 + berry->collider.width, GetScreenWidth() - berry->collider.width);
    berry->position.y = GetRandomValue(0 + berry->collider.height, GetScreenHeight() - berry->collider.height);
}

void DrawBerry(Berry *berry){
    DrawTexture(berry->image, berry->position.x, berry->position.y, WHITE);
}

void UpdateMenu(){
    if(IsKeyPressed(KEY_ENTER)){
        scene = 1;
    }
}

void DrawMenu(){
    DrawText(TextFormat("C Game Demo"), 10, 10, 80, RED);
    DrawText(TextFormat("By Ajibade Awofeso"), 10, 100, 40, RED);
    DrawText(TextFormat("Press Enter to Start"), 10, 200, 40, RED);
}

void UpdateEnd(){
    if(IsKeyPressed(KEY_ENTER)){
        player =  NewPlayer(0,0,400,500);
        blackHole = NewBlackHole(1000,500, 600,300);
        berry = NewBerry(GetRandomValue(0 + berry.collider.width, GetScreenWidth() - berry.collider.width), GetRandomValue(0 + berry.collider.height, GetScreenHeight() - berry.collider.height));
        score = 0;
        scene = 1;
    }
}

void DrawEnd(){
    DrawText(TextFormat("You died but you got a score of %i", score), 10, 10, 40, RED);

    DrawText(TextFormat("Press Enter to play again"), 10, 800, 40, RED);
}
