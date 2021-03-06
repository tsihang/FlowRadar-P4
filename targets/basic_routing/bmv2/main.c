/*
Copyright 2013-present Barefoot Networks, Inc.

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.
*/

/****************************************************************
 *
 * DO NOT EDIT THIS FILE: See below
 *
 * This file is copied into the target's top level directory for
 * all P4 programs. If you edit this file locally (in a P4 directory)
 * the changes will be lost. If you edit this file in targets/common,
 * the changes will apply to all targets.
 *
 * To change this behavior, edit targets/common/common.mk.
 *
 * main for thrift based protocol dependent layer based on
 * P4 program from p4-params.mk at top level of repository
 *
 ***************************************************************/

#include <errno.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/queue.h>
#include <sys/types.h>
#include <stdio.h>

#include <getopt.h>
#include <assert.h>

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include <pd/pd.h>
#include <thrift-src/pdfixed_rpc_server.h>
#include <thrift-src/pd_rpc_server.h>

static char *pd_server_str = NULL;

/**
 * The maximum number of ports to support:
 * @fixme should be runtime parameter
 */
#define PORT_COUNT 256
#define PD_SERVER_DEFAULT_PORT 9090

/**
 * Check an operation and return if there's an error.
 */
#define CHECK(op)                                                       \
    do {                                                                \
        int _rv;                                                        \
        if ((_rv = (op)) < 0) {                                         \
            fprintf(stderr, "%s: ERROR %d at %s:%d",                    \
                    #op, _rv, __FILE__, __LINE__);                      \
            return _rv;                                                 \
        }                                                               \
    } while (0)

static void
parse_options(int argc, char **argv)
{
  struct entry *np = NULL;

  while (1) {
    int option_index = 0;
    /* Options without short equivalents */
    enum long_opts {
      OPT_START = 256,
      OPT_PDSERVER,
    };
    static struct option long_options[] = {
      {"help", no_argument, 0, 'h' },
      {"pd-server", required_argument, 0, OPT_PDSERVER },
      {0, 0, 0, 0 }
    };
    int c = getopt_long(argc, argv, "h",
                        long_options, &option_index);
    if (c == -1) {
      break;
    }
    switch (c) {
    case OPT_PDSERVER:
      pd_server_str = strdup(optarg);
      break;
    case 'h':
    case '?':
      printf("Drivers! \n");
      printf("Usage: drivers [OPTION]...\n");
      printf("\n");
      printf(" --pd-server=IP:PORT Listen for PD RPC calls\n");
      printf(" -h,--help Display this help message and exit\n");
      exit(c == 'h' ? 0 : 1);
      break;
    }
  }
}

int
main(int argc, char* argv[])
{
    int rv = 0;
  
    parse_options(argc, argv);

    /*************************************************************
     * Initialize Modules. 
     ************************************************************/
    
    /* Start up the PD RPC server */
    void *pd_server_cookie;
    start_bfn_pd_rpc_server(&pd_server_cookie);
    add_to_rpc_server(pd_server_cookie);

    p4_pd_init();
    p4_pd_basic_routing_init();
    p4_pd_basic_routing_assign_device(0, "ipc:///tmp/bmv2-0-notifications.ipc",
                                      10001);

    while (1) pause();

    return rv;
}
