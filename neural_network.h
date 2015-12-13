#pragma once

#include "common.h"
#include "neuron.h"

#include <string>
#include <vector>

class Layer;
class TrainSample;
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
  std::vector<double> predict2(TrainSample& trainSample);
  std::vector<double> predict2(const Gesture& g);

  const int kBlockSizePerClass = 2;
  const double eps = 10e-6;

  int M_;
  int N_;
  std::vector<TrainSample> train_data_;

  std::vector<Epoch> epoches_;
  std::vector<Layer> nn_;
};

class Layer : public std::vector<Neuron> {
 public:
  Layer(int layer_size, int inputs, NeuronType neuron_type) {
    for(int i = 0; i < layer_size; ++i) {
      push_back(Neuron(i, neuron_type, inputs));
    }
  }

  std::vector<double> eval_layer(std::vector<double>) const;

  std::vector<double> w(int neuronNo) { return at(neuronNo).w(); }

  int layer_size() { return (int)size(); }
};

