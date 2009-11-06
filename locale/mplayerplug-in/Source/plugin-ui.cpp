#include "plugin.h"
#include <errno.h>

extern int errno;
extern int DEBUG;
#include <X11/xpm.h>
#include <X11/cursorfont.h>

#ifdef X_ENABLED
#include "../pixmaps/logo.xpm"
#include "../pixmaps/progress_left.xpm"
#include "../pixmaps/progress_middle.xpm"
#include "../pixmaps/progress_right.xpm"
#include "../pixmaps/progress_fill.xpm"
#endif

#ifdef GTK_ENABLED
#include <gdk/gdkkeysyms.h>

// 16 x 12 button pixmaps
#include "../pixmaps/play_down_small.xpm"
#include "../pixmaps/play_up_small.xpm"
#include "../pixmaps/pause_down_small.xpm"
#include "../pixmaps/pause_up_small.xpm"
#include "../pixmaps/stop_down_small.xpm"
#include "../pixmaps/stop_up_small.xpm"
#include "../pixmaps/ff_down_small.xpm"
#include "../pixmaps/ff_up_small.xpm"
#include "../pixmaps/rew_down_small.xpm"
#include "../pixmaps/rew_up_small.xpm"
#include "../pixmaps/volume.xpm"
#include "../pixmaps/fs_down_small.xpm"
#include "../pixmaps/fs_up_small.xpm"

// default src button
#include "../pixmaps/start.xpm"
#endif

#define BUTTON_WIDTH	43
#define BUTTON_HEIGHT	32
#define SMALL_BUTTON_WIDTH	21
#define SMALL_BUTTON_HEIGHT	16

#define BORDER               10
#define LOGO_WIDTH          257
#define LOGO_HEIGHT          48

#ifdef X_ENABLED
unsigned long GetFontAtom(Display * dpy, XFontStruct * fs, char *atomname)
{
    unsigned long val;
    Atom atom = XInternAtom(dpy, atomname, True);
    XGetFontProperty(fs, atom, &val);
    return val;
}
#endif

void Initialize(Display * dpy, Drawable d, nsPluginInstance * instance,
		GC gc)
{
#ifdef X_ENABLED
    Cursor guicursor;
    XSetWindowAttributes attrs;
    char **missing_charset_list_return;
    int missing_charset_count_return;
    char *def_string_return;

    XpmCreatePixmapFromData(dpy, d, logo_xpm, &instance->logo,
			    &instance->logomask, NULL);
    XpmCreatePixmapFromData(dpy, d, progress_left_xpm,
			    &instance->progress_left,
			    &instance->progress_leftmask, NULL);
    XpmCreatePixmapFromData(dpy, d, progress_middle_xpm,
			    &instance->progress_middle,
			    &instance->progress_middlemask, NULL);
    XpmCreatePixmapFromData(dpy, d, progress_right_xpm,
			    &instance->progress_right,
			    &instance->progress_rightmask, NULL);
    XpmCreatePixmapFromData(dpy, d, progress_fill_xpm,
			    &instance->progress_fill,
			    &instance->progress_fillmask, NULL);

    if (!instance->font) {
	XFontStruct *fstr;
	char *family;
	unsigned long size = 14;
	char *italic;
	char *bold;
	char fontname[256];

	fstr = XQueryFont(dpy, XGContextFromGC(gc));
	family = XGetAtomName(dpy, GetFontAtom(dpy, fstr, "FAMILY_NAME"));
	bold = XGetAtomName(dpy, GetFontAtom(dpy, fstr, "WEIGHT_NAME"));
	italic = XGetAtomName(dpy, GetFontAtom(dpy, fstr, "SLANT"));
	size = GetFontAtom(dpy, fstr, "PIXEL_SIZE");
	XFreeFontInfo(NULL, fstr, 0);

	sprintf(fontname, "-*-%s-%s-%s-*--%d-*",
		family, bold, italic, (int) size);
	instance->font = XCreateFontSet(dpy,
					fontname,
					&missing_charset_list_return,
					&missing_charset_count_return,
					&def_string_return);
    }

    guicursor = XCreateFontCursor(dpy, XC_watch);
    attrs.cursor = guicursor;
    XChangeWindowAttributes(dpy, d, CWCursor, &attrs);
#endif
}

void InitPixbufs(nsPluginInstance * instance)
{
#ifdef GTK2_ENABLED

    instance->pb_sm_play_up =
	gdk_pixbuf_new_from_xpm_data((const char **) play_up_small);
    instance->pb_sm_play_down =
	gdk_pixbuf_new_from_xpm_data((const char **) play_down_small);
    instance->pb_sm_pause_up =
	gdk_pixbuf_new_from_xpm_data((const char **) pause_up_small);
    instance->pb_sm_pause_down =
	gdk_pixbuf_new_from_xpm_data((const char **) pause_down_small);
    instance->pb_sm_stop_up =
	gdk_pixbuf_new_from_xpm_data((const char **) stop_up_small);
    instance->pb_sm_stop_down =
	gdk_pixbuf_new_from_xpm_data((const char **) stop_down_small);
    instance->pb_sm_ff_up =
	gdk_pixbuf_new_from_xpm_data((const char **) ff_up_small);
    instance->pb_sm_ff_down =
	gdk_pixbuf_new_from_xpm_data((const char **) ff_down_small);
    instance->pb_sm_rew_up =
	gdk_pixbuf_new_from_xpm_data((const char **) rew_up_small);
    instance->pb_sm_rew_down =
	gdk_pixbuf_new_from_xpm_data((const char **) rew_down_small);
    instance->pb_sm_vol =
	gdk_pixbuf_new_from_xpm_data((const char **) volume);
    instance->pb_sm_fs_up =
	gdk_pixbuf_new_from_xpm_data((const char **) fs_up_small);
    instance->pb_sm_fs_down =
	gdk_pixbuf_new_from_xpm_data((const char **) fs_down_small);
#endif

#ifdef GTK1_ENABLED

    instance->pb_sm_play_up =
	gdk_pixmap_create_from_xpm_d(instance->gtkwidget->window,
				     NULL, NULL, play_up_small);
    instance->pb_sm_play_down =
	gdk_pixmap_create_from_xpm_d(instance->gtkwidget->window,
				     NULL, NULL, play_down_small);
    instance->pb_sm_pause_up =
	gdk_pixmap_create_from_xpm_d(instance->gtkwidget->window,
				     NULL, NULL, pause_up_small);
    instance->pb_sm_pause_down =
	gdk_pixmap_create_from_xpm_d(instance->gtkwidget->window,
				     NULL, NULL, pause_down_small);
    instance->pb_sm_stop_up =
	gdk_pixmap_create_from_xpm_d(instance->gtkwidget->window,
				     NULL, NULL, stop_up_small);
    instance->pb_sm_stop_down =
	gdk_pixmap_create_from_xpm_d(instance->gtkwidget->window,
				     NULL, NULL, stop_down_small);
    instance->pb_sm_ff_up =
	gdk_pixmap_create_from_xpm_d(instance->gtkwidget->window,
				     NULL, NULL, ff_up_small);
    instance->pb_sm_ff_down =
	gdk_pixmap_create_from_xpm_d(instance->gtkwidget->window,
				     NULL, NULL, ff_down_small);
    instance->pb_sm_rew_up =
	gdk_pixmap_create_from_xpm_d(instance->gtkwidget->window,
				     NULL, NULL, rew_up_small);
    instance->pb_sm_rew_down =
	gdk_pixmap_create_from_xpm_d(instance->gtkwidget->window,
				     NULL, NULL, rew_down_small);
    instance->pb_sm_vol =
	gdk_pixmap_create_from_xpm_d(instance->gtkwidget->window,
				     NULL, NULL, volume);
    instance->pb_sm_fs_up =
	gdk_pixmap_create_from_xpm_d(instance->gtkwidget->window,
				     NULL, NULL, fs_up_small);
    instance->pb_sm_fs_down =
	gdk_pixmap_create_from_xpm_d(instance->gtkwidget->window,
				     NULL, NULL, fs_down_small);

#endif
}

void DrawLogo(Display * dpy, Drawable d, nsPluginInstance * instance,
	      GC gc)
{
#ifdef X_ENABLED
    XGCValues values;

    values.clip_mask = instance->logomask;
    values.clip_x_origin = 0;
    values.clip_y_origin = 0;
    XChangeGC(dpy, gc, GCClipMask | GCClipXOrigin | GCClipYOrigin,
	      &values);
    XCopyArea(dpy, instance->logo, d, gc, 0, 0, LOGO_WIDTH, LOGO_HEIGHT, 0,
	      0);
    values.clip_mask = (Pixmap) NULL;
    XChangeGC(dpy, gc, GCClipMask | GCClipXOrigin | GCClipYOrigin,
	      &values);
#endif
}


void DrawProgressBG(Display * dpy, Window w, nsPluginInstance * instance,
		    int x, int y, int width, int height)
{
#ifdef X_ENABLED
    XGCValues values;
    GC border_gc;

    border_gc = XCreateGC(dpy, w, GCForeground, &values);

    values.clip_mask = instance->progress_leftmask;
    values.clip_x_origin = x - 10;
    values.clip_y_origin = y - 10;
    XChangeGC(dpy, border_gc, GCClipMask | GCClipXOrigin | GCClipYOrigin,
	      &values);
    XCopyArea(dpy, instance->progress_left, (Drawable) w, border_gc, 0, 0,
	      10, 31, x - 10, y - 10);
    values.clip_mask = (Pixmap) NULL;
    XChangeGC(dpy, border_gc, GCClipMask | GCClipXOrigin | GCClipYOrigin,
	      &values);

    XSetTile(dpy, border_gc, instance->progress_middle);
    XSetFillStyle(dpy, border_gc, FillTiled);
    XSetTSOrigin(dpy, border_gc, x, y - 10);
    XFillRectangle(dpy, w, border_gc, x, y - 10, width, 31);
    XSetFillStyle(dpy, border_gc, FillSolid);

    values.clip_mask = instance->progress_rightmask;
    values.clip_x_origin = x + width;
    values.clip_y_origin = y - 10;
    XChangeGC(dpy, border_gc, GCClipMask | GCClipXOrigin | GCClipYOrigin,
	      &values);
    XCopyArea(dpy, instance->progress_right, (Drawable) w, border_gc, 0, 0,
	      10, 31, x + width, y - 10);
    values.clip_mask = (Pixmap) NULL;
    XChangeGC(dpy, border_gc, GCClipMask | GCClipXOrigin | GCClipYOrigin,
	      &values);

    XFreeGC(dpy, border_gc);
#endif
}

void DrawProgress(Display * dpy, Window w, nsPluginInstance * instance,
		  int x, int y, int width, int height, int percent)
{
#ifdef X_ENABLED
    GC progress_gc;
    int pixelpercent;
    XGCValues values;

    if (percent > 100)
	percent = 100;
    progress_gc = XCreateGC(dpy, w, GCForeground, &values);
    XSetTile(dpy, progress_gc, instance->progress_fill);
    XSetFillStyle(dpy, progress_gc, FillTiled);
    XSetTSOrigin(dpy, progress_gc, x, y);
    pixelpercent = (int) ((width - x) * (percent / 100.));
    XFillRectangle(dpy, w, progress_gc, x, y, pixelpercent, height);
    XSetFillStyle(dpy, progress_gc, FillSolid);
    XFreeGC(dpy, progress_gc);
#endif
}



void DrawUI(Widget w, nsPluginInstance * instance, char *message,
	    int FullRedraw, int percent)
{
#ifdef X_ENABLED
    GC black_gc, white_gc;
    XGCValues values;
    const char *id = "mplayerplug-in v" PACKAGE_VERSION;
    unsigned int id_width;
    int top, font_height, loops;
    int term, chop;
    int PercentRedraw;

    if (0) {
	printf("Widget: %p\n", w);
	printf("Message: %s\n", message);
    }

    if (DEBUG > 1)
	printf("state = %i, height = %i width = %i\n", instance->state,
	       instance->movie_height, instance->movie_width);

    if (w == NULL)
	return;
    if (instance->state < STATE_READY)
	return;
    if (message == NULL)
	return;
    if (instance->display == NULL)
	return;
    if (instance->state == STATE_PLAYING && instance->movie_height > 0
	&& instance->movie_width > 0)
	return;

    sprintf(instance->lastmessage, "%s", message);

    values.foreground =
	BlackPixel((Display *) instance->display,
		   DefaultScreen((Display *) instance->display));
    black_gc =
	XCreateGC((Display *) instance->display,
		  (Drawable) instance->window, GCForeground, &values);
    values.foreground =
	WhitePixel((Display *) instance->display,
		   DefaultScreen((Display *) instance->display));
    white_gc =
	XCreateGC((Display *) instance->display,
		  (Drawable) instance->window, GCForeground, &values);

    PercentRedraw = FALSE;
    if (percent != -1 && instance->lastpercent != -1
	&& instance->lastpercent > percent)
	PercentRedraw = TRUE;
    if (instance->font == NULL) {
	FullRedraw = TRUE;
	Initialize((Display *) instance->display,
		   (Drawable) instance->window, instance, black_gc);
    }

    XFontSetExtents *extent;
    extent = XExtentsOfFontSet(instance->font);
    font_height = extent->max_logical_extent.height;
    id_width = XmbTextEscapement(instance->font, id, strlen(id));

    if (FullRedraw || instance->window_height <= 80) {
	XFillRectangle((Display *) instance->display,
		       (Drawable) instance->window, white_gc, 0, 0,
		       instance->window_width, instance->window_height);
    } else {
	XFillRectangle((Display *) instance->display,
		       (Drawable) instance->window, white_gc, BORDER,
		       LOGO_HEIGHT, instance->window_width - BORDER * 2,
		       instance->window_height - font_height -
		       LOGO_HEIGHT - BORDER - 10);
    }

    if (instance->window_height > 80
	&& instance->window_width > id_width + BORDER * 2) {
	top = BORDER * 2 + LOGO_HEIGHT;
	loops = 0;
	if (FullRedraw) {
	    DrawLogo((Display *) instance->display,
		     (Drawable) instance->window, instance, black_gc);
	    XmbDrawString((Display *) instance->display,
			  (Drawable) instance->window, instance->font,
			  black_gc,
			  instance->window_width - id_width - BORDER,
			  instance->window_height - font_height, id,
			  strlen(id));
	}
	if ((FullRedraw || PercentRedraw)
	    && instance->window_width > id_width + BORDER * 3 + 100)
	    // The height is set to 11 (my value for font_height)
	    // since the progress image only has 11 pixels of space
	    DrawProgressBG((Display *) instance->display,
			   (Drawable) instance->window,
			   instance,
			   BORDER,
			   instance->window_height - font_height - BORDER,
			   instance->window_width - id_width - BORDER * 3,
			   11);


	// Loop a max of 10 times in case of problems (like too small of a width)
	while (strlen(message) != 0 && loops < 10) {
	    chop = 0;
	    term = 0;
	    while (XmbTextEscapement
		   (instance->font, message,
		    strlen(message) - chop) + BORDER * 2 >
		   (int) instance->window_width)
		chop++;
	    if (strchr(message, '\n') != NULL &&
		(int) (strchr(message, '\n') - message) <
		(int) (strlen(message) - chop)) {
		chop = strlen(message) - (strchr(message, '\n') - message);
		term = 1;
	    }
	    XmbDrawString((Display *) instance->display,
			  (Drawable) instance->window, instance->font,
			  black_gc, BORDER, top, message,
			  strlen(message) - chop);
	    message += strlen(message) - chop + term;
	    top += font_height;
	    loops++;
	    if (top > (int) instance->window_height - font_height - BORDER)
		break;
	}

	// Make sure we have at least 100 px in addition to the id tag and borders
	if (percent != -1
	    && instance->window_width > id_width + BORDER * 3 + 100)
	    DrawProgress((Display *) instance->display,
			 (Drawable) instance->window,
			 instance,
			 BORDER,
			 instance->window_height - font_height - BORDER,
			 instance->window_width - id_width - BORDER * 3,
			 11, percent);
	else if (FullRedraw && instance->lastpercent != -1
		 && instance->window_width > id_width + BORDER * 3 + 100)
	    DrawProgress((Display *) instance->display,
			 (Drawable) instance->window,
			 instance,
			 BORDER,
			 instance->window_height - font_height - BORDER,
			 instance->window_width - id_width - BORDER * 3,
			 11, instance->lastpercent);

    } else {
	XmbDrawString((Display *) instance->display,
		      (Drawable) instance->window, instance->font,
		      black_gc, BORDER, 15, message, strlen(message));
    }
    if (percent != -1)
	instance->lastpercent = percent;

    XFreeGC((Display *) instance->display, black_gc);
    XFreeGC((Display *) instance->display, white_gc);

    XFlush((Display *) instance->display);
    usleep(100);
#endif
}

void FreeUI(Display * dpy, nsPluginInstance * instance)
{
#ifdef X_ENABLED
    if (instance->font)
	XFreeFontSet(dpy, instance->font);
    // Free all the pixmaps used by the GUI
    if (instance->progress_left)
	XFreePixmap(dpy, instance->progress_left);
    if (instance->progress_leftmask)
	XFreePixmap(dpy, instance->progress_leftmask);
    if (instance->progress_middle)
	XFreePixmap(dpy, instance->progress_middle);
    if (instance->progress_middlemask)
	XFreePixmap(dpy, instance->progress_middlemask);
    if (instance->progress_right)
	XFreePixmap(dpy, instance->progress_right);
    if (instance->progress_rightmask)
	XFreePixmap(dpy, instance->progress_rightmask);
    if (instance->progress_fill)
	XFreePixmap(dpy, instance->progress_fill);
    if (instance->progress_fillmask)
	XFreePixmap(dpy, instance->progress_fillmask);
    if (instance->logo)
	XFreePixmap(dpy, instance->logo);
    if (instance->logomask)
	XFreePixmap(dpy, instance->logomask);
    // Don't free the font since we didn't create it
    instance->logo = (Pixmap) NULL;
    instance->logomask = (Pixmap) NULL;
    instance->progress_left = (Pixmap) NULL;
    instance->progress_leftmask = (Pixmap) NULL;
    instance->progress_middle = (Pixmap) NULL;
    instance->progress_middlemask = (Pixmap) NULL;
    instance->progress_right = (Pixmap) NULL;
    instance->progress_rightmask = (Pixmap) NULL;
    instance->progress_fill = (Pixmap) NULL;
    instance->progress_fillmask = (Pixmap) NULL;
    instance->font = (XFontSet) NULL;
#endif
}

void RedrawCB(Widget widget, XtPointer client_data, XtPointer call_data)
{
    char message[1024];

    nsPluginInstance *instance = (nsPluginInstance *) client_data;
    if (DEBUG)
	printf("redrawing window\n");

    if (instance->noredraw)
	return;

    if (instance != NULL) {
	if (instance->lastmessage != NULL) {
	    if (strlen(instance->lastmessage) != 0) {
		if (instance->state >= STATE_GETTING_PLAYLIST) {
		    strcpy(message, instance->lastmessage);
#ifdef X_ENABLED
		    DrawUI(instance->widget, instance, message, 1, -1);
#endif
		}
	    }
	}
    }
}

#ifdef GTK_ENABLED
void play_callback(GtkWidget * widget, GdkEventExpose * event,
		   nsPluginInstance * instance)
{
    if (DEBUG)
	printf("play clickd\n");

    if (instance == NULL)
	return;
    if (instance->panel_drawn == 0 && instance->controlsvisible == 1)
	return;
    if (instance->controlsvisible == 1) {
	if (instance->panel_height > 16)
	    instance->panel_height = 16;

	instance->redrawbuttons = 1;
	gtk_container_remove(GTK_CONTAINER(instance->play_event_box),
			     instance->image_play);
	gtk_container_remove(GTK_CONTAINER(instance->pause_event_box),
			     instance->image_pause);
	gtk_container_remove(GTK_CONTAINER(instance->stop_event_box),
			     instance->image_stop);
#ifdef GTK2_ENABLED
	instance->image_play =
	    gtk_image_new_from_pixbuf(instance->pb_sm_play_down);
	instance->image_pause =
	    gtk_image_new_from_pixbuf(instance->pb_sm_pause_up);
	instance->image_stop =
	    gtk_image_new_from_pixbuf(instance->pb_sm_stop_up);
#endif
#ifdef GTK1_ENABLED
	instance->image_play =
	    gtk_pixmap_new(instance->pb_sm_play_down, NULL);
	instance->image_pause =
	    gtk_pixmap_new(instance->pb_sm_pause_up, NULL);
	instance->image_stop =
	    gtk_pixmap_new(instance->pb_sm_stop_up, NULL);
#endif

	gtk_container_add(GTK_CONTAINER(instance->play_event_box),
			  instance->image_play);
	gtk_container_add(GTK_CONTAINER(instance->pause_event_box),
			  instance->image_pause);
	gtk_container_add(GTK_CONTAINER(instance->stop_event_box),
			  instance->image_stop);
	if (instance->showbuttons) {
	    gtk_widget_show(instance->image_play);
	    gtk_widget_show(instance->image_pause);
	    gtk_widget_show(instance->image_stop);

	    gtk_widget_show(instance->play_event_box);
	    gtk_widget_show(instance->pause_event_box);
	    gtk_widget_show(instance->stop_event_box);
	}
	gtk_widget_show(instance->fixed_container);
	if (instance->drawing_area != NULL) {
	    gtk_widget_hide(GTK_WIDGET(instance->drawing_area));
	    gtk_widget_show(GTK_WIDGET(instance->drawing_area));
	}
	if (GTK_IS_WIDGET(instance->src_event_box)
	    && instance->targetplayer == 0) {
	    gtk_widget_hide(instance->src_event_box);
	}
	gdk_flush();
    }
    if (widget != NULL)
	instance->Play();
}

void pause_callback(GtkWidget * widget, GdkEventExpose * event,
		    nsPluginInstance * instance)
{
    if (DEBUG)
	printf("pause clickd\n");

    if (instance == NULL)
	return;
    if (instance->panel_drawn == 0 && instance->controlsvisible == 1)
	return;
    if (instance->controlsvisible == 1) {
	if (instance->panel_height > 16)
	    instance->panel_height = 16;

	gtk_container_remove(GTK_CONTAINER(instance->play_event_box),
			     instance->image_play);
	gtk_container_remove(GTK_CONTAINER(instance->pause_event_box),
			     instance->image_pause);
	gtk_container_remove(GTK_CONTAINER(instance->stop_event_box),
			     instance->image_stop);

#ifdef GTK2_ENABLED
	instance->image_play =
	    gtk_image_new_from_pixbuf(instance->pb_sm_play_up);
	instance->image_pause =
	    gtk_image_new_from_pixbuf(instance->pb_sm_pause_down);
	instance->image_stop =
	    gtk_image_new_from_pixbuf(instance->pb_sm_stop_up);
#endif
#ifdef GTK1_ENABLED
	instance->image_play =
	    gtk_pixmap_new(instance->pb_sm_play_up, NULL);
	instance->image_pause =
	    gtk_pixmap_new(instance->pb_sm_pause_down, NULL);
	instance->image_stop =
	    gtk_pixmap_new(instance->pb_sm_stop_up, NULL);
#endif

	gtk_container_add(GTK_CONTAINER(instance->play_event_box),
			  instance->image_play);
	gtk_container_add(GTK_CONTAINER(instance->pause_event_box),
			  instance->image_pause);
	gtk_container_add(GTK_CONTAINER(instance->stop_event_box),
			  instance->image_stop);

	if (instance->showbuttons) {
	    gtk_widget_show(instance->image_play);
	    gtk_widget_show(instance->image_pause);
	    gtk_widget_show(instance->image_stop);

	    gtk_widget_show(instance->play_event_box);
	    gtk_widget_show(instance->pause_event_box);
	    gtk_widget_show(instance->stop_event_box);
	}
	gtk_widget_show(instance->fixed_container);
	if (instance->drawing_area != NULL)
	    gtk_widget_show(GTK_WIDGET(instance->drawing_area));
	gdk_flush();
    }
    if (widget != NULL)
	instance->Pause();
}

void stop_callback(GtkWidget * widget, GdkEventExpose * event,
		   nsPluginInstance * instance)
{
    if (DEBUG)
	printf("stop clickd\n");

    if (instance == NULL)
	return;
    if (instance->mInitialized == FALSE)
	return;
    if (instance->panel_drawn == 0 && instance->controlsvisible == 1)
	return;
    if (instance->controlsvisible == 1) {
	if (instance->panel_height > 16)
	    instance->panel_height = 16;

	gtk_container_remove(GTK_CONTAINER(instance->play_event_box),
			     instance->image_play);
	gtk_container_remove(GTK_CONTAINER(instance->pause_event_box),
			     instance->image_pause);
	gtk_container_remove(GTK_CONTAINER(instance->stop_event_box),
			     instance->image_stop);

#ifdef GTK2_ENABLED
	instance->image_play =
	    gtk_image_new_from_pixbuf(instance->pb_sm_play_up);
	instance->image_pause =
	    gtk_image_new_from_pixbuf(instance->pb_sm_pause_up);
	instance->image_stop =
	    gtk_image_new_from_pixbuf(instance->pb_sm_stop_down);
#endif
#ifdef GTK1_ENABLED
	instance->image_play =
	    gtk_pixmap_new(instance->pb_sm_play_up, NULL);
	instance->image_pause =
	    gtk_pixmap_new(instance->pb_sm_pause_up, NULL);
	instance->image_stop =
	    gtk_pixmap_new(instance->pb_sm_stop_down, NULL);
#endif
	gtk_container_add(GTK_CONTAINER(instance->play_event_box),
			  instance->image_play);
	gtk_container_add(GTK_CONTAINER(instance->pause_event_box),
			  instance->image_pause);
	gtk_container_add(GTK_CONTAINER(instance->stop_event_box),
			  instance->image_stop);
	if (instance->showbuttons) {
	    gtk_widget_show(instance->image_play);
	    gtk_widget_show(instance->image_pause);
	    gtk_widget_show(instance->image_stop);

	    gtk_widget_show(instance->play_event_box);
	    gtk_widget_show(instance->pause_event_box);
	    gtk_widget_show(instance->stop_event_box);
	}
	gtk_widget_show(instance->fixed_container);
	gtk_widget_hide(GTK_WIDGET(instance->progress_bar));
	if (instance->drawing_area != NULL) {
	    gtk_widget_hide(GTK_WIDGET(instance->drawing_area));
	    snprintf(instance->lastmessage, 1024, _("Stopped"));
	    g_idle_add(gtkgui_message, instance);
	    gtk_widget_hide(GTK_WIDGET(instance->mediaprogress_bar));
	}
	if (instance->showlogo)
	    if (GTK_IS_WIDGET(instance->image))
		gtk_widget_show(GTK_WIDGET(instance->image));

	if (!instance->hidestatus)
	    if (GTK_IS_WIDGET(instance->status))
		gtk_widget_show(GTK_WIDGET(instance->status));

	gdk_flush();
    }
    if (widget != NULL)
	instance->Stop();

}

void ff_callback(GtkWidget * widget, GdkEventExpose * event,
		 nsPluginInstance * instance)
{
    if (instance == NULL)
	return;
    if (instance->panel_drawn == 0 && instance->controlsvisible == 1)
	return;
    if (instance->controlsvisible == 1 && instance->mmsstream == 0) {
	if (instance->panel_height > 16)
	    instance->panel_height = 16;

	gtk_container_remove(GTK_CONTAINER(instance->ff_event_box),
			     instance->image_ff);
#ifdef GTK2_ENABLED
	instance->image_ff =
	    gtk_image_new_from_pixbuf(instance->pb_sm_ff_down);
#endif
#ifdef GTK1_ENABLED
	instance->image_ff = gtk_pixmap_new(instance->pb_sm_ff_down, NULL);
#endif

	gtk_container_add(GTK_CONTAINER(instance->ff_event_box),
			  instance->image_ff);
	if (instance->showbuttons) {
	    gtk_widget_show(instance->image_ff);
	    gtk_widget_show(instance->ff_event_box);
	}
	gdk_flush();
	instance->FastForward();
	usleep(500);

	gtk_container_remove(GTK_CONTAINER(instance->ff_event_box),
			     instance->image_ff);
#ifdef GTK2_ENABLED
	instance->image_ff =
	    gtk_image_new_from_pixbuf(instance->pb_sm_ff_up);
#endif
#ifdef GTK1_ENABLED
	instance->image_ff = gtk_pixmap_new(instance->pb_sm_ff_up, NULL);
#endif

	gtk_container_add(GTK_CONTAINER(instance->ff_event_box),
			  instance->image_ff);
	if (instance->showbuttons) {
	    gtk_widget_show(instance->image_ff);
	    gtk_widget_show(instance->ff_event_box);
	}
    }

}

void rew_callback(GtkWidget * widget, GdkEventExpose * event,
		  nsPluginInstance * instance)
{
    if (instance == NULL)
	return;
    if (instance->panel_drawn == 0 && instance->controlsvisible == 1)
	return;
    if (instance->controlsvisible == 1 && instance->mmsstream == 0) {

	if (instance->panel_height > 16)
	    instance->panel_height = 16;

	gtk_container_remove(GTK_CONTAINER(instance->rew_event_box),
			     instance->image_rew);
#ifdef GTK2_ENABLED
	instance->image_rew =
	    gtk_image_new_from_pixbuf(instance->pb_sm_rew_down);
#endif
#ifdef GTK1_ENABLED
	instance->image_rew =
	    gtk_pixmap_new(instance->pb_sm_rew_down, NULL);
#endif

	gtk_container_add(GTK_CONTAINER(instance->rew_event_box),
			  instance->image_rew);
	if (instance->showbuttons) {
	    gtk_widget_show(instance->image_rew);
	    gtk_widget_show(instance->rew_event_box);
	}
	gdk_flush();
	instance->FastReverse();
	usleep(500);

	gtk_container_remove(GTK_CONTAINER(instance->rew_event_box),
			     instance->image_rew);
#ifdef GTK2_ENABLED
	instance->image_rew =
	    gtk_image_new_from_pixbuf(instance->pb_sm_rew_up);
#endif
#ifdef GTK1_ENABLED
	instance->image_rew = gtk_pixmap_new(instance->pb_sm_rew_up, NULL);
#endif

	gtk_container_add(GTK_CONTAINER(instance->rew_event_box),
			  instance->image_rew);
	if (instance->showbuttons) {
	    gtk_widget_show(instance->image_rew);
	    gtk_widget_show(instance->rew_event_box);
	}
    }

}

gboolean vol_callback(GtkWidget * widget, GdkEvent * event,
		      nsPluginInstance * instance)
{
#ifdef GTK2_ENABLED
    if (event->type == GDK_SCROLL) {
	if (((GdkEventScroll *) event)->direction == GDK_SCROLL_UP) {
	    instance->VolumeUp();
	} else {
	    instance->VolumeDown();
	}
    }
#elif GTK1_ENABLED
    if (event->type == GDK_BUTTON_PRESS) {
        switch (((GdkEventButton *)event)->button) {
            case 4 :
                instance->VolumeUp();
                break;
            case 5 :
                instance->VolumeDown();
                break;
        }
    }
#endif
    return FALSE;
}

gboolean gtkgui_setvolumetip(void *data)
{
    char tiptext[1024];
    nsPluginInstance *instance;

    instance = (nsPluginInstance *) data;

    snprintf(tiptext, 1024, _("Volume %i%%"), instance->volume);

    if (instance->vol_tooltip != NULL) {
	gtk_tooltips_set_tip(instance->vol_tooltip,
			     instance->vol_event_box, tiptext, NULL);
    }

    return FALSE;
}


gboolean fs_callback(GtkWidget * widget, GdkEventExpose * event,
		     nsPluginInstance * instance)
{

    int fs_state;

    if (DEBUG)
	printf("fs_callback clicked\n");
    if (instance == NULL)
	return FALSE;


    if (instance->panel_drawn == 0 && instance->controlsvisible == 1)
	return FALSE;

    fs_state = instance->fullscreen;

    if (instance->controlsvisible == 1) {
	if (instance->panel_height > 16)
	    instance->panel_height = 16;

	gtk_container_remove(GTK_CONTAINER(instance->fs_event_box),
			     instance->image_fs);
	if (widget != NULL)
	    instance->SetFullscreen(!instance->fullscreen);

	if (instance->fullscreen == 0) {
#ifdef GTK2_ENABLED
	    instance->image_fs =
		gtk_image_new_from_pixbuf(instance->pb_sm_fs_up);
#endif
#ifdef GTK1_ENABLED
	    instance->image_fs =
		gtk_pixmap_new(instance->pb_sm_fs_up, NULL);
#endif
	} else {
#ifdef GTK2_ENABLED
	    instance->image_fs =
		gtk_image_new_from_pixbuf(instance->pb_sm_fs_down);
#endif
#ifdef GTK1_ENABLED
	    instance->image_fs =
		gtk_pixmap_new(instance->pb_sm_fs_down, NULL);
#endif


	}
	gtk_container_add(GTK_CONTAINER(instance->fs_event_box),
			  instance->image_fs);

	if (instance->showfsbutton && instance->showbuttons) {
	    gtk_widget_show(instance->image_fs);
	    gtk_widget_show(instance->fs_event_box);
	}
	gtk_widget_show(instance->fixed_container);
	gdk_flush();
    }
    // printf("****\nfs_state = %i\nfullscreen = %i\n****\n",fs_state,instance->fullscreen);
    while (gtk_events_pending())
	gtk_main_iteration();
    if (fs_state == 1 && instance->fullscreen == 0) {
	return TRUE;
    }

    return FALSE;

}

gboolean keyboard_callback(GtkWidget * widget, GdkEventKey * event,
			   nsPluginInstance * instance)
{
    if (DEBUG)
	printf("In keyboard_callback with %i\n", event->keyval);

    if ((event->keyval == GDK_P) || (event->keyval == GDK_p)
	|| (event->keyval == GDK_space)) {
	if (instance->paused) {
	    play_callback(widget, (GdkEventExpose *) NULL, instance);
	} else {
	    pause_callback(widget, (GdkEventExpose *) NULL, instance);
	}
	return TRUE;
    }

    if ((event->keyval == GDK_S) || (event->keyval == GDK_s)) {
	stop_callback(widget, (GdkEventExpose *) NULL, instance);
	return TRUE;
    }

    if ((event->keyval == GDK_less) || (event->keyval == GDK_comma)) {
	rew_callback(widget, (GdkEventExpose *) NULL, instance);
	return TRUE;
    }

    if ((event->keyval == GDK_greater) || (event->keyval == GDK_period)) {
	ff_callback(widget, (GdkEventExpose *) NULL, instance);
	return TRUE;
    }

    if ((event->keyval == GDK_F) || (event->keyval == GDK_f)) {
	fs_callback(widget, (GdkEventExpose *) NULL, instance);
	return TRUE;
    }

    if (event->keyval == GDK_9) {
        instance->VolumeDown();
    }

    if (event->keyval == GDK_0) {
        instance->VolumeUp();
    }


    return FALSE;
}

gboolean mouse_callback(GtkWidget * widget, GdkEventButton * event,
			nsPluginInstance * instance)
{
    char buffer1[1024];
    char buffer2[1024];
    char *p;

    if (DEBUG)
	printf("In mouse_callback\n");
    if (event->type == GDK_BUTTON_PRESS) {
	if (DEBUG)
	    printf("button press # %i\n", event->button);
	if (event->button == 1) {
	    if (instance->mouseClickCallback != NULL)
		NPN_GetURL(instance->mInstance,
			   instance->mouseClickCallback, "_self");
	}
	if (instance->mouseDownCallback != NULL) {
	    strlcpy(buffer1, instance->mouseDownCallback, 1024);
	    p = index(buffer1, '(');
	    if (p == NULL) {
		p = buffer1 + strlen(buffer1);
	    }
	    *p = '\0';
	    snprintf(buffer2, 1024, "%s(%i);", buffer1, event->button);
	    NPN_MemFree(instance->mouseDownCallback);
	    instance->mouseDownCallback =
		(char *) NPN_MemAlloc(strlen(buffer2));
	    strlcpy(instance->mouseDownCallback, buffer2, strlen(buffer2));
	    NPN_GetURL(instance->mInstance,
		       instance->mouseDownCallback, "_self");
	}
    }

    if (event->type == GDK_BUTTON_RELEASE) {
	if (DEBUG)
	    printf("button released # %i\n", event->button);
	if (instance->mouseUpCallback != NULL) {
	    strlcpy(buffer1, instance->mouseUpCallback, 1024);
	    p = index(buffer1, '(');
	    if (p == NULL) {
		p = buffer1 + strlen(buffer1);
	    }
	    *p = '\0';
	    snprintf(buffer2, 1024, "%s(%i);", buffer1, event->button);
	    NPN_MemFree(instance->mouseUpCallback);
	    instance->mouseUpCallback =
		(char *) NPN_MemAlloc(strlen(buffer2));
	    strlcpy(instance->mouseUpCallback, buffer2, strlen(buffer2));

	    NPN_GetURL(instance->mInstance,
		       instance->mouseUpCallback, "_self");
	}
    }

    return FALSE;
}

gboolean mediaprogress_callback(GtkWidget * widget, GdkEventButton * event,
				nsPluginInstance * instance)
{
    float percent;
    float bytepercent;
    float seektime;
    gint width;
    gint height;

#ifdef GTK2_ENABLED    
    gdk_drawable_get_size(GDK_DRAWABLE(widget->window),&width,&height);
#endif
#ifdef GTK1_ENABLED
    gdk_window_get_size(widget->window,&width,&height);
#endif
    percent = (float)event->x / (float)width;

    if (instance->currentnode != NULL) {
	if (instance->currentnode->totalbytes != 0) {
	    bytepercent =
		(float) instance->currentnode->bytes /
		(float) instance->currentnode->totalbytes;
	    if (percent > bytepercent) {
		percent = bytepercent - 0.05;
	    }
	}
    }
    seektime = instance->mediaLength * percent;


    if (DEBUG) {
	printf("widget size: %i x %i\n", width, height);
	printf("mouse click at %f x %f\n", event->x, event->y);
	printf("percent = %f\nseektime = %f\n", percent, seektime);
    }
    if (seektime > 0)
	instance->Seek(seektime);

    return TRUE;
}


gboolean mousenotify_callback(GtkWidget * widget, GdkEventCrossing * event,
			      nsPluginInstance * instance)
{
    if (event->type == GDK_ENTER_NOTIFY) {
	if (instance->mouseEnterCallback != NULL)
	    NPN_GetURL(instance->mInstance,
		       instance->mouseEnterCallback, "_self");

    }

    if (event->type == GDK_LEAVE_NOTIFY) {
	if (instance->mouseLeaveCallback != NULL)
	    NPN_GetURL(instance->mInstance,
		       instance->mouseLeaveCallback, "_self");

    }

    return FALSE;
}

gboolean gtkgui_draw(void *data)
{
    nsPluginInstance *instance;
    GtkTooltips *tooltip;

    instance = (nsPluginInstance *) data;

    int width, height;
    int multiplier;
    int win_height, win_width;

    if (instance == NULL)
	return FALSE;
    if (DEBUG > 1)
	printf("panel_height = %i\nDrawn = %i\n", instance->panel_height,
	       instance->panel_drawn);

    if (instance->mode == NP_EMBED && instance->window == 0)
	return FALSE;

    if (instance->mode == NP_EMBED) {
	win_height = instance->embed_height;
	win_width = instance->embed_width;
    } else {
	win_height = instance->window_height;
	win_width = instance->window_width;
    }

    if (instance->targetplayer == 1) {
	win_height = instance->movie_height + 16;
	win_width = instance->movie_width;
	if (instance->showcontrols && instance->movie_height)
	    instance->panel_height = 16;
    }

    if (instance->panel_drawn == 0 && instance->window != 0) {
	if (instance->mode == NP_FULL)
	    instance->panel_height = instance->window_height;
	if (instance->panel_height > 16)
	    instance->panel_height = 16;
	if (instance->panel_height <= 0) {
	    if (instance->showcontrols) {
		instance->panel_height = 16;
	    } else {
		instance->panel_height = 0;
	    }
	}
	if (DEBUG) {
	    printf("drawing panel\n height = %i panel= %i\n",
		   win_height, instance->panel_height);
	    printf("showbuttons = %i\nshowcontrols = %i\n",
		   instance->showbuttons, instance->showcontrols);
	}
	if (instance->panel_height > 0 && instance->showcontrols == 1) {
#ifdef GTK2_ENABLED
	    if (instance->autostart) {
		instance->image_play =
		    gtk_image_new_from_pixbuf(instance->pb_sm_play_down);
		instance->image_stop =
		    gtk_image_new_from_pixbuf(instance->pb_sm_stop_up);
	    } else {
		instance->image_play =
		    gtk_image_new_from_pixbuf(instance->pb_sm_play_up);
		instance->image_stop =
		    gtk_image_new_from_pixbuf(instance->pb_sm_stop_down);
	    }
	    instance->image_pause =
		gtk_image_new_from_pixbuf(instance->pb_sm_pause_up);

	    instance->image_ff =
		gtk_image_new_from_pixbuf(instance->pb_sm_ff_up);
	    instance->image_rew =
		gtk_image_new_from_pixbuf(instance->pb_sm_rew_up);
	    instance->image_vol =
		gtk_image_new_from_pixbuf(instance->pb_sm_vol);
	    instance->image_fs =
		gtk_image_new_from_pixbuf(instance->pb_sm_fs_up);
#endif
#ifdef GTK1_ENABLED
	    if (instance->autostart) {
		instance->image_play =
		    gtk_pixmap_new(instance->pb_sm_play_down, NULL);
		instance->image_stop =
		    gtk_pixmap_new(instance->pb_sm_stop_up, NULL);

	    } else {
		instance->image_play =
		    gtk_pixmap_new(instance->pb_sm_play_up, NULL);
		instance->image_stop =
		    gtk_pixmap_new(instance->pb_sm_stop_down, NULL);

	    }
	    instance->image_pause =
		gtk_pixmap_new(instance->pb_sm_pause_up, NULL);
	    instance->image_ff =
		gtk_pixmap_new(instance->pb_sm_ff_up, NULL);
	    instance->image_rew =
		gtk_pixmap_new(instance->pb_sm_rew_up, NULL);
	    instance->image_vol =
		gtk_pixmap_new(instance->pb_sm_vol, NULL);
	    instance->image_fs =
		gtk_pixmap_new(instance->pb_sm_fs_up, NULL);
#endif
	    width = SMALL_BUTTON_WIDTH;
	    height = SMALL_BUTTON_HEIGHT;

#ifdef GTK2_ENABLED
	    g_signal_connect(G_OBJECT(instance->gtkwidget),
			     "key_press_event",
			     G_CALLBACK(keyboard_callback), instance);
#endif
#ifdef GTK1_ENABLED
	    gtk_signal_connect(GTK_OBJECT(instance->gtkwidget),
			       "key-press-event",
			       GTK_SIGNAL_FUNC(keyboard_callback),
			       instance);
#endif
	    multiplier = 0;
	    // rewind
	    if ((instance->mmsstream == 0 && win_width > 126)
		|| instance->targetplayer == 1) {
		instance->rew_event_box = gtk_event_box_new();
		tooltip = gtk_tooltips_new();
		gtk_tooltips_set_tip(tooltip, instance->rew_event_box,
				     _("Rewind"), NULL);
		gtk_widget_set_events(instance->rew_event_box,
				      GDK_BUTTON_PRESS_MASK);
#ifdef GTK2_ENABLED
		g_signal_connect(G_OBJECT(instance->rew_event_box),
				 "button_press_event",
				 G_CALLBACK(rew_callback), instance);
		gtk_widget_set_size_request(GTK_WIDGET
					    (instance->rew_event_box),
					    width, height);
#endif
#ifdef GTK1_ENABLED
		gtk_signal_connect(GTK_OBJECT(instance->rew_event_box),
				   "button_press_event",
				   GTK_SIGNAL_FUNC(rew_callback),
				   instance);
		gtk_widget_set_usize(GTK_WIDGET(instance->rew_event_box),
				     width, height);
#endif
		gtk_container_add(GTK_CONTAINER(instance->rew_event_box),
				  instance->image_rew);
		gtk_fixed_put(GTK_FIXED(instance->fixed_container),
			      instance->rew_event_box,
			      width * multiplier++, win_height - height);

		if (instance->showbuttons) {
		    gtk_widget_show(instance->image_rew);
		    gtk_widget_show(instance->rew_event_box);
		}
	    }
	    // play
	    instance->play_event_box = gtk_event_box_new();
	    tooltip = gtk_tooltips_new();
	    gtk_tooltips_set_tip(tooltip, instance->play_event_box,
				 _("Play"), NULL);
	    gtk_widget_set_events(instance->play_event_box,
				  GDK_BUTTON_PRESS_MASK);
#ifdef GTK2_ENABLED
	    g_signal_connect(G_OBJECT(instance->play_event_box),
			     "button_press_event",
			     G_CALLBACK(play_callback), instance);
	    gtk_widget_set_size_request(GTK_WIDGET
					(instance->play_event_box), width,
					height);
#endif
#ifdef GTK1_ENABLED
	    gtk_signal_connect(GTK_OBJECT(instance->play_event_box),
			       "button_press_event",
			       GTK_SIGNAL_FUNC(play_callback), instance);
	    gtk_widget_set_usize(GTK_WIDGET(instance->play_event_box),
				 width, height);
#endif
	    gtk_container_add(GTK_CONTAINER(instance->play_event_box),
			      instance->image_play);
	    gtk_fixed_put(GTK_FIXED(instance->fixed_container),
			  instance->play_event_box, width * multiplier++,
			  win_height - height);
	    if (instance->showbuttons) {
		gtk_widget_show(instance->image_play);
		gtk_widget_show(instance->play_event_box);
	    }
	    // pause
	    instance->pause_event_box = gtk_event_box_new();
	    tooltip = gtk_tooltips_new();
	    gtk_tooltips_set_tip(tooltip, instance->pause_event_box,
				 _("Pause"), NULL);
	    gtk_widget_set_events(instance->pause_event_box,
				  GDK_BUTTON_PRESS_MASK);
#ifdef GTK2_ENABLED
	    g_signal_connect(G_OBJECT(instance->pause_event_box),
			     "button_press_event",
			     G_CALLBACK(pause_callback), instance);
	    gtk_widget_set_size_request(GTK_WIDGET
					(instance->pause_event_box), width,
					height);
#endif
#ifdef GTK1_ENABLED
	    gtk_signal_connect(GTK_OBJECT(instance->pause_event_box),
			       "button_press_event",
			       GTK_SIGNAL_FUNC(pause_callback), instance);
	    gtk_widget_set_usize(GTK_WIDGET(instance->pause_event_box),
				 width, height);
#endif
	    gtk_container_add(GTK_CONTAINER(instance->pause_event_box),
			      instance->image_pause);
	    if (win_width > 126 || instance->targetplayer == 1) {

		gtk_fixed_put(GTK_FIXED(instance->fixed_container),
			      instance->pause_event_box,
			      width * multiplier++, win_height - height);
		if (instance->showbuttons) {
		    gtk_widget_show(instance->image_pause);
		    gtk_widget_show(instance->pause_event_box);
		}
	    }
	    // stop
	    instance->stop_event_box = gtk_event_box_new();
	    tooltip = gtk_tooltips_new();
	    gtk_tooltips_set_tip(tooltip, instance->stop_event_box,
				 _("Stop"), NULL);
	    gtk_widget_set_events(instance->stop_event_box,
				  GDK_BUTTON_PRESS_MASK);
#ifdef GTK2_ENABLED
	    g_signal_connect(G_OBJECT(instance->stop_event_box),
			     "button_press_event",
			     G_CALLBACK(stop_callback), instance);
	    gtk_widget_set_size_request(GTK_WIDGET
					(instance->stop_event_box), width,
					height);
#endif
#ifdef GTK1_ENABLED
	    gtk_signal_connect(GTK_OBJECT(instance->stop_event_box),
			       "button_press_event",
			       GTK_SIGNAL_FUNC(stop_callback), instance);
	    gtk_widget_set_usize(GTK_WIDGET(instance->stop_event_box),
				 width, height);
#endif
	    gtk_container_add(GTK_CONTAINER(instance->stop_event_box),
			      instance->image_stop);
	    gtk_fixed_put(GTK_FIXED(instance->fixed_container),
			  instance->stop_event_box, width * multiplier++,
			  win_height - height);
	    if (instance->showbuttons) {
		gtk_widget_show(instance->image_stop);
		gtk_widget_show(instance->stop_event_box);
	    }
	    // fastforward
	    if ((instance->mmsstream == 0 && win_width > 126)
		|| instance->targetplayer == 1) {
		instance->ff_event_box = gtk_event_box_new();
		tooltip = gtk_tooltips_new();
		gtk_tooltips_set_tip(tooltip, instance->ff_event_box,
				     _("Fast Forward"), NULL);
		gtk_widget_set_events(instance->ff_event_box,
				      GDK_BUTTON_PRESS_MASK);
#ifdef GTK2_ENABLED
		g_signal_connect(G_OBJECT(instance->ff_event_box),
				 "button_press_event",
				 G_CALLBACK(ff_callback), instance);
		gtk_widget_set_size_request(GTK_WIDGET
					    (instance->ff_event_box),
					    width, height);
#endif
#ifdef GTK1_ENABLED
		gtk_signal_connect(GTK_OBJECT(instance->ff_event_box),
				   "button_press_event",
				   GTK_SIGNAL_FUNC(ff_callback), instance);
		gtk_widget_set_usize(GTK_WIDGET(instance->ff_event_box),
				     width, height);
#endif
		gtk_container_add(GTK_CONTAINER(instance->ff_event_box),
				  instance->image_ff);
		gtk_fixed_put(GTK_FIXED(instance->fixed_container),
			      instance->ff_event_box, width * multiplier++,
			      win_height - height);
		if (instance->showbuttons) {
		    gtk_widget_show(instance->image_ff);
		    gtk_widget_show(instance->ff_event_box);
		}
	    }
//          if ((int) (width * multiplier + 10) <
//              (int) (win_width - 10 - width)) {
	    instance->mediaprogress_bar =
		GTK_PROGRESS_BAR(gtk_progress_bar_new());
	    gtk_widget_set_events(GTK_WIDGET
				  (instance->mediaprogress_bar),
				  GDK_BUTTON_PRESS_MASK);
#ifdef GTK2_ENABLED
	    g_signal_connect(G_OBJECT(instance->mediaprogress_bar),
			     "button_press_event",
			     G_CALLBACK(mediaprogress_callback), instance);

            if ((win_width > width * multiplier - 20 - (width * 2)) && height > 2) {
		if ((win_width - width * multiplier - 20 - (width * 2)) > -1) {
	       		gtk_widget_set_size_request(GTK_WIDGET
					(instance->mediaprogress_bar),
					(win_width -
					 width * multiplier - 20 -
					 (width * 2)), height - 2);
		}
	    }
#endif
#ifdef GTK1_ENABLED
	    gtk_signal_connect(GTK_OBJECT(instance->mediaprogress_bar),
			       "button_press_event",
			       GTK_SIGNAL_FUNC(mediaprogress_callback),
			       instance);
	    gtk_widget_set_usize(GTK_WIDGET
				 (instance->mediaprogress_bar),
				 (win_width -
				  width * multiplier - 20 - (width * 2)),
				 height - 2);
#endif
	    gtk_fixed_put(GTK_FIXED(instance->fixed_container),
			  GTK_WIDGET(instance->mediaprogress_bar),
			  (width * multiplier + 10),
			  win_height - height + 1);
	    gtk_widget_realize(GTK_WIDGET(instance->mediaprogress_bar));
//          }

	    if (instance->mode == NP_EMBED && instance->noembed == 0)
		gtk_widget_hide(GTK_WIDGET(instance->progress_bar));

	    // volume
	    instance->vol_event_box = gtk_event_box_new();
	    instance->vol_tooltip = gtk_tooltips_new();
	    gtk_tooltips_set_tip(instance->vol_tooltip,
				 instance->vol_event_box, _("Volume"),
				 NULL);
	    gtk_widget_set_events(instance->vol_event_box,
				  GDK_BUTTON_PRESS_MASK);
#ifdef GTK2_ENABLED
	    g_signal_connect(G_OBJECT(instance->vol_event_box),
			     "scroll_event",
			     G_CALLBACK(vol_callback), instance);
	    gtk_widget_set_size_request(GTK_WIDGET
					(instance->vol_event_box), width,
					height);
#endif
#ifdef GTK1_ENABLED
	    gtk_signal_connect(GTK_OBJECT(instance->vol_event_box),
			       "button_press_event",
			       GTK_SIGNAL_FUNC(vol_callback), instance);
	    gtk_widget_set_usize(GTK_WIDGET(instance->vol_event_box),
				 width, height);
#endif
	    gtk_container_add(GTK_CONTAINER(instance->vol_event_box),
			      instance->image_vol);
	    if (win_width > 126 || instance->targetplayer == 1) {
		gtk_fixed_put(GTK_FIXED(instance->fixed_container),
			      instance->vol_event_box,
			      win_width - (width * 2),
			      win_height - height);
		if (instance->showbuttons) {
		    gtk_widget_show(instance->image_vol);
		    gtk_widget_show(instance->vol_event_box);
		}
	    }


	    // fullscreen
	    instance->fs_event_box = gtk_event_box_new();
	    tooltip = gtk_tooltips_new();
	    gtk_tooltips_set_tip(tooltip, instance->fs_event_box,
				 _("Full Screen"), NULL);
	    gtk_widget_set_events(instance->fs_event_box,
				  GDK_BUTTON_PRESS_MASK);
#ifdef GTK2_ENABLED
	    g_signal_connect(G_OBJECT(instance->fs_event_box),
			     "button_press_event",
			     G_CALLBACK(fs_callback), instance);
	    gtk_widget_set_size_request(GTK_WIDGET(instance->fs_event_box),
					width, height);
#endif
#ifdef GTK1_ENABLED
	    gtk_signal_connect(GTK_OBJECT(instance->fs_event_box),
			       "button_press_event",
			       GTK_SIGNAL_FUNC(fs_callback), instance);
	    gtk_widget_set_usize(GTK_WIDGET(instance->fs_event_box), width,
				 height);
#endif
	    gtk_container_add(GTK_CONTAINER(instance->fs_event_box),
			      instance->image_fs);
	    if (win_width > 126 || instance->targetplayer == 1) {
		gtk_fixed_put(GTK_FIXED(instance->fixed_container),
			      instance->fs_event_box,
			      win_width - width, win_height - height);
		if (instance->showfsbutton && instance->showbuttons) {
		    gtk_widget_show(instance->image_fs);
		    gtk_widget_show(instance->fs_event_box);
		}
	    }

	    gtk_widget_show(instance->fixed_container);
	    if (instance->targetplayer == 0)
		gtk_widget_show(instance->gtkwidget);

	    instance->controlsvisible = 1;
	    instance->panel_drawn = 1;
	}
    }
    return FALSE;
}

gboolean gtkgui_message(void *data)
{
    nsPluginInstance *instance;

    if (DEBUG > 1)
	printf("in gtkgui_message\n");

    instance = (nsPluginInstance *) data;
    if (instance == NULL)
	return FALSE;
    if (instance->mInitialized == FALSE)
	return FALSE;

    if (instance->status != NULL && instance->lastmessage != NULL)
	if (GTK_IS_LABEL(instance->status))
	    gtk_label_set_text(instance->status, instance->lastmessage);	//GTK_LABEL

    if (instance->js_state == JS_STATE_TRANSITIONING) {
	if (GTK_IS_WIDGET(instance->image) && instance->showlogo)
	    gtk_widget_show(GTK_WIDGET(instance->image));

	if (GTK_IS_WIDGET(instance->status) && (!(instance->hidestatus)))
	    gtk_widget_show(GTK_WIDGET(instance->status));
    }

    return FALSE;
}

gboolean gtkgui_progress(void *data)
{
    nsPluginInstance *instance;

    if (DEBUG > 1)
	printf("in gtkgui_progress\n");

    instance = (nsPluginInstance *) data;
    if (instance == NULL)
	return FALSE;
    if (instance->progress_bar != NULL) {
	if (GTK_IS_PROGRESS_BAR(instance->progress_bar)) {
	    if ((instance->percent > 0.0) && (instance->percent < 1.0)) {
		if (instance->movie_height == 0
		    && instance->movie_width == 0)
		    if (!(instance->hidestatus))
			gtk_widget_show(GTK_WIDGET
					(instance->progress_bar));
		gtk_progress_bar_update(instance->progress_bar,
					instance->percent);
	    }
	    if (instance->percent >= 0.99 || instance->percent == 0)
		gtk_widget_hide(GTK_WIDGET(instance->progress_bar));
	}
    }

    if (instance->js_state == JS_STATE_TRANSITIONING) {
	if (GTK_IS_WIDGET(instance->image) && instance->showlogo)
	    gtk_widget_show(GTK_WIDGET(instance->image));

	if (GTK_IS_WIDGET(instance->status) && (!(instance->hidestatus)))
	    gtk_widget_show(GTK_WIDGET(instance->status));
    }

    return FALSE;
}

gboolean gtkgui_resize(void *data)
{
    nsPluginInstance *instance;
    int new_width, new_height;

    instance = (nsPluginInstance *) data;
    if (instance == NULL)
	return FALSE;
    if (instance->mInitialized == FALSE)
	return FALSE;

    new_width = instance->movie_width;
    new_height = instance->movie_height;

    if (DEBUG > 1)
	printf("in resize method\nNew size is %i x %i\n",
	       instance->movie_width, instance->movie_height);
    if (instance->drawing_area != NULL) {
	if (GTK_IS_WIDGET(instance->drawing_area)) {
	    if (GTK_IS_WIDGET(instance->progress_bar))
		gtk_widget_hide(GTK_WIDGET(instance->progress_bar));
	    if (instance->mode == NP_FULL) {
		if ((instance->movie_width + 10 >= instance->window_width)
		    || (instance->movie_height + 100 >=
			instance->window_height)) {
		    if (instance->fullscreen == 0) {
			if (GTK_IS_CONTAINER(instance->fixed_container)) {
			    if (GTK_IS_WIDGET(instance->image))
				gtk_widget_hide(GTK_WIDGET
						(instance->image));
			    if (GTK_IS_WIDGET(instance->status))
				gtk_widget_hide(GTK_WIDGET
						(instance->status));
			    if (GTK_IS_WIDGET(instance->mediaprogress_bar))
				gtk_widget_hide(GTK_WIDGET
						(instance->
						 mediaprogress_bar));
			    instance->showtracker = 0;
			    if (gtk_widget_get_parent_window
				(instance->drawing_area) == NULL) {
				gtk_fixed_put(GTK_FIXED
					      (instance->fixed_container),
					      instance->drawing_area, 0,
					      0);
			    }
			}
		    }
		}

		if (instance->showcontrols) {
		    if (instance->movie_width > instance->window_width
			|| instance->movie_height >
			(instance->window_height - 17)) {
			if (instance->movie_width > instance->window_width) {
			    new_width = instance->window_width;
			    new_height =
				(instance->window_width *
				 (instance->movie_height -
				  17)) / instance->movie_width;
			}

			if (instance->movie_height >
			    (instance->window_height - 17)) {
			    new_height = (instance->window_height - 17);
			    new_width =
				((instance->window_height -
				  17) * instance->movie_width) /
				instance->movie_height;
			}

		    } else {
			new_width = instance->movie_width;
			new_height = instance->movie_height;

		    }
		} else {
		    if (instance->movie_width > instance->window_width
			|| instance->movie_height >
			instance->window_height) {
			if (instance->movie_width > instance->window_width) {
			    new_width = instance->window_width;
			    new_height =
				(instance->window_width *
				 instance->movie_height) /
				instance->movie_width;
			}

			if (instance->movie_height >
			    instance->window_height) {
			    new_height = instance->window_height;
			    new_width =
				(instance->window_height *
				 instance->movie_width) /
				instance->movie_height;
			}

		    } else {
			new_width = instance->movie_width;
			new_height = instance->movie_height;

		    }
		}
		if (new_width > 0 && new_height > 0 ) {
			gtk_widget_set_usize(instance->drawing_area,
				     	     new_width, new_height);
		}

		if ((instance->movie_width + 10) >= instance->window_width
		    || (instance->movie_height + 100) >=
		    instance->window_height) {
		    gtk_fixed_put(GTK_FIXED(instance->fixed_container),
				  instance->drawing_area, 0, 0);
		    if (GTK_IS_WIDGET(instance->image))
			gtk_widget_hide(GTK_WIDGET(instance->image));
		    if (GTK_IS_WIDGET(instance->status))
			gtk_widget_hide(GTK_WIDGET(instance->status));
		    if (GTK_IS_WIDGET(instance->mediaprogress_bar))
			gtk_widget_hide(GTK_WIDGET
					(instance->mediaprogress_bar));

		} else {
		    if (gtk_widget_get_parent_window
			(instance->drawing_area) == NULL) {
			gtk_fixed_put(GTK_FIXED(instance->fixed_container),
				      instance->drawing_area, 10, 100);
		    }
		    if (GTK_IS_WIDGET(instance->image))
			gtk_widget_show(GTK_WIDGET(instance->image));
		    if (GTK_IS_WIDGET(instance->status))
			gtk_widget_show(GTK_WIDGET(instance->status));
		}

	    } else {
		if (instance->targetplayer && instance->showcontrols) {
		    if (instance->movie_width > 0 && instance->movie_height > 0 )
			gtk_widget_set_usize(instance->drawing_area,
					 instance->movie_width,
					 instance->movie_height + 16);

		} else {
		    if (instance->showcontrols) {
			if (instance->movie_height >
			    (instance->window_height - 16)) {
			    gtk_widget_set_usize(GTK_WIDGET
						 (instance->drawing_area),
						 instance->movie_width,
						 instance->window_height -
						 16);
			} else {
			if (instance->movie_width > 0 && instance->movie_height > 0 )
			    gtk_widget_set_usize(GTK_WIDGET
						 (instance->drawing_area),
						 instance->movie_width,
						 instance->movie_height);
			}
		    } else {
			if (instance->movie_width > 0 && instance->movie_height > 0 )
			gtk_widget_set_usize(GTK_WIDGET
					     (instance->drawing_area),
					     instance->movie_width,
					     instance->movie_height);
		    }

		}

	    }
	}
    }


    if (instance->targetplayer == 1) {
#ifdef GTK2_ENABLED
	gtk_widget_set_size_request(instance->gtkwidget,
				    instance->movie_width,
				    instance->movie_height);
	gtk_widget_set_size_request(instance->drawing_area,
				    instance->movie_width,
				    instance->movie_height);
	gtk_window_resize(GTK_WINDOW(instance->gtkwidget),
			  instance->movie_width,
			  instance->movie_height + 16);
#endif
#ifdef GTK1_ENABLED
	gtk_widget_set_usize(instance->gtkwidget,
			     instance->movie_width,
			     instance->movie_height + 16);
	gtk_widget_set_usize(instance->drawing_area,
			     instance->movie_width,
			     instance->movie_height + 16);
#endif
	instance->redrawbuttons = 1;
	gtkgui_updatebuttons(data);
    }

    return FALSE;

}

gboolean gtkgui_stop(void *data)
{
    nsPluginInstance *instance;

    if (DEBUG > 1)
	printf("in gtkgui_stop\n");

    instance = (nsPluginInstance *) data;
    if (instance == NULL)
	return FALSE;
    if (instance->mInitialized == FALSE)
	return FALSE;
    if (instance->cancelled == 1)
	return FALSE;
    stop_callback(NULL, NULL, instance);
    instance->SetFullscreen(0);
    if (DEBUG > 1)
	printf("completed gtkgui_stop\n");
    gtkgui_resize(data);
    gtk_widget_hide(instance->drawing_area);
    return FALSE;
}

gboolean gtkgui_save_enable(void *data)
{
    nsPluginInstance *instance;
#ifdef GTK2_ENABLED
    char *filename;
    char newlabel[1024];
    GtkLabel *label;
#endif
    if (DEBUG > 1)
	printf("in gtkgui_save_enable\n");

    instance = (nsPluginInstance *) data;
    if (instance == NULL)
	return FALSE;
    if (instance->mInitialized == FALSE)
	return FALSE;
    if (instance->currentnode == NULL)
	return FALSE;
    if (instance->js_state != JS_STATE_PLAYING)
	return FALSE;

    if (!isMms(instance->currentnode->url, instance->nomediacache)) {
	pthread_mutex_lock(&(instance->playlist_mutex));
	if (DEBUG > 1)
	    printf("Save Enable called retrieved = %i\n",
		   instance->currentnode->retrieved);
#ifdef GTK2_ENABLED
	filename = getURLFilename(instance->currentnode->url);
	if (filename != NULL) {
	    snprintf(newlabel, 1024, _("Save as %s/%s"),
		     instance->download_dir, filename);
	    NPN_MemFree(filename);

	} else {
	    snprintf(newlabel, 1024, _("Save"));
	}

	if (GTK_IS_BIN(instance->menuitem_save)) {
	    label =
		GTK_LABEL(gtk_bin_get_child
			  (GTK_BIN(instance->menuitem_save)));
	    gtk_label_set_text(label, newlabel);
	}
#endif
	pthread_mutex_unlock(&(instance->playlist_mutex));
	gtk_widget_set_sensitive(GTK_WIDGET(instance->menuitem_save),
				 instance->currentnode->retrieved);
    } else {
#ifdef GTK2_ENABLED
	snprintf(newlabel, 1024, _("Append URL to %s/playlist"),
		 instance->download_dir);
	label =
	    GTK_LABEL(gtk_bin_get_child(GTK_BIN(instance->menuitem_save)));
	gtk_label_set_text(label, newlabel);
#endif
	gtk_widget_set_sensitive(GTK_WIDGET(instance->menuitem_save),
				 TRUE);
    }
    return FALSE;
}


gboolean gtkgui_drawMediaProgress(void *data)
{
    nsPluginInstance *instance;
    int hour = 0, min = 0, length_hour = 0, length_min = 0;
    long int seconds, length_seconds;
    char display[35];

    if (DEBUG > 1)
	printf("in drawMediaProgress method\n");

    instance = (nsPluginInstance *) data;
    if (instance == NULL)
	return FALSE;
    if (instance->mInitialized == FALSE)
	return FALSE;


    if (instance->mediaprogress_bar != NULL) {
	if (GTK_IS_PROGRESS_BAR(instance->mediaprogress_bar)) {
	    if (((int) instance->mediaLength > 0)
		&& (instance->mediaPercent <= 100)) {
		if (instance->controlsvisible && instance->showtracker) {
		    gtk_widget_show(GTK_WIDGET
				    (instance->mediaprogress_bar));
		    gtk_progress_bar_update(instance->mediaprogress_bar,
					    instance->mediaPercent /
					    100.0);

#ifdef GTK2_ENABLED
		    if (((int) instance->mediaTime > 0)
			&& (instance->showtime == 1)) {
			seconds = (int) instance->mediaTime;
			if (seconds >= 3600) {
			    hour = seconds / 3600;
			    seconds = seconds - (hour * 3600);
			}
			if (seconds >= 60) {
			    min = seconds / 60;
			    seconds = seconds - (min * 60);
			}
			length_seconds = (int) instance->mediaLength;
			if (length_seconds >= 3600) {
			    length_hour = length_seconds / 3600;
			    length_seconds =
				length_seconds - (length_hour * 3600);
			}
			if (length_seconds >= 60) {
			    length_min = length_seconds / 60;
			    length_seconds =
				length_seconds - (length_min * 60);
			}

			if ((int) (instance->percent * 100) > 0
			    && (int) (instance->percent * 100) < 99
			    && instance->mmsstream == 0) {

			    if (hour == 0 && length_hour == 0) {
				snprintf(display, 30,
					 _
					 ("%2i:%02i / %2i:%02i | %2i%% \342\226\274"),
					 min, (int) seconds, length_min,
					 (int) length_seconds,
					 (int) (instance->percent * 100));
			    } else {
				snprintf(display, 30,
					 _
					 ("%i:%02i:%02i / %i:%02i:%02i | %2i%% \342\226\274"),
					 hour, min, (int) seconds,
					 length_hour, length_min,
					 (int) length_seconds,
					 (int) (instance->percent * 100));
			    }

			} else {

			    if (hour == 0 && length_hour == 0) {
				snprintf(display, 30,
					 "%2i:%02i / %2i:%02i", min,
					 (int) seconds, length_min,
					 (int) length_seconds);
			    } else {
				snprintf(display, 30,
					 "%i:%02i:%02i / %i:%02i:%02i",
					 hour, min, (int) seconds,
					 length_hour, length_min,
					 (int) length_seconds);
			    }

			}

			gtk_progress_bar_set_text(instance->
						  mediaprogress_bar,
						  display);
		    } else {
			if ((int) (instance->percent * 100) > 0
			    && (int) (instance->percent * 100) < 99
			    && instance->mmsstream == 0) {

			    snprintf(display, 30, _("%2i%% \342\226\274"),
				     (int) (instance->percent * 100));
			    if (gtk_progress_bar_get_text
				(instance->mediaprogress_bar) != NULL)
				gtk_progress_bar_set_text(instance->
							  mediaprogress_bar,
							  display);

			} else {

			    if (gtk_progress_bar_get_text
				(instance->mediaprogress_bar) != NULL)
				gtk_progress_bar_set_text(instance->
							  mediaprogress_bar,
							  NULL);
			}
		    }
#endif

		} else {
		    gtk_widget_hide(GTK_WIDGET
				    (instance->mediaprogress_bar));
		}
	    } else {
		gtk_widget_hide(GTK_WIDGET(instance->mediaprogress_bar));
	    }
	}
    }

    if (instance->movie_width != 0 && instance->movie_height != 0) {
	if (GTK_IS_WIDGET(instance->progress_bar))
	    gtk_widget_hide(GTK_WIDGET(instance->progress_bar));
    }

    return FALSE;
}

gboolean gtkgui_refreshbuttonstate(void *data)
{
    nsPluginInstance *instance;

    if (DEBUG > 1)
	printf("in refreshbuttonstate method\n");
    instance = (nsPluginInstance *) data;
    if (instance == NULL)
	return FALSE;
    if (instance->mInitialized == FALSE)
	return FALSE;
    if (instance->panel_drawn == 1 && instance->controlsvisible) {

	if (instance->mmsstream == 0) {
	    if (instance->rew_event_box != NULL) {
		if (instance->showcontrols && instance->showbuttons
		    && instance->controlsvisible) {
		    if (instance->redrawbuttons)
			gtk_widget_hide(GTK_WIDGET
					(instance->rew_event_box));
		    gtk_widget_show(GTK_WIDGET(instance->rew_event_box));
		}
	    }
	} else {
	    if (GTK_IS_WIDGET(instance->rew_event_box))
		gtk_widget_hide(GTK_WIDGET(instance->rew_event_box));
	}
	if (instance->mmsstream == 0) {
	    if (instance->ff_event_box != NULL) {
		if (instance->showcontrols && instance->showbuttons
		    && instance->controlsvisible) {
		    if (instance->redrawbuttons)
			gtk_widget_hide(GTK_WIDGET
					(instance->ff_event_box));
		    gtk_widget_show(GTK_WIDGET(instance->ff_event_box));
		}
	    }
	} else {
	    if (GTK_IS_WIDGET(instance->ff_event_box))
		gtk_widget_hide(GTK_WIDGET(instance->ff_event_box));
	}
	if (instance->showcontrols && instance->showbuttons
	    && instance->controlsvisible) {
	    if (instance->redrawbuttons) {
		gtk_widget_hide(GTK_WIDGET(instance->play_event_box));
		gtk_widget_hide(GTK_WIDGET(instance->pause_event_box));
		gtk_widget_hide(GTK_WIDGET(instance->stop_event_box));
		gtk_widget_hide(GTK_WIDGET(instance->fs_event_box));
	    }
	    gtk_widget_show(GTK_WIDGET(instance->play_event_box));
	    gtk_widget_show(GTK_WIDGET(instance->pause_event_box));
	    gtk_widget_show(GTK_WIDGET(instance->stop_event_box));
	    gtk_widget_show(GTK_WIDGET(instance->fs_event_box));
	}
    }

    instance->redrawbuttons = 0;
//    gtk_widget_queue_draw_area(instance->gtkwidget,0,0,instance->embed_width,instance->embed_height);

    return FALSE;
}

gboolean gtkgui_updatebuttons(void *data)
{
    nsPluginInstance *instance;
    int height, width, multiplier;
    int win_height, win_width;

    if (DEBUG > 1)
	printf("in updatebuttons method\n");

    instance = (nsPluginInstance *) data;

    if (instance == NULL)
	return FALSE;
    if (instance->mInitialized == FALSE)
	return FALSE;

    if (instance->mode == NP_EMBED) {
	win_height = instance->embed_height;
	win_width = instance->embed_width;
    } else {
	win_height = instance->window_height;
	win_width = instance->window_width;
    }

    if (instance->targetplayer) {
	win_height = instance->movie_height + 16;
	win_width = instance->movie_width;
    }

    if (DEBUG)
	printf("buttons are at %i x %i\n", win_height, win_width);

    if (instance->panel_drawn == 1) {
	height = 16;
	width = 21;

	multiplier = 0;

	if (instance->mmsstream == 0) {
	    if (instance->rew_event_box != NULL) {
		if (instance->showcontrols && instance->showbuttons) {
		    gtk_widget_show(GTK_WIDGET(instance->rew_event_box));
		    gtk_fixed_move(GTK_FIXED
				   (instance->fixed_container),
				   GTK_WIDGET(instance->rew_event_box),
				   width * multiplier++,
				   win_height - height);
		}
	    }
	} else {
	    if (GTK_IS_WIDGET(instance->rew_event_box)) {
		gtk_widget_hide(GTK_WIDGET(instance->rew_event_box));
	    }
	}
	gtk_fixed_move(GTK_FIXED(instance->fixed_container),
		       GTK_WIDGET(instance->play_event_box),
		       width * multiplier++, win_height - height);
	gtk_fixed_move(GTK_FIXED(instance->fixed_container),
		       GTK_WIDGET(instance->pause_event_box),
		       width * multiplier++, win_height - height);
	gtk_fixed_move(GTK_FIXED(instance->fixed_container),
		       GTK_WIDGET(instance->stop_event_box),
		       width * multiplier++, win_height - height);
	if (instance->mmsstream == 0) {
	    if (instance->ff_event_box != NULL) {
		if (instance->showcontrols && instance->showbuttons) {
		    gtk_widget_show(GTK_WIDGET(instance->rew_event_box));
		    gtk_fixed_move(GTK_FIXED
				   (instance->fixed_container),
				   GTK_WIDGET(instance->ff_event_box),
				   width * multiplier++,
				   win_height - height);
		}
	    }
	} else {
	    if (GTK_IS_WIDGET(instance->ff_event_box)) {
		gtk_widget_hide(GTK_WIDGET(instance->ff_event_box));
	    }
	}
	gtk_fixed_move(GTK_FIXED(instance->fixed_container),
		       GTK_WIDGET(instance->mediaprogress_bar),
		       (width * multiplier + 10), win_height - height + 2);
	gtk_fixed_move(GTK_FIXED(instance->fixed_container),
		       GTK_WIDGET(instance->vol_event_box),
		       (win_width - (width * 2)), win_height - height + 2);
	gtk_fixed_move(GTK_FIXED(instance->fixed_container),
		       GTK_WIDGET(instance->fs_event_box),
		       (win_width - width), win_height - height + 2);

	gtk_widget_show(GTK_WIDGET(instance->mediaprogress_bar));
	gtk_widget_set_usize(GTK_WIDGET(instance->mediaprogress_bar),
			     win_width - (width * (multiplier + 2) +
					  20), height - 4);

    }
    return FALSE;
}

gboolean gtkgui_updatefullscreen(void *data)
{
    nsPluginInstance *instance;
    int fs_state;

    if (DEBUG > 1)
	printf("in updatefullscreen method\n");

    instance = (nsPluginInstance *) data;

    if (instance == NULL)
	return FALSE;
    if (instance->mInitialized == FALSE)
	return FALSE;

    fs_state = instance->fullscreen;
    instance->SetFullscreen(instance->fullscreen);

    // printf("****\nfs_state = %i\nfullscreen = %i\n****\n",fs_state,instance->fullscreen);
    if (fs_state == 1 && instance->fullscreen == 0) {
	return TRUE;
    }
    return FALSE;
}


gint popup_handler(GtkWidget * widget, GdkEvent * event)
{
    GtkMenu *menu;
    GdkEventButton *event_button;

    if (DEBUG)
	printf("popup menu callback\n");

    menu = GTK_MENU(widget);
    if (DEBUG)
	printf("event type # %i\n", event->type);
    if (event->type == GDK_BUTTON_PRESS) {
	event_button = (GdkEventButton *) event;
	if (DEBUG)
	    printf("button press # %i\n", event_button->button);
	if (event_button->button == 3) {
	    gtk_menu_popup(menu, NULL, NULL, NULL, NULL,
			   event_button->button, event_button->time);
	    return TRUE;
	}
    }
    return FALSE;
}

void menuitem_play_callback(GtkMenuItem * menuitem,
			    nsPluginInstance * instance)
{
    play_callback(GTK_WIDGET(menuitem), NULL, instance);
}

void menuitem_pause_callback(GtkMenuItem * menuitem,
			     nsPluginInstance * instance)
{
    pause_callback(GTK_WIDGET(menuitem), NULL, instance);
}

void menuitem_stop_callback(GtkMenuItem * menuitem,
			    nsPluginInstance * instance)
{
    stop_callback(GTK_WIDGET(menuitem), NULL, instance);
}

void menuitem_showcontrols_callback(GtkCheckMenuItem * menuitem,
				    nsPluginInstance * instance)
{
#ifdef GTK2_ENABLED
    instance->SetShowControls(gtk_check_menu_item_get_active(menuitem));
#endif
#ifdef GTK1_ENABLED
    instance->SetShowControls(menuitem->active);
#endif
}

void menuitem_fullscreen_callback(GtkCheckMenuItem * menuitem,
				  nsPluginInstance * instance)
{
#ifdef GTK2_ENABLED
    instance->SetFullscreen(gtk_check_menu_item_get_active(menuitem));
#endif
#ifdef GTK1_ENABLED
    instance->SetFullscreen(menuitem->active);
#endif

}

void store_filename(GtkWidget * widget, nsPluginInstance * instance)
{
    const gchar *filename;
    FILE *fin, *fout;
    char buffer[1000];
    int count;
    Node *n, *lastplayed;

    if (instance->mInitialized == FALSE)
	return;
    pthread_mutex_lock(&(instance->playlist_mutex));

    lastplayed = NULL;

    if (instance->currentnode != NULL) {
	lastplayed = instance->currentnode;
    } else {
	n = instance->list;
	while (n != NULL) {
	    if (n->played == 1)
		lastplayed = n;
	    n = n->next;
	}
    }

    if (DEBUG)
	printf("lastplayed = %p\n", lastplayed);

    if (lastplayed != NULL) {
	filename =
	    gtk_file_selection_get_filename(GTK_FILE_SELECTION
					    (instance->file_selector));
	if (DEBUG)
	    printf("filename = %s\n", filename);

	if (filename) {
	    if (lastplayed->retrieved == 1) {
		if (DEBUG) {
		    printf("lastplayed->fname = %s\n", lastplayed->fname);
		}

		if (rename(lastplayed->fname, filename) == 0) {
		    snprintf(lastplayed->fname, 1024, "%s", filename);
		    lastplayed->remove = 0;
		} else {

		    // rename failed (probably cross partitions), copy the file
		    fin = fopen(lastplayed->fname, "rb");
		    fout = fopen(filename, "wb");
		    if (fin != NULL && fout != NULL) {
			while (!feof(fin)) {
			    count = fread(buffer, 1, 1000, fin);
			    fwrite(buffer, 1, count, fout);
			}
			fclose(fout);
			fclose(fin);
		    }
		}
	    }
	}
    }
    pthread_mutex_unlock(&(instance->playlist_mutex));
    gtk_widget_destroy(instance->file_selector);
    instance->file_selector = NULL;
}



void menuitem_save_callback(GtkMenuItem * menuitem,
			    nsPluginInstance * instance)
{
    const gchar *filename;
    char mmsplaylist[1024];
    FILE *playlist;
    Node *n, *lastplayed;

    if (instance->mInitialized == FALSE)
	return;
    pthread_mutex_lock(&(instance->playlist_mutex));

    lastplayed = NULL;

    if (instance->currentnode != NULL) {
	lastplayed = instance->currentnode;
    } else {
	n = instance->list;
	while (n != NULL) {
	    if (n->played == 1)
		lastplayed = n;
	    n = n->next;
	}
    }

    if (lastplayed != NULL) {
	filename = getURLFilename(lastplayed->url);
	if (filename) {
	    instance->file_selector =
		gtk_file_selection_new(_("Save As..."));

	    gtk_file_selection_set_filename(GTK_FILE_SELECTION
					    (instance->file_selector),
					    filename);
#ifdef GTK2_ENABLED
	    g_signal_connect(GTK_FILE_SELECTION(instance->file_selector)->
			     ok_button, "clicked",
			     G_CALLBACK(store_filename), instance);

	    g_signal_connect_swapped(GTK_FILE_SELECTION
				     (instance->file_selector)->ok_button,
				     "clicked",
				     G_CALLBACK(gtk_widget_destroy),
				     instance->file_selector);

	    g_signal_connect_swapped(GTK_FILE_SELECTION
				     (instance->file_selector)->
				     cancel_button, "clicked",
				     G_CALLBACK(gtk_widget_destroy),
				     instance->file_selector);
#endif
#ifdef GTK1_ENABLED
	    gtk_signal_connect(GTK_OBJECT
			       (GTK_FILE_SELECTION
				(instance->file_selector)->ok_button),
			       "clicked", GTK_SIGNAL_FUNC(store_filename),
			       instance);

	    gtk_signal_connect_object(GTK_OBJECT
				      (GTK_FILE_SELECTION
				       (instance->file_selector)->
				       ok_button), "clicked",
				      GTK_SIGNAL_FUNC(gtk_widget_destroy),
				      GTK_OBJECT(instance->file_selector));

	    gtk_signal_connect_object(GTK_OBJECT
				      (GTK_FILE_SELECTION
				       (instance->file_selector)->
				       cancel_button), "clicked",
				      GTK_SIGNAL_FUNC(gtk_widget_destroy),
				      GTK_OBJECT(instance->file_selector));


#endif
	    /* Display that dialog */

	    gtk_widget_show(instance->file_selector);

	}
	if (lastplayed->mmsstream) {
	    snprintf(mmsplaylist, sizeof(mmsplaylist),
		     "%s/playlist", instance->download_dir);
	    playlist = fopen(mmsplaylist, "a");
	    if (playlist != NULL) {
		fprintf(playlist, "%s\n", instance->list->url);
		fclose(playlist);
	    }
	}
    }
    pthread_mutex_unlock(&(instance->playlist_mutex));
}

void menuitem_copy_callback(GtkMenuItem * menuitem,
			    nsPluginInstance * instance)
{
    Node *n, *lastplayed;
    char *localurl;
#ifdef GTK2_ENABLED
    GtkClipboard *clipboard;
#endif
#ifdef GTK1_ENABLED
    GtkWidget *ge;
#endif
    localurl = (char *) malloc(1024);

    lastplayed = NULL;

    if (instance->currentnode != NULL) {
	lastplayed = instance->currentnode;
    } else {
	n = instance->list;
	while (n != NULL) {
	    if (n->played == 1)
		lastplayed = n;
	    n = n->next;
	}
    }

    if (lastplayed != NULL) {
	fullyQualifyURL(instance, lastplayed->url, localurl);
#ifdef GTK2_ENABLED
	clipboard = gtk_clipboard_get(GDK_SELECTION_PRIMARY);
	gtk_clipboard_set_text(clipboard, localurl, -1);
	clipboard = gtk_clipboard_get(GDK_SELECTION_CLIPBOARD);
	gtk_clipboard_set_text(clipboard, localurl, -1);
#endif
#ifdef GTK1_ENABLED
	ge = gtk_entry_new_with_max_length(strlen(localurl));
	gtk_entry_set_text((GtkEntry *) ge, localurl);
	gtk_editable_select_region((GtkEditable *) ge, 0,
				   strlen(localurl));
	gtk_editable_copy_clipboard((GtkEditable *) ge);
#endif
    }
}

gint CloseConfig(GtkWidget * widget, nsPluginInstance * instance)
{

    if (GTK_IS_WIDGET(instance->conf_window))
	gtk_widget_destroy(instance->conf_window);
    instance->conf_window = NULL;

    return (FALSE);

}

gint CloseConfigEvent(GtkWidget * widget, GdkEvent * event,
		      nsPluginInstance * instance)
{

    if (GTK_IS_WIDGET(instance->conf_window))
	gtk_widget_destroy(instance->conf_window);
    instance->conf_window = NULL;
    return (FALSE);

}

#ifdef GTK2_ENABLED
gint ApplyConfig(GtkWidget * widget, nsPluginInstance * instance)
{
    FILE *input;
    FILE *output;
    char config_name[1000];
    char tmp_name[1000];
    char buffer[1000];
    int ret;

    instance->cachesize =
	(int) gtk_range_get_value(GTK_RANGE(instance->conf_cachesize));
    instance->cache_percent = (int)
	gtk_range_get_value(GTK_RANGE(instance->conf_cachepercent));

#ifdef GTK24_ENABLED
    if (instance->vo != NULL)
	free(instance->vo);
    instance->vo =
	strdup(gtk_entry_get_text
	       (GTK_ENTRY(GTK_BIN(instance->conf_vo)->child)));

    if (instance->ao != NULL)
	free(instance->ao);
    instance->ao =
	strdup(gtk_entry_get_text
	       (GTK_ENTRY(GTK_BIN(instance->conf_ao)->child)));

    if (instance->download_dir != NULL)
	free(instance->download_dir);
    instance->download_dir =
	strdup(gtk_entry_get_text
	       (GTK_ENTRY(GTK_BIN(instance->conf_dir)->child)));
#endif

    instance->showtime =
	gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON
				     (instance->conf_showtime));
    instance->enable_wmp =
	gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON
				     (instance->conf_enable_wmp));
    instance->enable_qt =
	gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON
				     (instance->conf_enable_qt));
    instance->enable_rm =
	gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON
				     (instance->conf_enable_rm));
    instance->enable_gmp =
	gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON
				     (instance->conf_enable_gmp));
    instance->enable_dvx =
	gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON
				     (instance->conf_enable_dvx));
    instance->enable_mpeg =
	gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON
				     (instance->conf_enable_mpeg));
    instance->enable_mp3 =
	gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON
				     (instance->conf_enable_mp3));
    instance->enable_ogg =
	gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON
				     (instance->conf_enable_ogg));
    instance->enable_midi =
	gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON
				     (instance->conf_enable_midi));
    instance->enable_pls =
	gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON
				     (instance->conf_enable_pls));
    instance->enable_smil =
	gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON
				     (instance->conf_enable_smil));
    instance->enable_helix =
	gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON
				     (instance->conf_enable_helix));

    instance->nomediacache =
	gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON
				     (instance->conf_nomediacache));

    instance->rtsp_use_tcp =
	gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON
				     (instance->conf_rtsptcp));

    instance->rtsp_use_http =
	gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON
				     (instance->conf_rtsphttp));

    instance->nopauseonhide =
	!gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON
				      (instance->conf_pauseonhide));

    gtk_widget_destroy(instance->conf_window);
    instance->conf_window = NULL;

    // write the setting to the conf file now

    snprintf(config_name, 1000, "%s", getenv("HOME"));
    strlcat(config_name, "/.mplayer/mplayerplug-in.conf", 1000);

    snprintf(tmp_name, 1000, "%s", getenv("HOME"));
    strlcat(tmp_name, "/.mplayer/mplayerplug-in.conf.tmp", 1000);

    if (DEBUG) {
	printf("config %s\n", config_name);
	printf("tmp %s\n", tmp_name);

    }

    input = fopen(config_name, "r");
    output = fopen(tmp_name, "w");

    if (input != NULL && output != NULL) {
	if (DEBUG)
	    printf("update existing config\n");
	if (strlen(instance->vo) > 0)
	    fprintf(output, "vo=%s\n", instance->vo);
	if (strlen(instance->ao) > 0)
	    fprintf(output, "ao=%s\n", instance->ao);
	fprintf(output, "cachesize=%i\n", instance->cachesize);
	fprintf(output, "cache-percent=%i\n", instance->cache_percent);
	if (strlen(instance->download_dir) > 0)
	    fprintf(output, "dload-dir=%s\n", instance->download_dir);
	fprintf(output, "showtime=%i\n", instance->showtime);
	fprintf(output, "enable-wmp=%i\n", instance->enable_wmp);
	fprintf(output, "enable-qt=%i\n", instance->enable_qt);
	fprintf(output, "enable-rm=%i\n", instance->enable_rm);
	fprintf(output, "enable-gmp=%i\n", instance->enable_gmp);
	fprintf(output, "enable-dvx=%i\n", instance->enable_dvx);
	fprintf(output, "enable-mpeg=%i\n", instance->enable_mpeg);
	fprintf(output, "enable-mp3=%i\n", instance->enable_mp3);
	fprintf(output, "enable-ogg=%i\n", instance->enable_ogg);
	fprintf(output, "enable-midi=%i\n", instance->enable_midi);
	fprintf(output, "enable-pls=%i\n", instance->enable_pls);
	fprintf(output, "enable-smil=%i\n", instance->enable_smil);
	fprintf(output, "enable-helix=%i\n", instance->enable_helix);
	fprintf(output, "nomediacache=%i\n", instance->nomediacache);
	fprintf(output, "nopauseonhide=%i\n", instance->nopauseonhide);
	fprintf(output, "rtsp-use-tcp=%i\n", instance->rtsp_use_tcp);
	fprintf(output, "rtsp-use-http=%i\n", instance->rtsp_use_http);


	while (fgets(buffer, sizeof(buffer), input) != NULL) {
	    if (strncasecmp(buffer, "vo", 2) == 0) {
	    } else if (strncasecmp(buffer, "ao", 2) == 0) {
	    } else if (strncasecmp(buffer, "cachesize", 9) == 0) {
	    } else if (strncasecmp(buffer, "cache-percent", 13) == 0) {
	    } else if (strncasecmp(buffer, "dload-dir", 9) == 0) {
	    } else if (strncasecmp(buffer, "showtime", 8) == 0) {
	    } else if (strncasecmp(buffer, "enable-wmp", 10) == 0) {
	    } else if (strncasecmp(buffer, "enable-qt", 9) == 0) {
	    } else if (strncasecmp(buffer, "enable-rm", 9) == 0) {
	    } else if (strncasecmp(buffer, "enable-mpeg", 11) == 0) {
	    } else if (strncasecmp(buffer, "enable-mp3", 10) == 0) {
	    } else if (strncasecmp(buffer, "enable-midi", 11) == 0) {
	    } else if (strncasecmp(buffer, "enable-pls", 10) == 0) {
	    } else if (strncasecmp(buffer, "enable-ogg", 10) == 0) {
	    } else if (strncasecmp(buffer, "enable-gmp", 10) == 0) {
	    } else if (strncasecmp(buffer, "enable-dvx", 10) == 0) {
	    } else if (strncasecmp(buffer, "enable-smil", 11) == 0) {
	    } else if (strncasecmp(buffer, "enable-helix", 12) == 0) {
	    } else if (strncasecmp(buffer, "nomediacache", 12) == 0) {
	    } else if (strncasecmp(buffer, "nopauseonhide", 13) == 0) {
	    } else if (strncasecmp(buffer, "rtsp-use-tcp", 12) == 0) {
	    } else if (strncasecmp(buffer, "rtsp-use-http", 13) == 0) {
	    } else {
		fprintf(output, "%s", buffer);
	    }

	}
	fclose(input);
	fclose(output);
	ret = unlink(config_name);
	if (ret == 0) {
	    ret = rename(tmp_name, config_name);
	    if (ret == -1) {
		ret = errno;
		if (DEBUG) {
		    printf("Unable to rename the file %s to %s\n",
			   tmp_name, config_name);
		    printf("Error code: %i - %s\n", ret, strerror(ret));
		}
	    }
	} else {
	    ret = errno;
	    if (DEBUG) {
		printf("Unable to delete the file %s\n", config_name);
		printf("Error code: %i - %s\n", ret, strerror(ret));
	    }
	}
    } else {
	if (DEBUG)
	    printf("write new config file\n");
	if (output != NULL) {
	    if (strlen(instance->vo) > 0)
		fprintf(output, "vo=%s\n", instance->vo);
	    if (strlen(instance->ao) > 0)
		fprintf(output, "ao=%s\n", instance->ao);
	    fprintf(output, "cachesize=%i\n", instance->cachesize);
	    fprintf(output, "cache-percent=%i\n", instance->cache_percent);
	    if (strlen(instance->download_dir) > 0)
		fprintf(output, "dload-dir=%s\n", instance->download_dir);
	    fprintf(output, "showtime=%i\n", instance->showtime);
	    fprintf(output, "enable-wmp=%i\n", instance->enable_wmp);
	    fprintf(output, "enable-qt=%i\n", instance->enable_qt);
	    fprintf(output, "enable-rm=%i\n", instance->enable_rm);
	    fprintf(output, "enable-gmp=%i\n", instance->enable_gmp);
	    fprintf(output, "enable-dvx=%i\n", instance->enable_dvx);
	    fprintf(output, "enable-mpeg=%i\n", instance->enable_mpeg);
	    fprintf(output, "enable-mp3=%i\n", instance->enable_mp3);
	    fprintf(output, "enable-midi=%i\n", instance->enable_midi);
	    fprintf(output, "enable-pls=%i\n", instance->enable_pls);
	    fprintf(output, "enable-ogg=%i\n", instance->enable_ogg);
	    fprintf(output, "enable-smil=%i\n", instance->enable_smil);
	    fprintf(output, "enable-helix=%i\n", instance->enable_helix);
	    fprintf(output, "nomediacache=%i\n", instance->nomediacache);
	    fprintf(output, "nopauseonhide=%i\n", instance->nopauseonhide);
	    fprintf(output, "rtsp-use-tcp=%i\n", instance->rtsp_use_tcp);
	    fprintf(output, "rtsp-use-http=%i\n", instance->rtsp_use_http);
	    fclose(output);
	    ret = rename(tmp_name, config_name);
	    if (ret == -1) {
		ret = errno;
		if (DEBUG) {
		    printf("Unable to rename the file %s to %s\n",
			   tmp_name, config_name);
		    printf("Error code: %i - %s\n", ret, strerror(ret));
		}
	    }
	}

    }

    snprintf(buffer, 1000, "%s", getenv("HOME"));
    strlcat(buffer, "/.mozilla/pluginreg.dat", 1000);
    unlink(buffer);
    snprintf(buffer, 1000, "%s", getenv("HOME"));
    strlcat(buffer, "/.firefox/pluginreg.dat", 1000);
    unlink(buffer);
    snprintf(buffer, 1000, "%s", getenv("HOME"));
    strlcat(buffer, "/.mozilla/firefox/pluginreg.dat", 1000);
    unlink(buffer);
    NPN_ReloadPlugins(FALSE);

    return (FALSE);
}

void menuitem_config_callback(GtkMenuItem * menuitem,
			      nsPluginInstance * instance)
{
    GtkWidget *conf_vbox;
    GtkWidget *conf_hbutton_box;
    GtkWidget *conf_ok;
    GtkWidget *conf_cancel;
    GtkWidget *conf_table;
    GtkWidget *conf_label;
    int row = 0;
    if (instance->conf_window != NULL)
	return;

    instance->conf_window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_widget_realize(instance->conf_window);
    gtk_window_set_resizable(GTK_WINDOW(instance->conf_window), FALSE);

    conf_vbox = gtk_vbox_new(FALSE, 10);
    conf_hbutton_box = gtk_hbutton_box_new();
    conf_table = gtk_table_new(20, 2, FALSE);

    gtk_container_add(GTK_CONTAINER(conf_vbox), conf_table);
    gtk_container_add(GTK_CONTAINER(conf_vbox), conf_hbutton_box);
    gtk_container_add(GTK_CONTAINER(instance->conf_window), conf_vbox);
    gtk_window_set_title(GTK_WINDOW(instance->conf_window),
			 _("mplayerplug-in configuration"));
    gtk_container_set_border_width(GTK_CONTAINER
				   (instance->conf_window), 5);
    g_signal_connect(GTK_OBJECT(instance->conf_window), "delete_event",
		     GTK_SIGNAL_FUNC(CloseConfigEvent), instance);

#ifdef GTK24_ENABLED
    instance->conf_vo = gtk_combo_box_entry_new_text();
    if (instance->conf_vo != NULL) {
	gtk_combo_box_append_text(GTK_COMBO_BOX(instance->conf_vo), "gl");
	gtk_combo_box_append_text(GTK_COMBO_BOX(instance->conf_vo), "x11");
	gtk_combo_box_append_text(GTK_COMBO_BOX(instance->conf_vo), "xv");
	if (instance->vo != NULL) {
	    if (strcmp(instance->vo, "gl") == 0)
		gtk_combo_box_set_active(GTK_COMBO_BOX
					 (instance->conf_vo), 0);
	    if (strcmp(instance->vo, "x11") == 0)
		gtk_combo_box_set_active(GTK_COMBO_BOX
					 (instance->conf_vo), 1);
	    if (strcmp(instance->vo, "xv") == 0)
		gtk_combo_box_set_active(GTK_COMBO_BOX
					 (instance->conf_vo), 2);
	    if (gtk_combo_box_get_active(GTK_COMBO_BOX(instance->conf_vo))
		== -1) {
		gtk_combo_box_append_text(GTK_COMBO_BOX
					  (instance->conf_vo),
					  instance->vo);
		gtk_combo_box_set_active(GTK_COMBO_BOX
					 (instance->conf_vo), 3);
	    }
	}
    }

    instance->conf_ao = gtk_combo_box_entry_new_text();
    if (instance->conf_ao != NULL) {
	gtk_combo_box_append_text(GTK_COMBO_BOX(instance->conf_ao),
				  "alsa");
	gtk_combo_box_append_text(GTK_COMBO_BOX(instance->conf_ao),
				  "arts");
	gtk_combo_box_append_text(GTK_COMBO_BOX(instance->conf_ao), "esd");
	gtk_combo_box_append_text(GTK_COMBO_BOX(instance->conf_ao),
				  "jack");
	gtk_combo_box_append_text(GTK_COMBO_BOX(instance->conf_ao), "oss");
	gtk_combo_box_append_text(GTK_COMBO_BOX(instance->conf_ao), "pulse");
	if (instance->ao != NULL) {
	    if (strcmp(instance->ao, "alsa") == 0)
		gtk_combo_box_set_active(GTK_COMBO_BOX
					 (instance->conf_ao), 0);
	    if (strcmp(instance->ao, "arts") == 0)
		gtk_combo_box_set_active(GTK_COMBO_BOX
					 (instance->conf_ao), 1);
	    if (strcmp(instance->ao, "esd") == 0)
		gtk_combo_box_set_active(GTK_COMBO_BOX
					 (instance->conf_ao), 2);
	    if (strcmp(instance->ao, "jack") == 0)
		gtk_combo_box_set_active(GTK_COMBO_BOX
					 (instance->conf_ao), 3);
	    if (strcmp(instance->ao, "oss") == 0)
		gtk_combo_box_set_active(GTK_COMBO_BOX
					 (instance->conf_ao), 4);
	    if (strcmp(instance->ao, "pulse") == 0)
		gtk_combo_box_set_active(GTK_COMBO_BOX
					 (instance->conf_ao), 5);
	    if (gtk_combo_box_get_active(GTK_COMBO_BOX(instance->conf_ao))
		== -1) {
		gtk_combo_box_append_text(GTK_COMBO_BOX
					  (instance->conf_ao),
					  instance->ao);
		gtk_combo_box_set_active(GTK_COMBO_BOX
					 (instance->conf_ao), 6);
	    }
	}
    }

    instance->conf_dir = gtk_combo_box_entry_new_text();
    if (instance->conf_dir != NULL) {
	gtk_combo_box_append_text(GTK_COMBO_BOX(instance->conf_dir),
				  "$HOME");
	if (instance->download_dir != NULL) {
	    if (strcmp(instance->download_dir, "$HOME") == 0)
		gtk_combo_box_set_active(GTK_COMBO_BOX
					 (instance->conf_dir), 0);
	    if (gtk_combo_box_get_active(GTK_COMBO_BOX(instance->conf_dir))
		== -1) {
		gtk_combo_box_append_text(GTK_COMBO_BOX
					  (instance->conf_dir),
					  instance->download_dir);
		gtk_combo_box_set_active(GTK_COMBO_BOX
					 (instance->conf_dir), 1);
	    }
	}
    }
#endif

    conf_ok = gtk_button_new_with_label(_("OK"));
    g_signal_connect(GTK_OBJECT(conf_ok), "clicked",
		     GTK_SIGNAL_FUNC(ApplyConfig), instance);

    conf_cancel = gtk_button_new_with_label(_("Cancel"));
    g_signal_connect(GTK_OBJECT(conf_cancel), "clicked",
		     GTK_SIGNAL_FUNC(CloseConfig), instance);

#ifdef GTK24_ENABLED
    conf_label = gtk_label_new(_("Video Output:"));
    gtk_misc_set_alignment(GTK_MISC(conf_label), 0.0, 0.0);
    gtk_table_attach_defaults(GTK_TABLE(conf_table), conf_label, 0, 1,
			      0, 1);
    gtk_widget_show(conf_label);
    gtk_table_attach_defaults(GTK_TABLE(conf_table), instance->conf_vo,
			      1, 2, 0, 1);

    conf_label = gtk_label_new(_("Audio Output:"));
    gtk_misc_set_alignment(GTK_MISC(conf_label), 0.0, 0.0);
    gtk_table_attach_defaults(GTK_TABLE(conf_table), conf_label, 0, 1,
			      1, 2);
    gtk_widget_show(conf_label);
    gtk_misc_set_alignment(GTK_MISC(conf_label), 0.0, 0.0);
    gtk_table_attach_defaults(GTK_TABLE(conf_table), instance->conf_ao,
			      1, 2, 1, 2);
    conf_label = gtk_label_new(_("Save to Location:"));
    gtk_table_attach_defaults(GTK_TABLE(conf_table), conf_label, 0, 1,
			      2, 3);
    gtk_widget_show(conf_label);
    gtk_misc_set_alignment(GTK_MISC(conf_label), 0.0, 0.0);
    gtk_table_attach_defaults(GTK_TABLE(conf_table),
			      instance->conf_dir, 1, 2, 2, 3);
#endif

    conf_label = gtk_label_new(_("Minimum Cache Size:"));
    gtk_misc_set_alignment(GTK_MISC(conf_label), 0.0, 0.0);
    gtk_table_attach_defaults(GTK_TABLE(conf_table), conf_label, 0, 1,
			      3, 4);
    gtk_widget_show(conf_label);
    instance->conf_cachesize = gtk_hscale_new_with_range(32, 32767, 512);
    gtk_table_attach_defaults(GTK_TABLE(conf_table),
			      instance->conf_cachesize, 1, 2, 3, 4);
    gtk_range_set_value(GTK_RANGE(instance->conf_cachesize),
			instance->cachesize);
    gtk_widget_show(instance->conf_cachesize);

    conf_label = gtk_label_new(_("Percent of Media to Cache:"));
    gtk_misc_set_alignment(GTK_MISC(conf_label), 0.0, 0.0);
    gtk_table_attach_defaults(GTK_TABLE(conf_table), conf_label, 0, 1,
			      4, 5);
    gtk_widget_show(conf_label);
    instance->conf_cachepercent = gtk_hscale_new_with_range(0, 100, 5);
    gtk_table_attach_defaults(GTK_TABLE(conf_table),
			      instance->conf_cachepercent, 1, 2, 4, 5);
    gtk_range_set_value(GTK_RANGE(instance->conf_cachepercent),
			instance->cache_percent);
    gtk_widget_show(instance->conf_cachepercent);

    row = 5;
    instance->conf_showtime =
	gtk_check_button_new_with_label(_("Show time in progress bar"));
    gtk_table_attach_defaults(GTK_TABLE(conf_table),
			      instance->conf_showtime, 1, 2, row, row + 1);
    row++;
    gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON
				 (instance->conf_showtime),
				 instance->showtime);
    gtk_widget_show(instance->conf_showtime);

    instance->conf_enable_wmp =
	gtk_check_button_new_with_label(_
					("Enable Windows Media Player Support"));
    gtk_table_attach_defaults(GTK_TABLE(conf_table),
			      instance->conf_enable_wmp, 1, 2, row,
			      row + 1);
    row++;
    gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON
				 (instance->conf_enable_wmp),
				 instance->enable_wmp);
    gtk_widget_show(instance->conf_enable_wmp);

    instance->conf_enable_qt =
	gtk_check_button_new_with_label(_("Enable QuickTime Support"));
    gtk_table_attach_defaults(GTK_TABLE(conf_table),
			      instance->conf_enable_qt, 1, 2, row,
			      row + 1);
    row++;
    gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON
				 (instance->conf_enable_qt),
				 instance->enable_qt);
    gtk_widget_show(instance->conf_enable_qt);

    instance->conf_enable_rm =
	gtk_check_button_new_with_label(_("Enable RealMedia Support"));
    gtk_table_attach_defaults(GTK_TABLE(conf_table),
			      instance->conf_enable_rm, 1, 2, row,
			      row + 1);
    row++;
    gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON
				 (instance->conf_enable_rm),
				 instance->enable_rm);
    gtk_widget_show(instance->conf_enable_rm);

    instance->conf_enable_smil =
	gtk_check_button_new_with_label(_
					("Enable SMIL Support (Effects QuickTime and RealMedia)"));
    gtk_table_attach_defaults(GTK_TABLE(conf_table),
			      instance->conf_enable_smil, 1, 2, row,
			      row + 1);
    row++;
    gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON
				 (instance->conf_enable_smil),
				 instance->enable_smil);
    gtk_widget_show(instance->conf_enable_smil);

    instance->conf_enable_helix =
	gtk_check_button_new_with_label(_("Enable Helix Emulation"));
    gtk_table_attach_defaults(GTK_TABLE(conf_table),
			      instance->conf_enable_helix, 1, 2, row,
			      row + 1);
    row++;
    gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON
				 (instance->conf_enable_helix),
				 instance->enable_helix);
    gtk_widget_show(instance->conf_enable_helix);

    instance->conf_enable_gmp =
	gtk_check_button_new_with_label(_
					("Enable Google Media Player Support"));
    gtk_table_attach_defaults(GTK_TABLE(conf_table),
			      instance->conf_enable_gmp, 1, 2, row,
			      row + 1);
    row++;
    gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON
				 (instance->conf_enable_gmp),
				 instance->enable_gmp);
    gtk_widget_show(instance->conf_enable_gmp);

    instance->conf_enable_mpeg =
	gtk_check_button_new_with_label(_("Enable MPEG Support"));
    gtk_table_attach_defaults(GTK_TABLE(conf_table),
			      instance->conf_enable_mpeg, 1, 2, row,
			      row + 1);
    row++;
    gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON
				 (instance->conf_enable_mpeg),
				 instance->enable_mpeg);
    gtk_widget_show(instance->conf_enable_mpeg);

    instance->conf_enable_mp3 =
	gtk_check_button_new_with_label(_
					("Enable MP3 Support (Requires MPEG Support)"));
    gtk_table_attach_defaults(GTK_TABLE(conf_table),
			      instance->conf_enable_mp3, 1, 2, row,
			      row + 1);
    row++;
    gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON
				 (instance->conf_enable_mp3),
				 instance->enable_mp3);
    gtk_widget_show(instance->conf_enable_mp3);

    instance->conf_enable_ogg =
	gtk_check_button_new_with_label(_("Enable Ogg Support"));
    gtk_table_attach_defaults(GTK_TABLE(conf_table),
			      instance->conf_enable_ogg, 1, 2, row,
			      row + 1);
    row++;
    gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON
				 (instance->conf_enable_ogg),
				 instance->enable_ogg);
    gtk_widget_show(instance->conf_enable_ogg);

    instance->conf_enable_midi =
	gtk_check_button_new_with_label(_("Enable MIDI Support"));
    gtk_table_attach_defaults(GTK_TABLE(conf_table),
			      instance->conf_enable_midi, 1, 2, row,
			      row + 1);
    row++;
    gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON
				 (instance->conf_enable_midi),
				 instance->enable_midi);
    gtk_widget_show(instance->conf_enable_midi);

    instance->conf_enable_pls =
	gtk_check_button_new_with_label(_("Enable Shoutcast PLS Support"));
    gtk_table_attach_defaults(GTK_TABLE(conf_table),
			      instance->conf_enable_pls, 1, 2, row,
			      row + 1);
    row++;
    gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON
				 (instance->conf_enable_pls),
				 instance->enable_pls);
    gtk_widget_show(instance->conf_enable_pls);

    instance->conf_enable_dvx =
	gtk_check_button_new_with_label(_("Enable DivX Support"));
    gtk_table_attach_defaults(GTK_TABLE(conf_table),
			      instance->conf_enable_dvx, 1, 2, row,
			      row + 1);
    row++;
    gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON
				 (instance->conf_enable_dvx),
				 instance->enable_dvx);
    gtk_widget_show(instance->conf_enable_dvx);

    instance->conf_nomediacache =
	gtk_check_button_new_with_label(_
					("Play media directly from site (No Caching)"));
    gtk_table_attach_defaults(GTK_TABLE(conf_table),
			      instance->conf_nomediacache, 1, 2, row,
			      row + 1);
    row++;
    gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON
				 (instance->conf_nomediacache),
				 instance->nomediacache);
    gtk_widget_show(instance->conf_nomediacache);

    instance->conf_rtsptcp =
	gtk_check_button_new_with_label(_
					("Connect to RTSP Media over TCP"));
    gtk_table_attach_defaults(GTK_TABLE(conf_table),
			      instance->conf_rtsptcp, 1, 2, row, row + 1);
    row++;
    gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON
				 (instance->conf_rtsptcp),
				 instance->rtsp_use_tcp);
    gtk_widget_show(instance->conf_rtsptcp);

    instance->conf_rtsphttp =
	gtk_check_button_new_with_label(_("Use HTTP instead of RTSP"));
    gtk_table_attach_defaults(GTK_TABLE(conf_table),
			      instance->conf_rtsphttp, 1, 2, row, row + 1);
    row++;
    gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON
				 (instance->conf_rtsphttp),
				 instance->rtsp_use_http);
    gtk_widget_show(instance->conf_rtsphttp);

    instance->conf_pauseonhide =
	gtk_check_button_new_with_label(_("Pause Video when hidden"));
    gtk_table_attach_defaults(GTK_TABLE(conf_table),
			      instance->conf_pauseonhide, 1, 2, row,
			      row + 1);
    row++;
    gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON
				 (instance->conf_pauseonhide),
				 !instance->nopauseonhide);
    gtk_widget_show(instance->conf_pauseonhide);


    gtk_container_add(GTK_CONTAINER(conf_hbutton_box), conf_ok);
    gtk_container_add(GTK_CONTAINER(conf_hbutton_box), conf_cancel);

#ifdef GTK24_ENABLED
    if (instance->conf_vo != NULL)
	gtk_widget_show(instance->conf_vo);
    if (instance->conf_ao != NULL)
	gtk_widget_show(instance->conf_ao);
    if (instance->conf_dir != NULL)
	gtk_widget_show(instance->conf_dir);
#endif
    gtk_widget_show(conf_ok);
    gtk_widget_show(conf_cancel);
    gtk_widget_show(conf_table);
    gtk_widget_show(conf_hbutton_box);
    gtk_widget_show(conf_vbox);
    gtk_widget_show(instance->conf_window);

}

gboolean window_visible(GtkWidget * widget, GdkEvent * event,
			nsPluginInstance * instance)
{

    //if (DEBUG)
    //      printf("**************\n\n\n\n in window_visible %i\n\n\n\n\n******************\n",GTK_WIDGET_VISIBLE(widget));

    // only pause if video is present, if we have streaming audio we want to keep playing

    if (instance->cancelled == 0) {

	if (event->type == GDK_VISIBILITY_NOTIFY) {

	    if (((GdkEventVisibility *) event)->state ==
		GDK_VISIBILITY_FULLY_OBSCURED) {
		if (instance->paused == 0 && instance->threadsignaled
		    && instance->nopauseonhide == 0) {

		    if (instance->movie_width != 0
			&& instance->movie_height != 0) {
			instance->Pause();
			instance->paused_wheninvisible = 1;
		    }
		}

		if (instance->onHiddenCallback != NULL)
		    NPN_GetURL(instance->mInstance,
			       instance->onHiddenCallback, "_self");

	    } else {
		if (instance->paused_wheninvisible == 1
		    && instance->threadsignaled
		    && instance->nopauseonhide == 0) {

		    if (instance->movie_width != 0
			&& instance->movie_height != 0) {
			instance->Play();
			instance->paused_wheninvisible = 0;
		    }
		}

		if (instance->onVisibleCallback != NULL)
		    NPN_GetURL(instance->mInstance,
			       instance->onVisibleCallback, "_self");

	    }

	}
    }
    return (FALSE);

}

gboolean load_href_callback(GtkWidget * widget, GdkEventExpose * event,
			    nsPluginInstance * instance)
{
    Node *n;
    GdkEventButton *event_button;

    if (DEBUG)
	printf("image clicked\n");

    if (instance->autohref || event->type == GDK_BUTTON_PRESS) {
	event_button = (GdkEventButton *) event;

	if (event_button != NULL && event_button->button == 3) {
	    gtk_menu_popup(instance->popup_menu, NULL, NULL, NULL,
			   NULL, event_button->button, event_button->time);
	    return TRUE;
	}

	if (instance->autohref || event_button->button == 1 ) {
	    n = instance->list;

	    while (n != NULL) {
		if (strcmp(n->url, instance->href) == 0)
		    break;
		else
		    n = n->next;
	    }

	    if (n == NULL) {
		n = newNode();
		snprintf(n->url, 1024, "%s", instance->href);
		n->frombutton = 1;
		addToEnd(instance->td->list, n);
		NPN_GetURL(instance->mInstance, instance->href, NULL);
		if (instance->targetplayer == 0)
		    gtk_widget_hide(instance->src_event_box);
	    } else {
		if (instance->targetplayer == 0)
		    gtk_widget_hide(instance->src_event_box);
		//n->play = 1;
		instance->cancelled = 0;
		pthread_mutex_lock(&(instance->playlist_mutex));	// manipulating the playlist, so lock it
		n = instance->list;
		while (n != NULL) {
		    if (n->played == 1)
			n->played = 0;	// reset played flag
		    n = n->next;
		}
		pthread_mutex_unlock(&(instance->playlist_mutex));	// unlock the playlist
		instance->Play();
	    }

	    if (instance->targetplayer == 1)
		gtk_widget_show(instance->gtkwidget);

	    return TRUE;
	}
    }

    return FALSE;

}
#endif
#endif				// GTK_ENABLED

int srcToButton(char *url, nsPluginInstance * instance)
{
#ifndef GTK2_ENABLED
    return FALSE;
#endif

#ifdef GTK2_ENABLED

    gchar *command;
    GError *error = NULL;
    int exit_status;
    gchar *dirname = NULL;
    gchar *filename = NULL;
    gboolean result;
    GtkWidget *fixed_button_container;

    if (DEBUG)
	printf("In srcToButton\n");

    // try to load the file natively

    instance->pb_src = gdk_pixbuf_new_from_file(url, &error);

    if (instance->pb_src == NULL) {
	// setup tmp directory
	dirname =
	    g_strdup_printf("%s", tempnam("/tmp", "mplayerplug-inXXXXXX"));
	filename = g_strdup_printf("%s/00000001.jpg", dirname);

	// run mplayer and try to get the first frame and convert it to a jpeg
	command =
	    g_strdup_printf("mplayer -vo jpeg:outdir=%s -frames 1 %s",
			    dirname, url);
	if (!g_spawn_command_line_sync
	    (command, NULL, NULL, &exit_status, &error))
	    printf("Error when running When running command: %s\n%s\n",
		   command, error->message);

	if (fexists(filename)) {
	    error = NULL;
	    instance->pb_src = gdk_pixbuf_new_from_file(filename, &error);
	} else {
	    instance->pb_src =
		gdk_pixbuf_new_from_xpm_data((const char **) start);
	}

    }

    if (instance->pb_src != NULL) {
	if (instance->targetplayer == 0) {
	    instance->src_event_box = gtk_event_box_new();
	    instance->image_src =
		gtk_image_new_from_pixbuf(instance->pb_src);
	    gtk_container_add(GTK_CONTAINER(instance->src_event_box),
			      instance->image_src);
	    g_signal_connect(G_OBJECT(instance->src_event_box),
			     "button_press_event",
			     G_CALLBACK(load_href_callback), instance);
	    gtk_fixed_put(GTK_FIXED(instance->fixed_container),
			  instance->src_event_box, 0, 0);
	    gtk_widget_set_size_request(GTK_WIDGET
					(instance->src_event_box),
					instance->embed_width,
					instance->embed_height);
	    gtk_widget_show(GTK_WIDGET(instance->image_src));
	    gtk_widget_show(instance->src_event_box);
	    gtk_widget_show(instance->fixed_container);
	} else {
	    gtk_widget_add_events(instance->button_window,
				  GDK_BUTTON_PRESS_MASK);
	    gtk_widget_realize(instance->button_window);
	    instance->src_event_box = gtk_event_box_new();
	    instance->image_src =
		gtk_image_new_from_pixbuf(instance->pb_src);
	    g_signal_connect(G_OBJECT(instance->src_event_box),
			     "button_press_event",
			     G_CALLBACK(load_href_callback), instance);

	    gtk_widget_set_size_request(GTK_WIDGET
					(instance->src_event_box),
					instance->embed_width,
					instance->embed_height);
	    gtk_container_add(GTK_CONTAINER(instance->src_event_box),
			      instance->image_src);
	    fixed_button_container = gtk_fixed_new();
	    gtk_widget_set_size_request(GTK_WIDGET
					(fixed_button_container),
					instance->embed_width,
					instance->embed_height);
	    gtk_container_add(GTK_CONTAINER(instance->button_window),
			      fixed_button_container);
	    gtk_fixed_put(GTK_FIXED(fixed_button_container),
			  instance->src_event_box, 0, 0);
	    gtk_widget_show(GTK_WIDGET(instance->image_src));
	    gtk_widget_show(instance->src_event_box);
	    gtk_widget_show(fixed_button_container);
	    gtk_widget_show(instance->button_window);
	    g_idle_add(gtkgui_draw, instance);
            if (instance->autohref)
                load_href_callback(instance->src_event_box, NULL, instance);

	}
	result = TRUE;
    } else {
	if (instance->targetplayer == 1)
	    gtk_widget_show(instance->gtkwidget);
	result = FALSE;
    }

    if (filename != NULL) {
	remove(filename);
	g_free(filename);
    }

    if (dirname != NULL) {
	remove(dirname);
	g_free(dirname);
    }

    return result;

#endif
}

#ifdef GTK_ENABLED
gboolean target_hide_callback(GtkWidget * widget, GdkEvent * event,
			      nsPluginInstance * instance)
{
    if (DEBUG)
	printf("Hiding gtkwidget, window\n");
    gtk_widget_hide(GTK_WIDGET(instance->gtkwidget));
    instance->Quit();
    return TRUE;
}

gboolean gtkgui_show_src(void *data)
{
    nsPluginInstance *instance;

    if (DEBUG > 1)
	printf("in gtkgui_stop\n");

    instance = (nsPluginInstance *) data;
    if (instance == NULL)
	return FALSE;
    if (GTK_IS_WIDGET(instance->src_event_box)) {
    	gtk_widget_show(instance->src_event_box);
    }
    return FALSE;

}
#endif				// GTK_ENABLED
