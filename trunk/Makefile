ALL_OBJ=networking.o message_management.o support.o 

CA_OBJ=ca_code/query_response.o ca_code/receive_query.o

all: ca client

%.o: %.cpp
	g++ -g -c $< -o $@
	
ca: $(ALL_OBJ) $(CA_OBJ) main_ca.cpp
	g++ -o CA -g main_ca.cpp $(ALL_OBJ) $(CA_OBJ) -lcrypto -lpthread
	
client: $(ALL_OBJ)
	g++ -o Client -g main_client.cpp $(ALL_OBJ) -lcrypto -lpthread
	
clean:
	rm *.o
	rm ca_code/*.o
	rm CA
	rm Client
