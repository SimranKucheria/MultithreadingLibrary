#!/bin/sh
#Runs a bash script for all test codes
echo "Testing OTO\n"
for i in 1 2 3 4 5 6 7 8  10 11 12 13
do
    gcc test_code/MT/test$i.c -o test$i -lotothread
    if [ $i -eq 5 ]
        then 
            ./test$i 8 >> output.txt 
    elif [ $i -eq 9 ]
        then 
            echo "Enter a number: "
    elif [ $i -eq 11 ]
        then
            ./test$i 8 >> output.txt 
    else
        ./test$i >> output.txt 
    fi

    if [ $? -ne 139 ]
        then echo "Test$i passed"
    fi
done

echo "\nTesting MTO\n"
for i in 1 2 3 4 5 6 7 8  10 11 12 13
do
    gcc test_code/MT/test$i.c -o test$i -lmtothread
    if [ $i -eq 5 ]
        then 
            ./test$i 8 >> output.txt 
    elif [ $i -eq 9 ]
        then 
            echo "Enter a number: "
    elif [ $i -eq 11 ]
        then
            ./test$i 8 >> output.txt 
    else
        ./test$i >> output.txt 
    fi
    
    if [ $? -ne 139 ]
        then echo "Test$i passed"
    fi
    
done
echo "For test 9 run separately for scanf input"