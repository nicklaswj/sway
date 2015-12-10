# sway [![](https://api.travis-ci.org/SirCmpwn/sway.svg)](https://travis-ci.org/SirCmpwn/sway) [![Donate with fosspay](https://drewdevault.com/donate/static/donate-with-fosspay.png)](https://drewdevault.com/donate?project=4)

"**S**irCmpwn's **Way**land window manager" is a **work in progress**
i3-compatible window manager for [Wayland](http://wayland.freedesktop.org/).
Read the [FAQ](https://github.com/SirCmpwn/sway/wiki). Join the
[IRC channel](http://webchat.freenode.net/?channels=sway&uio=d4) (#sway on
irc.freenode.net).

![](https://sr.ht/i4Cu.png)

## Rationale

I use i3 on xorg. Wayland is coming, and [i3way](http://i3way.org/) still has
zero lines of source code after two years.

## Status

[See "i3 feature support"](https://github.com/SirCmpwn/sway/issues/2)

## Installation

### From Packages

sway is not supported by many distributions yet. Here's a list of packages
available for you to install:

* [Arch Linux](https://aur.archlinux.org/packages/sway-git/)
* [Gentoo Linux](https://github.com/zetok/zetok-overlay/)
* [openSUSE](https://build.opensuse.org/project/show/X11:Wayland)

For other distros, [see this wiki page](https://github.com/SirCmpwn/sway/wiki/Install-on-other-distros).

### Compiling from Source

Install dependencies:

* cmake
* [wlc](https://github.com/Cloudef/wlc)
* wayland
* xwayland
* asciidoc
* pcre
* json-c
* pango
* cairo
* gdk-pixbuf2

Run these commands:

    mkdir build
    cd build
    cmake ..
    make
    sudo make install

On systems without logind, you need to suid the sway binary:

    sudo chmod a+s /usr/bin/sway

## Configuration

If you already use i3, then copy your i3 config to `~/.config/sway/config` and
it'll work out of the box. Otherwise, copy the sample configuration file to
`~/.config/sway/config`. It is located at `/etc/sway/config`, unless the 
`DFALLBACK_CONFIG_DIR` flag has been set. Run `man 5 sway` for information on
the configuration.

## Running

Instead of running `startx`, run `sway`. You can run `sway` from within X as
well, which is useful for testing.
