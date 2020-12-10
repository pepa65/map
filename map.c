// map.c
//
// Copyright (c) 2018 - Michel Martens <mail at soveran dot com>
//               2020 - https://gitlab.com/pepa65/map <pepa65@passchier.net>
//
// All rights reserved.
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are
// met:
//  *  Redistributions of source code must retain the above copyright
//     notice, this list of conditions and the following disclaimer.
//  *  Redistributions in binary form must reproduce the above copyright
//     notice, this list of conditions and the following disclaimer in the
//     documentation and/or other materials provided with the distribution.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
// "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
// LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
// A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
// HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
// SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
// LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
// DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
// THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
// (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
// OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

int main(int argc, char **argv) {
	char *self = argv[0]+strlen(argv[0])-1;
	while (*(self-1) != '/') --self;
	int mapf = strcmp("map", self);
	if (argc == 1 || argc > 3) {
		printf("Usage: %s [<variable>] '<commandline>'\n", self);
		printf("  Each line of stdin gets assigned to <variable> (default: m) ");
		printf("in succession,\n  and the <commandline> (using the variable) ");
		printf("gets executed. ");
		if (mapf) {
			printf("In mapf\n  (unlike map), the mapping stops on a non-zero ");
			printf("returncode of a commandline.\n");
		} else {
			printf("In map\n  (unlike mapf), all lines of stdin get mapped ");
			printf("regardless of any returncodes.\n");
		}
		exit(1);
	}
	char *line = NULL;
	size_t lcap = 0;
	ssize_t llen;
	char *cmd = malloc(65536);
	char *commandline;
	char *variable;
	int ret = 0;
	if (argc == 2) {
		variable = "m";
		commandline = argv[1];
	} else {
		variable = argv[1];
		commandline = argv[2];
	}
	sprintf(cmd, "/bin/bash -c '%s'", commandline);
	while ((llen = getline(&line, &lcap, stdin)) > 0) {
		if (llen > 1) {
			if (line[llen-1] == '\n') line[llen-1] = '\0';
			setenv(variable, line, 1);
			if (mapf && (ret = system(cmd))) break;
		}
	}
	free(cmd);
	return ret;
}
