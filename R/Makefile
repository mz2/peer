INTERFACES = peer.i
WRAPPERS   = $(INTERFACES:.i=_wrap.cpp)
PROXIES    = $(INTERFACES:.i=.py      )

all: peer_wrap.cpp
	R CMD SHLIB peer_wrap.cpp

peer_wrap.cpp: peer.i
	swig -c++ -r -o peer_wrap.cpp -I../External -I.. peer.i

clean:
	$(RM) ./peer_wrap.so
	$(RM) $(WRAPPERS)
	$(RM) $(PROXIES)
