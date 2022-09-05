BASEDIR="./tke_rootfiles/"

# DO data5
#for i in `seq 1904 1911`;
#for i in `seq 1912 1915`;
#for i in `seq 1916 2009`;
#for i in `seq 2010 2023`;
for i in `seq 2024 2763`;
do
    temp=$BASEDIR/run"$i"_spg.root
    echo "Processing file "$temp
    eventbuildgammas $temp ./tke_rootfiles/run"$i"_evtg.root
done
