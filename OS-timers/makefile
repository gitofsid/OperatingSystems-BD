all: timer msys mfunction thread_switch process_switch

timer: hr_timer.c
	gcc hr_timer.c -o timer

msys: minimal_sys.c
	gcc minimal_sys.c -o msys

mfunction: minimal_function.c
	gcc minimal_function.c -o mfunction

thread_switch: thread_switch.c
	gcc thread_switch.c -lpthread -o thread_switch

process_switch: process_switch.c
	gcc process_switch.c -o process_switch

clean:
	rm timer msys mfunction thread_switch process_switch