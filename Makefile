ALL_OBJ=networking.o message_management.o support.o 

CA_OBJ=ca_code/query_response.o

all: ca client

%.o: %.cpp
	g++ -g -c $< -o $@
	
ca: $(ALL_OBJ) $(CA_OBJ)
	g++ -o CA -g main_ca.cpp $(ALL_OBJ) $(CA_OBJ) -lcrypto -lpthread
	
client: $(ALL_OBJ)
	g++ -o Client -g main_client.cpp $(ALL_OBJ) -lcrypto -lpthread
	
clean:
	rm *.o
	rm CA
	rm Client
