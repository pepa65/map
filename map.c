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

int main(int argc, char **argv) {
	char *self = strrchr(argv[0], '/');
	if (!self) self = argv[0];
	else self++;

	int mapf = strcmp("map", self);
	if (argc != 2) {
		printf("Usage: %s '<commandline>'\n", self);
		printf("  For each line of stdin, variable i is set to the line number (starting at 0)\n  and variable m is set to the content of the line, and each <commandline>  \n  (that can refer to $i and $m) gets executed. ");
		if (mapf) {
			printf("In mapf (unlike map),\n  the mapping stops on a non-zero ");
			printf("returncode of a commandline.\n");
		} else {
			printf("In map (unlike mapf),\n  all lines of stdin get mapped ");
			printf("regardless of any returncodes.\n");
		}
		if (argc == 1) printf("\nError: missing <commandline>\n");
		else printf("\nError: too many arguments\n");
		exit(1);
	}

	char *cmd = malloc(strlen(argv[1])+16);
	sprintf(cmd, "/bin/bash -c '%s'", argv[1]);

	int ret = -1, i = 0;
	char istr[11];
	char *line = NULL;
	size_t lcap = 0;
	ssize_t llen;
	while ((llen = getline(&line, &lcap, stdin)) > 0) {
		if (llen > 1) {
			if (line[llen-1] == '\n') line[llen-1] = '\0';
			setenv("m", line, 1);
			sprintf(istr, "%d", i++);
			setenv("i", istr, 1);\
			ret = system(cmd);
			if (mapf && ret) break;
		}
	}
	free(cmd);
	return ret;
}
