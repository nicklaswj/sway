#include <wayland-client.h>
#include <wayland-cursor.h>
#include "wayland-xdg-shell-client-protocol.h"
#include "wayland-desktop-shell-client-protocol.h"
#include <cairo/cairo.h>
#include <pango/pangocairo.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <errno.h>
#include <sys/mman.h>
#include "client/window.h"
#include "client/buffer.h"
#include "list.h"
#include "log.h"

static void pointer_handle_enter(void *data, struct wl_pointer *pointer,
		     uint32_t serial, struct wl_surface *surface, wl_fixed_t sx_w, wl_fixed_t sy_w) {
	struct window *window = data;
	struct wl_cursor_image *image = window->cursor.cursor->images[0];
	wl_pointer_set_cursor(pointer, serial, window->cursor.surface, image->hotspot_x, image->hotspot_y);
}

static void pointer_handle_leave(void *data, struct wl_pointer *pointer,
		     uint32_t serial, struct wl_surface *surface) {
}

static void pointer_handle_motion(void *data, struct wl_pointer *pointer,
		      uint32_t time, wl_fixed_t sx_w, wl_fixed_t sy_w) {
}

static void pointer_handle_button(void *data, struct wl_pointer *pointer, uint32_t serial,
		      uint32_t time, uint32_t button, uint32_t state_w) {
}

static void pointer_handle_axis(void *data, struct wl_pointer *pointer,
		    uint32_t time, uint32_t axis, wl_fixed_t value) {
}

static const struct wl_pointer_listener pointer_listener = {
	.enter = pointer_handle_enter,
	.leave = pointer_handle_leave,
	.motion = pointer_handle_motion,
	.button = pointer_handle_button,
	.axis = pointer_handle_axis
};

void shell_surface_configure(void *data, struct wl_shell_surface *wl_shell_surface,
			  uint32_t edges, int32_t width, int32_t height) {
	struct window *window = data;
	window->width = width;
	window->height = height;
}

static const struct wl_shell_surface_listener surface_listener = {
	.configure = shell_surface_configure
};

struct window *window_setup(struct registry *registry, uint32_t width, uint32_t height, bool shell_surface) {
	struct window *window = malloc(sizeof(struct window));
	memset(window, 0, sizeof(struct window));
	window->width = width;
	window->height = height;
	window->registry = registry;
	window->font = "monospace 10";

	window->surface = wl_compositor_create_surface(registry->compositor);
	if (shell_surface) {
		window->shell_surface = wl_shell_get_shell_surface(registry->shell, window->surface);
		wl_shell_surface_add_listener(window->shell_surface, &surface_listener, window);
		wl_shell_surface_set_toplevel(window->shell_surface);
	}
	if (registry->pointer) {
		wl_pointer_add_listener(registry->pointer, &pointer_listener, window);
	}

	get_next_buffer(window);

	window->cursor.cursor_theme = wl_cursor_theme_load("default", 32, registry->shm); // TODO: let you customize this
	window->cursor.cursor = wl_cursor_theme_get_cursor(window->cursor.cursor_theme, "left_ptr");
	window->cursor.surface = wl_compositor_create_surface(registry->compositor);

	struct wl_cursor_image *image = window->cursor.cursor->images[0];
	struct wl_buffer *cursor_buf = wl_cursor_image_get_buffer(image);
	wl_surface_attach(window->cursor.surface, cursor_buf, 0, 0);
	wl_surface_damage(window->cursor.surface, 0, 0, image->width, image->height);
	wl_surface_commit(window->cursor.surface);

	return window;
}

static void frame_callback(void *data, struct wl_callback *callback, uint32_t time) {
	struct window *window = data;
	wl_callback_destroy(callback);
	window->frame_cb = NULL;
}

static const struct wl_callback_listener listener = {
	frame_callback
};

int window_prerender(struct window *window) {
	if (window->frame_cb) {
		return 0;
	}

	get_next_buffer(window);
	return 1;
}

int window_render(struct window *window) {
	window->frame_cb = wl_surface_frame(window->surface);
	wl_callback_add_listener(window->frame_cb, &listener, window);

	wl_surface_damage(window->surface, 0, 0, window->buffer->width, window->buffer->height);
	wl_surface_attach(window->surface, window->buffer->buffer, 0, 0);
	wl_surface_commit(window->surface);

	return 1;
}

void window_teardown(struct window *window) {
	// TODO
}
