#include "common.h"
#include "window.h"
#include "neural_network.h"

#include <string>
#include <sstream>
#include <vector>

#include <cstdio>
#include <cstdlib>

#include <sys/stat.h>
#include <unistd.h>

using std::string;
using std::vector;

#define TEST 1

namespace {
const int kSamplesPerClass = 20;
const char kDelimArh = 'x';

const string kCommandLineArgs =
    "./nenr [-f file -m M] -a a -h (2*M)x...x5 -s samplesNo";

bool fileExists(const std::string& name) {
  return (access(name.c_str(), F_OK) != -1);
}
}

const int MAX_BUFFER = 1024;
int main(int argc, char** argv) {
  //  -f: file
  string fileName = "";
  //  -m: M
  int M = 0;
  //  -a: (algorithm_no) [1, 2, 3]
  int a = 0;
  //  -h: (arhitektura 2Mx....x5
  string hStr = "";
  // -s (samples for testing)
  int samplesNo = 0;
  int opt;
  while ((opt = getopt(argc, argv, "m:f:a:h:s:")) != -1) {
    switch (opt) {
      case 'f':
        fileName = optarg;
        break;
      case 'm':
        M = atoi(optarg);
        break;
      case 'a':
        a = atoi(optarg);
        if (a < 1 || a > 3) {
          puts("Valid values of a are 1, 2, 3!");
          exit(1);
        }
        break;
      case 'h':
        hStr = optarg;
        break;
      case 's':
        samplesNo = atoi(optarg);
        break;
    }
  }

  if (!a || hStr.empty()) {
    puts(kCommandLineArgs.c_str());
    exit(1);
  }

  if (!fileExists(fileName)) {
    // create file`
    if (!M) {
      puts(kCommandLineArgs.c_str());
      exit(1);
    }
    Window* w =
        new Window(M, kSamplesPerClass, true /* write to file*/, fileName);
    delete w;
  }

  vector<int> arh = parseT<int>(hStr, kDelimArh);
  if (arh[0] != 2 * M || arh.back() != 5) {
    printf("Architecture %s is not valid!\n", hStr.c_str());
    exit(1);
  }

  NeuralNetwork* nn = new NeuralNetwork(a, arh, fileName);
  /*
  nn->train();
  while (samplesNo--) {
    Window* w = new Window(nn->getM(), 1, false);
    string prediction = nn->predict(w->last_drawing());
    printf("Prediction: %s\n", prediction.c_str());
    delete w;
  }
  */
  delete nn;
  return 0;
}
