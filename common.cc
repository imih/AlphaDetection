#include "common.h"

#include <cassert>

using std::string;
using std::vector;

Gesture Gesture::parse(const string& csv_format) {
  Gesture gesture;
  gesture.clear();
  vector<double> tokens = parseT<double>(csv_format, ',');
  for(int i = 0; i < kClassNum; ++i) {
    if((int) tokens[i] == 1) {
      gesture.setTargetClass(i);
      break;
    }
  }

  for(int i = kClassNum; i < (int) tokens.size(); i += 2) {
    gesture.push_back(Point(tokens[i], tokens[i + 1]));
  }

  return gesture;
}

void Gesture::normalize(int M) {
  double Tx = 0;
  double Ty = 0;
  for (int i = 0; i < (int)size(); ++i) {
    Tx += at(i).x;
    Ty += at(i).y;
  }
  Tx /= (int)size();
  Ty /= (int)size();

  double mx = 0;
  double my = 0;
  for (int i = 0; i < (int)size(); ++i) {
    at(i).x -= Tx;
    at(i).y -= Ty;
    if (at(i).x > 0 && mx < at(i).x) mx = at(i).x;
    if (at(i).x < 0 && mx < -at(i).x) mx = -at(i).x;
    if (at(i).y > 0 && my < at(i).y) my = at(i).y;
    if (at(i).y < 0 && my < -at(i).y) my = -at(i).y;
  }

  double m = mx;
  if (m < my) m = my;

  std::vector<Point> curPoints;
  for (int i = 0; i < (int)size(); ++i) {
    at(i).x /= m;
    at(i).y /= m;
    curPoints.push_back(at(i));
  }

  double D = length();
  clear();
  double curD = 0;
  int k = 0;
  for (int i = 1; i < (int)curPoints.size(); ++i) {
    if (k == M) break;
    double trazimD = k * D / (M - 1);
    while ((curD <= trazimD) &&
           (curD + Point::dist(curPoints[i - 1], curPoints[i]) >= trazimD)) {
      k++;
      Point in = interpolate(curD, trazimD, curPoints[i - 1], curPoints[i]);
      push_back(in);
      if (k == M) break;
      trazimD = k * D / (M - 1);
    }
    curD += Point::dist(curPoints[i - 1], curPoints[i]);
  }
  assert((int)size() == M);
}

string Gesture::toCsvString() {
  std::ostringstream oss;
  if (target_class() != -1) {
    fprintf(stderr, "Trying to print a gesture without a target class!");
    oss << kClassCode[target_class()];
  }
  for (int i = 0; i < (int)size(); ++i)
    oss << std::to_string(at(i).x) << "," << std::to_string(at(i).y) << ",";
  return oss.str();
}

Point Gesture::interpolate(double cur_dist, double target_dist, const Point& a,
                           const Point& b) {
  double diff = Point::dist(a, b);
  if (target_dist - cur_dist < cur_dist + diff - target_dist) {
    return a;
  } else
    return b;
}

double Gesture::length() {
  double curLen = 0;
  for (int i = 1; i < (int)size(); ++i) {
    curLen += Point::dist(at(i - 1), at(i));
  }
  return curLen;
}


