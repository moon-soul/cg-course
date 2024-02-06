// **********************************************************************************************
//
// Template cpp file with minimal functionality for processing
// keyboard and mouse events and for rendering a frame buffer
//
// build command: clang++ renderer.cpp -o renderer -std=c++20 -Wall -Wextra -pedantic -lGL -lglut
//
// **********************************************************************************************

#include <GL/gl.h>
#include <GL/glut.h>

#include <cstring>

constexpr int64_t FRAME_SIZE_X = 1000;
constexpr int64_t FRAME_SIZE_Y = 1000;

uint32_t frame_buf[FRAME_SIZE_X * FRAME_SIZE_Y];

void initGLUT(int argc, char** argv);
void mainLoop();
void onKeyDown(unsigned char key, int x, int y);
void onKeyUp(unsigned char key, int x, int y);
void specialKeyboardInput(int key, int x, int y);
void onWindowResize(int w, int h);
void onMouseMove(int x, int y);
void onMouseClick(int button, int state, int x, int y);

void mainLoop()
{
    memset(frame_buf, 0, sizeof(uint32_t) * FRAME_SIZE_X * FRAME_SIZE_Y);

    //
    // Rendering code
    //

    glRasterPos2i(0, 0);
    glDrawPixels(FRAME_SIZE_X, FRAME_SIZE_Y, GL_RGBA, GL_UNSIGNED_BYTE, frame_buf);

    glutSwapBuffers();
    glutPostRedisplay();
}

void onKeyDown(unsigned char key, int x, int y)
{
}

void onKeyUp(unsigned char key, int x, int y)
{
}

void specialKeyboardInput(int key, int x, int y)
{
}

void onWindowResize(int w, int h)
{
}

void onMouseMove(int x, int y)
{
}

void onMouseClick(int button, int state, int x, int y)
{
}

void initGLUT(int argc, char** argv)
{
    static int arg_cnt;
    glutInit(&arg_cnt, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA);
    glutInitWindowSize(FRAME_SIZE_X, FRAME_SIZE_Y);
    glutInitWindowPosition(500, 150);
    glutCreateWindow("Renderer");
    // glutSetCursor(GLUT_CURSOR_NONE);
    // glClearColor(0.0, 0.0, 0.0, 0.0);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0.0, FRAME_SIZE_X, 0.0, FRAME_SIZE_Y, 0.0, 10.0);
    // glMatrixMode(GL_MODELVIEW);

    glutDisplayFunc(mainLoop);
    glutKeyboardFunc(onKeyDown);
    glutKeyboardUpFunc(onKeyUp);
    glutSpecialFunc(specialKeyboardInput);
    glutPassiveMotionFunc(onMouseMove);
    glutReshapeFunc(onWindowResize);
    glutMouseFunc(onMouseClick);
}

int main(int argc, char** argv)
{
    initGLUT(argc, argv);
    glutMainLoop();

    return 0;
}
