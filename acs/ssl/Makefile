ACS_SERVER_DOMAIN_NAME:=my.tinyacs.acs

ifneq ($(shell which openssl3),) 
OPENSSL:=openssl3
else
OPENSSL:=openssl
endif

AllKeysAndCAs:ServerSignClient

ServerSignClient:GenServerKeyAndCA GenClientKeyAndCsr
	@if [ -f ./gen/ssl_server2client/client_ca_signed_by_server.pem ]; \
	then \
		echo "Client certificate already generated."; \
	else \
		echo "Server side signing the certificate request from Client..."; \
		mkdir -p ./gen/ssl_server2client/; \
		cat /etc/ssl/openssl.cnf ./client.cfg > ./gen/client_full.cfg; \
		$(OPENSSL) x509 -req -days 730 -sha384 \
			-in ./gen/ssl_client2server/client_ca.csr \
			-CA ./gen/ssl_server/server_ca.pem \
			-CAkey ./gen/ssl_server/server_private_key.pem \
			-CAcreateserial \
			-out ./gen/ssl_server2client/client_ca_signed_by_server.pem \
			-extensions v3_req -extfile  ./gen/client_full.cfg; \
		echo "All https protocol needs keys and certificates generated."; \
		touch .stamp_ssl_files_installed; \
	fi

GenServerKeyAndCA:
	@if [ -f ./gen/ssl_server/server_private_key.pem ] && [ -f ./gen/ssl_server/server_ca.pem ]; \
	then \
		echo "Server key and certificate already generated."; \
	else \
		mkdir -p ./gen/ssl_server/; \
		echo "generating Server side private key..."; \
		$(OPENSSL) ecparam -name secp384r1 -genkey -out ./gen/ssl_server/server_private_key.pem; \
		echo "generating Server side self-signed root certificate..."; \
		cat /etc/ssl/openssl.cnf ./server.cfg > ./gen/server_full.cfg; \
		$(OPENSSL) req -new -x509 -days 730 -sha384 \
			-key ./gen/ssl_server/server_private_key.pem \
			-out ./gen/ssl_server/server_ca.pem \
			-subj /C=US/ST=California/L=Mountain\ View/O=IamACS\ Inc/CN=$(ACS_SERVER_DOMAIN_NAME)/emailAddress=service@tinyacs.acs \
			-config ./gen/server_full.cfg; \
	fi

GenClientKeyAndCsr:
	@if [ -f ./gen/ssl_client/client_private_key.pem ] && [ -f ./gen/ssl_client2server/client_ca.csr ]; \
	then \
		echo "Client key and Certificate Signing Request(csr) already generated."; \
	else \
		mkdir -p ./gen/ssl_client/; \
		echo "Generating Client side private key and certificate Request..."; \
		$(OPENSSL) ecparam -name secp384r1 -genkey -out ./gen/ssl_client/client_private_key.pem; \
		echo "Generating Certificate Signing Request..."; \
		cat /etc/ssl/openssl.cnf ./client.cfg > ./gen/client_full.cfg; \
		mkdir -p ./gen/ssl_client2server/; \
		$(OPENSSL) req -new -sha384 \
			-key ./gen/ssl_client/client_private_key.pem \
			-out ./gen/ssl_client2server/client_ca.csr \
			-subj /C=US/ST=California/L=Mountain\ View/O=IamCPE\ Inc/CN=*.iamacpe.cpe/emailAddress=service@iamacpe.cpe \
			-config ./gen/client_full.cfg; \
	fi

clean:

prune:
	rm -rf gen
	rm -rf .stamp_ssl_files_installed
	rm -rf ssl/gen/client_full.cfg
	rm -rf ssl/gen/server_full.cfg
