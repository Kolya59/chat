build:
	gcc main.c -o ./bin/chat -lpthread -lrt
	chmod +x ./bin/chat