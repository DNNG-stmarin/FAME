#!/bin/bash

################################################################################
### analyze_runs.sh
### Devin S. Connolly
### dconnolly@lanl.gov
### brief documentation
###
### Detailed description
################################################################################

export ANALYSIS_DIR=/home/dconnolly/tke_analysis
export MID_DIR=$ANALYSIS_DIR/data
export ROOT_DIR=$ANALYSIS_DIR/rootfiles

cd $ANALYSIS_DIR

export runs=(`cat ${ANALYSIS_DIR}/scripts/random_runs.txt | awk '{print $1}'`)
export size=${#runs[@]}

if [[ $((size)) -lt $((`nproc --all`)) ]]; then
    export CORES=$((size))
else
    export CORES=$(nproc --all)
fi
printf "\nusing %d cores" $CORES
for run in "${runs[@]}"; do
    ((i=i%CORES)); ((i++==0)) && wait
    printf "\nunpacking run %d" $run
    printf "\n"
#    if [[ $((${run})) < 10000 ]]; then
        unpacker $MID_DIR/run0"$run".mid.gz $ROOT_DIR/run$run.root &
#    else
#        unpacker $MID_DIR/run"$run".mid.gz $ROOT_DIR/run$run.root &
#    fi
    sleep 5
done
for run in "${runs[@]}"; do
    ((i=i%CORES)); ((i++==0)) && wait
    printf "singles processing run %d" $run
    printf "\n"
    singlesprocessing $ROOT_DIR/run$run.root $ROOT_DIR/run${run}_sp.root $ANALYSIS_DIR/Np-237Params.cfg &
    sleep 5
done
for run in "${runs[@]}"; do
    ((i=i%CORES)); ((i++==0)) && wait
    printf "eventbuilding run %d" $run
    printf "\n"
    eventbuild $ROOT_DIR/run${run}_sp.root $ROOT_DIR/run${run}_evt.root $ANALYSIS_DIR/Np-237Params.cfg &
    sleep 5
done

# emacs
# Local Variables:
# mode: sh
# End:
