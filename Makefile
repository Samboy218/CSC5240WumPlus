modName = wumplus_world
sourceName = $(modName).cpp

all: $(modName)
    
$(modName): $(sourceName)
	 g++ -O2 -s -DNDEBUG -o $(modName) $(sourceName) -std=gnu++11

.PHONY: clean
clean:
	rm -f $(modName)

.PHONY: debug
debug: $(sourceName)
	 g++ -o $(modName) $(sourceName) -std=gnu++11
