BASEDIR="./rootfiles"

# DO data5
#for i in `seq 498 515`;
#for i in `seq 4760 4957`;
#for i in `seq 4958 5036`;
for i in `seq 6660 6660`; 
do
    temp=$BASEDIR/run0$i.root
    echo "Processing file "$temp
    #MakeSinglesProcessing
    time singlesprocessing $temp ./rootfiles/run0"$i"_sp.root
done
