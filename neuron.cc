#include "common.h"
#include "neuron.h"

#include <cassert>
#include <cmath>

using std::vector;

double Neuron::randW() { return distribution_(generator_); }

Neuron::Neuron(int neuron_id, NeuronType neuron_type, int inputsNo)
    : neuron_id_(neuron_id),
      neuron_type_(neuron_type),
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
  if(x.size() != inputs_) {
    printf("%d %d\n", (int) x.size(), inputs_);
  }
  assert((int) x.size() == inputs_);
  if (neuron_type_ == INPUT) {
    return x[neuron_id_];
  } else {
    double ret = w_[0];
    for (int i = 0; i < x.size(); ++i) ret += x[i] * w_[i + 1];
    return sigmoid(ret);
  }
}

