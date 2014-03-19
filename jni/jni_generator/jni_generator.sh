

#JAVA_FILE=$1

JAVA_FILE=../../src/com/example/fileobserver/uninstall/NativeFileObserver.java

JAVA_NEW_FILE=`basename $JAVA_FILE`

echo $CPP_FILE

sed  -e '/\*/d' -e '/\/\//d' -e '/^\s*$/d' $JAVA_FILE > $JAVA_NEW_FILE

#gcc -E -P $1 > $1.backup

./jni_generator.py --input_file=$JAVA_NEW_FILE --output_dir=../jni/

rm $JAVA_NEW_FILE
