/*
 * Break - a software debugger PoC
 *
 * Copyright 2018 Orange
 * <camille.oudot@orange.com>
 *
 */

#ifndef BREAK_UTILS_H_
#define BREAK_UTILS_H_

#include <unistd.h>
#include <sys/user.h>

#define DBG(_fmt, ...) printf("DEBUG: " _fmt, __VA_ARGS__)

void print_wait_status_infos(int wstatus);
int wait_for_signal(pid_t pid, int sig, int *wstatus);
void peek_mem(pid_t pid, void *address, unsigned char *buf, size_t len);
void poke_mem(pid_t pid, void *address, const unsigned char *buf, size_t len);
int set_registers(pid_t pid, struct user_regs_struct *regs);
void set_rip(pid_t pid, void *rip);
int get_registers(pid_t pid, struct user_regs_struct *regs);
void *get_rip(pid_t pid);
void print_registers(pid_t tracee);
void *prompt_address(char *prompt);

#endif /* BREAK_UTILS_H_ */
