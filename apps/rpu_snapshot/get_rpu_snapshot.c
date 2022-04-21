/*HEADER**********************************************************************
******************************************************************************
***
*** Copyright (c) 2011, 2012, 2013, Imagination Technologies Ltd.
***
*** This program is free software; you can redistribute it and/or
*** modify it under the terms of the GNU General Public License
*** as published by the Free Software Foundation; either version 2
*** of the License, or (at your option) any later version.
***
*** This program is distributed in the hope that it will be useful,
*** but WITHOUT ANY WARRANTY; without even the implied warranty of
*** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
*** GNU General Public License for more details.
***
*** You should have received a copy of the GNU General Public License
*** along with this program; if not, write to the Free Software
*** Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301,
*** USA.
***
*** File Name  : get_rpu_snapshot.c
***
*** File Description:
*** This file helps to collect RPU dumps from uccp20 driver for debugging.
***
******************************************************************************
*END**************************************************************************/
#include<stdio.h>
#include <netlink/netlink.h>
#include <netlink/genl/genl.h>
#include <netlink/genl/family.h>
#include <netlink/genl/ctrl.h>
#include <endian.h>
#include <stdbool.h>
#include <errno.h>
#include "nl80211_copy.h"
#include <netlink/msg.h>
#include <netlink/attr.h>
#include <fcntl.h>

bool debug_enable;
/* These enums need to be kept in sync with userspace */
enum rpu_testmode_attr {
	__RPU_TM_ATTR_INVALID = 0,
	RPU_TM_ATTR_CMD       = 1,
	RPU_TM_ATTR_DUMP      = 2,
	/* keep last */
	 __RPU_TM_ATTR_AFTER_LAST,
	RPU_TM_ATTR_MAX       = __RPU_TM_ATTR_AFTER_LAST - 1
};

enum rpu_testmode_cmd {
	RPU_TM_CMD_ALL	= 0,
	RPU_TM_CMD_GRAM  = 1,
	RPU_TM_CMD_COREA  = 2,
	RPU_TM_CMD_COREB  = 3,
	RPU_TM_CMD_PERIP = 4,
	RPU_TM_CMD_SYSBUS = 5,
	RPU_TM_CMD_MAX = 6,
};

static struct nla_policy rpu_testmode_policy[RPU_TM_ATTR_MAX + 1] = {
	[RPU_TM_ATTR_CMD] = { .type = NLA_U32 },
	[RPU_TM_ATTR_DUMP] = { .type = NLA_UNSPEC },
};
struct nl80211_state {
	struct nl_sock *nl_sock;
	int nl80211_id;
};

signed long long devidx;

static int phy_lookup(char *name)
{
	char buf[200];
	int fd, pos;

	snprintf(buf, sizeof(buf), "/sys/class/ieee80211/%s/index", name);

	fd = open(buf, O_RDONLY);
	if (fd < 0)
		return -1;
	pos = read(fd, buf, sizeof(buf) - 1);
	if (pos < 0) {
		close(fd);
		return -1;
	}
	buf[pos] = '\0';
	close(fd);
	return atoi(buf);
}


static int nl_dump_callback(struct nl_msg *msg, void *arg)
{
	FILE *fp = arg;
	unsigned char *data;
	unsigned int len, plen;
	struct nlmsghdr *nlh;

	struct nlattr *attrs[NL80211_ATTR_MAX + 1];
	struct nlattr *ntb[NL80211_ATTR_MAX + 1];
	struct nlattr *tb[RPU_TM_ATTR_MAX + 1];
	struct genlmsghdr *gnlh = nlmsg_data(nlmsg_hdr(msg));

	nla_parse(attrs, NL80211_ATTR_MAX, genlmsg_attrdata(gnlh, 0),
			genlmsg_attrlen(gnlh, 0), rpu_testmode_policy);
	if (!attrs[NL80211_ATTR_TESTDATA])
		return NL_SKIP;
	/* Get first netlink message header */
	/* While valid imessage herader is available */
	nlh = nlmsg_hdr(msg);
	if (nlh->nlmsg_type != NLMSG_DONE) {
		nla_parse(ntb, RPU_TM_ATTR_MAX,
			nla_data(attrs[NL80211_ATTR_TESTDATA]),
			nla_len(attrs[NL80211_ATTR_TESTDATA]),
			rpu_testmode_policy);
		if (!ntb[RPU_TM_ATTR_DUMP]) {
			printf("NL Attribute DUMP not found.\n");
			return NL_SKIP;
		}
		data = (unsigned char *) nla_data(ntb[RPU_TM_ATTR_DUMP]);
		len = nla_len(ntb[RPU_TM_ATTR_DUMP]);
		fwrite(data, sizeof(*data), len, fp);
		return NL_OK;
	}
	fclose(fp);
	return NL_STOP;
}

static int error_handler(struct sockaddr_nl *nla, struct nlmsgerr *err,
			void *arg)
{
	int *ret = arg;
	*ret = err->error;
	return NL_STOP;
}

static int finish_handler(struct nl_msg *msg, void *arg)
{
	int *ret = arg;
	*ret = 0;
	return NL_SKIP;
}

static int ack_handler(struct nl_msg *msg, void *arg)
{
	int *ret = arg;
	*ret = 0;
	return NL_STOP;
}


int nl80211_init(struct nl80211_state *state,
		enum rpu_testmode_cmd mycmd, FILE *fp)
{
	int err = 0, tmp = 0, left, count = 0;
	struct nl_msg *msg;
	struct nl_cb *cb, *s_cb;
	struct nlattr *tmdata;

	struct sockaddr_nl from;
	socklen_t fromlen;

	state->nl_sock = nl_socket_alloc();
	if (!state->nl_sock) {
		printf("Failed to allocate netlink socket.\n");
		return -ENOMEM;
	}
	if (genl_connect(state->nl_sock)) {
		printf("Failed to connect to generic netlink.\n");
		err = -ENOLINK;
		goto out_handle_destroy;
	}

	state->nl80211_id = genl_ctrl_resolve(state->nl_sock, "nl80211");

	if (state->nl80211_id < 0) {
		printf("nl80211 not found.\n");
		err = -ENOENT;
		goto out_handle_destroy;
	}
	/*500K Rx buffer size for RX dumps*/
	err = nl_socket_set_buffer_size(state->nl_sock, 1000 * 500, 8192);
	if (err < 0)
		return -1;
	cb = nl_cb_alloc(debug_enable ? NL_CB_DEBUG : NL_CB_DEFAULT);
	s_cb = nl_cb_alloc(debug_enable ? NL_CB_DEBUG : NL_CB_DEFAULT);

	if (!cb || !s_cb)
		printf("failed to allocate netlink callbacks\n");
	msg = nlmsg_alloc();
	genlmsg_put(msg, 0, 0, state->nl80211_id, 0, NLM_F_DUMP,
				  NL80211_CMD_TESTMODE, 0);
	NLA_PUT_U32(msg, NL80211_ATTR_WIPHY, devidx);

	tmdata = nla_nest_start(msg, NL80211_ATTR_TESTDATA);
		NLA_PUT_U32(msg, RPU_TM_ATTR_CMD, mycmd);
	nla_nest_end(msg, tmdata);
	nl_socket_set_cb(state->nl_sock, s_cb);

	tmp = nl_send_auto_complete(state->nl_sock, msg);
	err = 1;

	nl_cb_err(cb, NL_CB_CUSTOM, error_handler, &err);
	nl_cb_set(cb, NL_CB_FINISH, NL_CB_CUSTOM, finish_handler, &err);
	nl_cb_set(cb, NL_CB_ACK, NL_CB_CUSTOM, ack_handler, &err);
	nl_cb_set(cb, NL_CB_VALID, NL_CB_CUSTOM, nl_dump_callback, fp);

	while (err > 0)
		err = nl_recvmsgs(state->nl_sock, cb);
	return err;
 nla_put_failure:
 out_handle_destroy:
	nl_socket_free(state->nl_sock);
	return err;
}

char *get_string_from_enum(enum rpu_testmode_cmd cmd_e)
{
	switch (cmd_e) {
	case RPU_TM_CMD_GRAM:
		return "GRAM";
	case RPU_TM_CMD_COREA:
		return "COREA";
	case RPU_TM_CMD_COREB:
		return "COREB";
	case RPU_TM_CMD_PERIP:
		return "PERIP";
	case RPU_TM_CMD_SYSBUS:
		return "SYSBUS";
	default:
		return "NULL";
	}
}

enum rpu_testmode_cmd get_cmd_enum(char *mycmd)
{

	if (!strcmp(mycmd, "GRAM"))
		return RPU_TM_CMD_GRAM;
	else if (!strcmp(mycmd, "COREA"))
		return RPU_TM_CMD_COREA;
	else if (!strcmp(mycmd, "COREB"))
		return RPU_TM_CMD_COREB;
	else if (!strcmp(mycmd, "PERIP"))
		return RPU_TM_CMD_PERIP;
	else if (!strcmp(mycmd, "SYSBUS"))
		return RPU_TM_CMD_SYSBUS;
	else
		return RPU_TM_CMD_MAX;
}

int get_dump(char *input)
{
	char filename[50];
	enum rpu_testmode_cmd mycmd;
	FILE *fp;
	struct nl80211_state state;

	if (!strcmp(input, "NULL"))
		return -1;

	mycmd = get_cmd_enum(input);
	if (mycmd == RPU_TM_CMD_MAX)
		return -1;

	sprintf(filename, "rpu_%s_dump.bin", input);
	fp = fopen(filename, "w");
	if (!fp)
		return -1;

	return nl80211_init(&state, mycmd, fp);
}

int main(int argc, char **argv)
{
	const struct cmd *cmd = NULL;
	int args = 0, dumps = 0, err = 0;

	if ((argc > 6) || (argc == 1)) {
		printf("%s:%d Incorrect no.of arguments specified\n",
			__func__, __LINE__);
		return -1;
	}
	if (!strcmp(argv[1], "--debug")) {
		debug_enable = 1;
		argc--;
		argv++;
	}

	devidx = phy_lookup(argv[1]);
	if (devidx < 0) {
		printf("Check phyX paramater\n");
		return -1;
	}
	argc--;
	argv++;

	for (args = 1; args < argc; args++) {
		if (strcmp(argv[args], "ALL")) {
			err = get_dump(argv[args]);
		} else { /*ALL case*/
			for (dumps = 1; dumps < RPU_TM_CMD_MAX; dumps++)
				err = get_dump(get_string_from_enum(dumps));
		}
	}
	return 0;
}
