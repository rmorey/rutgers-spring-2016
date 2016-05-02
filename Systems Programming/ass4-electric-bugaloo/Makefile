# Start Bank
all: BankServer PrintBank

PrintBank: PrintBank.c
	gcc -g -Wall -o PrintBank PrintBank.c -pthread

BankServer: BankServer.c
	gcc -g -Wall -o BankServer BankServer.c -pthread

clean:
	rm -f BankServer PrintBank *.o
# End Bank

# Start Test
test: TestClient TestServer

TestClient: TestClient.c
	gcc -g -Wall -o TestClient TestClient.c

TestServer: TestServer.c
	gcc -g -Wall -o TestServer TestServer.c

test-clean:
	rm -f TestClient TestServer *.o
# End Test
