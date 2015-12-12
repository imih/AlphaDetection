#include "neuron.h"

#include <cassert>
#include <cmath>

using std::vector;

Neuron::Neuron(NeuronType neuron_type, int layerNo, int inputsNo = 1)
    : neuron_type_(neuron_type), layer_no_(layerNo), inputs_(inputsNo) {
  for (int j = 0; j < inputs_ + 1; ++j) {
    w_.push_back(Neuron::randW());
  }
}

namespace {
double sigmoid(double x) { return 1.0 / (1 + exp(-x)); }
}

double Neuron::y(const vector<double>& x) {
  assert(x.size() == inputs_);
  if (neuron_type_ == INPUT) {
    assert((int)x.size() == 1);
    return x[0];
  } else {
    double ret = w_[0];
    for (int i = 0; i < x.size(); ++i) ret += x[i] * w_[i + 1];
    // TODO(not sure)
    return neuron_type_ == INTERN ? sigmoid(ret) : ret;
  }
}

double Neuron::randW() { /* TODO */
  return 0;
}

