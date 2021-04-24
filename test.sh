#!/bin/sh
echo "Testing OTO"
for i in 1 2 3 4 6 7 8 10 12 13
do
gcc test_code/MT/test$i.c -o test$i -lotothread
./test$i >> output.txt 
if [ $? -ne 139 ]; 
then echo "Test$i passed";
fi
done
for i in 5 11
do
gcc test_code/MT/test$i.c -o test$i -lotothread
./test$i 8 >> output.txt 
if [ $? -ne 139 ]; 
then echo "Test$i passed";
fi
done
echo "Enter a number";
gcc test_code/MT/test9.c -o test9 -lotothread
./test9 >> output.txt 
if [ $? -ne 139 ]; 
then echo "Test9 passed";
fi

echo "Testing MTO"
for i in 1 2 3 4 6 7 8 10 12 13
do
gcc test_code/MT/test$i.c -o test$i -lmtothread
./test$i >> output.txt 
if [ $? -ne 139 ]; 
then echo "Test$i passed";
fi
done
for i in 5 11
do
gcc test_code/MT/test$i.c -o test$i -lotothread
./test$i 8 >> output.txt 
if [ $? -ne 139 ]; 
then echo "Test$i passed";
fi
done
echo "Enter a number here";
gcc test_code/MT/test9.c -o test9 -lotothread
./test9 >> output.txt 
if [ $? -ne 139 ]; 
then echo "Test9 passed";
fi