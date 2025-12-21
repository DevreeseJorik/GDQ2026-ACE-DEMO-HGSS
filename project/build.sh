rm -rf build
mkdir build
cd build
cmake -DCMAKE_TOOLCHAIN_FILE=../toolchain.cmake ../
make

cd ..

python3 ../assets/image_converter.py -input_path=images_in -output_path=images_out

python3 packager.py 
./dump.sh packager/packed.bin > packager/packed_dump.txt
./dump.sh packager/unpacked.bin > packager/unpacked_dump.txt
