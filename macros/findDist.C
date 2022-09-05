#include <fstream>
#include <sstream>

void findDist() {

  const double PI = 3.14159265;

  //open file
  ofstream cart;
  string filecart = "preConverted.txt";
  cart.open(filecart);

  ofstream out;
  string fileout = "cartesian.txt";
  out.open(fileout);

  string filein = "detectorCoordinates.txt";
  ifstream in (filein);
  if(!in.is_open()) {
    cout << "Warning: Could not find input file\n";
    exit(0);
  }

  int NUM_DETS = 54;
  int DETECTORS[] = {9, 10, 11, 12, 13, 14, 15, 16, 25,
                      26, 27, 28, 29, 30, 31, 32, 41, 42,
      	            	43, 44, 45, 46, 47, 48, 49, 50, 51,
      	            	52, 53, 54, 55, 56, 57, 58, 59, 60,
      	            	61, 62, 63, 64, 65, 66, 67, 68, 69,
      	            	70, 71, 72, 73, 74, 75, 76, 77, 78};



  double storage[NUM_DETS][3];
  int stored = 0;
  string line;
  string index;
  string value;
  int variable = 0;
  int i = 0;
  int lineIndex = 1;
  bool cont = 0;
  //istringstream iss;

  while(getline(in, line)) {
    if(line == "[/Analyzer/Parameters/Mapping/detector coords]") {
      cont = 1;
    }

    if(cont) {
      for(int det=0; det<NUM_DETS; det++) {
        istringstream iss(line);
        iss >> index >> value;
        if(index == ("["+to_string(DETECTORS[det])+"]")) {

          storage[i][variable] = stod(value);
          stored++;

          if(stored == NUM_DETS) {
            i = 0;
            stored = 0;
            variable++;
            continue;
          }

          i++;
        }
      }
    }
    lineIndex++;
  }

  for(int j = 0; j<NUM_DETS; j++) 
  {
    out << storage[j][0] << endl;
  }

}
