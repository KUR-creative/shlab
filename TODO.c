what's going on this project?

OBJECTIVE
all test passed

**
//just input '\n' then segfault? wtf?
undefined cmd -> don't fork anything but just filter it.
	is it impossible???
//test1	//maybe...
//test2
	//tsh need to deal with 'quit' cmd
test3
	//do foreground job - how to test?
	//is it ok with bg jobs?	need to implement bg & fg 

test4
	//why it prints in arbitrary order? 
	//because fork and execve echo. need to explicitly wait fg jobs..!

	//t3,t4 test:
	//1. explicit wait test: sh never wait bg explicitly
	//2. reaping test: use WNOHANG! 
		//can't use criterion. use my on test functions...
		//now WRITE SIGNAL HANDLER.
	//3. bg job allocation info output
		//[jobnum] (pid) cmdline
	//4.why SEGFAULT occurred?
		//"\n" 같은 거는 argv[0] = NULL이 들어가.

	//5.why waiting explicitly? (in sigchld handler)
		//jobs command error.
		//아마 WNOHANG을 안 쓰므로 다 기다린다고 그러는 것이다...
			//use different loop.
			//using WNOHANG waitpid. but.. 
			//need to implement joblist and operations.
		//use jobs! that will be work!
		//use git branch! it would be slightly dangerous..!

	6.eval("&") is exceptional.// is it ok?

test5
	//job list
		//addjob
		//if jobs are full? : job won't be allocated. already implemented.
		//delete job when job process terminated. using sigchld_handler
		//why [0] job never deleted??
			//change sigchld_handler... using jobs array.
			//change fg reaping test(use jobs too)
				//is fg reaped correctly?
				//is fg job in jobs deleted?
	//when eval, add job into jobs.
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
		//implement jobs command actually!
		//it's already implemented, but error: by sigchld_handler code
	//deleteAllJobs jobs기준으로 sigchld handler에서 job을 삭제한다면
	//이 함수 사용시 문제가 생길 수 있다.

//**sigchld_handler 변경으로 인해 고쳐질 수 있는 현상(테스트)들
		//t3, t4는 eval만 가지고는 테스트할 수 없다.. 유닛테스트가 아냐.
	//trace04: print echo ouptuts in no delay(delay after first echo)
	//trace05: command jobs actually prints jobs.
	//utest: (crash test)

test6
	//implement sigint_handler.
		//don't die
		//kill fg job.
		//kill fg job only 
			//kill ONLY fg job. not bg jobs.
			//-> delete fg job from jobs.
		//kill fg only test: test08 passed. but it actually didn't work!
			//need manual test. 
			//(ex)
			//./tsh
			//./myspin 50 &
			//./myspin 50 &
			//./myspin 50
			//C^
			//./jobs
				//then all jobs are gone! wtf?
		//when no fg job run, then C^ must never work.

		why test08 is a false test??? why?
test7
	//impl sigint_handler

test8
	//implement sigstop_handler
	//now fg --stop--> st: then no more explicit waiting.
		//1- change state in jobs.
		//2- check fg is in job list? at fg waiting loop in eval

test9
	implement bg builtin-command
		cmd bg interface
		do_bgfg
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
