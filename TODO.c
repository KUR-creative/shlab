what's going on this project?

OBJECTIVE
all test passed

//test1	//maybe...
//test2
	//tsh need to deal with 'quit' cmd
test3
	//do foreground job - how to test?
	is it ok with bg jobs?	need to implement bg & fg 
test4
	impl bg jobs.
	but how to print ref's output? just use printf???

	t3,t4 test:
	//1. explicit wait test: sh never wait bg explicitly
	//2. reaping test: use WNOHANG! 
		//can't use criterion. use my on test functions...
		//now WRITE SIGNAL HANDLER.
	3. bg job allocation info output
		[jobnum] (pid) cmdline
	4.
	why SEGFAULT occurred?
		maybe input(interrupt) when execute sigchld handler code.
		-> block signal?

	why waiting explicitly? 
		아마 WNOHANG을 안 쓰므로 다 기다린다고 그러는 것이다...
		-> use different loop.
		-> using WNOHANG waitpid. but.. 
		-> need to implement joblist and operations.
test5
	job list
		//addjob
		//if jobs are full? : job won't be allocated. already implemented.
		delete job when job process terminated. using sigchld_handler
		why [0] job never deleted??
->			maybe I have to change sigchld_handler... using jobs array.
->				and change fg reaping test(use jobs too)
	when eval, add job into jobs.
		more state job operations.
			//FG
			//BG
			ST
		//func: deleteAllJobs
		//func: isAllZero
	when child terminated, del child from jobs.
	//tsh> jobs   (interface)
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
