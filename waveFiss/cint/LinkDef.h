#ifdef __MAKECINT__
// turns off dictionary generation for all
#pragma link off all class;
#pragma link off all function;
// #pragma link off all global;
// #pragma link off all typedef;

#pragma link C++ defined_in ../include/WaveformProcessing.h;

#pragma link C++ function TKESettings::reset();
#pragma link C++ function CAEN_DGTZ_Event::reset();
#pragma link C++ function CAEN_DGTZ_Event::process_wf(TKESettings, double*, double*, double*);
#pragma link C++ function ProcessedRootEvent::reset();
#pragma link C++ function EventInfo::reset();

#pragma link C++ class TKESettings+;
#pragma link C++ class CAEN_DGTZ_Event+;
#pragma link C++ class ProcessedRootEvent+;
#pragma link C++ class EventInfo+;

#endif
