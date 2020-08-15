clear
cmake -B build && cmake --build build   \
&& cd shaders/                          \
&& ./compile.sh                         \
&& cd -                                 \
&& __NV_PRIME_RENDER_OFFLOAD=1 __GLX_VENDOR_LIBRARY_NAME=nvidia \
./app \
# -vsync

# tracking size of binary file
ls -l app >> stat.txt