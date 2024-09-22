
FROM ubuntu:22.04

RUN apt-get update
ENV DEBIAN_FRONTEND noninteractive
RUN apt-get install -y build-essential cmake clang-15 software-properties-common
RUN add-apt-repository ppa:ubuntu-toolchain-r/test
RUN apt install -y python3-pip gcc-13 g++-13 pkg-config
RUN pip3 install conan
#RUN ll /usr/bin/gcc-13
#RUN ll /usr/bin/g++-13

RUN mkdir -p server
WORKDIR ./server

COPY ./ ./ 

RUN mkdir -p build

#RUN echo "tools.system.package_manager:mode=install" > /root/.conan2/global.conf
RUN conan install . --build=missing -of build -pr:a .conan/profiles/linux_clang
RUN conan build . --build=missing -of build -pr:a .conan/profiles/linux_clang

#ENTRYPOINT ["./start_server.sh"]
#CMD ["firefox", "\":99.0+0,0 10 1280 720 :99.0\"", "0"]
