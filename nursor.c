#include <X11/Xlib.h>
#include <X11/extensions/XTest.h>
#include <X11/keysym.h>
#include <unistd.h>
#include <stdio.h>

#define SPEED 25
#define SLOW_SPEED 5

int cursor_speed = SPEED;
Display *display;

void move_cursor(Display *display, int x_offset, int y_offset) {
	XTestFakeRelativeMotionEvent(display, x_offset, y_offset, CurrentTime);
	XFlush(display);
}

void simulate_scroll(int direction) {
	XTestFakeButtonEvent(display, direction == 1 ? 4 : 5, True, CurrentTime);
	XTestFakeButtonEvent(display, direction == 1 ? 4 : 5, False, CurrentTime);
	XFlush(display);
}

void simulate_middle_click() {
	XTestFakeButtonEvent(display, 2, True, CurrentTime);
	XTestFakeButtonEvent(display, 2, False, CurrentTime);
	XFlush(display);
}

int is_key_pressed(char keymap[32], int keycode) {
	return keymap[keycode / 8] & (1 << (keycode % 8));
}

int main() {
	char keymap[32];
	int x_offset, y_offset;
	XKeyboardState keyboardState;

	display = XOpenDisplay(NULL);
	if (display == NULL) {
			fprintf(stderr, "Unable to open display.\n");
			return 1;
	}

	int keycode_up = XKeysymToKeycode(display, XK_KP_8);
	int keycode_down = XKeysymToKeycode(display, XK_KP_2);
	int keycode_left = XKeysymToKeycode(display, XK_KP_4);
	int keycode_right = XKeysymToKeycode(display, XK_KP_6);
	int keycode_up_left = XKeysymToKeycode(display, XK_KP_7);
	int keycode_up_right = XKeysymToKeycode(display, XK_KP_9);
	int keycode_down_left = XKeysymToKeycode(display, XK_KP_1);
	int keycode_down_right = XKeysymToKeycode(display, XK_KP_3);
	int keycode_lmb = XKeysymToKeycode(display, XK_KP_0);
	int keycode_rmb = XKeysymToKeycode(display, XK_KP_5);

	int keycode_divide = XKeysymToKeycode(display, XK_KP_Divide);
	int keycode_multiply = XKeysymToKeycode(display, XK_KP_Multiply);
	int keycode_separator = XKeysymToKeycode(display, XK_KP_Separator);

	int keycode_plus = XKeysymToKeycode(display, XK_KP_Add);
	int keycode_minus = XKeysymToKeycode(display, XK_KP_Subtract);

	int keycodes[] = {keycode_up, keycode_down, keycode_left, keycode_right,
										keycode_up_left, keycode_up_right, keycode_down_left,
										keycode_down_right, keycode_lmb, keycode_rmb, keycode_plus,
										keycode_minus, keycode_multiply, keycode_divide, keycode_separator};

	int lmb_pressed = 0;
	int rmb_pressed = 0;

	XAllowEvents(display, AsyncKeyboard, CurrentTime);

	for (int i = 0; i < sizeof(keycodes) / sizeof(int); i++) {
			XUngrabKey(display, keycodes[i], AnyModifier, DefaultRootWindow(display));
	}

	for (int i = 0; i < sizeof(keycodes) / sizeof(int); i++) {
			XGrabKey(display, keycodes[i], AnyModifier, DefaultRootWindow(display), True, GrabModeAsync, GrabModeAsync);
	}

	while (1) {
			x_offset = 0;
			y_offset = 0;

			XQueryKeymap(display, keymap);

			XGetKeyboardControl(display, &keyboardState);

			if (keyboardState.led_mask & 0x02) {
					if (is_key_pressed(keymap, keycode_divide)) cursor_speed = cursor_speed - 1;
					else if (is_key_pressed(keymap, keycode_multiply)) cursor_speed = cursor_speed + 1;

					if (is_key_pressed(keymap, keycode_separator)) {
							if (is_key_pressed(keymap, keycode_up)) y_offset = -SLOW_SPEED;
							else if (is_key_pressed(keymap, keycode_down)) y_offset = SLOW_SPEED;
							if (is_key_pressed(keymap, keycode_left)) x_offset = -SLOW_SPEED;
							else if (is_key_pressed(keymap, keycode_right)) x_offset = SLOW_SPEED;
							if (is_key_pressed(keymap, keycode_up_left)) { x_offset = -SLOW_SPEED; y_offset = -SLOW_SPEED; }
							else if (is_key_pressed(keymap, keycode_up_right)) { x_offset = SLOW_SPEED; y_offset = -SLOW_SPEED; }
							if (is_key_pressed(keymap, keycode_down_left)) { x_offset = -SLOW_SPEED; y_offset = SLOW_SPEED; }
							else if (is_key_pressed(keymap, keycode_down_right)) { x_offset = SLOW_SPEED; y_offset = SLOW_SPEED; }
					} else {
							if (is_key_pressed(keymap, keycode_up)) y_offset = -cursor_speed;
							else if (is_key_pressed(keymap, keycode_down)) y_offset = cursor_speed;
							if (is_key_pressed(keymap, keycode_left)) x_offset = -cursor_speed;
							else if (is_key_pressed(keymap, keycode_right)) x_offset = cursor_speed;
							if (is_key_pressed(keymap, keycode_up_left)) { x_offset = -cursor_speed; y_offset = -cursor_speed; }
							else if (is_key_pressed(keymap, keycode_up_right)) { x_offset = cursor_speed; y_offset = -cursor_speed; }
							if (is_key_pressed(keymap, keycode_down_left)) { x_offset = -cursor_speed; y_offset = cursor_speed; }
							else if (is_key_pressed(keymap, keycode_down_right)) { x_offset = cursor_speed; y_offset = cursor_speed; }
					}

					int lmb_current = is_key_pressed(keymap, keycode_lmb);
					if (lmb_current && !lmb_pressed) {
							XTestFakeButtonEvent(display, 1, True, CurrentTime);
							lmb_pressed = 1;
					} else if (!lmb_current && lmb_pressed) {
							XTestFakeButtonEvent(display, 1, False, CurrentTime);
							lmb_pressed = 0;
					}

					int rmb_current = is_key_pressed(keymap, keycode_rmb);
					if (rmb_current && !rmb_pressed) {
							XTestFakeButtonEvent(display, 3, True, CurrentTime);
							rmb_pressed = 1;
					} else if (!rmb_current && rmb_pressed) {
							XTestFakeButtonEvent(display, 3, False, CurrentTime);
							rmb_pressed = 0;
					}

					if (is_key_pressed(keymap, keycode_lmb) && is_key_pressed(keymap, keycode_rmb)) {
							simulate_middle_click();
					}

					if (is_key_pressed(keymap, keycode_plus)) simulate_scroll(-1);
					else if (is_key_pressed(keymap, keycode_minus)) simulate_scroll(1);

					if (x_offset != 0 || y_offset != 0) {
							move_cursor(display, x_offset, y_offset);
					}
			}

			usleep(10000);
    }

    for (int i = 0; i < sizeof(keycodes) / sizeof(int); i++) {
        XUngrabKey(display, keycodes[i], AnyModifier, DefaultRootWindow(display));
    }

    XCloseDisplay(display);
    return 0;
}
