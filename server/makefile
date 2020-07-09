obj = server.o common.o udp_server.o
isd = -D _D
inc = ../common/
server: $(obj)
	gcc -o server $(obj) -lpthread $(isd)
server.o: server.c $(inc)*.h
	gcc -c server.c $(isd) -I $(inc)
common.o: $(inc)common.c ../common/*.h
	gcc -c $(inc)common.c $(isd) -I $(inc)
udp_server.o:
	gcc -c $(inc)udp_server.c $(isd) -I $(inc)
.PHONY:clean
clean:
	rm $(obj)
