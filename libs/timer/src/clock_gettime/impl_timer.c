/*
 * WengoPhone, a voice over Internet phone
 * Copyright (C) 2004-2005  Wengo
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

/*
 * @author David Ferlier
 *
 * Timer implementation for POSIX systems. It uses pthreads + clock_gettime
 *
 */

#include <wtimer.h>
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <signal.h>
#include <time.h>
#include <assert.h>
#include <string.h>

//#define TIMER_DEBUG

#define CLOCKS_PER_NANOSEC	(CLOCKS_PER_SEC / 1000000000.)

struct clock_gettime_metadata {
    struct sigaction sa;
    struct timespec ts;
    int running;
    pthread_t w_timer_thread;
    pthread_mutex_t w_timer_mutex;
};

w_timer_t *cgt_timer_create () {
    w_timer_t *ti;

    ti = (w_timer_t *) malloc (sizeof(w_timer_t));
    memset(ti, 0, sizeof(w_timer_t));
    ti->impl_data = malloc (sizeof(struct clock_gettime_metadata));

    return ti;
}

void cgt_timer_set_delay(w_timer_t *ti, timer_delay_t delay) {
    struct clock_gettime_metadata *ptm = (struct clock_gettime_metadata *)
                                       ti->impl_data;

    ptm->ts.tv_sec = 0;
    ptm->ts.tv_nsec = delay * 1000 * 1000;
}

void cgt_timer_set_callback(w_timer_t *ti, void (*callback)(void *)) {
    assert(ti != 0);
    ti->callback = callback;
}

void ts_sub(register struct timespec *out, register struct timespec *in) {
    out->tv_nsec -= in->tv_nsec;

    while (out->tv_nsec < 0) {
        --out->tv_sec;
        out->tv_nsec += 1000 * 1000 * 1000;
    }

    out->tv_sec -= in->tv_sec;
}

void * cgt_timer_thread(void *parg) {
    w_timer_t *ti = (w_timer_t *) parg;
    struct clock_gettime_metadata *ptm = (struct clock_gettime_metadata *)
                                       ti->impl_data;

    struct timespec ts_start;
    struct timespec ts_end;
    struct timespec ts_tosleep;

	while (ptm->running) {
		ts_start.tv_nsec = clock() / CLOCKS_PER_NANOSEC;

        if (ti->callback) {
            ti->callback(ti->userdata);
		}
	
		ts_end.tv_nsec = clock() / CLOCKS_PER_NANOSEC;

        ts_tosleep.tv_nsec = ptm->ts.tv_nsec;
        ts_tosleep.tv_nsec -= (ts_end.tv_nsec - ts_start.tv_nsec);

        if (ts_tosleep.tv_nsec <= 0) {
            ts_tosleep.tv_nsec = 0;
        }

        if (ts_tosleep.tv_nsec > ptm->ts.tv_nsec) {
            ts_tosleep.tv_nsec = ptm->ts.tv_nsec;
        }

        nanosleep(&ts_tosleep, NULL);

#ifdef TIMER_DEBUG
        printf("Timer: user %d, effective %d\n",
            ptm->ts.tv_nsec / 1000 / 1000, (ts_tosleep.tv_nsec) / 1000 / 1000);
#endif
    }
    return 0;
}

int cgt_timer_start(w_timer_t *ti) {
    struct clock_gettime_metadata *ptm = (struct clock_gettime_metadata *)
                                       ti->impl_data;

    ptm->running = 1;
    pthread_create(&ptm->w_timer_thread, NULL, cgt_timer_thread, ti);
    return 0;
}

int cgt_timer_stop(w_timer_t *ti) {
    struct clock_gettime_metadata *ptm = (struct clock_gettime_metadata *)
                                       ti->impl_data;
    ptm->running = 0;
	return 0;
}

void cgt_timer_set_userdata(w_timer_t *ti, void *userdata) {
    ti->userdata = userdata;
}

void cgt_timer_destroy(w_timer_t *ti) {
    free(ti->impl_data);
}

struct timer_impl clock_gettime_impl = {
    "clock_gettime",
    cgt_timer_create,
    cgt_timer_set_delay,
    cgt_timer_set_callback,
    cgt_timer_set_userdata,
    cgt_timer_start,
    cgt_timer_stop,
    cgt_timer_destroy,
};

