rm -r build
mkdir build
cd src
./build_one.sh clean
./build_one.sh $1 $2
./build_one.sh clean
./build_one.sh $3 $4
cd ..
