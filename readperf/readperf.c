/**
 * @file
 * Main file, handling of input and output, starting the process.
 *
 * Copyright 2011 Urs Fässler, www.bitzgi.ch
 * SPDX-License-Identifier:	GPL-3.0+
 */
#include    <stdio.h>
#include    <linux/types.h>
#include    <linux/perf_event.h>
#include    <unistd.h>
#include    <sys/types.h>
#include    <sys/stat.h>
#include    <fcntl.h>
#include    <string.h>
#include    <stdlib.h>
#include    "util/types.h"
#include    "util/origperf.h"
#include    "util/errhandler.h"
#include    "decode/funcstat.h"
#include    "decode/processPrinter.h"
#include    "perffile/perffile.h"
#include    "perffile/records.h"
#include    "perffile/session.h"
#include    "decode/buildstat.h"
#include    "perffile/overviewPrinter.h"


#define OVERVIEW_LOG_NAME   "overview.csv"
static FILE *overview_file = NULL;

#define PROCESSES_LOG_NAME   "processes.csv"
static FILE *processes_file = NULL;


#define STAT_LOG_NAME   "stat.csv"
#define FUNC_LOG_NAME   "func.csv"

int main( int argc, char **argv ) {
    if( argc != 2 ){
        printf( "expect a file name\n" );
        return -1;
    }
    
    int fd = open( argv[1], O_RDONLY );
    if( fd < 0 ){
        perror( "open data file\n" );
        return -1;
    }
    
    
    if( !read_perf_file( fd ) ){ /* perf.data's fd */
        print_last_error();
        return -1;
    }
    
    if( get_entry_count() > 1 ){
      fprintf( stderr, "more than one event source not yet supported\n" );
      return -1;
    }
    
    struct event_type_entry *evt = get_entry_by_index( 0 );
    if( !evt->attr.freq ){
      fprintf( stderr, "period mode not yet supported\n" );
      return -1;
    }
    if( !evt->attr.mmap ){
      fprintf( stderr, "need mmap entries\n" );
      return -1;
    }
    
    /* "processes.csv" */
    processes_file  = fopen( PROCESSES_LOG_NAME, "w+" ); 

    /* add header of "processes.csv" */
    print_process_init( processes_file );

    /* overview.csv */
    overview_file = fopen( OVERVIEW_LOG_NAME, "w+" );

    /* add header of "overview.csv" */
    overviewInit( overview_file );
    /* analysis data */
    buildstat( get_record_order_tree() );/* TODO: read */
    fclose( processes_file );
    
    fclose( overview_file );
    
    FILE    *stat = fopen( STAT_LOG_NAME, "w+" ); /* stat.csv */
    /* print the number of different data type */
    print_type_summary( stat );
    fclose( stat );
    
    FILE *res  = fopen( "results.csv", "w+" );
    func_print_detailed( res );
    fclose( res );
    
    func_print_overview( stdout );    // print an overview to stdout
    
    return 0;
}
