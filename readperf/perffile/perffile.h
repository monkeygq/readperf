/*
 * @file
 * Reads the content of the file and adds the records to its internal data
 * structure.
 *
 * Copyright 2011 Urs Fässler, www.bitzgi.ch
 * SPDX-License-Identifier:	GPL-3.0+
 */

#ifndef _PERFFILE_H
#define	_PERFFILE_H

#ifdef	__cplusplus
extern "C" {
#endif
    
#include    "records.h"
    
    bool read_perf_file( int fd );
    record_order_tree_t* get_record_order_tree();
    
    
#ifdef	__cplusplus
}
#endif

#endif	/* _PERFFILE_H */

