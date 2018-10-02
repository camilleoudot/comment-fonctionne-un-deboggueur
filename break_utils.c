/*
 * Break - a software debugger PoC
 *
 * Copyright 2018 Orange
 * <camille.oudot@orange.com>
 *
 */

#include <signal.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/ptrace.h>
#include <sys/user.h>

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
	uintptr_t a = (uintptr_t) address;
	unsigned i;
	union word w;
	unsigned pre;

	pre = (4 - (a % 4)) % 4;

	if (pre) {
		w.l = ptrace(PTRACE_PEEKDATA, pid, a & ~3u);

		for (i = pre; i > 0 && len > 0; i--, len--) {
			switch (i) {
			case 3:
				*buf++ = w.b._1;
				break;
			case 2:
				*buf++ = w.b._2;
				break;
			case 1:
				*buf++ = w.b._3;
				break;
			}
		}

		a = (a & ~3u) + 4;
	}

	for (i = 0; i < len / 4; i++) {
		w.l = ptrace(PTRACE_PEEKDATA, pid, a);
		*buf++ = w.b._0;
		*buf++ = w.b._1;
		*buf++ = w.b._2;
		*buf++ = w.b._3;
		a += 4;
	}
	len %= 4;

	if (len) {
		w.l = ptrace(PTRACE_PEEKDATA, pid, a);
		switch (len) {
		case 3:
			buf[2] = w.b._2;
		case 2:
			buf[1] = w.b._1;
		case 1:
			buf[0] = w.b._0;
		}
	}
}

void poke_mem(pid_t pid, void *address, const unsigned char *buf, size_t len) {
	uintptr_t a = (uintptr_t) address;
	unsigned i;
	union word w;
	unsigned pre;

	pre = (4 - (a % 4)) % 4;

	if (pre) {
		w.l = ptrace(PTRACE_PEEKDATA, pid, a & ~3u);
		for (i = pre; i > 0 && len > 0; i--, len--) {
			switch (i) {
			case 3:
				w.b._1 = *buf++;
				break;
			case 2:
				w.b._2 = *buf++;
				break;
			case 1:
				w.b._3 = *buf++;
				break;
			}
		}

		ptrace(PTRACE_POKEDATA, pid, a & ~3u, w.l);

		a = (a & ~3u) + 4;
	}

	for (i = 0; i < len / 4; i++) {
		w.b._0 = *buf++;
		w.b._1 = *buf++;
		w.b._2 = *buf++;
		w.b._3 = *buf++;

		ptrace(PTRACE_POKEDATA, pid, a, w.l);

		a += 4;
	}

	len %= 4;

	if (len) {
		w.l = ptrace(PTRACE_PEEKDATA, pid, a);
		switch (len) {
		case 3:
			w.b._2 = buf[2];
			/* FALLTHRU */
		case 2:
			w.b._1 = buf[1];
			/* FALLTHRU */
		case 1:
			w.b._0 = buf[0];
		}

		ptrace(PTRACE_POKEDATA, pid, a, w.l);
	}
}

void print_registers(pid_t tracee) {
	struct user_regs_struct regs;
	if (ptrace(PTRACE_GETREGS, tracee, NULL, &regs ) != 0) {
		perror("PTRACE_GETREGS");
	} else {
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
		printf("DEBUGGER: child process exited with status %d\n",
				WEXITSTATUS(wstatus));
	}

	if (WIFSIGNALED(wstatus)) {
		printf("DEBUGGER: child process exited due to signal %d (%s)\n",
				WTERMSIG(wstatus), sig_name(WTERMSIG(wstatus)));
		if (WCOREDUMP(wstatus)) {
			puts("a core dump was generated");
		}
	}

	if (WIFSTOPPED(wstatus)) {
		printf("DEBUGGER: child process received signal %d (%s)\n",
				WSTOPSIG(wstatus), sig_name(WSTOPSIG(wstatus)));
	}

	if (WIFCONTINUED(wstatus)) {
		puts("child received SIGCONT");
	}
}

int wait_for_signal(pid_t pid, int sig, int *wstatus) {
	if (waitpid(pid, wstatus, 0) > 0) {
		if (WIFSTOPPED(*wstatus) && WSTOPSIG(*wstatus)) {
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
