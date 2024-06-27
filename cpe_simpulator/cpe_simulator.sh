
preare_sslcfg(){
	mkdir -p ./ssl_client/ ./ssl_server/
	docker cp tinyacs:/root/ssl_client/client_private_key.pem ./ssl_client/client_private_key.pem
	docker cp tinyacs:/root/ssl_client/client_ca_signed_by_server.pem ./ssl_client/client_ca_signed_by_server.pem
	docker cp tinyacs:/root/ssl_server/server_ca.pem ./ssl_server/server_ca.pem
}

send_inform(){
	curl -v --no-buffer \
	--resolve acs.debug.abc:10240:127.0.0.1 \
	-H "Content-Type: text/xml; charset=utf-8" \
	-H 'Expect:' \
	-H "Connection: close" \
	--data-binary @cpe_examples/cpe_inform.txt \
	--connect-timeout 3 \
	http://acs.debug.abc:10240 
}

send_inform_https(){
	preare_sslcfg;
	curl -v --no-buffer \
	--cert ssl_client/client_ca_signed_by_server.pem \
	--key ssl_client/client_private_key.pem \
	--cacert ssl_server/server_ca.pem \
	--capath ssl_server/ \
	--resolve my.tinyacs.acs:10240:127.0.0.1 \
	--digest -u "twoweek:helloworld" \
	-H "Content-Type: text/xml; charset=utf-8" \
	-H 'Expect:' \
	-H 'Connection: close' \
	--data-binary @cpe_examples/cpe_inform.txt \
	--connect-timeout 3 \
	https://my.tinyacs.acs:10240 \

}

if [[ "$1" == "send_inform" ]] ; then 
	send_inform
elif [[ "$1" == "send_inform_https" ]] ; then 
	send_inform_https
else
	echo "Usage: $0 send_inform"
	echo "       $0 send_inform_https"
	exit 1
fi
