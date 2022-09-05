# ROOT \
_Author_: Stefano Marin, Isabel Hernandez, DNNG: Fission Division .
based on: CoMPASS reader, Jason Nattress ORNL.

_Purpose_: This code performs all the analysis required for a fission experiment acquired using CoMPASS or MIDAS, including the system characterization, error analyisis, and data cuts.

_Date_: Ann Arbor, May 4th, 2020.

Content:

How to compile:
$ mkdir build
$ cd build
$ cmake ../
$ make

How to run
./ROOT_FAME inputFile dataFile

(data file should not include the file number or the .root extension, so if we are trying to compute:
data1.root
data2.roo
...

use "data", the rest will be autocompleted)

(the code is a prototype, reach out to stmarin@umich.edu to actually use it)
