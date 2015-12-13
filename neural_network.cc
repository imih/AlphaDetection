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
    train_data_.push_back(TrainSample::parse(line));
  }
  assert(M_ == train_data_.back().size());
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

    train_data_.push_back(TrainSample());
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
    train(epoches_[it % (int)epoches_.size()]);
  }
}

// Epoch: vector<int>
void NeuralNetwork::train(const Epoch& epoch) {
  vector<vector<double> > t;  // t[i][j] -> ith example, j-th target
  for (int i = 0; i < (int)epoch.size(); ++i) {
    t.push_back(train_data_[epoch[i]].getT());
    predict2(train_data_[epoch[i]]);
  }

  vector<vector<double> > delta;
  vector<Layer> nnNew = nn_;
  for (int i = (int)nn_.size(); i > 0; --i) {
    vector<vector<double> > newDelta;
    for (int j = 0; j < (int)epoch.size(); ++j) {
      vector<double> newD;
      TrainSample& ts = train_data_[epoch[j]];
      for (int k = 0; k < (int)nn_[i].size(); ++k) {
        // layer i, sample j, s, neuron k
        if (i == (int)epoch.size() - 1) {
          double d =
              ts.getY(i, k) * (1 - ts.getY(i, k)) * (ts.getT()[k] - ts.getY(i, k));
          newD.push_back(d);
        } else {
          double d = 0;
          for (int o = 0; o < delta[j].size(); ++o) {
            // TODO not sure!
            assert((int)delta[j].size() == (int)nn_[i].w(k).size());
            d += (delta[j][o] * nn_[i].w(k)[o]);
          }
          d *= ts.getY(i, k) * (1 - ts.getY(i, k));
          newD.push_back(d);
        }
      }
      newDelta.push_back(newD);
    }
    delta = newDelta;

    for (int k = 0; k < (int)nn_[i].size(); ++k) {
      for (int j = 0; j <  nn_[i][k].sizeW(); ++j) {
        double nabla = 0;
        for (int s = 0; s < (int)epoch.size(); ++i) {
          TrainSample& ts = train_data_[epoch[s]];
          nabla += delta[s][j] * ts.getY(s, k);
        }
        nnNew[i][k].setW(j, nn_[i][k].w()[j] + kNi * nabla / (int)epoch.size());
      }
    }
  }
  nn_ = nnNew;
}

double NeuralNetwork::avg_squared_error() {
  double ret = 0;
  for (const auto& g : train_data_) {
    vector<double> pred = predict2(g);
    for (int i = 0; i < kClassNum; ++i) {
      ret += ((g.target_class() == i) - pred[i]) *
             ((g.target_class() == i) - pred[i]);
    }
  }
  printf("%lf\n", ret / (2 * N_));
  return ret / (2 * N_);
}

string NeuralNetwork::predict(const Gesture& g) {
  vector<double> pred = predict2(g);
  int targetClassId = 0;
  for (int i = 0; i < kClassNum; ++i) {
    if (pred[i] > pred[targetClassId]) targetClassId = i;
  }
  return kClassCode[targetClassId];
}

std::vector<double> NeuralNetwork::predict2(TrainSample& trainSample) {
  vector<double> x = trainSample.serialize();
  trainSample.clearY();
  for (const Layer& l : nn_) {
    x = l.eval_layer(x);
    trainSample.addY(x);
  }

  assert((int)x.size() == kClassNum);
  return x;
}

vector<double> NeuralNetwork::predict2(const Gesture& g) {
  vector<double> x = g.serialize();
  for (const Layer& l : nn_) {
    x = l.eval_layer(x);
  }

  assert((int)x.size() == kClassNum);
  return x;
}

vector<double> Layer::eval_layer(std::vector<double> x) const {
  vector<double> y;
  for (int i = 0; i < (int)size(); ++i) {
    y.push_back(at(i).y(x));
  }
  return y;
}

