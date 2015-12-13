#include "window.h"

#include "GL/freeglut.h"
#include "GL/gl.h"
#include "GL/glut.h"

#include <string.h>
#include <thread>

using std::vector;
using std::string;

namespace {
/* Local variables for glut */
Gesture last_drawing_local_;
int M_local_;
}

Window::Window(int M, int drawingsNo, bool saveToFile, string csvFileName) {
  M_local_ = M;
  if (!saveToFile) {
    while (drawingsNo--) {
      draw("Draw any gesture");
    }
  } else {
    all_gestures_.clear();

    FILE* f = fopen(csvFileName.c_str(), "w");
    if (f == NULL) {
      printf("Cannot open file %s!\n", csvFileName.c_str());
      exit(1);
    }
    for (int i = 0; i < (int)kClassNames.size(); ++i) {
      for (int j = 0; j < drawingsNo; ++j) {
        printf("%d/%d Draw letter %s\n", j + 1, drawingsNo,
               kClassNames[i].c_str());
        draw(kClassNames[i].c_str());
        last_drawing_.setTargetClass(i);
        printf("%s\n", last_drawing_.toCsvString().c_str());
        all_gestures_.push_back(last_drawing_);
      }
    }

    for (Gesture& g : all_gestures_) {
      fprintf(stderr, "%s\n", g.toCsvString().c_str());
      fprintf(f, "%s\n", g.toCsvString().c_str());
    }

    fclose(f);
  }
}

void Window::myDisplay() {
  glClear(GL_COLOR_BUFFER_BIT);
  glClearColor(1.0, 1.0, 1.0, 0.0);
  //glLineWidth(1.0);
  glColor3f(1.0, 0.0, 0.0);
  glBegin(GL_LINE_STRIP);
  int n = (int)last_drawing_local_.size();
  for (int i = 0; i < n; ++i) {
    Point p = last_drawing_local_[i];
    glVertex2f((int)p.x, (int)p.y);
  }
  glEnd();
  glutSwapBuffers();
  glutPostRedisplay();
}

void Window::myMouseFunc(int button, int state, int x, int y) {
  if (button == GLUT_RIGHT_BUTTON && state == GLUT_DOWN) {
    last_drawing_local_.normalize(M_local_);
    puts("Exiting glut loop");
    glutLeaveMainLoop();
  }
}

void Window::myMouseMove(int x, int y) {
  last_drawing_local_.push_back(Point(x, y));
}

void Window::draw2(const string& window_name) {
  char* myargv[1];
  int myargc = 1;
  myargv[0] = strdup("DrawApp");
  glutInit(&myargc, myargv);
  glutSetOption(GLUT_ACTION_ON_WINDOW_CLOSE, GLUT_ACTION_GLUTMAINLOOP_RETURNS);

  glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
  glutInitWindowSize(600, 600);
  glutInitWindowPosition(100, 100);
  glutCreateWindow(window_name.c_str());

  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity;
  glOrtho(0, 600, 600, 0, -1, 0);

  glutDisplayFunc(myDisplay);
  glutMouseFunc(myMouseFunc);
  glutMotionFunc(myMouseMove);

  glutMainLoop();
}

void Window::draw(const string& window_name) {
  last_drawing_.clear();
  last_drawing_local_.clear();

  std::thread t([&](){draw2(window_name);});
  t.join();

  last_drawing_ = last_drawing_local_;
}

