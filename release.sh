cc main.cc \
d_math.cc solver.cc parser.cc io.cc test.cc cmd_flags.cc -std=c++17 \
-fsanitize=address,alignment,bool,bounds,enum,float-cast-overflow,float-divide-by-zero,integer-divide-by-zero,nonnull-attribute,null,return,returns-nonnull-attribute,shift,signed-integer-overflow,undefined,unreachable,vla-bound,vptr \
 -o build/release.out 
 ./build/release.out $*