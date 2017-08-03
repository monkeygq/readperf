/**
 * @file
 * Data types and functions to store and iterate the records sorted by the
 * timestamp.
 *
 * Copyright 2011 Urs Fässler, www.bitzgi.ch
 * SPDX-License-Identifier:	GPL-3.0+
 */

#ifndef _RECORDS_H
#define	_RECORDS_H

#ifdef	__cplusplus
extern "C" {
#endif


#include    <stdio.h>
#include    "../util/origperf.h"
#include    "../util/tree.h"

#define RECORD_TREE_LINK  ptl

    struct record_t {
        TREE_ENTRY(record_t)      RECORD_TREE_LINK;
        u32 pid, tid;
        u32 cpu;
        u64 time;
        u64 nr;
        u64 id;
        enum perf_event_type type;
        /*
         * PERF_RECORD_MMAP 1
         * PERF_RECORD_LOST 2
         * PERF_RECORD_COMM 3
         * PERF_RECORD_EXIT 4
         * PERF_RECORD_FORK 7
         * PERF_RECORD_SAMPLE 9
         */
    };

    struct record_mmap {
        struct record_t header;
        u64 start;
        u64 len;
        u64 pgoff;
        char filename[PATH_MAX];
    };

    struct record_mmap2 {
        struct record_t header;
        u64 start;
        u64 len;
        u64 pgoff;
        u32 maj;
        u32 min;
        u64 ino;
        u64 ino_generation;
        u32 prot;
        u32 flags;
        char filename[PATH_MAX];
    };

    struct record_comm {
        struct record_t header;
        char comm[16];
    };

    struct record_fork {
        struct record_t header;
        u32 ppid;
        u32 ptid;
    };

    struct record_sample {
        struct record_t header;
        u64 ip;
        u64 period;
    };

    typedef TREE_HEAD(_OrderTree, record_t) record_order_tree_t;
    /*
     * struct _OrderTree {
     *     struct record_t *th_root;
     *     int (*th_cmp)(struct record_t *lhs, struct record_t *rhs);
     * }
     *
     */

    record_order_tree_t init_record_order();
    bool add_record_order( record_order_tree_t *tree, struct record_t *rec );
    void iterate_order( record_order_tree_t *tree, void (*callback)(struct record_t *proc, void *data), void *data );

#ifdef	__cplusplus
}
#endif

#endif	/* _RECORDS_H */

