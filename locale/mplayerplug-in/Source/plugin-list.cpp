#include "plugin.h"
#include <errno.h>
#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif
#include <strings.h>

void cancelBySpeed(nsPluginInstance * instance);

extern int DEBUG;
extern int errno;
// Linked List methods
// This is how the playlist is going to be managed

void initialize(Node * l)
{
    l = NULL;
}

Node *newNode()
{
    Node *n = (Node *) NPN_MemAlloc(sizeof(Node));
    n->url[0] = '\0';
    n->fname[0] = '\0';
    n->next = NULL;
    n->bytes = 0;
    n->play = 1;
    n->playlist = 0;
    n->speed = -1;
    n->mmsstream = 0;
    n->remove = 1;
    n->status = STATE_NEWINSTANCE;
    n->retrieved = 0;
    n->cancelled = 0;
    n->played = 0;
    n->frombutton = 0;
    n->entry = -1;
    n->loop = 0;
    n->bytes = 0;
    n->totalbytes = 0;
    n->cachebytes = 0;
    n->actual_x = 0;
    n->actual_y = 0;
    n->play_x = 0;
    n->play_y = 0;
    n->copy = 0;
    n->prefer_url = 0;
    n->localcache = NULL;
    n->area = NULL;
    n->next = NULL;
    return n;
}

void deleteNode(Node * n)
{
    char *tmp = NULL;

    if (n != NULL) {
	if (n->copy == 0) {
	    if ((n->remove == 1) || (n->remove == 0 && n->play == 0)) {
		if (strlen(n->fname) != 0) {
		    if (strncasecmp(n->fname, "sdp://", 6) == 0) {
			tmp = strdup(n->fname);
			strcpy(n->fname, tmp + 6);
			free(tmp);
		    }
		    remove(n->fname);
		    if (DEBUG)
			printf("fname: %s deleted\n", n->fname);
		}
	    }
	}
	n->url[0] = '\0';
	n->fname[0] = '\0';
	n->bytes = 0;
	n->play = 1;
	n->mmsstream = 0;
	n->remove = 1;
	n->status = STATE_NEWINSTANCE;
	n->retrieved = 0;
	NPN_MemFree(n);
	n = NULL;
    }
}

void copyNode(Node * dest, Node * src)
{
    if (src != NULL && dest != NULL) {
	if (dest->url != NULL && src->url != NULL)
	    strlcpy(dest->url, src->url, 4096);
	if (dest->fname != NULL && src->fname != NULL)
	    strlcpy(dest->fname, src->fname, 1024);
	dest->bytes = src->bytes;
	dest->play = src->play;
	dest->mmsstream = src->mmsstream;
	dest->playlist = src->playlist;
	dest->speed = src->speed;
	dest->remove = src->remove;
	dest->cancelled = src->cancelled;
	dest->played = src->played;
	dest->retrieved = src->retrieved;
	dest->area = src->area;
	dest->copy = 1;
    }
}

void insert_area(Node * parent, char *tagtarget, char *tagurl, int tagtime)
{
    area *runner;
    area *n = (area *) NPN_MemAlloc(sizeof(area));

    strcpy(n->url, tagurl);
    strcpy(n->target, tagtarget);
    n->begin = tagtime;
    n->next = NULL;

    while (parent->next != NULL)
	parent = parent->next;

    if (parent->area == NULL) {
	parent->area = n;
	return;
    }

    runner = parent->area;

    while (runner->next != NULL)
	runner = runner->next;

    runner->next = n;
}

void find_area_tags(const char *smilbuffer, Node * parent)
{
    char *startarea;
    char *endvideo;
    char *start;

    int tagtime = 0;
    char tagtarget[128];
    char tagurl[4096];

    endvideo = strstr((char *)smilbuffer, "</video");
    startarea = strstr((char *)smilbuffer, "<area");

    while (startarea && startarea < endvideo) {
	start = strstr(startarea + 5, "begin=");
	if (start) {
	    start += 6;
	    if (*start == '"')
		++start;
	    tagtime = atoi(start);
	}
	start = strstr(startarea + 5, "target=");
	if (start) {
	    start += 7;
	    if (*start == '"')
		++start;
	    sscanf(start, "%127[^\" \t]", tagtarget);
	}
	start = strstr(startarea + 5, "href=");
	if (start) {
	    start += 5;
	    if (*start == '"')
		++start;
	    sscanf(start, "%1023[^\" \t]", tagurl);
	}

	insert_area(parent, tagtarget, tagurl, tagtime);
	startarea = strstr(startarea + 5, "<area");
    }

}

void deleteList(Node * l)
{
    Node *oldList;

    if (DEBUG)
	printf("Entering deleteList\n");
    while (l != NULL) {
	oldList = l;
	if (DEBUG) {
	    printf("DELETE %s\n", l->url);
	    if (l->remove)
		printf("File to delete %s\n", l->fname);
	    printf("size: %li\n", l->bytes);
	    printf("remove: %i\n", l->remove);
	    printf("play: %i\n", l->play);
	    printf("played: %i\n", l->played);
	    printf("playlist: %i\n", l->playlist);
	    printf("speed: %i\n", l->speed);
	    printf("cancelled: %i\n", l->cancelled);
	    printf("retrieved: %i\n", l->retrieved);
	    printf("entry: %i\n", l->entry);
	    printf("mms stream: %i\n", l->mmsstream);
	    printf("cache bytes: %li\n", l->cachebytes);
	    printf("Actual Size: %ix%i\n", l->actual_x, l->actual_y);
	    printf("Play Size: %ix%i\n", l->play_x, l->play_y);

	}
	l = l->next;
	deleteNode(oldList);
    }

}

/* Inserts node newnode after node item */
void insertafter(Node * item, Node * newnode)
{
    if (item == NULL || newnode == NULL) {
	if (DEBUG)
	    printf("Cannot insert\n");
    } else {
	newnode->next = item->next;
	item->next = newnode;
    }
}

void addToEnd(Node * l, Node * newnode)
{

    if (l == NULL) {
	l = newnode;
    } else {
	while (l->next != NULL)
	    l = l->next;
	l->next = newnode;
    }

}

void addToList(nsPluginInstance * instance, char *item, Node * parent,
	       int speed, int mmsstream, int entry, int loop)
{
    Node *node = NULL;
    Node *n;
    char *filename;
    char localitem[4096];

    fullyQualifyURL(instance, item, localitem);

    n = instance->td->list;
    if (DEBUG)
	printf("BUILD - traversing playlist\n");
    while (n != NULL) {
	if (URLcmp(n->url, localitem) == 0) {
	    if (DEBUG)
		printf("found duplicate entry\n%s\n", n->url);
	    break;
	}
	n = n->next;
    }

    if (n == NULL) {
	// add it in
	if (DEBUG)
	    printf("adding url %s\n", localitem);
	node = newNode();
	strlcpy(node->url, localitem, sizeof(node->url));
	if (speed == -1 && parent->speed > 0) {
	    node->speed = parent->speed;
	} else {
	    node->speed = speed;
	}
	addToEnd(parent, node);
	parent->play = 0;
	if (isMms(item, instance->nomediacache)
	    || strstr(node->url, parent->url) != NULL) {
	    node->mmsstream = 1;
	} else {
	    filename = getURLFilename(localitem);
/*	    if (instance->keep_download == 1 && filename != NULL) {
		snprintf(node->fname, sizeof(node->fname), "%s/%s",
			 instance->download_dir, filename);
		if (DEBUG)
		    printf("Assigned name: %s\n", node->fname);
		node->remove = 0;
	    }
*/
	    if (filename != NULL)
		NPN_MemFree(filename);
	    NPN_GetURLNotify(instance->mInstance, node->url, NULL, NULL);
	}
	if (mmsstream == 1)
	    node->mmsstream = 1;
	node->entry = entry;
        node->loop = loop;

    } else {
	if (n->playlist == 1)
	    n->mmsstream = 1;
	n->entry = entry;
        n->loop = loop;
    }



}

void buildPlaylist(nsPluginInstance * instance, char *file, Node * parent)
{
    FILE *fp;
    char buffer[16 * 1024];
    char buffer_lower[16 * 1024];
    char url[4096];
    unsigned long int size, found, i;
    long int remainder;
    char *p, *entry, *href, *repeat;
    char *nextrmda = NULL;
    char *rdrf;
    char *rmdr;
    char *sub, *refurl;
    char *seqtag, *rate, *videotag, *videosrc;
    unsigned int code;
    int speed, entry_counter = -1, loopentry;
    unsigned char *c1, *c2, *c3, *c4;

    fp = fopen(file, "r");

    found = 0;
    if (fp != NULL) {
	while (!feof(fp)) {
	    memset(buffer, 0, sizeof(buffer));
	    memset(buffer_lower, 0, sizeof(buffer));
	    memset(url, 0, sizeof(url));

	    size = fread(buffer, 1, sizeof(buffer) - 1, fp);
	    buffer[size] = '\0';

	    if (DEBUG > 1)
		printf("READ: %li\n%s\n", size, buffer);

	    // quicktime files have blocks of 4 characters..
	    // rdrf,XXXX,url ,XXXX,url ending with 00

	    if (memmem(buffer, size, "rmda", 4)) {
		if (DEBUG)
		    printf("found QT\n");

		p = buffer;
		// find the first data element
		p = (char *) memmem(p, size, "rmda", 4);
		if (size > 4) {
		    if (p != NULL)
			p = p + 4;
		    nextrmda = (char *) memmem(p, size - 4, "rmda", 4);
		    if (nextrmda == NULL)
			nextrmda = buffer + size;
		}
		refurl = NULL;
		speed = -1;
		code = 0;

		while ((p != NULL)
		       && ((long) p < ((long) buffer + (long) size))) {
		    rdrf =
			(char *) memmem(p,
					size - ((long) nextrmda -
						(long) p), "rdrf", 4);
		    rmdr =
			(char *) memmem(p,
					size - ((long) nextrmda -
						(long) p), "rmdr", 4);

		    if (rdrf != NULL) {
			code = (unsigned int) (rdrf[15]);
			rdrf = rdrf + 16;
			refurl = (char *) rdrf;
			if (DEBUG) {
			    printf("URL: %s\n", (refurl));
			    printf("CODE: %d\n", code);
			}
		    }

		    if (rmdr != NULL) {
			rmdr = rmdr + 8;

			c1 = (unsigned char *) rmdr;
			c2 = (unsigned char *) rmdr + 1;
			c3 = (unsigned char *) rmdr + 2;
			c4 = (unsigned char *) rmdr + 3;
			speed =
			    ((unsigned int) c1[0] << 24) +
			    ((unsigned int) c2[0] << 16) +
			    ((unsigned int) c3[0] << 8) +
			    (unsigned int) c4[0];

			if (parent->speed > speed)
			    speed = parent->speed;
		    }
		    if (refurl != NULL) {
			if (code == 163 || code == 165 || code == 167
			    || code == (unsigned int) -93
			    || code == (unsigned int) -91
			    || code == (unsigned int) -89) {
			    if (DEBUG)
				printf("Skipped URL: %s\n", refurl);
			} else {
			    parent->playlist = 1;
			    addToList(instance, refurl, parent, speed, 0,
				      -1, 0);
			    printf("ADDED URL: %s\n", (refurl));
			    printf("code: %i\n speed %i\n", code, speed);
			    found = 1;
			    refurl = NULL;
			    speed = -1;
			    code = 0;
			}
		    }
		    p = nextrmda + 4;
		    if (p > (buffer + size)) {
			p = NULL;
		    } else {
  		        nextrmda =
			    (char *) memmem(p,
				   	    size - ((long) p - (long) buffer),
					    "rmda", 4);
		        if (nextrmda == NULL)
			    nextrmda = buffer + size;
		    }
		}

		break;		// End QT Reference file
	    }
	    // smil format detection
	    snprintf((char *) buffer_lower, sizeof(buffer_lower), "%s",
		     buffer);
	    lowercase((char *) buffer_lower);
	    p = buffer_lower;
	    if (memmem(p, size, "<smil", 5) && found == 0) {
		while (p != NULL) {
		    speed = -1;
		    remainder = size - ((long) p - (long) buffer_lower);
		    if (remainder < 1)
			break;

		    // seq tags are optional, but if we do have
		    // one see if it has a speed and if so, find it
		    seqtag = (char *) memmem(p, remainder, "<seq", 4);
		    if (seqtag != NULL) {
			if (DEBUG)
			    printf("[smil] found seq tag\n");

			remainder =
			    size - ((long) seqtag - (long) buffer_lower);
			rate =
			    (char *) memmem(seqtag, remainder,
					    "system-bitrate", 14);
			if (rate != NULL) {
			    rate = strchr(rate, '\"');
			    if (rate != NULL)
				rate++;
			    sscanf(rate, "%i", &speed);
			    if (DEBUG)
				printf("[smil] found bit rate %i\n",
				       speed);
			} else {
			    // if we don't find a speed, then set the speed to -1
			    speed = -1;
			    // and reset p so that we can find the video tag
			    seqtag = (char *) p;
			}
		    } else {
			// if we didn't find a seqtag, then set seqtag equal to p
			// so that the next block will work
			seqtag = (char *) p;
		    }
		    // we have a speed now, lets find all the src
		    // files in the seq block
		    videotag =
			(char *) memmem(seqtag, remainder, "<video", 6);

		    // if we don't find "video" see if we find audio
		    if (videotag == NULL)
			videotag =
			    (char *) memmem(seqtag, remainder, "<audio",
					    6);
		    if (videotag != NULL) {
			while (videotag != NULL) {
			    if (DEBUG)
				printf("[smil] found video tag\n");
			    remainder =
				size - ((long) videotag -
					(long) buffer_lower);
			    videosrc =
				(char *) memmem(videotag, remainder, "src",
						3);
			    if (videosrc != NULL) {
				if (strchr(videosrc, '\"') == NULL)
				    videosrc = strchr(videosrc, '\'');
				else
				    videosrc = strchr(videosrc, '\"');

				if (videosrc == NULL) {
				    seqtag = (char *) NULL;
				    p = NULL;
				    videotag = NULL;
				    break;
				}
				i = (long) videosrc - (long) buffer_lower;
				sscanf(buffer + i, "\"%1023[^\"]\"", url);
				if (url == NULL)
				    sscanf(buffer + i, "\'%1023[^\']\'",
					   url);
				if (DEBUG)
				    printf("[smil] found url %s\n", url);
				parent->playlist = 1;
				addToList(instance, url, parent, speed, 0,
					  -1, 0);
				found = 1;
			    } else {
				seqtag = (char *) NULL;
				p = NULL;
				videotag = NULL;
				break;
			    }
			    find_area_tags(videosrc, instance->td->list);

			    videotag =
				(char *) memmem(videosrc, remainder,
						"<video", 6);
			    // if we don't find "video" see if we find audio
			    if (videotag == NULL)
				videotag =
				    (char *) memmem(videosrc,
						    remainder,
						    "<audio", 6);
			    seqtag =
				(char *) memmem(videosrc, remainder,
						"<seq", 4);
			    if (videotag == NULL)
				break;
			    if (seqtag == NULL && videotag != NULL)
				continue;
			    if ((long) videotag > (long) seqtag)
				videotag = NULL;
			}
			p = (char *) seqtag;
		    } else {
			p = (char *) seqtag + 4;	// for the seq block that contains no video or audio tags
		    }
		}
	    }
	    // quicktime media link files
	    snprintf((char *) buffer_lower, sizeof(buffer_lower), "%s",
		     buffer);
	    lowercase((char *) buffer_lower);
	    p = buffer_lower;
	    if (memmem
		(p, size, "quicktime-media-link",
		 strlen("quicktime-media-link")) && found == 0) {
		if (DEBUG)
		    printf("found QuickTime Media Link file\n");
		while (p != NULL) {
		    i = size - ((long) p - (long) buffer_lower);
		    if (i < 1)
			break;
		    p = (char *) memmem(p, i, "src", 3);
		    if (p == NULL)
			break;
		    p = p + 3;
		    p = strchr(p, '\"');
		    if (p != NULL) {
			p++;
			i = (long) p - (long) buffer_lower;
			snprintf(url, 4096, "%s", buffer + i);

			sub = strchr(url, '\"');
			if (sub != NULL) {
			    *sub = '\0';
			    if (DEBUG)
				printf("URL: %s\n", url);
			    parent->playlist = 1;
			    addToList(instance, url, parent, -1, 0, -1, 0);
			    p = p + strlen(url) + 1;
			    found = 1;
			} else {
			    break;
			}
		    }
		}
		break;
	    }
	    // some mp3 files start with ID3, not a reference file
	    // not all MP3 files have the ID tag
	    if (strncasecmp(buffer, "ID3", 3) == 0 && found == 0) {
		if (DEBUG)
		    printf("found MP3\n");
		break;
	    }
	    // asx version 3.0 files
	    // <Ref href = "url" />
	    snprintf((char *) buffer_lower, sizeof(buffer_lower), "%s",
		     buffer);
	    lowercase((char *) buffer_lower);
	    p = buffer_lower;
	    if ((memmem
		 (p, size, "<asx version=\"3.0\">",
		  strlen("<asx version=\"3.0\">"))
		 || memmem(p, size, "<asx version = \"3.0\">",
			   strlen("<asx version = \"3.0\">")))
		&& found == 0) {
                    parent->playlist = 1;
                    parent->prefer_url = 1;
                    found = 1;

/*
		if (DEBUG)
		    printf("found ASX 3.0file\n");
		while (p != NULL) {
		    i = size - ((long) p - (long) buffer_lower);

		    if (i < 1)
			break;
                    repeat = (char *) memmem(p, i, "<repeat", 7);
		    entry = (char *) memmem(p, i, "entry", 5);
		    href = (char *) memmem(p, i, "href", 4);
		    if (entry == NULL || href == NULL) {
			// do nothing
		    } else {
			if (entry < href) {
			    entry_counter =
				highest_entry(instance->list) + 1;
			    if (DEBUG)
				printf("incrementing entry counter\n");
			}
                        loopentry = 0;
                        if (repeat != NULL) {
                            if (repeat < entry && (char *) memmem(p, i, "</repeat", 8) > entry) {
                                loopentry = 1;
                            }
                        }
		    }

		    p = (char *) memmem(p, i, "href", 4);
		    if (p == NULL)
			break;
		    if ((p - 9) > buffer_lower) {
			// look backwards in the file and see if the href is preceeded by
			// a qualifier tag, but don't look too far forward otherwise we may
			// find another tag of this type
			if (memmem(p - 5, 5, "logo", 4) != NULL) {
			    if (DEBUG > 1)
				printf("found logo in ASX\n");
			    p = p + 4;
			    continue;
			}
			if (memmem(p - 7, 7, "banner", 6) != NULL) {
			    if (DEBUG > 1)
				printf("found banner in ASX\n");
			    p = p + 4;
			    continue;
			}
			if (memmem(p - 9, 9, "moreinfo", 8) != NULL) {
			    if (DEBUG > 1)
				printf("found moreinfo in ASX\n");
			    p = p + 4;
			    continue;
			}

			if (memmem(p - 9, 9, "entryref", 8) != NULL) {
			    if (DEBUG > 1)
				printf("found entryref in ASX\n");
			    //p = p + 4;
			    //continue;
			    //entry_counter = highest_entry(instance->list) + 1;
			}

		    }
		    p = p + 4;
		    p = strchr(p, '\"');
		    if (p != NULL) {
			p++;
			i = (long) p - (long) buffer_lower;
			if (i >= size)
			    break;
			snprintf(url, 1024, "%s", buffer + i);

			sub = strchr(url, '\"');
			if (sub != NULL) {
			    *sub = '\0';
			    unEscapeXML(url);
			    if (DEBUG)
				printf("URL: %s\n", url);
			    parent->playlist = 1;
			    addToList(instance, url, parent, -1, 1,
				      entry_counter,loopentry);
			    p = p + strlen(url) + 1;
			    found = 1;
			} else {
			    break;
			}
		    }
		}
*/
		break;
	    }
	    // asx files are XMLish
	    // <Ref href = "url" />
	    snprintf((char *) buffer_lower, sizeof(buffer_lower), "%s",
		     buffer);
	    lowercase((char *) buffer_lower);
	    p = buffer_lower;
	    if (memmem(p, size, "<asx", strlen("<asx")) && found == 0) {
		if (DEBUG)
		    printf("found ASX file\n");
                    parent->playlist = 1;
                    parent->prefer_url = 1;
                    found = 1;
/*
		while (p != NULL) {
		    i = size - ((long) p - (long) buffer_lower);

		    if (i < 1)
			break;
		    p = (char *) memmem(p, i, "href", 4);
		    if (p == NULL)
			break;
		    if ((p - 9) > buffer_lower) {
			// look backwards in the file and see if the href is preceeded by
			// a qualifier tag, but don't look too far forward otherwise we may
			// find another tag of this type
			if (memmem(p - 5, 5, "logo", 4) != NULL) {
			    if (DEBUG > 1)
				printf("found logo in ASX\n");
			    p = p + 4;
			    continue;
			}
			if (memmem(p - 7, 7, "banner", 6) != NULL) {
			    if (DEBUG > 1)
				printf("found banner in ASX\n");
			    p = p + 4;
			    continue;
			}
			if (memmem(p - 9, 9, "moreinfo", 8) != NULL) {
			    if (DEBUG > 1)
				printf("found moreinfo in ASX\n");
			    p = p + 4;
			    continue;
			}
		    }
		    p = p + 4;
		    p = strchr(p, '\"');
		    if (p != NULL) {
			p++;
			i = (long) p - (long) buffer_lower;
			if (i >= size)
			    break;
			snprintf(url, 1024, "%s", buffer + i);

			sub = strchr(url, '\"');
			if (sub != NULL) {
			    *sub = '\0';
			    unEscapeXML(url);
			    if (DEBUG)
				printf("URL: %s\n", url);
			    parent->playlist = 1;
			    addToList(instance, url, parent, -1, 1, -1,0);
			    p = p + strlen(url) + 1;
			    found = 1;
			} else {
			    break;
			}
		    }
		}
*/
		break;
	    }
	    // asf files
	    if (strncasecmp(buffer, "[reference]", 11) == 0 && found == 0) {
		if (DEBUG)
		    printf("found ASF file\n");
		snprintf(buffer_lower, 4096, "%s", buffer);
		lowercase(buffer_lower);
		p = buffer_lower;
		while (p != NULL) {
		    i = size - ((long) p - (long) buffer_lower);
		    if (i < 1)
			break;
		    p = (char *) memmem(p, i, "ref", 3);
		    if (p == NULL)
			break;
		    p = p + 3;
		    p = strchr(p, '=');
		    if (p != NULL) {
			p++;
			i = (long) p - (long) buffer_lower;
			snprintf(url, 4096, "%s", buffer + i);

			sub = strchr(url, '\r');
			if (sub == NULL)
			    sub = strchr(url, '\n');
			if (sub == NULL)
			    sub = strchr(url, EOF);
			if (sub != NULL) {
			    *sub = '\0';
			    if (DEBUG)
				printf("URL: %s\n", url);
			    // with these files they are mirror sites when multiple references exist
			    addToList(instance, url, parent, -1, 0, 0,0);
			    parent->playlist = 1;
			    if (parent->mmsstream == 1
				&& parent->next == NULL)
				parent->playlist = 0;
			    if ((long)
				((long) p - (long) buffer_lower +
				 strlen(url) + 1) < (long) size)
				p = p + strlen(url) + 1;
			    else
				p = NULL;
			    found = 1;
			} else {
			    break;
			}
		    }
		}
		break;
	    }
	    // if we have an HTML file, ignore it and don't process it
	    if (memmem(buffer_lower, size, "<html", 5) != NULL) {
		found = 1;
	    }
	    // we have an NSV file
	    if (memmem(buffer_lower, size, "icy 200 ok", 10) != NULL) {
		parent->mmsstream = 1;
		parent->cancelled = 1;
		found = 1;
	    }
	    // we have an SCP Playlist file
	    if (memmem(buffer_lower, size, "[playlist]", 10) != NULL) {
		parent->mmsstream = 1;
		parent->cancelled = 1;
		found = 1;
	    }
	    // simple playlist usually realmedia file
	    if (memmem(buffer_lower, size, "rtsp://", 7) != NULL && found == 0) {
		if (DEBUG) {
		    printf("Search RTSP File\n");
		}
		p = buffer_lower;
		while (p != NULL) {
		    i = size - ((long) p - (long) buffer_lower);
		    p = (char *) memmem(p, i, "rtsp://", 7);
		    if (p == NULL) {
			break;
		    } else {
			i = (long) p - (long) buffer_lower;
			snprintf(url, 4096, "rtsp://%s", buffer + i + 7);
			sub = strchr(url, 0xd);	// terminate on CR
			if (sub != NULL)
			    sub[0] = '\0';
			sub = strchr(url, 0xa);	// terminate on line feed
			if (sub != NULL)
			    sub[0] = '\0';
			//sub = strchr(url, 0x20);	// terminate on space
			sub = strchr(url, EOF);	// terminate on EOF
			if (sub != NULL)
			    sub[0] = '\0';
			if (DEBUG)
			    printf("URL: %s\n", url);
			parent->playlist = 1;
			addToList(instance, url, parent, -1, 0, -1,0);
			p = p + strlen(url) + 1;
			found = 1;
		    }
		}
	    }
	    // simple playlist usually realmedia file
	    if (memmem(buffer_lower, size, "pnm://", 6) != NULL && found == 0) {
		if (DEBUG) {
		    printf("Search for PNM url File\n");
		}
		p = buffer_lower;
		while (p != NULL) {
		    i = size - ((long) p - (long) buffer_lower);
		    p = (char *) memmem(p, i, "pnm://", 6);
		    if (p == NULL) {
			break;
		    } else {
			i = (long) p - (long) buffer_lower;
			snprintf(url, 4096, "pnm://%s", buffer + i + 6);
			sub = strchr(url, 0xd);	// terminate on CR
			if (sub != NULL)
			    sub[0] = '\0';
			sub = strchr(url, 0xa);	// terminate on line feed
			if (sub != NULL)
			    sub[0] = '\0';
			//sub = strchr(url, 0x20);	// terminate on space
			sub = strchr(url, EOF);	// terminate on EOF
			if (sub != NULL)
			    sub[0] = '\0';
			if (DEBUG)
			    printf("URL: %s\n", url);
			parent->playlist = 1;
			addToList(instance, url, parent, -1, 0, -1,0);
			p = p + strlen(url) + 1;
			found = 1;
		    }
		}
	    }

	    // simple playlist, usually old windows media
	    if ((strncasecmp(buffer, "http://", 7) == 0
		 || memmem(buffer_lower, size, "http://", 7) != NULL)
		&& found == 0
		&& memmem(buffer_lower, size, "<a", 2) == NULL) {
		p = buffer;
		while (p != NULL) {
		    i = size - ((long) p - (long) buffer);
		    p = (char *) memmem(p, i, "http://", 7);
		    if (p == NULL) {
			break;
		    } else {
			snprintf(url, 4096, "%s", p);
			sub = strchr(url, 0xd);
			if (sub != NULL)
			    sub[0] = '\0';
			sub = strchr(url, 0xa);
			if (sub != NULL)
			    sub[0] = '\0';
			sub = strchr(url, 0x20);	// terminate on space
			if (sub != NULL)
			    sub[0] = '\0';
			if (DEBUG)
			    printf("URL: %s\n", url);
			parent->playlist = 1;
			addToList(instance, url, parent, -1, 0, -1,0);
			p = p + strlen(url) + 1;
			found = 1;
		    }
		}
	    }
	    // simple playlist, usually old windows media
	    if (strncasecmp(buffer, "mms://", 6) == 0 && found == 0) {
		p = buffer;
		while (p != NULL) {
		    i = size - ((long) p - (long) buffer);
		    p = (char *) memmem(p, i, "mms://", 6);
		    if (p == NULL) {
			break;
		    } else {
			snprintf(url, 4096, "%s", p);
			sub = strchr(url, 0xd);
			if (sub != NULL)
			    sub[0] = '\0';
			sub = strchr(url, 0xa);
			if (sub != NULL)
			    sub[0] = '\0';
			sub = strchr(url, 0x20);	// terminate on space
			if (sub != NULL)
			    sub[0] = '\0';
			if (DEBUG)
			    printf("URL: %s\n", url);
			parent->playlist = 1;
			addToList(instance, url, parent, -1, 0, -1,0);
			p = p + strlen(url) + 1;
			found = 1;
		    }
		}
	    }

	    if (!found)
		break;
	}
	fclose(fp);

	cancelBySpeed(instance);

    } else {
	printf("Build Playlist error opening file %i : %s\n", errno,
	       strerror(errno));
    }
}

void cancelBySpeed(nsPluginInstance * instance)
{
    Node *n;
    int speed_low, speed_med, speed_high;

    // some QuickTime sites, out all the media in one reference file but speed is different
    // we need to figure out which one to play.
    speed_low = 0;
    speed_med = 0;
    speed_high = 0;

    if (DEBUG)
	printf("Scanning for speed\n");
    n = instance->td->list;
    while (n != NULL) {
	if ((n->speed > 0) && (n->play == 1)) {
	    if (speed_low == 0) {
		speed_low = n->speed;
		speed_med = n->speed;
		speed_high = n->speed;
	    }

	    if ((speed_low == speed_med) && (n->speed > speed_med))
		speed_med = n->speed;

	    if (n->speed < speed_low)
		speed_low = n->speed;

	    if (n->speed > speed_high)
		speed_high = n->speed;

	    if ((n->speed > speed_med) && (n->speed < speed_high))
		speed_med = n->speed;

	    if ((n->speed < speed_high) && (n->speed >= speed_low)
		&& (speed_high == speed_med))
		speed_med = n->speed;
	}
	n = n->next;
    }
    if (DEBUG)
	printf("low = %i, med = %i, high = %i\n", speed_low, speed_med,
	       speed_high);

    n = instance->td->list;
    while (n != NULL) {
	if ((n->speed > 0) && (n->play == 1)) {
	    if (instance->qt_speed == SPEED_LOW) {
		if (n->speed != speed_low) {
		    n->play = 0;
		    n->cancelled = 1;
		}
	    }

	    if (instance->qt_speed == SPEED_MED) {
		if (n->speed != speed_med) {
		    n->play = 0;
		    n->cancelled = 1;
		}
	    }

	    if (instance->qt_speed == SPEED_HIGH) {
		if (n->speed != speed_high) {
		    n->play = 0;
		    n->cancelled = 1;
		}
	    }
	}
	n = n->next;
    }
    if (DEBUG)
	printf("playlist adjustment complete\n");
}

void printNode(Node * l)
{

    printf("url: %s\n", l->url);
    printf("fname %s\n", l->fname);
    printf("bytes: %li\n", l->bytes);
    printf("remove: %i\n", l->remove);
    printf("play: %i\n", l->play);
    printf("playlist: %i\n", l->playlist);
    printf("entry: %i\n", l->entry);
    printf("speed: %i\n", l->speed);
    printf("cancelled: %i\n", l->cancelled);
    printf("retrieved: %i\n", l->retrieved);
    printf("mmsstream: %i\n", l->mmsstream);
    printf("cachebytes: %li\n", l->cachebytes);
    printf("Actual Size: %ix%i\n", l->actual_x, l->actual_y);
    printf("Play Size: %ix%i\n", l->play_x, l->play_y);

}

void printList(Node * l)
{

    Node *current = l;

    printf("Dumping Playlist:\n");
    while (current != NULL) {
	printNode(current);
	printf("^^^^^^^^^\n");
	current = current->next;
    }
}

void markPlayed(Node * l)
{
    Node *n;

    if (DEBUG)
	printf("markPlayed\n");

    if (l->entry == -1) {
	if (DEBUG)
	    printf("marking %s as played\n", l->url);
	l->played = 1;
    } else {
	n = l;
	while (n != NULL) {
	    if (n->entry == l->entry) {
		if (DEBUG)
		    printf("marking %s as played\n", n->url);
		n->played = 1;	// set played flag for all mirror lists
	    }
	    n = n->next;
	}
    }

}

int highest_entry(Node * l)
{

    Node *n;
    int entry = -1;

    if (DEBUG)
	printf("highest_entry\n");

    n = l;
    while (n != NULL) {
	if (n->entry > entry) {
	    entry = n->entry;
	}
	n = n->next;
    }

    return entry;
}
