// $Id: square.cpp,v 1.14 2017-08-08 15:32:23-07 - - $

// Draw a square.

#include <string>
using namespace std;

#include <GL/freeglut.h>
#include <libgen.h>

struct {
   string name;
   int width {256};
   int height {256};
} window;

const GLubyte RED[] = {0xFF, 0x00, 0x00};
const GLubyte GREEN[] = {0x00, 0xFF, 0x00};

void draw_square (GLfloat xpos, GLfloat ypos,
                  GLfloat width, const GLubyte* color) {
   glBegin (GL_POLYGON);
   glColor3ubv (color);
   glVertex2f (xpos, ypos);
   glVertex2f (xpos + width, ypos);
   glVertex2f (xpos + width, ypos + width);
   glVertex2f (xpos, ypos + width);
   glEnd();
}

void display() {
   glClearColor (0.0, 0.0, 1.0, 1.0);
   glClear (GL_COLOR_BUFFER_BIT);
   draw_square (window.width * 0.15, window.height * 0.15,
                window.height * 0.70, RED);
   draw_square (window.width * 0.30, window.height * 0.30,
                window.height * 0.40, GREEN);
   glutSwapBuffers();
}

void reshape (int width, int height) {
   window.width = width;
   window.height = height;
   glMatrixMode (GL_PROJECTION);
   glLoadIdentity();
   gluOrtho2D (0, window.width, 0, window.height);
   glMatrixMode (GL_MODELVIEW);
   glViewport (0, 0, window.width, window.height);
   glutPostRedisplay();
}

int main (int argc, char** argv) {
   glutInit (&argc, argv);
   glutInitDisplayMode (GLUT_RGBA | GLUT_DOUBLE);
   glutInitWindowSize (window.width, window.height);
   glutInitWindowPosition (128, 128);
   glutCreateWindow (basename (argv[0]));
   glutDisplayFunc (display);
   glutReshapeFunc (reshape);
   glutMainLoop();
   return 0;
}

