/* 
 * tsh - A tiny shell program with job control
 * 
* <Put your name and login ID here>
*/
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <ctype.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <errno.h>

#include "csapp.h"

/* Misc manifest constants */
//#define MAXLINE    1024   /* max line size */
#define MAXARGS     128   /* max args on a command line */
#define MAXJOBS      16   /* max jobs at any point in time */
#define MAXJID    1<<16   /* max job ID */

/* Job states */
#define UNDEF 0 /* undefined */
#define FG 1    /* running in foreground */
#define BG 2    /* running in background */
#define ST 3    /* stopped */

/* 
 * Jobs states: FG (foreground), BG (background), ST (stopped)
 * Job state transitions and enabling actions:
 *     FG -> ST  : ctrl-z
 *     ST -> FG  : fg command
 *     ST -> BG  : bg command
 *     BG -> FG  : fg command
 * At most 1 job can be in the FG state.
 */

/* Global variables */
extern char **environ;      /* defined in libc */
char prompt[] = "tsh> ";    /* command line prompt (DO NOT CHANGE) */
int verbose = 0;            /* if true, print additional output */
int nextjid = 1;            /* next job ID to allocate */
char sbuf[MAXLINE];         /* for composing sprintf messages */

struct job_t {              /* The job struct */
    pid_t pid;              /* job PID */
    int jid;                /* job ID [1, 2, ...] */
    int state;              /* UNDEF, BG, FG, or ST */
    char cmdline[MAXLINE];  /* command line */
};
struct job_t jobs[MAXJOBS]; /* The job list */
/* End global variables */


/* Function prototypes */

/* Here are the functions that you will implement */
void eval(char *cmdline);

int builtin_cmd(char **argv);
void do_bgfg(char **argv);
void waitfg(pid_t pid);

void sigchld_handler(int sig);
void sigtstp_handler(int sig);
void sigint_handler(int sig);

/* Here are helper routines that we've provided for you */
int parseline(const char *cmdline, char **argv); 
void sigquit_handler(int sig);

void clearjob(struct job_t *job);
void initjobs(struct job_t *jobs);
int maxjid(struct job_t *jobs); 
int addjob(struct job_t *jobs, pid_t pid, int state, char *cmdline);
int deletejob(struct job_t *jobs, pid_t pid); 
pid_t fgpid(struct job_t *jobs);
struct job_t *getjobpid(struct job_t *jobs, pid_t pid);
struct job_t *getjobjid(struct job_t *jobs, int jid); 
int pid2jid(pid_t pid); 
void listjobs(struct job_t *jobs);

void usage(void);
//void unix_error(char *msg);
//void app_error(char *msg);
typedef void handler_t(int);
//handler_t *Signal(int signum, handler_t *handler);

//mine
volatile sig_atomic_t spid = 0; // shared pid (shared by main and handler)
// is gpid really needed?
void utest(void);
void deleteAllJobs(struct job_t jobs[]);
int isAllZero(struct job_t* ptr, size_t size);
int areJobsEmpty(struct job_t* jobs);
#define RED		"\x1b[31m"
#define YELLOW	"\x1b[33m"
#define ENDCOL	"\x1b[0m"
#define cputs(col,str)	puts(col str ENDCOL)

#define ASSERT(cond,failstr)		if((cond) != 1) cputs(RED,failstr)
#define ASSERT_NOT(cond,failstr)	if((cond) == 1) cputs(RED,failstr)
#define ASSERT_EQ(a,b,failstr)		if((a) != (b)) cputs(RED,failstr)
#define ASSERT_NEQ(a,b,failstr)		if((a) == (b)) cputs(RED,failstr)

/*
 * main - The shell's main routine 
 */
/*------------------------------------------------------------------*/
#ifdef RELEASE
int main(int argc, char **argv) 
{
    char c;
    char cmdline[MAXLINE];
    int emit_prompt = 1; /* emit prompt (default) */

    /* Redirect stderr to stdout (so that driver will get all output
     * on the pipe connected to stdout) */
    dup2(1, 2);

    /* Parse the command line */
    while ((c = getopt(argc, argv, "hvp")) != EOF) {
        switch (c) {
        case 'h':             /* print help message */
            usage();
			break;
        case 'v':             /* emit additional diagnostic info */
            verbose = 1;
			break;
        case 'p':             /* don't print a prompt */
            emit_prompt = 0;  /* handy for automatic testing */
			break;
		default:
            usage();
		}
    }

    /* Install the signal handlers */

    /* These are the ones you will need to implement */
    Signal(SIGINT,  sigint_handler);   /* ctrl-c */
    Signal(SIGTSTP, sigtstp_handler);  /* ctrl-z */
    Signal(SIGCHLD, sigchld_handler);  /* Terminated or stopped child */

    /* This one provides a clean way to kill the shell */
    Signal(SIGQUIT, sigquit_handler); 

    /* Initialize the job list */
    initjobs(jobs);

							//utest();
    /* Execute the shell's read/eval loop */
    while (1) {
		/* Read command line */
		if (emit_prompt) {
			printf("%s", prompt); //prompt = "tsh>"
			fflush(stdout);
		}
		if ((fgets(cmdline, MAXLINE, stdin) == NULL) && ferror(stdin))
			app_error("fgets error");
		if (feof(stdin)) { /* End of file (ctrl-d) */
			fflush(stdout);
			exit(0);
		}

		/* Evaluate the command line */
		eval(cmdline);
		fflush(stdout);
		fflush(stdout);		
    } 

    exit(0); /* control never reaches here */
}
#endif
/*------------------------------------------------------------------*/
  
/* 
 * eval - Evaluate the command line that the user has just typed in
 * 
 * If the user has requested a built-in command (quit, jobs, bg or fg)
 * then execute it immediately. Otherwise, fork a child process and
 * run the job in the context of the child. If the job is running in
 * the foreground, wait for it to terminate and then return.  Note:
 * each child process must have a unique process group ID so that our
 * background children don't receive SIGINT (SIGTSTP) from the kernel
 * when we type ctrl-c (ctrl-z) at the keyboard.  
*/
void eval(char* cmdline) 
{
	char*		argv[MAXARGS];	
	int			isBg, isBuiltin, state;
	pid_t		pid;
	sigset_t	maskAll, maskChild, prev;

	// mask for all blocking
	Sigfillset(&maskAll);
	// mask for SIGCHLD blocking
	Sigemptyset(&maskChild);
	Sigaddset(&maskChild, SIGCHLD);
	
	isBg = parseline(cmdline, argv);
	state = isBg + 1; //BG = 1+1, FG = 0+1;
	isBuiltin = builtin_cmd(argv);	// run builtin or ret: 
	
	if(! isBuiltin){
		Sigprocmask(SIG_BLOCK, &maskChild, &prev); // block SIGCHLD
		if((pid = Fork()) == 0){
			// set child's own process group id.
			Setpgid(0, 0);
			// unblock SIGCHLD in child
			Sigprocmask(SIG_SETMASK, &prev, NULL); 
			Execve(argv[0], argv, environ);
		}

		//job allocation!
		Sigprocmask(SIG_BLOCK, &maskAll, NULL); // block all
		if( addjob(jobs, pid, state, cmdline) == 0 ){ 
			cputs(RED, "addjob error!");
		}

		// wait fg job explicitly.
		spid = 0;
		if(! isBg){
			while(! spid){ // or.. jobs?
				Sigsuspend(&prev);
			}
		}
		else{ // print bg job allocation info 
			struct job_t* tmpJob = getjobpid(jobs, pid);
			printf("[%d] (%d) %s", 
					tmpJob->jid, tmpJob->pid, tmpJob->cmdline);
		}

		// unblock signals to reap bg
		Sigprocmask(SIG_SETMASK, &prev, NULL);
	}
    return;
}

/* 
 * parseline - Parse the command line and build the argv array.
 * 
 * Characters enclosed in single quotes are treated as a single
 * argument.  Return true if the user has requested a BG job, false if
 * the user has requested a FG job.  
 * 
 * ret: 
 * 0	fg
 * 1	bg
 */
int parseline(const char *cmdline, char **argv) 
{
    static char array[MAXLINE]; /* holds local copy of command line */
    char *buf = array;          /* ptr that traverses command line */
    char *delim;                /* points to first space delimiter */
    int argc;                   /* number of args */
    int bg;                     /* background job? */

    strcpy(buf, cmdline);
    buf[strlen(buf)-1] = ' ';  /* replace trailing '\n' with space */
    while (*buf && (*buf == ' ')) /* ignore leading spaces */
	buf++;

    /* Build the argv list */
    argc = 0;
    if (*buf == '\'') {
		buf++;
		delim = strchr(buf, '\'');
    }
    else {
		delim = strchr(buf, ' ');
    }

    while (delim) {
		argv[argc++] = buf;
		*delim = '\0';
		buf = delim + 1;
		while (*buf && (*buf == ' ')) /* ignore spaces */
			   buf++;

		if (*buf == '\'') {
			buf++;
			delim = strchr(buf, '\'');
		}
		else {
			delim = strchr(buf, ' ');
		}
    }
    argv[argc] = NULL;
    
    if (argc == 0)  /* ignore blank line */
		return 1;

    /* should the job run in the background? */
    if ((bg = (*argv[argc-1] == '&')) != 0) {
		argv[--argc] = NULL;
    }
    return bg;
}

/* 
 * builtin_cmd - If the user has typed a built-in command then execute
 *    it immediately.  
 *
 *ret:
 * 0	not a bulitin cmd
 * 1	this is bulitin cmd.
 */
int builtin_cmd(char **argv) 
{
	if(argv[0] == NULL){ // input "\n" 
		return 1;
	}
	if( strcmp("quit", argv[0]) == 0 ){
		exit(0);
	}
	if( strcmp("jobs", argv[0]) == 0 ){
		listjobs(jobs);
		return 1;
	}
    return 0;     /* not a builtin command */
}

/* 
 * do_bgfg - Execute the builtin bg and fg commands
 */
void do_bgfg(char **argv) 
{
    return;
}

/* 
 * waitfg - Block until process pid is no longer the foreground process
 */
void waitfg(pid_t pid)
{
    return;
}

/*****************
 * Signal handlers
 *****************/

/* 
 * sigchld_handler - The kernel sends a SIGCHLD to the shell whenever
 *     a child job terminates (becomes a zombie), or stops because it
 *     received a SIGSTOP or SIGTSTP signal. The handler reaps all
 *     available zombie children, but doesn't wait for any other
 *     currently running children to terminate.  
 *
 *     SIGCHLD는 child가 죽거나/멈추면 parent에게 send된다.
 */
void sigchld_handler(int sig) 
{
	//sio_puts(">>>> SIG CHLD HANDLER <<<<\n");
	int			oldErrno = errno;
	sigset_t	maskAll, prevAll;

	Sigfillset(&maskAll);
	//sio_puts(" -------- \n");
	for(int i = 0; i < MAXJOBS; i++){
		pid_t tpid = jobs[i].pid; //temp pid
		if(tpid != 0){
			spid = waitpid(tpid, NULL, WNOHANG);
			if(spid > 0){
				sio_puts("\treaping child:");
				sio_putl(spid);
			}else{
				sio_puts("\ttry to reap: ");
				sio_putl(tpid);
				sio_puts(" but can't: ");
				sio_putl(spid);
			}
			sio_puts("\n");
			
			//delete tpid job from job list!
			Sigprocmask(SIG_BLOCK, &maskAll, &prevAll);
			deletejob(jobs, spid);
			Sigprocmask(SIG_SETMASK, &prevAll, NULL);
		}
	}

	//if(errno != ECHILD){
		//Sio_error("sigchld_handler>waitpid error");
	//}

	errno = oldErrno;
}

/* 
 * sigint_handler - The kernel sends a SIGINT to the shell whenver the

 *    user types ctrl-c at the keyboard.  Catch it and send it along
 *    to the foreground job.  
 */
void sigint_handler(int sig) 
{
	//sio_puts(">>> sigint! <<<");
	pid_t		pid;
	//sigset_t	maskAll, prevAll;

	// mask for all blocking
	//Sigfillset(&maskAll);

	// get pid from job list
	//Sigprocmask(SIG_BLOCK, &maskAll, &prevAll); // block all
	pid = fgpid(jobs);
	if(pid != 0){
		Kill(pid, SIGINT);
		int jid = pid2jid(pid);
		sio_puts("Job");
		sio_puts(" [");	sio_putl(jid);	sio_puts("]");
		sio_puts(" (");	sio_putl(pid);	sio_puts(")");
		sio_puts(" terminated by signal 2 \n");
	}
	//Sigprocmask(SIG_SETMASK, &prevAll, NULL);

	//exit(0);
    return;
}

/*
 * sigtstp_handler - The kernel sends a SIGTSTP to the shell whenever
 *     the user types ctrl-z at the keyboard. Catch it and suspend the
 *     foreground job by sending it a SIGTSTP.  
 */
void sigtstp_handler(int sig) 
{
	pid_t pid = fgpid(jobs);
	if(pid != 0){
		Kill(pid, SIGSTOP);
		int jid = pid2jid(pid);
		sio_puts("Job");
		sio_puts(" [");	sio_putl(jid);	sio_puts("]");
		sio_puts(" (");	sio_putl(pid);	sio_puts(")");
		sio_puts(" stopped by signal 20 \n");
	}
    return;
}

/*********************
 * End signal handlers
 *********************/


/***********************************************
 * Helper routines that manipulate the job list
 **********************************************/

/* clearjob - Clear the entries in a job struct */
void clearjob(struct job_t *job) {
    job->pid = 0;
    job->jid = 0;
    job->state = UNDEF;
    job->cmdline[0] = '\0';
}

/* initjobs - Initialize the job list */
void initjobs(struct job_t *jobs) {
    int i;

    for (i = 0; i < MAXJOBS; i++)
	clearjob(&jobs[i]);
}

/* maxjid - Returns largest allocated job ID */
int maxjid(struct job_t *jobs) 
{
    int i, max=0;

    for (i = 0; i < MAXJOBS; i++)
	if (jobs[i].jid > max)
	    max = jobs[i].jid;
    return max;
}

/* addjob - Add a job to the job list */
// ret: 0 error
// ret: 1 success
int addjob(struct job_t *jobs, pid_t pid, int state, char *cmdline) 
{
    int i;
    
    if (pid < 1)
	return 0;

    for (i = 0; i < MAXJOBS; i++) {
		if (jobs[i].pid == 0) {
			jobs[i].pid = pid;
			jobs[i].state = state;
			jobs[i].jid = nextjid++;
			if (nextjid > MAXJOBS)
				nextjid = 1;
			strcpy(jobs[i].cmdline, cmdline);
			if(verbose){
				printf("Added job [%d] %d %s\n", 
						jobs[i].jid, jobs[i].pid, jobs[i].cmdline);
			}
			return 1;
		}
    }
    printf("Tried to create too many jobs\n");
    return 0;
}

/* deletejob - Delete a job whose PID=pid from the job list */
int deletejob(struct job_t *jobs, pid_t pid) 
{
    int i;

    if (pid < 1)
	return 0;

    for (i = 0; i < MAXJOBS; i++) {
		if (jobs[i].pid == pid) {
			clearjob(&jobs[i]);
			nextjid = maxjid(jobs)+1;
			return 1;
		}
    }
    return 0;
}

/* fgpid - Return PID of current foreground job, 0 if no such job */
pid_t fgpid(struct job_t *jobs) {
    int i;

    for (i = 0; i < MAXJOBS; i++)
		if (jobs[i].state == FG)
			return jobs[i].pid;
    return 0;
}

/* getjobpid  - Find a job (by PID) on the job list */
struct job_t *getjobpid(struct job_t *jobs, pid_t pid) {
    int i;

    if (pid < 1)
		return NULL;
    for (i = 0; i < MAXJOBS; i++)
		if (jobs[i].pid == pid)
			return &jobs[i];
    return NULL;
}

/* getjobjid  - Find a job (by JID) on the job list */
struct job_t *getjobjid(struct job_t *jobs, int jid) 
{
    int i;

    if (jid < 1)
	return NULL;
    for (i = 0; i < MAXJOBS; i++)
	if (jobs[i].jid == jid)
	    return &jobs[i];
    return NULL;
}

/* pid2jid - Map process ID to job ID */
int pid2jid(pid_t pid) 
{
    int i;

    if (pid < 1)
	return 0;
    for (i = 0; i < MAXJOBS; i++)
	if (jobs[i].pid == pid) {
            return jobs[i].jid;
        }
    return 0;
}

/* listjobs - Print the job list */

void listjobs(struct job_t *jobs) 
{
    int i;
    
    for (i = 0; i < MAXJOBS; i++) {
		if (jobs[i].pid != 0) {
			printf("[%d] (%d) ", jobs[i].jid, jobs[i].pid);
			switch (jobs[i].state) {
			case BG: 
				printf("Running ");
				break;
			case FG: 
				printf("Foreground ");
				break;
			case ST: 
				printf("Stopped ");
				break;
			default:
				printf("listjobs: Internal error: job[%d].state=%d ", 
				   i, jobs[i].state);
			}
			printf("%s", jobs[i].cmdline);
		}
    }
}
/******************************
 * end job list helper routines
 ******************************/


/***********************
 * Other helper routines
 ***********************/

/*
 * usage - print a help message
 */
void usage(void) 
{
    printf("Usage: shell [-hvp]\n");
    printf("   -h   print this message\n");
    printf("   -v   print additional diagnostic information\n");
    printf("   -p   do not emit a command prompt\n");
    exit(1);
}



/*
 * sigquit_handler - The driver program can gracefully terminate the
 *    child shell by sending it a SIGQUIT signal.
 */
void sigquit_handler(int sig) 
{
    printf("Terminating after receipt of SIGQUIT signal\n");
    exit(1);
}

// my own test function...
void utest(void)
{

cputs(YELLOW,"\n----print bg info----");
	eval("/bin/echo print bg info & \n");
cputs(YELLOW,"\n----------------------------------------------|");

/*
cputs(YELLOW,"\n----eval('&') is exceptional----");
	eval("&\n");
cputs(YELLOW,"\n----------------------------------------------|");

cputs(YELLOW,"\n----builtin_cmd input= \\n then segfault? wtf?----");
	eval("\n");
cputs(YELLOW,"\n----------------------------------------------|");
	//FG child가 reap되는지 확인한다.
cputs(YELLOW,"\n--------shMustReapFgChild--------");
	int result;
	eval("./myspin 2 ");
	result = jobs[0].pid;
			printf("fg job pid: %d \n", result);
			system("ps -ef|grep defunct");
			listjobs(jobs);
			//system("ps -ef|grep defunct");
			sleep(2);
			system("ps -ef|grep defunct");
	result = waitpid(jobs[0].pid, NULL, WNOHANG);
			listjobs(jobs);
			printf(">>WNOHANG? %d \n", result);
	printf("job pid = %d\n", jobs[0].pid);

	ASSERT_NEQ( jobs[0].pid, result, "child is not reaped!" );
	ASSERT( areJobsEmpty(jobs), "jobs are not cleared!" );
cputs(YELLOW,"\n----------------------------------------------|");

cputs(YELLOW,"\n--------shMustReapBgChild--------");
	eval("./myspin 1 &\n");
			//system("ps -ef|grep defunct");
			sleep(2);
			//system("ps -ef|grep defunct");
			printf(">>WNOHANG? %d \n", result);
	result = waitpid(jobs[0].pid, NULL, WNOHANG);

	ASSERT_NEQ( jobs[0].pid, result, "child is not reaped!" );
	ASSERT( areJobsEmpty(jobs), "jobs are not cleared!" );
cputs(YELLOW,"\n----------------------------------------------|");

*/
cputs(YELLOW,"\n--------shMustReapMultipleBgChildren--------");
	eval("./myspin 1 & \n");
	eval("./myspin 1 & \n");
	eval("./myspin 1 & \n");
	eval("./myspin 1 & \n");
	sleep(10);	//sleep isn't work!
	//ASSERT( areJobsEmpty(jobs), "jobs are not cleared!" );
	//but... so... is this test wrong? maybe?
cputs(YELLOW,"\n----------------------------------------------|");
/*
cputs(YELLOW,"\n-----add job into jobs(reference)-----");
	pid_t tpid = 11;
	addjob(jobs, tpid, UNDEF, "test");
	listjobs(jobs);
	deletejob(jobs, tpid);	//clear
	listjobs(jobs);	
cputs(YELLOW,"\n----------------------------------------------|");

cputs(YELLOW,"\n--------deleteAllJobs del all job(s) in jobs--------");
	addjob(jobs, 1, FG, "test");
	addjob(jobs, 2, FG, "test");
	addjob(jobs, 3, FG, "test");
	listjobs(jobs);
	deleteAllJobs(jobs);
	ASSERT_EQ( areJobsEmpty(jobs), 1, "some uncleared jobs left.");
	//because clearjob func, deleteAllJobs but jobs are not zero...
	//clearjob clear cmdline with just one '\0' allocation.
cputs(YELLOW,"\n----------------------------------------------|");

//is jobs all deleted?
cputs(YELLOW,"\n--------when all zero, isAllZero return 1--------");
	struct job_t arr[5] = {{0},};
	if(isAllZero(arr,5) != 1)
		cputs(RED,"it is all zero memory!");
cputs(YELLOW,"\n----------------------------------------------|");

cputs(YELLOW,"\n--------when not all zero, isAllZero return 0-------");
	struct job_t arr2[3] = {{5},};
	if(isAllZero(arr2,3) != 0)
		cputs(RED,"it is not all zero memory!");
cputs(YELLOW,"\n----------------------------------------------|");

cputs(YELLOW,"\n--------not all zero, isAllZero ret = 0--------");
	struct job_t arr3[10];
	arr3[9].jid = 10;
	ASSERT_EQ( isAllZero(arr3, 10), 0, "incorrect: arr3 is not all zero." );
cputs(YELLOW,"\n----------------------------------------------|");

*/
//are job add operations correct?
cputs(YELLOW,"\n------exec lasting bg job will be added jobs------");
	eval("./myspin 1 &\n");
	ASSERT_NOT( areJobsEmpty(jobs), "./myspin wasn't allocated." );
	ASSERT_EQ( jobs[0].state, BG, "state of bg job must be BG=2" );
cputs(YELLOW,"\n----------------------------------------------|");

cputs(YELLOW,"\n--------more bg jobs and allocations--------");
	deleteAllJobs(jobs);
	for(int i = 0; i < 5; i++){
		eval("./myspin 1 &\n");
		// why this child never be reaped????? why???
	}
	for(int i = 0; i < 5; i++){
		ASSERT_EQ( jobs[i].state, BG, "state of bg job must be BG=2" );
	}
cputs(YELLOW,"\n----------------------------------------------|");

cputs(YELLOW,"\n----fg ended then sigchld ?----");
	eval("/bin/echo [fg job end.. then?] \n");
cputs(YELLOW,"\n----------------------------------------------|");

cputs(YELLOW,"\n----when fg job ended, that job must be deleted----");
	//for(int i = 0; i < MAXJOBS; i++){
		//printf("%d: %d %d %d %s \n", 
				//i,
				//jobs[i].pid, jobs[i].jid, 
				//jobs[i].state, jobs[i].cmdline);
	//}
	deleteAllJobs(jobs);
	eval("/bin/echo delete job test\n");
	printf(">> %d <<", areJobsEmpty(jobs));
	ASSERT( areJobsEmpty(jobs), "ended job must be deleted.");
cputs(YELLOW,"\n----------------------------------------------|");

	//for(int i = 0; i < MAXJOBS; i++){
		//printf("%d: %d %d %d %s \n", 
				//i,
				//jobs[i].pid, jobs[i].jid, 
				//jobs[i].state, jobs[i].cmdline);
	//}

	// sigint test
cputs(YELLOW,"\n----sigint kill only fg jobs, not bg jobs.----");
	eval("./myspin 4 &\n");
	ASSERT_NOT( areJobsEmpty(jobs), "bg job didn't allocated in jobs!" );
	eval("./myspin 4 &\n");
	eval("./myspin 4 &\n");
	pid_t thisPid = getpid();

	puts("");
	listjobs(jobs);

	kill(0, 2); //sigint

	puts("");
	listjobs(jobs);
	ASSERT_NOT( areJobsEmpty(jobs), "sigint killed bg jobs! incorrect!" );

cputs(YELLOW,"\n----------------------------------------------|");

	kill(0, SIGINT); //sigint
	// all terminated children has been reaped?
	//eval("/bin/ps\n");

	//while(1);
	// end of test.
	eval("quit\n"); 
}

// mine
void deleteAllJobs(struct job_t jobs[])
{
	for(int i = 0; i < MAXJOBS; i++){
		clearjob( &jobs[i] );
	}
}

int isAllZero(struct job_t* arr, size_t size)
{
	struct job_t zeroArr[size]; 
	size_t memsize = size*sizeof(struct job_t);
	memset(zeroArr, 0, memsize);
	return !memcmp((void*)arr, (void*)zeroArr, memsize);
}

// ret
// 1	job arr is empty.
// 0	job arr is not empty.
int areJobsEmpty(struct job_t* arr)
{
	//for(int i = 0; i < MAXJOBS; i++){
		//printf("%d: %d %d %d %s \n", 
				//i,
				//jobs[i].pid, jobs[i].jid, 
				//jobs[i].state, jobs[i].cmdline);
	//}
	for(int i = 0; i < MAXJOBS; i++){
		if(jobs[i].pid != 0){
					//printf("?? %d ??", i);
					//printf("pid: %d ??", jobs[i].pid);
			return 0;
		}
		if(jobs[i].jid != 0){
					//puts("jid");
			return 0;
		}
		if(jobs[i].state != 0){
					//puts("state");
			return 0;
		}
		if(jobs[i].cmdline[0] != '\0'){
					//puts("cmd");
			return 0;
		}
	}
	return 1;
}
/*--------------------------*/
#ifndef RELEASE

#include <criterion/criterion.h>

#define lxEXPECT_EQ(actual,expected)\
	cr_expect_eq((actual),(expected),\
	#actual":0x%lX != 0x%lX:"#expected"\n",(int64_t)(actual),(int64_t)(expected))

#define lxASSERT_EQ(actual,expected)\
	cr_assert_eq((actual),(expected),\
	#actual":0x%lx != 0x%lx:"#expected"\n",(int64_t)(actual),(int64_t)(expected))

#define dASSERT_EQ(actual,expected)\
	cr_assert_eq((actual),(expected),\
	#actual":%d != %d:"#expected"\n",(int32_t)(actual),(int32_t)(expected))

#define dASSERT_NEQ(actual,expected)\
	cr_assert_neq((actual),(expected),\
	#actual":%d == %d:"#expected" -must be not equal\n",(int32_t)(actual),(int32_t)(expected))

#define dEXPECT_EQ(actual,expected)\
	cr_expect_eq((actual),(expected),\
	#actual":%d != %d:"#expected"\n",(int32_t)(actual),(int32_t)(expected))

#define pASSERT_EQ(actual,expected)\
	cr_assert_eq((actual),(expected),\
	#actual":%p != %p:"#expected"\n",(void**)(actual),(void**)(expected))


Test(builtin_cmd, ifCmdIsntBuiltInThenReturn0){
	cr_skip_test("nope");
	//given
	char*	cmdline			= "nope";
	char*	argv[MAXARGS];	
	//when
	parseline(cmdline, argv);
	//then
	dASSERT_EQ( builtin_cmd(argv), 0 );
}

/*
// why signal handler cannot be called???
// can't test signal with criterion!
Test(eval, shMustReapBgChild){
	sigset_t	maskAll, maskOne, prevOne;
	// mask for all blocking
	Sigfillset(&maskAll);
	// mask for SIGCHLD blocking
	Sigemptyset(&maskOne);
	Sigaddset(&maskOne, SIGCHLD);
	Sigprocmask(SIG_SETMASK, &prevOne, NULL); 

	// waitpid를 WNOHANG와 함께 쓰면
	// wait할 child가 없으면 ret: 0
	// wait할 child가 있으면 ret: 그 child의 pid
	int status, result;
	eval("./myspin 1 & ");
			system("ps -ef|grep defunct");
	result = waitpid(dbgpid, &status, WNOHANG);
				printf(">>WNOHANG? %d \n", result);
			sleep(2);
			system("ps -ef|grep defunct");
	result = waitpid(dbgpid, &status, WNOHANG);
				printf(">>WNOHANG? %d \n", result);
	printf("dbgpid = %d\n", dbgpid);
	dASSERT_NEQ(result, dbgpid);
}

Test(eval, shMustReapFgChild){
	int status, result;
	eval("./myspin 1 ");
			system("ps -ef|grep defunct");
	result = waitpid(dbgpid, &status, WNOHANG);
				printf(">>WNOHANG? %d \n", result);
			sleep(2);
			system("ps -ef|grep defunct");
	result = waitpid(dbgpid, &status, WNOHANG);
				printf(">>WNOHANG? %d \n", result);
	printf("dbgpid = %d\n", dbgpid);
	dASSERT_NEQ(result, dbgpid);
}
*/
#endif
/*--------------------------*/
