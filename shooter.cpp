#include <stdio.h>
#include <GL/gl.h>          // Open Graphics Library (OpenGL) header
#include <GL/freeglut.h>    // The GL Utility Toolkit (GLUT) Header
#include <stdlib.h>
#include <math.h>
 
// Math defines
#define M_PI 3.14159265358979323846
 
#define RAD2DEG 180.0/M_PI
#define DEG2RAD M_PI/180.0
 
// Keyboard defines
#define KEY_ESCAPE 27
#define KEY_ONE 49
#define KEY_TWO 50
#define KEY_THREE 51 
// Keyboard arrows
#define KEY_UP 101
#define KEY_DOWN 103
#define KEY_LEFT 100
#define KEY_RIGHT 102
 
// 2D function defines, as 3D is not needed
#define myTranslate2D(x,y) glTranslated(x, y, 0.0)
#define myScale2D(x,y) glScalef(x, y, 1.0)
#define myRotate2D(angle) glRotatef(RAD2DEG*angle, 0.0, 0.0, 1.0)
 
// Various game defines
#define MAX_ACC_PLAYER  0.15
#define MAX_VELO_PLAYER 2

#define SPACEBAR 32
#define MAX_BULLET_ON_SCREEN 8
#define MAX_ENEMY_ON_SCREEN 12
#define MAX_VELO_BULLET 5
 
 
static int flag = 0; 
static int shoot = 0;
static int score = 0;
static int countenemy = MAX_ENEMY_ON_SCREEN;

float mycolor[][3]={{1,0.5,1},{0,1,0},{0,0.5,1},{0.5,0.5,0},{0.5,0.5,0.5},{0.5,0,0},{0.5,0,0.5},{0,0.5,0.5},{1,0.5,1},{0,1,0},{0,0.5,1},{0.5,0.5,0},{0.5,0.5,0.5},{0.5,0.5,0},{0.5,0.5,0.5},{0.5,0.5,0},{0.5,0.5,0.5},{0.5,0.5,0},{0.5,0.5,0.5}};
int t[MAX_ENEMY_ON_SCREEN];
int s[MAX_ENEMY_ON_SCREEN];
double dt[MAX_ENEMY_ON_SCREEN];

    
/* -- type definitions ------------------------------------------------------ */
typedef struct {
    int width;
        int height;
        char* title;
 
        float field_of_view_angle;
        float z_near;
        float z_far;
} glutWindow;
  
typedef struct {
    double  x, y, phi, dx, dy, vmax, vmax2, sizex,sizey;
    bool Destroyed;
} Player;

typedef struct {
    double  x, y, phi, dx, dy, vmax, vmax2, sizex,sizey;
    bool Destroyed;
} Enemy;
typedef struct {
    int active;
    double  x, y, dx, dy, bullet_phi,bsizex,bsizey;
}  Bullet;

 
/* -- function prototypes --------------------------------------------------- */
 
static void initialize ();
 
// Keyboard
void keyboard (unsigned char, int, int);
void keyPress (int, int, int);
void keyRelease (int, int, int);
 
void myTimer (int);
void Enemyupdate();
// Player
void drawPlayer ();
void drawEnemy(int);
void movePlayer ();
void moveEnemyBullet(int i);
void movePlayerBullet();
void checkMapBoundries ();
void DoCollision(); 
void updateScore(int);
// Display
void display ();
void myReshape (int, int);
 
void setWindowValues ();
void scoredisplay(int,int,int,int,int);
void mymenu();
void StartGame();
void GameOver();
void OnDestroy();
void OnEnemyDestroy(int i);
 
/* -- global variables ------------------------------------------------------ */
 
static glutWindow win;
 
// State of cursor keys
static int up = 0;
static int down = 0;
static int left = 0;
static int right = 0;
static int menu = 0;
 
static Player player;
static Enemy enemy[MAX_ENEMY_ON_SCREEN];
static Bullet bullets[MAX_BULLET_ON_SCREEN]; 
static Bullet ebullets[MAX_BULLET_ON_SCREEN];  

/* -- functions ------------------------------------------------------------- */
 
int main (int argc, char **argv) {
 
    setWindowValues();
 
    // initialize and run program
    glutInit(&argc, argv);  // GLUT initialization
    glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH );  // Display Mode
    glutInitWindowSize(win.width, win.height); // set window size
    glutCreateWindow("Shoot out"); // create Window
    glutDisplayFunc(display); // register Display Function
    glutIdleFunc(display); // register Idle Function
 
    // Keyboard
    glutIgnoreKeyRepeat(1);
    glutKeyboardFunc(keyboard);
    glutSpecialFunc(keyPress);
    glutSpecialUpFunc(keyRelease); // register Keyboard Handler
 
    glutReshapeFunc(myReshape);
    glutTimerFunc(33, myTimer, 0);
 
    initialize();
 
    glutMainLoop(); // run GLUT mainloop
 
    return 0;
}
 
static void initialize () {
 
    int space = 380/MAX_ENEMY_ON_SCREEN;
    
    for(int i=0;i< MAX_ENEMY_ON_SCREEN;i++)
    {
        if(space*i <400)
        s[i] = 90 + i*space;
        dt[i] = 2.75-i*0.10;
        if(i%2==0)
        t[i]=0;
        else
        t[i] = win.width-10;
    }

    // Player & enemy -> set parameters including the maximum velocity of the player, the velocity of the rocket shots etc
    player.Destroyed = false;
    player.x = win.width/2;
    player.y = 30.0;
    player.sizex = 5;
    player.sizey = 5;
    player.dx = player. dy = 0;
    player.vmax = MAX_VELO_PLAYER;
    player.vmax2 = MAX_VELO_PLAYER * MAX_VELO_PLAYER;

    for(int i=0;i<MAX_ENEMY_ON_SCREEN;i++)
    {
    enemy[i].Destroyed = false;
    enemy[i].x = t[i] ;
    enemy[i].y = s[i];
    enemy[i].sizex = 5;
    enemy[i].sizey = 5;
    enemy[i].phi = - DEG2RAD *180;    
    } 
}
 
void keyboard ( unsigned char key, int x , int y) {
 
    switch (key) {
 
        // Quits game
        case KEY_ESCAPE:
            exit (0);
            break;
        case SPACEBAR:
            shoot = 1;
            break;
        case KEY_ONE:
            menu = 1;
            break;
        case KEY_TWO:
            menu = 2;
            break;
        case KEY_THREE:
            menu = 3;
            break;
        default:
            break;
    }
}
 
// This function is called when a special key is pressed; 
void keyPress (int key, int x, int y) {
 
    switch (key) {
        case KEY_LEFT:
            left = 1;
            break;
 
        case KEY_UP:
            up = 1;
            break;
 
        case KEY_RIGHT:
            right = 1;
            break;
 
        case KEY_DOWN:
            down = 1;
            break;
 
        default:
            break;
    }
}
 
// This function is called when a special key is released
void keyRelease (int key, int x, int y) {
 
    switch (key) {
        case KEY_LEFT:
            left = 0;
            break;
 
        case KEY_UP:
            up = 0;
            break;
 
        case KEY_RIGHT:
            right = 0;
            break;
 
        case KEY_DOWN:
            down = 0;
            break;
 
        default:
            break;
    }
}
 
// Update
void myTimer (int value) {
    if(player.Destroyed == false)
    {
    movePlayer();
    movePlayerBullet();
    }
    if(countenemy <= MAX_ENEMY_ON_SCREEN ){
    for(int i=0;i<MAX_ENEMY_ON_SCREEN;i++)
    {
        if(enemy[i].Destroyed == false)
            moveEnemyBullet(i);           
    }    
    }
    checkMapBoundries();
    glutPostRedisplay();
    glutTimerFunc(30, myTimer, value);      // 30 frames per second 
 }
 
void movePlayer () {
 
     // Player Rotation
    if(left && right) {
        //do nothing
    }
     else if(left == 1) {
         player.phi = player.phi + DEG2RAD * 11.5; //Larger the number the faster it will rotate
     }
     else if(right == 1) {
         player.phi = player.phi - DEG2RAD * 11.5;
     }
 
    // Player Movement
    if(up && down) {
        // stop player
        player.dx = 0.0;
        player.dy = 0.0;
    }
    // Moves player forwards
    else if(up == 1) {
        player.dx = player.dx - MAX_ACC_PLAYER * sin(player.phi);
        player.dy = player.dy + MAX_ACC_PLAYER * cos(player.phi);
    }
    // Moves player backwards
    else if(down == 1) {
        player.dx = player.dx + (MAX_ACC_PLAYER - 0.09) * sin(player.phi);
        player.dy = player.dy - (MAX_ACC_PLAYER - 0.09) * cos(player.phi);
    }
    // Slows player down when up && down movement keys are not pressed
    else if (up == 0 && down == 0) {
 
        if (player.dx > 0) {
            player.dx -= 0.2;
        }
 
        if (player.dy > 0) {
            player.dy -= 0.2;
        }
 
        if (player.dx < 0 || player.dy < 0) {
            player.dx = 0;
            player.dy = 0;
        }
    }
 
    double temp;
    //If the player exceeds the max velocity (moving backwards), limit the velocity
    if(down == 1 && (temp = (player.dx * player.dx + player.dy * player.dy)) > (player.vmax)) {
        temp = player.vmax / sqrt(temp);
        player.dx *= temp - 0.5;
        player.dy *= temp - 0.5;
    }
     //If the player exceeds the max velocity (moving forwards), limit the velocity
    else if((temp = (player.dx * player.dx + player.dy * player.dy)) > player.vmax2) {
        temp = player.vmax / sqrt(temp);
        player.dx *= temp;
        player.dy *= temp;
    }
 
    // Puts the math in motion
    player.x = player.x + player.dx;
    player.y = player.y + player.dy;
 
} // end movePlayer()
 
// Bullets
void moveEnemyBullet (int i) {
    // Give the bullets velocity if shoot is true.
    
            if(ebullets[i].active == 0) {
                ebullets[i].active = 1;
                ebullets[i].x = enemy[i].x;
                ebullets[i].y = enemy[i].y;
                ebullets[i].bullet_phi = enemy[i].phi;
                ebullets[i].dx = -MAX_VELO_BULLET * sin(enemy[i].phi);
                ebullets[i].dy = MAX_VELO_BULLET * cos(enemy[i].phi);
                ebullets[i].bsizex = 3;
                ebullets[i].bsizey = 3;
        }
    
 
    // Advance bullets and eliminating those that have gone pastthe window boundaries 
    
        if(ebullets[i].active == 1) {
            ebullets[i].x = ebullets[i].x + ebullets[i].dx;
            ebullets[i].y = ebullets[i].y + ebullets[i].dy;
        }
        //Bullet Boundries/ Destroy bullet outside boundries
        if(ebullets[i].active == 1 && (ebullets[i].x > win.width || ebullets[i].x < 0 || ebullets[i].y > win.height || ebullets[i].y < 0)) {
            ebullets[i].active = 0;
        }
} // end moveBullet()

void movePlayerBullet () {
    int i = 0;
 
    // Give the bullets velocity if shoot is true.
    if (shoot == 1) {
 
         for(i = 0; i < MAX_BULLET_ON_SCREEN; i++) {
 
            if(bullets[i].active == 0) {
                bullets[i].active = 1;
                bullets[i].x = player.x;
                bullets[i].y = player.y;
                bullets[i].bullet_phi = player.phi;
                bullets[i].dx = -MAX_VELO_BULLET * sin(player.phi);
                bullets[i].dy = MAX_VELO_BULLET * cos(player.phi);
                bullets[i].bsizex = 3;
                bullets[i].bsizey = 3;
                break;
            }
        }
 
        // Resets shoot key to prevent rapid fire
        shoot = 0;
    }
 
    // Advance bullets and eliminating those that have gone pastthe window boundaries 
    for(i = 0; i < MAX_BULLET_ON_SCREEN; i++) {
 
        if(bullets[i].active == 1) {
            bullets[i].x = bullets[i].x + bullets[i].dx;
            bullets[i].y = bullets[i].y + bullets[i].dy;
        }
        //Bullet Boundries/ Destroy bullet outside boundries
        if(bullets[i].active == 1 && (bullets[i].x > win.width || bullets[i].x < 0 || bullets[i].y > win.height || bullets[i].y < 0)) {
            bullets[i].active = 0;
        }
    }
} // end moveBullet()

void drawPlayerBullet (int i) {
 
    glColor3f(0.9f, 0.0f, 0.0f);
    glPushMatrix();
        myTranslate2D(bullets[i].x, bullets[i].y);
        myRotate2D(bullets[i].bullet_phi);
        myScale2D(bullets[i].bsizex, bullets[i].bsizey);
        glBegin(GL_TRIANGLES);
            glVertex2f( 0.0f ,  2.0f );   // Top
            glVertex2f(-1.0f , -1.0f );   // Bottom Left
            glVertex2f( 1.0f , -1.0f );   // Bottom Right
        glEnd();
    glPopMatrix();
}

void drawEnemyBullet(int i)
{
        glColor3f(0.0f, 1.0f, 0.0f);
        glPushMatrix();
            myTranslate2D(ebullets[i].x, ebullets[i].y);
            myRotate2D(ebullets[i].bullet_phi);
            myScale2D(ebullets[i].bsizex, ebullets[i].bsizey);
            glBegin(GL_TRIANGLES);
                glVertex2f( 0.0f ,  2.0f );   // Top
                glVertex2f(-1.0f , -1.0f );   // Bottom Left
                glVertex2f( 1.0f , -1.0f );   // Bottom Right
            glEnd();
        glPopMatrix();   
}

// Map boundaries - prevents player from going out of designated map area 
void checkMapBoundries () {
 
    if(player.x > win.width) {
        player.x = win.width;
    }
    if(player.x < 0) {
        player.x = 0;
    }
    if(player.y > win.height) {
        player.y = win.height;
    }
    if(player.y < 0) {
        player.y = 0;
    }
}
 
/**
 * The display callback handles exposure events and is called whenever the display must be refreshed.
 * Values can be passed to the display callback function only by means of global variables.
 */
/**
 * The display callback handles exposure events and is called whenever the display must be refreshed.
 * Values can be passed to the display callback function only by means of global variables.
 */
void display () {
    
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // Clear Screen and Depth Buffer
    glLoadIdentity();
    glTranslatef(0.0f,0.0f,-3.0f);

    mymenu();

    glutSwapBuffers();
}
 
void myReshape (int w, int h) {
 
    glMatrixMode(GL_PROJECTION); // select projection matrix
    glViewport(0, 0, win.width, win.height); // set the viewport
    glMatrixMode(GL_PROJECTION);  // set matrix mode
    glLoadIdentity(); // reset projection matrix
   
    glOrtho(0.0, win.width, 0.0, win.height, win.z_near, win.z_far);
   
    glMatrixMode(GL_MODELVIEW); // specify which matrix is the current matrix
    glShadeModel( GL_SMOOTH );
    glClearDepth( 1.0f );   // specify the clear value for the depth buffer
    glEnable( GL_DEPTH_TEST );
    glDepthFunc( GL_LEQUAL );
    glHint( GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST ); // specify implementation-specific hints
    glClearColor(0.0, 0.0, 0.02, 1.0); // specify clear values for the color buffers
 
}
 
// Sets game window specifications
void setWindowValues () {
 
    win.width = 640;
    win.height = 480;
    win.field_of_view_angle = 0;
    win.z_near = 1.0f;
    win.z_far = 100.0f;
}
 
/* -- drawing functions ----------------------------------------------------- */
 
void drawPlayer () {
    glColor3f(0.2f, 0.9f, 1.0f);
    glPushMatrix();
        myTranslate2D(player.x, player.y);
        myRotate2D(player.phi);
        myScale2D(player.sizex,player.sizey);
        /* Starting position */
        glBegin(GL_TRIANGLES);
            glVertex2f( 0.0f ,  2.0f );   // Top
            glVertex2f(-1.0f , -1.0f );   // Bottom Left
            glVertex2f( 1.0f , -1.0f );   // Bottom Right
        glEnd();
    glPopMatrix();
}

void drawEnemy (int i){
    if(enemy[i].Destroyed == false){
    glColor3fv(mycolor[i]);
    glPushMatrix();
        myTranslate2D(enemy[i].x-27 , enemy[i].y);
        myScale2D(enemy[i].sizex,enemy[i].sizey);
        /* Starting position */
        glBegin(GL_POLYGON);
            glVertex2f( 5.0f, 0.0f);// Top left
            glVertex2f( 10.0f,0.0f);// Top Right
            glVertex2f( 5.0f,-2.0f);// Bottom Right
            glVertex2f( 0.0f, 0.0f);
            glVertex2f( -5.0f, -2.0f);// Top left
            glVertex2f( 0.0f,-2.0f);
        glEnd();
    glPopMatrix();
    }
}

// Updates the motion of enemy
void Enemyupdate()
{
    for (int i = 0; i < MAX_ENEMY_ON_SCREEN; ++i)
   {
        if(enemy[i].Destroyed == false){
            if(i%2 == 0){
            if(!flag)
            {
                 enemy[i].x+=dt[i];
            if(enemy[i].x>win.width)
                    flag=1;
            }

            if(flag)
            {
                    enemy[i].x+=-(dt[i]);
                if(enemy[i].x<5)
                    flag=0;
            }
        }
        else
        {
         if(!flag)
            {
                enemy[i].x+=-(dt[i]);
                if(enemy[i].x<5)
                    flag=1;
            }

            if(flag)
            {
                enemy[i].x+=dt[i];
            if(enemy[i].x>win.width)
                    flag=0;
            }   
        }
    }
    }
}  

// Check collision between player & enemy or bullet & enemy
void DoCollision()
{
    bool xt,yt,zt,ut,et,ft,playxt,playyt;
    for (int i = 0 ;i<MAX_BULLET_ON_SCREEN && i<MAX_ENEMY_ON_SCREEN; i++)
    {
         if(enemy[i].Destroyed == false && bullets[i].active == 1){
            xt = (bullets[i].x + bullets[i].bsizex * 3 >= enemy[i].x && enemy[i].x + enemy[i].sizex *5 >= bullets[i].x);
            yt = (bullets[i].y + bullets[i].bsizey * 3 >= enemy[i].y && enemy[i].y + enemy[i].sizey *5 >= bullets[i].y);

            zt = (bullets[i].x + bullets[i].bsizex * 3 <= -(enemy[i].x) && -(enemy[i].x) + -(enemy[i].sizex) *5 <= bullets[i].x);
            ut = (bullets[i].y + bullets[i].bsizey * 3 <= -(enemy[i].y) && -(enemy[i].y) + -(enemy[i].sizey) *5 <= bullets[i].y);
             
             if(xt && yt || zt && ut)
             {
                bullets[i].active = 0;
                ebullets[i].active = 0;
                enemy[i].Destroyed = true;
                score = score + 1;
                countenemy = countenemy - 1;
             }
        }
         playxt = player.x + player.sizex * 4 >= enemy[i].x && enemy[i].x + enemy[i].sizex *5 >= player.x;
         playyt = player.y + player.sizey * 4 >= enemy[i].y && enemy[i].y + enemy[i].sizey *5 >= player.y;   
         
         et = (ebullets[i].x + ebullets[i].bsizex * 3 >= player.x && player.x + player.sizex * 3 >= ebullets[i].x);
         ft = (ebullets[i].y + ebullets[i].bsizey * 3 >= player.y && player.y + player.sizey * 3 >= ebullets[i].y);
   
        if(playxt && playyt || et && ft )
        {
            ebullets[i].active = 0;
            bullets[i].active = 0;
            player.Destroyed = true;
        }
    }
}

// Update score
void scoredisplay (int posx, int posy, int posz, int space_char, int scorevar)
{
        int j=0,p,k;
        GLvoid *font_style1 = GLUT_BITMAP_TIMES_ROMAN_24;
        p = scorevar;
        j = 0;
        k = 0;
        glRasterPos2i(520, 440);
        glColor3f(0.0f, 0.0f, 1.0f);
        const unsigned char* t = reinterpret_cast<const unsigned char *>("Score: ");
        glutBitmapString(font_style1, t);
        while(p > 9)
        {
            k = p % 10;
            glRasterPos3f ((posx-(j*space_char)),posy, posz);   
            glutBitmapCharacter(font_style1,48+k);
            j++;
            p /= 10;
        }
            glRasterPos3f ((posx-(j*space_char)), posy, posz);   
            glutBitmapCharacter(font_style1,48+p);
      glFlush();
}
// Game over text
void GameOver()
{
    GLvoid *font_style1 = GLUT_BITMAP_TIMES_ROMAN_24;
    glRasterPos2f(270,240);
    glColor3f(1.0f, 0.0f, 0.7f);
    if(countenemy == 0)
    {
        const unsigned char* w = reinterpret_cast<const unsigned char *>("You win");
        glutBitmapString(font_style1, w);
    }
    else
    {    
        const unsigned char* t = reinterpret_cast<const unsigned char *>("Game Over !");
        glutBitmapString(font_style1, t);
    }
    for(int i=0;i<MAX_BULLET_ON_SCREEN;i++)
    {
        bullets[i].active=0;
        ebullets[i].active=0;
    }
}

//Loads the game
void StartGame()
{

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // Clear Screen and Depth Buffer

    if(player.Destroyed == false)
        drawPlayer();

    for(int i=0;i<MAX_ENEMY_ON_SCREEN;i++)
    {
        drawEnemy(i);
    }
    
    Enemyupdate();
    DoCollision();
    if(player.Destroyed == true)
        OnDestroy();

    for(int i=0;i<MAX_ENEMY_ON_SCREEN;i++)
        if(enemy[i].Destroyed == true)
            OnEnemyDestroy(i);

    scoredisplay(600,440,-1,1,score);
    
    if(player.Destroyed == true || countenemy == 0)
        GameOver();
    
    // Draws the bullets on screen when they are active
    for (int i = 0; i < MAX_BULLET_ON_SCREEN; i++) {
        if (ebullets[i].active) {
            drawEnemyBullet(i);
        }
        if (bullets[i].active) {
            drawPlayerBullet(i);
        }
    }
}

//Loads the instructions
void Instructions()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // Clear Screen and Depth Buffer

    GLvoid *font_style1 = GLUT_BITMAP_TIMES_ROMAN_24;
    glRasterPos2f(270,400);
    glColor3f(1.0f, 1.0f, 0.7f);
    const unsigned char* w = reinterpret_cast<const unsigned char *>("Instructions\n");
    glutBitmapString(font_style1, w);
    
    glRasterPos2f(20,340);
    glColor3f(1.0f, 0.0f, 0.7f);
    const unsigned char* o = reinterpret_cast<const unsigned char *>("Objective:\nEvade and destroy all the enemies on screen \n");
    glutBitmapString(font_style1, o);
    
    glRasterPos2f(20,240);
    glColor3f(1.0f, 0.3f, 0.7f);
    const unsigned char* p = reinterpret_cast<const unsigned char *>("0. Up and Down arrow keys - move player\n");
    const unsigned char* q = reinterpret_cast<const unsigned char *>("1. Left and Right arrow keys - control the direction\n");
    const unsigned char* r = reinterpret_cast<const unsigned char *>("2. Space button - Fire a rocket\n3. Esc -end game and exit as well\n");
    glutBitmapString(font_style1, p);
    glutBitmapString(font_style1, q);
    glutBitmapString(font_style1, r);   
}

//Loads the main menu
void mymenu()
{
    GLvoid *font_style1 = GLUT_BITMAP_TIMES_ROMAN_24;
    glRasterPos2f(240,360);
    glColor3f(1.0f, 0.0f, 0.7f);
    const unsigned char* s = reinterpret_cast<const unsigned char *>("Hit a number to select\n");
    glutBitmapString(font_style1, s);
    
    glRasterPos2f(240,260);
    glColor3f(1.0f, 0.0f, 0.0f);
    const unsigned char* t = reinterpret_cast<const unsigned char *>("1. Start Game\n");
    glutBitmapString(font_style1, t);
    const unsigned char* u = reinterpret_cast<const unsigned char *>("2. Instructions\n");
    glutBitmapString(font_style1, u);
    const unsigned char* v = reinterpret_cast<const unsigned char *>("3. Quit\n");
    glutBitmapString(font_style1, v);  

    switch(menu)
    {
        case 1:
            StartGame();
            break;
        case 2:
            Instructions();
            break;   
        case 3:
            exit(0);
            break;
        default:
            break;     
    }
}

void OnDestroy()
{
    glColor3f(0.0f, 0.0f, 1.0f);
    glPushMatrix();
        myTranslate2D(player.x-4, player.y-5);
        myRotate2D(45);
        myScale2D(player.sizex-2,player.sizey-2);
        /* Starting position */
        glBegin(GL_TRIANGLES);
            glVertex2f( 1.0f , -1.0f );   // Top
            glVertex2f(-1.0f , -1.0f );   // Bottom Left
            glVertex2f( 0.0f ,  2.0f );   // Bottom Right
        glEnd();
    glPopMatrix();

    glColor3f(0.0f, 0.0f, 1.0f);
    glPushMatrix();
        myTranslate2D(player.x+4, player.y+1);
        myRotate2D(60);
        myScale2D(player.sizex-1.5,player.sizey-1.5);
        /* Starting position */
        glBegin(GL_TRIANGLES);
            glVertex2f( 0.0f ,  2.0f );   // Top
            glVertex2f(-1.0f , -1.0f );   // Bottom Left
            glVertex2f( 1.0f , -1.0f );   // Bottom Right
        glEnd();
    glPopMatrix(); 

    glColor3f(0.0f, 0.0f, 1.0f);
    glPushMatrix();
        myTranslate2D(player.x, player.y);
        myRotate2D(30);
        myScale2D(player.sizex-2.5,player.sizey-2.5);
        /* Starting position */
        glBegin(GL_TRIANGLES);
            glVertex2f( 0.0f ,  2.0f );   // Top
            glVertex2f(-1.0f , -1.0f );   // Bottom Left
            glVertex2f( 1.0f , -1.0f );   // Bottom Right
        glEnd();
    glPopMatrix();    
}

void OnEnemyDestroy(int i)
{
    glColor3fv(mycolor[i]);
    glPushMatrix();
        myTranslate2D(enemy[i].x-4, enemy[i].y-5);
        myRotate2D(45);
        myScale2D(enemy[i].sizex-2,enemy[i].sizey-2);
        /* Starting position */
        glBegin(GL_TRIANGLES);
            glVertex2f( 1.0f , -1.0f );   // Top
            glVertex2f(-1.0f , -1.0f );   // Bottom Left
            glVertex2f( 0.0f ,  2.0f );   // Bottom Right
        glEnd();
    glPopMatrix();

    glColor3fv(mycolor[i]);
    glPushMatrix();
        myTranslate2D(enemy[i].x+14, enemy[i].y+11);
        myRotate2D(60);
        myScale2D(enemy[i].sizex-1.5,enemy[i].sizey-1.5);
        /* Starting position */
        glBegin(GL_TRIANGLES);
            glVertex2f( 0.0f ,  2.0f );   // Top
            glVertex2f(-1.0f , -1.0f );   // Bottom Left
            glVertex2f( 1.0f , -1.0f );   // Bottom Right
        glEnd();
    glPopMatrix(); 

    glColor3fv(mycolor[i]);
    glPushMatrix();
        myTranslate2D(enemy[i].x, enemy[i].y);
        myRotate2D(30);
        myScale2D(enemy[i].sizex-12.5,enemy[i].sizey-12.5);
        /* Starting position */
        glBegin(GL_TRIANGLES);
            glVertex2f( 0.0f ,  2.0f );   // Top
            glVertex2f(-1.0f , -1.0f );   // Bottom Left
            glVertex2f( 1.0f , -1.0f );   // Bottom Right
        glEnd();
    glPopMatrix();    

    glColor3fv(mycolor[i]);
    glPushMatrix();
        myTranslate2D(enemy[i].x, enemy[i].y-15);
        myRotate2D(70);
        myScale2D(enemy[i].sizex-2.5,enemy[i].sizey-2.5);
        /* Starting position */
        glBegin(GL_TRIANGLES);
            glVertex2f( 0.0f ,  2.0f );   // Top
            glVertex2f(-1.0f , -1.0f );   // Bottom Left
            glVertex2f( 1.0f , -1.0f );   // Bottom Right
        glEnd();
    glPopMatrix();    
}