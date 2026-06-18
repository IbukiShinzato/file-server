CC = gcc
CFLAGS = -Wall -Wextra -O2
LDFLAGS = -pthread

SERVER = server
CLIENT = client
TEST_CLIENT = test_client
TEST_FILE = ~/test*.txt

.PHONY: all run-server run-client run-test-client test-files check-files clean

all: $(SERVER) $(CLIENT) $(TEST_CLIENT)

$(SERVER): server.c
	$(CC) $(CFLAGS) -o $(SERVER) server.c $(LDFLAGS)

$(CLIENT): client.c
	$(CC) $(CFLAGS) -o $(CLIENT) client.c

$(TEST_CLIENT): test_client.c
	$(CC) $(CFLAGS) -o $(TEST_CLIENT) test_client.c $(LDFLAGS)

run-server: $(SERVER)
	./$(SERVER)

run-client: $(CLIENT)
	./$(CLIENT)

run-test-client: $(TEST_CLIENT)
	./$(TEST_CLIENT)

test-files:
	for i in $$(seq 1 16); do \
		LC_ALL=C tr -dc 'A-Za-z0-9' < /dev/urandom | head -c 32 > "$$HOME/test$$i.txt"; \
		echo >> "$$HOME/test$$i.txt"; \
	done

check-files:
	for i in $$(seq 1 16); do \
		echo "[test$$i.txt]: "; \
		cat "$$HOME/test$$i.txt"; \
	done

clean:
	rm -f $(SERVER) $(CLIENT) $(TEST_CLIENT) $(TEST_FILE)

