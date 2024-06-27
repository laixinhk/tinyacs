GSOAP_TOOLS_INSTALL_PREFIX:=$(shell pwd)/gsoap/output
export PATH := $(PATH):${GSOAP_TOOLS_INSTALL_PREFIX}/bin

ACS_SERVER_DOMAIN_NAME:=my.tinyacs.acs

.PHONY = all build_tools_check hosttools_for_gsoap gsoap_lib_source release \
         all gdb httpssupport clean info purge  

BUILD_DATE:=$(shell date)
TINYACS_VERSION:="v1.7.0"


all: build_tools_check hosttools_for_gsoap gsoap_lib_source httpssupport
	make tinyacs


################## prepare build env ###################################
build_tools_check:
	@which autoheader > /dev/null 2>&1 || ( echo "'autoconf' is missing on your system." && exit 1 )
	@test -f /usr/include/openssl/bio.h || ( echo "'libssl-dev' is missing on your system." && exit 1 )
	@test -f /usr/include/zlib.h || ( echo "'zlib1g-dev' is missing on your system." && exit 1 )
	@which yacc > /dev/null 2>&1 || ( echo "'bison' is missing on your system." && exit 1 )
	@which flex > /dev/null 2>&1 || ( echo "'flex' is missing on your system." && exit 1 )

hosttools_for_gsoap:
	@which wsdl2h > /dev/null 2>&1 || ( echo "'wsdl2h' is missing on your system." && make -C gsoap INSTALL_PREFIX=output)
	@which soapcpp2 > /dev/null 2>&1 || ( echo "'soapcpp2' is missing on your system." && make -C gsoap INSTALL_PREFIX=output)

gsoap_lib_source: hosttools_for_gsoap
	@test -f ./gsoap/libgsoap/config.h || ( make -C gsoap gsoap_lib_source )

################### generate sourcecode ################################
# wsdl2h -h  //help
# -c //gen C source code
# -c++    generate C++ source code (default)
# -c++11 //gen C++ 11 source code
# -tfile  use type map file instead of the default file typemap.dat
# -x      don't generate _XML any/anyAttribute extensibility elements
# -g      generate global top-level element and attribute declarations
# -F      add transient members to structs to simulate struct-type derivation in C //for ChangeDUState
# -Nname  use name for service prefixes to produce a service for each binding
# example: wsdl2h -t typemap.dat -c -x -o tr-069.wsdl https://www.broadband-forum.org/cwmp/cwmp-1-4.xsd \
#  https://www.broadband-forum.org/cwmp/cwmp-datamodel-1-1.xsd
wsdl2h_output/tr-069.wsdl:
	mkdir -p wsdl2h_output && \
	rm -rf ./cwmp_rpc_schema/cwmp.xsd && \
	cat ./cwmp_rpc_schema/cwmp-1-[234].xsd | grep -v "xs:include" > ./cwmp_rpc_schema/cwmp.xsd && \
	wsdl2h -t ./typemap.dat -c -x -g -Ncwmp -F -o ./wsdl2h_output/tr-069.wsdl ./cwmp_rpc_schema/cwmp.xsd

stdsoap2files := $(wildcard ./gsoap/libgsoap/*.c)
stdsoap2files += $(wildcard ./gsoap/libgsoap/plugin/*.c)

objects := $(stdsoap2files:%.c=%.o)


# cwmp = CPE WAN Management Protocol
# CPE  = customer-premises equipment
# WAN  = wide area network
# cwmpfiles include:
# cwmpStub.h a modified and annotated header file produced from the input interface header file, this file is compilable by C/C++ compilers while the input interface header file is not.
# cwmpH.h the main header file to be included by the application source code, this file also includes cwmpStub.h.
# cwmpC.c the serializers for the C/C++ types specified in the interface header file.
# cwmpServer.c the server-side skeleton functions to dispatch service requests to user-define service functions.
# cwmp.nsmap the XML namespace mapping table, generated for the first namespace prefix ns found in the interface header file input to the soapcpp2 tool.
# cwmpReadme.md service and data binding interface details.

cwmpfiles := ./soapcpp2_output/cwmpC.c ./soapcpp2_output/cwmpServer.c

objects += $(cwmpfiles:%.c=%.o)


# acs.wsdl included tr-069.wsdl
#
# soapcpp2 -h
# -C      generate client-side source code only
# -S      generate server-side source code only
# -L      don't generate soapClientLib/soapServerLib
# -c      generate C source code
# -r      generate soapReadme.md report
# -dpath  use path to save files
# -Ipath  use path(s) for #import (paths separated with ':')
# -pname  save files with new prefix name instead of 'soap'
#         Do not specify -1 or -2, SOAP transport, supporting both SOAP 1.1 and 1.2
# -1      generate SOAP 1.1 source code, client use, server handle 1.1 and 1.2 at same time
# -2      generate SOAP 1.2 source code, client use, server handle 1.1 and 1.2 at same time
# -g      generate XML sample messages in template format for testmsgr
# -T      generate server auto-test source code
$(cwmpfiles) &: ./acs.wsdl ./wsdl2h_output/tr-069.wsdl
	mkdir -p ./soapcpp2_output; \
	soapcpp2 -c -pcwmp -L -r -g ./acs.wsdl -I ./wsdl2h_output -d ./soapcpp2_output


################### Gcc compile ########################################
CC := gcc

INCLUDES := -I ./ -I ./soapcpp2_output -I ./wsdl2h_output -I ./gsoap/libgsoap

CFLAGS := -g -Wall -DHAVE_CONFIG_H -DWITH_OPENSSL -DWITH_IPV6 \
		-DBUILD_DATE="\"$(BUILD_DATE)\"" \
		-DTINYACS_VERSION="\"$(TINYACS_VERSION)\"" \
		-DWITH_SOAPDEFS_H \
		-Wno-deprecated-declarations $(INCLUDES)

LDLIBS := -lssl -lcrypto

tinyacsfiles := ./cpe_req.c ./handle_cpe_req.c

objects += $(tinyacsfiles:%.c=%.o)

CFLAGS += -MMD
-include $(objects:.o=.d)


tinyacs: $(objects)


release: all httpssupport
	rm -rf output
	mkdir -p output/rpc
	cp ./tinyacs output/
	cd output/; \
	strip ./tinyacs; \
	./tinyacs -h > Readme.txt 2>&1; \
	cd ../
	cp -rf ../rpc_examples output/
	cp -rf ../cpe_simpulator output/
	cp output/rpc_examples/000_get_rpc.xml output/rpc/
	cp output/rpc_examples/002_get_param_value.xml output/rpc/
	mkdir -p output/ssl_server
	mkdir -p output/ssl_client
	cat ssl/gen/ssl_server/server_private_key.pem ssl/gen/ssl_server/server_ca.pem > output/ssl_server/KeyAndCA.pem
	cp ssl/gen/ssl_server/server_ca.pem output/ssl_server/server_ca.pem
	cp ssl/gen/ssl_client/* output/ssl_client/
	cp ssl/gen/ssl_server2client/* output/ssl_client/
	tar -czvf ./tinyacs_$(TINYACS_VERSION).tar.gz --transform s/output/tinyacs/ output
	mv ./tinyacs_$(TINYACS_VERSION).tar.gz output/


httpssupport:
	@test -f ssl/.stamp_ssl_files_installed && echo "All https protocol needs keys and certificates has been generated, skip ssl." || make -C ssl AllKeysAndCAs ACS_SERVER_DOMAIN_NAME=$(ACS_SERVER_DOMAIN_NAME)


################### clean ##############################################
prune: clean
	make -C gsoap prune
	make -C ssl prune

clean:
	rm -rf tags
	find -name "*.log" -type f -delete
	find -name "*.d" -delete
	find ./ -name "*.o" -delete
	find gsoap -name "*.o" -delete
	rm -rf ./wsdl2h_output
	rm -rf ./soapcpp2_output
	rm -rf ./tinyacs
	rm -rf ./cscope*
	rm -rf ./httpdatest/gen
	rm -rf output
	rm -rf ./cwmp_rpc_schema/cwmp.xsd

