modName = wumplus_world
sourceName = $(modName).cpp
extras = agent.cpp

all: $(modName)
    
$(modName): $(sourceName) $(extras)
	 g++ -O2 -s -DNDEBUG -o $(modName) $(sourceName) $(extras) -std=gnu++11

.PHONY: clean
clean:
	rm -f $(modName)

.PHONY: debug
debug: $(sourceName) $(extras)
	 g++ -o $(modName) $(sourceName) $(extras) -std=gnu++11
