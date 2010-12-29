INTERFACES = peer.i
WRAPPERS   = $(INTERFACES:.i=_wrap.cxx)
PROXIES    = $(INTERFACES:.i=.py      )

all: $(WRAPPERS) peer_wrap.cxx 
	./setup.py build

%_wrap.cxx: %.i 
	swig -c++ -python -I../External -I.. $<

clean:
	$(RM) -r build
	$(RM) $(WRAPPERS)
	$(RM) $(PROXIES)
