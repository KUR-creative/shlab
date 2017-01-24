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
	1. explicit wait test: sh never wait bg explicitly
	2. reaping test: how?
test5
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

how to check child process has been reaped?? -> code it!
	maybe use waitpid w/ WNOHANG : 이전에 defunct이던 pid를 넣으면
	그 pid가 반환된다 - 이것으로 zombie인지 테스트할수도 있다...
	system("ps")


maybe I can check job list.. to test something -> code it!

* can you print test output in color?
