#pragma once

#include <cstdlib>

#include "common.h"

#include <string>
#include <vector>

class Window {
 public:
  Window(int M, int drawingsNo, bool saveToFile, std::string csvFileName = "");

  Gesture last_drawing() { return last_drawing_; }

  void draw(const std::string& window_name);

 private:
  static void myDisplay();
  static void myMouseFunc(int button, int state, int x, int y);
  static void myMouseMove(int x, int y);
  static void draw2(const std::string& window_name);
  void endDraw();

  Point getLastDrawing(int indx) { return last_drawing_[indx]; }

  int lastDrawingSize() { return (int)last_drawing_.size(); }

  Gesture last_drawing_;
  std::vector<Gesture> all_gestures_;
};
