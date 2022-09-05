BASEDIR="/mnt/hygd-data/7/fission_chamber/238U_12_data_saved1/"

# DO data5
for i in `seq 0 9`;
do
    temp=$BASEDIR/238U_12_data5_00$i.dat
    echo "Processing file "$temp
    unpack2root $temp ./data/data5_00$i.root
done
for i in `seq 10 99`;
do
    temp=$BASEDIR/238U_12_data5_0$i.dat
    echo "Processing file "$temp
    unpack2root $temp ./data/data5_0$i.root
done
for i in `seq 100 999`;
do
    temp=$BASEDIR/238U_12_data5_$i.dat
    echo "Processing file "$temp
    unpack2root $temp ./data/data5_$i.root
done
for i in `seq 1000 1085`;
do
    temp=$BASEDIR/238U_12_data5_$i.dat
    echo "Processing file "$temp
    unpack2root $temp ./data/data5_$i.root
done

# Do data7
for i in `seq 0 9`;
do
    temp=$BASEDIR/238U_12_data7_00$i.dat
    echo "Processing file "$temp
    unpack2root $temp ./data/data7_00$i.root
done
for i in `seq 10 99`;
do
    temp=$BASEDIR/238U_12_data7_0$i.dat
    echo "Processing file "$temp
    unpack2root $temp ./data/data7_0$i.root
done
for i in `seq 100 999`;
do
    temp=$BASEDIR/238U_12_data7_$i.dat
    echo "Processing file "$temp
    unpack2root $temp ./data/data7_$i.root
done
for i in `seq 1000 2983`;
do
    temp=$BASEDIR/238U_12_data7_$i.dat
    echo "Processing file "$temp
    unpack2root $temp ./data/data7_$i.root
done

# Do data9
for i in `seq 0 9`;
do
    temp=$BASEDIR/238U_12_data9_00$i.dat
    echo "Processing file "$temp
    unpack2root $temp ./data/data9_00$i.root
done
for i in `seq 10 99`;
do
    temp=$BASEDIR/238U_12_data9_0$i.dat
    echo "Processing file "$temp
    unpack2root $temp ./data/data9_0$i.root
done
for i in `seq 100 999`;
do
    temp=$BASEDIR/238U_12_data9_$i.dat
    echo "Processing file "$temp
    unpack2root $temp ./data/data9_$i.root
done
for i in `seq 1000 2639`;
do
    temp=$BASEDIR/238U_12_data9_$i.dat
    echo "Processing file "$temp
    unpack2root $temp ./data/data9_$i.root
done

# Do data12
for i in `seq 0 9`;
do
    temp=$BASEDIR/238U_12_data12_00$i.dat
    echo "Processing file "$temp
    unpack2root $temp ./data/data12_00$i.root
done
for i in `seq 10 99`;
do
    temp=$BASEDIR/238U_12_data12_0$i.dat
    echo "Processing file "$temp
    unpack2root $temp ./data/data12_0$i.root
done
for i in `seq 100 999`;
do
    temp=$BASEDIR/238U_12_data12_$i.dat
    echo "Processing file "$temp
    unpack2root $temp ./data/data12_$i.root
done
for i in `seq 1000 3986`;
do
    temp=$BASEDIR/238U_12_data12_$i.dat
    echo "Processing file "$temp
    unpack2root $temp ./data/data12_$i.root
done
