echo "We've been dancin' with Mr. Brownstone"
cp /Users/giha/Documents/buildWave/CONVERT CONVERT

echo "Beginning CONVERT"
# ./CONVERT wave.in > outConvert.txt

# cp /Users/giha/Documents/root/waves/bin/singlesprocessing singlesprocessing

for ((file=0; file <= $1; file++));
do
	echo "file number $file" 
	echo "Beginning singlesprocessing"
	singlesprocessing danaFile_${file}.root outFile_${file}.root # > outSingles.txt

	# cp /Users/giha/Documents/root/waveFiss/bin/eventbuild eventbuild
	echo "Beginning eventbuild"
	eventbuild outFile_${file}.root event_${file}.root # > outEvent.txt
	
	rm danaFile_${file}.root
done
