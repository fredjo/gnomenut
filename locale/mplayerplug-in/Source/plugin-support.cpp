#include "plugin.h"
#include <strings.h>
#include <errno.h>
#include <sys/wait.h>

extern int errno;
extern int DEBUG;

void lowercase(char string[])
{
    int i = 0;

    if (DEBUG > 1)
	printf("in lowercase\n");

    while (string[i]) {
	string[i] = tolower(string[i]);
	i++;
    }

    return;
}

void unEscapeXML(char *string)
{
//    char *p;

    if (DEBUG > 1)
	printf("in unHTML\n");

//    while ((p = strstr(string, "&amp;")) != NULL) {
//	strcpy(p + 1, p + 5);
//    }

    return;
}

int fexists(char *file)
{

    FILE *fp;

    if (DEBUG > 1)
	printf("in fexists\n");

    if (file == NULL)
	return 0;

    fp = fopen(file, "r");
    if (fp != NULL) {
	fclose(fp);
	return 1;
    } else {
	return 0;
    }

}

char *getURLHostname(char *url)
{

    char *hostname;
    char *loc;
    int i, len;

    if (DEBUG > 1)
	printf("entering getURLHostname\n");

    if (url == NULL)
	return NULL;
    len = strlen(url);
    if (len == 0)
	return NULL;
    hostname = (char *) NPN_MemAlloc(sizeof(char) * (len + 1));
    strcpy(hostname, url);

    loc = strstr(url, "://");
    if (loc == NULL) {
	NPN_MemFree(hostname);
	return NULL;
    }
    loc = loc + 3;

    i = 0;

    while (loc[0] != '/') {
	hostname[i] = loc[0];
	loc++;
	i++;
	if (i > len) {
	    i = 0;
	    break;
	}
    }
    if (i == 0) {
	NPN_MemFree(hostname);
	hostname = NULL;
    } else {
	hostname[i] = '\0';
    }
    if (DEBUG > 1)
	printf("exiting getURLHostname with %s\n", hostname);

    return hostname;

}

char *getURLFilename(const char *url)
{

    char *filename;
    char *tmp;
    int len;

    if (DEBUG > 1)
	printf("in getURLFilename\n");

    if (url == NULL)
	return NULL;
    len = strlen(url);
    if (len == 0)
	return NULL;
    filename = (char *) NPN_MemAlloc(sizeof(char) * (len + 1));
    tmp = rindex((char *)url, '/');
    if (tmp == NULL) {
	strcpy(filename, url);
	return filename;
    }
    strcpy(filename, tmp + 1);	// take everything upto, but not including the / starting from the right.
    return filename;
}

char *getURLBase(char *url)
{

    char *base;
    int i, len;

    if (DEBUG > 1)
	printf("in getURLBase\n");

    if (url == NULL)
	return NULL;
    len = strlen(url);
    if (len == 0)
	return NULL;
    base = (char *) NPN_MemAlloc(sizeof(char) * (len + 1));
    strcpy(base, url);
    if (DEBUG > 1)
	printf("in getURLBase base: %s\n", base);

    for (i = len - 1; i >= 0; i--) {
	if (base[i] != '/') {
	    base[i] = '\0';
	} else {
	    break;
	}
    }
    if ((strlen(base) == 0) || (i <= 0)) {
	NPN_MemFree(base);
	base = NULL;
    }
    if (base != NULL) {
	if (DEBUG)
	    printf("exiting URL base with %s\n", base);
    } else {
	if (DEBUG)
	    printf("exiting URL base with NULL\n");
    }
    return base;
}

int isMms(char *url, int nomediacache)
{
    if (url == NULL)
	return 0;
    if ((strncasecmp(url, "mms://", 6) == 0)
	|| (strncasecmp(url, "mmst://", 7) == 0)
	|| (strncasecmp(url, "mmsu://", 7) == 0)
	|| (strncasecmp(url, "dvd://", 6) == 0)
	|| (strncasecmp(url, "smb://", 6) == 0)
	|| (strncasecmp(url, "tv://", 5) == 0)
	|| (strncasecmp(url, "pnm://", 6) == 0)
	|| (strncasecmp(url, "rtsp://", 7) == 0)) {
	if (DEBUG > 1)
	    printf("isMms = true\n");
	return 1;
    } else {
	if (nomediacache) {
	    if ((strncasecmp(url, "file://", 7) == 0)
		|| (strncasecmp(url, "smb://", 6) == 0)
		|| fexists(url)) {
		if (DEBUG > 1)
		    printf("isMms = false\nurl = %s\n", url);
		return 0;

	    } else {
		if (DEBUG > 1)
		    printf("isMms = true\n");
		return 1;
	    }

	} else {
	    if (DEBUG > 1)
		printf("isMms = false\nurl = %s\n", url);
	    return 0;
	}
    }
}

void mmsToHttp(char *dest, char *src)
{
    char *temp;

    if (strncasecmp(src, "mms", 3) == 0) {
	temp = (char *) NPN_MemAlloc(sizeof(char) * (strlen(src) + 1 +
						     sizeof("http") -
						     sizeof("mms")));
	strcpy(temp, "http");
	strcat(temp, src + 3);
	strcpy(dest, temp);
	NPN_MemFree(temp);
    }
    return;
}



int URLcmp(const char *url1, const char *url2)
{
    char *buffer1, *buffer2;
    char *tmp1;
    char *tmp2;
    char *q1;			// question mark in tmp
    char *q2;			// question mark in tmp2
    int tmp1hasq = 0;		// tmp1 has a ?
    int tmp2hasq = 0;		// tmp2 has a ?
    char *hostname1 = NULL;	// hostname1
    char *hostname2 = NULL;	// hostname2
    char *protocol1 = NULL;
    char *protocol2 = NULL;
    int ret;

    if (DEBUG > 1)
	printf("in URLcmp\n");

    if (strcmp(url1, url2) == 0)
	return 0;

    // replace %20 with spaces in both strings
    buffer1 = strdup(url1);
    buffer2 = strdup(url2);

    while ((tmp1 = strstr(buffer1, "%20"))) {
	if (tmp1 != NULL) {
	    *tmp1 = ' ';
	    tmp1++;
	    *tmp1 = '\0';
	    strcat(buffer1, tmp1 + 2);
	}
    }

    while ((tmp1 = strstr(buffer2, "%20"))) {
	if (tmp1 != NULL) {
	    *tmp1 = ' ';
	    tmp1++;
	    *tmp1 = '\0';
	    strcat(buffer2, tmp1 + 2);
	}
    }

    if (strcmp(buffer1, buffer2) == 0) {
	//if (DEBUG) printf("URL's match\n buffer1 %s len:%i\n buffer2 %s len %i\n",buffer1,strlen(buffer1),buffer2,strlen(buffer2));
	free(buffer1);
	free(buffer2);
	ret = 0;
    } else {
	//if (DEBUG) printf("URL's don't match\n buffer1 %s len:%i\n buffer2 %s len%i\n",buffer1,strlen(buffer1),buffer2,strlen(buffer2));
	ret = -1;
    }

    if (strncasecmp(buffer1, "file://", 7) == 0) {
	if (strcmp(buffer1 + 7, buffer2) == 0) {
	    free(buffer1);
	    free(buffer2);
	    ret = 0;
	}
    }

    if (strncasecmp(buffer2, "file://", 7) == 0) {
	if (strcmp(buffer1, buffer2 + 7) == 0) {
	    free(buffer1);
	    free(buffer2);
	    ret = 0;
	}
    }

    if (ret == -1) {
	hostname1 = getURLHostname(buffer1);
	hostname2 = getURLHostname(buffer2);

	if (hostname1 != NULL && hostname2 != NULL
	    && strstr(hostname2, hostname1) == NULL) {
	    // hostname1 is a not substring of hostname2
	    // this is done for the case where the playlist has two urls that basically point to the same site
	    // one of the sites is an internal URL and the other is an external. We want to be able to play either
	    // site. So as long as the hostnames are completly different we should be ok. 
	    if (DEBUG > 1)
		printf("URLcmp: hostnames do not match\n");
	    ret = -1;
	} else {
	    // url1 is a substring of url2, so continue comparing
	    if (DEBUG > 1)
		printf("hostname1 = %s\nhostname2 = %s\n", hostname1,
		       hostname2);
	    // compare the paths, some sites change the hostname mid stream (like cartoonnetwork.com -> www.cartoonnetwork.com);
	    tmp1 = strstr(buffer1, "://");
	    if (tmp1 != NULL) {
		protocol1 =
		    (char *) malloc((long) tmp1 - (long) buffer1 + 1);
		strncpy(protocol1, buffer1,
			(long) tmp1 - (long) buffer1 + 1);
		protocol1[(long) tmp1 - (long) buffer1] = '\0';
	    }

	    if (DEBUG > 1)
		printf("protocol1: %s\n", protocol1);

	    if (tmp1 != NULL) {
		tmp1 = tmp1 + 3;
		while (tmp1[0] != '/') {
		    if (tmp1[0] == '\0')
			break;
		    tmp1++;
		}
	    }
	    tmp2 = strstr(buffer2, "://");
	    if (tmp2 != NULL) {
		protocol2 =
		    (char *) malloc((long) tmp2 - (long) buffer2 + 1);
		strncpy(protocol2, buffer2,
			(long) tmp2 - (long) buffer2 + 1);
		protocol2[(long) tmp2 - (long) buffer2] = '\0';
	    }

	    if (DEBUG > 1)
		printf("protocol2: %s\n", protocol2);

	    if (tmp2 != NULL) {
		tmp2 = tmp2 + 3;
		while (tmp2[0] != '/') {
		    if (tmp2[0] == '\0')
			break;
		    tmp2++;
		}
	    }
	    if (tmp1 != NULL && tmp2 != NULL) {
		if (strcmp(tmp1, tmp2) == 0) {
		    // if either protocol is file:// then they are the same at this point usually one will be NULL and the other file://
		    if ((strncmp(protocol1, "file://", 7) == 0)
			|| (strncmp(protocol2, "file://", 7) == 0)) {
			ret = 0;
		    } else {
			// NetFlix puts out the same preview on multiple protocols, we have to add them all to the playlist
			if (strcmp(protocol1, protocol2) == 0) {
			    ret = 0;
			} else {
			    ret = -1;
			}
		    }
		} else {
		    ret = -1;
		    q1 = strchr(tmp1, '?');
		    q2 = strchr(tmp2, '?');
		    if (q1 != NULL || q2 != NULL) {
			if (q1 != NULL) {
			    q1[0] = '\0';
			    tmp1hasq = 1;
			}
			if (q2 != NULL) {
			    q2[0] = '\0';
			    tmp2hasq = 1;
			}
			if (strcmp(tmp1, tmp2) == 0) {
			    if (tmp1hasq != tmp2hasq) {
				ret = -1;
			    } else {
				if (strcmp(q1 + 1, q2 + 1) == 0) {
				    ret = 0;
				} else {
				    ret = -1;
				}
			    }
			} else {
			    ret = -1;
			}
		    }
		}
	    }
	}
	free(buffer1);
	free(buffer2);

    }
    if (hostname1 != NULL)
	NPN_MemFree(hostname1);
    if (hostname2 != NULL)
	NPN_MemFree(hostname2);
    if (protocol1 != NULL)
	free(protocol1);
    if (protocol2 != NULL)
	free(protocol2);

    if (DEBUG > 1)
	printf("exiting URLcmp\n");

    return ret;

}

// Remove all single quotes from url to prevent remote execution
void remove_quotes(char *url)
{
    char *p;

    if (DEBUG > 1)
	printf("in remove_quotes\n");

    // don't scan the string if it not in there, this should be a bit faster than the while loop

    if (url == NULL)
	return;

    p = strchr(url, '`');

    if (p == NULL) {
	return;
    } else {
	if (DEBUG) {
	    printf("single quotes in url (%s), truncating\n"
		   "WARNING: probably won't work! FIXME!\n", url);
	}
	*p = '\0';
    }
}



//  sendCommand is safe to use only with control_mutex held
//  (and in player thread surounded by cleanup_push/cleanup_pop)

// command will be ignored if js_state is JS_STATE_TRANSITIONING

// in cleanup routines (like destroyCB and shut), when we know that
// the player thread is not running, it is safe to call without locking

int sendCommand(nsPluginInstance * instance, char *command)
{
    int retval;
    char buffer[1024];

    if (DEBUG > 1)
	printf("in sendcommand - command %s\n", command);

    buffer[1023] = '\0';
    retval = 0;

    if (command == NULL || instance == NULL || instance->cancelled == 1)
	return 0;

    if (instance->threadsignaled == 0)
	return 0;

    if (instance->control == -1)
	return 0;


    if (instance->js_state != JS_STATE_TRANSITIONING) {

	snprintf(buffer, 1023, "%s\n", command);

	retval = write(instance->control, buffer, strlen(buffer));
	//operations on pipes are atomic

	if (retval < (int) strlen(buffer)) {
	    if (DEBUG)
		printf("*****sendCommand Truncated*****\n");

	}

    }

    return retval;

}

void killmplayer(nsPluginInstance * instance)
{
    void *thread_return;
    int count, status;

    if (DEBUG > 1)
	printf("in killmplayer\n");

    if (instance->paused == 1)
	sendCommand(instance, "pause\n");
    sendCommand(instance, "quit\n");
#ifndef BSD
    pthread_mutex_lock(&(instance->read_mutex));
#endif
    instance->cancelled = 1;
#ifndef BSD
    pthread_mutex_unlock(&(instance->read_mutex));
#endif

/*    count = 0;
    while (count < 500) {
	if (DEBUG)
	    printf("waiting for quit to be handled\n");
	usleep(100);
	count++;
    	if (instance->player == NULL)
	    break;
    }
*/
    //we can do the following twice on the same thread with no ill effects
    pthread_cancel(instance->player_thread);
    pthread_join(instance->player_thread, &thread_return);
    instance->js_state = JS_STATE_UNDEFINED;

    if (DEBUG)
	printf
	    ("Trying to kill mplayer process(%d), if it still exists\n",
	     instance->pid);

    count = 0;
    while (instance->player != NULL && count < 10) {
	if (DEBUG)
	    printf("waiting for player to go NULL\n");
	usleep(100);
	count++;
    }


    if (instance->player == NULL) {
	instance->pid = 0;
    } else {
	if (DEBUG > 1)
	    printf("closing player\n");
	if (instance->player != NULL) {
            fclose(instance->player);
	}
	instance->player = NULL;

	if (DEBUG > 1)
	    printf("closing control pipe\n");
	if (instance->control > 0) {
	    close(instance->control);
	    instance->control = -1;
	}
    }

    if (DEBUG > 1)
	printf("player should be closed\n");

    instance->threadlaunched = 0;

    if (instance->pid != 0) {
	count = 0;
	status = 1;

	while ((status != 0) && (count < 10)) {
	    status = kill(instance->pid, 15);
	    if (DEBUG)
		printf("kill(15) status = %i\n", status);
	    if (status == -1) {
		if (errno == ESRCH) {
		    status = 0;
		    break;
		}

		usleep(100);
	    }
	    count++;
	}

	if (status != 0) {
	    status = kill(instance->pid, 9);
	    if (DEBUG)
		printf("kill(9) status = %i\n", status);
	    if (status == 0) {
		instance->pid = 0;
	    }
	}
	//wait(&status);
    }
#ifdef DPMSExtension
    if (instance->DPMSEnabled)
	DPMSReenable(instance);
#endif

    if (instance->threadsetup == 1) {
	count = 0;
	while (count < 50) {
	    if (instance->td->argv[count] != NULL)
		free(instance->td->argv[count]);
	    instance->td->argv[count++] = NULL;
	}
	instance->threadsetup = 0;
#ifdef GTK_ENABLED
//      if (GTK_IS_WIDGET(instance->drawing_area)) {
//          gtk_widget_destroy(instance->drawing_area);
//          instance->drawing_area = NULL;
//      }
#endif
    }
}

void fullyQualifyURL(nsPluginInstance * instance, char *initem,
		     char *localitem)
{
    char tmpdir[4096];
    char *tmp;
    char *item;

    if (DEBUG > 1)
	printf("in fullyQualifyURL\n");

    item = strdup(initem);

    tmp = strstr(item, "<");
    if (tmp != NULL) {
	strlcpy(item, tmp + 1, 4096);
	tmp = strstr(item, ">");
	if (DEBUG > 1)
	    printf("item = %p tmp = %p   diff = %i\n", item, tmp,
		   (tmp - item));
	if (tmp != NULL) {
	    strlcpy(tmp, "", 4096);
	}
	if (DEBUG > 1)
	    printf("item = %s\n", item);
    }

    if (DEBUG > 1)
	printf("item: %s\nbaseurl: %s\nhostname: %s\n",
	       item, instance->baseurl, instance->hostname);

    if (!isMms(item, instance->nomediacache)) {
	if ((strncasecmp(item, "http", 4) != 0)
	    && (strncasecmp(item, "file", 4) != 0)) {

	    if (DEBUG > 1)
		printf("not http and not file\n");

	    if (item[0] != '/') {
		strlcpy(tmpdir, item, 4096);	// reuse the buffer
		if (instance->baseurl != NULL) {
		    strlcpy(localitem, instance->baseurl, 4096);
		} else {
		    strlcpy(localitem, "", 4096);
		}
		strlcat(localitem, tmpdir, 4096);
	    } else {
		if (instance->hostname != NULL) {
		    if (fexists(item) == 0) {
			snprintf(tmpdir, 4096, "http://%s%s",
				 instance->hostname, item);
			strlcpy(localitem, tmpdir, 4096);
		    } else {
			// first char == / and hostname is null, sounds like a filename
			strlcpy(localitem, item, 4096);
		    }
		} else {
		    // first char == / and hostname is null, sounds like a filename
		    strlcpy(localitem, item, 4096);
		}
	    }

	} else {

	    // if :80 is in the URL, cut it out
	    strlcpy(localitem, item, 4096);
	    tmp = strstr(localitem, ":8080");
	    if (tmp == NULL) {
		tmp = strstr(localitem, ":80/");
		if (tmp != NULL) {
		    *tmp = '\0';
		    strlcat(localitem, tmp + 3, 4096);
		}
	    }


	    if (strncasecmp(localitem, "file://", 7) == 0) {
		// chop off file://
		strlcpy(tmpdir, localitem, 4096);
		strlcpy(localitem, tmpdir + 7, 4096);
		// replace %20's in url with a space
		if (strstr(localitem, "%20") != NULL) {
		    while ((tmp = strstr(localitem, "%20"))) {
			if (tmp != NULL) {
			    *tmp = ' ';
			    tmp++;
			    *tmp = '\0';
			    strcat(localitem, tmp + 2);
			}
		    }
		}
		// check and see if the file exists, if it does not then prepend smb:// to the filename
		if (fexists(localitem) == 0) {
		    strlcpy(tmpdir, "smb://", 4096);
		    strlcat(tmpdir, localitem, 4096);
		    strlcpy(localitem, tmpdir, 4096);
		}

	    }

	}

    } else {

	strlcpy(localitem, item, 4096);

    }

    free(item);
    if (DEBUG > 1)
	printf("fqu result: %s\n", localitem);

}

int toolkitOk(NPP instance, int *mozilla_toolkit, int *plugin_toolkit)
{

#ifdef X_ENABLED
    *plugin_toolkit = 0;
#endif

#ifdef GTK1_ENABLED
    *plugin_toolkit = 1;
#endif

#ifdef GTK2_ENABLED
    *plugin_toolkit = 2;
#endif

    if (DEBUG)
	printf("checking toolkit\n");
    NPN_GetValue(instance, NPNVToolkit, mozilla_toolkit);

    if (DEBUG)
	printf("toolkitOk? mozilla = %i, plugin = %i\n", *mozilla_toolkit,
	       *plugin_toolkit);

#ifdef OO_GTK2_ENABLED

    // this is an openoffice workaround, but it does not seem to work.
    if (*mozilla_toolkit == 0) {
	if (DEBUG)
	    printf("initializing GDK/GTK\n");
	//g_type_init();
	//gdk_init(NULL,NULL);
	gtk_init(NULL, NULL);
	gtk_main();
    }
#endif

    if (*mozilla_toolkit == 0 || *mozilla_toolkit == 1
	|| *mozilla_toolkit == 2) {

	if (*plugin_toolkit == 0 || *mozilla_toolkit == 0
	    || (*mozilla_toolkit == *plugin_toolkit))
	    return 0;
	else
	    return 1;

    } else {
	printf("Unknown Mozilla toolkit (%i), assuming toolkit is GTK%i\n",
	       *mozilla_toolkit, *plugin_toolkit);
	printf
	    ("This may cause Mozilla to crash. If it crashes recompile mplayerplug-in with a different toolkit.\n");
	return 0;
    }
}

#ifdef DPMSExtension
int DPMSIsEnabled(nsPluginInstance * instance)
{
    int dummy;
    BOOL onoff;
    CARD16 state;

    if (DEBUG > 1)
	printf("Checking if DPMS is enabled\n");

    if (DPMSQueryExtension(instance->display, &dummy, &dummy)) {
	if (DPMSCapable(instance->display))
	    DPMSInfo(instance->display, &state, &onoff);
    }

    if (DEBUG > 1)
	printf("DPMS is enabled = %i\n", onoff);
    return onoff;
}

void DPMSReenable(nsPluginInstance * instance)
{
    int dummy;

    if (DEBUG > 1)
	printf("DPMS is enabled\n");

    if (DPMSQueryExtension(instance->display, &dummy, &dummy)) {
	if (DPMSCapable(instance->display))
	    DPMSEnable(instance->display);
    }
}
#endif
