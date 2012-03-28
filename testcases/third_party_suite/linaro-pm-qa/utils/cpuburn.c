/*******************************************************************************
 * PM-QA validation test suite for the power management on Linux
 *
 * Copyright (C) 2011, Linaro Limited.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 *
 * Contributors:
 *     Daniel Lezcano <daniel.lezcano@linaro.org> (IBM Corporation)
 *       - initial API and implementation
 *
 *******************************************************************************/

#define _GNU_SOURCE
#include <sched.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <signal.h>
#include <unistd.h>
#include <regex.h>
#include <sys/types.h>
#include <sys/time.h>
#include <sys/resource.h>

int main(int argc, char *argv[])
{
	regex_t reg;
	const char *regex = "cpu[0-9].*";
	char **aargv = NULL;
	regmatch_t m[1];
	cpu_set_t cpuset;
	int i;

	if (argc == 1) {
		fprintf(stderr, "%s <cpuN> [cpuM] ... \n", argv[0]);
		return 1;
	}

	aargv = &argv[1];

	if (regcomp(&reg, regex, 0)) {
		fprintf(stderr, "failed to compile the regex\n");
		return 1;
	}

	CPU_ZERO(&cpuset);

	for (i = 0; i < (argc - 1); i++) {

		char *aux;
		int cpu;

		if (regexec(&reg, aargv[i], 1, m, 0)) {
			fprintf(stderr, "'%s' parameter not recognized, " \
				"should be cpu[0-9]\n", aargv[i]);
			return 1;
		}

		aux = aargv[i] + 3;
		cpu = atoi(aux);

		CPU_SET(cpu, &cpuset);
	}

	if (sched_setaffinity(0, sizeof(cpuset), &cpuset)) {
		perror("sched_setaffinity");
		return 1;
	}

	if (setpriority(PRIO_PROCESS, 0, -20) < 0) {
		perror("setpriority");
		return 1;
	}

	for (;;);

	return 0;
}
