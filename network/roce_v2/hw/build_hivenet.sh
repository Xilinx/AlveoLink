if [ $1 == 'def' ]
then
    cd HiveNet
    if [ ! -d "./build" ]
    then
        ./build.sh 0 0 1 3
    fi
    cd ..
else
    cd HiveNet
    if [ ! -d "./build" ]
    then
        ./build.sh 0 $1 1 $2
    fi
    cd ..
fi
