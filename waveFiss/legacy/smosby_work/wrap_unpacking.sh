BASEDIR="./data/"

# DO data5
for i in `seq 5177 5186`;
do
    temp=$BASEDIR/run0$i.mid
    echo "Processing file "$temp
    analyzer $temp ./rootfiles/run$i.root
done
