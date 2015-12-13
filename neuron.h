#pragma once

#include <random>
#include <vector>

enum NeuronType { INPUT, OUTPUT, INTERN };

class Neuron {
 public:
  Neuron(NeuronType neuron_type, int inputsNo);

  double y(const std::vector<double>& x);

 private:
  double randW();

  typedef std::vector<double> W;
  W w_;

  NeuronType neuron_type_;
  int inputs_;

  std::default_random_engine generator_;
  std::uniform_real_distribution<double> distribution_;
};

