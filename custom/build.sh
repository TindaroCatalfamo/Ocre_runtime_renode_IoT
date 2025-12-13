if [ -z "$1" ]; then
    echo "Errore: Devi specificare il file C da compilare."
    echo "Esempio: ./build.sh hello_world.c"
    exit 1
fi


WAMR_DIR=${PWD}/../../..

SRC_FILE=$1                  
BASE_NAME=${SRC_FILE%.c}     

echo "Build wasm app from ${SRC_FILE} ..."
/home/tindaro/wasi-sdk-20.0/bin/clang -O3 \
        -z stack-size=4096 -Wl,--initial-memory=65536 \
        -o ${BASE_NAME}.wasm ${SRC_FILE} \
        -Wl,--export=main -Wl,--export=__main_argc_argv \
        -Wl,--export=__data_end -Wl,--export=__heap_base \
        -Wl,--strip-all,--no-entry \
        -Wl,--allow-undefined \
        -nostdlib \

echo "Build binarydump tool .."
rm -fr build && mkdir build && cd build
cmake ../../../../test-tools/binarydump-tool
make
cd ..

echo "Generate Standardized Header for Zephyr ..."
./build/binarydump -o test_wasm.h -n wasm_test_file ${BASE_NAME}.wasm

echo "Moving to Zephyr source folder..."
mv test_wasm.h ${WAMR_DIR}/product-mini/platforms/zephyr/simple/src/test_wasm.h
echo "Done, File moved."

echo "Starting the WASM module on Zephyr with native_sim ..."

cd "${WAMR_DIR}/product-mini/platforms/zephyr/simple" || exit 1
rm -rf build
west build -b native_sim -t run
