
#define NSAMPLES_H_ANODE 4915
#define NSAMPLES_H_CATHODE 11996
// raws
namespace raw {
    // a Hambsch ionization chamber
    class hambsch_ic
    {
        public:
            float t0[NSAMPLED_H_CATHODE];
            float c[NSAMPLED_H_CATHODE];
            float a[2][NSAMPLED_H_CATHODE];
            float g[2][NSAMPLED_H_CATHODE];
    };
}

// variables
namespace var {
    // a gridded 2 sided ionization chamber
    class ic
    {
        public:
            int decimation; // how much to decimate the waveform by
    }
}

// cals
namespace cal {
}
