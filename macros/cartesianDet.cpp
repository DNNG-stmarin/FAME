#include <fstream>
#include <sstream>

void cartesianDet() {

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

  // string currentDet;
  // string line;
  // int variable;
  // int equation[4];
  // string number1, number2;
  // int pos;
  // int lineIndex;
  // for(int i=0; i<NUM_DETS; i++) {
  //   currentDet = "[" + to_string(DETECTORS[i]) + "]";
  //   cout << currentDet;
  //
  //   //string line;
  //   variable = 0;
  //   //int equation[4];
  //   lineIndex = 1;
  //   while(getline(in, line)) {
  //
  //     pos = line.find(currentDet, 0);
  //     while(pos!=std::string::npos) {
  //       istringstream iss(line);
  //       //string number1, number2;
  //       iss >> number1 >> number2;
  //       cout << lineIndex << ": " << number1 << " " << number2 << endl;
  //
  //
  //       equation[variable] = stoi(number2);
  //       variable++;
  //       pos = line.find(currentDet, pos+1);
  //     }
  //     lineIndex++;
  //
  //   }
  //
  //   double x = equation[1]*sin(equation[3])*cos(equation[2]);
  //   double y = equation[1]*sin(equation[3])*sin(equation[2]);
  //   double z = equation[1]*cos(equation[3]);
  //
  //   //cout << x << " " << y << " " << z << endl;
  // }

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
          cout << lineIndex << " " << index << " " << value << endl;
          cout << "   " << i << " " << variable << endl;
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

  for(int j=0; j<NUM_DETS; j++) {
    //out << DETECTORS[j] << " ";
    for(int k=0; k<3; k++) {
      cart << storage[j][k] << " ";
    }
    cart << "\n";
  }

  for(int j=0; j<NUM_DETS; j++) {
    out << storage[j][0]*sin(storage[j][1]*PI/180)*cos(storage[j][2]*PI/180) << " ";
    out << storage[j][0]*sin(storage[j][1]*PI/180)*sin(storage[j][2]*PI/180) << " ";
    out << storage[j][0]*cos(storage[j][1]*PI/180) << "\n";
  }

  string testline;
  string x, y, z;
  ifstream testin ("cartesian.txt");
  // for(int i=0; i<NUM_DETS; i++) {
  //   getline(testin, testline);
  //   istringstream isstest(testline);
  //   isstest >> x >> y >> z;
  //   cout << stod(x) << " " << stod(y) << " " << stod(x) << "\n";
  // }

  out.close();
  cart.close();

  getline(testin, testline);
  istringstream isstest(testline);
  isstest >> x >> y >> z;
  cout << stod(x) << " " << stod(y) << " " << stod(z) << "\n";
  cout << testline << "\n";

  bool test = 0;
  !test;
  cout << test;

}
