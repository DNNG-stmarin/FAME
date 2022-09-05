#include "myANN.h"
#include <cmath>

double myANN::Value(int index,double in0,double in1) {
   input0 = (in0 - 18.819)/8.89114;
   input1 = (in1 - 0.149219)/0.249001;
   switch(index) {
     case 0:
         return neuron0x7fed3ad57a20();
     default:
         return 0.;
   }
}

double myANN::Value(int index, double* input) {
   input0 = (input[0] - 18.819)/8.89114;
   input1 = (input[1] - 0.149219)/0.249001;
   switch(index) {
     case 0:
         return neuron0x7fed3ad57a20();
     default:
         return 0.;
   }
}

double myANN::neuron0x7fed3ad4e200() {
   return input0;
}

double myANN::neuron0x7fed3ad4f730() {
   return input1;
}

double myANN::input0x7fed3ad562e0() {
   double input = 1.85065;
   input += synapse0x7fed3ad548d0();
   input += synapse0x7fed3ad57ea0();
   return input;
}

double myANN::neuron0x7fed3ad562e0() {
   double input = input0x7fed3ad562e0();
   return ((input < -709. ? 0. : (1/(1+exp(-input)))) * 1)+0;
}

double myANN::input0x7fed3ad50ef0() {
   double input = 5.03805;
   input += synapse0x7fed3ad4f1b0();
   input += synapse0x7fed3ad51070();
   return input;
}

double myANN::neuron0x7fed3ad50ef0() {
   double input = input0x7fed3ad50ef0();
   return ((input < -709. ? 0. : (1/(1+exp(-input)))) * 1)+0;
}

double myANN::input0x7fed3ad4da50() {
   double input = 1.30945;
   input += synapse0x7fed3ad4dbd0();
   input += synapse0x7fed3ad56460();
   return input;
}

double myANN::neuron0x7fed3ad4da50() {
   double input = input0x7fed3ad4da50();
   return ((input < -709. ? 0. : (1/(1+exp(-input)))) * 1)+0;
}

double myANN::input0x7fed3ad505c0() {
   double input = 0.981456;
   input += synapse0x7fed3ad4f8b0();
   input += synapse0x7fed3ad4e380();
   return input;
}

double myANN::neuron0x7fed3ad505c0() {
   double input = input0x7fed3ad505c0();
   return ((input < -709. ? 0. : (1/(1+exp(-input)))) * 1)+0;
}

double myANN::input0x7fed3ad57070() {
   double input = -0.349604;
   input += synapse0x7fed3ad56f30();
   input += synapse0x7fed3ad57e20();
   return input;
}

double myANN::neuron0x7fed3ad57070() {
   double input = input0x7fed3ad57070();
   return ((input < -709. ? 0. : (1/(1+exp(-input)))) * 1)+0;
}

double myANN::input0x7fed3ad57a20() {
   double input = 2.65365;
   input += synapse0x7fed3ad4f650();
   input += synapse0x7fed3ad4d8f0();
   input += synapse0x7fed3ad571f0();
   return input;
}

double myANN::neuron0x7fed3ad57a20() {
   double input = input0x7fed3ad57a20();
   return ((input < -709. ? 0. : (1/(1+exp(-input)))) * 1)+0;
}

double myANN::synapse0x7fed3ad548d0() {
   return (neuron0x7fed3ad4e200()*-3.34825);
}

double myANN::synapse0x7fed3ad57ea0() {
   return (neuron0x7fed3ad4f730()*-0.936417);
}

double myANN::synapse0x7fed3ad4f1b0() {
   return (neuron0x7fed3ad4e200()*2.94433);
}

double myANN::synapse0x7fed3ad51070() {
   return (neuron0x7fed3ad4f730()*-0.593138);
}

double myANN::synapse0x7fed3ad4dbd0() {
   return (neuron0x7fed3ad562e0()*-0.325269);
}

double myANN::synapse0x7fed3ad56460() {
   return (neuron0x7fed3ad50ef0()*3.77968);
}

double myANN::synapse0x7fed3ad4f8b0() {
   return (neuron0x7fed3ad562e0()*-5.49409);
}

double myANN::synapse0x7fed3ad4e380() {
   return (neuron0x7fed3ad50ef0()*3.34664);
}

double myANN::synapse0x7fed3ad56f30() {
   return (neuron0x7fed3ad562e0()*5.40647);
}

double myANN::synapse0x7fed3ad57e20() {
   return (neuron0x7fed3ad50ef0()*-1.35947);
}

double myANN::synapse0x7fed3ad4f650() {
   return (neuron0x7fed3ad4da50()*-2.64259);
}

double myANN::synapse0x7fed3ad4d8f0() {
   return (neuron0x7fed3ad505c0()*-3.95849);
}

double myANN::synapse0x7fed3ad571f0() {
   return (neuron0x7fed3ad57070()*3.5231);
}
