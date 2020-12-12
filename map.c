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
#include <string.h>
#include <unistd.h>
#include <ctype.h>

char *self;
int mapf;
extern char *optarg;
extern int optind, optopt, opterr;

void help() {
	printf("Usage:  %s [options] '<commandline>'\n", self);
	printf("Options:  -i <variable>:  Index variable name, default: ");
	printf("i\n          -m <variable>:  Mapping variable name, ");
	printf("default: m\n  For each line of stdin, the index variable is set ");
	printf("to the line number\n  (starting at 0) and the mapping variable is ");
	printf("set to the content of the line,\n  and each <commandline> ");
	printf("(that can use these variables) gets executed. ");
	if (mapf) {
		printf("In mapf\n  (unlike map), the mapping stops on a non-zero ");
		printf("returncode of a commandline.\n");
	} else {
		printf("In map\n  (unlike mapf), all lines of stdin get mapped ");
		printf("regardless of any returncodes.\n");
	}
}

int bashvar(char *var) {
	if (*var != '_' && !isalpha(*var)) return 0;
	while (*(++var))
		if (*var != '_' && !isalnum(*var)) return 0;
	return 1;
}

int main(int argc, char **argv) {
	self = strrchr(argv[0], '/');
	if (!self) self = argv[0];
	else self++;
	mapf = strcmp("map", self);
	opterr = 0;
	int opt, error = 0;
	char *ivar = "i", *mvar = "m";
	while ((opt = getopt(argc, argv, "hi:m:")) != -1) {
		switch (opt) {
			case 'i':
			case 'm':
				if (!bashvar(optarg)) {
					fprintf(stderr, "Error: Invalid variable name: '%s'\n\n", optarg);
					error = 1;
					break;
				}
				if (opt == 'm') mvar = optarg;
				else ivar = optarg;
				break;
			case '?':
				error = 2;
				if (optopt == 'i')
					fprintf(stderr, "Error: Index variable name missing after -i\n\n");
				else if (optopt == 'm')
					fprintf(stderr, "Error: Mapping variable name missing after -m\n\n");
				else
					fprintf(stderr, "Error: unknown flag -%c\n\n", optopt);
				break;
			case 'h':
				help();
				 return 0;
			default:
				error = 3;
				fprintf(stderr, "Error: missing <commandline> argument\n\n");
		}
	}
	if (!strcmp(ivar, mvar)) {
		error = 4;
		fprintf(stderr, "Error: index and mapping variable same: '%s'\n\n", ivar);
	}
	if (error) {
		help();
		return error;
	}

	char *commandline;
	if (optind == argc) {
		fprintf(stderr, "Error: missing <commandline> argument\n\n");
		help();
		return 5;
	}
	else commandline = argv[optind++];
	if (optind < argc) {
		fprintf(stderr, "Error: too many arguments\n\n");
		help();
		return 6;
	}

	char cmd[strlen(commandline)+16];
	sprintf(cmd, "/bin/bash -c '%s'", commandline);

	int ret = 255, i = 0;
	char istr[11];
	char *line = NULL;
	size_t lcap = 0;
	ssize_t llen;
	while ((llen = getline(&line, &lcap, stdin)) > 0) {
		if (llen > 1) {
			if (line[llen-1] == '\n') line[llen-1] = '\0';
			setenv(mvar, line, 1);
			sprintf(istr, "%d", i++);
			setenv(ivar, istr, 1);\
			ret = system(cmd);
			if (mapf && ret) break;
		}
	}
	return ret;
}
