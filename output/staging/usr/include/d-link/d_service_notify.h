/**
 * @ingroup ServiceNotify
 *
 * @file
 * Модуль оповещения dsysinit из сервисов.
 */

#ifndef D_SERVICE_NOTIFY_H
#define D_SERVICE_NOTIFY_H

#include <jansson.h>

#ifndef DLINK_EXPORT
#define DLINK_EXPORT __attribute__((visibility("default")))
#endif

/*
 *
 * Copyright 2017 D-Link Trade
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated
 * documentation files (the "Software"), to deal in the Software without restriction, including without limitation the
 * rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to
 * permit persons to whom the Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all copies or substantial portions of the
 * Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
 * WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
 * COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR
 * OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 */

#ifdef __cplusplus
extern "C" {
#endif



/** Path to AF_UNIX socket of 'service manager'. */
#define DLINK_SERVICE_MANAGER_SOCK "/var/run/dsysinit.sock"



/**
 * Connects the service to D-Link service manager.
 * Opens a socket (AF_UNIX, SOCK_STREAM) and connects to 'service manager'.
 * This is essential for proper service initialization.
 *
 * @note If a service was launched from the 'service manager' and DID NOT connect
 * after initialization, it will be launched again and again and so on.
 *
 * @param[in] id - service identifier, passed from 'service manager'.
 */
DLINK_EXPORT void dlink_service_connect(int id);


/**
 * Disconnects the service from D-Link service manager.
 * Closes the socket (AF_UNIX, SOCK_STREAM) and disconnects from
 * 'service manager'. This is essential for proper service initialization.
 *
 * @param[in] id - service identifier, passed from 'service manager'.
 */
DLINK_EXPORT void dlink_service_disconnect(int id);



/**
 * Sends the message to D-Link service manager (generic version).
 * Packs custom *method* and *params*, then sends it via socket.
 *
 * @note Socket must be previously opened by dlink_service_connect().
 *
 * @param[in] method - method name ('Update' is currently supported);
 * @param[in] params - method parameters.
 *
 * @retval  0      - success;
 * @retval -EINVAL - incorrect *method* or *params*;
 * @retval -EBADFD - socket was not previously opened;
 * @retval -ENOMSG - *params* is not a valid JSON;
 * @retval -ENOMEM - malloc() failure;
 * @retval <0      - send() failure;
 */
DLINK_EXPORT int dlink_service_send(const char *method, json_t *params);



/**
 * Calls dlink_service_send() with method 'Update'.
 *
 * @param[in] params - JSON-parameters.
 *
 * @retval  0      - success;
 * @retval -EINVAL - incorrect *method* or *params*;
 * @retval -EBADFD - socket was not previously opened;
 * @retval -ENOMSG - *params* is not a valid JSON;
 * @retval -ENOMEM - malloc() failure;
 * @retval <0      - send() failure;
 */
DLINK_EXPORT int dlink_service_update(json_t *params);



/**
 * Gets the D-Link service manager socket file descriptor.
 *
 * @retval  socket_fd - success;
 * @retval -1         - socket was not opened.
 */
DLINK_EXPORT int dlink_service_socket(void);



/**
 * Calls dlink_service_send() with method 'SetCmdline'.
 *
 * dlink_service_update_cmdline() uses data from
 * <b>/proc/self/cmdline</b>.
 *
 * @retval  0 - success;
 * @retval -1 - send() failure.
 */
DLINK_EXPORT int dlink_service_update_cmdline(void);

DLINK_EXPORT void dlink_service_use_hash(void);
DLINK_EXPORT char *dlink_service_hash(void);
DLINK_EXPORT int dlink_service_update_by_hash(json_t *params, const char *service_hash);

/**
 * Sets FD_CLOEXEC to the socket of D-Link service manager.
 *
 * @retval  0 - success;
 * @retval <0 - error.
 */
DLINK_EXPORT int dlink_service_set_cloexec(void);


#ifdef __cplusplus
}
#endif

#endif //D_SERVICE_NOTIFY_H
