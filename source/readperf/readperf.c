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
#include    "perffile/processPrinter.h"
#include    "perffile/perffile.h"
#include    "perffile/records.h"
#include    "decode/buildstat.h"
#include    "perffile/overviewPrinter.h"


#define OVERVIEW_LOG_NAME   "overview.csv"
static FILE *overview_file = NULL;

#define SUMMARY_LOG_NAME   "summary.csv"
static FILE *summary_file = NULL;


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
    
    
    if( !read_perf_file( fd ) ){
        print_last_error();
        return -1;
    }
    
    summary_file  = fopen( SUMMARY_LOG_NAME, "w+" );
    print_process_init( summary_file );
    overview_file = fopen( OVERVIEW_LOG_NAME, "w+" );
    overviewInit( overview_file );
    buildstat( &orderTree );
    fclose( summary_file );
    
    fclose( overview_file );
    
    FILE    *stat = fopen( STAT_LOG_NAME, "w+" );
    print_type_summary( stat );
    fclose( stat );
    
    FILE *res  = fopen( "results.csv", "w+" );
    func_print_detailed( res );
    fclose( res );
    
    func_print_overview( stdout );
    
    return 0;
}
