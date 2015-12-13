#include "neural_network.h"
#include "common.h"

#include <algorithm>
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
    while ((int)train_data_.back().size() > M_) {
      assert(train_data_.back().back().x > 1);
      train_data_.back().pop_back();
    }
    if (M_ != (int)train_data_.back().size()) {
      puts(line.c_str());
    }
    assert(M_ == (int)train_data_.back().size());
  }
  trainDataStream.close();
  N_ = (int)train_data_.size();
  printf("N: %d\n", N_);
  if (alg_type == 1) {
    Epoch e;
    for (int i = 0; i < N_; ++i) {
      e.push_back(i);
    }
    std::random_shuffle(e.begin(), e.end());
    epoches_.push_back(e);
  } else if (alg_type == 2) {
    for (int i = 0; i < N_; ++i) {
      Epoch e;
      e.push_back(i);
      epoches_.push_back(e);
    }
    std::random_shuffle(epoches_.begin(), epoches_.end());
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
      std::random_shuffle(e.begin(), e.end());
      if (e.size() == 0) break;
      epoches_.push_back(e);
    }
    std::random_shuffle(epoches_.begin(), epoches_.end());
    train_data_.pop_back();
  }

  for (int i = 0; i < (int)architecture.size(); ++i) {
    if (!i)
      nn_.push_back(Layer(architecture[i], architecture[i], INPUT));
    else {
      nn_.push_back(
          Layer(architecture[i], architecture[i - 1],
                (i + 1 == (int)architecture.size() ? OUTPUT : INTERN)));
    }
  }
  printf("Layers: %d\n", (int)architecture.size());
  printf("Init error: ");
  avg_squared_error();
}

void NeuralNetwork::train() {
  int it = -1;
  while (avg_squared_error() > eps) {
    it++;
    printf("Ep %d. ", it);
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
  for (int k = (int)nn_.size() - 1; k > 0; --k) {
    vector<vector<double> > newDelta;

    for (int s = 0; s < (int)epoch.size(); ++s) {
      vector<double> newD;
      TrainSample& ts = train_data_[epoch[s]];
      for (int j = 0; j < (int)nn_[k].size(); ++j) {
        // layer k, sample s, neuron j
        if (k == (int)nn_.size() - 1) {
          // last_layer
          double d = ts.getY(k, j) * (1 - ts.getY(k, j)) *
                     (ts.getT()[j] - ts.getY(k, j));
          newD.push_back(d);
        } else {
          double d = 0;
          for (int o = 0; o < delta[s].size(); ++o) {
            d += (delta[s][o] * nn_[k + 1].w(o)[j + 1]);
          }
          d *= ts.getY(k, j) * (1 - ts.getY(k, j));
          newD.push_back(d);
        }
      }
      newDelta.push_back(newD);
    }
    delta = newDelta;

    // for each neuron j
    for (int j = 0; j < (int)nn_[k].size(); ++j) {
      // for each weight i
      for (int i = 0; i < nn_[k][j].sizeW(); ++i) {
        double nabla = 0;
        for (int s = 0; s < (int)epoch.size(); ++s) {
          TrainSample& ts = train_data_[epoch[s]];
          if (!i) {
            nabla += delta[s][j];
          } else {
            nabla += delta[s][j] * ts.getY(k - 1, i - 1);
          }
        }
        nnNew[k][j].setW(i, nn_[k][j].w()[i] + kNi * nabla / (int)epoch.size());
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
  assert((int)x.size() == 2 * M_);
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
    vector<double> newX = l.eval_layer(x);
    x = newX;
  }

  assert((int)x.size() == kClassNum);
  return x;
}

vector<double> Layer::eval_layer(std::vector<double> x) const {
  vector<double> y;
  y.clear();
  for (int i = 0; i < (int)size(); ++i) {
    y.push_back(at(i).y(x));
  }
  return y;
}

