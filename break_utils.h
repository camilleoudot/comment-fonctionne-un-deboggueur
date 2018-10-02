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

void print_wait_status_infos(int wstatus);
int wait_for_signal(pid_t pid, int sig, int *wstatus);
void peek_mem(pid_t pid, void *address, unsigned char *buf, size_t len);
void poke_mem(pid_t pid, void *address, const unsigned char *buf, size_t len);
void print_registers(pid_t tracee);
void *prompt_address(char *prompt);

#endif /* BREAK_UTILS_H_ */
