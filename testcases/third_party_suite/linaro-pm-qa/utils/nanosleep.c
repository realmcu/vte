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
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <time.h>

int main(int argc, char *argv[])
{
	struct timespec req = { 0 }, rem = { 0 };

	if (argc != 2) {
		fprintf(stderr, "%s <nanoseconds>\n", argv[0]);
		return 1;
	}

	req.tv_nsec = atoi(argv[1]);

	for (;;) {
		if (!nanosleep(&req, &rem))
			break;

		if (errno == EINTR) {
			req = rem;
			continue;
		}

		perror("failed to nanosleep");
		return 1;
	}

	return 0;
}
