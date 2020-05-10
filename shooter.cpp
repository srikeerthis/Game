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
#define MAX_ENEMY_ON_SCREEN 8
#define MAX_VELO_BULLET 5
 
 
static int flag = 0; 
static int shoot = 0;
static int score = 0;

float mycolor[][3]={{1,0.5,1},{0,1,0},{0,0.5,1},{0.5,0.5,0},{0.5,0.5,0.5},{0.5,0,0},{0.5,0,0.5},{0,0.5,0.5}};
int t[MAX_ENEMY_ON_SCREEN]={10,10,10,60,10,70,35},s[MAX_ENEMY_ON_SCREEN]={100,150,200,250,300,350,400,430};
double dt[MAX_ENEMY_ON_SCREEN]={2.75,0.5,1,1.25,1.5,1.25,1,0.75};

    
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
void drawPlayer (Player *p);
void drawEnemy(int);
void movePlayer ();
void moveBullet();
void checkMapBoundries ();
void DoCollision(); 
void updateScore(int);
// Display
void display ();
void myReshape (int, int);
 
void setWindowValues ();
void scoredisplay(int,int,int,int,int);
 
/* -- global variables ------------------------------------------------------ */
 
static glutWindow win;
 
// State of cursor keys
static int up = 0;
static int down = 0;
static int left = 0;
static int right = 0;
 
static Player player;
static Enemy enemy[MAX_ENEMY_ON_SCREEN];
static Bullet bullets[MAX_BULLET_ON_SCREEN]; 
 
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
 
    // Player & enemy -> set parameters including the maximum velocity of the player, the velocity of the laser shots etc
    player.x = win.width/2;
    player.y = 30.0;
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
 
    movePlayer();
    moveBullet();
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
void moveBullet () {
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
        //Bullet Boundries/ Destory bullet outside boundries
        if(bullets[i].active == 1 && (bullets[i].x > win.width || bullets[i].x < 0 || bullets[i].y > win.height || bullets[i].y < 0)) {
            bullets[i].active = 0;
        }
    }
} // end moveBullet()

void drawBullet (Bullet *b) {
 
    glLineWidth(0.5);
    glColor3f(0.9f, 0.0f, 0.0f);
    glPushMatrix();
        myTranslate2D(b->x, b->y);
        myRotate2D(b->bullet_phi);
        myScale2D(b->bsizex, b->bsizey);
        glBegin(GL_TRIANGLES);
            glVertex3f( 0.0f ,  2.0f , 0.0f);   // Top
            glVertex3f(-1.0f , -1.0f , 0.0f);   // Bottom Left
            glVertex3f( 1.0f , -1.0f , 0.0f);   // Bottom Right
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
    int i = 0;
 
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // Clear Screen and Depth Buffer
    glLoadIdentity();
    glTranslatef(0.0f,0.0f,-3.0f);
 
    drawPlayer(&player);
    for(int i=0;i<MAX_ENEMY_ON_SCREEN;i++)
    {
    drawEnemy(i);
    }
    Enemyupdate();
    DoCollision();
    scoredisplay(600,440,-1,1,score);
    
    // Draws the bullets on screen when they are active
    for (i = 0; i < MAX_BULLET_ON_SCREEN; i++) {
        if (bullets[i].active) {
            drawBullet(&bullets[i]);
        }
    }
 
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
 
void drawPlayer (Player *p) {
 
    glLineWidth(1.5);
    //glEnable( GL_LINE_SMOOTH );
    glColor3f(0.2f, 0.9f, 1.0f);
    glPushMatrix();
        myTranslate2D(p->x, p->y);
        myRotate2D(p->phi);
        myScale2D(5,5);
        /* Starting position */
        glBegin(GL_TRIANGLES);
            glVertex3f( 0.0f ,  2.0f , 0.0f);   // Top
            glVertex3f(-1.0f , -1.0f , 0.0f);   // Bottom Left
            glVertex3f( 1.0f , -1.0f , 0.0f);   // Bottom Right
        glEnd();
    glPopMatrix();
}

void drawEnemy (int i){
    if(enemy[i].Destroyed == false){
    glLineWidth(1.5);
    //glEnable( GL_LINE_SMOOTH );
    glColor3fv(mycolor[i]);
    glPushMatrix();
        glTranslatef(enemy[i].x, enemy[i].y,-1);
        myScale2D(enemy[i].sizex,enemy[i].sizey);
        /* Starting position */
        glBegin(GL_POLYGON);
            glVertex3f(-5.0f, 0.0f, 2.0f);// Top left
            glVertex3f( 5.0f, 0.0f, 2.0f);// Top Right
            glVertex3f( 5.0f,-2.0f, 2.0f);// Bottom Right
            glVertex3f(-5.0f,-2.0f, 2.0f);// Bottom Left
        glEnd();
    glPopMatrix();
    }
}

// Updates the motion of enemy
void Enemyupdate()
{
    for (int i = 0; i < MAX_ENEMY_ON_SCREEN; ++i)
   {
        if(enemy[i].Destroyed!=true){
            if(!flag)
            {
                 enemy[i].x+=dt[i];
            if(enemy[i].x>win.width)
                    flag=1;
            }

            if(flag)
            {
                    enemy[i].x+=-(dt[i]);
                if(enemy[i].x<0)
                    flag=0;
            }
        }          
    }
}  

// Check collision between player & enemy or bullet & enemy
void DoCollision()
{
    for (int i = 0 ;i<MAX_BULLET_ON_SCREEN && i<MAX_ENEMY_ON_SCREEN; i++)
    {
         if(enemy[i].Destroyed !=true && bullets[i].active == 1){
            bool xt = (bullets[i].x + bullets[i].bsizex >= (enemy[i].x ) && enemy[i].x + enemy[i].sizex >= (bullets[i].x));
            bool yt = (bullets[i].y + bullets[i].bsizey >= (enemy[i].y ) && enemy[i].y + enemy[i].sizey >= (bullets[i].y));
             if((xt && yt))
             {
                bullets[i].active = 0;
                enemy[i].Destroyed = true;
                score = score + 1;
             }
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