/**************************************************************************
clip: A tool to manage the clipboard from the command line
To compile, just do:
    $ gcc `pkg-config gtk+-2.0 --cflags --libs` -O2 clip.c -o clip
Or:
    $ gcc `pkg-config gtk+-3.0 --cflags --libs` -O2 clip.c -o clip

***************************************************************************
MIT License

Copyright (c) 2010-2017 Rodrigo Rivas Costa 

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
 
***************************************************************************/

#include <stdlib.h>
#include <string.h>
#include <gtk/gtk.h>

void Help(const gchar *name)
{
    g_print("Copyright (c) 2010 Rodrigo Rivas Costa. http://rodrigo.dualnot.com\n");
    g_print("Syntax:\n\t%s [options] : Echoes the text from the clipboard.\n", name);
    g_print("Or:\n\t%s [options] -s <text> : Sets the text of the clipboard to <text>.\n", name);
    g_print("Or:\n\t%s [options] -f <file> : Sets the text of the clipboard to the contents of <file> (- is stdin).\n", name);
    g_print("\nOptions:\n\t-p: Use the primary clipboard (selection) instead of the default.\n");
    g_print("\t-w: With -s or -f, do not exit. Probably needed when you set the primary clipboard (-p) as it cannot be saved.\n");
    g_print("\n");
    exit(1);
}

void clip_change(GtkClipboard *clip, GdkEventOwnerChange *ev, gpointer data)
{
    int *changes = (int*)data;
    if ((*changes)-- == 0)
        gtk_main_quit();
}

int main(int argc, char **argv)
{
    GtkClipboard *clip;
    int op;
    GdkAtom use_clip = GDK_SELECTION_CLIPBOARD;
    char *text = NULL, *file = NULL;
    gint text_len = -1;
    int do_wait = 0;

    gtk_init(&argc, &argv);

    while ((op = getopt(argc, argv, "h?pws:f:")) != -1)
    {
        switch (op)
        {
        case 'h':
        case '?':
            Help(argv[0]);
        case 'p':
            use_clip = GDK_SELECTION_PRIMARY;
            break;
        case 's':
            text = optarg;
            break;
        case 'w':
            do_wait = 1;
            break;
        case 'f':
            file = optarg;
            break;
        }
    }

    clip = gtk_clipboard_get(use_clip);

    if (file)
    {
        if (text)
            Help(argv[0]);

        FILE *f = strcmp(file, "-")? fopen(file, "rb") : stdin;
        if (!f)
        {
            perror(file);
            return 1;
        }
        text = (char *)malloc(4096);
        size_t len = 0, r;
        do
        {
            r = fread(text + len, 1, 4096, f);
            len += r;
            text = (char*)realloc(text, len + 4096);
        } while (r == 4096);

        text = (char*)realloc(text, len);
        text_len = len;

        if (strcmp(file, "-"))
            fclose(f);
    }
    if (text)
    {
        if (do_wait)
        {
            static int changes;
            changes = 1;
            g_signal_connect(clip, "owner_change", (GCallback)clip_change, &changes);
            gtk_clipboard_set_text(clip, text, text_len);
            gtk_main();
        }
        else
        {
            gtk_clipboard_set_text(clip, text, text_len);
            gtk_clipboard_store(clip);
        }
        if (file)
            free(text);
    }
    else
    {
        gchar *txt = gtk_clipboard_wait_for_text(clip);
        if (txt)
        {
            g_print("%s", txt);
            g_free(txt);
        }
    }

    return 0;
}
