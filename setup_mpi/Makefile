#Makefile

hi : hello_c.c
	mpicc hello_c.c -o hi

go :
	mpirun  -np 8 --hostfile jupiter.hosts ./hi
	
cp_hosts: 
	cp ../../ServerControl/ServerCtl/machinefile.server_ctl jupiter.hosts
