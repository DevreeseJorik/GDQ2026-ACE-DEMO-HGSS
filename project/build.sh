set -e

rm -rf build
mkdir build
cd build
cmake -DCMAKE_TOOLCHAIN_FILE=../tools/toolchain.cmake ../
make

cd ..

image_converter.py -i=assets -o=/home/project/output/images/bins

mkdir -p /home/project/output/packages
packager.py -i=/home/project/output/payloads -o=/home/project/output/packages/bins/

qhexdump /home/project/output/packages/bins/packed.bin > /home/project/output/packages/bins/packed_dump.txt
qhexdump /home/project/output/packages/bins/unpacked.bin > /home/project/output/packages/bins/unpacked_dump.txt
