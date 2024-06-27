INSTALL_PREFIX:=output
.PHONY = gsoap_lib_source clean prune

gsoap-2.8:
	@test -f gsoap_2.8.*.zip || (echo "Need to download gsoap tools." && exit 1) && \
	unzip -n gsoap_2.8.*.zip -d ./ && \
	cd gsoap-2.8 && \
	mkdir -p ${shell pwd}/${INSTALL_PREFIX} && \
	./configure --enable-ipv6 --prefix=${shell pwd}/${INSTALL_PREFIX} && \
	make && \
	make install && cd - 


################### prepare library sourcecode #########################
GSOAP_PATCHES:=$(wildcard ./gsoap_patches/*.patch)
gsoap_lib_source: gsoap-2.8
	@echo Preparing stdsoap and plugin files...
	@mkdir -p ./libgsoap/plugin/
	@cp ./gsoap-2.8/config.h                   ./libgsoap/config.h
	@cp ./gsoap-2.8/gsoap/stdsoap2.c           ./libgsoap/stdsoap2.c
	@cp ./gsoap-2.8/gsoap/stdsoap2.h           ./libgsoap/stdsoap2.h
	@cp ./gsoap-2.8/gsoap/plugin/httpda.c      ./libgsoap/plugin/httpda.c
	@cp ./gsoap-2.8/gsoap/plugin/httpda.h      ./libgsoap/plugin/httpda.h
	@cp ./gsoap-2.8/gsoap/plugin/threads.h     ./libgsoap/plugin/threads.h
	@cp ./gsoap-2.8/gsoap/plugin/smdevp.c      ./libgsoap/plugin/smdevp.c
	@cp ./gsoap-2.8/gsoap/plugin/smdevp.h      ./libgsoap/plugin/smdevp.h
	@echo Preparing stdsoap and plugin files done.
	@for i in $(GSOAP_PATCHES) ; do \
		echo "Applying $${i} : "; \
		cat $${i} | patch -f -p1 -E ; \
		if [ $$? != 0 ] ; then \
			echo "Patch failed!  Please fix $$i!"; \
			exit 1; \
		fi \
	done

clean:

prune:
	rm -rf output
	rm -rf libgsoap
	rm -rf gsoap-2.8
