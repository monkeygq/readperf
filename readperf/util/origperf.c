/**
 * Copyright 2011 Urs Fässler, www.bitzgi.ch
 * SPDX-License-Identifier:	GPL-3.0+
 */
#include  "origperf.h"
#include  <stdio.h>

/*
 * sample_id_all
 *     If set, then TID, TIME, ID, STREAM_ID, and CPU can 
 *     additionally be included in non-PERF_RECORD_SAMPLEs
 *
 * The layout is described by this pseudo-structure:
 *  struct sample_id {
 *      { u32 pid, tid; }   * if PERF_SAMPLE_TID set *
 *      { u64 time;     }   * if PERF_SAMPLE_TIME set *
 *      { u64 id;       }   * if PERF_SAMPLE_ID set *
 *      { u64 stream_id;}   * if PERF_SAMPLE_STREAM_ID set  *
 *      { u32 cpu, res; }   * if PERF_SAMPLE_CPU set *
 *      { u64 id;       }   * if PERF_SAMPLE_IDENTIFIER set *
 *  };
 */
static int perf_event__parse_id_sample(const union perf_event *event, u64 type, struct perf_sample *sample)
{
	const u64 *array = event->sample.array;

    /* array points to the last u64 of sample_id */
	array += ((event->header.size -
		   sizeof(event->header)) / sizeof(u64)) - 1;

    /* Assign from backward to forward */
	if (type & PERF_SAMPLE_CPU) {
		u32 *p = (u32 *)array;
		sample->cpu = *p;
		array--;
	}

	if (type & PERF_SAMPLE_STREAM_ID) {
		sample->stream_id = *array;
		array--;
	}

	if (type & PERF_SAMPLE_ID) {
		sample->id = *array;
		array--;
	}

	if (type & PERF_SAMPLE_TIME) {
		sample->time = *array;
		array--;
	}

	if (type & PERF_SAMPLE_TID) {
		u32 *p = (u32 *)array;
		sample->pid = p[0];
		sample->tid = p[1];
	}

	return 0;
}

int perf_event__parse_sample(const union perf_event *event, u64 type, bool sample_id_all, struct perf_sample *data)
{
	const u64 *array;

	data->cpu = data->pid = data->tid = -1;
	data->stream_id = data->id = data->time = -1ULL;

	if (event->header.type != PERF_RECORD_SAMPLE) {
		if (!sample_id_all)
			return 0;
		return perf_event__parse_id_sample(event, type, data);
	}

	array = event->sample.array;

	if (type & PERF_SAMPLE_IP) {
		data->ip = event->ip.ip;
		array++;
	}

	if (type & PERF_SAMPLE_TID) {
		u32 *p = (u32 *)array;
		data->pid = p[0];
		data->tid = p[1];
		array++;
	}

	if (type & PERF_SAMPLE_TIME) {
		data->time = *array;
		array++;
	}

	if (type & PERF_SAMPLE_ADDR) {
		data->addr = *array;
		array++;
	}

	data->id = -1ULL; /* useful? */
	if (type & PERF_SAMPLE_ID) {
		data->id = *array;
		array++;
	}

	if (type & PERF_SAMPLE_STREAM_ID) {
		data->stream_id = *array;
		array++;
	}

	if (type & PERF_SAMPLE_CPU) {
		u32 *p = (u32 *)array;
		data->cpu = *p;
		array++;
	}

	if (type & PERF_SAMPLE_PERIOD) {
		data->period = *array;
		array++;
	}

	if (type & PERF_SAMPLE_READ) {
		fprintf(stderr, "PERF_SAMPLE_READ is unsuported for now\n");
		return -1;
	}

	if (type & PERF_SAMPLE_CALLCHAIN) {
		data->callchain = (struct ip_callchain *)array;
		array += 1 + data->callchain->nr; /* u64 nr + u64 * nr */
	}

	if (type & PERF_SAMPLE_RAW) {
		u32 *p = (u32 *)array;
		data->raw_size = *p;
		p++;
		data->raw_data = p;
	}

	return 0;
}

