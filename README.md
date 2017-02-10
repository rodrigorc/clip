# clip
Manage the clipboard from the console

## Introduction

Witht this tool you can copy and paste text from your Linux virtual terminal, not with a combination of keys but with a command.

Do you think it is useless? See some examples:

To download a web page, copy the url and then run:

    $ wget "$(clip)"

To copy the current date to the clipboard:

    $ clip -s "$(date)"

If you want to send someone a file, you run:

    $ clip -f myfile.txt

and then paste into an email message.

There are endless uses!

## Compilation

This program uses Gtk to handle the clipboard, either Gtk-2.0 or Gtk-3.0, to be chosen at compilation time.

To compile for Gtk-2:

    $ gcc `pkg-config gtk+-2.0 --cflags --libs` -O2 clip.c -o clip

Or for Gtk-3:

    $ gcc `pkg-config gtk+-3.0 --cflags --libs` -O2 clip.c -o clip

## Clipboards

Before going on, a couple of clarifications about the clipboard in X systems:

1. There are two clipboards:
 * The *default* clipboard is the usual copy-paste one.
 * The *primary* clipboard is the current selection, that needs no copy and usually is pasted with the middle mouse button.

2. The clipboard is a client-to-client protocol, the X server does not store the copied data. So when the *copier* program finishes, the data can no longer be pasted. Many (most) desktop systems include a clipboard server, that is a process that holds the copied text whenever the *copier* process ends so that the clipboard data will not be lost. That will work for the *default* clipboard but not for the *primary* one.

## Options

If run without options it will print to the standard output the contents of the default clipboard.

You can use option `-p` to use the primary clipboard instead. For example, just select the URL you want to download and run:

    $ wget $(clip -p)

If you want to set the clipboard use the `-s <text>` option. If the clipboard server is not working, for any reason, you can use the `-w` option to prevent the program from finishing.

If you set the primary clipboard you will probably need the `-w` option too, or else the program will end immediately and the text will be lost. For example, to copy the current directory to the primary clipboard:

    $ clip -pws "$(pwd)"

You can also copy the contents of a file using the option `-f <file>`. You can use `-f -` to specify standard input. For example to copy the contents of a web page:

    $ wget https://en.wikipedia.org/ -O - | clip -f -
