/*
 * Break - a software debugger PoC
 *
 * Copyright 2018 Orange
 * <camille.oudot@orange.com>
 *
 */

#define _LARGEFILE64_SOURCE
#include <signal.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/ptrace.h>
#include <sys/user.h>
#include <sys/stat.h>
#include <fcntl.h>


#include "break_utils.h"

union word {
	long l;
	struct {
		unsigned char _0; /* LSB on little endian */
		unsigned char _1;
		unsigned char _2;
		unsigned char _3; /* MSB on little endian */
	} b;
};

void peek_mem(pid_t pid, void *address, unsigned char *buf, size_t len) {
	char name[512];
	int memfd;

	sprintf(name, "/proc/%d/mem", pid);
	memfd = open(name, O_RDONLY);

	lseek64(memfd, (off64_t)address, SEEK_CUR);

	read(memfd, buf, len);

	close(memfd);
}

void poke_mem(pid_t pid, void *address, const unsigned char *buf, size_t len) {
	char name[512];
	int memfd;

	sprintf(name, "/proc/%d/mem", pid);
	memfd = open(name, O_WRONLY);

	lseek64(memfd, (off64_t)address, SEEK_CUR);

	write(memfd, buf, len);

	close(memfd);
}

int set_registers(pid_t pid, struct user_regs_struct *regs) {
	if (ptrace(PTRACE_SETREGS, pid, NULL, regs ) != 0) {
		perror("PTRACE_SETREGS");
		return 0;
	} else {
		return 1;
	}
}

void set_rip(pid_t pid, void *rip) {
	struct user_regs_struct regs;
	if (get_registers(pid, &regs)) {
		regs.rip = (unsigned long long)rip;
		set_registers(pid, &regs);
	}
}

int get_registers(pid_t pid, struct user_regs_struct *regs) {
	if (ptrace(PTRACE_GETREGS, pid, NULL, regs ) != 0) {
		perror("PTRACE_GETREGS");
		return 0;
	} else {
		return 1;
	}
}

void *get_rip(pid_t pid) {
	struct user_regs_struct regs;
	if (get_registers(pid, &regs)) {
		return (void*)regs.rip;
	} else {
		return 0;
	}
}


void print_registers(pid_t tracee) {
	struct user_regs_struct regs;

	if (get_registers(tracee, &regs)) {
		printf("    rip: 0x%016llx\n", regs.rip);
		printf("    rsp: 0x%016llx\n", regs.rsp);
		printf("    rbp: 0x%016llx\n", regs.rbp);
		printf("    rax: 0x%016llx\n", regs.rax);
		printf("    rdi: 0x%016llx\n", regs.rdi);
		printf("    rsi: 0x%016llx\n", regs.rsi);
		puts("    (...)");
	}
}

static const char *sig_name(int signum) {
	static const char *signames[] = {
		[SIGHUP   ] = "SIGHUP",
		[SIGINT   ] = "SIGINT",
		[SIGQUIT  ] = "SIGQUIT",
		[SIGILL   ] = "SIGILL",
		[SIGTRAP  ] = "SIGTRAP",
		[SIGABRT  ] = "SIGABRT",
		[SIGBUS	  ] = "SIGBUS",
		[SIGFPE	  ] = "SIGFPE",
		[SIGKILL  ] = "SIGKILL",
		[SIGUSR1  ] = "SIGUSR1",
		[SIGSEGV  ] = "SIGSEGV",
		[SIGUSR2  ] = "SIGUSR2",
		[SIGPIPE  ] = "SIGPIPE",
		[SIGALRM  ] = "SIGALRM",
		[SIGTERM  ] = "SIGTERM",
		[SIGSTKFLT] = "SIGSTKFLT",
		[SIGCHLD  ] = "SIGCHLD",
		[SIGCONT  ] = "SIGCONT",
		[SIGSTOP  ] = "SIGSTOP",
		[SIGTSTP  ] = "SIGTSTP",
		[SIGTTIN  ] = "SIGTTIN",
		[SIGTTOU  ] = "SIGTTOU",
		[SIGURG	  ] = "SIGURG",
		[SIGXCPU  ] = "SIGXCPU",
		[SIGXFSZ  ] = "SIGXFSZ",
		[SIGVTALRM] = "SIGVTALRM",
		[SIGPROF  ] = "SIGPROF",
		[SIGWINCH ] = "SIGWINCH",
		[SIGIO    ] = "SIGIO",
		[SIGPWR	  ] = "SIGPWR",
		[SIGUNUSED] = "SIGUNUSED"
	};
	return signames[signum];
}

void print_wait_status_infos(int wstatus) {
	if (WIFEXITED(wstatus)) {
		DBG("child process exited with status %d\n",
				WEXITSTATUS(wstatus));
	}

	if (WIFSIGNALED(wstatus)) {
		DBG("child process exited due to signal %d (%s)\n",
				WTERMSIG(wstatus), sig_name(WTERMSIG(wstatus)));
		if (WCOREDUMP(wstatus)) {
			puts("a core dump was generated");
		}
	}

	if (WIFSTOPPED(wstatus)) {
		DBG("child process received signal %d (%s)\n",
				WSTOPSIG(wstatus), sig_name(WSTOPSIG(wstatus)));
	}

	if (WIFCONTINUED(wstatus)) {
		puts("child received SIGCONT");
	}
}

int wait_for_signal(pid_t pid, int sig, int *wstatus) {
	if (waitpid(pid, wstatus, 0) > 0) {
		if (WIFSTOPPED(*wstatus) && WSTOPSIG(*wstatus) == sig) {
			return 1;
		}
	}
	return 0;
}

void *prompt_address(char *prompt) {
	void *ret = NULL;
	char buf[128];

	printf("%s", prompt);

	if (fgets(buf, sizeof buf, stdin)) {
		ret = (void*)strtoull(buf, NULL, 0);
	}

	return ret;
}
