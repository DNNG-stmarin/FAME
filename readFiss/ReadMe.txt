
If the Qt parts don't work, you need to add an environment variable Qt5_DIR in .bash_profile,
that is set to the location of Qt5Config.cmake

Here's an example:

##### Qt path
export Qt5_DIR
Qt5_DIR="$Qt5_DIR:/home/jgiha/Qt/5.15.2/gcc_64/lib/cmake/Qt5"
