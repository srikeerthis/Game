#include <stdlib.h>

#include <GL/freeglut.h>

#include <iostream>

// including files

using namespace std;

 	float xr = 0, yr = 0; //to control the object’s movement

 	void specialkey(int key,int x, int y){
 		switch(key){
 			case GLUT_KEY_UP:
 				yr++;
 				cout<<y<<endl;
 				glutPostRedisplay();
 				break;
 			case GLUT_KEY_DOWN:
 				yr--;
 				cout<<y<<endl;
 				glutPostRedisplay();
 				break;
 			case GLUT_KEY_LEFT:
 				xr--;
 				cout<<x<<endl;
 				glutPostRedisplay();
 				break;
 			case GLUT_KEY_RIGHT:
 				xr++;
 				cout<<x<<endl;
 				glutPostRedisplay();
 				break;	
 		}
 	}

 	void display(){
 		glClear(GL_COLOR_BUFFER_BIT);
 		glColor3f(0,0,1);

 		glBegin(GL_QUADS);
 		glVertex2f(200+xr,100+yr);
 		glVertex2f(300+xr,100+yr);
 		glVertex2f(300+xr,200+yr);
 		glVertex2f(200+xr,200+yr);
 		glEnd();

 		glFlush();
 		glutPostRedisplay();
 		glutSwapBuffers();
 	}

 	int main(int argc, char** argv){

 		glutInit(&argc,argv);
 		glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
 		glutInitWindowSize(400,400);
 		glutInitWindowPosition(600,50);
 		glutCreateWindow("Shooter");

 		glutDisplayFunc(display);

 		glClearColor(0,1,0,0);
 		gluOrtho2D(0.0,400,0.0,400);
 		glutSpecialFunc(specialkey);

 		glutMainLoop();
 	}