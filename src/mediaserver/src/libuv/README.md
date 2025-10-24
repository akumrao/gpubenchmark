Version details of libuv

for i in *.c; do mv -- "$i" "${i%.c}.cpp"; done



Android getifaddrs fail issue:
for android sdk older than 23 libuv/src/unix/android-ifaddrs.cpp required otherwise remove it

better use android skd and ndk above 23 because getifaddrs is not implemented correctly below 23, it use ioctrl some complex way 

