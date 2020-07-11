obj = client.o common.o udp_client.o chat_ui.o send_chat.o
isd = -fPIC -lncursesw
inc = ../common/
client: $(obj)
	gcc -o client $(obj) -lpthread $(isd)
client.o: client.c $(inc)*.h
	gcc -c client.c $(isd) -I $(inc)
common.o: $(inc)common.c ../common/*.h
	gcc -c $(inc)common.c $(isd) -I $(inc)
udp_client.o:
	gcc -c $(inc)udp_client.c $(isd) -I $(inc)
chat_ui.o: chat_ui.c
	gcc -c chat_ui.c -I $(inc) $(isd)
send_chat.o: send_chat.c
	gcc -c send_chat.c -I $(inc) $(isd) 
.PHONY:clean
clean:
	rm $(obj)
