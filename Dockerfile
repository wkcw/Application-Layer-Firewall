FROM ubuntu:14.04

COPY ./sources.list /etc/apt/sources.list
RUN apt-get update
RUN apt-get -y install gcc build-essential g++ cmake
RUN apt-get -y install autoconf libtool
RUN apt-get -y install pkg-config nginx
ADD ./env /dep
RUN cd /dep && bash ./install_dep.sh
ENV LD_LIBRARY_PATH /usr/local/lib
RUN apt-get -y install iptables

CMD service nginx start && cd /project/target && cmake ../src && bash
