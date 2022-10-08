source .env

FLAGS='-O3 -fPIC -shared'
WARNINGS='-Winline -Wno-invalid-noreturn'
COMPILER=gcc-12

OUTPUT_LOC="$OUTPUT_DIR/graphrox-x86.dylib"
BUILD_SPECIFIC_FILES=''

mkdir -p $OUTPUT_DIR

$COMPILER $WARNINGS -fvisibility=hidden $FLAGS -I$INCLUDE_DIR $FILES $BUILD_SPECIFIC_FILES -o $OUTPUT_LOC
