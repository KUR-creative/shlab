what's going on this project?

OBJECTIVE
all test passed

**
//just input '\n' then segfault? wtf?
undefined cmd -> don't fork anything but just filter it.
	is it impossible???
cputs(YELLOW,"\n---undefined cmd: don't fork anything just filter it---");
	sig_atomic_t prev = spid;
	eval("foo\n");
	sleep(1);
	ASSERT_EQ(spid, prev, "spid changed! sigchld_handler was called!");
cputs(YELLOW,"\n----------------------------------------------|");
//test1	//maybe...
//test2
	//tsh need to deal with 'quit' cmd
test3
	//do foreground job - how to test?
	is it ok with bg jobs?	need to implement bg & fg 

test4
	//why it prints in arbitrary order? 
	//because fork and execve echo. need to explicitly wait fg jobs..!

	t3,t4 test:
	//1. explicit wait test: sh never wait bg explicitly
	//2. reaping test: use WNOHANG! 
		//can't use criterion. use my on test functions...
		//now WRITE SIGNAL HANDLER.
	//3. bg job allocation info output
		//[jobnum] (pid) cmdline
	//4.why SEGFAULT occurred?
		//"\n" 같은 거는 argv[0] = NULL이 들어가.

	5.why waiting explicitly? (in sigchld handler)
		jobs command error.
		아마 WNOHANG을 안 쓰므로 다 기다린다고 그러는 것이다...
			use different loop.
			using WNOHANG waitpid. but.. 
			need to implement joblist and operations.
		use jobs! that will be work!
		use git branch! it would be slightly dangerous..!

	6.eval("&") is exceptional.// is it ok?

test5
	job list
		//addjob
		//if jobs are full? : job won't be allocated. already implemented.
		delete job when job process terminated. using sigchld_handler
		//why [0] job never deleted??
			change sigchld_handler... using jobs array.
			//change fg reaping test(use jobs too)
				//is fg reaped correctly?
				//is fg job in jobs deleted?
	when eval, add job into jobs.
		more state job operations.
			//FG
			//BG
			ST
		//func: deleteAllJobs
		//func: isAllZero
	//when child terminated, del child from jobs.
		//FG
		//BG
	//tsh> jobs   (interface)
		implement jobs command actually!
		it's already implemented, but error: by sigchld_handler code
	deleteAllJobs jobs기준으로 sigchld handler에서 job을 삭제한다면
	이 함수 사용시 문제가 생길 수 있다.
test6
test7
test8
test9
test10
test11
test12
test13
test14
test15
test16

mt own tiny tester library
//ASSERT
//ASSERT_EQ

maybe I can check job list.. to test something -> code it!

* can you print test output in color?
