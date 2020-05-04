#include <stdio.h>
#include <GL/gl.h>          // Open Graphics Library (OpenGL) header
#include <GL/freeglut.h>    // The GL Utility Toolkit (GLUT) Header
#include <time.h>
#include <stdlib.h>
#include <math.h>
 
// Math defines
#ifndef M_PI
  #define M_PI 3.14159265358979323846
#endif
 
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
#define MAX_VELO_BULLET 5
 
typedef struct {
    int active;
    double  x, y, dx, dy, bullet_phi,bsizex,bsizey;
}  Bullet;
 
static int flag = 0; 
static int shoot = 0;
static Bullet bullets[MAX_BULLET_ON_SCREEN]; 
/* -- type definitions ------------------------------------------------------ */
typedef struct {
    int width;
        int height;
        char* title;
 
        float field_of_view_angle;
        float z_near;
        float z_far;
} glutWindow;
 
typedef struct Coords {
    double x, y;
} Coords;
 
typedef struct {
    double  x, y, phi, dx, dy, vmax, vmax2, sizex,sizey;
} Player;

typedef struct {
    double  x, y, phi, dx, dy, vmax, vmax2, sizex,sizey;
    bool Destroyed;
} Enemy;
 
/* -- function prototypes --------------------------------------------------- */
 
static void initialize ();
 
// Keyboard
void keyboard (unsigned char, int, int);
void keyPress (int, int, int);
void keyRelease (int, int, int);
 
void myTimer (int);
void Enemyupdate(Enemy *e);
// Player
void drawPlayer (Player *p);
void drawEnemy(Enemy *e);
void movePlayer ();
void moveBullet();
void checkMapBoundries ();
void DoCollision(Enemy *e); 
// Display
void display ();
void myReshape (int, int);
 
void setWindowValues ();
 
/* -- global variables ------------------------------------------------------ */
 
static glutWindow win;
 
// State of cursor keys
static int up = 0;
static int down = 0;
static int left = 0;
static int right = 0;
 
static double x2;
static double y2;
static Player player;
static Enemy enemy;
static Coords coords;
 
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
 
    /** PLAYER
     * set parameters including the numbers photons present,
     * the maximum velocity of the player, the velocity of the laser shots, the
     * player's coordinates and velocity, etc.
     */
    player.x = win.width/2;
    player.y = 30.0;
    player.dx = player. dy = 0;
    player.vmax = MAX_VELO_PLAYER;
    player.vmax2 = MAX_VELO_PLAYER * MAX_VELO_PLAYER;

    enemy.Destroyed = false;
    enemy.x = 0;
    enemy.y = win.height/2.0;
    enemy.dx = enemy. dy = 0;
    enemy.vmax = MAX_VELO_PLAYER;
    enemy.vmax2 = MAX_VELO_PLAYER * MAX_VELO_PLAYER;
    enemy.sizex = 5;
    enemy.sizey = 5;
 
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
 
/**
 *  This function is called when a special key is pressed; we are
 *  interested in the cursor keys only
 */
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
 
/**
 *  This function is called when a special key is released; we are
 *  interested in the cursor keys only
 */
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
 
/**
 * Update
 */
void myTimer (int value) {
 
    movePlayer();
    moveBullet();
    checkMapBoundries();
    glutPostRedisplay();
    glutTimerFunc(30, myTimer, value);      /* 30 frames per second */
 
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
 
/**
 *  Bullets
 */
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
 
    /* Advance bullets and eliminating those that have gone past
     * the window boundaries
     */
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

/**
 * Map boundaries - prevents player from going out of designated map area
 */
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
    drawEnemy(&enemy);
    Enemyupdate(&enemy);
    DoCollision(&enemy);
    
    // Draws the bullets on screen when they are active
    for (i = 0; i < MAX_BULLET_ON_SCREEN; i++) {
        if (bullets[i].active) {
            drawBullet(&bullets[i]);
        }
    }
 
    glutSwapBuffers();
}
 
/**
 *  reshape callback function; the aspect ratio is
 *  determined by the aspect ratio of the viewport
 */
void myReshape (int w, int h) {
 
    glMatrixMode(GL_PROJECTION); // select projection matrix
    glViewport(0, 0, win.width, win.height); // set the viewport
    glMatrixMode(GL_PROJECTION);  // set matrix mode
    glLoadIdentity(); // reset projection matrix
    //GLfloat aspect = (GLfloat) win.width / win.height;
 
    glOrtho(0.0, win.width, 0.0, win.height, win.z_near, win.z_far);
    //gluPerspective(win.field_of_view_angle, aspect, win.z_near, win.z_far);  // set up a perspective projection matrix
 
    glMatrixMode(GL_MODELVIEW); // specify which matrix is the current matrix
    glShadeModel( GL_SMOOTH );
    glClearDepth( 1.0f );                                                                                                               // specify the clear value for the depth buffer
    glEnable( GL_DEPTH_TEST );
    glDepthFunc( GL_LEQUAL );
    glHint( GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST ); // specify implementation-specific hints
    glClearColor(0.0, 0.0, 0.02, 1.0); // specify clear values for the color buffers
 
}
 
/**
 * Sets game window specifications
 */
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
    glEnable( GL_LINE_SMOOTH );
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

void drawEnemy (Enemy *e){
    if(e->Destroyed == false){
    glLineWidth(1.5);
    glEnable( GL_LINE_SMOOTH );
    glColor3f(1.f, 0.2f, 1.0f);
    glPushMatrix();
        glTranslatef(e->x, e->y,-1);
        myRotate2D(e->phi);
        myScale2D(e->sizex,e->sizey);
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

void Enemyupdate(Enemy *e)
{
   
    if(!flag)
    {
            e->x+=2;
        if(e->x>win.width)
            flag=1;
    }

    if(flag)
    {
            e->x+=-3;
        if(e->x<0)
            flag=0;
    }
}  

void DoCollision(Enemy *e)
{
    for (int i = 0 ;i<MAX_BULLET_ON_SCREEN; i++)
    {
        bool xt = ((bullets[i].x + bullets[i].bsizex >= e->x-e->sizex || bullets[i].x + bullets[i].bsizex >= e->x -2*(e->sizex)) && (e->x + e->sizex >= bullets[i].x-bullets[i].bsizex || e->x + e->sizex >= bullets[i].x -2*(bullets[i].bsizex)));
        bool yt = ((bullets[i].y + bullets[i].bsizey >= e->y-e->sizex || bullets[i].y + bullets[i].bsizey >= e->y -2*(e->sizex)) && (e->y + e->sizey >= bullets[i].y-bullets[i].bsizey || e->y + e->sizey >= bullets[i].y -2*(bullets[i].bsizey)));
     if(bullets[i].active == 1 && (xt && yt))
     {
        e->Destroyed = true;
        bullets[i].active = 0;
     }       
    }
}
