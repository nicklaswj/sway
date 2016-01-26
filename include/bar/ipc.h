#ifndef _SWAYBAR_IPC_H
#define _SWAYBAR_IPC_H

#include "bar.h"

/**
 * Initialize ipc connection to sway and get sway state, outputs, bar_config.
 */
void ipc_bar_init(struct bar *bar, int outputi, const char *bar_id);

/**
 * Handle ipc event from sway.
 */
bool handle_ipc_event(struct bar *bar);

#endif /* _SWAYBAR_IPC_H */

