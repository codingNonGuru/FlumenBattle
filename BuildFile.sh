FLUMEN_CORE_PATH="/home/andrei/Documents/Code/Libraries/FlumenCore" 
FLUMEN_ENGINE_PATH="/home/andrei/Documents/Code/Libraries/FlumenEngine" 
FLUMEN_GAME_PATH="/home/andrei/Documents/Code/Projects/FlumenBattle" 
GLM_PATH="/home/andrei/Documents/Code/Libraries/glm"
GENERAL_PATH="/usr/include"

FLAGS="-std=gnu++11 -fpermissive -w -g"

FIND_RESULT=$(find ./FlumenBattle/ -iname "*.cpp")

echo $FIND_RESULT

#BUILD_RESULT=$(x86_64-w64-mingw32-g++ $FIND_RESULT -o Main -I${FLUMEN_CORE_PATH} -I$GLM_PATH -I$FLUMEN_ENGINE_PATH -I$FLUMEN_GAME_PATH -I$GENERAL_PATH $FLAGS)
BUILD_RESULT=$(x86_64-w64-mingw32-g++ $FIND_RESULT -o Main -I${FLUMEN_CORE_PATH} -I$GLM_PATH -I$FLUMEN_ENGINE_PATH -I$FLUMEN_GAME_PATH $FLAGS)

echo $BUILD_RESULT