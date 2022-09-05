#!/bin/bash

################################################################################
### analyze_runs.sh
### Devin S. Connolly
### dconnolly@lanl.gov
### brief documentation
###
### Detailed description
################################################################################

print_usage(){
    printf "usage:\n"
    printf "analyze_runs.sh -[ehsu] -c <cfg filename(string)> -l <runlist(string)> -n <nproc(int)> -r <startrun(int)> <endrun(int)>\n"
    return 0
}

export ANALYSIS_DIR=$HOME/gitlab/lanl/tke_analysis
export MID_DIR=$ANALYSIS_DIR/data
export ROOT_DIR=$ANALYSIS_DIR/rootfiles

cd $ANALYSIS_DIR

unpack=false
singles=false
eventbuild=false

while getopts 'c:ehl:n:rsu' opt; do
    case ${opt} in
        c) CFG_FILE="${OPTARG}" ;;
        e) eventbuild=true ;;
        l)
            l_flag=true
            runseq=false
            list="${OPTARG}"
            ;;
        n) nproc=$(($OPTARG)) ;;
        r)
            runseq=true
            l_flag=false
            ;;
        s) singles=true ;;
        u) unpack=true ;;
        h|\?)
            print_usage
            exit 0
            ;;
    esac
done
shift $(($OPTIND-1))

[ "${1:-}" = "--" ] && shift

if [ "$runseq" = true ]; then
    printf "Processing runs $1 - $2\n"
    runs=( $(seq $1 1 $2) )
elif [ "$l_flag" = true ]; then
    printf "Processing runs from ${list}\n"
    export runs=(`cat ${ANALYSIS_DIR}/${list} | awk '{print $1}'`)
fi

printf "Running analysis stages:\n"
printf "==============================================\n"
printf "unpacker:          $unpack \n"
printf "singlesprocessing: $singles \n"
printf "eventbuild:        $eventbuild \n"
printf "==============================================\n"
printf "config file:       $CFG_FILE \n"

export size=${#runs[@]}

if (( $size < $nproc )); then
    export CORES=$size
else
    export CORES=$nproc
fi

printf "\nanalyzing %d runs\n" $$size
printf "using %d cores\n" $CORES
if [ "$unpack" = true ]; then
    for run in "${runs[@]}"; do
        ((i=i%CORES)); ((i++==0)) && wait
        printf "\nunpacking run %d" $run
        printf "\n"
        if (( ${run} < 10000 )); then
            unpacker $MID_DIR/run0"$run".mid.gz $ROOT_DIR/run$run.root &
        else
            unpacker $MID_DIR/run"$run".mid.gz $ROOT_DIR/run$run.root &
        fi
        sleep 2
    done
fi
wait
if [ "$singles" = true ]; then
    for run in "${runs[@]}"; do
        ((i=i%CORES)); ((i++==0)) && wait
        printf "\nsingles processing run %d" $run
        printf "\n"
        singlesprocessing $ROOT_DIR/run$run.root $ROOT_DIR/run${run}_sp.root $ANALYSIS_DIR/$CFG_FILE &
        sleep 2
    done
fi
wait
if [ "$eventbuild" = true ]; then
    for run in "${runs[@]}"; do
        ((i=i%CORES)); ((i++==0)) && wait
        printf "\neventbuilding run %d" $run
        printf "\n"
        eventbuild $ROOT_DIR/run${run}_sp.root $ROOT_DIR/run${run}_evt.root $ANALYSIS_DIR/$CFG_FILE &
        sleep 2
    done
fi
wait

printf "\ndone!\n"
exit 0

# emacs
# Local Variables:
# mode: sh
# End:
