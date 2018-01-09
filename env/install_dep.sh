cd libnfnetlink && ./autogen.sh && ./configure && make && make install
cd ..
cd libmnl && ./autogen.sh && ./configure && make && make install
cd ..
cd libnetfilter_queue && ./autogen.sh && ./configure && make && make install