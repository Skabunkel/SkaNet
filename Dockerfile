from ubuntu AS setup
#Ordering here matters since if we do an add before installing the compiler we install the compiler everytime.
WORKDIR build
RUN apt update && apt -y install build-essential clang
#Add files
ADD . .
#Compile the tcp example
RUN ls
RUN gcc -D_POSIX_C_SOURCE=200112L -c -std=c99 skanet.c -DPLATFORM_OS_LINUX -o skanet.o
RUN gcc -std=c99 skanet.o tcp_test.c -DPLATFORM_OS_LINUX -o test
CMD [ "./test" ]