/*
 * .Oo DERRICK - A Simple Network Stream Recorder
 * Copyright (C) 2011-2012 Konrad Rieck (konrad@mlsec.org)
 * Derrick is licensed under the new BSD License.
 * --
 * Compress log format
 */

#include "config.h"
#include "common.h"
#include "utils.h"
#include "net.h"
#include "log.h"

#define MAX_FILE_NAME_SIZE 128 

extern int max_log_lines;
extern int truncate_payload_N; // add by syf
static int sample_seqno = 1; // add by syf
extern char *write_file; // add by syf

/* Global variables */
static gzFile *gz = NULL;
static char *gzfile = NULL;
static unsigned long lines = 0;
static unsigned long rotation = 0;

/**
 * Open a compressed textfile for writing
 * @param file File name
 */
void log_open(char *file)
{
    if (!file)
        return;

    gzfile = strdup(file);;
    gz = gzopen(file, "wb");
    if (!gz)
        error("Could not create file %s", file);

    lines = 0;
    rotation = 0;
}

/**
 * Write a network payload to a file
 * @param time Timestamp
 * @param flags Flags
 * @param addr Addresses
 * @param buf Payload data
 * @param len Payload length
 */
void log_write(double time, char *flags, struct tuple4 addr, char *buf,
               int len)
{
    if(len > truncate_payload_N){  // add by syf
	len = truncate_payload_N;
    }

    char *p = payl_to_str(buf, len);

    if (!gz) {

	char * file_path = (char *) malloc (sizeof(char) * MAX_FILE_NAME_SIZE);
	memset(file_path, '\0', MAX_FILE_NAME_SIZE);
	sprintf(file_path, "%s_%d.dat", write_file, sample_seqno);
	FILE * fp = fopen(file_path, "wb");
	if (!fp) {
		perror("ERROR: Open file failure.\n");
		if(file_path) {
			free(file_path);
			file_path = NULL;
		}
		return ;
	}
	if(len != fwrite(p, 1, len, fp)) {
		perror("ERROR: Write file failure.\n");
		if(file_path) {
			free(file_path);
			file_path = NULL;
		}
		return ;
	}
	fclose(fp);
	if(file_path) {
		free(file_path);
		file_path = NULL;
	}
	sample_seqno++;

        //printf("%.3f %s %s %s\n", time, flags, addr_to_str(addr), p);
    } else {
        gzprintf(gz, "%.3f %s %s %s\n", time, flags, addr_to_str(addr), p);

        /* Sync output stream */
        if (lines++ % SYNC_INTERVAL == 0)
            gzflush(gz, Z_SYNC_FLUSH);

        /* Check for log rotation */
        if (lines >= max_log_lines) {
            rotation++;
            lines = 0;

            /* Create backup file name */
            int len = strlen(gzfile) + 10;
            char *backup = malloc(len);
            snprintf(backup, len, "%s.%lu", gzfile, rotation);

            /* Switch files */
            gzclose(gz);
            rename(gzfile, backup);
            gz = gzopen(gzfile, "wb");
        }
    }

    free(p);
}

/**
 * Close an open file
 * @param z Open file
 */
void log_close()
{
    if (gzfile)
        free(gzfile);
    if (gz)
        gzclose(gz);
    lines = 0;
}
