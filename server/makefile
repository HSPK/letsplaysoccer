obj = server.o common.o udp_server.o udp_epoll.o thread_pool.o sub_reactor.o
isd = -D _D -g
inc = ../common/
server: $(obj)
	gcc -o server $(obj) -lpthread $(isd)
server.o: server.c $(inc)*.h
	gcc -c server.c $(isd) -I $(inc)
common.o: $(inc)common.c ../common/*.h
	gcc -c $(inc)common.c $(isd) -I $(inc)
udp_server.o: $(inc)udp_server.c $(inc)*.h 
	gcc -c $(inc)udp_server.c $(isd) -I $(inc)

udp_epoll.o: $(inc)udp_epoll.c  $(inc)*.h
	gcc -c $(inc)udp_epoll.c $(isd) -I $(inc)
thread_pool.o: $(inc)thread_pool.c $(inc)*.h
	gcc -c $(inc)thread_pool.c $(isd) -I $(inc)
sub_reactor.o: $(inc)sub_reactor.c $(inc)
	gcc -c $(inc)sub_reactor.c $(isd) -I $(inc)

.PHONY:clean
clean:
	rm $(obj)
