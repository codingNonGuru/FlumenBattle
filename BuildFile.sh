FLUMEN_CORE_PATH="/home/andrei/Documents/Code/Libraries/FlumenCore" 
FLUMEN_ENGINE_PATH="/home/andrei/Documents/Code/Libraries/FlumenEngine" 
GLM_PATH="/home/andrei/Documents/Code/Libraries/glm"

FLAGS="-fpermissive -w"

BUILD_RESULT=$(g++ *.cpp -o Main -g -I${FLUMEN_CORE_PATH} -I$GLM_PATH -I$FLUMEN_ENGINE_PATH $FLAGS)

echo $BUILD_RESULT