#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <getopt.h>
#include <stdint.h>
#include <sys/un.h>
#include <sys/socket.h>
#include <unistd.h>
#include "log.h"
#include "stringop.h"
#include "ipc.h"
#include "readline.h"
#include "ipc-client.h"

static const char ipc_magic[] = {'i', '3', '-', 'i', 'p', 'c'};
static const size_t ipc_header_size = sizeof(ipc_magic)+8;

char *get_socketpath(void) {
	FILE *fp = popen("sway --get-socketpath", "r");
	if (!fp) {
		return NULL;
	}
	char *line = read_line(fp);
	pclose(fp);
	return line;
}

int ipc_open_socket(const char *socket_path) {
	struct sockaddr_un addr;
	int socketfd;
	if ((socketfd = socket(AF_UNIX, SOCK_STREAM, 0)) == -1) {
		sway_abort("Unable to open Unix socket");
	}
	addr.sun_family = AF_UNIX;
	strcpy(addr.sun_path, socket_path);
	int l = sizeof(addr.sun_family) + strlen(addr.sun_path);
	if (connect(socketfd, (struct sockaddr *)&addr, l) == -1) {
		sway_abort("Unable to connect to %s", socket_path);
	}
	return socketfd;
}

struct ipc_response *ipc_recv_response(int socketfd) {
	char data[ipc_header_size];
	uint32_t *data32 = (uint32_t *)(data + sizeof(ipc_magic));

	size_t total = 0;
	while (total < ipc_header_size) {
		ssize_t received = recv(socketfd, data + total, ipc_header_size - total, 0);
		if (received < 0) {
			sway_abort("Unable to receive IPC response");
		}
		total += received;
	}

	struct ipc_response *response = malloc(sizeof(struct ipc_response));
	total = 0;
	response->size = data32[0];
	response->type = data32[1];
	char *payload = malloc(response->size + 1);
	while (total < response->size) {
		ssize_t received = recv(socketfd, payload + total, response->size - total, 0);
		if (received < 0) {
			sway_abort("Unable to receive IPC response");
		}
		total += received;
	}
	payload[response->size] = '\0';
	response->payload = payload;

	return response;
}

void free_ipc_response(struct ipc_response *response) {
	free(response->payload);
	free(response);
}

char *ipc_single_command(int socketfd, uint32_t type, const char *payload, uint32_t *len) {
	char data[ipc_header_size];
	uint32_t *data32 = (uint32_t *)(data + sizeof(ipc_magic));
	memcpy(data, ipc_magic, sizeof(ipc_magic));
	data32[0] = *len;
	data32[1] = type;

	if (write(socketfd, data, ipc_header_size) == -1) {
		sway_abort("Unable to send IPC header");
	}

	if (write(socketfd, payload, *len) == -1) {
		sway_abort("Unable to send IPC payload");
	}

	struct ipc_response *resp = ipc_recv_response(socketfd);
	char *response = resp->payload;
	*len = resp->size;
	free(resp);

	return response;
}
