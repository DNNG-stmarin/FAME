#include "readFiss.h"

#include <string>
#include <algorithm>
#include <sstream>

using namespace std;


void readFiss::GenerateAngles()
{
  ifstream fin(nameCoords); // detector coordinates
  cout << "reading coordinate file from " << nameCoords << endl;
  if(!fin.is_open())
  {
    w->noCoordsFile();
  }

  double* x = new double[NUM_DETECTORS];
  double* y = new double[NUM_DETECTORS];
  double* z = new double[NUM_DETECTORS];
  string line;

  for(int i = 0; i < NUM_DETECTORS; ++i)
  {
    getline(fin, line);
    // replace(line.begin(), line.end(), ',', ' ');
    // replace(line.begin(), line.end(), '{', ' ');
    // replace(line.begin(), line.end(), '}', ' ');
    stringstream in(line);
    in >> x[i] >> y[i] >> z[i];
    // **************
    // cout << x[i] << " " << y[i] << " " <<  z[i] << endl;
    // ******************
  }

  angles = new double*[NUM_DETECTORS];
  for(int i = 0; i < NUM_DETECTORS; ++i)
  {
    angles[i] = new double[NUM_DETECTORS];
    for(int j = 0; j < NUM_DETECTORS; ++j)
    {
      angles[i][j] = ((x[i] * x[j]) + (y[i] * y[j]) + (z[i] * z[j])) / (sqrt(pow(x[i], 2) + pow(y[i], 2) + pow(z[i], 2)) * sqrt(pow(x[j], 2) + pow(y[j], 2) + pow(z[j], 2)));
    }
  }
}
