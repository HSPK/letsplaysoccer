obj = client.o common.o udp_client.o
isd = 
inc = ../common/
client: $(obj)
	gcc -o client $(obj) -lpthread $(isd)
client.o: client.c $(inc)*.h
	gcc -c client.c $(isd) -I $(inc)
common.o: $(inc)common.c ../common/*.h
	gcc -c $(inc)common.c $(isd) -I $(inc)
udp_client.o:
	gcc -c $(inc)udp_client.c $(isd) -I $(inc)
.PHONY:clean
clean:
	rm $(obj)
