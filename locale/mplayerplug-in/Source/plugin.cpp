/* ***** BEGIN LICENSE BLOCK *****
 * Version: NPL 1.1/GPL 2.0/LGPL 2.1
 *
 * The contents of this file are subject to the Netscape Public License
 * Version 1.1 (the "License"); you may not use this file except in
 * compliance with the License. You may obtain a copy of the License at
 * http://www.mozilla.org/NPL/
 *
 * Software distributed under the License is distributed on an "AS IS" basis,
 * WITHOUT WARRANTY OF ANY KIND, either express or implied. See the License
 * for the specific language governing rights and limitations under the
 * License.
 *
 * The Original Code is mozilla.org code.
 *
 * The Initial Developer of the Original Code is
 * Netscape Communications Corporation.
 * Portions created by the Initial Developer are Copyright (C) 1998
 * the Initial Developer. All Rights Reserved.
 *
 * Contributor(s):
 *
 * Alternatively, the contents of this file may be used under the terms of
 * either the GNU General Public License Version 2 or later (the "GPL"), or
 * the GNU Lesser General Public License Version 2.1 or later (the "LGPL"),
 * in which case the provisions of the GPL or the LGPL are applicable instead
 * of those above. If you wish to allow use of your version of this file only
 * under the terms of either the GPL or the LGPL, and not to allow others to
 * use your version of this file under the terms of the NPL, indicate your
 * decision by deleting the provisions above and replace them with the notice
 * and other provisions required by the GPL or the LGPL. If you do not delete
 * the provisions above, a recipient may use your version of this file under
 * the terms of any one of the NPL, the GPL or the LGPL.
 *
 * ***** END LICENSE BLOCK ***** */

#include "plugin.h"
#include "nsIServiceManager.h"
#include "nsIMemory.h"
#include "nsISupportsUtils.h"	// this is where some useful macros defined
#include <errno.h>
#include <fcntl.h>

#ifdef GTK2_ENABLED
#include "../pixmaps/gtk_logo.h"
#endif

#ifdef GTK1_ENABLED
#include "../pixmaps/logo.xpm"
#endif

#ifdef HAVE_GETTEXT
#ifdef X_ENABLED
#include <locale.h>
#endif
#endif

extern int errno;
extern int DEBUG;
extern int instance_counter;

int32 STREAMBUFSIZE = 0X0FFFFFFF;

// service manager which will give the access to all public browser services
// we will use memory service as an illustration
nsIServiceManager *gServiceManager = NULL;

// Unix needs this
#ifdef XP_UNIX

char *NPP_GetMIMEDescription(void)
{
    return GetMIMEDescription();
}

// get values per plugin
NPError NS_PluginGetValue(NPPVariable aVariable, void *aValue)
{
    return GetValue(aVariable, aValue);
}
#endif				//XP_UNIX

//////////////////////////////////////
//
// general initialization and shutdown
//
NPError NS_PluginInitialize()
{
    // this is probably a good place to get the service manager
    // note that Mozilla will add reference, so do not forget to release
    nsISupports *sm = NULL;


    NPN_GetValue(NULL, NPNVserviceManager, &sm);

    // Mozilla returns nsIServiceManager so we can use it directly; doing QI on
    // nsISupports here can still be more appropriate in case something is changed
    // in the future so we don't need to do casting of any sort.
    if (sm) {
	sm->QueryInterface(NS_GET_IID(nsIServiceManager),
			   (void **) &gServiceManager);
	NS_RELEASE(sm);
    }

    return NPERR_NO_ERROR;
}

void NS_PluginShutdown()
{
    // we should release the service manager
    NS_IF_RELEASE(gServiceManager);
    gServiceManager = NULL;
}

/////////////////////////////////////////////////////////////
//
// construction and destruction of our plugin instance object
//
nsPluginInstanceBase *NS_NewPluginInstance(nsPluginCreateData *
					   aCreateDataStruct)
{
    if (!aCreateDataStruct)
	return NULL;

    nsPluginInstance *plugin =
	new nsPluginInstance(aCreateDataStruct->instance);

    New(plugin, aCreateDataStruct);

    // This line makes the plugin "windowless"
    // problem is there is no window now to draw too
    // Also according to 
    // http://devedge-temp.mozilla.org/library/manuals/2002/plugin/1.0/drawevent.html
    // X can't do windowless plugins, but that is old
    // NPN_SetValue(aCreateDataStruct->instance,NPPVpluginWindowBool, FALSE);
    return plugin;
}

void NS_DestroyPluginInstance(nsPluginInstanceBase * aPlugin)
{
    if (aPlugin)
	delete(nsPluginInstance *) aPlugin;
}

////////////////////////////////////////
//
// nsPluginInstance class implementation
//
nsPluginInstance::nsPluginInstance(NPP aInstance):nsPluginInstanceBase(),
mInstance(aInstance),
mInitialized(FALSE), mScriptablePeer(NULL), mControlsScriptablePeer(NULL)
{
    int i;

    instance_counter++;

    toolkitok = toolkitOk(mInstance, &moz_toolkit, &plug_toolkit);

    mScriptablePeer = getScriptablePeer();
    mControlsScriptablePeer = getControlsScriptablePeer();
    mScriptablePeer->InitControls(mControlsScriptablePeer);
    mControlsScriptablePeer->AddRef();

    // init the instance variables here.
    mimetype = NULL;
    state = 0;
    url = NULL;
    fname = NULL;
    href = NULL;
    autohref = 0;
    lastmessage = (char *) NPN_MemAlloc(sizeof(char) * 1024);
    memset(lastmessage, '\0', 1);
    mode = 0;
    window_width = 0;
    window_height = 0;
    embed_width = 0;
    embed_height = 0;
    movie_width = 1;
    movie_height = 1;
    setwindow = 0;
    baseurl = NULL;
    hostname = NULL;
    pid = 0;
    noredraw = 0;
    hrefrequested = 0;
    threadsetup = 0;
    threadlaunched = 0;
    threadsignaled = 0;
    cancelled = 0;
    list = newNode();
    currentnode = NULL;
    td = (ThreadData *) NPN_MemAlloc(sizeof(ThreadData));
    td->list = NULL;
    td->instance = NULL;
    control = -1;
    player = NULL;
    autostart = 1;
    showcontrols = 1;
    showtracker = 1;
    showbuttons = 1;
    showfsbutton = 1;
    redrawbuttons = 0;
    mmsstream = 0;
    js_state = JS_STATE_UNDEFINED;
    nQtNext = 0;
    for (i = 0; i < 256; i++)
	qtNext[i] = NULL;
    widget = 0;
    display = NULL;
    window = 0;
    player_window = 0;
    controlwindow = 0;
    panel_height = 0;
    panel_drawn = 0;
    mediaCompleteCallback = NULL;
    mediaCompleteWithErrorCallback = NULL;
    mouseClickCallback = NULL;
    mouseDownCallback = NULL;
    mouseUpCallback = NULL;
    mouseEnterCallback = NULL;
    mouseLeaveCallback = NULL;
    onVisibleCallback = NULL;
    onHiddenCallback = NULL;
    onDestroyCallback = NULL;
    mediaLength = 0.0;
    mediaPercent = 0;
    mediaTime = 0.0;
    mediaPos = 0.0;
    nomediacache = 0;
    controlsvisible = 0;
    fullscreen = 0;
    showlogo = 1;
    showtime = 1;
    DPMSEnabled = 0;
    black_background = 0;
    nomouseinput = 1;
    noconsolecontrols = 1;
    cookies = 1;
    starttime = 0;
    nopauseonhide = 0;
    targetplayer = 0;
    hidestatus = 0;
    enablecontextmenu = 1;
    dontparent = 0;
    volume = -1;

#ifdef GTK_ENABLED
    image = NULL;
    progress_bar = NULL;
    mediaprogress_bar = NULL;
    status = NULL;
    play_event_box = NULL;
    pause_event_box = NULL;
    stop_event_box = NULL;
    ff_event_box = NULL;
    rew_event_box = NULL;
    vol_event_box = NULL;
    vol_tooltip = NULL;
    fs_event_box = NULL;
    src_event_box = NULL;
    gtkwidget = NULL;
    fixed_container = NULL;
    drawing_area = NULL;
    popup_menu = NULL;
    conf_window = NULL;
    file_selector = NULL;
#ifdef GTK2_ENABLED
    fs_window = NULL;
    paused_wheninvisible = 0;
#endif
#endif

#ifdef X_ENABLED
    font = NULL;
    logo = (Pixmap) NULL;
    logomask = (Pixmap) NULL;
    progress_left = (Pixmap) NULL;
    progress_leftmask = (Pixmap) NULL;
    progress_middle = (Pixmap) NULL;
    progress_middlemask = (Pixmap) NULL;
    progress_right = (Pixmap) NULL;
    progress_rightmask = (Pixmap) NULL;
    progress_fill = (Pixmap) NULL;
    progress_fillmask = (Pixmap) NULL;
    lastpercent = -1;
#endif

    // options
    vo = NULL;
    vop = NULL;
    novop = 0;
    noembed = 0;
    ao = NULL;
    af = NULL;
    loop = -1;
    rtsp_use_tcp = 0;
    rtsp_use_http = 0;
    keep_download = 0;
    maintain_aspect = 1;
    download_dir = strdup(getenv("HOME"));
    cachesize = 512;
    output_display = NULL;
    osdlevel = 1;
    qt_speed = SPEED_MED;
    cache_percent = 25;
    framedrop = 0;
    autosync = 0;
    mc = 0;
    softvol = 1;
    useragent = NULL;
    enable_smil = 1;
    enable_helix = 1;
    enable_wmp = 1;
    enable_qt = 1;
    enable_rm = 1;
    enable_gmp = 1;
    enable_mpeg = 1;
    enable_mp3 = 1;
    enable_ogg = 1;
    enable_midi = 0;
    enable_pls = 1;
    tv_driver = NULL;
    tv_input = NULL;
    tv_width = 0;
    tv_height = 0;
    tv_outfmt = NULL;
    profile = NULL;

    // JavaScript
    paused = 0;

    // thread setup
    pthread_mutex_init(&playlist_mutex, NULL);
    pthread_mutex_init(&playlist_cond_mutex, NULL);
    pthread_mutex_init(&control_mutex, NULL);
    pthread_mutex_init(&read_mutex, NULL);
    pthread_attr_init(&thread_attr);
    pthread_cond_init(&playlist_complete_cond, NULL);


    LoadConfigFile(this);

#ifdef HAVE_GETTEXT
#ifdef X_ENABLED
    setlocale(LC_ALL, "");
#endif
    textdomain("mplayerplug-in");
    bind_textdomain_codeset("mplayerplug-in", "utf-8");
#endif
    time(&lastOutput);
}

nsPluginInstance::~nsPluginInstance()
{
    // mScriptablePeer may be also held by the browser
    // so releasing it here does not guarantee that it is over
    // we should take precaution in case it will be called later
    // and zero its mPlugin member
    if (DEBUG)
	printf("~nsPluginInstance called\n");
#ifdef GTK_ENABLED
    gdk_flush();
#endif

    instance_counter--;

    mInstance = NULL;
    mInitialized = FALSE;
    if (mControlsScriptablePeer != NULL) {
	mControlsScriptablePeer->SetInstance(NULL);
	mControlsScriptablePeer->Release();
	NS_IF_RELEASE(mControlsScriptablePeer);
    }

    if (mScriptablePeer != NULL) {
	mScriptablePeer->InitControls(NULL);
	mScriptablePeer->SetInstance(NULL);
	NS_IF_RELEASE(mScriptablePeer);
    }

}

NPBool nsPluginInstance::init(NPWindow * aWindow)
{
    if (aWindow == NULL)
	return FALSE;

    mInitialized = TRUE;
    return TRUE;
}

void nsPluginInstance::shut()
{
    if (DEBUG)
	printf("shut called\n");

    if (onDestroyCallback != NULL) {
	if (DEBUG)
	    printf("Destroy Callback = %s\n", onDestroyCallback);
	NPN_GetURL(mInstance, onDestroyCallback, NULL);
    }

    shutdown();

}

void nsPluginInstance::shutdown()
{
    int i;
    long flags;

    if (DEBUG)
	printf("shutdown called\n");

    if (threadsetup == 1 && threadsignaled == 0) {
	if (DEBUG)
	    printf
		("Thread is setup but waiting for signal so we need to shut it down\n");
	signalPlayerThread(this);
	threadsignaled = 1;

    }

    if (threadsetup == 1) {

	if (threadlaunched == 1) {

	    // switch to non blocking mode
	    if (player != NULL) {
		flags = fcntl(fileno(player), F_GETFL, 0);
		flags |= O_NONBLOCK;
		fcntl(fileno(player), F_SETFL, flags);
	    }

	    pthread_mutex_lock(&control_mutex);
	    if (paused == 1) {
		sendCommand(this, "pause\n");
		paused = 0;
	    }
	    sendCommand(this, "quit\n");
#ifndef BSD
	    pthread_mutex_lock(&read_mutex);
#endif
	    cancelled = 1;
#ifndef BSD
	    pthread_mutex_unlock(&read_mutex);
#endif
	    pthread_mutex_unlock(&control_mutex);
	    pthread_cancel(player_thread);
	    pthread_join(player_thread, NULL);
	    js_state = JS_STATE_UNDEFINED;

	}
    }
#ifdef GTK_ENABLED
    while (g_idle_remove_by_data(this)) {
	if (DEBUG)
	    printf("Removing function from idle handler\n");
    }
#endif

    if (pid != 0) {
	killmplayer(this);
    }

    if (DEBUG)
	printf("mplayer dead\n");

#ifdef X_ENABLED
    FreeUI((Display *) display, this);
#endif

    mInitialized = FALSE;

#ifdef GTK_ENABLED
    while (g_idle_remove_by_data(this)) {
	if (DEBUG)
	    printf("Removing function from idle handler\n");
    }
#endif

#ifdef GTK2_ENABLED
    if (controlwindow == 0) {
	if (conf_window != NULL) {
	    if (GTK_IS_WIDGET(conf_window))
		gtk_widget_destroy(conf_window);
	}
	if (targetplayer != 0) {
	    if (GTK_IS_WIDGET(gtkwidget)) {
		g_signal_handler_disconnect(GTK_OBJECT(gtkwidget),
					    delete_signal_id);
		g_signal_handler_disconnect(GTK_OBJECT(gtkwidget),
					    visible_signal_id);
	    }
	    if (GTK_IS_WIDGET(button_window))
		gtk_widget_destroy(button_window);

	}
	if (GTK_IS_WIDGET(gtkwidget))
	    gtk_widget_destroy(gtkwidget);

    }
#endif

    if (DEBUG)
	printf("Window Cleaned up\n");

#ifdef GTK1_ENABLED
    if (controlwindow == 0) {
	if (targetplayer != 0)
	    gtk_signal_disconnect(GTK_OBJECT(gtkwidget), delete_signal_id);
	if (GTK_IS_WIDGET(gtkwidget))
	    gtk_widget_destroy(gtkwidget);
    }

    gdk_flush();
    XSync(display, FALSE);

#endif

#ifdef GTK_ENABLED
    image = NULL;
    progress_bar = NULL;
    mediaprogress_bar = NULL;
    status = NULL;
    play_event_box = NULL;
    pause_event_box = NULL;
    stop_event_box = NULL;
    ff_event_box = NULL;
    rew_event_box = NULL;
    fs_event_box = NULL;
    gtkwidget = NULL;
    fixed_container = NULL;
    drawing_area = NULL;
    popup_menu = NULL;
#ifdef GTK2_ENABLED
    fs_window = NULL;
#endif
    gtkwidget = NULL;
#endif

#ifdef DPMSExtension
    if (DPMSEnabled)
	DPMSReenable(this);
#endif

    if (mimetype != NULL) {
	free(mimetype);
	mimetype = NULL;
    }

    if (href != NULL) {
	free(href);
	href = NULL;
    }

    if (fname != NULL) {
	free(fname);
	fname = NULL;
    }

    if (url != NULL) {
	free(url);
	url = NULL;
    }

    if (baseurl != NULL) {
	NPN_MemFree(baseurl);
	baseurl = NULL;
    }

    if (hostname != NULL) {
	NPN_MemFree(hostname);
	hostname = NULL;
    }

    if (vo != NULL) {
	free(vo);
	vo = NULL;
    }

    if (vop != NULL) {
	free(vop);
	vop = NULL;
    }

    if (ao != NULL) {
	free(ao);
	ao = NULL;
    }

    if (useragent != NULL) {
	free(useragent);
	useragent = NULL;
    }

    if (output_display != NULL) {
	free(output_display);
	output_display = NULL;
    }

    if (tv_driver != NULL) {
	free(tv_driver);
	tv_driver = NULL;
    }

    if (tv_input != NULL) {
	free(tv_input);
	tv_input = NULL;

    }

    if (tv_outfmt != NULL) {
	free(tv_outfmt);
	tv_outfmt = NULL;
    }

    if (profile != NULL) {
	free(profile);
	profile = NULL;
    }


    nQtNext = 0;
    for (i = 0; i < 256; i++) {
	if (qtNext[i] != NULL)
	    free(qtNext[i]);
	qtNext[i] = NULL;

    }

    if (download_dir != NULL) {
	free(download_dir);
	download_dir = NULL;
    }

    if (td->list != NULL) {
	pthread_mutex_lock(&playlist_mutex);
	deleteList(td->list);
	td->list = NULL;
	list = NULL;
	pthread_mutex_unlock(&playlist_mutex);
    }

    if (td != NULL) {
	td->instance = NULL;
	NPN_MemFree(td);
	td = NULL;
    }

    if (lastmessage != NULL) {
	NPN_MemFree(lastmessage);
	lastmessage = NULL;
    }

    if (mediaCompleteCallback != NULL) {
	NPN_MemFree(mediaCompleteCallback);
	mediaCompleteCallback = NULL;
    }

    if (mouseClickCallback != NULL) {
	NPN_MemFree(mouseClickCallback);
	mouseClickCallback = NULL;
    }

    if (onVisibleCallback != NULL) {
	NPN_MemFree(onVisibleCallback);
	onVisibleCallback = NULL;
    }

    if (onHiddenCallback != NULL) {
	NPN_MemFree(onHiddenCallback);
	onHiddenCallback = NULL;
    }

    if (DEBUG)
	printf("memory free\n");

    autostart = 1;
    showcontrols = 1;
    showtracker = 1;
    showbuttons = 1;
    showfsbutton = 1;
    panel_drawn = 0;
    mmsstream = 0;
    cancelled = 0;
    js_state = JS_STATE_UNDEFINED;

    if (DEBUG > 1)
	printf("destorying pthread attrs, mutexes and conds\n");
    pthread_attr_destroy(&thread_attr);
    if (DEBUG > 1)
	printf("thread_attr destroyed\n");
    pthread_mutex_destroy(&playlist_mutex);
    if (DEBUG > 1)
	printf("playlist_mutex destroyed\n");
    pthread_mutex_destroy(&playlist_cond_mutex);
    if (DEBUG > 1)
	printf("playlist_cond_mutex destroyed\n");
    pthread_mutex_destroy(&control_mutex);
    if (DEBUG > 1)
	printf("control_mutex destroyed\n");
    pthread_mutex_destroy(&read_mutex);
    if (DEBUG > 1)
	printf("read_mutex destroyed\n");
    pthread_cond_destroy(&playlist_complete_cond);
    if (DEBUG > 1)
	printf("playlist_complete_cond destroyed\n");

}

NPBool nsPluginInstance::isInitialized()
{
    return mInitialized;
}

NPError nsPluginInstance::SetWindow(NPWindow * aWindow)
{
    NPSetWindowCallbackStruct *ws;
    GC black_gc;
    XGCValues values;
    char message[100];
#ifdef GTK_ENABLED
    int multiplier, height, width;
    GdkColormap *colormap;
    GdkColor black, white;
#endif
#ifdef GTK1_ENABLED
//    GtkStyle *s;
#endif

    if (DEBUG >= 2)
	printf("*****SetWindow Callback Enter************\n");


    if ((aWindow == NULL) || (aWindow->window == NULL)) {
	return NPERR_NO_ERROR;
    }

    if ((Window) window != (Window) aWindow->window) {
	if (DEBUG)
	    printf("New window! old: 0x%li    new 0x%li\n",
		   (unsigned long int) window,
		   (unsigned long int) aWindow->window);
    }

    if (controlwindow == 1)
	return NPERR_NO_ERROR;

    if (toolkitok != 0) {	// something is wrong
	ws = (NPSetWindowCallbackStruct *) aWindow->ws_info;
	values.foreground =
	    BlackPixel(ws->display, DefaultScreen(ws->display));
	black_gc =
	    XCreateGC(ws->display, (Window) aWindow->window, GCForeground,
		      &values);
	snprintf(message, 100,
		 "Toolkit mismatch mozilla(GTK%i), plug-in(GTK%i)",
		 moz_toolkit, plug_toolkit);
	XDrawString(ws->display, (Window) aWindow->window, black_gc, 10,
		    10, message, strlen(message));
	XFreeGC(ws->display, black_gc);
	return NPERR_NO_ERROR;
    }

    if (state < STATE_WINDOWSET) {
	if (DEBUG) {
	    printf("Size: %d %d %p\n", aWindow->x, aWindow->y,
		   aWindow->window);
	    printf("Size: %dx%d \n", aWindow->width, aWindow->height);
	}

	ws = (NPSetWindowCallbackStruct *) aWindow->ws_info;
	display = ws->display;

#ifdef X_ENABLED
	widget =
	    XtWindowToWidget((Display *) ws->display,
			     (Window) aWindow->window);
	XtAddEventHandler(widget, ExposureMask, FALSE,
			  (XtEventHandler) RedrawCB, this);
#endif
#ifdef DPMSExtension
	DPMSEnabled = DPMSIsEnabled(this);
#endif
	window = (Window) aWindow->window;
	window_width = aWindow->width;
	window_height = aWindow->height;
	state = STATE_WINDOWSET;

#ifdef GTK_ENABLED
	gtkplug = gtk_plug_new(window);

	if (targetplayer == 0) {
	    if (dontparent == 0) {
		gtkwidget = gtk_window_new(GTK_WINDOW_POPUP);
	    } else {
		gtkwidget = gtk_window_new(GTK_WINDOW_TOPLEVEL);
	    }
	} else {
	    gtkwidget = gtk_window_new(GTK_WINDOW_TOPLEVEL);
	    button_window = gtk_window_new(GTK_WINDOW_POPUP);
#ifdef GTK2_ENABLED
	    delete_signal_id =
		g_signal_connect(GTK_OBJECT(gtkwidget), "delete_event",
				 G_CALLBACK(target_hide_callback), this);
#endif
#ifdef GTK1_ENABLED
	    delete_signal_id = gtk_signal_connect(GTK_OBJECT(gtkwidget),
						  "delete_event",
						  GTK_SIGNAL_FUNC
						  (target_hide_callback),
						  this);
#endif
	    gtk_window_set_title(GTK_WINDOW(gtkwidget),
				 "mplayerplug-in player");

	}

	gtk_widget_realize(gtkwidget);
	gdk_flush();

	gtk_widget_add_events(gtkwidget, GDK_BUTTON_PRESS_MASK);
	gtk_widget_add_events(gtkwidget, GDK_BUTTON_RELEASE_MASK);
	gtk_widget_add_events(gtkwidget, GDK_ENTER_NOTIFY_MASK);
	gtk_widget_add_events(gtkwidget, GDK_LEAVE_NOTIFY_MASK);
	gtk_widget_add_events(gtkwidget, GDK_KEY_PRESS_MASK);
	gtk_widget_add_events(gtkwidget, GDK_VISIBILITY_NOTIFY_MASK);

	if (targetplayer == 1) {
	    window_width = 400;
	    window_height = 200;
	    movie_width = 400;
	    movie_height = 200;
	}
	if (DEBUG > 1)
	    printf("setting window size\n");
#ifdef GTK2_ENABLED
	gtk_widget_set_size_request(gtkwidget, window_width,
				    window_height);
	logo = gdk_pixbuf_new_from_inline(-1, gtk_logo, FALSE, NULL);
	image = gtk_image_new_from_pixbuf(logo);
#endif
#ifdef GTK1_ENABLED
	gtk_widget_set_usize(gtkwidget, window_width, window_height);
#endif
	popup_menu = GTK_MENU(gtk_menu_new());
	menuitem_play =
	    GTK_MENU_ITEM(gtk_menu_item_new_with_label(_("Play")));
	gtk_menu_append(popup_menu, GTK_WIDGET(menuitem_play));
	gtk_widget_show(GTK_WIDGET(menuitem_play));
	menuitem_pause =
	    GTK_MENU_ITEM(gtk_menu_item_new_with_label(_("Pause")));
	gtk_menu_append(popup_menu, GTK_WIDGET(menuitem_pause));
	gtk_widget_show(GTK_WIDGET(menuitem_pause));
	menuitem_stop =
	    GTK_MENU_ITEM(gtk_menu_item_new_with_label(_("Stop")));
	gtk_menu_append(popup_menu, GTK_WIDGET(menuitem_stop));
	gtk_widget_show(GTK_WIDGET(menuitem_stop));
#ifdef GTK2_ENABLED
	menuitem_sep1 = GTK_MENU_ITEM(gtk_separator_menu_item_new());
#endif
#ifdef GTK1_ENABLED
	menuitem_sep1 = GTK_MENU_ITEM(gtk_menu_item_new());
#endif
	gtk_menu_append(popup_menu, GTK_WIDGET(menuitem_sep1));
	gtk_widget_show(GTK_WIDGET(menuitem_sep1));
	menuitem_showcontrols =
	    GTK_MENU_ITEM(gtk_check_menu_item_new_with_label
			  (_("Show Controls")));
	gtk_menu_append(popup_menu, GTK_WIDGET(menuitem_showcontrols));
	gtk_widget_show(GTK_WIDGET(menuitem_showcontrols));
	if (showcontrols)
	    gtk_check_menu_item_set_active(GTK_CHECK_MENU_ITEM
					   (menuitem_showcontrols), TRUE);
	menuitem_fullscreen =
	    GTK_MENU_ITEM(gtk_check_menu_item_new_with_label
			  (_("Full Screen")));
	gtk_menu_append(popup_menu, GTK_WIDGET(menuitem_fullscreen));
	gtk_widget_show(GTK_WIDGET(menuitem_fullscreen));
#ifdef GTK2_ENABLED
	menuitem_sep2 = GTK_MENU_ITEM(gtk_separator_menu_item_new());
#endif
#ifdef GTK1_ENABLED
	menuitem_sep2 = GTK_MENU_ITEM(gtk_menu_item_new());
#endif
	gtk_menu_append(popup_menu, GTK_WIDGET(menuitem_sep2));
	gtk_widget_show(GTK_WIDGET(menuitem_sep2));
	menuitem_copy =
	    GTK_MENU_ITEM(gtk_menu_item_new_with_label(_("Copy URL")));
	gtk_menu_append(popup_menu, GTK_WIDGET(menuitem_copy));
	gtk_widget_show(GTK_WIDGET(menuitem_copy));
	menuitem_save =
	    GTK_MENU_ITEM(gtk_menu_item_new_with_label(_("Save")));
	gtk_menu_append(popup_menu, GTK_WIDGET(menuitem_save));
	gtk_widget_show(GTK_WIDGET(menuitem_save));
	gtk_widget_set_sensitive(GTK_WIDGET(menuitem_save), FALSE);

#ifdef GTK2_ENABLED
	menuitem_sep3 = GTK_MENU_ITEM(gtk_separator_menu_item_new());
	gtk_menu_append(popup_menu, GTK_WIDGET(menuitem_sep3));
	gtk_widget_show(GTK_WIDGET(menuitem_sep3));
	menuitem_config =
	    GTK_MENU_ITEM(gtk_menu_item_new_with_label(_("Configure")));
	gtk_menu_append(popup_menu, GTK_WIDGET(menuitem_config));
	gtk_widget_show(GTK_WIDGET(menuitem_config));
#endif

#ifdef GTK2_ENABLED
	if (enablecontextmenu != 0) {
	    g_signal_connect_swapped(G_OBJECT(gtkwidget),
				     "button_press_event",
				     G_CALLBACK(popup_handler),
				     GTK_OBJECT(popup_menu));
	}
	g_signal_connect(G_OBJECT(gtkwidget),
			 "key_press_event",
			 G_CALLBACK(keyboard_callback), this);
	g_signal_connect(GTK_OBJECT(menuitem_play), "activate",
			 G_CALLBACK(menuitem_play_callback), this);
	g_signal_connect(GTK_OBJECT(menuitem_pause), "activate",
			 G_CALLBACK(menuitem_pause_callback), this);
	g_signal_connect(GTK_OBJECT(menuitem_stop), "activate",
			 G_CALLBACK(menuitem_stop_callback), this);
	g_signal_connect(GTK_OBJECT(menuitem_showcontrols), "toggled",
			 G_CALLBACK(menuitem_showcontrols_callback), this);
	g_signal_connect(GTK_OBJECT(menuitem_fullscreen), "toggled",
			 G_CALLBACK(menuitem_fullscreen_callback), this);
	g_signal_connect(GTK_OBJECT(menuitem_save), "activate",
			 G_CALLBACK(menuitem_save_callback), this);
	g_signal_connect(GTK_OBJECT(menuitem_copy), "activate",
			 G_CALLBACK(menuitem_copy_callback), this);
	g_signal_connect(GTK_OBJECT(menuitem_config), "activate",
			 G_CALLBACK(menuitem_config_callback), this);
	g_signal_connect(G_OBJECT(gtkwidget), "button_press_event",
			 G_CALLBACK(mouse_callback), this);
	g_signal_connect(G_OBJECT(gtkwidget), "button_release_event",
			 G_CALLBACK(mouse_callback), this);
	g_signal_connect(G_OBJECT(gtkwidget), "enter_notify_event",
			 G_CALLBACK(mousenotify_callback), this);
	g_signal_connect(G_OBJECT(gtkwidget), "leave_notify_event",
			 G_CALLBACK(mousenotify_callback), this);

#endif

#ifdef GTK1_ENABLED
	gtk_signal_connect_object(GTK_OBJECT(gtkwidget),
				  "button_press_event",
				  GTK_SIGNAL_FUNC(popup_handler),
				  GTK_OBJECT(popup_menu));
	gtk_signal_connect(GTK_OBJECT(gtkwidget),
			   "key_press_event",
			   GTK_SIGNAL_FUNC(keyboard_callback), this);
	gtk_signal_connect(GTK_OBJECT(menuitem_play), "activate",
			   GTK_SIGNAL_FUNC(menuitem_play_callback), this);
	gtk_signal_connect(GTK_OBJECT(menuitem_pause), "activate",
			   GTK_SIGNAL_FUNC(menuitem_pause_callback), this);
	gtk_signal_connect(GTK_OBJECT(menuitem_stop), "activate",
			   GTK_SIGNAL_FUNC(menuitem_stop_callback), this);
	gtk_signal_connect(GTK_OBJECT(menuitem_showcontrols), "toggled",
			   GTK_SIGNAL_FUNC(menuitem_showcontrols_callback),
			   this);
	gtk_signal_connect(GTK_OBJECT(menuitem_fullscreen), "toggled",
			   GTK_SIGNAL_FUNC(menuitem_fullscreen_callback),
			   this);
	gtk_signal_connect(GTK_OBJECT(menuitem_save), "activate",
			   GTK_SIGNAL_FUNC(menuitem_save_callback), this);
	gtk_signal_connect(GTK_OBJECT(menuitem_copy), "activate",
			   GTK_SIGNAL_FUNC(menuitem_copy_callback), this);
	gtk_signal_connect(GTK_OBJECT(gtkwidget), "button_press_event",
			   GTK_SIGNAL_FUNC(mouse_callback), this);

#endif
	if (DEBUG > 1)
	    printf("menus built\n");

	fixed_event_box = gtk_event_box_new();
	fixed_container = gtk_fixed_new();


#ifdef GTK2_ENABLED

	gtk_widget_set_size_request(GTK_WIDGET(fixed_container),
				    window_width, window_height);
	gtk_widget_set_size_request(GTK_WIDGET(fixed_event_box),
				    window_width, window_height);

#endif
#ifdef GTK1_ENABLED
	gtk_widget_set_usize(GTK_WIDGET(fixed_container), window_width,
			     window_height);
	gtk_widget_set_usize(GTK_WIDGET(fixed_event_box), window_width,
			     window_height);

#endif
	gtk_container_add(GTK_CONTAINER(fixed_event_box), fixed_container);

	status = GTK_LABEL(gtk_label_new(_("Initializing")));
	// I don't really like this as it messes up the right click menu
	// gtk_label_set_selectable(status,TRUE);
	progress_bar = GTK_PROGRESS_BAR(gtk_progress_bar_new());

	if (window_height > 125) {

#ifdef GTK2_ENABLED
	    gtk_fixed_put(GTK_FIXED(fixed_container), image, 10, 10);
	    gtk_widget_set_size_request(GTK_WIDGET(progress_bar),
					window_width - 20, 15);
	    gtk_widget_set_size_request(GTK_WIDGET(status),
					window_width - 20,
					window_height - 125);
#endif
#ifdef GTK1_ENABLED
	    if (DEBUG > 1)
		printf("> 125 setup\n");
	    gtk_widget_show(gtkwidget);
	    logo = gdk_pixmap_create_from_xpm_d(gtkwidget->window,
						NULL, NULL, logo_xpm);
	    image = gtk_pixmap_new(logo, NULL);
	    gtk_fixed_put(GTK_FIXED(fixed_container), GTK_WIDGET(image),
			  10, 10);
	    gtk_widget_set_usize(GTK_WIDGET(progress_bar),
				 aWindow->width - 20, 15);
	    gtk_widget_set_usize(GTK_WIDGET(status),
				 aWindow->width - 20,
				 aWindow->height - 125);
#endif
	    gtk_fixed_put(GTK_FIXED(fixed_container),
			  GTK_WIDGET(progress_bar), 10,
			  window_height - 50);
	    if (showlogo)
		gtk_widget_show(image);
	    if (!hidestatus)
		gtk_widget_show(GTK_WIDGET(progress_bar));
	    gtk_fixed_put(GTK_FIXED(fixed_container), GTK_WIDGET(status),
			  10, 60);
	} else {
	    // small window layout
	    gtk_fixed_put(GTK_FIXED(fixed_container),
			  GTK_WIDGET(status), 0, 0);
	    gtk_fixed_put(GTK_FIXED(fixed_container),
			  GTK_WIDGET(progress_bar), 10,
			  window_height - 31);
	    if (!hidestatus)
		gtk_widget_show(GTK_WIDGET(progress_bar));
#ifdef GTK2_ENABLED
	    if (window_width > 20)
		gtk_widget_set_size_request(GTK_WIDGET(progress_bar),
					    window_width - 20, 15);
	    if (window_height > 31)
		gtk_widget_set_size_request(GTK_WIDGET(status),
					    window_width,
					    window_height - 31);
#endif
#ifdef GTK1_ENABLED
	    if (aWindow->width > 20)
		gtk_widget_set_usize(GTK_WIDGET(progress_bar),
				     aWindow->width - 20, 15);
	    if (aWindow->height > 31)
		gtk_widget_set_usize(GTK_WIDGET(status),
				     aWindow->width, aWindow->height - 31);
#endif
	}

	gtk_misc_set_alignment(GTK_MISC(status), 0, 0);
	gtk_label_set_line_wrap(status, TRUE);
	drawing_area = gtk_socket_new();

	if (mode == NP_EMBED) {
	    gtk_fixed_put(GTK_FIXED(fixed_container), drawing_area, 0, 0);
	} else {
	    gtk_fixed_put(GTK_FIXED(fixed_container),
			  drawing_area, 10, 100);
	}
	gtk_widget_set_usize(drawing_area, 1, 1);
	gtk_widget_show(drawing_area);


	gtk_container_add(GTK_CONTAINER(gtkwidget), fixed_event_box);
	if (!hidestatus)
	    gtk_widget_show(GTK_WIDGET(status));
	gtk_widget_show(fixed_container);
	gtk_widget_show(fixed_event_box);
	if (targetplayer == 0)
	    gtk_widget_show(gtkwidget);

	if (black_background != 0) {
#ifdef GTK_ENABLED
	    colormap = gdk_window_get_colormap(gtkwidget->window);
	    gdk_color_parse("black", &black);
	    gdk_color_parse("white", &white);
	    gdk_colormap_alloc_color(colormap, &black, FALSE, TRUE);
	    gdk_colormap_alloc_color(colormap, &white, FALSE, TRUE);
#endif

#ifdef GTK2_ENABLED
	    gtk_widget_modify_bg(gtkwidget, GTK_STATE_NORMAL, &black);
	    gtk_widget_modify_bg(fixed_event_box, GTK_STATE_NORMAL,
				 &black);
	    gtk_widget_modify_fg(GTK_WIDGET(status), GTK_STATE_NORMAL,
				 &white);
#endif
#ifdef GTK_ENABLED
	    gdk_window_set_background(gtkwidget->window, &black);
	    gdk_window_clear(gtkwidget->window);
#endif
	}

	gdk_flush();
	InitPixbufs(this);
#ifdef GTK_ENABLED

	if (targetplayer == 0) {
	    if (dontparent == 0) {
		XReparentWindow(GDK_WINDOW_XDISPLAY(gtkwidget->window),
				GDK_WINDOW_XWINDOW(gtkwidget->window),
				window, 0, 0);
		gtk_widget_map(gtkwidget);
	    }
	} else {
	    gtk_widget_show(button_window);
	    XReparentWindow(GDK_WINDOW_XDISPLAY(button_window->window),
			    GDK_WINDOW_XWINDOW(button_window->window),
			    window, 0, 0);
	    gtk_widget_map(button_window);
	}
#endif
	if (panel_drawn == 0 && autostart == 0 && nomediacache == 1) {
	    if (showcontrols) {
		panel_height = window_height;
#ifdef GTK_ENABLED
		g_idle_add(gtkgui_draw, this);
#endif
	    }
	}
#endif

    } else {
	if (setwindow == 0) {
	    if (DEBUG)
		printf("back in SetWindow\n");
	    if (DEBUG)
		printf("New Size: %ix%i\n", aWindow->width,
		       aWindow->height);
	    setwindow++;

	    if (DEBUG > 1)
		printf("Current state is %i\n", state);

	    if (state != STATE_GETTING_PLAYLIST) {
		state = STATE_GETTING_PLAYLIST;

		if (url != NULL) {
		    // hard code 0 here by doing this it actually makes nomediacache work properly
		    // http src url's are automatically requested, so if we request an http here too, we get it twice
		    if (isMms(url, 0)) {
			snprintf(list->url, 4096, "%s", url);
			state = STATE_STARTED_PLAYER;
			list->mmsstream = 1;
			if (threadsetup == 0 && controlwindow == 0)
			    SetupPlayer(this, NULL);
		    }
		}

		if (fname != NULL) {
		    if (isMms(fname, nomediacache)) {
			snprintf(list->url, 4096, "%s", fname);
			state = STATE_STARTED_PLAYER;
			list->mmsstream = 1;
			if (threadsetup == 0 && controlwindow == 0)
			    SetupPlayer(this, NULL);
		    } else {
			if (autostart) {
			    Play();
			} else {
			    pthread_mutex_lock(&control_mutex);
			    js_state = JS_STATE_READY;
			    pthread_mutex_unlock(&control_mutex);
			}
		    }
		}

		if ((href != NULL) && (url == NULL) && (fname == NULL)) {
		    if (isMms(href, nomediacache)) {
			snprintf(list->url, 4096, "%s", href);
			state = STATE_STARTED_PLAYER;
			list->mmsstream = 1;
			if (threadsetup == 0 && controlwindow == 0)
			    SetupPlayer(this, NULL);
		    } else {
			if (autostart) {
			    Play();
			} else {
			    pthread_mutex_lock(&control_mutex);
			    js_state = JS_STATE_READY;
			    pthread_mutex_unlock(&control_mutex);
			}
		    }
		}
	    }
	}
	if (mode == NP_EMBED) {
	    embed_width = aWindow->width;
	    embed_height = aWindow->height;
	} else {
	    window_width = aWindow->width;
	    window_height = aWindow->height;
	}
    }

    if (DEBUG)
	printf("resizing widgets to %i x %i \n", aWindow->width,
	       aWindow->height);

#ifdef GTK_ENABLED
    if ((status != NULL) && (targetplayer == 0)) {
	window_height = aWindow->height;
	window_width = aWindow->width;
	if (!hidestatus)
	    gtk_widget_show(GTK_WIDGET(status));
#ifdef GTK2_ENABLED
	if (window_width > 20 && window_height > 25)
	    gtk_widget_set_size_request(GTK_WIDGET(status),
					window_width - 20,
					window_height - 25);

	if (window_width > 0 && window_height > 0)
	    gtk_widget_set_size_request(GTK_WIDGET(gtkwidget),
					window_width, window_height);

#endif
#ifdef GTK1_ENABLED
	if (window_width > 20 && window_height > 25)
	    gtk_widget_set_usize(GTK_WIDGET(status),
				 window_width - 20, window_height - 25);
	if (window_width > 0 && window_height > 0)
	    gtk_widget_set_usize(GTK_WIDGET(gtkwidget),
			     window_width, window_height);
#endif
	if (panel_drawn == 1) {
	    height = 16;
	    width = 21;

	    multiplier = 0;

	    if (mmsstream == 0) {
		gtk_fixed_move(GTK_FIXED(fixed_container),
			       GTK_WIDGET(rew_event_box),
			       width * multiplier++,
			       window_height - height);
	    }
	    gtk_fixed_move(GTK_FIXED(fixed_container),
			   GTK_WIDGET(play_event_box),
			   width * multiplier++, window_height - height);
	    gtk_fixed_move(GTK_FIXED(fixed_container),
			   GTK_WIDGET(pause_event_box),
			   width * multiplier++, window_height - height);
	    gtk_fixed_move(GTK_FIXED(fixed_container),
			   GTK_WIDGET(stop_event_box),
			   width * multiplier++, window_height - height);
	    if (mmsstream == 0) {
		gtk_fixed_move(GTK_FIXED(fixed_container),
			       GTK_WIDGET(ff_event_box),
			       width * multiplier++,
			       window_height - height);
	    }
	    if (GTK_IS_WIDGET(mediaprogress_bar)) {
		gtk_fixed_move(GTK_FIXED(fixed_container),
			       GTK_WIDGET(mediaprogress_bar),
			       (width * multiplier + 10),
			       window_height - height + 2);

		gtk_widget_set_usize(GTK_WIDGET(mediaprogress_bar),
				     window_width -
				     (width * (multiplier + 2) + 20),
				     height - 4);
	    }
	    gtk_fixed_move(GTK_FIXED(fixed_container),
			   GTK_WIDGET(vol_event_box),
			   (window_width - (width * 2)),
			   window_height - height);
	    gtk_fixed_move(GTK_FIXED(fixed_container),
			   GTK_WIDGET(fs_event_box),
			   (window_width - width), window_height - height);

	}
	if (progress_bar != NULL) {
	    gtk_fixed_move(GTK_FIXED(fixed_container),
			   GTK_WIDGET(progress_bar),
			   10, window_height - 31);
	    if (window_width > 20 && window_height > 0)
		gtk_widget_set_usize(GTK_WIDGET(progress_bar),
				     window_width - 20, 15);
	}

	if (GTK_IS_WIDGET(drawing_area) && (movie_width > 0)
	    && (movie_height > 0) && mode == NP_EMBED) {
	    if (showcontrols) {
		if (window_width > 0 && window_height > 16)
		    gtk_widget_set_usize(GTK_WIDGET(drawing_area),
					 window_width, window_height - 16);
	    } else {
		if (window_width > 0 && window_height > 0)
		    gtk_widget_set_usize(GTK_WIDGET(drawing_area),
					 window_width, window_height);
	    }
	}

    }
    if (gtkwidget == NULL)
	return NPERR_NO_ERROR;

    if (targetplayer == 0)
	gtk_widget_show(gtkwidget);

    if (DEBUG > 1)
	printf("resize is complete\n");

    if (panel_drawn == 0 && autostart == 0 && nomediacache == 1) {
	if (showcontrols) {
	    panel_height = window_height;
#ifdef GTK_ENABLED
	    g_idle_add(gtkgui_draw, this);
#endif
	}
    }
#endif

    if (DEBUG >= 2) {
	printf("***********SetWindow Callback Exit**************\n");
    }


    return NPERR_NO_ERROR;
}

NPError nsPluginInstance::NewStream(NPMIMEType type, NPStream * stream,
				    NPBool seekable, uint16 * stype)
{

    if (DEBUG >= 2)
	printf("**********NewStream Callback %s ****************\n",
	       stream->url);

    if (baseurl == NULL)
	baseurl = getURLBase((char *) stream->url);
    if (hostname == NULL)
	hostname = getURLHostname((char *) stream->url);

    if (mode == NP_FULL) {
	url = strdup(stream->url);
    }

    if ((threadsetup == 0) && (controlwindow == 0)) {
	state = STATE_GETTING_PLAYLIST;
	SetupPlayer(this, NULL);
    }
    *stype = NP_NORMAL;

    if (DEBUG >= 2)
	printf("*********Exiting NewStream Callback*****************\n");

    return NPERR_NO_ERROR;
}


NPError nsPluginInstance::DestroyStream(NPStream * stream, NPError reason)
{
    int playable, all_retrieved, all_above_cache;
    Node *n;
    char *tmp;

    if (DEBUG >= 2)
	printf("***********NPP_DestroyStream called %i\n URL: %s\n",
	       reason, stream->url);


    if (reason == NPRES_DONE) {
	playable = 0;
	if (strlen(stream->url) >= 1023) {
	    return NPERR_NO_ERROR;
	}
	pthread_mutex_lock(&playlist_mutex);
	n = td->list;
	while (n != NULL) {
	    if (URLcmp(n->url, stream->url) == 0) {
		if (DEBUG)
		    printf("Destroy stream found a URL match\n%s\n%s\n",
			   n->url, stream->url);
		break;
	    } else {
		if (strstr(stream->url, n->url) != NULL) {
		    break;
		}
	    }
	    n = n->next;
	}

	if (n != NULL) {
	    n->retrieved = 1;
#ifdef GTK_ENABLED
	    g_idle_add(gtkgui_save_enable, this);
#endif
	    if (n->localcache != NULL) {
		if (fclose(n->localcache) != 0) {
		    if (DEBUG)
			printf("fclose had an error %i : %s\n", errno,
			       strerror(errno));
		};
		n->localcache = NULL;
	    }

	    if (controlwindow == 1) {
		if (n->fname != NULL)
		    remove(n->fname);
	    }

	    if (DEBUG)
		printf
		    ("checking to see if we need to make a button\nn->url=%s\nurl=%s\nhref=%s\n",
		     n->url, url, (href == NULL) ? "(NULL)" : href);

	    if (n->url != NULL && url != NULL) {
		if ((strncmp(n->url, url, 4096) == 0) && href != NULL) {
		    if (srcToButton(n->fname, this)) {
			n->play = 0;
			pthread_mutex_unlock(&playlist_mutex);
			return NPERR_NO_ERROR;
		    } else {
			n = newNode();
			snprintf(n->url, 4096, "%s", href);
			addToEnd(td->list, n);
			pthread_mutex_unlock(&playlist_mutex);
			NPN_GetURL(mInstance, href, NULL);
			return NPERR_NO_ERROR;
		    }
		} else if (strstr(n->url, url) && href != NULL) {
		    // we are in this block because the url is a relative url and so
		    // it is a subset of the fully qualified n->url
		    if (srcToButton(n->fname, this)) {
			n->play = 0;
			pthread_mutex_unlock(&playlist_mutex);
			return NPERR_NO_ERROR;
		    } else {
			n = newNode();
			snprintf(n->url, 4096, "%s", href);
			addToEnd(td->list, n);
			pthread_mutex_unlock(&playlist_mutex);
			NPN_GetURL(mInstance, href, NULL);
			return NPERR_NO_ERROR;
		    }
		}
	    }

	    if (!isMms(n->url, nomediacache)) {
		if (DEBUG)
		    printf("calling buildPlaylist with filename %s\n",
			   n->fname);
		buildPlaylist(this, n->fname, n);
		if (mode == NP_FULL || noembed == 1) {
#ifdef X_ENABLED
		    DrawUI(widget, this, _("Download Complete"), 0, 99);
#endif
#ifdef GTK_ENABLED
		    if (status != NULL) {
			gtk_label_set_text(status, _("Download Complete"));
		    }
		    g_idle_add(gtkgui_save_enable, this);
		    if (progress_bar != NULL) {
			gtk_progress_bar_update(progress_bar, 1.0);
			if (n->next == NULL) {
			    gtk_widget_hide(GTK_WIDGET(progress_bar));
			} else {
			    if (movie_width == 0 && movie_height == 0)
				if (!hidestatus)
				    gtk_widget_show(GTK_WIDGET
						    (progress_bar));
			}
		    }
#endif
		}
		if (DEBUG > 1)
		    printf("buildPlaylist is complete\n");
		if (strncasecmp(mimetype, "application/sdp", 15) == 0) {
		    tmp = strdup(n->fname);
		    strcpy(n->fname, "sdp://");
		    strcat(n->fname, tmp);
		    free(tmp);
		}
		if (n->playlist == 1 || n->cancelled == 1) {
		    if (n->mmsstream == 0) {
			if (n->next == NULL) {
			    pthread_mutex_unlock(&playlist_mutex);
			    if (DEBUG > 1)
				printf
				    ("Exiting DS with playlist = %i, cancelled = %i, mmsstream = 0, and n->url = %s and threadsignaled = %i\n",
				     n->playlist, n->cancelled, n->url,
				     threadsignaled);
			    if (autostart && threadsignaled == 0) {
				signalPlayerThread(this);
				threadsignaled = 1;
			    } else {
				if (showcontrols && panel_drawn == 0) {
				    panel_height = 16;
#ifdef GTK_ENABLED
				    g_idle_add(gtkgui_draw, this);
#endif
				    pthread_mutex_lock(&control_mutex);
				    js_state = JS_STATE_READY;
				    pthread_mutex_unlock(&control_mutex);

				}
			    }

			    return NPERR_NO_ERROR;
			}
		    }
		}
	    } else {
		n->mmsstream = 1;
		pthread_mutex_unlock(&playlist_mutex);
	    }


	    // test for all retrieved
	    if (threadsignaled == 0) {
		all_retrieved = 1;
		n = td->list;
		while (n != NULL) {
		    if ((n->retrieved == 0) && (n->play == 1)) {
			all_retrieved = 0;
			break;
		    }
		    n = n->next;
		}
		// if all retrieved signal player
		if (all_retrieved) {
		    if (threadsignaled == 0) {
			if (DEBUG)
			    printf("signalling player (retrieved)\n");
			if (autostart) {
			    signalPlayerThread(this);
			    threadsignaled = 1;
			} else {
			    if (showcontrols && panel_drawn == 0) {
				panel_height = 16;
#ifdef GTK_ENABLED
				g_idle_add(gtkgui_draw, this);
#endif
				pthread_mutex_lock(&control_mutex);
				js_state = JS_STATE_READY;
				pthread_mutex_unlock(&control_mutex);

			    }
			}
		    }
		}
	    }
	    // if not then see if all not retrieved are above cache,
	    if (threadsignaled == 0) {
		all_above_cache = 1;
		n = td->list;
		while (n != NULL) {

		    if (DEBUG) {
			printf
			    ("n->url= %s\nn->bytes = %li\nn->cachebytes = %li\nn->play= %i\nn->playlist= %i\nn->mmsstream= %i\n",
			     n->url, n->bytes, n->cachebytes, n->play,
			     n->playlist, n->mmsstream);

		    }


		    if ((n->bytes <= n->cachebytes) && (n->play == 1)) {
			all_above_cache = 0;
			break;
		    }
		    n = n->next;
		}
		// if all above cache signal player
		if (all_above_cache) {
		    if (threadsignaled == 0) {
			if (DEBUG)
			    printf("signalling player (above cache)\n");
			if (autostart) {
			    signalPlayerThread(this);
			    threadsignaled = 1;
			} else {
			    pthread_mutex_lock(&control_mutex);
			    js_state = JS_STATE_READY;
			    pthread_mutex_unlock(&control_mutex);
			}
		    }
		}
	    }
	    // check for streaming media
	    if (threadsignaled == 0) {
		// look for mmsstream
		n = td->list;
		while (n != NULL) {
		    if ((n->mmsstream == 1)
			&& (n->play == 1)) {
			if (threadsignaled == 0) {
			    if (DEBUG)
				printf("signalling player (mmsstream)\n");
			    if (autostart) {
				signalPlayerThread(this);
				threadsignaled = 1;
			    } else {
				if (showcontrols && panel_drawn == 0) {
				    panel_height = 16;
#ifdef GTK_ENABLED
				    g_idle_add(gtkgui_draw, this);
#endif
				    pthread_mutex_lock(&control_mutex);
				    js_state = JS_STATE_READY;
				    pthread_mutex_unlock(&control_mutex);

				}
			    }
			}
			break;
		    }
		    n = n->next;
		}
	    }
	}
	pthread_mutex_unlock(&playlist_mutex);
    }

    if (reason == NPRES_USER_BREAK) {
	if (DEBUG)
	    printf("User cancelled the download\n");
    }

    if (DEBUG >= 2)
	printf
	    ("*******Exiting DestroyStream Callback, state = %d, js_state = %d\n",
	     state, js_state);


    return NPERR_NO_ERROR;
}

void nsPluginInstance::URLNotify(const char *url, NPReason reason,
				 void *notifyData)
{
    Node *n;
    bool isHttpStream = false;


    if (DEBUG)
	printf("URL: %s\nReason %i\n", url, reason);

    if (reason == 1) {
	// check for streaming media
	if (threadsignaled == 0) {
	    // look for mmsstream
	    n = td->list;
	    while (n != NULL) {
		if ((useragent != NULL) && (strlen(useragent) != 0) && (strstr(mimetype, "quicktime") == NULL))	// if we have a user agent 
		    if (strncasecmp(n->url, "http://", 7) == 0)	// and we have an http url
			isHttpStream = true;	// then we might have an http stream
		if ((isHttpStream || n->mmsstream == 1)
		    && (n->play == 1)) {
		    if (threadsignaled == 0) {
			if (DEBUG)
			    printf("signalling player (mmsstream)\n");
			if (autostart) {
			    signalPlayerThread(this);
			    threadsignaled = 1;
			} else {
			    if (showcontrols && panel_drawn == 0) {
				panel_height = 16;
#ifdef GTK_ENABLED
				g_idle_add(gtkgui_draw, this);
#endif
				pthread_mutex_lock(&control_mutex);
				js_state = JS_STATE_READY;
				pthread_mutex_unlock(&control_mutex);

			    }
			}
		    }
		    break;
		}
		n = n->next;
	    }
	}
    }

}

int32 nsPluginInstance::WriteReady(NPStream * stream)
{
    Node *n;
    char *filename;

    if (state == STATE_PLAY_CANCELLED || cancelled == 1)
	return -1;

    if (td == NULL)
	return -1;

    if (strlen(stream->url) >= 1023)
	return -1;

    pthread_mutex_lock(&playlist_mutex);

    if (DEBUG >= 3)
	printf("**WriteReady for %s, state =%d, js_state = %d\n",
	       stream->url, state, js_state);

    n = td->list;
    while (n != NULL) {
	if (DEBUG > 1)
	    printf("WR:\nn->url= %s\nstream->url= %s\n", n->url,
		   stream->url);
	if (strlen(n->url) != 0) {
	    if (URLcmp(n->url, stream->url) == 0) {
		break;
	    } else {
		if (strstr(stream->url, n->url) != NULL) {
		    break;
		}
	    }
	} else {
	    snprintf(n->url, 4096, "%s", stream->url);
	    break;
	}
	n = n->next;
    }

    // redirect case on initial node
    if (n == NULL) {
	if (DEBUG)
	    printf("n == NULL\n");
	if (td->list != NULL) {
	    if (DEBUG)
		printf("td->list != NULL\n");
	    if (((strlen(td->list->fname) == 0)
		 ||
		 (strncmp
		  (getURLFilename(td->list->url),
		   getURLFilename(stream->url), 4096) == 0))
		&& (mmsstream == 0)) {
		if (DEBUG)
		    printf("Redirected initial URL\n");
		n = list;
		snprintf(n->url, 4096, "%s", stream->url);
	    }
	}
    }


    if (n != NULL) {
	// if we have determined that we are not going to play the file
	// stop downloading it, and clean it up during cleanup
	if (n->cancelled == 1) {
	    n->remove = 1;
	    //check locking here
	    NPN_DestroyStream(mInstance, stream, NPRES_DONE);
	}
	// don't download it twice
	if (n->retrieved == 1) {
	    NPN_DestroyStream(mInstance, stream, NPRES_DONE);
	}

	if ((nomediacache == 1) && (stream->end > 16384)) {
	    n->mmsstream = 1;
	    pthread_mutex_unlock(&playlist_mutex);
	    if (threadsignaled == 0) {
		if (autostart) {
		    if (DEBUG)
			printf("signalling player from write ready\n");
		    signalPlayerThread(this);
		    threadsignaled = 1;
		} else {
		    if (showcontrols && panel_drawn == 0) {
			panel_height = 16;
#ifdef GTK_ENABLED
			g_idle_add(gtkgui_draw, this);
#endif
			pthread_mutex_lock(&control_mutex);
			js_state = JS_STATE_READY;
			pthread_mutex_unlock(&control_mutex);

		    }
		}
	    }
	    return -1;
	} else {
	    if (strlen(n->fname) == 0) {
		if (keep_download == 1) {
		    n->remove = 0;
		    filename = getURLFilename(n->url);
		    snprintf(n->fname, 1024, "%s/%s",
			     download_dir, filename);
		    if (filename)
			NPN_MemFree(filename);
		} else {
		    snprintf(n->fname, 1024, "%s",
			     tempnam("/tmp", "mplayerplug-inXXXXXX"));
		    if (strstr(mimetype, "midi") != NULL) {
			strlcat(n->fname, ".mid", 1024);
		    }
		    if (strstr(mimetype, "mp3") != NULL) {
			strlcat(n->fname, ".mp3", 1024);
		    }
		    if (strstr(mimetype, "audio/mpeg") != NULL) {
			strlcat(n->fname, ".mp3", 1024);
		    }
		    if (strstr(mimetype, "audio/x-mod") != NULL) {
			strlcat(n->fname, ".mod", 1024);
		    }
		    if (strstr(mimetype, "flac") != NULL) {
			strlcat(n->fname, ".flac", 1024);
		    }

		}

		if (DEBUG)
		    printf("WR tempname: %s\n", n->fname);

	    }
	    if (n->totalbytes != (int) stream->end)
		n->totalbytes = stream->end;

	    if (n->cachebytes <
		(long int) (stream->end * cache_percent / 100))
		n->cachebytes =
		    (long int) (stream->end * cache_percent / 100);

	    if (n->cachebytes < (cachesize * 1024))
		n->cachebytes = cachesize * 1024;

	    // if the file is really big , cap the amount to be cached
	    if (n->cachebytes > (cachesize * 1024 * 2)
		&& cache_percent != 100)
		n->cachebytes = cachesize * 1024 * 2;

	    pthread_mutex_unlock(&playlist_mutex);
	    return STREAMBUFSIZE;
	}
    } else {
	if (DEBUG)
	    printf("didn't find the node in the playlist\n %s\n",
		   stream->url);
	n = newNode();
	snprintf(n->url, 4096, "%s", stream->url);
	if ((nomediacache == 1) && (stream->end > 16384)) {
	    addToEnd(td->list, n);
	    pthread_mutex_unlock(&playlist_mutex);
	    if (showcontrols && panel_drawn == 0) {
		panel_height = 16;
#ifdef GTK_ENABLED
		g_idle_add(gtkgui_draw, this);
#endif
	    }
	    return -1;
	} else {
	    if (keep_download == 1) {
		n->remove = 0;
		filename = getURLFilename(n->url);
		snprintf(n->fname, 1024, "%s/%s", download_dir, filename);
		if (filename)
		    NPN_MemFree(filename);
	    } else {
		snprintf(n->fname, 1024, "%s",
			 tempnam("/tmp", "mplayerplug-inXXXXXX"));
	    }
	    addToEnd(td->list, n);
	    if (n->totalbytes != (int) stream->end)
		n->totalbytes = stream->end;
	    pthread_mutex_unlock(&playlist_mutex);
	    if (DEBUG >= 3) {
		printf
		    ("**Exiting WriteReady Callback, state = %d, js_state = %d\n",
		     state, js_state);
	    }
	    return STREAMBUFSIZE;
	}
    }

}

int32 nsPluginInstance::Write(NPStream * stream, int32 offset, int32 len,
			      void *buffer)
{
    int ret = 0;
    long int currdownload, maxdownload;
    FILE *fp;
    Node *n;
    char message[1024];
    char *burl;

    if (state == STATE_PLAY_CANCELLED || cancelled == 1)
	return -1;

    if (td == NULL)
	return -1;

    if (DEBUG >= 3)
	printf("****Write Callback %s : %i : %i\n", stream->url,
	       offset, len);

    if (strlen(stream->url) >= 1023)
	return -1;

    pthread_mutex_lock(&playlist_mutex);

    // find the node that matches the URL and open the file name in it.
    currdownload = 0;
    maxdownload = 0;
    for (n = td->list; n != NULL; n = n->next) {
	//if (URLcmp(n->url, stream->url) == 0) continue;
	//if (strstr(stream->url, n->url) == NULL) continue;
	if (n->play == 0)
	    continue;
	if (n->cancelled)
	    continue;
	currdownload += n->bytes;
	maxdownload += n->totalbytes;
    }

    n = td->list;
    if (DEBUG > 2)
	printf("Write - scanning playlist for %s\n", stream->url);
    while (n != NULL) {
	if (DEBUG > 2)
	    printf("Write - current item is %s\n", n->url);
	if (URLcmp(n->url, stream->url) == 0) {
	    break;
	} else {
	    if (strstr(stream->url, n->url) != NULL) {
		break;
	    }
	}
	n = n->next;
    }

    if (n == NULL) {
	pthread_mutex_unlock(&playlist_mutex);
	return -1;
    } else {
	if (n->cancelled == 1 || state == STATE_PLAY_CANCELLED) {
	    if (n->localcache != NULL) {
		fclose(n->localcache);
		n->localcache = NULL;
	    }
	    pthread_mutex_unlock(&playlist_mutex);

	    if (DEBUG) {
		printf
		    ("*******Exiting Write: CANCELLED, state = %d, js_state = %d\n",
		     state, js_state);
	    }

	    return -1;
	}

	if (n->status != STATE_CANCELLED) {
	    // Detect NSV media 
	    snprintf(message, 1024, "%s", (char *) buffer);
	    if (memmem(message, 1024, "ICY 200 OK", 10) != NULL) {
		n->mmsstream = 1;
		n->cancelled = 1;
		n->status = STATE_CANCELLED;
	    } else {
		// normal media so keep going.

		if (n->localcache == NULL) {
		    fp = fopen(n->fname, "w+");
		    n->localcache = fp;
		} else {
		    fp = n->localcache;
		}
		if (fp == NULL) {
		    pthread_mutex_unlock(&playlist_mutex);
		    return -1;
		}
		fseek(fp, offset, SEEK_SET);
		ret = fwrite(buffer, 1, len, fp);
		n->bytes = n->bytes + ret;
		if (maxdownload == 0) {
		    snprintf(message, 1024, _("Buffering %li KB"),
			     (n->bytes / 1024));

		} else {
		    if (currdownload < maxdownload) {
			snprintf(message, 1024,
				 _("Buffering %i%% - %li KB"),
				 (int) (100.0 *
					((double) currdownload /
					 (double) maxdownload)),
				 (n->bytes / 1024));
			percent = ((double) currdownload /
				   (double) maxdownload);

		    } else {
			snprintf(message, 1024,
				 _("Buffering Complete - %li KB"),
				 (n->bytes / 1024));
			percent = 1.0;
			n->retrieved = 1;
		    }
		}

		if (difftime(time(NULL), lastOutput) > 0.5) {

		    if (mode == NP_EMBED && noembed == 0
			&& fullscreen == 0) {
			if (state < STATE_PLAYING) {
#ifdef X_ENABLED
			    DrawUI(widget, this, message,
				   0,
				   (int) ((currdownload /
					   (maxdownload * 1.0)) * 100));
#endif
#ifdef GTK_ENABLED
			    if (GTK_IS_WIDGET(progress_bar)
				&& maxdownload != 0) {
				if (movie_width == 0 && movie_height == 0) {
				    gtk_progress_bar_update(progress_bar,
							    ((currdownload
							      * 1.0) /
							     (maxdownload *
							      1.0)));
				    if (!hidestatus)
					gtk_widget_show(GTK_WIDGET
							(progress_bar));
				    gtk_widget_queue_draw(GTK_WIDGET
							  (progress_bar));
				}
			    }
			    if (status != NULL) {
				gtk_label_set_text(status, message);
				if (!hidestatus)
				    gtk_widget_show(GTK_WIDGET(status));
				gtk_widget_queue_draw(GTK_WIDGET(status));
			    }
#endif
			}
		    } else {
			if (fullscreen == 0) {
#ifdef X_ENABLED
			    DrawUI(widget, this, message,
				   0,
				   (int) ((currdownload /
					   (maxdownload * 1.0)) * 100));
#endif
#ifdef GTK_ENABLED
			    if (GTK_IS_WIDGET(progress_bar)
				&& maxdownload != 0) {
				if (movie_width == 0 && movie_height == 0) {
				    gtk_progress_bar_update(progress_bar,
							    ((currdownload
							      * 1.0) /
							     (maxdownload *
							      1.0)));
				    if (!hidestatus)
					gtk_widget_show(GTK_WIDGET
							(progress_bar));
				    gtk_widget_queue_draw(GTK_WIDGET
							  (progress_bar));
				}
			    }
			    if (status != NULL) {
				if (state < STATE_PLAYING) {
				    gtk_label_set_text(status, message);
				    if (!hidestatus)
					gtk_widget_show(GTK_WIDGET
							(status));
				    gtk_widget_queue_draw(GTK_WIDGET
							  (status));
				}
			    }
#endif
			}
		    }
		    time(&lastOutput);
		}
	    }

	    if (n->status != STATE_DOWNLOADED_ENOUGH) {
		burl = getURLBase(n->url);
		if (burl != NULL) {
		    if (baseurl == NULL) {
			baseurl = burl;
		    } else {
			if (strcmp(baseurl, burl) != 0) {
			    NPN_MemFree(baseurl);
			    baseurl = burl;
			} else {
			    NPN_MemFree(burl);
			}
		    }
		}

		if (isMms(n->url, nomediacache)) {
		    n->mmsstream = 1;
		}
	    }
	    if (n->play == 1) {
		if (n->localcache != NULL && nomediacache == 1) {
		    fclose(n->localcache);
		    n->localcache = NULL;
		}
		if (n->mmsstream == 1) {
		    if (threadsignaled == 0) {
			if (autostart) {
			    if (DEBUG)
				printf("signalling player from write\n");
			    signalPlayerThread(this);
			    threadsignaled = 1;
			} else {
			    pthread_mutex_lock(&control_mutex);
			    js_state = JS_STATE_READY;
			    pthread_mutex_unlock(&control_mutex);
			}
		    }
		} else {
		    if (n->bytes > n->cachebytes) {
			if (threadsignaled == 0) {
			    if (autostart) {
				if (DEBUG)
				    printf
					("signalling player from write\n");
				signalPlayerThread(this);
				threadsignaled = 1;
			    } else {
				if (showcontrols && panel_drawn == 0) {
				    panel_height = 16;
#ifdef GTK_ENABLED
				    g_idle_add(gtkgui_draw, this);
#endif
				    pthread_mutex_lock(&control_mutex);
				    js_state = JS_STATE_READY;
				    pthread_mutex_unlock(&control_mutex);

				}
			    }
			}
		    }
		}
	    }

	}
	n->status = STATE_DOWNLOADED_ENOUGH;
	pthread_mutex_unlock(&playlist_mutex);

	if (DEBUG >= 3) {
	    printf("*******Exiting Write, state = %d, js_state = %d\n",
		   state, js_state);
	}
	return ret;
    }


}

// methods called from nsScriptablePeer

void nsPluginInstance::Play()
{
    Node *n;
    int clearlist = 1;

    if (DEBUG > 1) {
	printf("*****Play Called\n");
    }
    // when js_state == JS_STATE_UNDEFINED, this means that the playlist
    // has been completely processed and the window is clear with the controls
    // visible
    // someone just pressed play so we have to reset the playlist
    if (js_state == JS_STATE_UNDEFINED) {
	//reset the playlist
	if (DEBUG)
	    printf("Play: resetting playlist\n");

	pthread_mutex_lock(&playlist_mutex);	// manipulating the playlist, so lock it

	n = list;
	while (n != NULL) {
	    if (n->played == 0 && n->play == 1) {
		clearlist = 0;
	    }
	    n = n->next;
	}
	if (clearlist) {
	    n = list;
	    while (n != NULL) {
		if (n->played == 1)
		    n->played = 0;	// reset played flag
		n = n->next;
	    }
	}
	pthread_mutex_unlock(&playlist_mutex);	// unlock the playlist
    }
    // the thread is not inited
    // controlwindow is set to 1 when the controls are in one instance and the 
    // output is in another, right now we can't control both instances so when
    // we detect a control window we just don't set it up.
    if (threadsetup == 0 && controlwindow == 0) {
	if (DEBUG > 1)
	    printf("Play: setupplayer\n");
	state = STATE_GETTING_PLAYLIST;
	SetupPlayer(this, NULL);
	if (nomediacache == 1)
	    js_state = JS_STATE_BUFFERING;
    }

    if (threadsetup == 1 && threadlaunched == 0 && controlwindow == 0) {
	if (DEBUG > 1)
	    printf("Play: launching thread\n");
	pthread_mutex_lock(&control_mutex);
	launchPlayerThread(this);
	pthread_mutex_unlock(&control_mutex);	// unlock the control pipe, so that playPlaylist can grab it in a separate thread

    }
    // we are here cause Play 
    // also the thread has not been signalled to start, so signal it
    if (threadsignaled == 0 && threadsetup == 1 && threadlaunched == 1) {
	if (DEBUG > 1)
	    printf("Play: ready to signal\n");
	while (state < STATE_WAITING_FOR_SIGNAL) {
	    if (DEBUG > 1)
		printf("sleeping state = %i\n", state);
	    usleep(100);
	}
	if (DEBUG > 1)
	    printf("Play: signalling thread\n");
	signalPlayerThread(this);
	threadsignaled = 1;
    }
    // normal state, the media is paused or stopped and the thread is active.
    if (paused == 1) {
	if (DEBUG)
	    printf("Play: sending play\n");
	pthread_mutex_lock(&control_mutex);
	sendCommand(this, "pause\n");	// send pause again to unpause
	paused = 0;
	js_state = JS_STATE_PLAYING;
	pthread_mutex_unlock(&control_mutex);
    }
#ifdef GTK_ENABLED
    play_callback(NULL, NULL, this);
#endif

    if (DEBUG > 1) {
	printf("***********Exiting Play*************\n");
    }

}

void nsPluginInstance::PlayAt(double counter)
{
    starttime = (long int) counter;
    Play();
}


void nsPluginInstance::Pause()
{
    if (threadlaunched == 0)
	return;
    pthread_mutex_lock(&control_mutex);
    if (paused == 0) {
	if (DEBUG)
	    printf("sending pause\n");
	sendCommand(this, "pause\n");
#ifdef GTK_ENABLED
	pause_callback(NULL, NULL, this);
#endif
	paused = 1;
	js_state = JS_STATE_PAUSED;
    }
    pthread_mutex_unlock(&control_mutex);
}

void nsPluginInstance::Stop()
{
    Node *n;

    if (threadlaunched == 0)
	return;
    pthread_mutex_lock(&control_mutex);
    if (DEBUG)
	printf("sending stop\n");
    if (paused == 1)
	sendCommand(this, "pause\n");
    if (mmsstream == 0) {
	sendCommand(this, "seek 0 2\npause\n");
    } else {
	pthread_mutex_trylock(&playlist_mutex);	// manipulating the playlist, so lock it
	n = list;
	while (n != NULL) {
	    if (n->played)
		n->played = 0;	// reset playlist
	    n = n->next;
	}
	pthread_mutex_unlock(&playlist_mutex);	// unlock the playlist
	sendCommand(this, "quit\n");
    }
#ifdef GTK_ENABLED
    stop_callback(NULL, NULL, this);
#endif
    paused = 1;
    js_state = JS_STATE_STOPPED;
    pthread_mutex_unlock(&control_mutex);
}

void nsPluginInstance::Quit()
{
    if (threadlaunched == 0)
	return;
    pthread_mutex_lock(&control_mutex);
    if (DEBUG)
	printf("sending quit\n");
    if (paused == 1)
	sendCommand(this, "pause\n");
    sendCommand(this, "quit\n");
    paused = 0;
    js_state = JS_STATE_UNDEFINED;
    pthread_mutex_unlock(&control_mutex);
    killmplayer(this);
}

void nsPluginInstance::FastForward()
{
    int pre_state;

    if (threadlaunched == 0)
	return;

    if ((js_state == JS_STATE_PLAYING) || (js_state == JS_STATE_PAUSED)) {
	// do nothing, since these are ok states to FF in
    } else {
	return;
    }

    pthread_mutex_lock(&control_mutex);
    pre_state = js_state;
    js_state = JS_STATE_SCANFORWARD;
    if (DEBUG)
	printf("sending FastForward\n");
    if (paused == 1)
	sendCommand(this, "pause\n");
    sendCommand(this, "seek +10 0\n");
    if (paused == 1)
	sendCommand(this, "pause\n");
    js_state = pre_state;
    pthread_mutex_unlock(&control_mutex);
}

void nsPluginInstance::FastReverse()
{
    int pre_state;

    if (threadlaunched == 0)
	return;

    if ((js_state == JS_STATE_PLAYING) || (js_state == JS_STATE_PAUSED)) {
	// do nothing, since these are ok states to REW in
    } else {
	return;
    }

    pthread_mutex_lock(&control_mutex);
    pre_state = js_state;
    js_state = JS_STATE_SCANREVERSE;
    if (DEBUG)
	printf("sending FastReverse\n");
    if (paused == 1)
	sendCommand(this, "pause\n");
    sendCommand(this, "seek -10 0\n");
    if (paused == 1)
	sendCommand(this, "pause\n");
    js_state = pre_state;
    pthread_mutex_unlock(&control_mutex);
}

void nsPluginInstance::Seek(double counter)
{
    char command[32];

    if (threadlaunched == 0)
	return;
    pthread_mutex_lock(&control_mutex);
    if (paused == 1)
	sendCommand(this, "pause\n");
    snprintf(command, 32, "seek %5.0f 2\n", counter);
    sendCommand(this, command);
    if (paused == 1)
	sendCommand(this, "pause\n");
    pthread_mutex_unlock(&control_mutex);

}

void nsPluginInstance::VolumeUp()
{
    char command[32];

    if (threadlaunched == 0)
	return;
    pthread_mutex_lock(&control_mutex);
    snprintf(command, 32, "volume +1\n");
    sendCommand(this, command);
    sendCommand(this, "get_property volume\n");
    pthread_mutex_unlock(&control_mutex);

}

void nsPluginInstance::VolumeDown()
{
    char command[32];

    if (threadlaunched == 0)
	return;
    pthread_mutex_lock(&control_mutex);
    snprintf(command, 32, "volume -1\n");
    sendCommand(this, command);
    sendCommand(this, "get_property volume\n");
    pthread_mutex_unlock(&control_mutex);

}

void nsPluginInstance::GetVolume(double *_retval)
{
    *_retval = (double) volume;

}

void nsPluginInstance::SetVolume(double value)
{
    volume = (int) value;
    char command[32];

    if (threadlaunched == 0)
	return;
    pthread_mutex_lock(&control_mutex);
    snprintf(command, 32, "volume %i 1\n", volume);
    sendCommand(this, command);
    pthread_mutex_unlock(&control_mutex);
}

void nsPluginInstance::GetPlayState(PRInt32 * playstate)
{
    pthread_mutex_lock(&control_mutex);
    *playstate = js_state;
    pthread_mutex_unlock(&control_mutex);
}

void nsPluginInstance::GetTime(double *_retval)
{
    if (js_state == JS_STATE_STOPPED) {
	mediaTime = 0.0;
    }
    *_retval = (double) mediaTime;
}

void nsPluginInstance::GetDuration(double *_retval)
{
    *_retval = (double) mediaLength;
}

void nsPluginInstance::GetPercent(double *_retval)
{
    *_retval = (double) mediaPercent;
}

void nsPluginInstance::GetFilename(char **filename)
{
    if (DEBUG >= 2)
	printf("***************** GetFilename called %s\n", *filename);
    if (href != NULL)
	*filename = strdup(href);
    if (fname != NULL)
	*filename = strdup(fname);
    if (url != NULL)
	*filename = strdup(url);
    if (DEBUG >= 2)
	printf("***************** GetFilename exited %s\n", *filename);
}

void nsPluginInstance::SetFilename(const char *filename)
{
    char localurl[1024];
    char *media_save;
    if (DEBUG >= 2)
	printf("***************** SetFilename called %s\n", filename);
/*
    killmplayer(this);
    // reset some vars
    paused = 0;
    threadsetup = 0;
    threadsignaled = 0;
    // reset the list
*/
    if (filename == NULL)
	return;

    if (DEBUG)
	printf("threadsetup = %i, threadsignaled = %i\n", threadsetup,
	       threadsignaled);

    if (threadsetup == 1 && threadsignaled == 1) {
	media_save = NULL;

	if (mediaCompleteCallback != NULL) {
	    media_save = mediaCompleteCallback;
	    mediaCompleteCallback = NULL;
	}
	Quit();
	while (threadsetup != 0) {
	    if (DEBUG)
		printf("waiting to quit\n");
	    usleep(100);
	}

	if (media_save != NULL) {
	    mediaCompleteCallback = media_save;
	    media_save = NULL;
	}
    }

    pthread_mutex_lock(&playlist_mutex);
    if (baseurl != NULL) {
	free(baseurl);
	baseurl = NULL;
    }
    if (hostname != NULL) {
	free(hostname);
	hostname = NULL;
    }

    deleteList(list);
    list = newNode();

    td->list = NULL;

    // need to convert to Fully Qualified URL here
    fullyQualifyURL(this, (char *) filename, localurl);

    if (href != NULL) {
	free(href);
	href = NULL;
    }

    if (fname != NULL) {
	free(fname);
	fname = NULL;
    }

    if (url != NULL) {
	free(url);
	url = NULL;
    }

    pthread_mutex_unlock(&playlist_mutex);

    url = strdup(localurl);
    cancelled = 0;
    if (DEBUG)
	printf("SetFilename getting %s\n", localurl);
    if (!isMms(localurl, nomediacache))
	NPN_GetURL(mInstance, localurl, NULL);


    if (DEBUG >= 2) {
	printf("**********SetFilename Exit***************\n");
    }
}

void nsPluginInstance::GetShowControls(PRBool * _retval)
{
    *_retval = (PRBool) controlsvisible;

}

void nsPluginInstance::SetShowControls(PRBool value)
{
#ifdef GTK_ENABLED
    if (value) {
	if (panel_drawn == 0) {
	    gtkgui_draw(this);
	} else {
	    if (play_event_box != NULL)
		gtk_widget_show(play_event_box);
	    if (pause_event_box != NULL)
		gtk_widget_show(pause_event_box);
	    if (stop_event_box != NULL)
		gtk_widget_show(stop_event_box);
	    if (ff_event_box != NULL)
		gtk_widget_show(ff_event_box);
	    if (rew_event_box != NULL)
		gtk_widget_show(rew_event_box);
	    if (mediaprogress_bar != NULL && mediaPercent > 0)
		gtk_widget_show(GTK_WIDGET(mediaprogress_bar));
	    if (vol_event_box != NULL)
		gtk_widget_show(GTK_WIDGET(vol_event_box));
	    if (fs_event_box != NULL)
		gtk_widget_show(GTK_WIDGET(fs_event_box));
	}
	controlsvisible = 1;

    } else {			// hide everything

	if (panel_drawn == 0) {
	    // do nothing
	} else {
	    if (play_event_box != NULL)
		gtk_widget_hide(play_event_box);
	    if (pause_event_box != NULL)
		gtk_widget_hide(pause_event_box);
	    if (stop_event_box != NULL)
		gtk_widget_hide(stop_event_box);
	    if (ff_event_box != NULL)
		gtk_widget_hide(ff_event_box);
	    if (rew_event_box != NULL)
		gtk_widget_hide(rew_event_box);
	    if (mediaprogress_bar != NULL)
		gtk_widget_hide(GTK_WIDGET(mediaprogress_bar));
	    if (vol_event_box != NULL)
		gtk_widget_hide(GTK_WIDGET(vol_event_box));
	    if (fs_event_box != NULL)
		gtk_widget_hide(GTK_WIDGET(fs_event_box));
	}
	controlsvisible = 0;
    }

#endif
#ifdef GTK2_ENABLED
    gtk_check_menu_item_set_active(GTK_CHECK_MENU_ITEM
				   (menuitem_showcontrols),
				   controlsvisible);
#endif
}

void nsPluginInstance::GetFullscreen(PRBool * _retval)
{
    *_retval = (PRBool) fullscreen;

}

void nsPluginInstance::SetFullscreen(PRBool value)
{

#ifdef GTK2_ENABLED
    GdkScreen *screen;
    GdkRectangle rect;
    gint wx, wy;
    gint wwidth, wheight;
#endif

#ifdef GTK_ENABLED
    int multiplier, height = 0, width;
    int x, y, disp_x, pos_x, disp_y, pos_y, button_top;
    GdkColormap *colormap;
    GdkColor black;
#endif
    int win_height, win_width;

    if (threadlaunched == 0 && value == 1)
	return;

    if (DEBUG)
	printf("in SetFullscreen with %i\nfullscreen = %i\n", value,
	       fullscreen);
#ifdef GTK_ENABLED
    black.red = 0;
    black.green = 0;
    black.blue = 0;
    if (drawing_area != NULL) {
	colormap = gdk_window_get_colormap(drawing_area->window);
	gdk_color_alloc(colormap, &black);
    }
#endif
    if (mode == NP_EMBED) {
	win_height = embed_height;
	win_width = embed_width;
    } else {
	win_height = window_height;
	win_width = window_width;
    }

    if (targetplayer == 1) {
	win_height = movie_height;
	win_width = movie_width;
    }

    if (DEBUG)
	printf("height = %i and width = %i\n", win_height, win_width);

    if (win_height == 0 || win_width == 0 || hidden == 1)
	return;

    if (fullscreen) {
	if (value) {
	    // do nothing
	    if (DEBUG > 1)
		printf("SetFullscreen doing nothing\n");
#ifdef GTK2_ENABLED

	    screen = gtk_window_get_screen(GTK_WINDOW(gtkwidget));
	    gdk_screen_get_monitor_geometry(screen,
					    gdk_screen_get_monitor_at_window
					    (screen, gtkwidget->window),
					    &rect);
	    //gtk_widget_reparent(gtkwidget, fs_window);
	    gtk_window_get_size(GTK_WINDOW(gtkwidget), &wwidth, &wheight);
	    if ((wwidth != rect.width) && (wheight != rect.height)) {
		if (targetplayer == 0) {
		    if (dontparent == 0) {
			XReparentWindow(GDK_WINDOW_XDISPLAY
					(gtkwidget->window),
					GDK_WINDOW_XWINDOW(gtkwidget->
							   window),
					GDK_WINDOW_XWINDOW(fs_window->
							   window), 0, 0);

			gtk_widget_map(gtkwidget);

			gtk_window_resize(GTK_WINDOW(gtkwidget),
					  rect.width, rect.height);
			if (DEBUG)
			    printf("gtkwidget resized to %i x %i\n",
				   rect.width, rect.height);
		    }
		}
	    } else {
		if (DEBUG)
		    printf("gtkwidget is %i x %i\n", wwidth, wheight);
	    }
	    x = rect.width;
	    y = rect.height;
	    disp_y = y;
	    disp_x = x;
	    if (movie_height != 0 && movie_width != 0) {
		disp_y = ((long int) (x * movie_height)) / movie_width;
		disp_x = ((long int) (y * movie_width)) / movie_height;
	    } else {
		if (win_width != 0) {
		    disp_y = ((long int) (x * win_height)) / win_width;
		    disp_x = ((long int) (y * win_width)) / win_height;
		}
	    }
	    pos_y = (y - disp_y) / 2;
	    pos_x = (x - disp_x) / 2;
	    if (drawing_area != NULL) {
		if (disp_y <= y) {
		    gtk_widget_set_usize(drawing_area, x, disp_y);
		    gtk_fixed_move(GTK_FIXED(fixed_container),
				   GTK_WIDGET(drawing_area), 0, pos_y);
		    if (DEBUG)
			printf("drawing_area is %i x %i\n", x, disp_y);
		} else {
		    gtk_widget_set_usize(drawing_area, disp_x, y);
		    gtk_fixed_move(GTK_FIXED(fixed_container),
				   GTK_WIDGET(drawing_area), pos_x, 0);
		    if (DEBUG)
			printf("drawing_area is %i x %i\n", disp_x, y);
		}
	    }
#endif
	    fullscreen = 1;
	} else {
#ifdef GTK_ENABLED
	    if (DEBUG > 1)
		printf("SetFullscreen returning to original size\n");
#ifdef GTK2_ENABLED
	    if (fs_window != NULL) {
		gtk_window_unfullscreen(GTK_WINDOW(fs_window));
		if (targetplayer == 0) {
		    if (dontparent == 0) {
			XReparentWindow(GDK_WINDOW_XDISPLAY
					(gtkwidget->window),
					GDK_WINDOW_XWINDOW(gtkwidget->
							   window), window,
					0, 0);
			gtk_widget_map(gtkwidget);
		    }
		}
		gtk_widget_destroy(fs_window);
		fs_window = NULL;
	    } else {
		gtk_window_unfullscreen(GTK_WINDOW(gtkwidget));
	    }
	    if (targetplayer == 0) {
		gtk_window_move(GTK_WINDOW(gtkwidget), 0, 0);
		gtk_window_resize(GTK_WINDOW(gtkwidget), win_width,
				  win_height);
	    }
#endif
#ifdef GTK1_ENABLED
	    if (fs_window != NULL) {
		XReparentWindow(GDK_WINDOW_XDISPLAY(gtkwidget->window),
				GDK_WINDOW_XWINDOW(gtkwidget->window),
				window, 0, 0);
		XMapWindow(GDK_WINDOW_XDISPLAY(gtkwidget->window),
			   GDK_WINDOW_XWINDOW(gtkwidget->window));
		gtk_widget_destroy(fs_window);
		fs_window = NULL;
	    }

	    XResizeWindow(GDK_WINDOW_XDISPLAY(gtkwidget->window),
			  GDK_WINDOW_XWINDOW(gtkwidget->window),
			  win_width, win_height);
#endif
	    if (panel_drawn == 1) {
		height = 16;
		width = 21;

		if (targetplayer == 0) {
		    button_top = win_height - height;
		} else {
		    button_top = win_height;
		}

		multiplier = 0;

		if (mmsstream == 0 && win_width > 126) {
		    gtk_fixed_move(GTK_FIXED(fixed_container),
				   GTK_WIDGET(rew_event_box),
				   width * multiplier++, button_top);
		}
		gtk_fixed_move(GTK_FIXED(fixed_container),
			       GTK_WIDGET(play_event_box),
			       width * multiplier++, button_top);
		if (win_width > 126)
		    gtk_fixed_move(GTK_FIXED(fixed_container),
				   GTK_WIDGET(pause_event_box),
				   width * multiplier++, button_top);
		gtk_fixed_move(GTK_FIXED(fixed_container),
			       GTK_WIDGET(stop_event_box),
			       width * multiplier++, button_top);
		if (mmsstream == 0 && win_width > 126) {
		    gtk_fixed_move(GTK_FIXED(fixed_container),
				   GTK_WIDGET(ff_event_box),
				   width * multiplier++, button_top);
		}

		if (mediaprogress_bar != NULL) {
		    gtk_fixed_move(GTK_FIXED(fixed_container),
				   GTK_WIDGET(mediaprogress_bar),
				   (width * multiplier + 10),
				   button_top + 2);
		    gtk_widget_set_usize(GTK_WIDGET(mediaprogress_bar),
					 win_width -
					 (width * (multiplier + 2) + 20),
					 height - 4);
		}
		if (win_width > 126) {
		    gtk_fixed_move(GTK_FIXED(fixed_container),
				   GTK_WIDGET(vol_event_box),
				   (win_width - (width * 2)), button_top);
		    gtk_fixed_move(GTK_FIXED(fixed_container),
				   GTK_WIDGET(fs_event_box),
				   (win_width - width), button_top);
		}

	    }

	    if (image != NULL && showlogo)
		gtk_widget_show(GTK_WIDGET(image));

	    if (embed_height > 125 || mode == NP_FULL) {
		gtk_widget_set_usize(GTK_WIDGET(status), embed_width - 20,
				     embed_height - 125);
		gtk_fixed_move(GTK_FIXED(fixed_container),
			       GTK_WIDGET(status), 10, 60);
	    } else {
		gtk_widget_set_usize(GTK_WIDGET(status), embed_width - 20,
				     embed_height - 20);
		gtk_fixed_move(GTK_FIXED(fixed_container),
			       GTK_WIDGET(status), 10, 10);
	    }

	    if (mode == NP_EMBED) {
		if (movie_height != 0 && movie_width != 0) {
		    if (drawing_area != NULL)
			gtk_widget_set_usize(drawing_area,
					     movie_width, movie_height);
		} else {
		    if (drawing_area != NULL)
			gtk_widget_set_usize(drawing_area,
					     embed_width, embed_height);
		}
		if (drawing_area != NULL)
		    gtk_fixed_move(GTK_FIXED(fixed_container),
				   GTK_WIDGET(drawing_area), 0, 0);
	    } else {
		if (drawing_area != NULL) {
		    gtk_fixed_move(GTK_FIXED(fixed_container),
				   GTK_WIDGET(drawing_area), 10, 100);
		    if (movie_height != 0 && movie_width != 0) {
			gtk_widget_set_usize(drawing_area,
					     movie_width, movie_height);
			if ((movie_width + 10 >= window_width)
			    || (movie_height + 100 >= window_height))
			    gtk_fixed_put(GTK_FIXED(fixed_container),
					  drawing_area, 0, 0);
		    } else {
			gtk_widget_set_usize(drawing_area,
					     window_width - 20,
					     window_height - 200);
		    }
		}
	    }
#ifdef GTK2_ENABLED
	    gtk_widget_modify_bg(fixed_event_box, GTK_STATE_NORMAL,
				 &(gtk_widget_get_style(image)->
				   bg[GTK_STATE_NORMAL]));
	    if (drawing_area != NULL)
		gtk_widget_modify_bg(drawing_area, GTK_STATE_NORMAL,
				     &(gtk_widget_get_style(image)->
				       bg[GTK_STATE_NORMAL]));
#endif

#endif
	    fullscreen = 0;
	}
    } else {
	if (value) {
	    if (DEBUG > 1)
		printf("SetFullscreen setting fullscreen\n");
#ifdef GTK_ENABLED
#ifdef GTK2_ENABLED
	    if (targetplayer == 0) {
		fs_window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
		gtk_widget_add_events(fs_window, GDK_BUTTON_PRESS_MASK);
		gtk_widget_add_events(fs_window, GDK_BUTTON_RELEASE_MASK);
		gtk_widget_add_events(fs_window, GDK_ENTER_NOTIFY_MASK);
		gtk_widget_add_events(fs_window, GDK_LEAVE_NOTIFY_MASK);
		gtk_widget_add_events(fs_window, GDK_KEY_PRESS_MASK);

		g_signal_connect(G_OBJECT(fs_window),
				 "key_press_event",
				 G_CALLBACK(keyboard_callback), this);
		g_signal_connect_swapped(G_OBJECT(fs_window),
					 "button_press_event",
					 G_CALLBACK(popup_handler),
					 GTK_OBJECT(popup_menu));

		screen = gtk_window_get_screen(GTK_WINDOW(gtkwidget));
		gtk_window_set_screen(GTK_WINDOW(fs_window), screen);
		gtk_window_set_title(GTK_WINDOW(fs_window),
				     "mplayerplug-in fullscreen");
		gdk_screen_get_monitor_geometry(screen,
						gdk_screen_get_monitor_at_window
						(screen,
						 gtkwidget->window),
						&rect);

		x = rect.width;
		y = rect.height;
		gtk_widget_realize(fs_window);

		gdk_window_get_root_origin(gtkwidget->window, &wx, &wy);
		gtk_window_move(GTK_WINDOW(fs_window), wx, wy);

		gtk_widget_show(fs_window);
		gtk_window_fullscreen(GTK_WINDOW(fs_window));
		//gtk_widget_reparent(gtkwidget, fs_window);
		XReparentWindow(GDK_WINDOW_XDISPLAY(gtkwidget->window),
				GDK_WINDOW_XWINDOW(gtkwidget->window),
				GDK_WINDOW_XWINDOW(fs_window->window), 0,
				0);

		gtk_widget_map(gtkwidget);
		gtk_window_resize(GTK_WINDOW(gtkwidget), rect.width,
				  rect.height);
	    } else {
		screen = gtk_window_get_screen(GTK_WINDOW(gtkwidget));
		gtk_window_set_title(GTK_WINDOW(gtkwidget),
				     "mplayerplug-in fullscreen");
		gdk_screen_get_monitor_geometry(screen,
						gdk_screen_get_monitor_at_window
						(screen,
						 gtkwidget->window),
						&rect);

		x = rect.width;
		y = rect.height;
		gtk_window_fullscreen(GTK_WINDOW(gtkwidget));
	    }
#endif

#ifdef GTK1_ENABLED
	    fs_window = gtk_window_new(GTK_WINDOW_POPUP);
	    gtk_widget_add_events(fs_window, GDK_BUTTON_PRESS_MASK);
	    gtk_widget_add_events(fs_window, GDK_BUTTON_RELEASE_MASK);
	    gtk_widget_add_events(fs_window, GDK_ENTER_NOTIFY_MASK);
	    gtk_widget_add_events(fs_window, GDK_LEAVE_NOTIFY_MASK);
	    gtk_widget_add_events(fs_window, GDK_KEY_PRESS_MASK);
	    gtk_widget_realize(fs_window);
	    gtk_signal_connect(GTK_OBJECT(fs_window),
			       "key-press-event",
			       GTK_SIGNAL_FUNC(keyboard_callback), this);
	    gtk_signal_connect_object(GTK_OBJECT(fs_window),
				      "button_press_event",
				      GTK_SIGNAL_FUNC(popup_handler),
				      GTK_OBJECT(popup_menu));

	    gtk_widget_show(fs_window);
	    XReparentWindow(GDK_WINDOW_XDISPLAY(fs_window->window),
			    GDK_WINDOW_XWINDOW(fs_window->window),
			    GDK_ROOT_WINDOW(), 0, 0);
	    XReparentWindow(GDK_WINDOW_XDISPLAY(gtkwidget->window),
			    GDK_WINDOW_XWINDOW(gtkwidget->window),
			    GDK_WINDOW_XWINDOW(fs_window->window), 0, 0);
	    XMapWindow(GDK_WINDOW_XDISPLAY(gtkwidget->window),
		       GDK_WINDOW_XWINDOW(gtkwidget->window));
	    x = gdk_screen_width();
	    y = gdk_screen_height();
	    XResizeWindow(GDK_WINDOW_XDISPLAY(fs_window->window),
			  GDK_WINDOW_XWINDOW(fs_window->window), x, y);
	    XResizeWindow(GDK_WINDOW_XDISPLAY(gtkwidget->window),
			  GDK_WINDOW_XWINDOW(gtkwidget->window), x, y);
#endif


	    if (panel_drawn == 1) {
		height = 16;
		width = 21;


		multiplier = 0;

		if (mmsstream == 0) {
		    gtk_fixed_move(GTK_FIXED(fixed_container),
				   GTK_WIDGET(rew_event_box),
				   width * multiplier++, y - height);
		}
		gtk_fixed_move(GTK_FIXED(fixed_container),
			       GTK_WIDGET(play_event_box),
			       width * multiplier++, y - height);
		gtk_fixed_move(GTK_FIXED(fixed_container),
			       GTK_WIDGET(pause_event_box),
			       width * multiplier++, y - height);
		gtk_fixed_move(GTK_FIXED(fixed_container),
			       GTK_WIDGET(stop_event_box),
			       width * multiplier++, y - height);
		if (mmsstream == 0) {
		    gtk_fixed_move(GTK_FIXED(fixed_container),
				   GTK_WIDGET(ff_event_box),
				   width * multiplier++, y - height);
		}
		if (mediaprogress_bar != NULL) {
		    gtk_fixed_move(GTK_FIXED(fixed_container),
				   GTK_WIDGET(mediaprogress_bar),
				   (width * multiplier + 10),
				   y - height + 2);

		    gtk_widget_set_usize(GTK_WIDGET(mediaprogress_bar),
					 x - (width * (multiplier + 2) +
					      20), height - 4);
		}
		gtk_fixed_move(GTK_FIXED(fixed_container),
			       GTK_WIDGET(vol_event_box),
			       (x - (width * 2)), y - height);
		gtk_fixed_move(GTK_FIXED(fixed_container),
			       GTK_WIDGET(fs_event_box),
			       (x - width), y - height);

	    }

	    disp_y = y;
	    disp_x = x;
	    if (movie_height != 0 && movie_width != 0) {
		disp_y = ((long int) (x * movie_height)) / movie_width;
		disp_x = ((long int) (y * movie_width)) / movie_height;
	    } else {
		if (win_width != 0) {
		    disp_y = ((long int) (x * win_height)) / win_width;
		    disp_x = ((long int) (y * win_width)) / win_height;
		}
	    }
	    pos_y = (y - disp_y) / 2;
	    pos_x = (x - disp_x) / 2;
	    if (drawing_area != NULL) {
		if (disp_y <= y) {
		    gtk_widget_set_usize(drawing_area, x, disp_y);
		    gtk_fixed_move(GTK_FIXED(fixed_container),
				   GTK_WIDGET(drawing_area), 0, pos_y);
		} else {
		    gtk_widget_set_usize(drawing_area, disp_x, y);
		    gtk_fixed_move(GTK_FIXED(fixed_container),
				   GTK_WIDGET(drawing_area), pos_x, 0);
		}
	    }
	    if (DEBUG > 1)
		printf
		    ("x=%i, y=%i, movie_width=%i, movie_height=%i, disp_y=%i, pos_y=%i\n",
		     x, y, movie_width, movie_height, disp_y, pos_y);

	    if (image != NULL)
		gtk_widget_hide(GTK_WIDGET(image));

	    if (progress_bar != NULL)
		gtk_widget_hide(GTK_WIDGET(progress_bar));

	    gtk_widget_set_usize(GTK_WIDGET(status), x - 20, 30);
	    gtk_fixed_move(GTK_FIXED(fixed_container), GTK_WIDGET(status),
			   10, 10);
#ifdef GTK2_ENABLED
	    gtk_widget_modify_bg(fixed_event_box, GTK_STATE_NORMAL,
				 &black);
	    if (drawing_area != NULL)
		gtk_widget_modify_bg(drawing_area, GTK_STATE_NORMAL,
				     &black);
#endif
#ifdef GTK_ENABLED
	    gdk_window_set_background(gtkwidget->window, &black);
	    gdk_window_clear(gtkwidget->window);
	    if (targetplayer == 0) {
		gdk_window_set_background(fs_window->window, &black);
		gdk_window_clear(fs_window->window);
	    }
	    if (drawing_area != NULL) {
		gdk_window_set_background(drawing_area->window, &black);
		gdk_window_clear(drawing_area->window);
	    }
#endif
#endif
	    fullscreen = 1;
	} else {
	    // do nothing
	    if (DEBUG > 1)
		printf("SetFullscreen doing nothing\n");
	    fullscreen = 0;
	}
    }
#ifdef GTK_ENABLED
    gtk_check_menu_item_set_active(GTK_CHECK_MENU_ITEM
				   (menuitem_fullscreen), fullscreen);
#endif

}

void nsPluginInstance::GetShowlogo(PRBool * _retval)
{
    *_retval = (PRBool) showlogo;

}

void nsPluginInstance::SetShowlogo(PRBool value)
{
    showlogo = (int) value;
#ifdef GTK_ENABLED
    if (image != NULL)
	if (showlogo)
	    gtk_widget_show(GTK_WIDGET(image));
	else
	    gtk_widget_hide(GTK_WIDGET(image));

#endif
}

void nsPluginInstance::GetPlaying(PRBool * _retval)
{
    int i;

    if (js_state > 0 && js_state < 7) {
	i = 1;
    } else {
	i = 0;
    }

    *_retval = (PRBool) i;

}

void nsPluginInstance::GetAutoPlay(PRBool * _retval)
{
    *_retval = (PRBool) autostart;

}

void nsPluginInstance::SetAutoPlay(PRBool value)
{
    autostart = (int) value;
}

void nsPluginInstance::GetLoop(PRBool * _retval)
{
    *_retval = (PRBool) loop;

}

void nsPluginInstance::SetLoop(PRBool value)
{
    loop = (int) value;
}

void nsPluginInstance::GetMIMEType(char **_retval)
{
    *_retval = strdup(mimetype);
}

void nsPluginInstance::PlaylistAppend(const char *item)
{
    Node *tail;
    char localurl[1024];

    pthread_mutex_lock(&playlist_mutex);
    tail = list;
    if (tail != NULL) {
	while (tail->next != NULL)
	    tail = tail->next;
    }
    fullyQualifyURL(this, (char *) item, localurl);
    addToList(this, localurl, tail, -1, 0, -1, 0);
    pthread_mutex_unlock(&playlist_mutex);
}

void nsPluginInstance::PlaylistClear(PRBool * _retval)
{
    // can only clear the list when nothing is running
    if (js_state != JS_STATE_UNDEFINED) {
	*_retval = (PRBool) FALSE;
    } else {
	pthread_mutex_lock(&playlist_mutex);	// manipulating the playlist, so lock it
	deleteList(list);
	pthread_mutex_unlock(&playlist_mutex);
	*_retval = (PRBool) TRUE;
    }
}

void nsPluginInstance::SetOnClick(const char *event)
{
    if(mouseClickCallback != NULL) {
            NPN_MemFree(mouseClickCallback);
    }
    mouseClickCallback = (char *)NPN_MemAlloc(strlen(event) +
					  12);	 
    if (strncasecmp(event, "javascript:", 11) == 0) {
        snprintf(mouseClickCallback, strlen(event),"%s", event);
    } else {
        snprintf(mouseClickCallback, strlen(event)+12,"javascript:%s", event);
    }
}

// ==============================
// ! Scriptability related code !
// ==============================
//
// here the plugin is asked by Mozilla to tell if it is scriptable
// we should return a valid interface id and a pointer to
// nsScriptablePeer interface which we should have implemented
// and which should be defined in the corressponding *.xpt file
// in the bin/components folder
NPError nsPluginInstance::GetValue(NPPVariable aVariable, void *aValue)
{
    NPError rv = NPERR_NO_ERROR;

    switch (aVariable) {
    case NPPVpluginScriptableInstance:{
	    // addref happens in getter, so we don't addref here
	    nsIScriptableMplayerPlugin *scriptablePeer =
		getScriptablePeer();
	    if (scriptablePeer) {
		*(nsISupports **) aValue = scriptablePeer;
	    } else
		rv = NPERR_OUT_OF_MEMORY_ERROR;
	}
	break;

    case NPPVpluginScriptableIID:{
	    static nsIID scriptableIID = NS_ISCRIPTABLEMPLAYERPLUGIN_IID;
	    nsIID *ptr = (nsIID *) NPN_MemAlloc(sizeof(nsIID));
	    if (ptr) {
		*ptr = scriptableIID;
		*(nsIID **) aValue = ptr;
	    } else
		rv = NPERR_OUT_OF_MEMORY_ERROR;
	}
	break;

#ifdef GTK_ENABLED
    case NPPVpluginNeedsXEmbed:{
	    *(PRBool *) aValue = PR_TRUE;
	    rv = NPERR_NO_ERROR;
	}
	break;
#endif

    default:
	break;
    }

    return rv;
}

// ==============================
// ! Scriptability related code !
// ==============================
//
// this method will return the scriptable object (and create it if necessary)
nsScriptablePeer *nsPluginInstance::getScriptablePeer()
{
    if (!mScriptablePeer) {
	mScriptablePeer = new nsScriptablePeer(this);
	if (!mScriptablePeer)
	    return NULL;

	NS_ADDREF(mScriptablePeer);
    }
    // add reference for the caller requesting the object
    NS_ADDREF(mScriptablePeer);
    return mScriptablePeer;
}

nsControlsScriptablePeer *nsPluginInstance::getControlsScriptablePeer()
{
    if (!mControlsScriptablePeer) {
	mControlsScriptablePeer = new nsControlsScriptablePeer(this);
	if (!mControlsScriptablePeer)
	    return NULL;

	NS_ADDREF(mControlsScriptablePeer);
    }
    // add reference for the caller requesting the object
    NS_ADDREF(mControlsScriptablePeer);
    return mControlsScriptablePeer;
}
