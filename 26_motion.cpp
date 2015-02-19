/*This source code copyrighted by Lazy Foo' Productions (2004-2014)
and may not be redistributed without written permission.*/

//Using SDL, SDL_image, standard IO, and strings
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <stdio.h>
#include <string>
#include <vector>

//Screen dimension constants
const int SCREEN_WIDTH = 640;
const int SCREEN_HEIGHT = 480;

struct Circle{
	int x, y;
	int r;
};

//Texture wrapper class
class LTexture
{
	public:
		//Initializes vaables
		LTexture();

		//Deallocates memory
		~LTexture();

		//Loads image at specified path
		bool loadFromFile( std::string path );

		#ifdef _SDL_TTF_H
		//Creates image from font string
		bool loadFromRenderedText( std::string textureText, SDL_Color textColor );
		#endif

		//Deallocates texture
		void free();

		//Set color modulation
		void setColor( Uint8 red, Uint8 green, Uint8 blue );

		//Set blending
		void setBlendMode( SDL_BlendMode blending );

		//Set alpha modulation how opaque
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

//The application time based timer
class LTimer
{
    public:
		//Initializes variables
		LTimer();

		//The various clock actions
		void start();
		void stop();
		void pause();
		void unpause();

		//Gets the timer's time
		Uint32 getTicks();

		//Checks the status of the timer
		bool isStarted();
		bool isPaused();

    private:
		//The clock time when the timer started
		Uint32 mStartTicks;

		//The ticks stored when the timer was paused
		Uint32 mPausedTicks;

		//The timer status
		bool mPaused;
		bool mStarted;
};



//The dot that will move around on the screen
class Dot
{
    public:
		//The dimensions of the dot
		static const int DOT_WIDTH = 20;
		static const int DOT_HEIGHT = 20;

		//Maximum axis velocity of the dot
		static const int DOT_VEL = 10;

		//Initializes the variables
		Dot(int x, int y);

		//Takes key presses and adjusts the dot's velocity

		//Player 1
		void handleEventP1( SDL_Event& e);

        //Player 2
        void handleEventP2( SDL_Event& e);

		//Moves the dot
		void move(Circle& other);

		//Shows the dot on the screen
		void render();

		//Moves the collision circle relative to the ball's offset
		void shiftColliders();

		//Gets collision circle
		Circle& getCollider();

    //private:
		//The X and Y offsets of the dot
		int mPosX, mPosY;

		//The velocity of the dot
		int mVelX, mVelY;

		//Ball's collision circle
		Circle mCollider;
};
//variables for shooting the bullet
bool createBul = false;
bool createBul2 = false;
class Bullet
{
	 public:
		//The dimensions of the dot
		static const int BULLET_WIDTH = 20;
		static const int BULLET_HEIGHT = 20;

		//Maximum axis velocity of the dot
		static const int BULLET_VEL = 10;


		Bullet(Dot d);

        void handleEvent(SDL_Event& e);

        void shoot(bool bul, Dot d,int vel);

        void setCreateBul(Dot d);

		void render();

		int bulPosX, bulPosY,bulVelX,bulVelY;


};

//The dot that will be moving around on the screen

//Starts up SDL and creates window
bool init();

//Loads media
bool loadMedia();

//Frees media and shuts down SDL
void close();

bool checkCollision(Circle&, Circle&);

//The window we'll be rendering to
SDL_Window* gWindow = NULL;

//The window renderer
SDL_Renderer* gRenderer = NULL;

//Scene textures
LTexture gDotTexture;
LTexture gBulletTexture;

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

#ifdef _SDL_TTF_H
bool LTexture::loadFromRenderedText( std::string textureText, SDL_Color textColor )
{
	//Get rid of preexisting texture
	free();

	//Render text surface
	SDL_Surface* textSurface = TTF_RenderText_Solid( gFont, textureText.c_str(), textColor );
	if( textSurface != NULL )
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
	else
	{
		printf( "Unable to render text surface! SDL_ttf Error: %s\n", TTF_GetError() );
	}


	//Return success
	return mTexture != NULL;
}
#endif

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


Dot::Dot(int x, int y)
{
    //Initialize the offsets
    mPosX = x;
    mPosY = y;
    //Initialize the velocity
    mVelX = 0;
    mVelY = 0;

    mCollider.r = DOT_WIDTH/ 2;

    shiftColliders();
}

Bullet::Bullet(Dot d)
{
    bulPosX = d.mPosX;
    bulPosY = d.mPosY;

    bulVelX = 0;
    bulVelY = 0;
}

void Dot::handleEventP1( SDL_Event& e )
{
    //If a key was pressed
	if( e.type == SDL_KEYDOWN && e.key.repeat == 0 )
    {
        //Adjust the velocity
        switch( e.key.keysym.sym )
        {
            case SDLK_UP: mVelY -= DOT_VEL; break;
            case SDLK_DOWN: mVelY += DOT_VEL; break;
            case SDLK_LEFT: mVelX -= DOT_VEL; break;
            case SDLK_RIGHT: mVelX += DOT_VEL; break;
        }
    }
    //If a key was released
    else if( e.type == SDL_KEYUP && e.key.repeat == 0 )
    {
        //Adjust the velocity
        switch( e.key.keysym.sym )
        {
            case SDLK_UP: mVelY += DOT_VEL; break;
            case SDLK_DOWN: mVelY -= DOT_VEL; break;
            case SDLK_LEFT: mVelX += DOT_VEL; break;
            case SDLK_RIGHT: mVelX -= DOT_VEL; break;
        }
    }
}

void Dot::handleEventP2( SDL_Event& e)
{
    //If a key was pressed
	if( e.type == SDL_KEYDOWN && e.key.repeat == 0 )
    {
        //Adjust the velocity
        switch( e.key.keysym.sym )
        {
            case SDLK_w: mVelY -= DOT_VEL; break;
            case SDLK_s: mVelY += DOT_VEL; break;
            case SDLK_a: mVelX -= DOT_VEL; break;
            case SDLK_d: mVelX += DOT_VEL; break;
        }
    }
    //If a key was released
    else if( e.type == SDL_KEYUP && e.key.repeat == 0 )
    {
        //Adjust the velocity
        switch( e.key.keysym.sym )
        {
            case SDLK_w: mVelY += DOT_VEL; break;
            case SDLK_s: mVelY -= DOT_VEL; break;
            case SDLK_a: mVelX += DOT_VEL; break;
            case SDLK_d: mVelX -= DOT_VEL; break;
        }
    }
}

void Bullet::handleEvent(SDL_Event& e){
    //shoots a bullet when q is pressed
    if((e.type == SDL_KEYDOWN && e.key.repeat == 0)&& e.key.keysym.sym == SDLK_RSHIFT )
    {
        createBul = true;
    }
    if((e.type == SDL_KEYDOWN && e.key.repeat == 0)&& e.key.keysym.sym == SDLK_q )
    {
        createBul2 = true;
    }
}

Circle& Dot::getCollider(){
	return mCollider;
}

bool checkCollision(Circle& c1, Circle& c2){
    if(sqrt(pow(c1.x-c2.x, 2)+pow(c1.y-c2.y, 2)) < c1.r+c2.r){
        return true;
    }
    return false;
}

void Dot::shiftColliders(){
	//Align collider to center of ball
	mCollider.x = mPosX;
	mCollider.y = mPosY;
}

void Dot::move(Circle& other)
{
    //Move the dot left or right
    mPosX += mVelX;
    shiftColliders();

    //If the dot went too far to the left or right
    if( ( mPosX < 0 ) || ( mPosX + DOT_WIDTH > SCREEN_WIDTH ) || checkCollision(mCollider, other))
    {
        //Move back
        mPosX -= mVelX;
        shiftColliders();
    }

    //Move the dot up or down
    mPosY += mVelY;
    shiftColliders();

    //If the dot went too far up or down
    if( ( mPosY < 0 ) || ( mPosY + DOT_HEIGHT > SCREEN_HEIGHT ) ||checkCollision(mCollider, other) )
    {
        //Move back
        mPosY -= mVelY;
        shiftColliders();
    }
}

void Bullet::shoot(bool shoot,Dot d,int vel){
    //follows the dot
    if(shoot == false){
        bulPosX = d.mPosX;
        bulPosY = d.mPosY;
    }
    //the actual shooting
    else if(shoot == true){
        bulPosX = d.mPosX;
        bulPosY = bulPosY + vel;
    }
}

//sets createBul to false amd moves the bullet back to the dot after bullet has exited the screen
void Bullet::setCreateBul(Dot d){
    if( ( bulPosY + BULLET_HEIGHT > SCREEN_HEIGHT ) )
    {
        //Move back to bullet position
        bulPosY = d.mPosY;
        bulPosX = d.mPosX;
        createBul = false;
    }
    if(( bulPosY < 0 )  )
    {
        bulPosY = d.mPosY;
        bulPosX = d.mPosX;
        createBul2 = false;
    }
}

void Dot::render()
{
    //Show the dot
	gDotTexture.render( mPosX, mPosY );
}

void Bullet::render()
{
    //Show the Bullet
	gBulletTexture.render( bulPosX, bulPosY );
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
			}
		}
	}
	return success;
}

bool loadMedia()
{
	//Loading success flag
	bool success = true;

	//Load dot texture
	if( !gDotTexture.loadFromFile( "Images/dot.bmp" ) )
	{
		printf( "Failed to load dot texture!\n" );
		success = false;
	}

    if( !gBulletTexture.loadFromFile( "Images/bullet.bmp" ) )
	{
		printf( "Failed to load dot texture!\n" );
		success = false;
	}

	return success;
}

void close()
{
	//Free loaded images
	gDotTexture.free();
	gBulletTexture.free();

	//Destroy window
	SDL_DestroyRenderer( gRenderer );
	SDL_DestroyWindow( gWindow );
	gWindow = NULL;
	gRenderer = NULL;

	//Quit SDL subsystems
	IMG_Quit();
	SDL_Quit();
}
//http://gamedev.stackexchange.com/questions/22616/sdl-bullet-movement
/***split the controls for bullet one and bullet two***/
int main( int argc, char* args[] )
{
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

			//dots and bullets on the screen
			Dot dot(SCREEN_WIDTH/2,10);
            Dot dot1(SCREEN_WIDTH/2,SCREEN_HEIGHT-dot1.DOT_HEIGHT-10);
            std::vector<Bullet> vecBul;
            std::vector<Bullet> vecBul2;
            //counter for bullet
            int i = 0;
            for(int i =0; i<10; i++){
                 Bullet bul(dot);
                 vecBul.push_back(bul);
                 vecBul2.push_back(bul);
            }


			//While application is running
			while( !quit )
			{

				//Handle events on queue,to check the keystate of the keys
				while( SDL_PollEvent( &e ) != 0 )
				{
					//User requests quit
					if( e.type == SDL_QUIT )
					{
						quit = true;
					}
     				//Handle input for the dot
					dot.handleEventP1(e);
					dot1.handleEventP2(e);
					for(int i = 0; i<10; i++){
                        vecBul[i].handleEvent(e);
                        vecBul2[i].handleEvent(e);
                    }
                    if(createBul == true)
                    {
                        i++;
                    }
                    if(i == 5){
                        i=0;
                    }
				}
				//Move the dot
				dot.move(dot1.getCollider());
				dot1.move(dot.getCollider());


                vecBul[i].shoot(createBul,dot,10);

                vecBul2[i].shoot(createBul2,dot1,-10);

                //sets createBul to false after bullet has exited the screen
                vecBul[i].setCreateBul(dot);
                vecBul2[i].setCreateBul(dot1);
                /*if( ( bulPosY < 0 ) || ( bulPosY + BULLET_HEIGHT > SCREEN_HEIGHT ) )
                {
                    //Move back to bullet position
                    bulPosY = d.mPosY;
                    bulPosX = d.mPosX;
                    createBul = false;
                }*/
				//Clear screen
				SDL_SetRenderDrawColor( gRenderer, 0xFF, 0xFF, 0xFF, 0xFF );
				SDL_RenderClear( gRenderer );
				//Render objects
				dot.render();
				dot1.render();
				//makes a colored dot
                vecBul[i].render();

                vecBul2[i].render();
				//Update screen
				SDL_RenderPresent( gRenderer );
			}
		}
	}
	//Free resources and close SDL
	close();

	return 0;
}
