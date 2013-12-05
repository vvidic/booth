/* 
 * Copyright (C) 2013 Philipp Marek <philipp.marek@linbit.com>
 * 
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 * 
 * This software is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#ifndef _INLINE_FN_H
#define _INLINE_FN_H

#include <time.h>
#include <assert.h>
#include <string.h>
#include "config.h"
#include "transport.h"



inline static uint32_t get_local_id(void)
{
	return local ? local->site_id : -1;
}


inline static uint32_t get_node_id(struct booth_site *node)
{
	return node ? node->site_id : NO_OWNER;
}


inline static int ticket_valid_for(const struct ticket_config *tk)
{
	int left;

	left = tk->current_state.expires - time(NULL);
	return (left < 0) ? 0 : left;
}


/** Returns number of seconds left, if any. */
inline static int owner_and_valid(const struct ticket_config *tk)
{
	if (tk->current_state.owner != local)
		return 0;

	return ticket_valid_for(tk);
}

static inline void init_header_bare(struct boothc_header *h) {
	h->magic   = htonl(BOOTHC_MAGIC);
	h->version = htonl(BOOTHC_VERSION);
	h->from    = htonl(local->site_id);
}

static inline void init_header(struct boothc_header *h, int cmd,
			int result, int data_len)
{
	init_header_bare(h);
	h->length  = htonl(data_len);
	h->cmd     = htonl(cmd);
	h->result  = htonl(result);
}

static inline void init_ticket_site_header(struct boothc_ticket_msg *msg, int cmd)
{
	init_header(&msg->header, cmd, 0, sizeof(*msg));
}

static inline void init_ticket_msg(struct boothc_ticket_msg *msg,
		int cmd, struct ticket_config *tk)
{
	assert(sizeof(msg->ticket.id) == sizeof(tk->name));

	init_header(&msg->header, cmd, 0, sizeof(*msg));

	if (!tk) {
		memset(&msg->ticket, 0, sizeof(msg->ticket));
	} else {
		memcpy(msg->ticket.id, tk->name, sizeof(msg->ticket.id));

		msg->ticket.expiry      = htonl(ticket_valid_for(tk));
		msg->ticket.owner       = htonl(get_node_id(tk->current_state.owner));
		msg->ticket.ballot      = htonl(tk->current_state.ballot);
		msg->ticket.prev_ballot = htonl(tk->current_state.prev_ballot);
	}
}


static inline struct booth_transport const *transport(void) {
	return booth_transport + booth_conf->proto;
}

#endif
