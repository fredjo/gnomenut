/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
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

#ifndef __PLUGIN_H__
#define __PLUGIN_H__
#ifdef BSD
#include <cmath>
#endif
#define _XOPEN_SOURCE 500

#ifndef BSD
#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif
#endif
#ifdef GECKOSDK_ENABLED
#include "mozilla-config.h"
#endif
#include "pluginbase.h"
#include "nsScriptablePeer.h"
#include "plugin-setup.h"
#include <pthread.h>
#include <sys/types.h>
#include <signal.h>
#include <string.h>
#include <time.h>
#ifdef HAVE_GETTEXT
# include <libintl.h>
# define _(x) (gettext(x))
#else
# define _(x) (x)
#endif

class nsPluginInstance:public nsPluginInstanceBase {
  public:
    nsPluginInstance(NPP aInstance);
    virtual ~ nsPluginInstance();

    NPBool init(NPWindow * aWindow);
    void shut();
    void shutdown();
    NPBool isInitialized();
    NPError NewStream(NPMIMEType type, NPStream * stream, NPBool seekable,
		      uint16 * stype);
    NPError SetWindow(NPWindow * aWindow);
    NPError DestroyStream(NPStream * stream, NPError reason);
    void URLNotify(const char *url, NPReason reason, void *notifyData);
    int32 WriteReady(NPStream * stream);
    int32 Write(NPStream * stream, int32 offset, int32 len, void *buffer);

    // JS Methods
    void Play();
    void PlayAt(double counter);
    void Pause();
    void Stop();
    void Quit();
    void FastForward();
    void FastReverse();
    void Seek(double counter);
	void VolumeUp();
	void VolumeDown();
    void SetVolume(double value);
    void GetVolume(double * _retval);
    void GetPlayState(PRInt32 * playstate);
    void GetTime(double *_retval);
    void GetDuration(double *_retval);
    void GetPercent(double *_retval);
    void GetFilename(char **filename);
    void SetFilename(const char *filename);
    void GetShowControls(PRBool * _retval);
    void SetShowControls(PRBool value);
    void GetFullscreen(PRBool * _retval);
    void SetFullscreen(PRBool value);
    void GetShowlogo(PRBool * _retval);
    void SetShowlogo(PRBool value);
    void GetAutoPlay(PRBool * _retval);
    void SetAutoPlay(PRBool value);
    void GetLoop(PRBool * _retval);
    void SetLoop(PRBool value);
    void GetMIMEType(char **_retval);
    void GetPlaying(PRBool * _retval);
    void PlaylistAppend(const char *item);
    void PlaylistClear(PRBool *_retval);
    void SetOnClick(const char *event);


    // we need to provide implementation of this method as it will be
    // used by Mozilla to retrive the scriptable peer
    // and couple of other things on Unix
    NPError GetValue(NPPVariable variable, void *value);

    nsScriptablePeer *getScriptablePeer();
    nsControlsScriptablePeer *getControlsScriptablePeer();

  public:
     NPP mInstance;
    NPBool mInitialized;
    nsScriptablePeer *mScriptablePeer;
    nsControlsScriptablePeer *mControlsScriptablePeer;

    // put member data here
    char *mimetype;
    int state;
    char *url;
    char *fname;
    char *href;
    int autohref;
    char *lastmessage;
    uint16 mode;
    uint32 window_width;
    uint32 window_height;
    uint32 embed_width;
    uint32 embed_height;
    uint32 movie_width;
    uint32 movie_height;
    int setwindow;
    char *baseurl;
    char *hostname;
    int control;
    FILE *player;
    pid_t pid;
    int noredraw;
    int hrefrequested;
    int threadsetup;		// has the child thread been setup but not signalled
    int threadlaunched;		// only allow 1 child thread per instance
    int threadsignaled;		// keep track if the thread has been told to start
    int cancelled;
    int autostart;
    int controlwindow;
    int showcontrols;
    int showtracker;
    int showbuttons;
    int showfsbutton;
    int redrawbuttons;
    int mmsstream;
    Node *list;
    Node *currentnode;
    ThreadData *td;
    Window window;
    Window player_window;
    Display *display;
    Widget widget;
    uint32 nQtNext;
    char *qtNext[256];
    int panel_height;
    int panel_drawn;
    float percent;
    char *mediaCompleteCallback;
    char *mediaCompleteWithErrorCallback;
    char *mouseClickCallback;
    char *mouseDownCallback;
    char *mouseUpCallback;
    char *mouseEnterCallback;
    char *mouseLeaveCallback;
    char *onVisibleCallback;
    char *onHiddenCallback;
    char *onDestroyCallback;
    float mediaLength;		// length of media in seconds
    int mediaPercent;		// percentage of media played
    float mediaTime;		// time in seconds
    float mediaPos;		// seconds into the media
    int nomediacache;
    int controlsvisible;
    int fullscreen;
    int showlogo;
    int showtime;
    int DPMSEnabled;
    int hidden;
    int black_background;
    int nomouseinput;
    int noconsolecontrols;
    int cookies;
    long int starttime;
    int nopauseonhide;
    int targetplayer;		// if we have this create a new window and play there
    int hidestatus;
    int enablecontextmenu;
    int dontparent;
	int volume;
    
#ifdef GTK_ENABLED
    gulong delete_signal_id;
    gulong visible_signal_id;
    GtkWidget *gtkwidget;
    GtkWidget *button_window;
    GtkWidget *gtkplug;
    GtkWidget *image;
    GtkWidget *fixed_container;
    GtkWidget *fixed_event_box;
    GtkProgressBar *progress_bar;
    GtkProgressBar *mediaprogress_bar;
    GtkLabel *status;
    GtkWidget *drawing_area;
    GtkWidget *fs_window;

    GtkWidget *play_event_box;
    GtkWidget *pause_event_box;
    GtkWidget *stop_event_box;
    GtkWidget *ff_event_box;
    GtkWidget *rew_event_box;
    GtkWidget *vol_event_box;
    GtkWidget *fs_event_box;
    GtkWidget *src_event_box;

    GtkTooltips *vol_tooltip;

    GtkWidget *image_play;
    GtkWidget *image_pause;
    GtkWidget *image_stop;
    GtkWidget *image_ff;
    GtkWidget *image_rew;
    GtkWidget *image_vol;
    GtkWidget *image_fs;
    GtkWidget *image_src;

    GtkMenu *popup_menu;
    GtkMenuItem *menuitem_play;
    GtkMenuItem *menuitem_pause;
    GtkMenuItem *menuitem_stop;
    GtkMenuItem *menuitem_sep1;
    GtkMenuItem *menuitem_sep2;
    GtkMenuItem *menuitem_sep3;
    GtkMenuItem *menuitem_showcontrols;
    GtkMenuItem *menuitem_fullscreen;
    GtkMenuItem *menuitem_save;
    GtkMenuItem *menuitem_copy;
    GtkMenuItem *menuitem_config;

    GtkWidget *conf_window;
    GtkWidget *conf_vo;
    GtkWidget *conf_ao;
    GtkWidget *conf_dir;
    GtkWidget *conf_cachesize;
    GtkWidget *conf_cachepercent;
    GtkWidget *conf_showtime;
    GtkWidget *conf_enable_smil;
    GtkWidget *conf_enable_helix;
    GtkWidget *conf_nomediacache;
    GtkWidget *conf_rtsptcp;
    GtkWidget *conf_rtsphttp;
    GtkWidget *conf_enable_wmp;
    GtkWidget *conf_enable_qt;
    GtkWidget *conf_enable_rm;
    GtkWidget *conf_enable_gmp;
    GtkWidget *conf_enable_ogg;
    GtkWidget *conf_enable_mpeg;
    GtkWidget *conf_enable_mp3;
    GtkWidget *conf_enable_midi;
    GtkWidget *conf_enable_pls;
    GtkWidget *conf_enable_dvx;
    GtkWidget *conf_pauseonhide;
    GtkWidget *file_selector;   
#endif

    pthread_t player_thread;
    pthread_attr_t thread_attr;
    pthread_cond_t playlist_complete_cond;
    pthread_mutex_t playlist_mutex;
    pthread_mutex_t playlist_cond_mutex;
    pthread_mutex_t control_mutex;
    pthread_mutex_t read_mutex;

#ifdef GTK2_ENABLED
    // Pixbufs - GTK1 does not use Pixbufs
    GdkPixbuf *logo;

    GdkPixbuf *pb_sm_play_up;
    GdkPixbuf *pb_sm_play_down;
    GdkPixbuf *pb_sm_pause_up;
    GdkPixbuf *pb_sm_pause_down;
    GdkPixbuf *pb_sm_stop_up;
    GdkPixbuf *pb_sm_stop_down;
    GdkPixbuf *pb_sm_ff_up;
    GdkPixbuf *pb_sm_ff_down;
    GdkPixbuf *pb_sm_rew_up;
    GdkPixbuf *pb_sm_rew_down;
    GdkPixbuf *pb_sm_vol;
    GdkPixbuf *pb_sm_fs_up;
    GdkPixbuf *pb_sm_fs_down;
    GdkPixbuf *pb_src;

    int paused_wheninvisible;
#endif

#ifdef GTK1_ENABLED
    GdkPixmap *logo;

    GdkPixmap *pb_sm_play_up;
    GdkPixmap *pb_sm_play_down;
    GdkPixmap *pb_sm_pause_up;
    GdkPixmap *pb_sm_pause_down;
    GdkPixmap *pb_sm_stop_up;
    GdkPixmap *pb_sm_stop_down;
    GdkPixmap *pb_sm_ff_up;
    GdkPixmap *pb_sm_ff_down;
    GdkPixmap *pb_sm_rew_up;
    GdkPixmap *pb_sm_rew_down;
    GdkPixmap *pb_sm_vol;
    GdkPixmap *pb_sm_fs_up;
    GdkPixmap *pb_sm_fs_down;

    GtkRcStyle *style;
    GdkColor color;
    Window moz_window;
    int moz_x;
    int moz_y;
    XWindowAttributes window_attr;
#endif

#ifdef X_ENABLED
    XFontSet font;
    Pixmap logo;
    Pixmap logomask;
    Pixmap progress_left;
    Pixmap progress_leftmask;
    Pixmap progress_middle;
    Pixmap progress_middlemask;
    Pixmap progress_right;
    Pixmap progress_rightmask;
    Pixmap progress_fill;
    Pixmap progress_fillmask;
    int lastpercent;
#endif

    // options
    char *vo;
    char *vop;
    int novop;
    int noembed;
    char *ao;
    char *af;
    int loop;
    int rtsp_use_tcp;
    int rtsp_use_http;
    int keep_download;
    int maintain_aspect;
    int qt_speed;
    char *download_dir;
    int cachesize;
    char *output_display;
    int osdlevel;
    int cache_percent;
    int toolkitok;
    int moz_toolkit;
    int plug_toolkit;
    int framedrop;
    int autosync;
    int mc;
	int softvol;
    char *useragent;
    int enable_smil;
    int enable_helix;
    int enable_wmp;
    int enable_qt;
    int enable_rm;
    int enable_gmp;
    int enable_dvx;
    int enable_mpeg;
    int enable_mp3;
    int enable_ogg;
    int enable_midi;
    int enable_pls;

    char *tv_driver;
    char *tv_device;
    char *tv_input;
    int tv_width;
    int tv_height;
    char *tv_outfmt;
	char *profile;

    // JavaScript State
    int paused;
    int js_state;

    time_t lastOutput;
};

#endif				// __PLUGIN_H__
