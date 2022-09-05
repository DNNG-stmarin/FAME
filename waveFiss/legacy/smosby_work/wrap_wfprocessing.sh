BASEDIR="./data/"

# data5
for i in `seq 0 9`;
do
    temp=$BASEDIR/data5_00$i.root
    echo "Processing file "$temp
    bandaid.py $temp ./data/data5_00"$i"_pt.root
done
for i in `seq 10 99`;
do
    temp=$BASEDIR/data5_0$i.root
    echo "Processing file "$temp
    bandaid.py $temp ./data/data5_0"$i"_pt.root
done
for i in `seq 100 1085`;
do
    temp=$BASEDIR/data5_$i.root
    echo "Processing file "$temp
    bandaid.py $temp ./data/data5_"$i"_pt.root
done

# data7
for i in `seq 0 9`;
do
    temp=$BASEDIR/data7_00$i.root
    echo "Processing file "$temp
    bandaid.py $temp ./data/data7_00"$i"_pt.root
done
for i in `seq 10 99`;
do
    temp=$BASEDIR/data7_0$i.root
    echo "Processing file "$temp
    bandaid.py $temp ./data/data7_0"$i"_pt.root
done
for i in `seq 100 2983`;
do
    temp=$BASEDIR/data7_$i.root
    echo "Processing file "$temp
    bandaid.py $temp ./data/data7_"$i"_pt.root
done

# data9
for i in `seq 0 9`;
do
    temp=$BASEDIR/data9_00$i.root
    echo "Processing file "$temp
    bandaid.py $temp ./data/data9_00"$i"_pt.root
done
for i in `seq 10 99`;
do
    temp=$BASEDIR/data9_0$i.root
    echo "Processing file "$temp
    bandaid.py $temp ./data/data9_0"$i"_pt.root
done
for i in `seq 100 2639`;
do
    temp=$BASEDIR/data9_$i.root
    echo "Processing file "$temp
    bandaid.py $temp ./data/data9_"$i"_pt.root
done

# data12
for i in `seq 0 9`;
do
    temp=$BASEDIR/data12_00$i.root
    echo "Processing file "$temp
    bandaid.py $temp ./data/data12_00"$i"_pt.root
done
for i in `seq 10 99`;
do
    temp=$BASEDIR/data12_0$i.root
    echo "Processing file "$temp
    bandaid.py $temp ./data/data12_0"$i"_pt.root
done
for i in `seq 100 3986`;
do
    temp=$BASEDIR/data12_$i.root
    echo "Processing file "$temp
    bandaid.py $temp ./data/data12_"$i"_pt.root
done
