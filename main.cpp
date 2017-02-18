#include<iostream>
#include<cstdlib>
#include<string>
#include<sstream>
#include "SDL/SDL.h"
#include "SDL_image.h"
using namespace std;

const int SCREEN_WIDTH = 640;
const int SCREEN_HEIGHT = 400;
const int SCREEN_BPP = 32;
const int TAILLEBALL=20;
const int RAQUETTE_WIDTH=11;
const int RAQUETTE_HEIGHT=43;

struct raquette
{
    int x;
    int y;
    int w;
    int h;
    int mvt_y;
    SDL_Surface *source1;
};

struct ball
{
    int x;     // abscisse du centre de la balle
    int y;     // ordonnée du centre de la balle
    int w;     // largeur de la balle
    int h;     // hauteur de la balle
    int mvt_x; // mouvement sur l'axe des abscisses
    int mvt_y; // mouvement sur l'axe des ordonnées
    SDL_Surface *source;
};

void initRaquette(raquette &r,int x, int y)
{
    r.x=x;
    r.y=y;
    r.w=RAQUETTE_WIDTH;
    r.h=RAQUETTE_HEIGHT;
    r.mvt_y=2;
}

void initBall(ball &b)
{
    b.x=SCREEN_WIDTH/2;
    b.y=SCREEN_HEIGHT/2;
    b.w=TAILLEBALL;
    b.h=TAILLEBALL;
    b.mvt_x=2;
    b.mvt_y=2;
}

bool collision(SDL_Rect a, SDL_Rect b)
{
    int leftA, leftB;
    int rightA, rightB;
    int topA, topB;
    int bottomA, bottomB;

    leftA = a.x;
    rightA = a.x + a.w;
    topA = a.y;
    bottomA = a.y + a.h;

    leftB = b.x;
    rightB = b.x + b.w;
    topB = b.y;
    bottomB = b.y + b.h;

    if(bottomA <= topB)
        return false;
    if(topA >= bottomB)
        return false;
    if(rightA <= leftB)
        return false;
    if(leftA >= rightB)
        return false;

    return true;
}

void moveBall(ball &b, raquette &r1, raquette &r2)
{
    SDL_Rect balle;
    SDL_Rect raq1;
    SDL_Rect raq2;

    b.x+=b.mvt_x;
    balle.x=b.x-TAILLEBALL/2;
    balle.y=b.y-TAILLEBALL/2;
    balle.h=TAILLEBALL;
    balle.w=TAILLEBALL;

    raq1.x=r1.x-RAQUETTE_WIDTH/2;
    raq1.y=r1.y-RAQUETTE_HEIGHT/2;
    raq1.w=RAQUETTE_WIDTH;
    raq1.h=RAQUETTE_HEIGHT;

    raq2.x=r2.x-RAQUETTE_WIDTH/2;
    raq2.y=r2.y-RAQUETTE_HEIGHT/2;
    raq2.w=RAQUETTE_WIDTH;
    raq2.h=RAQUETTE_HEIGHT;

    // Correction Mouvement Horizontal
    if(collision(balle,raq1) || collision(balle,raq2))
    {
        b.x-=b.mvt_x;
        b.mvt_x*=-1;
    }
    b.y+=b.mvt_y;

    balle.x=b.x-TAILLEBALL/2;
    balle.y=b.y-TAILLEBALL/2;

    // Correction Mouvement Vertical
    if((b.y+TAILLEBALL/2>SCREEN_HEIGHT) || (b.y-TAILLEBALL/2<0) || collision(balle,raq1) || collision(balle,raq2))
    {
        b.y-=b.mvt_y;
        b.mvt_y*=-1;
    }
}

void moveRaquette(raquette &r,SDLKey haut,SDLKey bas)
{
    Uint8 *keystates = SDL_GetKeyState( NULL );
    //If up is pressed
    if( keystates[ haut ] )
    {
        r.y-=r.mvt_y;
        if (r.y==1)
        {
            r.y+=r.mvt_y;
        }
    }
    //If down is pressed
    if( keystates[ bas ] )
    {
        r.y+=r.mvt_y;
        if (r.y==SCREEN_HEIGHT-RAQUETTE_HEIGHT)
        {
            r.y-=r.mvt_y;
        }
    }
}

// chargement d'une image
SDL_Surface *load_image( std::string filename )
{
    //Temporary storage for the image that's loaded
    SDL_Surface* loadedImage = NULL;

    //The optimized image that will be used
    SDL_Surface* optimizedImage = NULL;
    //Load the image
    loadedImage = SDL_LoadBMP( filename.c_str() );
    //If nothing went wrong in loading the image
    if( loadedImage != NULL )
    {
        //Create an optimized image
        optimizedImage = SDL_DisplayFormat( loadedImage );

        //Free the old image
        SDL_FreeSurface( loadedImage );
    }
    //Return the optimized image
    return optimizedImage;
}

// -- loadImageWithColorKey ---------------------
// chargement d'une image
// * paramètres entrées :
// - "filename" : nom de l'image
// - "r,g,b"    : couleur de la ColorKey; cette
//   couleur devient transparente !
// * paramètre de sortie : SDL_Surface contenant
//   l'image.
// ----------------------------------------------
SDL_Surface *loadImageWithColorKey(string filename, int r, int g, int b)
{
    //The image that's loaded
    SDL_Surface* loadedImage = NULL;

    //The optimized image that will be used
    SDL_Surface* optimizedImage = NULL;

    //Load the image
    loadedImage = IMG_Load( filename.c_str() );

    //If the image loaded
    if( loadedImage != NULL )
    {
        //Create an optimized image
        optimizedImage = SDL_DisplayFormat( loadedImage );

        //Free the old image
        SDL_FreeSurface( loadedImage );

        //If the image was optimized just fine
        if( optimizedImage != NULL )
        {
            //Map the color key
            Uint32 colorkey = SDL_MapRGB( optimizedImage->format, r, g, b );

            //Set all pixels of color R 0, G 0xFF, B 0xFF to be transparent
            SDL_SetColorKey( optimizedImage, SDL_SRCCOLORKEY, colorkey );
        }
    }
    //Return the optimized image
    return optimizedImage;
}

void applySurface(int x, int y, SDL_Surface* source, SDL_Surface* destination, SDL_Rect* clip)
{
    SDL_Rect offset;
    offset.x = x;
    offset.y = y;
    SDL_BlitSurface( source, clip, destination, &offset );
}

SDL_Surface *affScore(int scoreJoueur)
{
    SDL_Surface *scoreJ =NULL;
    switch (scoreJoueur)
    {
    case 0:
    {
        scoreJ=loadImageWithColorKey("images/0.bmp",255,255,255);
        break;
    }
    case 1:
    {
        scoreJ=loadImageWithColorKey("images/1.bmp",255,255,255);
        break;
    }
    case 2:
    {
        scoreJ=loadImageWithColorKey("images/2.bmp",255,255,255);
        break;
    }
    case 3:
    {
        scoreJ=loadImageWithColorKey("images/3.bmp",255,255,255);
        break;
    }
    case 4:
    {
        scoreJ=loadImageWithColorKey("images/4.bmp",255,255,255);
        break;
    }
    case 5:
    {
        scoreJ=loadImageWithColorKey("images/5.bmp",255,255,255);
        break;
    }
    }
    return scoreJ;
}

void play(SDL_Surface *screen)
{
    bool quit=false;
    ball b;
    raquette r1;
    raquette r2;
    initBall(b);
    initRaquette(r1,0,(SCREEN_HEIGHT/2)-(RAQUETTE_HEIGHT/2));
    initRaquette(r2,SCREEN_WIDTH-RAQUETTE_WIDTH,(SCREEN_HEIGHT/2)-(RAQUETTE_HEIGHT/2));

    SDL_Surface *fond;
    SDL_Event event;

    fond=load_image("fond.bmp");

    b.source=loadImageWithColorKey("images/ball.bmp",0,255,255);
    r1.source1=loadImageWithColorKey("images/player01.bmp",0,255,255);
    r2.source1=loadImageWithColorKey("images/player02.bmp",0,255,255);
    SDL_Surface *score;
    score =loadImageWithColorKey("images/score.bmp",255,255,255);

    SDL_Surface *joueur1;
    SDL_Surface *joueur2;
    int scoreJ1=0;
    int scoreJ2=0;
    joueur1=affScore(scoreJ1);
    joueur2=affScore(scoreJ2);

    while(quit==false)
    {
        SDL_FillRect(screen,&screen->clip_rect, SDL_MapRGB(screen->format,255,255,255));
        applySurface(0,0,fond,screen,NULL);
        applySurface(b.x,b.y,b.source,screen,NULL);
        applySurface(r1.x,r1.y,r1.source1,screen,NULL);
        applySurface(r2.x,r2.y,r2.source1,screen,NULL);

        applySurface(SCREEN_WIDTH/2-14,10,score,screen,NULL);
        joueur1=affScore(scoreJ1);
        joueur2=affScore(scoreJ2);
        applySurface(SCREEN_WIDTH/2-50,10,joueur1,screen,NULL);
        applySurface(SCREEN_WIDTH/2+50,10,joueur2,screen,NULL);

        SDL_Flip(screen);
        //lecture des évènements dans la queue d'évènements
        while(SDL_PollEvent(&event))
        {
            //événement de fermeture d'application, on sort la boucle
            if( event.type == SDL_QUIT )
            {
                quit = true;
            }
        }
        moveRaquette(r1,SDLK_w,SDLK_s);
        moveRaquette(r2,SDLK_UP,SDLK_DOWN);
        moveBall(b,r1,r2);

        if(b.x+TAILLEBALL/2>SCREEN_WIDTH)
        {
            scoreJ1=scoreJ1+1;
            initBall(b);
            initRaquette(r1,0,(SCREEN_HEIGHT/2)-(RAQUETTE_HEIGHT/2));
            initRaquette(r2,SCREEN_WIDTH-RAQUETTE_WIDTH,(SCREEN_HEIGHT/2)-(RAQUETTE_HEIGHT/2));

        }
        if(b.x-TAILLEBALL/2<0)
        {
            scoreJ2=scoreJ2+1;
            initBall(b);
            initRaquette(r1,0,(SCREEN_HEIGHT/2)-(RAQUETTE_HEIGHT/2));
            initRaquette(r2,SCREEN_WIDTH-RAQUETTE_WIDTH,(SCREEN_HEIGHT/2)-(RAQUETTE_HEIGHT/2));
        }
        if (scoreJ1>5||scoreJ2>5)
        {
            exit(EXIT_SUCCESS);
        }
        SDL_Delay(5);
    }
    SDL_FreeSurface(screen);//Effacer l'écran
    SDL_Quit();//Quit SDL
}

struct Button
{
    int x,y;
    bool deFace; //booléen indiquant si le bouton est rouge
    SDL_Surface *source4;
    SDL_Rect lectureFront;
    SDL_Rect lectureBack;
};

//coordonnées du rectangle
void setRect(SDL_Rect &rect,int x,int y,int w,int h)
{
    rect.x=x;
    rect.y=y;
    rect.w=w;
    rect.h=h;
}

//place du bouton
void SetBoutonXY(Button &b,int x,int y,bool f)
{
    b.x=x;
    b.y=y;
    b.deFace=f;
}

void menu(SDL_Surface* destination)
{
    SDL_Event souris;
    SDL_Surface *menu;
    menu=load_image("images/fond.bmp");
    applySurface(0,0,menu,destination,NULL);
    Button p;
    SetBoutonXY(p,SCREEN_WIDTH/2-(150/2),SCREEN_HEIGHT/3 -60,true);
    setRect(p.lectureFront,0,80,175,80);
    setRect(p.lectureBack,200,80,175,80);

    Button q;
    SetBoutonXY(q,SCREEN_WIDTH/2-(150/2),SCREEN_HEIGHT/3 +60,true);
    setRect(q.lectureFront,0,0,175,80);
    setRect(q.lectureBack,200,0,175,80);

    q.source4=loadImageWithColorKey("images/button.bmp", 0, 0, 0);
    p.source4=loadImageWithColorKey("images/button.bmp", 0, 0, 0);
    while (true)
    {
        if (p.deFace)
        {
            applySurface(p.x,p.y,p.source4,destination,&p.lectureFront);
        }
        else
        {
            applySurface(p.x,p.y,p.source4,destination,&p.lectureBack);
        }

        if(q.deFace)
        {
            applySurface(q.x,q.y,q.source4,destination,&q.lectureFront);

        }
        else
        {
            applySurface(q.x,q.y,q.source4,destination,&q.lectureBack);
        }

        while(SDL_PollEvent(&souris))
        {
            if(souris.motion.x >=p.x && souris.motion.x <=p.x+150 && souris.motion.y >=p.y && souris.motion.y <=p.y+60)
            {
                p.deFace= false;
                if (souris.button.button== SDL_BUTTON_LEFT)
                {
                    play(destination);
                }
            }
            else
            {
                p.deFace= true;
            }

            if(souris.motion.x >=q.x && souris.motion.x <=q.x+150 && souris.motion.y >=q.y && souris.motion.y <=q.y+60)
            {
                q.deFace= false;
                if(souris.button.button== SDL_BUTTON_LEFT)
                {
                    exit(EXIT_SUCCESS);
                }
            }
            else
            {
                q.deFace= true;
            }
            SDL_Flip(destination);
        }
    }
}

int main(int argc, char **argv)
{
    SDL_Surface *screen;
    SDL_Init(SDL_INIT_EVERYTHING);
    screen=SDL_SetVideoMode(SCREEN_WIDTH,SCREEN_HEIGHT,SCREEN_BPP,SDL_SWSURFACE);

    menu(screen);

    return 0;
}
