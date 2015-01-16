#tracenum=16
#must_num=1
#must=18

mkdir -p /var/www/appeck/Beta
mkdir -p /var/www/appeck/Beta/automata
mkdir -p /var/www/appeck/Beta/upload
mkdir -p /var/www/appeck/Beta/parse
cp -a tester1_android /var/www/appeck/Beta/upload
cp code/construct.cpp /var/www/appeck/Beta/automata
cp code/graph.cpp /var/www/appeck/Beta/automata
cp code/testNu_v3.cpp /var/www/appeck/Beta/automata
g++ -std=c++11 -o /var/www/appeck/Beta/automata/construct /var/www/appeck/Beta/automata/construct.cpp

read -p "Please input the trace name: " tracenum 
cd /var/www/appeck/Beta/automata/
./construct ../upload/tester1_android/$tracenum/ dd
cd /var/www/appeck/
cp code/main.cpp /var/www/appeck/Beta/parse
cp code/tinystr.cpp /var/www/appeck/Beta/parse
cp code/tinyxml.cpp /var/www/appeck/Beta/parse
cp code/tinystr.h /var/www/appeck/Beta/parse
cp code/tinyxml.h /var/www/appeck/Beta/parse
cp code/tinyxmlparser.cpp /var/www/appeck/Beta/parse
cp code/tinyxmlerror.cpp /var/www/appeck/Beta/parse
cp -a /var/www/appeck/Beta/upload/tester1_android/$tracenum  /var/www/appeck/Beta/parse/
cd /var/www/appeck/Beta/parse/
g++ -c tinyxmlparser.cpp -DTIXML_USE_STL
g++ -c tinyxmlerror.cpp -DTIXML_USE_STL
g++ -c tinystr.cpp -DTIXML_USE_STL
g++ -c tinyxml.cpp -DTIXML_USE_STL                                          
g++ -c main.cpp -DTIXML_USE_STL
g++ -o g++ -o main main.o tinyxmlparser.o tinyxmlerror.o tinystr.o tinyxml.o
./main $tracenum txt_dir
./main $tracenum txt_dir
cd /var/www/appeck/Beta/automata/
cp /var/www/appeck/Beta/parse/test /var/www/appeck/Beta/automata/

g++ -std=c++11 -o testNu testNu_v3.cpp
./testNu /var/www/appeck/Beta/automata/tester1_android/dd test test.svm
g++ -std=c++11 -o /var/www/appeck/Beta/automata/graph /var/www/appeck/Beta/automata/graph.cpp
rm limit.txt 
read -p "Enter a must-have num : " must_num
echo $must_num >> limit.txt
read -p "Enter a must-have : " must
echo $must >> limit.txt
read -p "Enter a must-not-have num : " must_n_num
echo $must_n_num >> limit.txt
read -p "Enter a must-not-have : " must_n
echo $must_n >> limit.txt

./graph test.svm limit.txt
read -p "Please input the loop name: " loop
read -p "Please input the num of times: " numtime
./graph test.svm limit.txt $loop $numtime
cp /var/www/appeck/Beta/automata/tester1_android/dd/state /var/www/appeck/Beta/upload/tester1_android/$tracenum
cp /var/www/appeck/Beta/automata/loop /var/www/appeck/Beta/upload/tester1_android/$tracenum
cp /var/www/appeck/code/adb_com.cpp /var/www/appeck/Beta/upload/tester1_android/$tracenum
cd /var/www/appeck/Beta/upload/tester1_android/$tracenum
g++ -std=c++11 -o adb_com adb_com.cpp
./adb_com loop $tracenum
