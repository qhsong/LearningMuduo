a.out:
	g++ -g $(in) -I/home/sqh/build/debug-install/include -L/home/sqh/build/debug-install/lib -lmuduo_net -lmuduo_base -lpthread
clean:
	rm a.out
