CC = gcc
CLAVES_PATH = claves
FUNCIONES_SERVIDOR_PATH = funciones_servidor
FUNCIONES_SOCKETS_PATH = funciones_sockets
CFLAGS = -lrt -lpthread
OBJS = servidor cliente_tests cliente_concurrente
BIN_FILES = servidor cliente_tests cliente_concurrente

all: $(OBJS)

libsockets.so: $(FUNCIONES_SOCKETS_PATH)/funciones_sockets.c
	$(CC) -fPIC -c -o $(FUNCIONES_SOCKETS_PATH)/funciones_sockets.o $<
	$(CC) -shared -fPIC -o $@ $(FUNCIONES_SOCKETS_PATH)/funciones_sockets.o

libclaves.so: $(CLAVES_PATH)/claves.c libsockets.so
	$(CC) -fPIC -c -o $(CLAVES_PATH)/claves.o $< -L. -lsockets
	$(CC) -shared -fPIC -o $@ $(CLAVES_PATH)/claves.o -L. -lsockets

libserverclaves.so: $(FUNCIONES_SERVIDOR_PATH)/funciones_servidor.c libsockets.so
	$(CC) -fPIC -c -o $(FUNCIONES_SERVIDOR_PATH)/funciones_servidor.o $< -L. -lsockets
	$(CC) -shared -fPIC -o $@ $(FUNCIONES_SERVIDOR_PATH)/funciones_servidor.o -L. -lsockets

servidor:  servidor.c libserverclaves.so libsockets.so
	$(CC) -L. -lserverclaves -lsockets -o $@.out $< ./libserverclaves.so ./libsockets.so $(CFLAGS)

cliente_tests: cliente_tests.c libclaves.so
	$(CC) -L. -lclaves -o $@.out $< ./libclaves.so -L. -lsockets $(CFLAGS)

cliente_concurrente: cliente_concurrente.c libclaves.so
	$(CC) -L. -lclaves -o $@.out $< ./libclaves.so -L. -lsockets $(CFLAGS)

clean:
	rm -f $(BIN_FILES) *.out *.o *.so $(CLAVES_PATH)/*.o $(FUNCIONES_SERVIDOR_PATH)/*.o $(FUNCIONES_SOCKETS_PATH)/*.o tuplas.txt

re:	clean all

.PHONY: all libclaves.so libserverclaves.so libsockets.so servidor cliente_tests cliente_concurrente clean re