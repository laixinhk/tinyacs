FROM alpine:3.19 as build

#############  set alpine apk repositories mirror server  ##########
#RUN echo "https://mirrors.tuna.tsinghua.edu.cn/alpine/v3.19/main" > /etc/apk/repositories ; 
#RUN echo "https://mirrors.tuna.tsinghua.edu.cn/alpine/v3.19/community" >> /etc/apk/repositories ;

RUN apk add gcc g++ libtool binutils 
RUN apk add make automake autoconf pkgconfig musl-dev flex bison m4 perl
RUN apk add openssl-dev zlib-dev
RUN apk add unzip patch
RUN apk add openssl3 libcrypto3

#############  download gsoap_2.8.*.zip from https://sourceforge.net/projects/gsoap2/  ##########
COPY ./acs /usr/src/acs
COPY ./LICENSE /usr/src/acs/

WORKDIR /usr/src/acs/gsoap

RUN unzip gsoap_2.8.* -d /usr/src/acs/gsoap

WORKDIR /usr/src/acs/gsoap/gsoap-2.8

#############  remove the libc fuctions if not support  ##########
RUN sed -i 's/strtod_l//g' configure.ac

RUN sed -i 's/strtof_l//g' configure.ac

RUN sed -i 's/strerror_r//g' configure.ac

RUN sed -i 's/gethostbyname_r//g' configure.ac

RUN ./configure --enable-ipv6

RUN make && make install 


############# build tinyacs #####################
WORKDIR /usr/src/acs/

ARG TINYACS_VERSION
ARG ACS_SERVER_DOMAIN_NAME

RUN make all TINYACS_VERSION=$TINYACS_VERSION ACS_SERVER_DOMAIN_NAME=$ACS_SERVER_DOMAIN_NAME


############# tinyacs in image #####################
FROM alpine:3.19 as main

COPY --from=build /usr/src/acs/tinyacs /usr/bin/tinyacs
COPY --from=build /lib/libcrypto.so.3 /lib/libcrypto.so.3
COPY --from=build /lib/libssl.so.3 /lib/libssl.so.3
COPY --from=build /usr/src/acs/ssl/gen/ssl_server/ /root/ssl_server/
RUN cat /root/ssl_server/server_private_key.pem /root/ssl_server/server_ca.pem > /root/ssl_server/KeyAndCA.pem
COPY --from=build /usr/src/acs/ssl/gen/ssl_client/ /root/ssl_client/
COPY --from=build /usr/src/acs/ssl/gen/ssl_server2client/ /root/ssl_client/

WORKDIR /root
EXPOSE 10240


ENTRYPOINT ["/usr/bin/tinyacs"]

