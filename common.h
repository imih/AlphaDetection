#pragma once

#include <cmath>

#include <string>
#include <vector>
#include <sstream>
#include <iostream>

const int kClassNum = 5;
const std::vector<std::string> kClassNames =
    std::vector<std::string>({"alpha", "beta", "gamma", "delta", "epsilon"});
const std::vector<std::string> kClassCode = std::vector<std::string>(
    {"1,0,0,0,0", "0,1,0,0,0", "0,0,1,0,0", "0,0,0,1,0", "0,0,0,0,1"});

struct Point {
  double x;
  double y;
  Point(double x, double y) : x(x), y(y) {}

  static double dist(const Point& a, const Point& b) {
    return sqrt((a.x - b.x) * (a.x - b.x) + (a.y - b.y) * (a.y - b.y));
  }
};

class Gesture : public std::vector<Point> {
 public:
  Gesture() { target_class_ = -1; }

  static Gesture parse(const std::string& csv_format);

  void normalize(int M);
  std::string toCsvString();

  void setTargetClass(int target_class) { target_class_ = target_class; }
  int target_class() const { return target_class_; }

 private:
  Point interpolate(double cur_dist, double target_dist, const Point& a,
                    const Point& b);
  double length();

  int target_class_;
};

template <typename T>
std::vector<T> parseT(const std::string& ret_s, char delim) {
  std::vector<std::string> tokens;
  std::stringstream ss(ret_s);
  std::string item;
  while (std::getline(ss, item, delim)) {
    tokens.push_back(item);
  }
  std::vector<T> ret;
  for (std::string token : tokens) {
    std::stringstream st(token);
    T i;
    if ((st >> i).fail() || !(ss >> std::ws).eof()) {
      printf("Parsing of %s failed\n", ret_s.c_str());
      exit(1);
    }
    ret.push_back(i);
  }
  return ret;
}

template std::vector<int> parseT<int>(const std::string&, char);
template std::vector<double> parseT<double>(const std::string&, char);

