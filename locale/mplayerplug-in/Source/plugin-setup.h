#include "plugin.h"
#include "../config.h"
#include <X11/Xlib.h>
#include <X11/Intrinsic.h>
#include <X11/StringDefs.h>
#include <ctype.h>
#include <stdio.h>
#include <unistd.h>
#include <strings.h>

#ifdef GTK_ENABLED
#include <gdk/gdkx.h>
#include <gtk/gtk.h>
#include <glib.h>
#endif

#ifdef GTK1_ENABLED
#include <gdk/gdk.h>
extern GMutex *gdk_threads_mutex;
#endif

#ifdef DPMSExtension
#include <X11/Xlib.h>
#ifndef DPMS_SERVER

#include <X11/X.h>
#include <X11/Xmd.h>
//#include <X11/extensions/dpms.h>
extern "C" Bool DPMSQueryExtension(Display *, int *, int *);
extern "C" Bool DPMSCapable(Display *);
extern "C" Status DPMSInfo(Display *, CARD16 *, BOOL *);
extern "C" Status DPMSEnable(Display *);
#endif
#endif

#define STATE_RESET 0
#define STATE_NEW 1
#define STATE_HAVEURL 3
#define STATE_WINDOWSET 4
#define STATE_READY 5
#define STATE_QUEUED 6
#define STATE_DOWNLOADING 7
#define STATE_DOWNLOADED_ENOUGH 8
#define STATE_CANCELLED 11

#define STATE_NEWINSTANCE 100
#define STATE_SETUPTHREAD 105
#define STATE_GETTING_PLAYLIST 110
#define STATE_WAITING_FOR_SIGNAL 112
#define STATE_STARTED_PLAYER 115
#define STATE_PLAYLIST_COMPLETE 120
#define STATE_PLAYLIST_NEXT 125
#define STATE_PLAYING 130
#define STATE_PLAY_COMPLETE 140
#define STATE_PLAY_CANCELLED 150

// speed options
#define SPEED_LOW 1
#define SPEED_MED 2
#define SPEED_HIGH 3

// JavaScript Playstates
#define JS_STATE_UNDEFINED 	0
#define JS_STATE_STOPPED 	1
#define JS_STATE_PAUSED 	2
#define JS_STATE_PLAYING	3
#define JS_STATE_SCANFORWARD	4
#define JS_STATE_SCANREVERSE 	5
#define JS_STATE_BUFFERING	6
#define JS_STATE_WAITING	7
#define JS_STATE_MEDIAENDED	8
#define JS_STATE_TRANSITIONING  9
#define JS_STATE_READY		10
#define JS_STATE_RECONNECTING	11
// not sure we should just add a state here since the states are from WMP
#define JS_STATE_INITIALIZING   12

// PlayNode Error Codes
#define ERROR_NO_ERROR			0
#define ERROR_NO_STREAM			1
#define ERROR_CODEC_FAILURE		ERROR_NO_STREAM << 1
#define ERROR_EXPLICIT_KILL		ERROR_NO_STREAM << 2
#define ERROR_PLAYER_INTERRUPTED	ERROR_NO_STREAM << 3
#define ERROR_EXECV			ERROR_NO_STREAM << 4
#define ERROR_NOT_PLAYLIST		ERROR_NO_STREAM << 5
#define ERROR_FILE_NOT_FOUND		ERROR_NO_STREAM << 6
#define ERROR_PLAYLIST_EMPTY		ERROR_NO_STREAM << 7
#define ERROR_QUIT			ERROR_NO_STREAM << 8
#define ERROR_USE_URL			ERROR_NO_STREAM << 9


#define TRYAGAIN_FALSE 			0
#define TRYAGAIN_TRUE 			1
#define TRYAGAIN_FALLBACK 		2


#ifndef STRUCTURES
#define STRUCTURES
typedef struct area {
    char url[4096];		/* url in smil <area> tag */
    char target[128];		/* target in <area>       */
    int begin;			/* begin (seconds)        */
    struct area *next;		/* this is linked list    */
} area;

typedef struct lnode {
    char url[4096];		// url to media
    char fname[1024];		// local filename
    int status;			// node status
    int retrieved;		// set to 1 when media is 100% downloaded
    int play;			// set to 1 to play this node
    int speed;			// used by QT reference files
    int playlist;		// This entry is a playlist file entry 
    int mmsstream;		// set to 1 when playlist stream
    int remove;			// set to 1 to delete file from disk
    int cancelled;		// set to 1 to cancel download of file
    int played;			// set to 1 when media play is complete
    int frombutton;		// set to 1 when link is called from srcToButton
    int entry;			// set to -1 by default, otherwise is the entry # in an ASX file
    int loop;                   // single node loop
    long int bytes;		// bytes downloaded
    long int totalbytes;	// total bytes of file
    long int cachebytes;	// low water mark before play begins
    uint32 actual_x, actual_y;	// x & y values from media
    uint32 play_x, play_y;	// x & y values that media is using
    int copy;			// set to 1 if this node is a copy
    FILE *localcache;
    int prefer_url;		// prefer using the url over the local file
    struct area *area;
    struct lnode *next;
} Node;

typedef struct _ThreadData {
    Widget w;
    nsPluginInstance *instance;
    char *argv[50];
    Node *list;
} ThreadData;

typedef struct _PlayResult {
	int errorcode;
	int tryagain;
	int retval;
} PlayResult;

#endif

// function defintions, grouped by source file
#ifndef HAVE_MEMMEM
extern "C" void *memmem(const void *haystack, const size_t haystack_len,
		    const void *needle, const size_t needle_len);
#endif

#ifndef HAVE_STRLCPY
extern "C" size_t strlcpy(char *dst, const char *src, size_t siz);
#endif

#ifndef HAVE_STRLCAT
extern "C" size_t strlcat(char *dst, const char *src, size_t siz);
#endif

// plugin-setup.cpp
char *GetMIMEDescription();
NPError GetValue(NPPVariable variable, void *value);
void New(nsPluginInstance * instance, nsPluginCreateData * parameters);
void LoadConfigFile(nsPluginInstance * instance);

// plugin-support.cpp
void lowercase(char string[]);
void unEscapeXML(char *string);
int fexists(char *file);
char *getURLBase(char *url);
char *getURLHostname(char *url);
char *getURLFilename(const char *url);
int isMms(char *url, int nomediacache);
void mmsToHttp(char *dest, char *src);
int sendCommand(nsPluginInstance * instance, char *command);
int URLcmp(const char *url1, const char *url2);
extern void remove_quotes(char *url);
void killmplayer(nsPluginInstance * instance);
void fullyQualifyURL(nsPluginInstance * instance, char *initem, char *localitem);
int toolkitOk(NPP instance, int *mozilla_toolkit, int *plugin_toolkit);
#ifdef DPMSExtension
int DPMSIsEnabled(nsPluginInstance * instance);
void DPMSReenable(nsPluginInstance * instance);
#endif

//plugin-list.cpp
void initialize(Node * l);
Node *newNode();
void deleteNode(Node * n);
void copyNode(Node * dest, Node * src);
void deleteList(Node * l);
void insertafter(Node * item, Node * newnode);
void addToList(nsPluginInstance * instance, char *item, Node * parent, int speed,int mmsstream, int entry, int loop);
void addToEnd(Node * l, Node * newnode);
void buildPlaylist(nsPluginInstance * instance, char *file, Node * parent);
void printNode(Node * l);
void printList(Node * l);
void markPlayed(Node * l);
int highest_entry(Node * l);

//plugin-ui.cpp
void InitPixbufs(nsPluginInstance * instance);
void DrawUI(Widget w, nsPluginInstance * instance, char *message,
	    int FullRedraw, int percent);
extern void FreeUI(Display * dpy, nsPluginInstance * instance);
extern void RedrawCB(Widget widget, XtPointer client_data,
		     XtPointer call_data);
#ifdef GTK_ENABLED
void play_callback(GtkWidget * widget, GdkEventExpose * event,
		   nsPluginInstance * instance);
void pause_callback(GtkWidget * widget, GdkEventExpose * event,
		   nsPluginInstance * instance);
void stop_callback(GtkWidget * widget, GdkEventExpose * event,
		   nsPluginInstance * instance);
gboolean keyboard_callback(GtkWidget *widget, GdkEventKey *event,
	               	   nsPluginInstance * instance);
gboolean mouse_callback(GtkWidget *widget, GdkEventButton *event,
	               	   nsPluginInstance * instance);
gboolean mousenotify_callback(GtkWidget * widget, GdkEventCrossing * event,
			nsPluginInstance * instance);
gboolean gtkgui_draw(void *data);
gboolean gtkgui_message(void *data);
gboolean gtkgui_progress(void *data);
gboolean gtkgui_resize(void *data);
gboolean gtkgui_stop(void *data);
gboolean gtkgui_save_enable(void *data);
gboolean gtkgui_drawMediaProgress(void *data);
gboolean gtkgui_refreshbuttonstate(void *data);
gboolean gtkgui_updatebuttons(void *data);
gboolean gtkgui_updatefullscreen(void *data);
gboolean gtkgui_setvolumetip(void *data);
gint popup_handler(GtkWidget *widget, GdkEvent *event);
void menuitem_play_callback(GtkMenuItem *menuitem, nsPluginInstance* instance);
void menuitem_pause_callback(GtkMenuItem *menuitem, nsPluginInstance* instance);
void menuitem_stop_callback(GtkMenuItem *menuitem, nsPluginInstance* instance);
void menuitem_showcontrols_callback(GtkCheckMenuItem *menuitem, nsPluginInstance* instance);
void menuitem_fullscreen_callback(GtkCheckMenuItem *menuitem, nsPluginInstance* instance);
void menuitem_save_callback(GtkMenuItem * menuitem, nsPluginInstance * instance);
void menuitem_copy_callback(GtkMenuItem * menuitem, nsPluginInstance * instance);
void menuitem_config_callback(GtkMenuItem * menuitem, nsPluginInstance * instance);
gboolean window_visible(GtkWidget *widget,GdkEvent *event,nsPluginInstance * instance);
gboolean target_hide_callback(GtkWidget *widget, GdkEvent *event, nsPluginInstance *instance);
gboolean gtkgui_show_src(void *data);
#endif
int srcToButton(char *url, nsPluginInstance * instance);

//plugin-threads.cpp
void launchPlayerThread(nsPluginInstance * instance);
void signalPlayerThread(nsPluginInstance * instance);
void SetupPlayer(nsPluginInstance * instance, XEvent * event);
void *playPlaylist(void *td);
