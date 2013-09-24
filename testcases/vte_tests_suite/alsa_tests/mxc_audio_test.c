#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
/*
 * This test app is to test channel swap in the audio driver. like ssi, esai.
 * default to read the output data from arecorder then check the amplitude.
 * if record data is different with the playback data, then channel swap is happened.
 *
 * assume the channel is 2, bit is 16, the input signal is square wave, raw data.
 *
 * command
 * audio_chan_swap threshold_high threshold_low inputfile
 */

static ssize_t safe_read(int fd, void *buf, size_t count) {
	ssize_t result = 0, res;
	while (count > 0) {
		if ((res = read(fd, buf, count)) == 0)
			break;
		if (res < 0) {
			printf("read error\n");
			return result > 0? result: res;
		}
		count -=res;
		result += res;
		buf = (char*) buf + res;
	}
	return result;
}


int main(int argc, char **argv) {
	void  *buffer;
	short *data;
	short  left,right;
	int    size;
	int    i;
	short  threshold_high = 16384;
	short  threshold_low  = 16384;
	char  *name;
	int    fd = -1;
	int    swap_start =0;
	int    swap_end= 0;
	int   swap_happen = 0;
	int   buffersize = 2048;

	if (argc < 3) {
		threshold_high = 5000;
		threshold_low  = 100;
	} else {
		threshold_high = atoi(argv[1]);
		threshold_low  = atoi(argv[2]);
	}

	if (argc < 4) {
		fd = fileno(stdin);
		name = "stdin";
	} else {
		name = argv[3];
		fd = open(name, O_RDONLY,0);
		if (fd == -1) {
			printf("can not open input file\n");
			return 0;
		}
	}

	buffer = malloc(buffersize);
	if (buffer == NULL) {
		printf("cat not alloc memory\n");
		return 0;
	}
	data = (short *)buffer;

	/* drop fist about 5s data*/
	for( i=0; i<200 ; i++)
		size = safe_read(fd, buffer, buffersize);

	size = safe_read(fd, buffer, buffersize);
	while (size > 0) {
		for(i = 0; i < size/4; i++) {
		 	left  = *data++;
			right = *data++;
			if ( left < 0 ) left  = -left;
			if (right < 0 ) right = - right;
			if (left > threshold_high && right < threshold_low) {
				//printf("left %d, right %d\n",left,right);
				//printf("channel swap!!\n");
				if(!swap_happen && !swap_start)  {swap_happen = 1; swap_start = 1;}
			}
			if (swap_start == 1 && right > threshold_high && left < threshold_low) {
				swap_start  = 0;
				swap_happen = 1;
			}

			if (swap_happen) {
				if( swap_start) printf("channel swap!\n");
				else printf("channel swap back!\n");
				swap_happen = 0;
			}
		}
		size = safe_read(fd, buffer, buffersize);
		data = (short*)buffer;
	}
	close(fd);
	free(buffer);
	return 0;
}
