#pragma once

#include "common.h"

#include <string>
#include <vector>

class NeuralNetwork {
 public:
  NeuralNetwork(int alg_type, std::vector<int> architecture,
                std::string train_data_filename);

  void train();
  std::string predict(const Gesture& g);
  int getM() { return M_; }

 private:
  typedef std::vector<int> Epoch;
  void train(const Epoch& epoch);
  double avg_squared_error();
  std::vector<double> predict2(const Gesture& g);

  const int kBlockSizePerClass = 2;
  const double eps = 10e-6;

  std::vector<Epoch> epoches_;

  int M_;
  int N_;
  std::vector<Gesture> train_data_;

};
