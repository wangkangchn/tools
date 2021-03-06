CC = gcc -g

bstree_demo:
	${CC} bstree_demo.c -o app_bstree_demo
graph_demo:
	${CC} graph_demo.c -o app_graph_demo

list_demo:
	${CC} list_demo.c -o app_list_demo

app:
	${CC} test_list1.c -o app_list

log:
	${CC} test_log.c log.c -o app_log -lpthread

kill:
	${CC} test_kill_main.c -o app_kill -lpthread

signo:
	${CC} test_signo.c -o app_signo

color:
	${CC} test_color.c -o app_color 

filesize:
	${CC} test_filesize.c -o app_filesize

test_queue:
	gcc test_queue.c -o app_test_queue

stack:
	gcc stack.c -o app_stack

list:
	gcc test_list.c -o app_list

clean:
	rm -rf app*