# Version: 0.0.1
FROM ubuntu:19.04

ENV REFRESHED_AT 2018-12-14
ENV container docker

RUN apt-get -y update; apt-get clean all
RUN apt-get -y install cmake
RUN apt-get -y install default-jdk
RUN apt-get -y install yui-compressor 2.4.7

# symlink so the Doxygen Make file can find the yuicompressor jar
RUN ln -s /usr/share/java/yuicompressor.jar /usr/local/bin/yuicompressor-2.4.7.jar

# need ruby-full for compass
RUN apt-get -y install ruby-full
RUN gem install -n /usr/local/bin compass

RUN apt-get -y install g++
RUN apt-get -y install flex
RUN apt-get -y install bison

VOLUME /doxygen
VOLUME /source_to_document

CMD ["/doxygen/docker/build.sh"]
