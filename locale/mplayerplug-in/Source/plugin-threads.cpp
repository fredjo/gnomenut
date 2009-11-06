#include "plugin.h"
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/time.h>
#include "plugin-setup.h"
#include <fcntl.h>
#include <signal.h>
#include <sys/wait.h>
#include <sys/param.h>
#include <errno.h>

extern int DEBUG;
extern int errno;

static void sig_child(int signo)
{
    // Not used, so commented out
    // wait(NULL);
    pid_t pid;
    int status;
    
    pid = wait(&status);
    return;

}


/*
 * Copyright (c) 2002 - 2003
 * NetGroup, Politecnico di Torino (Italy)
 * All rights reserved.
 * 
 * Redistribution and use in source and binary forms, with or without 
 * modification, are permitted provided that the following conditions 
 * are met:
 * 
 * 1. Redistributions of source code must retain the above copyright 
 * notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright 
 * notice, this list of conditions and the following disclaimer in the 
 * documentation and/or other materials provided with the distribution. 
 * 3. Neither the name of the Politecnico di Torino nor the names of its 
 * contributors may be used to endorse or promote products derived from 
 * this software without specific prior written permission. 
 * 
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS 
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT 
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR 
 * A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT 
 * OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, 
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT 
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, 
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY 
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT 
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE 
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 * 
 */


/* 
 * the above copyright applies to pthread_suspend only 
 */

#ifdef GTK_ENABLED
gboolean mediacallback(void *data)
{
    nsPluginInstance *instance;

    if (DEBUG > 1)
	printf("in mediacallback\n");
    instance = (nsPluginInstance *) data;
    if (instance->mediaCompleteCallback != NULL)
	NPN_GetURL(instance->mInstance,
		   instance->mediaCompleteCallback, "_self");
    if (instance->mediaCompleteWithErrorCallback != NULL)
	NPN_GetURL(instance->mInstance,
		   instance->mediaCompleteWithErrorCallback, "_self");
    return FALSE;
}
#endif

void pthread_suspend(int msec)
{

    struct timespec abstime;
    struct timeval now;

    pthread_cond_t cond;
    pthread_mutex_t mutex;
    pthread_mutexattr_t attr;

    pthread_mutexattr_init(&attr);
    pthread_mutex_init(&mutex, &attr);
    pthread_mutex_lock(&mutex);

    pthread_cond_init(&cond, NULL);

    gettimeofday(&now, NULL);

    abstime.tv_sec = now.tv_sec + msec / 1000;
    abstime.tv_nsec = now.tv_usec * 1000 + (msec % 1000) * 1000 * 1000;

    pthread_cond_timedwait(&cond, &mutex, &abstime);

    pthread_mutex_destroy(&mutex);
    pthread_cond_destroy(&cond);

}

//this function must be called with control_mutex locked
void launchPlayerThread(nsPluginInstance * instance)
{
    void *thread_return;

    if (DEBUG)
	printf("In launchPlayerThread, state = %d\n", instance->state);

    if (instance->threadlaunched == 1) {
	if (DEBUG)
	    printf("launchPlayerThread - joining thread\n");
	pthread_join(instance->player_thread, &thread_return);
    }
    if (instance->js_state == JS_STATE_UNDEFINED) {
	if (DEBUG)
	    printf("launchPlayerThread - creating new thread\n");
	pthread_create(&(instance->player_thread),
		       &(instance->thread_attr), playPlaylist,
		       (void *) (instance->td));
	instance->js_state = JS_STATE_INITIALIZING;
	instance->threadlaunched = 1;
	instance->threadsignaled = 0;
    } else {
	printf
	    ("****WARNING: launching duplicate player thread, js_state = %d\n",
	     instance->js_state);
	instance->threadlaunched = 0;
    }
}

void signalPlayerThread(nsPluginInstance * instance)
{
    //signal the player thread
    if (DEBUG) {
	printf("Signalling Player thread, state = %d, js_state = %d\n",
	       instance->state, instance->js_state);
    }

    if (instance->threadlaunched != 1)
	if (DEBUG)
	    printf("****Player thread did not launch correctly****\n");

    pthread_mutex_lock(&(instance->control_mutex));
    while (instance->js_state == JS_STATE_INITIALIZING) {
	if (DEBUG)
	    printf("Waiting for player thread to start....%i\n",
		   instance->js_state);
	pthread_mutex_unlock(&(instance->control_mutex));
	pthread_suspend(10);
	pthread_mutex_lock(&(instance->control_mutex));
    }
    if (instance->js_state == JS_STATE_BUFFERING
	|| instance->js_state == JS_STATE_READY) {
	pthread_mutex_lock(&(instance->playlist_cond_mutex));
	pthread_cond_signal(&(instance->playlist_complete_cond));
	pthread_mutex_unlock(&(instance->playlist_cond_mutex));
	instance->threadsignaled = 1;
    } else {
	if (DEBUG)
	    printf("****Player thread did not start correctly****\n");
    }
    pthread_mutex_unlock(&(instance->control_mutex));
}

FILE *mypopen(char **argv, pid_t * pid, int *control,
	      nsPluginInstance * instance)
{
    int filedesr[2], filedesw[2], err;
    pid_t child;
    long flags;
    sigset_t newmask;
    FILE *retfd;

    // Reap any unreaped children...
    int r = 0;
    while ((r = waitpid(-1, NULL, WNOHANG)) > 0) {
            ;
    }

    if (pipe(filedesr) < 0) {
	if (DEBUG) {
		printf("Unable to open pipe (filedesr)\n");
	}
	return NULL;
    }
    if (pipe(filedesw) < 0) {
	if (DEBUG) {
		printf("Unable to open pipe (filedesw)\n");
	}
	return NULL;
    }

    child = fork();

    if (!child) {
	if (DEBUG) {
	    char **tmp;
	    tmp = argv;
	    printf("Starting: ");
	    while (*tmp)
		printf("%s ", *(tmp++));
	    printf("\n");
	}

	dup2(filedesw[0], 0);
	dup2(filedesr[1], 1);
	dup2(filedesr[1], 2);
	close(filedesw[1]);
	close(filedesr[0]);
	setsid();
	setpgid(0, 0);
	sigemptyset(&newmask);
	sigaddset(&newmask, SIGTERM);
	sigaddset(&newmask, SIGKILL);
	pthread_sigmask(SIG_UNBLOCK, &newmask, 0L);
	usleep(500);
	if (execvp(argv[0], argv) < 0) {
	    err = errno;
#ifdef GTK_ENABLED
	    snprintf(instance->lastmessage, 1024, "Error: %i - %s", err,
		     strerror(err));
	    g_idle_add(gtkgui_message, instance);
#endif
	    perror("execv");

	}
	_exit(0);

    } else {
	// setup signal handler for child
	signal(SIGCHLD, sig_child);
	sigemptyset(&newmask);
	sigaddset(&newmask, SIGCHLD);
	sigaddset(&newmask, SIGTERM);
	sigaddset(&newmask, SIGKILL);
	pthread_sigmask(SIG_UNBLOCK, &newmask, 0L);

	*pid = child;
	*control = filedesw[1];
	close(filedesw[0]);
	close(filedesr[1]);

	// make the operations on the control pipe non-blocking
	flags = fcntl(*control, F_GETFL, 0);
	flags |= O_NONBLOCK;
#ifndef BSD
	flags |= O_NDELAY;
#endif
	fcntl(*control, F_SETFL, flags);

	retfd = fdopen(filedesr[0], "r");

	return retfd;
    }
}


void SetupPlayer(nsPluginInstance * instance, XEvent * event)
{
    int i;
    char xval[32], yval[32];
    char *filename;
    char *baseurl;
    char buffer[4096];
#ifndef X_DISPLAY_MISSING
    char *dispName = XDisplayName(NULL);
#endif
    bool isRemoteDisplay = false;

    if (instance->threadsetup == 1)
	return;
    if (instance->toolkitok != 0)
	return;
    instance->td->w = instance->widget;
    instance->td->instance = instance;

    if (DEBUG > 1)
	printf("Entering SetupPlayer\n");

#ifdef X_ENABLED
    DrawUI(instance->widget, instance, _("Loading Media..."), 0, -1);
#endif
#ifdef GTK_ENABLED
    if (instance->status != NULL)
	gtk_label_set_text(instance->status, _("Loading Media..."));
#endif

    if (instance->td->list == NULL) {
	instance->td->list = instance->list;
    }

/*
    if (instance->href) {
	if (DEBUG)
	    printf("using href for url\n");
	snprintf(instance->td->list->url, 1024, "%s", instance->href);
    } else 
*/
    if (instance->fname) {
	if (DEBUG)
	    printf("using fname for url\n");
	snprintf(instance->td->list->url, 4096, "%s", instance->fname);
    } else {
	if (DEBUG)
	    printf("using url for url\n");
	snprintf(instance->td->list->url, 4096, "%s", instance->url);
    }

    if ((instance->fname == NULL) && (instance->url == NULL)) {
	if (DEBUG)
	    printf("using href for url\n");
	snprintf(instance->td->list->url, 4096, "%s", instance->href);
    }

    if (instance->mode == NP_FULL) {
	snprintf(xval, 32, "%i", instance->window_width);
	snprintf(yval, 32, "%i", instance->window_height);
    } else {
	snprintf(xval, 32, "%i", instance->embed_width);
	if (instance->maintain_aspect == 0 && instance->showcontrols == 1) {
	    snprintf(yval, 32, "%i", instance->embed_height - 16);
	} else {
	    snprintf(yval, 32, "%i", instance->embed_height);
	}

    }

    baseurl = NULL;
    if (instance->baseurl == NULL) {
	baseurl = getURLBase(instance->td->list->url);

	if (baseurl != NULL) {
	    if (instance->baseurl == NULL) {
		instance->baseurl = baseurl;
	    } else {
		if (strcmp(instance->baseurl, baseurl) != 0) {
		    NPN_MemFree(instance->baseurl);
		    instance->baseurl = baseurl;
		} else {
		    NPN_MemFree(baseurl);
		}
	    }
	}
    }

    if (instance->hostname == NULL)
	instance->hostname = getURLHostname(instance->td->list->url);

    if (instance->keep_download == 1) {
	instance->td->list->remove = 0;
	filename = getURLFilename(instance->td->list->url);
	snprintf(instance->td->list->fname, 1024, "%s/%s",
		 instance->download_dir, filename);
	if (filename)
	    NPN_MemFree(filename);
    } else {
	if ((instance->nomediacache == 0)
	    && (instance->td->list->bytes > 0)) {
	    if (strlen(instance->td->list->fname) == 0) {
		snprintf(instance->td->list->fname, 1024, "%s",
			 tempnam("/tmp", "mplayerplug-inXXXXXX"));
	    }
	} else {
	    // fix up the URL
	    pthread_mutex_lock(&(instance->playlist_mutex));
	    fullyQualifyURL(instance, instance->td->list->url, buffer);
	    if (DEBUG)
		printf("url %s\nbuffer %s\n", instance->td->list->url,
		       buffer);
	    snprintf(instance->td->list->url, 4096, "%s", buffer);
	    pthread_mutex_unlock(&(instance->playlist_mutex));
	}
    }

    // set up the vars
    i = 0;
    while (i < 50) {
	instance->td->argv[i++] = NULL;
    }

    i = 0;
    snprintf(buffer, 1024, "mplayer");
    instance->td->argv[i++] = strdup(buffer);
    if (instance->novop == 1) {
	snprintf(buffer, 1024, "-vop");
	instance->td->argv[i++] = strdup(buffer);
	snprintf(buffer, 1024, "null");
	instance->td->argv[i++] = strdup(buffer);
    } else {
	if (instance->vop != NULL) {
	    snprintf(buffer, 1024, "-vop");
	    instance->td->argv[i++] = strdup(buffer);
	    snprintf(buffer, 1024, "%s", instance->vop);
	    instance->td->argv[i++] = strdup(buffer);
	    if (strncmp(instance->vop, "scale=", 6) == 0) {
		snprintf(buffer, 1024, "-fs");
		instance->td->argv[i++] = strdup(buffer);
	    }
	}
    }

    if ((instance->mode == NP_EMBED) && (instance->noembed == 0)) {
	if (instance->window != 0) {
	    snprintf(buffer, 1024, "-wid");
	    instance->td->argv[i++] = strdup(buffer);
#ifdef X_ENABLED
//          instance->player_window =
//              XCreateSimpleWindow(instance->display, instance->window, 0,
//                                  0, instance->embed_width,
//                                  instance->embed_height, 0, 0, 0);
	    snprintf(buffer, 1024, "0x%x", (int) instance->window);
	    instance->td->argv[i++] = strdup(buffer);
#endif
#ifdef GTK_ENABLED
//          if (instance->targetplayer == 1) {
//              gtk_widget_show(instance->gtkwidget);
//          }

	    if (GTK_IS_WIDGET(instance->drawing_area)) {
		gtk_widget_show(instance->drawing_area);
		gtk_widget_set_usize(instance->drawing_area, 1, 1);
//		while (gtk_events_pending())
//		    gtk_main_iteration();

	    }
#ifdef GTK2_ENABLED
	    instance->player_window =
		gtk_socket_get_id(GTK_SOCKET(instance->drawing_area));
#endif
#ifdef GTK1_ENABLED
	    instance->player_window =
		GDK_WINDOW_XWINDOW(instance->drawing_area->window);
#endif				/* if (instance->targetplayer == 1) */
	    snprintf(buffer, 1024, "0x%x", (int) instance->player_window);
	    instance->td->argv[i++] = strdup(buffer);
#ifdef GTK2_ENABLED
	    instance->visible_signal_id =
		g_signal_connect_after(G_OBJECT(instance->gtkwidget),
				       "visibility-notify-event",
				       G_CALLBACK(window_visible),
				       instance);
#endif
	    if (instance->targetplayer == 1)
		gtk_widget_hide(instance->gtkwidget);

#endif
	} else {
	    instance->player_window = 0;
	}
    }

    if ((instance->mode == NP_FULL) && (instance->noembed == 0)) {
	if (instance->window != 0) {
	    snprintf(buffer, 1024, "-wid");
	    instance->td->argv[i++] = strdup(buffer);
#ifdef X_ENABLED
	    snprintf(buffer, 1024, "0x%x", (int) instance->window);
	    instance->td->argv[i++] = strdup(buffer);
#endif
#ifdef GTK_ENABLED
	    gtk_widget_set_usize(GTK_WIDGET(instance->status),
				 instance->window_width - 20, 19);
	    gtk_widget_show(instance->drawing_area);
#ifdef GTK2_ENABLED
	    instance->player_window =
		gtk_socket_get_id(GTK_SOCKET(instance->drawing_area));
#endif
#ifdef GTK1_ENABLED
	    instance->player_window =
		GDK_WINDOW_XWINDOW(instance->drawing_area->window);
#endif
	    snprintf(buffer, 1024, "0x%x", (int) instance->player_window);
	    instance->td->argv[i++] = strdup(buffer);
#endif
	} else {
	    instance->player_window = 0;
	}
    }

    if ((instance->embed_width == 0)
	|| (instance->noembed == 1)) {
	// do nothing
    } else {
	if (instance->mode == NP_EMBED) {
#ifndef X_DISPLAY_MISSING
	    // on remote display, XShm will be disabled.
	    // it should not specified aspect. 
	    if (dispName) {
		if (strncmp(dispName, "unix:", 5) == 0)
		    dispName += 4;
		else if (strncmp(dispName, "localhost:", 10) == 0)
		    dispName += 9;
		if (*dispName != ':' || atoi(dispName + 1) >= 10) {
		    isRemoteDisplay = true;
		    instance->maintain_aspect = 0;
		    if (DEBUG)
			printf("x11 is running on remote display.\n");
		}
	    }
#endif
	    if ((isRemoteDisplay == false)
		&& (instance->targetplayer == 0)) {
		if (instance->maintain_aspect == 1) {
		    snprintf(buffer, 1024, "-vf");
		    instance->td->argv[i++] = strdup(buffer);
		    snprintf(buffer, 1024, "scale=%s:-3", xval);
		    instance->td->argv[i++] = strdup(buffer);
		} else {
		    snprintf(buffer, 1024, "-x");
		    instance->td->argv[i++] = strdup(buffer);
		    snprintf(buffer, 1024, "%s", xval);
		    instance->td->argv[i++] = strdup(buffer);
		    snprintf(buffer, 1024, "-y");
		    instance->td->argv[i++] = strdup(buffer);
		    snprintf(buffer, 1024, "%s", yval);
		    instance->td->argv[i++] = strdup(buffer);
		}
	    }
	}
    }

//    if (instance->rtsp_use_tcp == 1) {
//      snprintf(buffer, 1024, "-rtsp-stream-over-tcp");
//      instance->td->argv[i++] = strdup(buffer);
//    }
//    if (instance->cachesize > 0) {
//      snprintf(buffer, 1024, "-cache");
//      instance->td->argv[i++] = strdup(buffer);
//      snprintf(buffer, 1024, "%i", instance->cachesize);
//      instance->td->argv[i++] = strdup(buffer);
//    } else {
//      snprintf(buffer, 1024, "-nocache");
//      instance->td->argv[i++] = strdup(buffer);
//    }

//    if (instance->loop == 1) {
//      snprintf(instance->td->argv[i++], 1024, "-loop");
//      snprintf(instance->td->argv[i++], 1024, "0");
//    }

    if (instance->vo) {
	snprintf(buffer, 1024, "-vo");
	instance->td->argv[i++] = strdup(buffer);
	snprintf(buffer, 1024, "%s", instance->vo);
	instance->td->argv[i++] = strdup(buffer);
	if ((strncmp(buffer, "x11", 3) == 0)
	    || (strstr(buffer, ",x11") != NULL)) {
	    snprintf(buffer, 1024, "-zoom");	/* -vo x11 needs -zoom for rescaling */
	    instance->td->argv[i++] = strdup(buffer);
	}
    }

    if (instance->ao) {
	snprintf(buffer, 1024, "-ao");
	instance->td->argv[i++] = strdup(buffer);
	snprintf(buffer, 1024, "%s", instance->ao);
	instance->td->argv[i++] = strdup(buffer);
    }

    if (instance->profile) {
	snprintf(buffer, 1024, "-profile");
	instance->td->argv[i++] = strdup(buffer);
	snprintf(buffer, 1024, "%s", instance->profile);
	instance->td->argv[i++] = strdup(buffer);
    }

    if (instance->af) {
	snprintf(buffer, 1024, "-af");
	instance->td->argv[i++] = strdup(buffer);
	snprintf(buffer, 1024, "%s", instance->af);
	instance->td->argv[i++] = strdup(buffer);
    }

    if (instance->output_display) {
	snprintf(buffer, 1024, "-display");
	instance->td->argv[i++] = strdup(buffer);
	snprintf(buffer, 1024, "%s", instance->output_display);
	instance->td->argv[i++] = strdup(buffer);
    }

    if (instance->framedrop == 1) {
	snprintf(buffer, 1024, "-framedrop");
	instance->td->argv[i++] = strdup(buffer);
    }

    if (instance->autosync > 0) {
	snprintf(buffer, 1024, "-autosync");
	instance->td->argv[i++] = strdup(buffer);
	snprintf(buffer, 1024, "%i", instance->autosync);
	instance->td->argv[i++] = strdup(buffer);
    }

    if (instance->mc > 0) {
	snprintf(buffer, 1024, "-mc");
	instance->td->argv[i++] = strdup(buffer);
	snprintf(buffer, 1024, "%i", instance->mc);
	instance->td->argv[i++] = strdup(buffer);
    }

    snprintf(buffer, 1024, "-osdlevel");
    instance->td->argv[i++] = strdup(buffer);
    snprintf(buffer, 1024, "%i", instance->osdlevel);
    instance->td->argv[i++] = strdup(buffer);

    /* some extra flags for mplayer */
    snprintf(buffer, 1024, "-nojoystick");	/* annoying... */
    instance->td->argv[i++] = strdup(buffer);
    if (instance->noconsolecontrols) {
	snprintf(buffer, 1024, "-noconsolecontrols");
	instance->td->argv[i++] = strdup(buffer);
    }

    if (instance->cookies) {
	snprintf(buffer, 1024, "-cookies");
	instance->td->argv[i++] = strdup(buffer);
    }
//    snprintf(buffer, 1024, "-nofs");  /* no full screen */
//    instance->td->argv[i++] = strdup(buffer);
    if (instance->softvol) {
	snprintf(buffer, 1024, "-softvol");	/* use software volume control */
	instance->td->argv[i++] = strdup(buffer);
    }
    snprintf(buffer, 1024, "-slave");	/* slave mode */
    instance->td->argv[i++] = strdup(buffer);
    instance->td->argv[i++] = NULL;

    // ok we have the command line setup
    if (DEBUG)
	printf("ready to setup threads\n");

    pthread_attr_setdetachstate(&(instance->thread_attr),
				PTHREAD_CREATE_JOINABLE);

    if (DEBUG)
	printf("creating thread - NP_EMBED\n");
#ifdef X_ENABLED
    DrawUI(instance->widget, instance, _("Getting playlist..."), 0, -1);
#endif
#ifdef GTK_ENABLED
    if (instance->status != NULL)
	gtk_label_set_text(instance->status, _("Getting playlist..."));
#endif

    if (instance->state < STATE_GETTING_PLAYLIST)
	instance->state = STATE_GETTING_PLAYLIST;

    if (0) {
	printf("SetupPlayer: printing instance->list\n");
	printList(instance->list);
	printf("SetupPlayer: printing instance->td->list\n");
	printList(instance->td->list);
    }

    if (DEBUG)
	printf("creating player thread\n");


    pthread_mutex_lock(&(instance->control_mutex));
    instance->js_state = JS_STATE_UNDEFINED;
    launchPlayerThread(instance);
    instance->threadsetup = 1;
    pthread_mutex_unlock(&(instance->control_mutex));

    // recommended slight pause
    usleep(1);

    if (DEBUG)
	printf("MAIN THREAD DONE\n");
}

void refresh_frame(char *buffer, _ThreadData * td, Node * node)
{
    int seconds;
    static int oldseconds = -1;
    char *start;
    char *endptr;
    area *runner;
    area *this_area;
#ifdef SMILDEBUG
    static FILE *error = NULL;
    char errortmp[64];
    if (!error)
	error = fopen("/tmp/mplayerplugin-smil.log", "a");
#endif

    /* just to be sure */
    if (node == NULL || node->area == NULL)
	return;

    start = buffer;
    while ((start = strstr(start, "A:")) != NULL && strlen(start) > 7) {
	start += 2;
	seconds = (int) strtol(start, &endptr, 0);
	if (seconds == oldseconds || start == endptr)
	    continue;
#ifdef SMILDEBUG
	strlcpy(errortmp, start, 63);
	errortmp[64] = 0;
	fprintf(error, "->seconds: %d, oldseconds: %d, buffer: '%s'\n",
		seconds, oldseconds, errortmp);
	fflush(error);
#endif
	runner = node->area;
	this_area = node->area;
	while (runner) {
	    if (runner->begin < seconds
		&& runner->begin > this_area->begin)
		this_area = runner;
	    if (runner->begin == seconds) {
		NPN_GetURL(td->instance->mInstance,
			   runner->url, runner->target);
		break;
	    }
	    runner = runner->next;
	}
	/*
	 *  If it was a seek we have to follow.
	 */
	if ((oldseconds - seconds > 1 || seconds - oldseconds > 1)
	    && !runner)
	    NPN_GetURL(td->instance->mInstance, this_area->url,
		       this_area->target);
	oldseconds = seconds;
    }
}


//return true if we should try to play this again immediately, false if not 

PlayResult *playNode(ThreadData * local_td, Node * local_list,
		     char *local_url, int local_mmsstream, int *usefps,
		     int *nomouseinput, int *maybeplaylist)
{

    PlayResult *result = (PlayResult *) NPN_MemAlloc(sizeof(PlayResult));
    char buffer[1024];
    char message[1024];
    int notfound;
    char *vo;
    char vm[10];
    char *cf;
    double cfpercent;
    char *eos;
    char *msg;

    long cfbytes, flags = 0;
    int i, amt;
    int zerocfbytes_count = 0;
    char url_copy[4096];
    int c;
    char ch;
    int length_request_count = 0;
    float lastmedialength = -1.0;
    int lastmediapercent = -1;
    int lastpercent = -1;
#ifdef GTK_ENABLED
    int fsupdated = 0;
#endif

    result->errorcode = ERROR_NO_ERROR;
    result->tryagain = TRYAGAIN_FALLBACK;
    result->retval = FALSE;

/*
  the meaning of the above is as follows:
   TRYAGAIN_TRUE:  we should definitely try playing this url again immediately
   TRYAGAIN_FALSE: we should definitely NOT do that.
   TRYAGAIN_FALLBACK if we should try playing this url again immediately
                    only if we have a fallback url.

   these are for internal use in this function only. The function
   should still return either 0 or 1 (and never 2). 

*/

    pthread_cleanup_push((void (*)(void *))
			 pthread_mutex_unlock,
			 (void *) &(local_td->instance->control_mutex));
    pthread_mutex_lock(&(local_td->instance->control_mutex));
    sendCommand(local_td->instance, "get_time_length");
    local_td->instance->mediaLength = 0.0;
    pthread_mutex_unlock(&(local_td->instance->control_mutex));
    pthread_cleanup_pop(0);
#ifdef GTK_ENABLED
    g_idle_add(gtkgui_save_enable, local_td->instance);
//      g_idle_add(gtkgui_refreshbuttonstate, local_td->instance);
#endif

    // set this to 0 for every new media file;
    local_td->instance->movie_height = 0;
    local_td->instance->movie_width = 0;

    if (local_td->instance->player != NULL) {
	if (DEBUG) {
	    printf("Getting file mode flags\n");
	}
	flags = fcntl(fileno(local_td->instance->player), F_GETFL, 0);
    }

    while (1) {
	pthread_testcancel();
	if (local_td->instance->player == NULL)
	    break;

	if (feof(local_td->instance->player)) {
	    pthread_testcancel();
	    break;
	}
	pthread_testcancel();

	assert(local_td != NULL);
	assert(local_td->instance != NULL);
	assert(local_td->instance->control > 0);
	assert(local_td->instance->player != NULL);

	pthread_testcancel();
#ifdef GTK_ENABLED
//      g_idle_add(gtkgui_save_enable, local_td->instance);
//      g_idle_add(gtkgui_refreshbuttonstate, local_td->instance);
#endif

	pthread_testcancel();
	if (local_td->instance->player != NULL) {
	    // if (fgets(buffer, 1024, local_td->instance->player) == NULL) {
	    //  continue;
	    // }

	    // fgets is not a pthread cancel point, so we basically have to rewrite fgets
	    // to make this work better. If we cancel the thread while fgets is waiting for 
	    // for an EOS then we crash.
	    buffer[0] = '\0';
	    i = 0;
	    do {
		pthread_testcancel();
		// need to lock around the read
		pthread_mutex_lock(&(local_td->instance->read_mutex));

		if ((local_td->instance->cancelled == 0)
		    && (local_td->instance->player != NULL)) {
		    // c = fgetc(local_td->instance->player);
                    // read a char here, otherwise reading int causes endian issue
                    amt = fread(&ch, 1, 1, local_td->instance->player);
                    c = amt == 1 ? ch : EOF;
		} else {
		    c = EOF;
		    amt = 0;
		}
		pthread_mutex_unlock(&(local_td->instance->read_mutex));

		if (c == EOF) {
		    buffer[i] = '\0';
		    break;
		}
		if (c == 0)
		    continue;
		buffer[i] = (char) c;
		i += amt;
		if (i >= 1024) {
		    buffer[1023] = '\0';
		    break;
		}
	    } while (((unsigned char) c != '\n'));
	    if (buffer[0] == '\0')
		continue;
	    else if (i < 1024)	// make sure we don't access out of bounds
		buffer[i] = '\0';	// make sure we NULL terminate the string

	}

	pthread_testcancel();

	if (DEBUG) {
	    if (strlen(buffer) > 0)
		printf("READ: %s \n", buffer);
	}

	refresh_frame(buffer, local_td, local_list);


	pthread_testcancel();
	if (strstr(buffer, "STREAM") != NULL) {
	    if (local_td->instance->player != NULL) {
		if (DEBUG) {
		    printf("Setting file mode to non-blocking\n");
		}
		fcntl(fileno(local_td->instance->player), F_SETFL,
		      flags | O_NONBLOCK);
	    }
	}

	pthread_testcancel();
	if (strstr(buffer, "Cache size") != NULL
	    || strstr(buffer, "Starting playback") != NULL
	    || strstr(buffer, "mmst packet_length") != NULL ) {
	    if (local_td->instance->player != NULL) {
		if (DEBUG) {
		    printf("Setting file mode back to blocking\n");
		}
		fcntl(fileno(local_td->instance->player), F_SETFL, flags);
	    }
	}


	pthread_testcancel();
	if (strstr(buffer, "Cache fill:") != NULL) {
	    strlcpy(message, strstr(buffer, "Cache fill"), 1024);
	    notfound = 1;
	    while (notfound) {
		eos = strrchr(message, '\r');
		if (eos == NULL)
		    eos = message;
		if (strstr(eos, "Cache fill:") == NULL) {
		    // if we don't find "Cache fill:" then replace the \r with \0, making message shorter
		    eos[0] = '\0';
		    // break out of the loop if too short
		    if (strlen(message) < 5)
			break;
		} else {
		    // if we found the last one in the message string, move from \r to C in the string
		    eos++;
		    notfound = 0;
		}
	    }

	    // Only update the display if there is something worth displaying
	    if (strstr(eos, "Cache fill:") != NULL) {
		cf = strstr(eos, "Cache fill:");
		lastpercent = (int) cfpercent;
		i = sscanf(cf, "Cache fill: %lf %% (%ld bytes)",
			   &cfpercent, &cfbytes);
		//the following is a workaround for an mplayer bug
		// we can try to fallback to mmst when we get
		// Cache Fill: 0% (0 bytes) several times.
		if (i == 2 && cfbytes == 0) {
		    zerocfbytes_count++;
		}
		if (DEBUG && zerocfbytes_count > 0) {
		    printf("Cache Fill: 0%% (0 bytes), count = %d\n",
			   zerocfbytes_count);
		}
		if (zerocfbytes_count >= 3 &&
		    ((strncmp(local_url, "mms://", 6) == 0) ||
		     (local_mmsstream
		      && strncmp(local_url, "http://", 7) == 0))) {

		    if (DEBUG) {
			printf("Exiting. Will try again with mmst://\n");
		    }
		    result->tryagain = TRYAGAIN_FALLBACK;
		    break;
		}

		snprintf(message, 1024, _("Buffering %s"), local_url);

#ifdef X_ENABLED
		DrawUI(local_td->instance->widget,
		       local_td->instance, eos, -1, (int) cfpercent);
#endif
#ifdef GTK_ENABLED
		snprintf(local_td->instance->lastmessage, 1024, "%s",
			 message);

		local_td->instance->percent = (cfpercent / 100.0);
		if (lastpercent != cfpercent) {
		    g_idle_add(gtkgui_message, local_td->instance);
		    g_idle_add(gtkgui_progress, local_td->instance);
		}
#endif
		pthread_mutex_lock(&(local_td->instance->control_mutex));
		local_td->instance->js_state = JS_STATE_BUFFERING;
		pthread_mutex_unlock(&(local_td->instance->control_mutex));

	    }
	}

	pthread_testcancel();
	if (strstr(buffer, "Starting") != NULL) {
	    snprintf(message, 1024, _("Playing %s"), local_url);
#ifdef X_ENABLED
	    DrawUI(local_td->instance->widget,
		   local_td->instance, message, 0, -1);
#endif
#ifdef GTK_ENABLED
	    snprintf(local_td->instance->lastmessage, 1024, "%s", message);
	    g_idle_add(gtkgui_message, local_td->instance);
#endif
	    pthread_mutex_lock(&(local_td->instance->control_mutex));
	    local_td->instance->js_state = JS_STATE_PLAYING;
	    pthread_mutex_unlock(&(local_td->instance->control_mutex));
	}

	pthread_testcancel();
	if (strstr(buffer, "VO:") != NULL) {
	    fcntl(fileno(local_td->instance->player), F_SETFL, flags);
	    if (local_td->instance->mode == NP_EMBED
		&& local_td->instance->noembed == 0)
		local_td->instance->noredraw = 1;
	    vo = strstr(buffer, "VO:");
	    sscanf(vo, "VO: [%9[^]]] %ix%i => %ix%i", vm,
		   &(local_list->actual_x),
		   &(local_list->actual_y),
		   &(local_list->play_x), &(local_list->play_y));
	    if (local_td->instance->mode == NP_EMBED
		&& local_td->instance->noembed == 0) {
		if (local_td->instance->panel_height == 0)
		    local_td->instance->panel_height =
			local_td->instance->embed_height -
			local_list->play_y;

		if (local_td->instance->player_window != 0
		    && local_td->instance->movie_height !=
		    local_list->play_y) {

		    local_td->instance->movie_height = local_list->play_y;
		    local_td->instance->movie_width = local_list->play_x;
#ifdef GTK_ENABLED
		    g_idle_add(gtkgui_resize, local_td->instance);
		    g_idle_add(gtkgui_draw, local_td->instance);
#endif
		}

		if (DEBUG)
		    printf
			("----player thread: panel height in thread = %i\n",
			 local_td->instance->panel_height);

	    } else {
		local_td->instance->panel_height = 16;
		if (local_td->instance->player_window != 0
		    && local_td->instance->movie_height !=
		    local_list->play_y) {
		    local_td->instance->movie_height = local_list->play_y;
		    local_td->instance->movie_width = local_list->play_x;

#ifdef GTK_ENABLED
		    g_idle_add(gtkgui_resize, local_td->instance);
#endif
		}
		local_td->instance->embed_height =
		    local_td->instance->window_height;
		local_td->instance->embed_width =
		    local_td->instance->window_width;
		local_td->instance->panel_height =
		    local_td->instance->window_height;
#ifdef GTK_ENABLED
		g_idle_add(gtkgui_draw, local_td->instance);
#endif
	    }

	}

	pthread_testcancel();
	if (strstr(buffer, "Video: no video") != NULL) {
	    fcntl(fileno(local_td->instance->player), F_SETFL, flags);
	    if (local_td->instance->panel_height == 0)
		local_td->instance->panel_height = 32;
	    local_td->instance->noredraw = 0;
	    local_td->instance->movie_height = 1;
	    local_td->instance->movie_width = 1;
#ifdef GTK_ENABLED
	    g_idle_add(gtkgui_resize, local_td->instance);
	    g_idle_add(gtkgui_draw, local_td->instance);
#endif
	}

	pthread_testcancel();
	// mplayer answer back messages
	if (strstr(buffer, "ANS_LENGTH") != 0) {
	    if ((int) local_td->instance->mediaLength != 0)
		lastmedialength = local_td->instance->mediaLength;
	    msg = strstr(buffer, "ANS_LENGTH");
	    sscanf(msg, "ANS_LENGTH=%f",
		   &(local_td->instance->mediaLength));
	    if (DEBUG > 1)
		printf("Media Length = %f\n",
		       local_td->instance->mediaLength);
	}

	pthread_testcancel();
	// mplayer answer back messages
	if (strstr(buffer, "ANS_volume") != 0) {
	    if ((int) local_td->instance->mediaLength != 0)
		lastmedialength = local_td->instance->mediaLength;
	    msg = strstr(buffer, "ANS_volume");
	    sscanf(msg, "ANS_volume=%i", &(local_td->instance->volume));
	    if (DEBUG)
		printf("Volume = %i\n", local_td->instance->volume);
#ifdef GTK_ENABLED
	    g_idle_add(gtkgui_setvolumetip, local_td->instance);
#endif
	}

	pthread_testcancel();
	// mplayer answer back messages
	if (strstr(buffer, "ANS_TIME_POSITION") != 0) {
	    msg = strstr(buffer, "ANS_TIME_POSITION");
	    sscanf(msg, "ANS_TIME_POSITION=%f",
		   &(local_td->instance->mediaPos));
	    if (DEBUG > 1)
		printf("Media Position = %f\n",
		       local_td->instance->mediaPos);
	    if ((int) local_td->instance->mediaLength > 1) {
		lastmediapercent = local_td->instance->mediaPercent;
		local_td->instance->mediaPercent =
		    (int) ((local_td->instance->mediaPos * 100) /
			   local_td->instance->mediaLength);
#ifdef GTK_ENABLED
		if (lastmediapercent != local_td->instance->mediaPercent) {
		    g_idle_add(gtkgui_drawMediaProgress,
			       local_td->instance);
		    g_idle_add(gtkgui_refreshbuttonstate,
			       local_td->instance);
		}
#endif
	    }
	}

	pthread_testcancel();
	if (strstr(buffer, "ANS_PERCENT_POSITION") != 0) {
	    msg = strstr(buffer, "ANS_PERCENT_POSITION");
	    sscanf(msg, "ANS_PERCENT_POSITION=%i",
		   &(local_td->instance->mediaPercent));
	    if (local_td->instance->mediaPercent == 0) {
		if ((int) local_td->instance->mediaLength > 1)
		    lastmediapercent = local_td->instance->mediaPercent;
		local_td->instance->mediaPercent =
		    (int) ((local_td->instance->mediaPos * 100) /
			   local_td->instance->mediaLength);
#ifdef GTK_ENABLED
		if (lastmediapercent != local_td->instance->mediaPercent) {
		    g_idle_add(gtkgui_drawMediaProgress,
			       local_td->instance);
		    g_idle_add(gtkgui_refreshbuttonstate,
			       local_td->instance);
		}
#endif
	    }
	    if (DEBUG > 1)
		printf("Percent Played = %i\n",
		       local_td->instance->mediaPercent);
	}

	pthread_testcancel();
	// this is only needed for the retry code
	if (strstr(buffer, "\rA:") != 0 || strstr(buffer, "\rV:") != 0) {
	    //audio or video is playing
	    msg = strstr(buffer, "\rA:");
	    if (msg != NULL) {
		sscanf(msg, "\rA:%f", &(local_td->instance->mediaTime));
		if (DEBUG > 1)
		    printf("mediaTime = %f\n",
			   local_td->instance->mediaTime);
#ifdef GTK_ENABLED
//              g_idle_add(gtkgui_draw, local_td->instance);
#endif
	    } else {
		msg = strstr(buffer, "\rV:");
		if (msg != NULL) {
		    sscanf(msg, "\rV:%f",
			   &(local_td->instance->mediaTime));
		    if (DEBUG > 1)
			printf("mediaTime = %f\n",
			       local_td->instance->mediaTime);
		}
	    }
	    pthread_cleanup_push((void (*)(void *))
				 pthread_mutex_unlock,
				 (void *) &(local_td->instance->
					    control_mutex));
	    pthread_mutex_lock(&(local_td->instance->control_mutex));

	    if ((local_td->instance->js_state != JS_STATE_STOPPED)
		&& (local_td->instance->js_state != JS_STATE_PAUSED))
		local_td->instance->js_state = JS_STATE_PLAYING;

	    pthread_mutex_unlock(&(local_td->instance->control_mutex));
	    pthread_cleanup_pop(0);
	    pthread_testcancel();
	    if (local_td->instance->paused == 0) {
		pthread_cleanup_push((void (*)(void *))
				     pthread_mutex_unlock,
				     (void *) &(local_td->instance->
						control_mutex));
		pthread_mutex_lock(&(local_td->instance->control_mutex));
		if ((int) local_td->instance->mediaLength > 1)
		    sendCommand(local_td->instance, "get_time_pos");
		if (local_td->instance->mediaLength > lastmedialength) {
		    sendCommand(local_td->instance, "get_time_length");
		} else {
		    if (length_request_count < 10) {
			sendCommand(local_td->instance, "get_time_length");
			length_request_count++;
		    }
		}
		if (local_td->instance->volume == -1) {
		    sendCommand(local_td->instance, "get_property volume");
		}
		pthread_mutex_unlock(&(local_td->instance->control_mutex));
		pthread_cleanup_pop(0);
#ifdef GTK_ENABLED
		if (fsupdated == 0) {
		    g_idle_add(gtkgui_updatefullscreen,
			       local_td->instance);
		    fsupdated = 1;
		}
#endif
	    }
	    result->tryagain = TRYAGAIN_FALSE;
	}

	pthread_testcancel();
	if (strstr(buffer, "Connect") != NULL) {
	    snprintf(message, 1024, "%s", buffer);
#ifdef X_ENABLED
	    DrawUI(local_td->instance->widget,
		   local_td->instance, message, 0, -1);
#endif
#ifdef GTK_ENABLED
	    snprintf(local_td->instance->lastmessage, 1024, "%s", message);
	    g_idle_add(gtkgui_message, local_td->instance);
#endif
	    pthread_mutex_lock(&(local_td->instance->control_mutex));
	    local_td->instance->js_state = JS_STATE_WAITING;
	    pthread_mutex_unlock(&(local_td->instance->control_mutex));

	}

/*
	ICY Info: StreamTitle='Three Days Grace - Animal I Have Become';StreamUrl='http://www.thebuzz.fm';
*/
	pthread_testcancel();
	if (strstr(buffer, "ICY Info") != NULL) {
	    msg = strstr(buffer, "'");
	    if (msg != NULL) {
		strncpy(message, _("Stream Title: "), 1024);
		strncat(message, msg + 1, 1024);
		for (i = 0; i < (int) strlen(message) - 1; i++) {
		    if (message[i] == '\'') {
			message[i] = '\0';
			break;
		    }
		}
		if (DEBUG) {
		    printf("Title: %s\n", message);
		}
	    }
#ifdef X_ENABLED
	    DrawUI(local_td->instance->widget,
		   local_td->instance, message, 0, -1);
#endif
#ifdef GTK_ENABLED
	    snprintf(local_td->instance->lastmessage, 1024, "%s", message);
	    g_idle_add(gtkgui_message, local_td->instance);
#endif
	    pthread_mutex_lock(&(local_td->instance->control_mutex));
	    local_td->instance->js_state = JS_STATE_WAITING;
	    pthread_mutex_unlock(&(local_td->instance->control_mutex));

	}

	pthread_testcancel();
	if (strstr(buffer, "Initiated") != NULL) {
	    snprintf(message, 1024, "%s", buffer);
#ifdef X_ENABLED
	    DrawUI(local_td->instance->widget,
		   local_td->instance, message, 0, -1);
#endif
#ifdef GTK_ENABLED
	    snprintf(local_td->instance->lastmessage, 1024, "%s", message);
	    g_idle_add(gtkgui_message, local_td->instance);
#endif
	}

	pthread_testcancel();
	if (strstr(buffer, "No stream found") != NULL) {
	    snprintf(message, 1024, "%s", buffer);
#ifdef X_ENABLED
	    DrawUI(local_td->instance->widget,
		   local_td->instance, message, 0, -1);
#endif
#ifdef GTK_ENABLED
	    snprintf(local_td->instance->lastmessage, 1024, "%s", message);
	    g_idle_add(gtkgui_message, local_td->instance);
#endif
	    //result->tryagain = TRYAGAIN_FALSE;
	    //result->errorcode = ERROR_NO_STREAM;
	    //break;
	}


	pthread_testcancel();
	if (strstr(buffer, "Example: mplayer -playlist ") != NULL) {
	    if (*maybeplaylist == 0) {
		result->tryagain = TRYAGAIN_TRUE;
		*maybeplaylist = 1;
	    } else {
		result->tryagain = TRYAGAIN_FALSE;
		result->errorcode = ERROR_NOT_PLAYLIST;
	    }
	    break;
	}


	pthread_testcancel();
	if (strstr(buffer, "Failed to open") != NULL) {
	    if ((strstr(buffer, "/dev/rtc") == NULL)
		&& (strstr(buffer, "LIRC") == NULL)
		&& (strstr(buffer, "registry file") == NULL)) {	// ignore RTC device and LIRC device failures
		if (local_td->instance->currentnode->mmsstream == 1) {
		    if (DEBUG)
			printf("Trying fallback\n");
		    result->tryagain = TRYAGAIN_FALLBACK;
		    result->errorcode = ERROR_FILE_NOT_FOUND;
		    break;
		} else {
		    result->tryagain = TRYAGAIN_FALSE;
		    result->errorcode = ERROR_FILE_NOT_FOUND;
		    break;
		}
	    }
	}

	pthread_testcancel();
	if (strstr(buffer, "Error while decoding") != NULL) {
	    if (!isMms(local_url, local_td->instance->nomediacache)) {
		if (DEBUG)
		    printf("Resetting stream, 1 sec rewind\n");
		pthread_suspend(1000);
		pthread_cleanup_push((void (*)(void *))
				     pthread_mutex_unlock,
				     (void *) &(local_td->instance->
						control_mutex));
		pthread_mutex_lock(&(local_td->instance->control_mutex));
		sendCommand(local_td->instance, "seek -1 0\n");
		pthread_mutex_unlock(&(local_td->instance->control_mutex));
		pthread_cleanup_pop(0);

	    }
	}

	pthread_testcancel();
	if (strstr(buffer, "Quit") != NULL) {
	    if (DEBUG)
		printf("----player thread: breaking read loop - Quit\n");
	    local_td->instance->js_state = JS_STATE_UNDEFINED;
	    result->tryagain = TRYAGAIN_FALSE;
	    result->errorcode = ERROR_QUIT;
	    break;
	}

	pthread_testcancel();
	if (strstr
	    (buffer,
	     "MPlayer interrupted by signal 13 in module: demux_open") !=
	    NULL) {
	    if (DEBUG)
		printf
		    ("----player thread: breaking read loop - demux_open\n");
	    local_td->instance->js_state = JS_STATE_UNDEFINED;
	    if (local_td->instance->rtsp_use_tcp == 1) {
		local_td->instance->rtsp_use_tcp = 0;
		result->tryagain = TRYAGAIN_TRUE;
	    } else {
		result->tryagain = TRYAGAIN_FALSE;
		result->errorcode = ERROR_PLAYER_INTERRUPTED;
	    }
	    break;
	}


	pthread_testcancel();
	// detect Quicktime file with old codec
	if (strstr(buffer, "MOV: missing header (moov/cmov) chunk!") !=
	    NULL) {
	    if (DEBUG) {
		printf
		    ("----player thread: waiting to download entire movie\n");
	    }
	    result->tryagain = TRYAGAIN_TRUE;
	    while (1) {
		pthread_testcancel();
		usleep(100);
		pthread_testcancel();
		pthread_mutex_lock(&(local_td->instance->control_mutex));
		local_td->instance->js_state = JS_STATE_BUFFERING;
		pthread_mutex_unlock(&(local_td->instance->control_mutex));
		pthread_mutex_lock(&(local_td->instance->playlist_mutex));
		local_td->instance->state = STATE_DOWNLOADING;
		if (local_list->retrieved == 1) {
		    local_td->instance->state = STATE_PLAYING;
		    pthread_mutex_lock(&
				       (local_td->instance->
					control_mutex));
		    local_td->instance->js_state = JS_STATE_PLAYING;
		    pthread_mutex_unlock(&
					 (local_td->instance->
					  control_mutex));
		    pthread_mutex_unlock(&
					 (local_td->instance->
					  playlist_mutex));
		    printf("----player thread: Movie downloaded\n");
		    break;
		}
		pthread_mutex_unlock(&
				     (local_td->instance->playlist_mutex));
	    }
	    break;
	}

	pthread_testcancel();
	if (strstr(buffer, "FPS not specified") != NULL) {
	    result->tryagain = TRYAGAIN_TRUE;
	    *usefps = 1;
	    break;
	}

	pthread_testcancel();
	if (strstr(buffer, "nomouseinput") != NULL) {
	    *nomouseinput = 0;
	    result->tryagain = TRYAGAIN_TRUE;
	    break;
	}

/*	pthread_testcancel();
	if (strstr(buffer, "Win32 LoadLibrary failed to load") != NULL) {
	    if (local_td->instance->useragent != NULL) {
		free(local_td->instance->useragent);
		local_td->instance->useragent = NULL;
		result->tryagain = TRYAGAIN_TRUE;
		break;
	    }
	}
*/
	pthread_testcancel();
	if (strstr(buffer, "Exiting") != NULL) {
	    if (DEBUG)
		printf
		    ("----player thread: breaking read loop - Exiting\n");
	    if (local_td->instance->mediaPercent == 0) {
		if (DEBUG)
		    printf("local_url = %s\nprefer_url = %i\n", local_url,local_list->prefer_url);
		if (local_list->prefer_url == 1) {

		    result->tryagain = TRYAGAIN_FALSE;
		    result->errorcode = ERROR_NO_ERROR;

		} else {
		    result->tryagain = TRYAGAIN_TRUE;
		    result->errorcode = ERROR_USE_URL;
		}

	    } else {
		local_td->instance->mediaPercent = 100;
#ifdef GTK_ENABLED
		g_idle_add(gtkgui_drawMediaProgress, local_td->instance);
#endif
		result->tryagain = TRYAGAIN_FALSE;
		result->errorcode = ERROR_NO_ERROR;
	    }

	    break;
	}

	pthread_testcancel();
	// break out if mplayer crashes
	if (strstr(buffer, "interrupted") != NULL) {
	    if (DEBUG)
		printf
		    ("----player thread: breaking read loop - interrupted\n");
	    if (strstr(buffer, "video_read_frame") != NULL) {
		local_td->instance->js_state = JS_STATE_UNDEFINED;
		if (local_td->instance->rtsp_use_tcp == 1) {
		    local_td->instance->rtsp_use_tcp = 0;
		    result->tryagain = TRYAGAIN_TRUE;
		} else {
#ifdef GTK_ENABLED
		    snprintf(local_td->instance->lastmessage, 1024,
			     "Error: %s", buffer);
		    g_idle_add(gtkgui_message, local_td->instance);
#endif
		    result->tryagain = TRYAGAIN_FALSE;
		    result->errorcode = ERROR_PLAYER_INTERRUPTED;
		}
	    }
	    break;
	}

	pthread_testcancel();
	if (strstr(buffer, "execv") != NULL) {
	    if (DEBUG)
		printf
		    ("----player thread: breaking read loop - execv failed\n");
	    local_td->instance->js_state = JS_STATE_UNDEFINED;
#ifdef GTK_ENABLED
	    snprintf(local_td->instance->lastmessage, 1024, "Error: %s %s",
		     local_td->argv[0], buffer);
	    g_idle_add(gtkgui_message, local_td->instance);
#endif
	    result->tryagain = TRYAGAIN_FALSE;
	    result->errorcode = ERROR_EXECV;
	    break;
	}

	pthread_testcancel();
	if (strstr(buffer, "explicit kill") != NULL) {
	    if (DEBUG)
		printf("----player thread: breaking read loop - killed\n");
	    result->tryagain = TRYAGAIN_FALSE;
	    result->errorcode = ERROR_EXPLICIT_KILL;
	    break;
	}

	pthread_testcancel();
	if (strstr(buffer, "decoder is not supposed") != NULL) {
	    if (DEBUG)
		printf
		    ("----player thread: breaking read loop - codec failure\n");
#ifdef GTK_ENABLED
	    snprintf(local_td->instance->lastmessage, 1024, "Error: %s %s",
		     local_td->argv[0], buffer);
	    g_idle_add(gtkgui_message, local_td->instance);
#endif
	    result->tryagain = TRYAGAIN_FALSE;
	    result->errorcode = ERROR_CODEC_FAILURE;
	    break;
	}

	pthread_testcancel();
	if (strstr(buffer, "Failed to decode frame") != NULL) {
	    if (DEBUG)
		printf
		    ("----player thread: breaking read loop - codec failure\n");
#ifdef GTK_ENABLED
	    snprintf(local_td->instance->lastmessage, 1024, "Error: %s %s",
		     local_td->argv[0], buffer);
	    g_idle_add(gtkgui_message, local_td->instance);
#endif
	    result->tryagain = TRYAGAIN_FALSE;
	    result->errorcode = ERROR_CODEC_FAILURE;
	    break;
	}

	pthread_testcancel();
/*	commmented out to make BBC site work    
	if (strstr(buffer, "everything done") != NULL) {
	    if (DEBUG)
		printf
		    ("----player thread: breaking read loop - codec issue\n");
	    result->tryagain = TRYAGAIN_FALSE;
	    result->errorcode = ERROR_CODEC_FAILURE;
	    break;
	}
*/

	assert(local_td->instance->player != NULL);
    }				// end of READ LOOP

    if (DEBUG) {
	printf("----player thread: tryagain = %d\n", result->tryagain);
    }
    //we always return either true or false
    if (result->tryagain == TRYAGAIN_TRUE) {
	result->retval = TRUE;
    }

    if (result->tryagain == TRYAGAIN_FALSE) {
	result->retval = FALSE;
    }

    if (result->tryagain == TRYAGAIN_FALLBACK) {
	//fallback to msst and then to http if we can
	strlcpy(url_copy, local_url, 4095);
	url_copy[4095] = '\0';

	if (strncmp(local_url, "mms://", 6) == 0) {
	    snprintf(local_url, 4095, "mmst://%s", url_copy + 6);
	    result->retval = TRUE;
	} else if (local_mmsstream &&
		   strncmp(local_url, "mmst://", 7) == 0) {

	    snprintf(local_url, 4095, "http://%s", url_copy + 7);
	    result->retval = TRUE;
	} else {
	    result->retval = FALSE;
	}
	if (DEBUG)
	    printf("TRYAGAIN_FALLBACK entered, new url is %s\n",
		   local_url);
    }
#ifdef GTK_ENABLED
    if (strstr(local_td->instance->lastmessage, "Error") == NULL) {
	snprintf(local_td->instance->lastmessage, 1024, _("Stopped"));
	g_idle_add(gtkgui_message, local_td->instance);
    }
#endif
    return result;
}

void *playPlaylist(void *td)
{

    PlayResult *result = NULL;
    FILE *playlist;
    char cmd[8192];
    char message[1024];
    char mmsplaylist[1024];
    char buffer[1024];		// scratch pad
    char *p;			// pointer to find the ( in the callback

    int argc = 0, base_argc = 0;
    ThreadData *local_td;
    Node *local_list;
    char local_url[4096];
    int local_mmsstream;
    int i;
    int loop_single = 0;
    int loop_count;
    Node *node;

    int tryagain;
    int usefps;
    int nomouseinput;
    int maybeplaylist;
    int listempty;
    int lasterror = -1;
    int force_url = 0;

    local_td = (ThreadData *) td;
    local_list = local_td->list;

    if (local_td == NULL)
	pthread_exit(0);

    if (DEBUG)
	printf("----player thread: in playPlaylist\n");

    pthread_cleanup_push((void (*)(void *)) pthread_mutex_unlock,
			 (void *) &(local_td->instance->
				    playlist_cond_mutex));
    pthread_mutex_lock(&(local_td->instance->playlist_cond_mutex));

    pthread_mutex_lock(&(local_td->instance->control_mutex));
    local_td->instance->js_state = JS_STATE_BUFFERING;
    pthread_mutex_unlock(&(local_td->instance->control_mutex));

    if (DEBUG)
	printf
	    ("----player thread: about to go to sleep, js_state = %d, state = %d\n",
	     local_td->instance->js_state, local_td->instance->state);


    if (local_td->instance->state < STATE_STARTED_PLAYER) {
	// wait for playlist_complete_cond to be signalled
	// this should happen when we have completed getting all the playlist elements
	local_td->instance->state = STATE_WAITING_FOR_SIGNAL;
	pthread_cond_wait(&(local_td->instance->playlist_complete_cond),
			  &(local_td->instance->playlist_cond_mutex));
	pthread_mutex_unlock(&(local_td->instance->playlist_cond_mutex));

	pthread_testcancel();
	sleep(1);
	// playlist_cond_mutex should be unlocked now
	if (local_td != NULL) {
	    if (DEBUG > 1)
		printf("local_td = %p\n", local_td);
	    if (local_td->instance != NULL) {
		if (DEBUG > 1)
		    printf("local_td->instance = %p\n",
			   local_td->instance);

		pthread_testcancel();
		local_td->instance->state = STATE_STARTED_PLAYER;
		if (DEBUG)
		    printf
			("---player thread: got wakeup signal, js_state = %d, state = %d\n",
			 local_td->instance->js_state,
			 local_td->instance->state);
	    } else {
		if (DEBUG)
		    printf
			("We were signalled to start but local_td->instance is NULL, should not happen\n");
		pthread_exit(0);
	    }
	} else {
	    if (DEBUG)
		printf
		    ("We were signalled to start but local_td is NULL, should not happen\n");
	    pthread_exit(0);
	}
    } else {
	local_td->instance->threadsignaled = 1;
    }

    pthread_cleanup_pop(0);

    if (DEBUG)
	printf("----player thread: playPlayList - waiting complete\n");

    pthread_testcancel();
#ifdef X_ENABLED
    DrawUI(local_td->w, local_td->instance, _("Loading Media..."), 0, -1);
#endif
#ifdef GTK_ENABLED
    snprintf(local_td->instance->lastmessage, 1024, _("Loading Media..."));
    g_idle_add(gtkgui_message, local_td->instance);
#endif

    memset(cmd, '\0', 1);
    i = 0;
    if (DEBUG)
	printf("----player thread: building command string\n");

    pthread_cleanup_push((void (*)(void *)) pthread_mutex_unlock,
			 (void *) &(local_td->instance->playlist_mutex));
    pthread_mutex_lock(&(local_td->instance->playlist_mutex));

    while (local_td->argv[argc] != NULL) {
	if (DEBUG)
	    printf("PLAY %i:%s\n", i, local_td->argv[argc]);
	argc++;
    }

    base_argc = argc;

    pthread_mutex_unlock(&(local_td->instance->playlist_mutex));
    pthread_cleanup_pop(0);

    //handle qtNext
    //shouldn't this be somewhere else?
    i = 0;
    while (local_td->instance->qtNext[i] != NULL) {
	node = newNode();
	if (DEBUG)
	    printf("----player thread: adding %s\n",
		   local_td->instance->qtNext[i]);
	strlcpy(node->url, local_td->instance->qtNext[i], 4096);
	i++;
	addToEnd(local_list, node);
    }

    // preview playlist
    local_list = local_td->list;
    if (DEBUG) {
	while (local_list != NULL) {
	    printf
		("----player thread:\nlocal_list =  %p\nPL URL: %s\nplay = %i, cancelled= %i\nnext= %p\n",
		 local_list,
		 local_list->url, local_list->play,
		 local_list->cancelled, local_list->next);
	    local_list = local_list->next;
	}
    }
    // count playable items in the list, if playable =1 and loop = 1 then add a couple of args
    if (local_td->instance->loop >= 0) {
	i = 0;
	local_list = local_td->list;
	while (local_list != NULL) {
	    if (local_list->play)
		i++;
	    local_list = local_list->next;
	}
	if (DEBUG)
	    printf
		("----player thread:\nFound %i files to play and loop is true\n",
		 i);

	// if the play count is exactly 1 then tell mplayer to loop continuously and to use the same frame buffer (should eliminate blinking)
	if (i == 1 || local_td->instance->loop == 1) {
	    loop_single = 1;
	    local_td->argv[argc++] = strdup("-loop");
	    snprintf(buffer, 32, "%i", local_td->instance->loop);
	    local_td->argv[argc++] = strdup(buffer);
	    local_td->argv[argc++] = strdup("-fixed-vo");
	    base_argc = argc;
	}
    }

    loop_count = local_td->instance->loop;
    if (DEBUG)
	printf("----player thread: set loop_count to %i\n", loop_count);

    while (1) {
	if (DEBUG)
	    printf("----player thread: entering loop. loop_count = %i\n",
		   loop_count);

	pthread_cleanup_push((void (*)(void *)) pthread_mutex_unlock,
			     (void *) &(local_td->instance->
					playlist_mutex));

	pthread_mutex_lock(&(local_td->instance->playlist_mutex));

	//find a node to play
	if (DEBUG)
	    printf("----player thread: looking for node to play\n");

	listempty = 0;
	local_list = local_td->list;
	while (local_list != NULL) {
	    if (DEBUG)
		printf
		    ("----player thread: play = %i played = %i entry = %i fname = %s\n",
		     local_list->play, local_list->played,
		     local_list->entry, local_list->fname);
	    if (local_list->play && !(local_list->played)) {
		break;
	    }
	    local_list = local_list->next;
	}

	if (local_list == NULL) {
	    //nothing to play: will exit, unless we have loop
	    // the -- is to countdown the loop
	    if (DEBUG)
		printf
		    ("----player thread: local_list is NULL loop_count = %i\n",
		     loop_count);

	    if (local_td->instance->loop != 0)
		loop_count--;

	    if (DEBUG)
		printf
		    ("----player thread: loop_count = %i loop_single = %i\n",
		     loop_count, loop_single);

	    // we're gonna reset the playlist, because we are going to play it again
	    if ((loop_count > 0) && (loop_single == 0)
		|| (local_td->instance->loop == 0)) {
		local_list = local_td->list;
		while (local_list != NULL) {
		    if (local_list->play)
			local_list->played = 0;
		    local_list = local_list->next;
		}

		// rescan again, find the first playable item from the top of the list.
		local_list = local_td->list;
		while (local_list != NULL) {
		    if (local_list->play && !(local_list->played)) {
			break;
		    }
		    local_list = local_list->next;
		}
	    }
	}

	local_td->instance->currentnode = local_list;
	if (local_list == NULL) {
	    if (DEBUG)
		printf("----player thread: nothing on the list to play\n");
	    listempty = 1;
	    local_mmsstream = 0;
	    // update MediaCompleteWithError Callback with PLAYLIST EMPTY but only if we have not played anything
	    if ((local_td->instance->mediaCompleteWithErrorCallback !=
		 NULL) && (lasterror == -1)) {
		if (DEBUG)
		    printf("mediaCompleteWithError(before) = %s\n",
			   local_td->instance->
			   mediaCompleteWithErrorCallback);
		strlcpy(message,
			local_td->instance->mediaCompleteWithErrorCallback,
			1024);
		p = index(message, '(');
		if (p == NULL) {
		    p = message + strlen(message);
		}
		*p = '\0';
		snprintf(buffer, 1024, "%s(%i);", message,
			 ERROR_PLAYLIST_EMPTY);
		NPN_MemFree(local_td->instance->
			    mediaCompleteWithErrorCallback);
		local_td->instance->mediaCompleteWithErrorCallback =
		    (char *) NPN_MemAlloc(strlen(buffer));
		strlcpy(local_td->instance->mediaCompleteWithErrorCallback,
			buffer, strlen(buffer));
		if (DEBUG)
		    printf("mediaCompleteWithError(after) = %s\n",
			   local_td->instance->
			   mediaCompleteWithErrorCallback);
	    }
	} else {
	    //we have a node to play
	    if (DEBUG)
		printf("----player thread: chose url %s\n",
		       local_list->url);
	    listempty = 0;
	    snprintf(local_url, 4096, "%s", local_list->url);
	    local_url[4095] = '\0';
	    local_mmsstream = local_list->mmsstream;
	}
	if (local_td->instance->rtsp_use_http
	    && strncmp(local_url, "rtsp://", 7) == 0)
	    memcpy(local_url, "http://", 7);

	pthread_mutex_unlock(&(local_td->instance->playlist_mutex));
	pthread_cleanup_pop(0);

	if (listempty == 1)
	    break;

	pthread_testcancel();

	usefps = 0;
	nomouseinput = local_td->instance->nomouseinput;
	maybeplaylist = 0;

	do {

	    pthread_testcancel();

	    tryagain = 0;
	    argc = base_argc;
	    while (argc < 50) {
		if (local_td->argv[argc] != NULL)
		    free(local_td->argv[argc]);
		local_td->argv[argc++] = NULL;
	    }
	    argc = base_argc;

	    unEscapeXML(local_url);

	    if (DEBUG)
		printf("----player thread: playing url: %s\n", local_url);
	    snprintf(message, 1024, _("Playing %s"), local_list->url);
#ifdef X_ENABLED
	    DrawUI(local_td->w, local_td->instance, message, 0, -1);
#endif
#ifdef GTK_ENABLED
	    snprintf(local_td->instance->lastmessage, 1024, "%s", message);
	    g_idle_add(gtkgui_message, local_td->instance);
#endif
	    pthread_cleanup_push((void (*)(void *))
				 pthread_mutex_unlock,
				 (void *) &(local_td->instance->
					    playlist_mutex));
	    pthread_mutex_lock(&(local_td->instance->playlist_mutex));



	    // for rtsp streams we need to specify FPS
	    if (usefps == 1) {
		if (strncmp(local_list->url, "rtsp", 4) == 0) {
		    local_td->argv[argc++] = strdup("-fps");
		    local_td->argv[argc++] = strdup("30");
		}
		usefps = 0;
	    }

	    if (local_td->instance->rtsp_use_tcp == 1) {
		if (strncmp(local_list->url, "rtsp", 4) == 0) {
		    snprintf(buffer, 1024, "-rtsp-stream-over-tcp");
		    local_td->argv[argc++] = strdup(buffer);
		}
	    }

	    if (local_td->instance->useragent) {
		snprintf(buffer, 1024, "-user-agent");
		local_td->argv[argc++] = strdup(buffer);
		snprintf(buffer, 1024, "%s",
			 local_td->instance->useragent);
		local_td->argv[argc++] = strdup(buffer);
	    }

	    if (nomouseinput) {
		local_td->argv[argc++] = strdup("-nomouseinput");
//          } else {
//              nomouseinput = 0;
	    }

	    if (strcmp(local_url, "tv://") == 0) {
		if (local_td->instance->tv_driver) {
		    snprintf(buffer, 1024, "-tv:driver");
		    local_td->argv[argc++] = strdup(buffer);
		    snprintf(buffer, 1024, "%s",
			     local_td->instance->tv_driver);
		    local_td->argv[argc++] = strdup(buffer);
		}

		if (local_td->instance->tv_device) {
		    snprintf(buffer, 1024, "-tv:device");
		    local_td->argv[argc++] = strdup(buffer);
		    snprintf(buffer, 1024, "%s",
			     local_td->instance->tv_device);
		    local_td->argv[argc++] = strdup(buffer);
		}

		if (local_td->instance->tv_input) {
		    snprintf(buffer, 1024, "-tv:input");
		    local_td->argv[argc++] = strdup(buffer);
		    snprintf(buffer, 1024, "%s",
			     local_td->instance->tv_input);
		    local_td->argv[argc++] = strdup(buffer);
		}

		if (local_td->instance->tv_width > 0) {
		    snprintf(buffer, 1024, "-tv:width");
		    local_td->argv[argc++] = strdup(buffer);
		    snprintf(buffer, 1024, "%i",
			     local_td->instance->tv_width);
		    local_td->argv[argc++] = strdup(buffer);
		}

		if (local_td->instance->tv_height > 0) {
		    snprintf(buffer, 1024, "-tv:height");
		    local_td->argv[argc++] = strdup(buffer);
		    snprintf(buffer, 1024, "%i",
			     local_td->instance->tv_height);
		    local_td->argv[argc++] = strdup(buffer);
		}

		if (local_td->instance->tv_outfmt) {
		    snprintf(buffer, 1024, "-tv:outfmt");
		    local_td->argv[argc++] = strdup(buffer);
		    snprintf(buffer, 1024, "%s",
			     local_td->instance->tv_outfmt);
		    local_td->argv[argc++] = strdup(buffer);
		}
	    }

	    if (local_td->instance->starttime > 0) {
		local_td->argv[argc++] = strdup("-ss");
		snprintf(message, 1024, "%li",
			 local_td->instance->starttime);
		local_td->argv[argc++] = strdup(message);
		local_td->instance->starttime = 0;
	    }


	    if (local_td->instance != NULL)
		local_td->instance->mmsstream = local_list->mmsstream;
	    if (local_list->mmsstream) {
#ifdef GTK_ENABLED
		g_idle_add(gtkgui_updatebuttons, local_td->instance);
#endif
		if (local_td->instance->nomediacache == 0) {
		    local_td->argv[argc++] = strdup("-cache");
		    snprintf(message, 1024, "%i",
			     local_td->instance->cachesize);
		    local_td->argv[argc++] = strdup(message);
		}
		if (DEBUG)
		    printf("pre local_url is %s\n", local_url);
		if (result != NULL) {
		    if (result->tryagain != TRYAGAIN_FALLBACK) {
			// mmsToHttp(local_url, local_list->url);
		    }
		} else {
		    if (DEBUG)
			printf("result = null\n");
		    // mmsToHttp(local_url, local_list->url);
		}
		if (DEBUG)
		    printf("local_url is %s\n", local_url);
		local_td->argv[argc++] = strdup(local_url);
		local_list->prefer_url = 1;
		if (local_td->instance->keep_download) {
		    snprintf(mmsplaylist, sizeof(mmsplaylist),
			     "%s/playlist",
			     local_td->instance->download_dir);
		    playlist = fopen(mmsplaylist, "a");
		    if (playlist != NULL) {
			fprintf(playlist, "%s\n", local_url);
			fclose(playlist);
		    }
		}
	    } else {
		if (result != NULL) {
		    if (result->errorcode == ERROR_USE_URL) {
			force_url = 1;
			local_list->prefer_url = 1;
		    } else {
			force_url = 0;
		    }
		}

		if (strlen(local_list->fname) == 0 || force_url == 1) {
		    if (local_td->instance->nomediacache == 0) {
			local_td->argv[argc++] = strdup("-cache");
			snprintf(message, 1024, "%i",
				 local_td->instance->cachesize);
			local_td->argv[argc++] = strdup(message);
		    }
		    if (local_list->playlist == 1 || maybeplaylist != 0) {
			local_td->argv[argc++] = strdup("-cache");
			snprintf(message, 1024, "%i",
				 local_td->instance->cachesize);
			local_td->argv[argc++] = strdup(message);
			local_td->argv[argc++] = strdup("-playlist");
		    }
		    local_td->argv[argc++] = strdup(local_url);
		    local_list->prefer_url = 1;
		} else {
//		    local_td->argv[argc++] = strdup("-nocache");
                    local_td->argv[argc++] = strdup("-cache");
                    snprintf(message, 1024, "%i",
                                local_td->instance->cachesize);
                    local_td->argv[argc++] = strdup(message);
		    if (local_list->playlist == 1 || maybeplaylist != 0)
			local_td->argv[argc++] = strdup("-playlist");
		    local_td->argv[argc++] = strdup(local_list->fname);
		}
	    }

	    if (DEBUG) {
		printf("----player thread: URL: %s\n", local_url);
	    }

	    pthread_mutex_unlock(&(local_td->instance->playlist_mutex));
	    pthread_cleanup_pop(0);

	    pthread_testcancel();
#ifdef GTK_ENABLED

//		while (gtk_events_pending())
//		    gtk_main_iteration();

#endif


	    //start up mplayer
	    pthread_cleanup_push((void (*)(void *))
				 pthread_mutex_unlock,
				 (void *) &(local_td->instance->
					    control_mutex));
	    pthread_mutex_lock(&(local_td->instance->control_mutex));

	    local_td->instance->player =
		mypopen(local_td->argv, &(local_td->instance->pid),
			&(local_td->instance->control),
			local_td->instance);
	    if (local_td->instance->player != NULL) {
		local_td->instance->js_state = JS_STATE_PLAYING;
	    }

	    pthread_mutex_unlock(&(local_td->instance->control_mutex));
	    pthread_cleanup_pop(0);

	    if (result != NULL) {
		lasterror = result->errorcode;
		NPN_MemFree(result);
		result = NULL;
	    }

	    local_td->instance->state = STATE_PLAYING;
	    if (local_td->instance->player != NULL) {

		assert(local_td->instance->control > 0);

		// reset media counters
		local_td->instance->mediaLength = 0.0;
		local_td->instance->mediaPercent = 0;

		result =
		    playNode(local_td, local_list, local_url,
			     local_mmsstream, &usefps, &nomouseinput,
			     &maybeplaylist);

		pthread_mutex_lock(&(local_td->instance->control_mutex));
		local_td->instance->js_state = JS_STATE_TRANSITIONING;
		pthread_mutex_unlock(&(local_td->instance->control_mutex));

		if (DEBUG)
		    printf
			("----player thread: playNode returned = %d\n",
			 result->retval);

		pthread_testcancel();
		assert(local_list != NULL);

		local_td->instance->mediaPercent = 0;
#ifdef GTK_ENABLED
		g_idle_add(gtkgui_drawMediaProgress, local_td->instance);
#endif
		if (local_td->instance->targetplayer == 0) {
		    local_td->instance->movie_height = 1;
		    local_td->instance->movie_width = 1;
#ifdef GTK_ENABLED
		    g_idle_add(gtkgui_resize, local_td->instance);
		    g_idle_add(gtkgui_draw, local_td->instance);
#endif
		}
		// this cancels the download of this media
		// if there are more than 1, saves bandwidth
		// also prevents us from playing it again
		pthread_mutex_lock(&(local_td->instance->playlist_mutex));
		if (result->retval == 0) {
		    if (local_list->mmsstream
			&& (result->errorcode == ERROR_QUIT)) {
			local_list->played = 0;
		    } else {
			if (result->errorcode == ERROR_FILE_NOT_FOUND) {
			    // if we got a file not found, don't mark the whole 
			    // mirror list as not found
			    local_list->played = 1;
			} else {
			    markPlayed(local_list);
			}
			//local_list->played = 1;
		    }
		}
		pthread_mutex_unlock(&
				     (local_td->instance->playlist_mutex));

		assert(local_td->instance != NULL);
		if (DEBUG)
		    printf("----player thread: played = %i - %s\n",
			   local_list->played, local_list->fname);

		//close the pipes to mplayer

		assert(local_td->instance->control > 0);
		close(local_td->instance->control);
		local_td->instance->control = -1;

		assert(local_td->instance->player != NULL);
		fclose(local_td->instance->player);
		local_td->instance->player = NULL;

		if (DEBUG)
		    printf("----player thread: close done\n");

		local_td->instance->state = STATE_PLAYLIST_NEXT;

	    }
	    // update MediaCompleteWithError Callback
	    if (local_td->instance->mediaCompleteWithErrorCallback != NULL) {
		if (DEBUG)
		    printf("mediaCompleteWithError(before) = %s\n",
			   local_td->instance->
			   mediaCompleteWithErrorCallback);
		strlcpy(message,
			local_td->instance->mediaCompleteWithErrorCallback,
			1024);
		p = index(message, '(');
		if (p == NULL) {
		    p = message + strlen(message);
		}
		*p = '\0';
		snprintf(buffer, 1024, "%s(%i);", message,
			 result->errorcode);
		NPN_MemFree(local_td->instance->
			    mediaCompleteWithErrorCallback);
		local_td->instance->mediaCompleteWithErrorCallback =
		    (char *) NPN_MemAlloc(strlen(buffer));
		strlcpy(local_td->instance->mediaCompleteWithErrorCallback,
			buffer, strlen(buffer));
		if (DEBUG)
		    printf("mediaCompleteWithError(after) = %s\n",
			   local_td->instance->
			   mediaCompleteWithErrorCallback);
	    }

	} while (result->retval);

	// if we are in a list of mmsstreams, then we need to stop 

	if (DEBUG)
	    printf("mmsstream = %i errorcode = %i \n",
		   local_list->mmsstream, result->errorcode);

	if (local_list->mmsstream && (result->errorcode == ERROR_QUIT)) {
	    if (DEBUG)
		printf("breaking out of next item loop\n");
	    break;
	}
	local_td->instance->currentnode = NULL;

	//if we get here, either the control pipes are closed
	//  or they were never opened in the first place

	if (DEBUG) {
	    printf("----player thread: transitioning to next item\n");
	}

	pthread_mutex_lock(&(local_td->instance->control_mutex));
	local_td->instance->js_state = JS_STATE_TRANSITIONING;
	pthread_mutex_unlock(&(local_td->instance->control_mutex));

	pthread_testcancel();
	assert(local_td != NULL);
	assert(local_td->instance != NULL);
	pthread_testcancel();


	// if we have media type that changes from video to audio 
	// in X mode this will take care of that situation
	local_td->instance->noredraw = 0;

	/*
	   assert(local_td->instance->state != STATE_PLAY_CANCELLED);
	 */
    }				// end main while loop

    if (DEBUG) {
	printf("----player thread: nothing to play\n");
    }
#ifdef GTK_ENABLED
    g_idle_add(gtkgui_stop, local_td->instance);
    // call media complete when we are done with the playlist
    if (local_td->instance->mediaCompleteCallback != NULL) {
	g_idle_add(mediacallback, local_td->instance);
    }
#endif
#ifdef X_ENABLED
    if (local_td->instance->mediaCompleteCallback != NULL)
	NPN_GetURL(local_td->instance->mInstance,
		   local_td->instance->mediaCompleteCallback, "_self");
    if (local_td->instance->mediaCompleteWithErrorCallback != NULL)
	NPN_GetURL(local_td->instance->mInstance,
		   local_td->instance->mediaCompleteWithErrorCallback,
		   "_self");
#endif
#ifdef GTK2_ENABLED
    if (GTK_IS_WIDGET(local_td->instance->src_event_box)) {
	g_idle_add(gtkgui_show_src,local_td->instance);
    }
#endif
    if (DEBUG) {
	printf("----player thread: callbacks complete\n");
    }
    // free all the data (command line parameters) that is used to start mplayer
    pthread_mutex_lock(&(local_td->instance->control_mutex));
    local_td->instance->js_state = JS_STATE_UNDEFINED;
    argc = base_argc;
    while (argc < 50) {
	if (local_td->argv[argc] != NULL)
	    free(local_td->argv[argc]);
	local_td->argv[argc++] = NULL;
    }
    pthread_mutex_unlock(&(local_td->instance->control_mutex));
    pthread_mutex_unlock(&(local_td->instance->playlist_cond_mutex));
    // since thread is dead, reset thread state variables.
    local_td->instance->threadsetup = 0;
    local_td->instance->threadlaunched = 0;
    local_td->instance->threadsignaled = 0;

    //once we get here, the thread is only allowed to access stack
    // variables and then exit (since another player thread may have
    // been started at this time).    

    if (DEBUG) {
	printf("----player thread: normal exit\n");
    }
    //pthread_exit(0);

    return NULL;
}
