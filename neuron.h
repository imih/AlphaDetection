#pragma once

#include <vector>

enum NeuronType { INPUT, OUTPUT, INTERN };

class Neuron {
 public:
  Neuron(NeuronType neuron_type, int layerNo, int inputsNo);

  double y(const std::vector<double>& x);

  static double randW();

 private:
  typedef std::vector<double> W;
  W w_;

  NeuronType neuron_type_;
  int layer_no_;
  int inputs_;
};

