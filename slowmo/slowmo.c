#include <stdio.h>
#include <stdlib.h>
/* getopt_long */
#include <getopt.h>
/* fork */
#include <unistd.h>
/* nanosleep */
#include <time.h>
/* wait */
#include <sys/types.h>
#include <sys/wait.h>
/* ptrace */
#include <sys/ptrace.h>

void usage()
{
    fprintf(stderr,"usage: slowmo -d nSEC | -s uSEC [-f] [-p PID | prg [args...]]\n");
    fprintf(stderr,"\n");
    fprintf(stderr,"Execute prg, or attach to PID, performing a delay every instruction\n");
    fprintf(stderr,"or syscall to slow execution. -s & -d cannot be used together, but one\n");
    fprintf(stderr,"or another is required.\n");
    fprintf(stderr,"\n");
    fprintf(stderr,"  -d, --delay nSEC  Delay nSEC nanoseconds per tick (Default 1ns)\n");
    fprintf(stderr,"  -s, --sys uSEC    Delay uSEC microseconds per syscall\n");
    fprintf(stderr,"  -f, --follow      Follow children of fork/vfork/exec/vexec/clone\n");
    fprintf(stderr,"  -p, --pid PID     Attach to PID instead of exec program\n");
    fprintf(stderr,"  prg [args...]     Exec prg with optional args\n");
    fprintf(stderr,"\n");
    fprintf(stderr,"  -v, --verbose     Enable verbose output\n");
    fprintf(stderr,"  -h, --help        This output\n");
}

int main(int argc,char* argv[])
{
    /* from getopt.h - for getopt_long */
    extern int optind;
    extern char* optarg;

    long int delaytime=0;
    int delaysyscalls=0;
    int follow=0;
    int verbose=0;
    /* default to no pid given */
    int pid=0;
    int signal=0;
    int status;
    int ptr_type;
    long int ptr_options;
    struct timespec nanotime;

    int opt;
    const struct option long_options[]={
        {"delay"  ,1,NULL,'d'},
        {"sys"    ,1,NULL,'s'},
        {"pid"    ,1,NULL,'p'},
        {"follow" ,0,NULL,'f'},
        {"verbose",0,NULL,'v'},
        {"help"   ,0,NULL,'h'},
        {0        ,0,0   ,0  }
    };

    while ((opt=getopt_long(argc,argv,"+d:s:p:fvh",long_options,NULL))!=EOF) {
        switch (opt) {
            case 'd':
                delaytime=strtol(optarg,NULL,0);
                delaysyscalls=0;
                if (delaytime<0) {
                    fprintf(stderr,"Huh? Time only goes one way. Quitting!\n");
                    exit(1);
                }
                break;
            case 's':
                delaytime=strtol(optarg,NULL,0)*1000;
                delaysyscalls=1;
                if (delaytime<0) {
                    fprintf(stderr,"Wait, what? Can't go back in time. Quitting!\n");
                    exit(1);
                }
                break;
            case 'p':
                pid=strtol(optarg,NULL,0);
                if (pid<=1) {
                    fprintf(stderr,"Invalid pid");
                    /* disallow slowing init, REALLY non-productive */
                    if (pid==1)
                        fprintf(stderr," (slowing 'init'==BAD!)");
                    fprintf(stderr,"\n");
                    exit(1);
                }
                break;
            case 'f':
                follow=1; break;
            case 'v':
                verbose=1; break;
            case '?':
                /* something mistyped, just bail */
                exit(1); /* doesn't return */
            case 'h':
                /* user requested help! */
            default:
                /* weird! how'd you get here? All cases covered...? */
                usage();
                exit(1); /* doesn't return */
        }
    }
    /* check to make sure either -s or -d was used */
    if (delaytime==0) {
        /* still 0, not assigned, or assigned = 0 */
        fprintf(stderr,"-s or -d required\n");
        exit(1);
    }
    /* prevent attaching & running at the same time */
    if ((optind<argc)&&(pid)) {
        fprintf(stderr,"Can't run program and attach to another at same time.\n");
        exit(1);
    }
    follow=follow;
    /* start 'prg [args]'*/
    if (pid) {
        /* attach to given pid */
        if (ptrace(PTRACE_ATTACH,pid,NULL,NULL)) {
            perror("Failed to attach to pid");
            exit(1);
        }
    } else {
        if (verbose) {
            fprintf(stderr,"Attempting fork\n");
        }
        pid=fork();
       if (pid<0) {
            /* fork failed */
            perror("fork failed");
            exit(1);
        }
        if (pid==0) {
            /* we're the child, start ptrace'ing and exec 'prg' */
            /* start ptrace */
            if (verbose) {
                fprintf(stderr,"Initializing trace\n");
            }
            if (ptrace(PTRACE_TRACEME,0,NULL,NULL)) {
                perror("TRACEME failed");
                exit(1);
            }
            /* exec prg [args] */
            execvp(argv[optind],(char* const*)(argv+optind));
            perror("Something didn't work");
            exit(1);
        }
        /* we're the parent... fall into processing loop */
    }
    /* wait for pid to trigger and process accordingly */
    ptr_options=PTRACE_O_TRACEEXIT;
    if (follow) {
        ptr_options|=
            PTRACE_O_TRACEFORK|
            PTRACE_O_TRACEVFORK|
            PTRACE_O_TRACECLONE|
            PTRACE_O_TRACEEXEC|
            PTRACE_O_TRACEVFORKDONE;
    }
    ptrace(PTRACE_SETOPTIONS,pid,NULL,ptr_options);

    /* preset type of ptrace continue */
    ptr_type=(delaysyscalls==1)?PTRACE_SYSCALL:PTRACE_SINGLESTEP;

    /* preset nanotime for delay */
    nanotime.tv_sec=0;
    nanotime.tv_nsec=delaytime;

    while (1) {
        wait(&status);
        if (WIFEXITED(status)||WIFSIGNALED(status)) {
            if (verbose) {
                fprintf(stderr,"Child exited\n");
            }
            break;
        }
        if (WIFSTOPPED(status)) {
            /* stopped by tracing? */
            signal=WSTOPSIG(status);
            if (signal==SIGTRAP) {
                /* this signal is from us */
                signal=0;
                nanosleep(&nanotime,NULL);
            } else if (signal==SIGSTOP) {
                /* fork or relatives? */
                if (verbose) {
                    fprintf(stderr,"Fork/exec of %d\n",pid);
                }
                /* read new pid of child */
                ptrace(PTRACE_GETEVENTMSG,pid,NULL,&status);
                pid=status;
                ptrace(PTRACE_SETOPTIONS,pid,NULL,ptr_options);
                if (verbose) {
                    fprintf(stderr,"Following %d\n",pid);
                }
            }
        }
        ptrace(ptr_type,pid,NULL,signal);
        signal=0;
        /* reset nanotime for delay */
        nanotime.tv_sec=0;
        nanotime.tv_nsec=delaytime;
    }

    return 0;
}
