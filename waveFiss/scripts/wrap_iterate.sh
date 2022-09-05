BASEDIR="./tke_rootfiles/"

# DO data5
#for i in `seq 1634 1855`;
for i in `seq 1634 1637`;
do
    temp=$BASEDIR/run"$i"_eloss.root
    echo "Processing file "$temp
    iterate $temp ./tke_rootfiles/run"$i"_iter_nua.root
done
