#!/bin/bash

################################################################################
### analyze_data.sh
### Devin S. Connolly
### dconnolly@lanl.gov
### script to run tke analysis when new midas files present
################################################################################

export ANALYSIS_DIR=/home/daq/tke_analysis
export MID_DIR=/mnt/arachne-data/1/spider/tke/data
export ROOT_DIR=/mnt/arachne-data/1/spider/tke/rootfiles

cd $ANALYSIS_DIR

export t_end=$(date --date="Nov 1 2020" +%F)
export t_now=$(date +%F)

export last_run=10000

while [[ $t_now < $t_end ]]; do
    printf "\n"
    date
    printf "\nchecking for new data\n"
    # get run numbers of last midas file transferred && last rootfile written
    export f0mid=$(ls -ltr $MID_DIR | grep .gz | awk 'END{print $9}')
    export f0root=$(ls -l $ROOT_DIR | grep .root | awk 'END{print $9}')

    # check if last midas file newer than last analyzed root file
    # if [[ ${f0mid:4:4} > ${f0root:3:4} ]]; then
    if (( $((${f0mid:3:5}+0)) > $last_run )); then
        # yes! -> analyze all runs betwee midas file run # && root file run #
        runs=( $(seq $(($last_run + 1)) 1 $((${f0mid:3:5}+0)) ) )
        if (( $((${runs[-1]} - ${runs[0]})) < $(nproc --all) )); then
            export CORES=$((${runs[-1]} - ${runs[0]} + 1))
        else
            export CORES=$(nproc --all)
        fi
        for run in "${runs[@]}"; do
            ((i=i%CORES)); ((i++==0)) && wait
            printf "\nunpacking run %d" $run
            printf "\n"
            if [[ $run < 10000 ]]; then
                unpacker $MID_DIR/run0"$run".mid.gz $ROOT_DIR/run$run.root &
            else
                unpacker $MID_DIR/run"$run".mid.gz $ROOT_DIR/run$run.root &
            fi
        done
        for run in "${runs[@]}"; do
            ((i=i%CORES)); ((i++==0)) && wait
            printf "singles processing run %d" $run
            printf "\n"
            singlesprocessing $ROOT_DIR/run$run.root $ROOT_DIR/run${run}_sp.root fitParams.cfg &
        done
        for run in "${runs[@]}"; do
            ((i=i%CORES)); ((i++==0)) && wait
            printf "eventbuilding run %d" $run
            printf "\n"
            eventbuild $ROOT_DIR/run${run}_sp.root $ROOT_DIR/run${run}_evt.root &
        done
        printf "\ndone!\n"
        export last_run=$((${f0mid:3:5}+0))
    else
        printf "\nno new files\n"
    fi
    sleep 1800
    export t_now=$(date +%F)
done

# emacs
# Local Variables:
# mode: sh
# End:
