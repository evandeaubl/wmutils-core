/* See LICENSE file for copyright and license details. */

#include <stdio.h>
#include <stdlib.h>
#include <err.h>
#include <string.h>
#include <xcb/xcb.h>
#include <xcb/xcb_aux.h>

#include "arg.h"
#include "util.h"

static xcb_connection_t *conn;

static void usage(char *);

static void
usage (char *name)
{
	fprintf(stderr, "usage: %s <wid> [wid...]\n", name);
	exit(1);
}

static void
close_window (xcb_connection_t *conn, int wid)
{
	xcb_client_message_event_t *event = calloc(32, 1);

	event->response_type = XCB_CLIENT_MESSAGE;
	event->format = 32;
	event->sequence = 0;
	event->window = wid;
	event->type = add_atom(conn, XCB_ATOM_STRING, "WM_PROTOCOLS", strlen("WM_PROTOCOLS"));
	event->data.data32[0] = add_atom(conn, XCB_ATOM_STRING, "WM_DELETE_WINDOW", strlen("WM_DELETE_WINDOW"));
	event->data.data32[1] = 0; /*XCBCurrentTime;*/
	xcb_send_event(conn, 0, wid, 0, (char*)event);
	xcb_flush(conn);
	free(event);
}

int
main(int argc, char **argv)
{
	char *argv0;

	if (argc < 2)
		usage(argv[0]);

	ARGBEGIN{
	default: usage(argv0);
	}ARGEND;

	init_xcb(&conn);

	/* assume remaining arguments are windows */
	while (*argv) {
		/* gracefully request the window close */
		close_window(conn, strtoul(*argv++, NULL, 16));
	}

	xcb_aux_sync(conn);
	kill_xcb(&conn);

	return 0;
}
