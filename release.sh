rm ./build/qdr
cp -r ./res build/res
cc src/*.cc -std=c++17 -I./include -I./external/raygui/src $(pkg-config --libs --cflags raylib) \
-fsanitize=address,alignment,bool,bounds,enum,float-cast-overflow,float-divide-by-zero,integer-divide-by-zero,nonnull-attribute,null,return,returns-nonnull-attribute,shift,signed-integer-overflow,undefined,unreachable,vla-bound,vptr \
 -o build/qdr 
 ./build/qdr $*