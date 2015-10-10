/*
 * .Oo DERRICK - A Simple Network Stream Recorder
 * Copyright (C) 2011-2012 Konrad Rieck (konrad@mlsec.org)
 * Derrick is licensed under the new BSD License.
 * --
 * Main file
 */

#include "config.h"
#include "common.h"
#include "utils.h"
#include "derrick.h"
#include "net.h"
#include "log.h"

/* Global variables */
int verbose = FALSE;
int merge_tcp_payloads = FALSE;
int max_tcp_bytes = MAX_TCP_BYTES;
int max_log_lines = MAX_LOG_LINES;
dmode_t dmode = ENCODED;


/*Some configure paramters or thresholds by user */
char * write_file = NULL; // add by syf
char *cs_log_file  = NULL;	//add by syf
char *sc_log_file = NULL;	//add by syf


/* Some thresholds with default values can be set*/
int max_num_first_bytes_flow = 64; // The maximum threshold of the number of bytes in payload for each direction flow
int max_num_first_bytes_pkt = 64;  // The maximum threshold of the number of bytes in payload for each packet
int max_num_first_pkts = 3;  // The maximum threshold of the number of packets for each dierction flow
int max_num_samples = 1000;    // We only want to save the maximum of the samples (valid and writed samples), for tcp and udp
int max_num_cs_samples = 500;    // Only for tcp flow
int max_num_sc_samples = 500;    // Only for tcp flow


/* Some statistic information by program, e.g., the number of flows/packets/bytes, etc.*/
int num_total_tcp_connections; // Conuter the number of the tcp connections(sessions)
int num_total_flows;  // Conuter the number of unidirectional flow (not bidirectional flow)
int num_total_pkts;  // Counter the number of pkts(processed by program, not the true number of packets from pcap file) 
 

/* Local variables */
static char *read_file = NULL;
static char *device = NULL;
static char *log_file = NULL;
static char *pcap_filter = PCAP_FILTER;


/**
 * Print usage of command line tool
 */
static void print_usage(void)
{
    printf
        ("Usage: derrick [-mvVhn] [-i interface] [-r file] [-f expression] [-l file]\n"
         "               [-b bytes] [-d mode] [-t lines] [-o file]\n"
         "Options:\n"
         "  -i: Network interface to listen on.\n"
         "  -r: Read packets from tcpdump file.\n"
         "  -f: Filter expression for PCAP library. Default: \"%s\"\n"
         "  -l: Write output to compressed log file instead of stdout.\n"
         "  -b: Maximum number of bytes in TCP stream. Default: %u\n"
         "  -m: Merge assembled TCP payloads.\n"
         "  -d: Display mode: encoded, ascii or hex\n"
         "  -t: Maximum number of log lines for rotation. Default: %u\n"
         "  -v: Increase verbosity.\n" "  -V: Print version and copyright.\n"
         "  -h: Print this help screen.\n"
	 "  -n: The number of bytes for every flow payload.\n"
	 "  -o: The prefix of output file name.\n"
	 "  -c: Write the flow payloads from client to server to the cs_log_file\n"
	 "  -s: Write the flow payloads from server to client to the sc_log_file\n"
	 , pcap_filter, max_tcp_bytes,
         max_log_lines);
}

/**
 * Print version of Derrick.
 */
static void print_version()
{
	printf("This program is based on the derrick.\n");
	/*
    printf(".Oo DERRICK - A Simple Network Stream Recorder\n"
           "Copyright (C) 2011-2012 Konrad Rieck (konrad@mlsec.org)\n"
           "Derrick is licensed under the new BSD License.\n");
	*/
}

/**
 * Parse command line options
 * @param argc Number of arguments
 * @param argv Argument values
 */
static void parse_options(int argc, char **argv)
{
    int ch;
    while ((ch = getopt(argc, argv, "mvVhd:t:l:b:i:r:f:n:N:M:o:c:s:C:S:B:")) != -1) {
        switch (ch) {
        case 'd':
            if (!strcasecmp(optarg, "ascii")) {
                dmode = ASCII;
            } else if (!strcasecmp(optarg, "encoded")) {
                dmode = ENCODED;
            } else if (!strcasecmp(optarg, "hex")) {
                dmode = HEX;
            } else if (!strcasecmp(optarg, "bin")) { // add by syf
                dmode = BIN;
            } else {
                warning("Unknown mode '%s'. Using 'encoded'.", optarg);
                dmode = ENCODED;
            }
            break;
        case 't':
            max_log_lines = atoi(optarg);
            break;
        case 'm':
            merge_tcp_payloads = TRUE;
            break;
        case 'f':
            pcap_filter = optarg;
            break;
        case 'b':
            max_tcp_bytes = atoi(optarg);
            break;
        case 'i':
            device = optarg;
            break;
        case 'r':
            read_file = optarg;
            break;
        case 'l':
            log_file = optarg;
            break;
	case 'n':   // add syf
	    max_num_first_bytes_flow = atoi(optarg);
	    break;
	case 'N':   // add syf
	    max_num_first_bytes_pkt = atoi(optarg);
	    break;
	case 'M':   // add syf
	    max_num_first_pkts = atoi(optarg);
	    break;
	case 'o':   // add syf
	    write_file = optarg;
	    break;
	case 'c':   // add syf
	    cs_log_file = optarg;
	    break;
	case 's':   // add syf
	    sc_log_file = optarg;
	    break;
	case 'C':   // add syf
	    max_num_cs_samples = atoi(optarg);
	    break;
	case 'S':   // add syf
	    max_num_sc_samples = atoi(optarg);
	    break;
	case 'B':   // add syf
	    max_num_samples = atoi(optarg);
	    break;
        case 'v':
            verbose++;
            break;
        case 'V':
            print_version();
            exit(EXIT_SUCCESS);
            break;
        case 'h':
        case '?':
            print_usage();
            exit(EXIT_SUCCESS);
            break;
        }
    }

    argc -= optind;
    argv += optind;

    if ((device && read_file) || (!device && !read_file))
        fatal("Specify either a device (-i) or a tcpdump file (-r).");
}

/**
 * Exit derrick.
 * @param s Signal number
 */
void derrick_exit(int s)
{
    /* Exit network monitoring */
    net_exit();

    /* Close log file */
    log_close();

    /* Point of no return  */
    exit(EXIT_SUCCESS);
}

/**
 * Init derrick.
 */
void derrick_init()
{
    /* Install signal handlers */
    signal(SIGTERM, derrick_exit);
    signal(SIGINT, derrick_exit);
    signal(SIGQUIT, derrick_exit);

    /* Init network monitoring */
    net_init(device, read_file, pcap_filter);

    /* Open compressed log */
    log_open(log_file);
}

/**
 * Main loop of derrick.
 */
void derrick_run()
{
    nids_run();
}

/*
 * Main function
 */
int main(int argc, char **argv)
{
    parse_options(argc, argv);  /* Start main loop */

    derrick_init();
    derrick_run();
    derrick_exit(0);

    return EXIT_SUCCESS;
}
