#include "plugin.h"
#include <sys/stat.h>

#ifndef STATICDECLS
#define STATICDECLS
int DEBUG = 0;
int instance_counter = 0;

#define MAX_BUF_LEN 255
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
#define STATE_GETTING_PLAYLIST 110
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

#endif


char *GetMIMEDescription()
{

    char *ret;
    char MimeTypes[4000], parse[1000], buffer[1000];
    FILE *config;
    int i, use_custom_mime_types;
    int standard;
    int enable_mpeg;
    int enable_ogg;
    int enable_smil;
    int enable_helix;
    int enable_wmp;
    int enable_qt;
    int enable_rm;
    int enable_gmp;
    int enable_dvx;
    int enable_mp3;
    int enable_midi;
    int enable_pls;

#ifdef STD
    FILE *customtypes;
    char customline[255];
#endif
    char config_name[3][1000];

    // load config file

    snprintf(config_name[0], 1000, "/etc/mplayerplug-in.conf");

    snprintf(config_name[1], 1000, "%s", getenv("HOME"));
    strlcat(config_name[1], "/.mozilla/mplayerplug-in.conf", 1000);

    snprintf(config_name[2], 1000, "%s", getenv("HOME"));
    strlcat(config_name[2], "/.mplayer/mplayerplug-in.conf", 1000);

    config = NULL;


    enable_mpeg = 1;
    enable_mp3 = 1;
    enable_ogg = 1;
    enable_smil = 1;
    enable_helix = 1;
    enable_wmp = 1;
    enable_qt = 1;
    enable_rm = 1;
    enable_gmp = 1;
    enable_dvx = 1;
    enable_midi = 0;
    enable_pls = 0;
    use_custom_mime_types = 0;


    DEBUG = 0;			//set to 1 when debugging this function
    if (DEBUG)
	printf("Reading config file for codecs\n");
    // clear the buffer
    for (i = 0; i < 4000; i++) {
	MimeTypes[i] = '\0';
    }

    config = NULL;

    for (i = 0; i < 3; i++) {
	config = fopen(config_name[i], "r");

	if (config == NULL) {
	    // no config file
	} else {
	    while (fgets(buffer, 1000, config) != NULL) {

		if (DEBUG)
		    printf("Buffer: %s\n", buffer);

		// SMIL
		if (strncasecmp(buffer, "enable-smil", 11) == 0) {
		    snprintf(parse, 1000, "%s", strtok(buffer, "="));
		    snprintf(parse, 1000, "%s", strtok(NULL, "="));
		    sscanf(parse, "%i", &enable_smil);
		    if (DEBUG)
			printf("real:%i\n", enable_smil);
		    continue;
		}
		// Helix RPM MimeType
		if (strncasecmp(buffer, "enable-helix", 12) == 0) {
		    snprintf(parse, 1000, "%s", strtok(buffer, "="));
		    snprintf(parse, 1000, "%s", strtok(NULL, "="));
		    sscanf(parse, "%i", &enable_helix);
		    if (DEBUG)
			printf("helix:%i\n", enable_helix);
		    continue;
		}
		// MPEG
		if (strncasecmp(buffer, "enable-mpeg", 11) == 0) {
		    snprintf(parse, 1000, "%s", strtok(buffer, "="));
		    snprintf(parse, 1000, "%s", strtok(NULL, "="));
		    sscanf(parse, "%i", &enable_mpeg);
		    if (DEBUG)
			printf("mpeg:%i\n", enable_mpeg);
		    continue;
		}
		// Ogg Vorbis
		if (strncasecmp(buffer, "enable-ogg", 10) == 0) {
		    snprintf(parse, 1000, "%s", strtok(buffer, "="));
		    snprintf(parse, 1000, "%s", strtok(NULL, "="));
		    sscanf(parse, "%i", &enable_ogg);
		    if (DEBUG)
			printf("ogg:%i\n", enable_ogg);
		    continue;
		}

		if (strncasecmp(buffer, "use-mimetypes", 13) == 0) {
		    snprintf(parse, 1000, "%s", strtok(buffer, "="));
		    snprintf(parse, 1000, "%s", strtok(NULL, "="));
		    sscanf(parse, "%i", &use_custom_mime_types);
		    if (DEBUG)
			printf("custom mimetypes:%i\n",
			       use_custom_mime_types);
		    continue;
		}
		// Windows Media Player
		if (strncasecmp(buffer, "enable-wmp", 10) == 0) {
		    snprintf(parse, 1000, "%s", strtok(buffer, "="));
		    snprintf(parse, 1000, "%s", strtok(NULL, "="));
		    sscanf(parse, "%i", &enable_wmp);
		    if (DEBUG)
			printf("wmp:%i\n", enable_wmp);
		    continue;
		}
		// QuickTime
		if (strncasecmp(buffer, "enable-qt", 9) == 0) {
		    snprintf(parse, 1000, "%s", strtok(buffer, "="));
		    snprintf(parse, 1000, "%s", strtok(NULL, "="));
		    sscanf(parse, "%i", &enable_qt);
		    if (DEBUG)
			printf("qt:%i\n", enable_qt);
		    continue;
		}
		// RealMedia
		if (strncasecmp(buffer, "enable-rm", 9) == 0) {
		    snprintf(parse, 1000, "%s", strtok(buffer, "="));
		    snprintf(parse, 1000, "%s", strtok(NULL, "="));
		    sscanf(parse, "%i", &enable_rm);
		    if (DEBUG)
			printf("rm:%i\n", enable_rm);
		    continue;
		}
		// Google Media Player
		if (strncasecmp(buffer, "enable-gmp", 10) == 0) {
		    snprintf(parse, 1000, "%s", strtok(buffer, "="));
		    snprintf(parse, 1000, "%s", strtok(NULL, "="));
		    sscanf(parse, "%i", &enable_gmp);
		    if (DEBUG)
			printf("gmp:%i\n", enable_gmp);
		    continue;
		}
		// Divx Media Player
		if (strncasecmp(buffer, "enable-dvx", 10) == 0) {
		    snprintf(parse, 1000, "%s", strtok(buffer, "="));
		    snprintf(parse, 1000, "%s", strtok(NULL, "="));
		    sscanf(parse, "%i", &enable_dvx);
		    if (DEBUG)
			printf("dvx:%i\n", enable_dvx);
		    continue;
		}
		// MP3
		if (strncasecmp(buffer, "enable-mp3", 10) == 0) {
		    snprintf(parse, 1000, "%s", strtok(buffer, "="));
		    snprintf(parse, 1000, "%s", strtok(NULL, "="));
		    sscanf(parse, "%i", &enable_mp3);
		    if (DEBUG)
			printf("mp3:%i\n", enable_mp3);
		    continue;
		}
		// MIDI
		if (strncasecmp(buffer, "enable-midi", 11) == 0) {
		    snprintf(parse, 1000, "%s", strtok(buffer, "="));
		    snprintf(parse, 1000, "%s", strtok(NULL, "="));
		    sscanf(parse, "%i", &enable_midi);
		    if (DEBUG)
			printf("midi:%i\n", enable_midi);
		    continue;
		}
		// PLS
		if (strncasecmp(buffer, "enable-pls", 10) == 0) {
		    snprintf(parse, 1000, "%s", strtok(buffer, "="));
		    snprintf(parse, 1000, "%s", strtok(NULL, "="));
		    sscanf(parse, "%i", &enable_pls);
		    if (DEBUG)
			printf("pls:%i\n", enable_pls);
		    continue;
		}

	    }
	    fclose(config);
	}
    }

#ifdef STD
    standard = 1;
#endif
#ifndef STD
    standard = 0;
#endif

    if (use_custom_mime_types == 1 && standard == 1) {

#ifdef STD
	customtypes = NULL;

	if (customtypes == NULL) {
	    snprintf(buffer, 1000, "%s", getenv("HOME"));
	    strlcat(buffer, "/.mplayer/mplayerplug-in.types", 1000);
	    customtypes = fopen(buffer, "r");
	}

	if (customtypes == NULL) {
	    snprintf(buffer, 1000, "%s", getenv("HOME"));
	    strlcat(buffer, "/.mozilla/mplayerplug-in.types", 1000);
	    config = fopen(buffer, "r");
	}

	if (customtypes == NULL) {
	    customtypes = fopen("/etc/mplayerplug-in.types", "r");
	}

	if (customtypes != NULL) {
	    while (fgets(customline, sizeof(customline), customtypes)) {
		if (customline[0] != '\0' && customline[0] != '#'
		    && customline[0] != '\n')
		    strlcat(MimeTypes, customline, sizeof(MimeTypes));
	    }
	    fclose(customtypes);
	}
#endif

    } else {

#ifdef QT
	if (enable_qt) {
	    strlcat(MimeTypes,
		    "video/quicktime:mov:Quicktime;"
		    "video/x-quicktime:mov:Quicktime;"
		    "image/x-quicktime:mov:Quicktime;"
		    "video/quicktime:mp4:Quicktime;"
		    "video/quicktime:sdp:Quicktime - Session Description Protocol;"
		    "application/x-quicktimeplayer:mov:Quicktime;",
		    sizeof(MimeTypes));

	    if (enable_smil)
		strlcat(MimeTypes,
			"application/smil:smil:SMIL;", sizeof(MimeTypes));
	}
#endif

#ifdef WMP
	if (enable_wmp) {
	    strlcat(MimeTypes,
		    "application/asx:*:Media Files;"
		    "video/x-ms-asf-plugin:*:Media Files;"
		    "video/x-msvideo:avi,*:AVI;"
		    "video/msvideo:avi,*:AVI;"
		    "application/x-mplayer2:*:Media Files;"
		    "application/x-ms-wmv:wmv,*:Microsoft WMV video;"
		    "video/x-ms-asf:asf,asx,*:Media Files;"
		    "video/x-ms-wm:wm,*:Media Files;"
		    "video/x-ms-wmv:wmv,*:Microsoft WMV video;"
		    "audio/x-ms-wmv:wmv,*:Windows Media;"
		    "video/x-ms-wmp:wmp,*:Windows Media;"
		    "application/x-ms-wmp:wmp,*:Windows Media;"
		    "video/x-ms-wvx:wvx,*:Windows Media;"
		    "audio/x-ms-wax:wax,*:Windows Media;"
		    "audio/x-ms-wma:wma,*:Windows Media;"
		    "application/x-drm-v2:asx,*:Windows Media;"
		    "audio/wav:wav,*:Microsoft wave file;"
		    "audio/x-wav:wav,*:Microsoft wave file;",
		    sizeof(MimeTypes));
	}
#endif

#ifdef RM
	if (enable_rm) {
	    strlcat(MimeTypes,
		    "audio/x-pn-realaudio:ram,rm:RealAudio;"
		    "application/vnd.rn-realmedia:rm:RealMedia;"
		    "application/vnd.rn-realaudio:ra,ram:RealAudio;"
		    "video/vnd.rn-realvideo:rv:RealVideo;"
		    "audio/x-realaudio:ra:RealAudio;", sizeof(MimeTypes));

	    if (enable_helix)
		strlcat(MimeTypes,
			"audio/x-pn-realaudio-plugin:rpm:RealAudio;",
			sizeof(MimeTypes));

	    if (enable_smil)
		strlcat(MimeTypes,
			"application/smil:smil:SMIL;", sizeof(MimeTypes));
	}
#endif

#ifdef STD
	if (enable_mpeg) {
	    strlcat(MimeTypes,
		    "video/mpeg:mpg,mpeg:MPEG;"
		    "audio/mpeg:mpg,mpeg:MPEG;"
		    "video/x-mpeg:mpg,mpeg:MPEG;"
		    "video/x-mpeg2:mpv2,mp2ve:MPEG2;"
		    "audio/mpeg:mpg,mpeg:MPEG;"
		    "audio/x-mpeg:mpg,mpeg:MPEG;"
		    "audio/mpeg2:mp2:MPEG audio;"
		    "audio/x-mpeg2:mp2:MPEG audio;"
		    "audio/mp4:mp4:MPEG 4 audio;"
		    "audio/x-mp4:mp4:MPEG 4 audio;"
		    "video/mp4:mp4:MPEG 4 Video;"
		    "video/3gpp:mp4,3gp:MPEG 4 Video;", sizeof(MimeTypes));

	    if (enable_mp3)
		strlcat(MimeTypes,
			"audio/mpeg3:mp3:MPEG audio;"
			"audio/x-mpeg3:mp3:MPEG audio;"
			"audio/x-mpegurl:m3u:MPEG url;"
			"audio/mp3:mp3:MPEG audio;", sizeof(MimeTypes));

	}

	if (enable_ogg) {
	    strlcat(MimeTypes,
		    "application/x-ogg:ogg:Ogg Vorbis Media;"
		    "audio/ogg:ogg:Ogg Vorbis Audio;"
		    "audio/x-ogg:ogg:Ogg Vorbis Audio;"
		    "application/ogg:ogg:Ogg Vorbis / Ogg Theora;",
		    sizeof(MimeTypes));
	}
	// FLAC
	strlcat(MimeTypes,
		"audio/flac:flac:FLAC Audio;"
		"audio/x-flac:flac:FLAC Audio;", sizeof(MimeTypes));

	// FLI
	strlcat(MimeTypes,
		"video/fli:fli,flc:FLI animation;"
		"video/x-fli:fli,flc:FLI animation;", sizeof(MimeTypes));

	// FLV
	strlcat(MimeTypes, "video/x-flv:flv:Flash Video;",
		sizeof(MimeTypes));

	// Vivo
	strlcat(MimeTypes, "video/vnd.vivo:viv,vivo:VivoActive;",
		sizeof(MimeTypes));

	// NSV
	strlcat(MimeTypes,
		"application/x-nsv-vp3-mp3:nsv:Nullsoft Streaming Video;",
		sizeof(MimeTypes));

	// Soundtracker
	strlcat(MimeTypes,
		"audio/x-mod:mod:Soundtracker;", sizeof(MimeTypes));

	// Basic
	strlcat(MimeTypes,
		"audio/basic:au,snd:Basic Audio File;"
		"audio/x-basic:au,snd:Basic Audio File;",
		sizeof(MimeTypes));

	// MIDI
	if (enable_midi) {
	    strlcat(MimeTypes,
		    "audio/midi:mid,midi,kar:MIDI Audio;",
		    sizeof(MimeTypes));
	}
	// PLS
	if (enable_pls) {
	    strlcat(MimeTypes,
		    "audio/x-scpls:pls:Shoutcast Playlist;",
		    sizeof(MimeTypes));
	}
#endif
	// Google Video
#ifdef GMP
	if (enable_gmp) {
	    strlcat(MimeTypes,
		    "application/x-google-vlc-plugin::Google Video;",
		    sizeof(MimeTypes));
	}
#endif

#ifdef DVX
	// DIVX
	if (enable_dvx) {
	    strlcat(MimeTypes,
		    "video/divx:divx:DivX Media Format;"
		    "video/vnd.divx:divx:DivX Media Format;",
		    sizeof(MimeTypes));
	}
#endif


    }
    ret = strdup(MimeTypes);
    if (DEBUG)
	printf("%s\n", ret);
    DEBUG = 0;
    return ret;
}

NPError GetValue(NPPVariable variable, void *value)
{

    NPError err = NPERR_NO_ERROR;

    DEBUG = 0;			// set to 1 when debugging this function

    // some sites use this description to figure out what formats can be played. So we have to make sure the 
    // description matches the features

    if (variable == NPPVpluginNameString) {
#ifdef STD
	*((const char **) value) = "mplayerplug-in " PACKAGE_VERSION;
#endif
#ifdef WMP
	*((const char **) value) = "Windows Media Player Plug-in";
#endif

#ifdef QT
	*((const char **) value) = "QuickTime Plug-in 7.4.5";
#endif

#ifdef RM
	*((const char **) value) = "RealPlayer 9";
#endif

#ifdef GMP
	*((const char **) value) = "Google VLC multimedia plugin 1.0";
#endif

#ifdef DVX
	*((const char **) value) = "DivX Browser Plug-In";
#endif

    }
    if (variable == NPPVpluginDescriptionString) {
#ifdef GTK2_ENABLED
	*((const char **) value) =
	    "<a href=\"http://mplayerplug-in.sourceforge.net/\">mplayerplug-in</a> "
	    PACKAGE_VERSION
	    "<br><br>Video Player Plug-in for QuickTime, RealPlayer and Windows Media Player streams using <a href=\"http://mplayerhq.hu\">MPlayer</a> <br>JavaScript Enabled and Using GTK2 Widgets<br>";
#endif
#ifdef GTK1_ENABLED
	*((const char **) value) =
	    "<a href=\"http://mplayerplug-in.sourceforge.net/\">mplayerplug-in</a> "
	    PACKAGE_VERSION
	    "<br><br>Video Player Plug-in for QuickTime, RealPlayer and Windows Media Player streams using <a href=\"http://mplayerhq.hu\">MPlayer</a> <br>JavaScript Enabled and Using GTK1 Widgets<br>";
#endif
#ifdef X_ENABLED
	*((const char **) value) =
	    "<a href=\"http://mplayerplug-in.sourceforge.net/\">mplayerplug-in</a> "
	    PACKAGE_VERSION
	    "<br><br>Video Player Plug-in for QuickTime, RealPlayer and Windows Media Player streams using <a href=\"http://mplayerhq.hu\">MPlayer</a> <br>JavaScript Enabled and Using X Widgets<br>";
#endif

    }

    if (variable == NPPVpluginNeedsXEmbed) {
#ifdef GTK2_ENABLED
	*((PRBool *) value) = PR_FALSE;
#endif
#ifdef GTK1_ENABLED
	*((PRBool *) value) = PR_TRUE;
#endif
    }

    if ((variable != NPPVpluginNameString)
	&& (variable != NPPVpluginDescriptionString)
	&& (variable != NPPVpluginNeedsXEmbed)) {
	err = NPERR_GENERIC_ERROR;
    }

    DEBUG = 0;
    return err;

}

void New(nsPluginInstance * instance, nsPluginCreateData * parameters)
{
    int i;
    int real_master_console = 0;
    char parse[1000];
    char *cp;
    char *tmp;

    if (DEBUG)
	printf("mimetype: %s\n", parameters->type);

    instance->mode = parameters->mode;
    instance->mInstance = parameters->instance;
    instance->mimetype = strdup(parameters->type);

/*
    if ((strstr(instance->mimetype, "mpeg") != NULL)
	|| (strstr(instance->mimetype, "mp3") != NULL)) {
	instance->cache_percent = 100;
    }
*/
    if (parameters->mode == NP_EMBED) {

	if (DEBUG)
	    printf("Embedded mode\n");
	for (i = 0; i < parameters->argc; i++) {

	    if (DEBUG) {
		printf("Argument Name: %s\n", parameters->argn[i]);
		printf("Argument Value: %s\n", parameters->argv[i]);
	    }

	    if (parameters->argn[i] == NULL || parameters->argv[i] == NULL) {
		// break;
		// NOTE: for compatability reasons, the NPAPI places a fake
		// parameter with a null value in the list between parameters
		// set in the actual <embed> or <object> node and ones set as
		// <param> nodes
		continue;	    
            }

	    if (strncasecmp(parameters->argn[i], "debug", 5) == 0) {
		lowercase(parameters->argv[i]);

		if (strstr(parameters->argv[i], "true")
		    || strstr(parameters->argv[i], "yes")
		    || strstr(parameters->argv[i], "1")) {
		    DEBUG = 1;
		}
	    }

	    if (strncasecmp(parameters->argn[i], "nocache", 7) == 0) {
		lowercase(parameters->argv[i]);

		if (strstr(parameters->argv[i], "true")
		    || strstr(parameters->argv[i], "yes")
		    || strstr(parameters->argv[i], "1")) {
		    instance->nomediacache = 1;
		}
		if (DEBUG)
		    printf
			("No media cache is set to %i in the embed tag\n",
			 instance->nomediacache);
	    }

	    if (strncasecmp(parameters->argn[i], "src", 3) == 0) {
		if (instance->fname != NULL) {
		    if (strcmp(instance->fname, parameters->argv[i]) != 0) {
			instance->url = strdup(parameters->argv[i]);
			if (strncasecmp(parameters->argv[i], "file://", 7)
			    == 0)
			    fullyQualifyURL(instance, parameters->argv[i],
					    instance->url);
			instance->state = STATE_HAVEURL;
		    }
		} else {
		    instance->url = strdup(parameters->argv[i]);
		    if (strncasecmp(parameters->argv[i], "file://", 7) ==
			0)
			fullyQualifyURL(instance, parameters->argv[i],
					instance->url);
		    instance->state = STATE_HAVEURL;
		}
	    }

	    if (strncasecmp(parameters->argn[i], "type", 4) == 0) {
		if (instance->mimetype != NULL)
		    free(instance->mimetype);
		instance->mimetype = strdup(parameters->argv[i]);
	    }

	    if ((strncasecmp(parameters->argn[i], "filename", 8) == 0)
		|| (strncasecmp(parameters->argn[i], "url", 3) == 0)
		|| (strncasecmp(parameters->argn[i], "location", 8) == 0)) {
		if (instance->url != NULL) {
		    if (strcmp(instance->url, parameters->argv[i]) != 0) {
			instance->fname = strdup(parameters->argv[i]);
			if (strncasecmp(parameters->argv[i], "file://", 7)
			    == 0)
			    fullyQualifyURL(instance, parameters->argv[i],
					    instance->fname);
			instance->state = STATE_HAVEURL;
		    }
		} else {
		    instance->fname = strdup(parameters->argv[i]);
		    if (strncasecmp(parameters->argv[i], "file://", 7) ==
			0)
			fullyQualifyURL(instance, parameters->argv[i],
					instance->fname);
		    instance->state = STATE_HAVEURL;
		}
	    }

	    if (strncasecmp(parameters->argn[i], "href", 4) == 0) {

		instance->href = strdup(parameters->argv[i]);
		if (strncasecmp(parameters->argv[i], "file://", 7) == 0)
		    fullyQualifyURL(instance, parameters->argv[i],
				    instance->href);
		if (strstr(parameters->argv[i], "<") != NULL)
		    fullyQualifyURL(instance, parameters->argv[i],
				    instance->href);
		instance->state = STATE_HAVEURL;
	    }

	    if (strncasecmp(parameters->argn[i], "qtsrc", 5) == 0) {
                instance->href = (char *)malloc(4096);
                tmp = rindex(instance->url,'/');
                if (tmp) {
                    instance->href = strcpy(instance->href,instance->url);
                    tmp = rindex(instance->href,'/');
                    tmp[1] = '\0';
		    instance->href = strncat(instance->href,parameters->argv[i],1024);
                } else {
		    instance->href = strcpy(instance->href,parameters->argv[i]);
                }
		if (strncasecmp(parameters->argv[i], "file://", 7) == 0)
		    fullyQualifyURL(instance, parameters->argv[i],
				    instance->href);
		if (strstr(parameters->argv[i], "<") != NULL)
		    fullyQualifyURL(instance, parameters->argv[i],
				    instance->href);
		instance->state = STATE_HAVEURL;
	    }

            if (strncasecmp(parameters->argn[i], "autohref", 8) == 0) {

		lowercase(parameters->argv[i]);
		if (DEBUG)
		    printf("argv[i]=%s\n", parameters->argv[i]);

		if (strstr(parameters->argv[i], "true")
		    || strstr(parameters->argv[i], "yes")
		    || strstr(parameters->argv[i], "1")) {
		    instance->autohref = 1;
		} else {
		    instance->autohref = 0;
		}
		if (DEBUG)
		    printf("autohref=%i\n", instance->autohref);
	    }

	    if (strncasecmp(parameters->argn[i], "height", 6) == 0) {
		if (strstr(parameters->argv[i], "%") == NULL)
		    sscanf(parameters->argv[i], "%i",
			   &instance->embed_height);
	    }

	    if (strncasecmp(parameters->argn[i], "width", 5) == 0) {
		if (strstr(parameters->argv[i], "%") == NULL)
		    sscanf(parameters->argv[i], "%i",
			   &instance->embed_width);
	    }

	    if (strncasecmp(parameters->argn[i], "starttime", 9) == 0) {
		sscanf(parameters->argv[i], "%li", &instance->starttime);
	    }

	    if (strncasecmp(parameters->argn[i], "hidden", 6) == 0) {

		lowercase(parameters->argv[i]);
		if (DEBUG)
		    printf("argv[i]=%s\n", parameters->argv[i]);

		if (strstr(parameters->argv[i], "true")
		    || strstr(parameters->argv[i], "yes")
		    || strstr(parameters->argv[i], "1")) {
		    instance->hidden = 1;
		} else {
		    instance->hidden = 0;
		}
		if (DEBUG)
		    printf("hidden=%i\n", instance->hidden);
	    }

	    if (strncasecmp
		(parameters->argn[i], "nopauseonhide",
		 strlen("nopauseonhide")) == 0) {

		lowercase(parameters->argv[i]);
		if (DEBUG)
		    printf("argv[i]=%s\n", parameters->argv[i]);

		if (strstr(parameters->argv[i], "true")
		    || strstr(parameters->argv[i], "yes")
		    || strstr(parameters->argv[i], "1")) {
		    instance->nopauseonhide = 1;
		} else {
		    instance->nopauseonhide = 0;
		}
		if (DEBUG)
		    printf("hidden=%i\n", instance->hidden);
	    }

	    if (strncasecmp
		(parameters->argn[i], "displaysize",
		 strlen("displaysize")) == 0) {

		lowercase(parameters->argv[i]);
		if (DEBUG)
		    printf("argv[i]=%s\n", parameters->argv[i]);

		if (strstr(parameters->argv[i], "true")
		    || strstr(parameters->argv[i], "yes")
		    || strstr(parameters->argv[i], "1")) {
		    instance->dontparent = 1;
		} else {
		    instance->dontparent = 0;
		}
		if (DEBUG)
		    printf("targetplayer=%i\n", instance->targetplayer);
	    }
	    // target is used by Google Video to specifiy the URL to be played
	    // so we might have to do something here.
	    if (strncasecmp(parameters->argn[i], "target", 6) == 0) {
		if (strncasecmp(parameters->argv[i], "quicktimeplayer", 15)
		    == 0) {
#ifdef GTK2_ENABLED
		    instance->targetplayer = 1;
#endif
#ifndef GTK2_ENABLED
		    instance->targetplayer = 0;
		    instance->noembed = 1;
#endif
		}
#ifdef GMP
		instance->fname = strdup(parameters->argv[i]);
		if (strncasecmp(parameters->argv[i], "file://", 7) == 0)
		    fullyQualifyURL(instance, parameters->argv[i],
				    instance->url);
		instance->state = STATE_HAVEURL;

#endif
	    }

	    /* handle 'scale' attribute used by QT instance */
	    if (strncasecmp(parameters->argn[i], "scale", 5) == 0) {
		if (strncasecmp(parameters->argv[i], "aspect", 6) == 0) {
		    instance->maintain_aspect = 1;
		}
	    }

	    if ((strncasecmp(parameters->argn[i], "loop", 4) == 0)
		|| (strncasecmp(parameters->argn[i], "autorewind", 10)
		    == 0)
		|| (strncasecmp(parameters->argn[i], "repeat", 6) == 0)) {

		lowercase(parameters->argv[i]);
		if (DEBUG)
		    printf("argv[i]=%s\n", parameters->argv[i]);

		if (strstr(parameters->argv[i], "true")
		    || strstr(parameters->argv[i], "yes")
		    || strstr(parameters->argv[i], "infinite")) {
		    instance->loop = 0;	// 0 = infinite (like mplayer)
		} else if (isdigit((int) *(parameters->argv[i]))) {
		    sscanf(parameters->argv[i], "%i", &instance->loop);
		    if (instance->loop == 0)
			instance->loop = -1;
		} else {
		    instance->loop = -1;	// -1 = loop disabled
		}
		if (DEBUG)
		    printf("loop=%i\n", instance->loop);
	    }

	    if ((strncasecmp(parameters->argn[i], "autostart", 9) == 0)
		|| (strncasecmp(parameters->argn[i], "autoplay", 8) == 0)) {

		lowercase(parameters->argv[i]);
		if (DEBUG)
		    printf("argv[i]=%s\n", parameters->argv[i]);

		if (strstr(parameters->argv[i], "true")
		    || strstr(parameters->argv[i], "yes")
		    || strstr(parameters->argv[i], "1")) {
		    // instance->autostart = 1;
		} else {
		    instance->autostart = 0;
		}
		if (DEBUG)
		    printf("autostart=%i\n", instance->autostart);
	    }

	    if ((strncasecmp(parameters->argn[i], "showcontrols", 12)
		 == 0)
		|| ((strncasecmp(parameters->argn[i], "controls", 8) == 0)
		    && (strstr(instance->mimetype, "quicktime") != NULL))) {

		lowercase(parameters->argv[i]);
		if (DEBUG)
		    printf("argv[i]=%s\n", parameters->argv[i]);

		if (strstr(parameters->argv[i], "true")
		    || strstr(parameters->argv[i], "yes")
		    || strstr(parameters->argv[i], "1")) {
		    instance->showcontrols = 1;
		} else {
		    instance->showcontrols = 0;
		}
		if (DEBUG)
		    printf("showcontrols=%i\n", instance->showcontrols);
	    }

	    if (strncasecmp(parameters->argn[i], "showtracker", 11) == 0) {

		lowercase(parameters->argv[i]);
		if (DEBUG)
		    printf("argv[i]=%s\n", parameters->argv[i]);

		if (strstr(parameters->argv[i], "true")
		    || strstr(parameters->argv[i], "yes")
		    || strstr(parameters->argv[i], "1")) {
		    instance->showtracker = 1;
		} else {
		    instance->showtracker = 0;
		}
		if (DEBUG)
		    printf("showtracker=%i\n", instance->showtracker);
	    }

	    if (strncasecmp(parameters->argn[i], "showbuttons", 11) == 0) {

		lowercase(parameters->argv[i]);
		if (DEBUG)
		    printf("argv[i]=%s\n", parameters->argv[i]);

		if (strstr(parameters->argv[i], "true")
		    || strstr(parameters->argv[i], "yes")
		    || strstr(parameters->argv[i], "1")) {
		    instance->showbuttons = 1;
		} else {
		    instance->showbuttons = 0;
		}
		if (DEBUG)
		    printf("showbuttons=%i\n", instance->showbuttons);
	    }

	    if (strncasecmp(parameters->argn[i], "showfsbutton", 12) == 0) {

		lowercase(parameters->argv[i]);
		if (DEBUG)
		    printf("argv[i]=%s\n", parameters->argv[i]);

		if (strstr(parameters->argv[i], "true")
		    || strstr(parameters->argv[i], "yes")
		    || strstr(parameters->argv[i], "1")) {
		    instance->showfsbutton = 1;
		} else {
		    instance->showfsbutton = 0;
		}
		if (DEBUG)
		    printf("showfsbutton=%i\n", instance->showfsbutton);
	    }

	    if (strncasecmp(parameters->argn[i], "showlogo", 8) == 0) {

		lowercase(parameters->argv[i]);
		if (DEBUG)
		    printf("argv[i]=%s\n", parameters->argv[i]);

		if (strstr(parameters->argv[i], "false")
		    || strstr(parameters->argv[i], "no")
		    || strstr(parameters->argv[i], "0")) {
		    instance->showlogo = 0;
		} else {
		    instance->showlogo = 1;
		}
		if (DEBUG)
		    printf("showlogo=%i\n", instance->showlogo);
	    }

	    if (strncasecmp(parameters->argn[i], "enablecontextmenu", 8) ==
		0) {

		lowercase(parameters->argv[i]);
		if (DEBUG)
		    printf("argv[i]=%s\n", parameters->argv[i]);

		if (strstr(parameters->argv[i], "false")
		    || strstr(parameters->argv[i], "no")
		    || strstr(parameters->argv[i], "0")) {
		    instance->enablecontextmenu = 0;
		} else {
		    instance->enablecontextmenu = 1;
		}
		if (DEBUG)
		    printf("enablecontextmenu=%i\n",
			   instance->enablecontextmenu);
	    }

	    if ((strncasecmp(parameters->argn[i], "controls", 8) == 0)
		&& (strstr(instance->mimetype, "quicktime") == NULL)) {

		lowercase(parameters->argv[i]);
		if (DEBUG)
		    printf("argv[i]=%s\n", parameters->argv[i]);

		if (strstr(parameters->argv[i], "statusfield")
		    || strstr(parameters->argv[i], "statuspanel")
		    || strstr(parameters->argv[i], "playbutton")
		    || strstr(parameters->argv[i], "volumeslider")
		    || strstr(parameters->argv[i], "stopbutton")
		    || strstr(parameters->argv[i], "positionslider")) {
		    instance->controlwindow = 1;
		} else {
		    instance->controlwindow = 0;
		}
		if (strstr(parameters->argv[i], "controlpanel")
		    || strstr(parameters->argv[i], "statusbar")) {
		    instance->showcontrols = 1;
		    if (instance_counter > 1)
			instance->controlwindow = 1;
		}
		if (strstr(parameters->argv[i], "imagewindow")
		    || strstr(parameters->argv[i], "true")
		    || strstr(parameters->argv[i], "yes")
		    || strstr(parameters->argv[i], "1")
		    || strstr(parameters->argv[i], "all")) {
		    instance->controlwindow = 0;
		}
		if (DEBUG)
		    printf("controlwindow=%i\n", instance->controlwindow);
	    }

	    if ((strncasecmp(parameters->argn[i], "console", 7) == 0)
		&& (strstr(instance->mimetype, "quicktime") == NULL)) {

		lowercase(parameters->argv[i]);
		if (DEBUG)
		    printf("argv[i]=%s\n", parameters->argv[i]);

		if (strstr(parameters->argv[i], "_master")) {
		    real_master_console = 1;
		} else {
		    real_master_console = 0;
		}

	    }

	    if ((strncasecmp
		 (parameters->argn[i], "onmediacomplete", 15) == 0)
		|| (strncasecmp(parameters->argn[i], "onendofstream", 13)
		    == 0)) {
		instance->mediaCompleteCallback =
		    (char *) NPN_MemAlloc(strlen(parameters->argv[i]) +
					  12);
		if (strncasecmp(parameters->argv[i], "javascript:", 11)
		    == 0) {
		    snprintf(instance->mediaCompleteCallback,
			     strlen(parameters->argv[i]), "%s",
			     parameters->argv[i]);
		} else {
		    snprintf(instance->mediaCompleteCallback,
			     strlen(parameters->argv[i]) + 12,
			     "javascript:%s", parameters->argv[i]);
		}
		if (DEBUG)
		    printf("mediaCompleteCallback=%s\n",
			   instance->mediaCompleteCallback);
	    }

	    if (strncasecmp
		(parameters->argn[i], "onmediacompletewitherror",
		 24) == 0) {
		instance->mediaCompleteWithErrorCallback =
		    (char *) NPN_MemAlloc(strlen(parameters->argv[i]) +
					  12);
		if (strncasecmp(parameters->argv[i], "javascript:", 11)
		    == 0) {
		    snprintf(instance->mediaCompleteWithErrorCallback,
			     strlen(parameters->argv[i]), "%s",
			     parameters->argv[i]);
		} else {
		    snprintf(instance->mediaCompleteWithErrorCallback,
			     strlen(parameters->argv[i]) + 12,
			     "javascript:%s", parameters->argv[i]);
		}
		if (DEBUG)
		    printf("mediaCompleteWithErrorCallback=%s\n",
			   instance->mediaCompleteWithErrorCallback);
	    }

	    if (strncasecmp(parameters->argn[i], "onclick", 7) == 0) {
		instance->mouseClickCallback =
		    (char *) NPN_MemAlloc(strlen(parameters->argv[i]) +
					  12);
		if (strncasecmp(parameters->argv[i], "javascript:", 11)
		    == 0) {
		    snprintf(instance->mouseClickCallback,
			     strlen(parameters->argv[i]), "%s",
			     parameters->argv[i]);
		} else {
		    snprintf(instance->mouseClickCallback,
			     strlen(parameters->argv[i]) + 12,
			     "javascript:%s", parameters->argv[i]);
		}
		if (DEBUG)
		    printf("mouseClickCallback=%s\n",
			   instance->mouseClickCallback);
	    }

	    if (strncasecmp(parameters->argn[i], "onmousedown", 11) == 0) {
		instance->mouseDownCallback =
		    (char *) NPN_MemAlloc(strlen(parameters->argv[i]) +
					  12);
		if (strncasecmp(parameters->argv[i], "javascript:", 11)
		    == 0) {
		    snprintf(instance->mouseDownCallback,
			     strlen(parameters->argv[i]), "%s",
			     parameters->argv[i]);
		} else {
		    snprintf(instance->mouseDownCallback,
			     strlen(parameters->argv[i]) + 12,
			     "javascript:%s", parameters->argv[i]);
		}
		if (DEBUG)
		    printf("mouseDownCallback=%s\n",
			   instance->mouseDownCallback);
	    }

	    if (strncasecmp(parameters->argn[i], "onmouseup", 9) == 0) {
		instance->mouseUpCallback =
		    (char *) NPN_MemAlloc(strlen(parameters->argv[i]) +
					  12);
		if (strncasecmp(parameters->argv[i], "javascript:", 11)
		    == 0) {
		    snprintf(instance->mouseUpCallback,
			     strlen(parameters->argv[i]), "%s",
			     parameters->argv[i]);
		} else {
		    snprintf(instance->mouseUpCallback,
			     strlen(parameters->argv[i]) + 12,
			     "javascript:%s", parameters->argv[i]);
		}
		if (DEBUG)
		    printf("mouseUpCallback=%s\n",
			   instance->mouseUpCallback);
	    }

	    if (strncasecmp(parameters->argn[i], "onmouseover", 11) == 0) {
		instance->mouseEnterCallback =
		    (char *) NPN_MemAlloc(strlen(parameters->argv[i]) +
					  12);
		if (strncasecmp(parameters->argv[i], "javascript:", 11)
		    == 0) {
		    snprintf(instance->mouseEnterCallback,
			     strlen(parameters->argv[i]), "%s",
			     parameters->argv[i]);
		} else {
		    snprintf(instance->mouseEnterCallback,
			     strlen(parameters->argv[i]) + 12,
			     "javascript:%s", parameters->argv[i]);
		}
		if (DEBUG)
		    printf("mouseEnterCallback=%s\n",
			   instance->mouseEnterCallback);
	    }

	    if (strncasecmp(parameters->argn[i], "onmouseout", 10) == 0) {
		instance->mouseLeaveCallback =
		    (char *) NPN_MemAlloc(strlen(parameters->argv[i]) +
					  12);
		if (strncasecmp(parameters->argv[i], "javascript:", 11)
		    == 0) {
		    snprintf(instance->mouseLeaveCallback,
			     strlen(parameters->argv[i]), "%s",
			     parameters->argv[i]);
		} else {
		    snprintf(instance->mouseLeaveCallback,
			     strlen(parameters->argv[i]) + 12,
			     "javascript:%s", parameters->argv[i]);
		}
		if (DEBUG)
		    printf("mouseLeaveCallback=%s\n",
			   instance->mouseLeaveCallback);
	    }

	    if (strncasecmp
		(parameters->argn[i], "onvisible",
		 strlen("onvisible")) == 0) {
		instance->onVisibleCallback =
		    (char *) NPN_MemAlloc(strlen(parameters->argv[i]) +
					  12);
		if (strncasecmp(parameters->argv[i], "javascript:", 11)
		    == 0) {
		    snprintf(instance->onVisibleCallback,
			     strlen(parameters->argv[i]), "%s",
			     parameters->argv[i]);
		} else {
		    snprintf(instance->onVisibleCallback,
			     strlen(parameters->argv[i]) + 12,
			     "javascript:%s", parameters->argv[i]);
		}
		if (DEBUG)
		    printf("onVisibleCallback=%s\n",
			   instance->onVisibleCallback);
	    }

	    if (strncasecmp
		(parameters->argn[i], "onhidden",
		 strlen("onhidden")) == 0) {
		instance->onHiddenCallback =
		    (char *) NPN_MemAlloc(strlen(parameters->argv[i]) +
					  12);
		if (strncasecmp(parameters->argv[i], "javascript:", 11)
		    == 0) {
		    snprintf(instance->onHiddenCallback,
			     strlen(parameters->argv[i]), "%s",
			     parameters->argv[i]);
		} else {
		    snprintf(instance->onHiddenCallback,
			     strlen(parameters->argv[i]) + 12,
			     "javascript:%s", parameters->argv[i]);
		}
		if (DEBUG)
		    printf("onHiddenCallback=%s\n",
			   instance->onHiddenCallback);
	    }

	    if (strncasecmp
		(parameters->argn[i], "ondestroy",
		 strlen("ondestroy")) == 0) {
		instance->onDestroyCallback =
		    (char *) NPN_MemAlloc(strlen(parameters->argv[i]) +
					  12);
		if (strncasecmp(parameters->argv[i], "javascript:", 11)
		    == 0) {
		    snprintf(instance->onDestroyCallback,
			     strlen(parameters->argv[i]), "%s",
			     parameters->argv[i]);
		} else {
		    snprintf(instance->onDestroyCallback,
			     strlen(parameters->argv[i]) + 12,
			     "javascript:%s", parameters->argv[i]);
		}
		if (DEBUG)
		    printf("onDestroyCallback=%s\n",
			   instance->onDestroyCallback);
	    }

	    if (strncasecmp
		(parameters->argn[i], "tvdriver",
		 strlen("tvdriver")) == 0) {
		instance->tv_driver = strdup(parameters->argv[i]);
		if (DEBUG)
		    printf("tv_driver=%s\n", instance->tv_driver);
	    }

	    if (strncasecmp
		(parameters->argn[i], "tvdevice",
		 strlen("tvdevice")) == 0) {
		instance->tv_device = strdup(parameters->argv[i]);
		if (DEBUG)
		    printf("tv_device=%s\n", instance->tv_device);
	    }

	    if (strncasecmp
		(parameters->argn[i], "tvinput", strlen("tvinput")) == 0) {
		instance->tv_input = strdup(parameters->argv[i]);
		if (DEBUG)
		    printf("tv_input=%s\n", instance->tv_input);
	    }

	    if (strncasecmp
		(parameters->argn[i], "tvheight",
		 strlen("tvheight")) == 0) {
		if (strstr(parameters->argv[i], "%") == NULL)
		    sscanf(parameters->argv[i], "%i",
			   &instance->tv_height);
	    }

	    if (strncasecmp
		(parameters->argn[i], "tvwidth", strlen("tvwidth")) == 0) {
		if (strstr(parameters->argv[i], "%") == NULL)
		    sscanf(parameters->argv[i], "%i", &instance->tv_width);
	    }

	    if (strncasecmp
		(parameters->argn[i], "tvoutfmt",
		 strlen("tvoutfmt")) == 0) {
		instance->tv_outfmt = strdup(parameters->argv[i]);
		if (DEBUG)
		    printf("tv_outfmt=%s\n", instance->tv_outfmt);
	    }

	    if (instance->nQtNext < 256
		&& (strncasecmp(parameters->argn[i], "qtnext", 6) == 0)
		&& parameters->argv[i][0] == '<') {
		snprintf(parse, 1000, "%s",
			 strtok(&parameters->argv[i][1], ">"));
		if ((cp = strchr(parse, ' '))
		    && strlen(parse) == (unsigned int) (cp - parse + 1))
		    *cp = (char) NULL;
		instance->qtNext[instance->nQtNext++] = strdup(parse);
		snprintf(parse, 1000, "%s", strtok(NULL, "<"));
		if (strcmp(parse, "T")) {
		    if (DEBUG)
			printf
			    ("qtNext%i expected \"T\" found \"%s\"\n",
			     instance->nQtNext, parse);
		    instance->nQtNext--;
		} else {
		    snprintf(parse, 1000, "%s", strtok(NULL, ">\n"));
		    if (strcmp(parse, "myself")) {
			if (DEBUG)
			    printf
				("qtNext%i expected \"myself\" found \"%s\"\n",
				 instance->nQtNext, parse);
			instance->nQtNext--;
		    } else if (DEBUG)
			printf("qtNext%i=%s\n",
			       instance->nQtNext,
			       instance->qtNext[instance->nQtNext - 1]);
		}
	    }
	}
	if (instance->controlwindow == 1 && real_master_console == 1)
	    instance->controlwindow = 0;

	if (instance->controlwindow == 0) {
	    if ((instance->fname != NULL)
		&& (!isMms(instance->fname, instance->nomediacache))) {
		NPN_GetURL(parameters->instance, instance->fname, NULL);
	    }
	}

	if (instance->targetplayer == 1 && instance->href == NULL) {
	    instance->targetplayer = 0;
	}

    } else {
	if (DEBUG)
	    printf("New, full mode %i\n", instance->mode);
    }

    return;

}

void LoadConfigFile(nsPluginInstance * instance)
{

    FILE *config;
    int i;
    char buffer[1000];
    char parse[1000];
    char config_name[3][1000];

    // load config file

    snprintf(config_name[0], 1000, "/etc/mplayerplug-in.conf");

    snprintf(config_name[1], 1000, "%s", getenv("HOME"));
    strlcat(config_name[1], "/.mozilla/mplayerplug-in.conf", 1000);

    snprintf(config_name[2], 1000, "%s", getenv("HOME"));
    strlcat(config_name[2], "/.mplayer/mplayerplug-in.conf", 1000);

    config = NULL;

    for (i = 0; i < 3; i++) {
	config = fopen(config_name[i], "r");

	if (config == NULL) {
	    // no config file
	} else {
	    while (fgets(buffer, sizeof(buffer), config) != NULL) {
		if ((strncasecmp(buffer, "cachesize", 9) == 0)
		    || (strncasecmp(buffer, "cachemin", 8) == 0)) {
		    snprintf(parse, 1000, "%s", strtok(buffer, "="));
		    snprintf(parse, 1000, "%s", strtok(NULL, "="));
		    sscanf(parse, "%i", &instance->cachesize);
		    if (instance->cachesize < 0)
			instance->cachesize = 0;
		    if (instance->cachesize > 65535)
			instance->cachesize = 65535;
		    continue;
		}

		if (strncasecmp(buffer, "debug", 5) == 0) {
		    snprintf(parse, 1000, "%s", strtok(buffer, "="));
		    snprintf(parse, 1000, "%s", strtok(NULL, "="));
		    sscanf(parse, "%i", &DEBUG);
//              if (DEBUG != 0)
//                  DEBUG = 1;
		    continue;
		}

		if (strncasecmp(buffer, "showlogo", 8) == 0) {
		    snprintf(parse, 1000, "%s", strtok(buffer, "="));
		    snprintf(parse, 1000, "%s", strtok(NULL, "="));
		    sscanf(parse, "%i", &instance->showlogo);
		    if (instance->showlogo != 0)
			instance->showlogo = 1;
		    continue;
		}

		if (strncasecmp(buffer, "showtime", 8) == 0) {
		    snprintf(parse, 1000, "%s", strtok(buffer, "="));
		    snprintf(parse, 1000, "%s", strtok(NULL, "="));
		    sscanf(parse, "%i", &instance->showtime);
		    if (instance->showtime != 0)
			instance->showtime = 1;
		    continue;
		}

		if (strncasecmp(buffer, "hidestatus", 10) == 0) {
		    snprintf(parse, 1000, "%s", strtok(buffer, "="));
		    snprintf(parse, 1000, "%s", strtok(NULL, "="));
		    sscanf(parse, "%i", &instance->hidestatus);
		    if (instance->hidestatus != 0)
			instance->hidestatus = 1;
		    continue;
		}

		if (strncasecmp(buffer, "showstatus", 10) == 0) {
		    snprintf(parse, 1000, "%s", strtok(buffer, "="));
		    snprintf(parse, 1000, "%s", strtok(NULL, "="));
		    // crappy reuse of the hidestatus variable
		    sscanf(parse, "%i", &instance->hidestatus);
		    // flip the logic here
		    if (instance->hidestatus == 0) {
			instance->hidestatus = 1;
		    } else {
			instance->hidestatus = 0;
		    }
		    continue;
		}

		if (strncasecmp(buffer, "showtracker", 11) == 0) {
		    snprintf(parse, 1000, "%s", strtok(buffer, "="));
		    snprintf(parse, 1000, "%s", strtok(NULL, "="));
		    sscanf(parse, "%i", &instance->showtracker);
		    if (instance->showtracker != 0)
			instance->showtracker = 1;
		    continue;
		}

		if (strncasecmp(buffer, "showcontrols", 12) == 0) {
		    snprintf(parse, 1000, "%s", strtok(buffer, "="));
		    snprintf(parse, 1000, "%s", strtok(NULL, "="));
		    sscanf(parse, "%i", &instance->showcontrols);
		    if (instance->showcontrols != 0)
			instance->showcontrols = 1;
		    continue;
		}

		if (strncasecmp(buffer, "novop", 5) == 0) {
		    snprintf(parse, 1000, "%s", strtok(buffer, "="));
		    snprintf(parse, 1000, "%s", strtok(NULL, "="));
		    sscanf(parse, "%i", &instance->novop);
		    if (instance->novop != 0)
			instance->novop = 1;
		    continue;
		}

		if (strncasecmp(buffer, "noembed", 7) == 0) {
		    snprintf(parse, 1000, "%s", strtok(buffer, "="));
		    snprintf(parse, 1000, "%s", strtok(NULL, "="));
		    sscanf(parse, "%i", &instance->noembed);
		    if (instance->noembed != 0)
			instance->noembed = 1;
		    continue;
		}

		if (strncasecmp(buffer, "nomediacache", 12) == 0) {
		    snprintf(parse, 1000, "%s", strtok(buffer, "="));
		    snprintf(parse, 1000, "%s", strtok(NULL, "="));
		    sscanf(parse, "%i", &instance->nomediacache);
		    if (instance->nomediacache != 0)
			instance->nomediacache = 1;
		    if (DEBUG)
			printf
			    ("No media cache is set to %i in conf file\n",
			     instance->nomediacache);

		    continue;
		}


		if (strncasecmp(buffer, "vopopt", 6) == 0) {
		    snprintf(parse, 1000, "%s", strtok(buffer, "="));
		    snprintf(parse, 1000, "%s", strtok(NULL, "\n"));
		    instance->novop = 0;
		    if (instance->vop != NULL)
			free(instance->vop);
		    instance->vop = strdup(parse);
		    continue;
		}

		if (strncasecmp(buffer, "af", 2) == 0) {
		    snprintf(parse, 1000, "%s", strtok(buffer, "="));
		    snprintf(parse, 1000, "%s", strtok(NULL, "\n"));
		    if (instance->af != NULL)
			free(instance->af);
		    instance->af = strdup(parse);
		    continue;
		}

		if (strncasecmp(buffer, "prefer-aspect", 13) == 0) {
		    snprintf(parse, 1000, "%s", strtok(buffer, "="));
		    snprintf(parse, 1000, "%s", strtok(NULL, "="));
		    sscanf(parse, "%i", &instance->maintain_aspect);
		    if (instance->maintain_aspect != 0)
			instance->maintain_aspect = 1;
		    continue;
		}

		if (strncasecmp(buffer, "rtsp-use-tcp", 12) == 0) {
		    snprintf(parse, 1000, "%s", strtok(buffer, "="));
		    snprintf(parse, 1000, "%s", strtok(NULL, "="));
		    sscanf(parse, "%i", &instance->rtsp_use_tcp);
		    if (instance->rtsp_use_tcp != 0)
			instance->rtsp_use_tcp = 1;
		    continue;
		}

		if (strncasecmp(buffer, "rtsp-use-http", 13) == 0) {
		    snprintf(parse, 1000, "%s", strtok(buffer, "="));
		    snprintf(parse, 1000, "%s", strtok(NULL, "="));
		    sscanf(parse, "%i", &instance->rtsp_use_http);
		    if (instance->rtsp_use_http != 0)
			instance->rtsp_use_http = 1;
		    continue;
		}

		if (strncasecmp(buffer, "qt-speed", 8) == 0) {
		    sprintf(parse, "%s", strtok(buffer, "="));
		    sprintf(parse, "%s", strtok(NULL, "="));
		    if (strncasecmp(parse, "low", 3) == 0)
			instance->qt_speed = SPEED_LOW;
		    if (strncasecmp(parse, "medium", 6) == 0)
			instance->qt_speed = SPEED_MED;
		    if (strncasecmp(parse, "high", 4) == 0)
			instance->qt_speed = SPEED_HIGH;
		    if (DEBUG)
			printf("QT Speed: %i\n", instance->qt_speed);
		    continue;
		}

		if (strncasecmp(buffer, "vo", 2) == 0) {
		    snprintf(parse, 1000, "%s", strtok(buffer, "="));
		    snprintf(parse, 1000, "%s", strtok(NULL, "=\n"));
		    if (instance->vo != NULL)
			free(instance->vo);
		    if (strstr(parse, "`") == NULL)	// don't allow shell commands
			instance->vo = strdup(parse);
		    continue;
		}

		if (strncasecmp(buffer, "ao", 2) == 0) {
		    snprintf(parse, 1000, "%s", strtok(buffer, "="));
		    snprintf(parse, 1000, "%s", strtok(NULL, "\n"));
		    if (instance->ao != NULL)
			free(instance->ao);
		    if (strstr(parse, "`") == NULL)	// don't allow shell commands
			instance->ao = strdup(parse);
		    continue;
		}

		if (strncasecmp(buffer, "display", 7) == 0) {
		    snprintf(parse, 1000, "%s", strtok(buffer, "="));
		    snprintf(parse, 1000, "%s", strtok(NULL, "=\n"));
		    if (instance->output_display != NULL)
			free(instance->output_display);
		    if (strstr(parse, "`") == NULL)	// don't allow shell commands
			instance->output_display = strdup(parse);
		    continue;
		}

		if (strncasecmp(buffer, "dload-dir", 9) == 0) {
		    snprintf(parse, 1000, "%s", strtok(buffer, "="));
		    snprintf(parse, 1000, "%s", strtok(NULL, "=\n"));
		    if (strstr(parse, "$HOME") != NULL) {
			snprintf(buffer, sizeof(buffer), "%s%s",
				 getenv("HOME"), parse + 5);
			strlcpy(parse, buffer, sizeof(parse));
		    }
		    if (instance->download_dir != NULL)
			free(instance->download_dir);
		    instance->download_dir = strdup(parse);
		    mkdir(instance->download_dir, 0777);
		    continue;
		}

		if (strncasecmp(buffer, "keep-download", 13) == 0) {
		    snprintf(parse, 1000, "%s", strtok(buffer, "="));
		    snprintf(parse, 1000, "%s", strtok(NULL, "="));
		    sscanf(parse, "%i", &instance->keep_download);
		    if (instance->keep_download != 0)
			instance->keep_download = 1;
		    continue;
		}

		if (strncasecmp(buffer, "framedrop", 9) == 0) {
		    snprintf(parse, 1000, "%s", strtok(buffer, "="));
		    snprintf(parse, 1000, "%s", strtok(NULL, "="));
		    sscanf(parse, "%i", &instance->framedrop);
		    if (instance->framedrop != 0)
			instance->framedrop = 1;
		    continue;
		}

		if (strncasecmp(buffer, "autosync", 8) == 0) {
		    snprintf(parse, 1000, "%s", strtok(buffer, "="));
		    snprintf(parse, 1000, "%s", strtok(NULL, "="));
		    sscanf(parse, "%i", &instance->autosync);
		    if (instance->autosync < 0)
			instance->autosync = 0;
		    continue;
		}

		if ((strncasecmp(buffer, "autoplay", 8) == 0)
		    || (strncasecmp(buffer, "autostart", 9) == 0)) {
		    snprintf(parse, 1000, "%s", strtok(buffer, "="));
		    snprintf(parse, 1000, "%s", strtok(NULL, "="));
		    sscanf(parse, "%i", &instance->autostart);
		    if (instance->autostart < 0)
			instance->autostart = 0;
		    continue;
		}

		if (strncasecmp(buffer, "mc", 2) == 0) {
		    snprintf(parse, 1000, "%s", strtok(buffer, "="));
		    snprintf(parse, 1000, "%s", strtok(NULL, "="));
		    sscanf(parse, "%i", &instance->mc);
		    if (instance->mc < 0)
			instance->mc = 0;
		    continue;
		}

		if (strncasecmp(buffer, "black-background", 16) == 0) {
		    snprintf(parse, 1000, "%s", strtok(buffer, "="));
		    snprintf(parse, 1000, "%s", strtok(NULL, "="));
		    sscanf(parse, "%i", &instance->black_background);
		    if (instance->black_background != 0)
			instance->black_background = 1;
		    continue;
		}

		if (strncasecmp(buffer, "nomouseinput", 12) == 0) {
		    snprintf(parse, 1000, "%s", strtok(buffer, "="));
		    snprintf(parse, 1000, "%s", strtok(NULL, "="));
		    sscanf(parse, "%i", &instance->nomouseinput);
		    if (instance->nomouseinput != 0)
			instance->nomouseinput = 1;
		    continue;
		}

		if (strncasecmp(buffer, "noconsolecontrols", 17) == 0) {
		    snprintf(parse, 1000, "%s", strtok(buffer, "="));
		    snprintf(parse, 1000, "%s", strtok(NULL, "="));
		    sscanf(parse, "%i", &instance->noconsolecontrols);
		    if (instance->noconsolecontrols != 0)
			instance->noconsolecontrols = 1;
		    continue;
		}

		if (strncasecmp(buffer, "cookies", 7) == 0) {
		    snprintf(parse, 1000, "%s", strtok(buffer, "="));
		    snprintf(parse, 1000, "%s", strtok(NULL, "="));
		    sscanf(parse, "%i", &instance->cookies);
		    if (instance->cookies != 0)
			instance->cookies = 1;
		    continue;
		}

		if (strncasecmp(buffer, "softvol", strlen("softvol")) == 0) {
		    snprintf(parse, 1000, "%s", strtok(buffer, "="));
		    snprintf(parse, 1000, "%s", strtok(NULL, "="));
		    sscanf(parse, "%i", &instance->softvol);
		    if (instance->softvol != 0)
			instance->softvol = 1;
		    continue;
		}

		if (strncasecmp
		    (buffer, "nopauseonhide",
		     strlen("nopauseonhide")) == 0) {
		    snprintf(parse, 1000, "%s", strtok(buffer, "="));
		    snprintf(parse, 1000, "%s", strtok(NULL, "="));
		    sscanf(parse, "%i", &instance->nopauseonhide);
		    if (instance->nopauseonhide != 0)
			instance->nopauseonhide = 1;
		    continue;
		}

		if (strncasecmp(buffer, "osdlevel", 8) == 0) {
		    snprintf(parse, 1000, "%s", strtok(buffer, "="));
		    snprintf(parse, 1000, "%s", strtok(NULL, "="));
		    sscanf(parse, "%i", &instance->osdlevel);
		    if (instance->osdlevel < 0)
			instance->osdlevel = 0;
		    if (instance->osdlevel > 3)
			instance->osdlevel = 3;
		}

		if (strncasecmp(buffer, "cache-percent", 13) == 0) {
		    snprintf(parse, 1000, "%s", strtok(buffer, "="));
		    snprintf(parse, 1000, "%s", strtok(NULL, "="));
		    sscanf(parse, "%i", &instance->cache_percent);
		    if (instance->cache_percent < 0)
			instance->cache_percent = 0;
		    if (instance->cache_percent > 100)
			instance->cache_percent = 100;
		}

		if (strncasecmp(buffer, "user-agent", 10) == 0) {
		    snprintf(parse, 1000, "%s", strtok(buffer, "="));
		    snprintf(parse, 1000, "%s", strtok(NULL, "=\n"));
		    if (instance->useragent != NULL)
			free(instance->useragent);
		    if (strstr(parse, "`") == NULL)	// don't allow shell commands
			instance->useragent = strdup(parse);
		    continue;
		}

		if (strncasecmp(buffer, "tv-driver", strlen("tv-driver"))
		    == 0) {
		    snprintf(parse, 1000, "%s", strtok(buffer, "="));
		    snprintf(parse, 1000, "%s", strtok(NULL, "=\n"));
		    if (instance->tv_driver != NULL)
			free(instance->tv_driver);
		    if (strstr(parse, "`") == NULL)	// don't allow shell commands
			instance->tv_driver = strdup(parse);
		    continue;
		}

		if (strncasecmp(buffer, "tv-input", strlen("tv-input")) ==
		    0) {
		    snprintf(parse, 1000, "%s", strtok(buffer, "="));
		    snprintf(parse, 1000, "%s", strtok(NULL, "=\n"));
		    if (instance->tv_input != NULL)
			free(instance->tv_input);
		    if (strstr(parse, "`") == NULL)	// don't allow shell commands
			instance->tv_input = strdup(parse);
		    continue;
		}

		if (strncasecmp(buffer, "tv-height", strlen("tv-height"))
		    == 0) {
		    snprintf(parse, 1000, "%s", strtok(buffer, "="));
		    snprintf(parse, 1000, "%s", strtok(NULL, "="));
		    sscanf(parse, "%i", &instance->tv_height);
		    if (instance->tv_height < 0)
			instance->tv_height = 0;
		    continue;
		}

		if (strncasecmp(buffer, "tv-width", strlen("tv-width")) ==
		    0) {
		    snprintf(parse, 1000, "%s", strtok(buffer, "="));
		    snprintf(parse, 1000, "%s", strtok(NULL, "="));
		    sscanf(parse, "%i", &instance->tv_width);
		    if (instance->tv_width < 0)
			instance->tv_width = 0;
		    continue;
		}

		if (strncasecmp(buffer, "tv-outfmt", strlen("tv-outfmt"))
		    == 0) {
		    snprintf(parse, 1000, "%s", strtok(buffer, "="));
		    snprintf(parse, 1000, "%s", strtok(NULL, "=\n"));
		    if (instance->tv_outfmt != NULL)
			free(instance->tv_outfmt);
		    if (strstr(parse, "`") == NULL)	// don't allow shell commands
			instance->tv_outfmt = strdup(parse);
		    continue;
		}

		if (strncasecmp(buffer, "profile", strlen("profile"))
		    == 0) {
		    snprintf(parse, 1000, "%s", strtok(buffer, "="));
		    snprintf(parse, 1000, "%s", strtok(NULL, "=\n"));
		    if (instance->profile != NULL)
			free(instance->profile);
		    if (strstr(parse, "`") == NULL)	// don't allow shell commands
			instance->profile = strdup(parse);
		    continue;
		}
		// SMIL
		if (strncasecmp(buffer, "enable-smil", 11) == 0) {
		    snprintf(parse, 1000, "%s", strtok(buffer, "="));
		    snprintf(parse, 1000, "%s", strtok(NULL, "="));
		    sscanf(parse, "%i", &instance->enable_smil);
		    if (DEBUG)
			printf("smil:%i\n", instance->enable_smil);
		    continue;
		}
		// Helix RPM MimeType
		if (strncasecmp(buffer, "enable-helix", 12) == 0) {
		    snprintf(parse, 1000, "%s", strtok(buffer, "="));
		    snprintf(parse, 1000, "%s", strtok(NULL, "="));
		    sscanf(parse, "%i", &instance->enable_helix);
		    if (DEBUG)
			printf("helix:%i\n", instance->enable_helix);
		    continue;
		}
		// Windows Media Player
		if (strncasecmp(buffer, "enable-wmp", 10) == 0) {
		    snprintf(parse, 1000, "%s", strtok(buffer, "="));
		    snprintf(parse, 1000, "%s", strtok(NULL, "="));
		    sscanf(parse, "%i", &instance->enable_wmp);
		    if (DEBUG)
			printf("wmp:%i\n", instance->enable_wmp);
		    continue;
		}
		// QuickTime
		if (strncasecmp(buffer, "enable-qt", 9) == 0) {
		    snprintf(parse, 1000, "%s", strtok(buffer, "="));
		    snprintf(parse, 1000, "%s", strtok(NULL, "="));
		    sscanf(parse, "%i", &instance->enable_qt);
		    if (DEBUG)
			printf("qt:%i\n", instance->enable_qt);
		    continue;
		}
		// RealMedia
		if (strncasecmp(buffer, "enable-rm", 9) == 0) {
		    snprintf(parse, 1000, "%s", strtok(buffer, "="));
		    snprintf(parse, 1000, "%s", strtok(NULL, "="));
		    sscanf(parse, "%i", &instance->enable_rm);
		    if (DEBUG)
			printf("rm:%i\n", instance->enable_rm);
		    continue;
		}
		// Google Media Player
		if (strncasecmp(buffer, "enable-gmp", 10) == 0) {
		    snprintf(parse, 1000, "%s", strtok(buffer, "="));
		    snprintf(parse, 1000, "%s", strtok(NULL, "="));
		    sscanf(parse, "%i", &instance->enable_gmp);
		    if (DEBUG)
			printf("gmp:%i\n", instance->enable_gmp);
		    continue;
		}
		// MP3
		if (strncasecmp(buffer, "enable-mp3", 10) == 0) {
		    snprintf(parse, 1000, "%s", strtok(buffer, "="));
		    snprintf(parse, 1000, "%s", strtok(NULL, "="));
		    sscanf(parse, "%i", &instance->enable_mp3);
		    if (DEBUG)
			printf("mp3:%i\n", instance->enable_mp3);
		    continue;
		}
		// MIDI
		if (strncasecmp(buffer, "enable-midi", 11) == 0) {
		    snprintf(parse, 1000, "%s", strtok(buffer, "="));
		    snprintf(parse, 1000, "%s", strtok(NULL, "="));
		    sscanf(parse, "%i", &instance->enable_midi);
		    if (DEBUG)
			printf("midi:%i\n", instance->enable_midi);
		    continue;
		}
		// PLS
		if (strncasecmp(buffer, "enable-pls", 10) == 0) {
		    snprintf(parse, 1000, "%s", strtok(buffer, "="));
		    snprintf(parse, 1000, "%s", strtok(NULL, "="));
		    sscanf(parse, "%i", &instance->enable_pls);
		    if (DEBUG)
			printf("pls:%i\n", instance->enable_pls);
		    continue;
		}
		// Ogg
		if (strncasecmp(buffer, "enable-ogg", 10) == 0) {
		    snprintf(parse, 1000, "%s", strtok(buffer, "="));
		    snprintf(parse, 1000, "%s", strtok(NULL, "="));
		    sscanf(parse, "%i", &instance->enable_ogg);
		    if (DEBUG)
			printf("ogg:%i\n", instance->enable_ogg);
		    continue;
		}



	    }
	    fclose(config);
	}
    }

    if (instance->useragent == NULL)
	instance->useragent = strdup("NSPlayer");

    if (instance->download_dir == NULL && instance->keep_download == 1)
	instance->download_dir = strdup(getenv("HOME"));

}
