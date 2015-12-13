#include "common.h"
#include "neuron.h"

#include <cassert>
#include <cmath>

using std::vector;

double Neuron::randW() { return distribution_(generator_); }

Neuron::Neuron(NeuronType neuron_type, int inputsNo = 1)
    : neuron_type_(neuron_type),
      inputs_(inputsNo),
      distribution_(-kMaxWVal, kMaxWVal) {
  if (neuron_type != INPUT) {
    for (int j = 0; j < inputs_ + 1; ++j) {
      w_.push_back(randW());
    }
  }
}

namespace {
double sigmoid(double x) { return 1.0 / (1 + exp(-x)); }
}

double Neuron::y(const vector<double>& x) const {
  assert(x.size() == inputs_);
  if (neuron_type_ == INPUT) {
    assert((int)x.size() == 1);
    return x[0];
  } else {
    double ret = w_[0];
    for (int i = 0; i < x.size(); ++i) ret += x[i] * w_[i + 1];
    return sigmoid(ret);
  }
}

