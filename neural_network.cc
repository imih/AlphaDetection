#include "neural_network.h"
#include "common.h"

#include <cassert>
#include <fstream>

using std::string;
using std::vector;

NeuralNetwork::NeuralNetwork(int alg_type, vector<int> architecture,
                             string train_data_filename)
    : M_(architecture[0] / 2) {
  std::ifstream trainDataStream(train_data_filename);
  string line;
  while (std::getline(trainDataStream, line)) {
    train_data_.push_back(Gesture::parse(line));
  }
  assert(M_ == (int)train_data_.back().size());
  N_ = (int)train_data_.size();
  if (alg_type == 1) {
    Epoch e;
    for (int i = 0; i < N_; ++i) {
      e.push_back(i);
    }
    epoches_.push_back(e);
  } else if (alg_type == 2) {
    for (int i = 0; i < N_; ++i) {
      Epoch e;
      e.push_back(i);
      epoches_.push_back(e);
    }
  } else if (alg_type == 3) {
    vector<int> pos = {0, 0, 0, 0, 0};
    for (int i = N_ - 1; i >= 0; --i) {
      pos[train_data_[i].target_class()] = i;
    }

    train_data_.push_back(Gesture());
    while (true) {
      Epoch e;
      for (int i = 0; i < kClassNum; ++i) {
        int t = kBlockSizePerClass;
        while (t--) {
          if (train_data_[pos[i]].target_class() != i) {
            break;
          }
          e.push_back(pos[i]);
          pos[i]++;
        }
      }
      if (e.size() == 0) break;
      epoches_.push_back(e);
    }
    train_data_.pop_back();
  }

  for (int i = 0; i < (int)architecture.size(); ++i) {
    if (!i)
      nn_.push_back(Layer(architecture[i], 1, INPUT));
    else {
      nn_.push_back(
          Layer(architecture[i], architecture[i - 1],
                (i + 1 == (int)architecture.size() ? OUTPUT : INTERN)));
    }
  }
}

void NeuralNetwork::train() {
  int it = -1;
  while (avg_squared_error() > eps) {
    it++;
    printf("\tEp %d. \n", it);
    train(epoches_[it % (int) epoches_.size()]);
  }
}

void NeuralNetwork::train(const Epoch& epoch) {
  // TODO
}

double NeuralNetwork::avg_squared_error() {
  double ret = 0;
  for (const Gesture& g : train_data_) {
    vector<double> pred = predict2(g);
    for (int i = 0; i < kClassNum; ++i) {
      ret += ((g.target_class() == i) - pred[i]) *
             ((g.target_class() == i) - pred[i]);
    }
  }
  printf("%lf\n", ret / N_);
  return ret / N_;
}

string NeuralNetwork::predict(const Gesture& g) {
  vector<double> pred = predict2(g);
  int targetClassId = 0;
  for (int i = 0; i < kClassNum; ++i) {
    if (pred[i] > pred[targetClassId]) targetClassId = i;
  }
  return kClassCode[targetClassId];
}

vector<double> NeuralNetwork::predict2(const Gesture& g) {
  vector<double> x = g.serialize();
  for(Layer l : nn_) {
    x = l.eval_layer(x);
  }

  assert((int) x.size() == kClassNum);
  return x;
}

vector<double> Layer::eval_layer(std::vector<double> x) {
  vector<double> y;
  for(int i = 0; i < (int) size(); ++i) {
    y.push_back(at(i).y(x));
  }
  return y;
}

