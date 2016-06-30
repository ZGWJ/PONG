//
//  main.cpp
//  PONG
//
//  Created by Wedge on 16/6/28.
//  Copyright © 2016年 Wedge. All rights reserved.
//

#include <SDL2/SDL.h>
#include <SDL2_image/SDL_image.h>
#include <SDL2_ttf/SDL_ttf.h>
#include <SDL2_mixer/SDL_mixer.h>
#include <math.h>
#include <time.h>
#include <string>

//Screen dimension constants
const int SCREEN_WIDTH = 1440;
const int SCREEN_HEIGHT = 900;

int score1=0;
int score2=0;
//Texture wrapper class
class LTexture
{
public:
    //Initializes variables
    LTexture();
    
    //Deallocates memory
    ~LTexture();
    
    //Creates image from font string
    bool loadFromRenderedText( std::string textureText, SDL_Color textColor );
    
    //Loads image at specified path
    bool loadFromFile( std::string path );
    
    //Deallocates texture
    void free();
    
    //Set color modulation
    void setColor( Uint8 red, Uint8 green, Uint8 blue );
    
    //Set blending
    void setBlendMode( SDL_BlendMode blending );
    
    //Set alpha modulation
    void setAlpha( Uint8 alpha );
    
    //Renders texture at given point
    void render( int x, int y, SDL_Rect* clip = NULL, double angle = 0.0, SDL_Point* center = NULL, SDL_RendererFlip flip = SDL_FLIP_NONE );
    
    //Gets image dimensions
    int getWidth();
    int getHeight();
    
private:
    //The actual hardware texture
    SDL_Texture* mTexture;
    
    //Image dimensions
    int mWidth;
    int mHeight;
};

//The dot that will move around on the screen
class Player
{
public:
    //The dimensions of the dot
    static const int Player_WIDTH = 100;
    static const int Player_HEIGHT = 20;
    
    //Maximum axis velocity of the dot
    static const int Player_VEL = 15;
    
    //Initializes the variables
    Player();
    
    void init();
    
    //Takes key presses and adjusts the dot's velocity
    void handleEvent( SDL_Event& e );
    
    //Moves the dot and checks collision
    void move();
    
    //Shows the dot on the screen
    void render();
    
    //The X and Y offsets of the dot
    int mPosX, mPosY;
    
    //The velocity of the dot
    int mVelX, mVelY;
    
    //Dot's collision box
    SDL_Rect mCollider;
};

//The dot that will move around on the screen
class Computer
{
public:
    //The dimensions of the dot
    static const int Computer_WIDTH = 100;
    static const int Computer_HEIGHT = 20;
    
    //Maximum axis velocity of the dot
    static const int Computer_VEL = 15;
    
    //Initializes the variables
    Computer();
    
    void init();
    
    //Moves the dot and checks collision
    void move();
    
    //Shows the dot on the screen
    void render();
    
    //The X and Y offsets of the dot
    int mPosX, mPosY;
    
    //The velocity of the dot
    int mVelX, mVelY;
    
    //Dot's collision box
    SDL_Rect mCollider;
};

class Ball
{
public:
    //The dimensions of the dot
    static const int Ball_WIDTH = 20;
    static const int Ball_HEIGHT = 20;
    
    //Maximum axis velocity of the dot
    static const int Ball_VEL = 3;
    
    //Initializes the variables
    Ball();
    
    void init();
    
    //Moves the dot and checks collision
    void move();
    
    //Shows the dot on the screen
    void render();
    
    //The X and Y offsets of the dot
    int mPosX, mPosY;
    
    //The velocity of the dot
    int mVelX, mVelY;
    
    //Dot's collision box
    SDL_Rect mCollider;
};

//Starts up SDL and creates window
bool init();

//Loads media
bool loadMedia();

//Frees media and shuts down SDL
void close();

//Box collision detector
bool CheckBallCollisions(Ball ball,Player player,Computer computer);

bool CheckWin(Ball ball,Computer computer);
bool CheckLose(Ball ball,Player player);

void HandleAI(Ball ball,Computer* computer);
void HandleBall(Ball* ball,Player player,Computer computer);

void Restart();

//The window we'll be rendering to
SDL_Window* gWindow = NULL;

//The window renderer
SDL_Renderer* gRenderer = NULL;

Mix_Music *gMusic = NULL;

Mix_Chunk *gPong = NULL;

TTF_Font *gFont = NULL;

//Scene textures
LTexture gPlayerTexture;

LTexture gBallTexture;

LTexture gComputerTexture;

LTexture gTextTexture;

LTexture gComputerScore;

LTexture gPlayerScore;

LTexture::LTexture()
{
    //Initialize
    mTexture = NULL;
    mWidth = 0;
    mHeight = 0;
}

LTexture::~LTexture()
{
    //Deallocate
    free();
}

bool LTexture::loadFromFile( std::string path )
{
    //Get rid of preexisting texture
    free();
    
    //The final texture
    SDL_Texture* newTexture = NULL;
    
    //Load image at specified path
    SDL_Surface* loadedSurface = IMG_Load( path.c_str() );
    if( loadedSurface == NULL )
    {
        printf( "Unable to load image %s! SDL_image Error: %s\n", path.c_str(), IMG_GetError() );
    }
    else
    {
        //Color key image
        SDL_SetColorKey( loadedSurface, SDL_TRUE, SDL_MapRGB( loadedSurface->format, 0, 0xFF, 0xFF ) );
        
        //Create texture from surface pixels
        newTexture = SDL_CreateTextureFromSurface( gRenderer, loadedSurface );
        if( newTexture == NULL )
        {
            printf( "Unable to create texture from %s! SDL Error: %s\n", path.c_str(), SDL_GetError() );
        }
        else
        {
            //Get image dimensions
            mWidth = loadedSurface->w;
            mHeight = loadedSurface->h;
        }
        
        //Get rid of old loaded surface
        SDL_FreeSurface( loadedSurface );
    }
    
    //Return success
    mTexture = newTexture;
    return mTexture != NULL;
}

bool LTexture::loadFromRenderedText( std::string textureText, SDL_Color textColor )
{
    //Get rid of preexisting texture
    free();
    
    //Render text surface
    SDL_Surface* textSurface = TTF_RenderText_Solid( gFont, textureText.c_str(), textColor );
    if( textSurface == NULL )
    {
        printf( "Unable to render text surface! SDL_ttf Error: %s\n", TTF_GetError() );
    }
    else
    {
        //Create texture from surface pixels
        mTexture = SDL_CreateTextureFromSurface( gRenderer, textSurface );
        if( mTexture == NULL )
        {
            printf( "Unable to create texture from rendered text! SDL Error: %s\n", SDL_GetError() );
        }
        else
        {
            //Get image dimensions
            mWidth = textSurface->w;
            mHeight = textSurface->h;
        }
        
        //Get rid of old surface
        SDL_FreeSurface( textSurface );
    }
    
    //Return success
    return mTexture != NULL;
}

void LTexture::free()
{
    //Free texture if it exists
    if( mTexture != NULL )
    {
        SDL_DestroyTexture( mTexture );
        mTexture = NULL;
        mWidth = 0;
        mHeight = 0;
    }
}

void LTexture::setColor( Uint8 red, Uint8 green, Uint8 blue )
{
    //Modulate texture rgb
    SDL_SetTextureColorMod( mTexture, red, green, blue );
}

void LTexture::setBlendMode( SDL_BlendMode blending )
{
    //Set blending function
    SDL_SetTextureBlendMode( mTexture, blending );
}

void LTexture::setAlpha( Uint8 alpha )
{
    //Modulate texture alpha
    SDL_SetTextureAlphaMod( mTexture, alpha );
}

void LTexture::render( int x, int y, SDL_Rect* clip, double angle, SDL_Point* center, SDL_RendererFlip flip )
{
    //Set rendering space and render to screen
    SDL_Rect renderQuad = { x, y, mWidth, mHeight };
    
    //Set clip rendering dimensions
    if( clip != NULL )
    {
        renderQuad.w = clip->w;
        renderQuad.h = clip->h;
    }
    
    //Render to screen
    SDL_RenderCopyEx( gRenderer, mTexture, clip, &renderQuad, angle, center, flip );
}

int LTexture::getWidth()
{
    return mWidth;
}

int LTexture::getHeight()
{
    return mHeight;
}

Player::Player()
{
    //Initialize the offsets
    mPosX = 350;
    mPosY = 800;
    
    //Set collision box dimension
    mCollider.w = Player_WIDTH;
    mCollider.h = Player_HEIGHT;
    
    //Initialize the velocity
    mVelX = 0;
    mVelY = 0;
}

Computer::Computer()
{
    //Initialize the offsets
    mPosX = 350;
    mPosY = 100;
    
    //Set collision box dimension
    mCollider.w = Computer_WIDTH;
    mCollider.h = Computer_HEIGHT;
    
    //Initialize the velocity
    mVelX = 10;
    mVelY = 0;
}

Ball::Ball()
{
    //Initialize the offsets
    mPosX = 400;
    mPosY = 300;
    
    //Set collision box dimension
    mCollider.w = Ball_WIDTH;
    mCollider.h = Ball_HEIGHT;
    
    //Initialize the velocity
    mVelX = 0;
    mVelY = 8;
}

void Player::init(){
    //Initialize the offsets
    mPosX = 350;
    mPosY = 800;
    
    //Set collision box dimension
    mCollider.w = Player_WIDTH;
    mCollider.h = Player_HEIGHT;
    
    //Initialize the velocity
    mVelX = 0;
    mVelY = 0;
}

void Computer::init(){
    //Initialize the offsets
    mPosX = 350;
    mPosY = 100;
    
    //Set collision box dimension
    mCollider.w = Computer_WIDTH;
    mCollider.h = Computer_HEIGHT;
    
    //Initialize the velocity
    mVelX = 10;
    mVelY = 0;
}

void Ball::init(){
    //Initialize the offsets
    mPosX = 400;
    mPosY = 300;
    
    //Set collision box dimension
    mCollider.w = Ball_WIDTH;
    mCollider.h = Ball_HEIGHT;
    
    //Initialize the velocity
    mVelX = 0;
    mVelY = 8;
}


void Player::handleEvent( SDL_Event& e )
{
    //If a key was pressed
    if( e.type == SDL_KEYDOWN && e.key.repeat == 0 )
    {
        //Adjust the velocity
        switch( e.key.keysym.sym )
        {
            case SDLK_LEFT: mVelX -= Player_VEL; break;
            case SDLK_RIGHT: mVelX += Player_VEL; break;
            case SDLK_9:
                //If there is no music playing
                if( Mix_PlayingMusic() == 0 )
                {
                    //Play the music
                    Mix_PlayMusic( gMusic, -1 );
                }
                //If music is being played
                else
                {
                    //If the music is paused
                    if( Mix_PausedMusic() == 1 )
                    {
                        //Resume the music
                        Mix_ResumeMusic();
                    }
                    //If the music is playing
                    else
                    {
                        //Pause the music
                        Mix_PauseMusic();
                    }
                }
                break;
                
            case SDLK_0:
                //Stop the music
                Mix_HaltMusic();
                break;
        }
    }
    //If a key was released
    else if( e.type == SDL_KEYUP && e.key.repeat == 0 )
    {
        //Adjust the velocity
        switch( e.key.keysym.sym )
        {
            case SDLK_LEFT: mVelX += Player_VEL; break;
            case SDLK_RIGHT: mVelX -= Player_VEL; break;
        }
    }
}

void Player::move()
{
    //Move the dot left or right
    mPosX += mVelX;
    mCollider.x = mPosX;
    
    //If the dot collided or went too far to the left or right
    if( ( mPosX < 0 ) || ( mPosX + Player_WIDTH > SCREEN_WIDTH ))
    {
        //Move back
        mPosX -= mVelX;
        mCollider.x = mPosX;
    }
}

void Computer::move()
{
    //Move the dot left or right
    mPosX += mVelX;
    mCollider.x = mPosX;
    
    //If the dot collided or went too far to the left or right
    if( ( mPosX < 0 ) || ( mPosX + Computer_WIDTH > SCREEN_WIDTH ))
    {
        //Move back
        mPosX -= mVelX;
        mCollider.x = mPosX;
    }
}

void Ball::move()
{
    //Move the dot left or right
    mPosX += mVelX;
    mPosY += mVelY;
    mCollider.x = mPosX;
    
    //If the dot collided or went too far to the left or right
    if( ( mPosX < 0 ) || ( mPosX + Ball_WIDTH > SCREEN_WIDTH ))
    {
        //Move back
        mPosX -= mVelX;
        mCollider.x = mPosX;
        mVelX=-mVelX;
    }
}

void HandleAI(Ball ball,Computer* computer)
{
    // The first time this function gets called, we record the current speed
    // of the ball and decide on which part of the paddle to hit the ball with.
    static int last_speed = ball.mVelX;
    static int decision = rand() % 3 + 1;
    
    // Keep the code tidy
    int computer_x=0;
    int ball_center = ball.mPosX + ball.Ball_WIDTH / 2;
    
    // See if ball has changed direction
    if (last_speed != ball.mVelX)
    {
        // Make a new decision if the ball changed direction.
        // 1 == left side, 2 == right side, 3 = center
        decision = rand() % 3 + 1;
        
        last_speed = ball.mVelX;
    }
    
    // Determine part of paddle to hit ball with according to decision
    switch (decision)
    {
        case 1:
        {
            computer_x = computer->mPosX;
        } break;
            
        case 2:
        {
            computer_x = computer->mPosX +
            computer->Computer_WIDTH;
        } break;
            
        case 3:
        {
            computer_x = computer->mPosX +
            computer->Computer_WIDTH / 2;
        } break;
    }
    
    // See if ball is near computer's center. Prevents
    // computer from rapidly moving back and forth.
    if ( abs(computer_x - ball_center) < 10 )
    {
        return;
    }
    
    // Ball is to the left of the paddle
    if (computer_x > ball_center)
    {
        // Make sure the paddle hasn't hit a side of the screen
        if ( (computer->mPosX)>0 )
        {
            computer->mPosX -= computer->mVelX;
        }
    }
    // Ball is to the right of the paddle
    else if (computer_x < ball_center)
    {
        // Make sure the paddle hasn't hit a side of the screen
        if ( (computer->mPosX+computer->Computer_WIDTH)<SCREEN_WIDTH )
        {
            computer->mPosX += computer->mVelX;
        }
    }    
}

void HandleBall(Ball* ball,Player player,Computer computer)
{
    int player_center = player.mPosX + player.Player_WIDTH /2;
    int ball_center = ball->mPosX+ball->Ball_WIDTH /2;
    int computer_center=computer.mPosX+computer.Computer_WIDTH / 2;
    
    // Find the location on the paddle that the ball hit
    int player_location = ball_center - player_center;
    int computer_location = ball_center - computer_center;
    if ( CheckBallCollisions(*ball,player,computer) )
    {
        if ((ball->mPosY)<300) {
            ball->mVelX = computer_location / 6;
            ball->mVelY = -ball->mVelY;
        }else{
            ball->mVelX = player_location / 6;
            ball->mVelY = -ball->mVelY;
        }
        Mix_PlayChannel( -1, gPong, 0 );
    }
}

void Player::render()
{
    //Show the dot
    gPlayerTexture.render( mPosX, mPosY );
}

void Computer::render()
{
    //Show the dot
    gComputerTexture.render( mPosX, mPosY );
}

void Ball::render()
{
    //Show the dot
    gBallTexture.render( mPosX, mPosY );
}

bool init()
{
    //Initialization flag
    bool success = true;
    
    //Initialize SDL
    if( SDL_Init( SDL_INIT_VIDEO ) < 0 )
    {
        printf( "SDL could not initialize! SDL Error: %s\n", SDL_GetError() );
        success = false;
    }
    else
    {
        //Set texture filtering to linear
        if( !SDL_SetHint( SDL_HINT_RENDER_SCALE_QUALITY, "1" ) )
        {
            printf( "Warning: Linear texture filtering not enabled!" );
        }
        
        //Create window
        gWindow = SDL_CreateWindow( "SDL Tutorial", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN );
        if( gWindow == NULL )
        {
            printf( "Window could not be created! SDL Error: %s\n", SDL_GetError() );
            success = false;
        }
        else
        {
            //Create vsynced renderer for window
            gRenderer = SDL_CreateRenderer( gWindow, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC );
            if( gRenderer == NULL )
            {
                printf( "Renderer could not be created! SDL Error: %s\n", SDL_GetError() );
                success = false;
            }
            else
            {
                //Initialize renderer color
                SDL_SetRenderDrawColor( gRenderer, 0xFF, 0xFF, 0xFF, 0xFF );
                
                //Initialize PNG loading
                int imgFlags = IMG_INIT_PNG;
                if( !( IMG_Init( imgFlags ) & imgFlags ) )
                {
                    printf( "SDL_image could not initialize! SDL_image Error: %s\n", IMG_GetError() );
                    success = false;
                }
                
                //Initialize SDL_ttf
                if( TTF_Init() == -1 )
                {
                    printf( "SDL_ttf could not initialize! SDL_ttf Error: %s\n", TTF_GetError() );
                    success = false;
                }
                
                //Initialize SDL_mixer
                if( Mix_OpenAudio( 44100, MIX_DEFAULT_FORMAT, 2, 2048 ) < 0 )
                {
                    printf( "SDL_mixer could not initialize! SDL_mixer Error: %s\n", Mix_GetError() );
                    success = false;
                }
            }
        }
    }
    
    return success;
}

bool loadMedia()
{
    //Loading success flag
    bool success = true;
    
    //Load press texture
    if( !gPlayerTexture.loadFromFile( "/Users/MrGuo/Desktop/PONG/res/block.png" ) )
    {
        printf( "Failed to load dot texture!\n" );
        success = false;
    }
    if( !gComputerTexture.loadFromFile( "/Users/MrGuo/Desktop/PONG/res/block2.png" ) )
    {
        printf( "Failed to load dot texture!\n" );
        success = false;
    }
    if( !gBallTexture.loadFromFile( "/Users/MrGuo/Desktop/PONG/res/ball.png" ) )
    {
        printf( "Failed to load dot texture!\n" );
        success = false;
    }
    
    gMusic = Mix_LoadMUS( "/Users/MrGuo/Desktop/PONG/res/fever.wav" );
    if( gMusic == NULL )
    {
        printf( "Failed to load beat music! SDL_mixer Error: %s\n", Mix_GetError() );
        success = false;
    }
    
    //Load sound effects
    gPong = Mix_LoadWAV( "/Users/MrGuo/Desktop/PONG/res/gPong.wav" );
    if( gPong == NULL )
    {
        printf( "Failed to load scratch sound effect! SDL_mixer Error: %s\n", Mix_GetError() );
        success = false;
    }
    
    //Open the font
    gFont = TTF_OpenFont( "/Users/MrGuo/Desktop/PONG/res/lazy.ttf", 120 );
    if( gFont == NULL )
    {
        printf( "Failed to load lazy font! SDL_ttf Error: %s\n", TTF_GetError() );
        success = false;
    }
    else
    {
        //Render text
        SDL_Color textColor = { 0, 0, 0 };
        if( !gPlayerScore.loadFromRenderedText( "Player's Score:%d", textColor ) )
        {
            printf( "Failed to render text texture!\n" );
            success = false;
        }
        if( !gComputerScore.loadFromRenderedText( "Computer's Score:%d", textColor ) )
        {
            printf( "Failed to render text texture!\n" );
            success = false;
        }
    }
    
    return success;
}

void close()
{
    //Free loaded images
    gPlayerTexture.free();
    gComputerTexture.free();
    gBallTexture.free();
    
    //Free loaded images
    gTextTexture.free();
    
    //Free global font
    TTF_CloseFont( gFont );
    gFont = NULL;
    
    Mix_FreeChunk( gPong );
    gPong = NULL;
    
    Mix_FreeMusic( gMusic );
    gMusic = NULL;
    
    //Destroy window
    SDL_DestroyRenderer( gRenderer );
    SDL_DestroyWindow( gWindow );
    gWindow = NULL;
    gRenderer = NULL;
    
    //Quit SDL subsystems
    IMG_Quit();
    SDL_Quit();
    Mix_Quit();
    TTF_Quit();
}

// Check to see if the ball is going to hit a paddle
bool CheckBallCollisions(Ball ball,Player player,Computer computer)
{
    // Temporary values to keep things tidy
    int ball_x = ball.mPosX;
    int ball_y = ball.mPosY;
    int ball_width = ball.Ball_WIDTH;
    int ball_height = ball.Ball_HEIGHT;
    int ball_speed = ball.mVelY;
    
    // Check to see if ball is in Y range of the player's paddle.
    // We check its speed to see if it's even moving towards the player's paddle.
    if ( (ball_speed > 0) && (ball_y + ball_height >= player.mPosY) &&
        (ball_y + ball_height <= player.mPosY + player.Player_HEIGHT) ) // side hit
    {
        // If ball is in the X range of the paddle, return true.
        if ( (ball_x <= player.mPosX + player.Player_WIDTH) && (ball_x + ball_width>= player.mPosX) )
        {
            return true;
        }
    }
    // Check to see if ball is in Y range of the computer's paddle.
    // We check its speed to see if it's even moving towards the computer's paddle.
    if ( (ball_speed < 0) && (ball_y >= computer.mPosY) && (ball_y <= computer.mPosY +computer.Computer_HEIGHT) )
    {
        // If ball is in the X range of the paddle, return true.
        if ( (ball_x <= computer.mPosX + computer.Computer_WIDTH) && (ball_x + ball_width >=computer.mPosX) )
        {
            return true;
        }
    }
    
    return false;
}

bool CheckWin(Ball ball,Computer computer){
    if (ball.mPosY+ball.Ball_HEIGHT<computer.mPosY) {
        return true;
    }
    return false;
}

bool CheckLose(Ball ball,Player player){
    if (ball.mPosY>player.mPosY+player.Player_HEIGHT) {
        return true;
    }
    return false;
}

//void Restart(Ball ball,Computer computer,Player player){
//    SDL_SetRenderDrawColor( gRenderer, 0xFF, 0xFF, 0xFF, 0xFF );
//    SDL_RenderClear( gRenderer );
//    
//    player.Player();
//    Computer();
//    Ball();
//}

int main( int argc, char* args[] )
{
    int flag=0;
    //Start up SDL and create window
    if( !init() )
    {
        printf( "Failed to initialize!\n" );
    }
    else
    {
        //Load media
        if( !loadMedia() )
        {
            printf( "Failed to load media!\n" );
        }
        else
        {	
            //Main loop flag
            bool quit = false;
            
            //Event handler
            SDL_Event e;
            
            //The dot that will be moving around on the screen
            Player player;
            Computer computer;
            Ball ball;
            
            Mix_PlayMusic( gMusic, -1 );
            
            //While application is running
            while( !quit )
            {
                //Handle events on queue
                while( SDL_PollEvent( &e ) != 0 )
                {
                    //User requests quit
                    if( e.type == SDL_QUIT )
                    {
                        quit = true;
                    }
                    //Handle input for the dot
                    player.handleEvent( e );
                }
                if (flag==1) {
                    flag=0;
                    player.mVelX=0;//IN THIS PLACE I CANNOT FIND THE PROBLEM YET...I'll fix it later
                }
                //Move the dot and check collision
                HandleBall(&ball, player, computer);
                HandleAI(ball, &computer);
                player.move();
                ball.move();
                
                //Clear screen
                SDL_SetRenderDrawColor( gRenderer, 0xFF, 0xFF, 0xFF, 0xFF );
                SDL_RenderClear( gRenderer );
                
                //Render
                player.render();
                computer.render();
                ball.render();
                if (CheckWin(ball, computer)) {
                    SDL_Color textColor = { 0, 0, 0 };
                    gTextTexture.loadFromRenderedText( "OH SHIT,YOU WIN!", textColor );
                    gTextTexture.render( ( SCREEN_WIDTH - gTextTexture.getWidth() ) / 2, (SCREEN_HEIGHT - gTextTexture.getHeight() ) / 2 );
                    SDL_RenderPresent( gRenderer );
                    SDL_Delay( 2000 );
                    player.init();
                    ball.init();
                    computer.init();
                    flag=1;
                }
                if (CheckLose(ball, player)) {
                    SDL_Color textColor = { 0, 0, 0 };
                    gTextTexture.loadFromRenderedText( "TRASH,SUCK MY ASS", textColor );
                    gTextTexture.render( ( SCREEN_WIDTH - gTextTexture.getWidth() ) / 2, (SCREEN_HEIGHT - gTextTexture.getHeight() ) / 2 );
                    SDL_RenderPresent( gRenderer );
                    SDL_Delay( 2000 );
                    player.init();
                    ball.init();
                    computer.init();
                    flag=1;
                }
                
                //Update screen
                SDL_RenderPresent( gRenderer );
            }
        }
    }
    
    //Free resources and close SDL
    close();
    
    return 0;
}
