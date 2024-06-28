# tinyacs

#### Description
Tiny ACS (Auto Configuration Server) is an open source TR-069(CPE WAN Management Protocol) test tool for developers to debug the TR-069 protocol.

#### License
tinyacs is copyrighted by Laixin LIU in 2024.
Licensed under GPLv2. See source distribution for detailed copyright notices.

#### Installation

1.  Download gsoap_2.8.*.zip from https://sourceforge.net/projects/gsoap2/
	and follow the instructions to place the file.

2.  Update cwmp_rpc_schema/cwmp-1-*.xsd    from https://github.com/BroadbandForum/cwmp-data-models
	and follow the instructions to place the file.

3.  build tinyacs with command:
```shell
	cd acs;make release
#or
	docker compse up -d
```

#### Instructions
```
$ tree
.
├── acs
│   ├── acs.wsdl
│   ├── cpe_req.c
│   ├── cwmp_rpc_schema
│   │   ├── cwmp-1-0.xsd
│   │   ├── cwmp-1-1.xsd
│   │   ├── cwmp-1-2.xsd
│   │   ├── cwmp-1-3.xsd
│   │   └── cwmp-1-4.xsd
│   ├── gsoap
│   │   ├── gsoap_2.8.*.zip
│   │   ├── gsoap_patches
│   │   │   └── 0001_fix_self-closing_element_id_miss_parsing_issue.patch
│   │   └── Makefile
│   ├── handle_cpe_req.c
│   ├── Makefile
│   ├── tinyacs.c
│   ├── soapdefs.h
│   ├── ssl
│   │   ├── client.cfg
│   │   ├── Makefile
│   │   └── server.cfg
│   └── typemap.dat
├── cpe_simpulator
│   ├── cpe_examples
│   │   └── cpe_inform.txt
│   ├── cpe_simulator.sh
├── docker-compose.yml
├── LICENSE
├── README.md
├── rpc
├── rpc_examples
│   ├── 000_get_rpc.xml
│   ├── 001_get_param_name.xml
│   ├── 002_get_param_value.xml
│   ├── 003_set_param_values.xml
│   ├── 004_get_param_attr.xml
│   ├── 005_set_param_attr.xml
│   ├── 006_add_obj.xml
│   ├── 007_del_obj.xml
│   ├── 008_reboot.xml
│   ├── 009_factory_reset.xml
│   ├── 010_download.xml
│   ├── 011_upload.xml
│   ├── 012_changedustate.friendlyacs.install.xml
│   ├── 012_changedustate.friendlyacs.uninstall.xml
│   ├── 012_changedustate.xml
│   ├── 013_schedule_inform.xml
│   ├── 014_schedule_download.xml
│   ├── 015_cancel_transfer.xml
│   ├── 016_Download_diagnostic.xml
│   ├── 016_IPPing_diagnostic.xml
│   └── 016_Upload_diagnostic.xml
├── tinyacs.dockerfile

```

#### Usage:
```
Usage:
    -h, --help                  Print this help and exit.
    -v, --version               Print the version and exit.
    -p, --port=PORT             The PORT that tinyacs will listen on.
                                Default PORT is 10240.
    -c, --auth                  Enable HTTP authentication.
    -u, --userid=USERNAME       USERNAME that the CPE will use to send to tinyacs.
    -w, --passwd=PASSWORD       The PASSWORD that the CPE will use to send to tinyacs.
    -s, --https                 Accept HTTPS connection instead of HTTP.
    -k, --keyfile=KEYFILE       KEYFILE (private key and certificate) for HTTPS connections use.
                                Default KEYFILE is "ssl_server/KeyAndCA.pem".
        --keypass=KEYPASS       KEYPASS to read KEYFILE.
                                Default KEYPASS is NULL.
        --cacert=CACERT         CACERT file to authenticate clients, for HTTPS connections use.
                                Default CACERT is NULL.
        --capath=CAPATH         CAPATH directory to authenticate clients, for HTTPS connections use.
                                Default CAPATH is NULL.
    -d, --debuglevel=LEVEL      For debugging. Default LEVEL is "notice", possible options:
                                "error", "notice", "info", "debug".
    -r, --rpcdir=DIR            A DIR containing a set of RPC request files that 
                                will be sent to the CPE once tinyacs receives INFORM.
                                Default DIR is rpc.
    -l, --loop=NUMBER           Send the request files under the DIR directory cyclically, 
                                Send only once by default(loop=0). -1 for infinite loop.
    -t, --servetimeout=TIMEOUT  Service TIMEOUT in seconds.
                                Default TIMEOUT is 0(Never timeout).
    -f, --tracefile=FILE        A FILE name for saving plain HTTP logs between tinyacs and CPE.
                                Default FILE name is "trace.log".
```
#### Examples:
    tinyacs --port 10240 --https --keyfile ssl_server/KeyAndCA.pem \
                           --cacert ssl_server/server_ca.pem  \
                           --auth --userid SN001 --passwd secret  \
                           --rpcdir rpc --tracefile trace.log
    tinyacs -d info -r rpc -l -1 -t trace.log
    tinyacs -t trace.log

#### Features:
	Support IPv4 and IPv6
	Support HTTP/HTTPS with basic and digest authentication
	Support cwmp-1-0 cwmp-1-1 cwmp-1-2
	Support docker compose

#### Contribution

1.  Fork the repository
2.  Create Feat_xxx branch
3.  Commit your code
4.  Create Pull Request
