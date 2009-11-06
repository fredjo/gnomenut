/*
 * DO NOT EDIT.  THIS FILE IS GENERATED FROM Source/nsIScriptableMplayerPlugin.idl
 */

#ifndef __gen_nsIScriptableMplayerPlugin_h__
#define __gen_nsIScriptableMplayerPlugin_h__


#ifndef __gen_nsISupports_h__
#include "nsISupports.h"
#endif

/* For IDL files that don't want to include root IDL files. */
#ifndef NS_NO_VTABLE
#define NS_NO_VTABLE
#endif

/* starting interface:    nsIScriptableWMPPlugin */
#define NS_ISCRIPTABLEWMPPLUGIN_IID_STR "f728830e-1dd1-4444-6666-fb9f414f2465"

#define NS_ISCRIPTABLEWMPPLUGIN_IID \
  {0xf728830e, 0x1dd1, 0x4444, \
    { 0x66, 0x66, 0xfb, 0x9f, 0x41, 0x4f, 0x24, 0x65 }}

class NS_NO_VTABLE NS_SCRIPTABLE nsIScriptableWMPPlugin : public nsISupports {
 public: 

  NS_DECLARE_STATIC_IID_ACCESSOR(NS_ISCRIPTABLEWMPPLUGIN_IID)

  /* void play (); */
  NS_SCRIPTABLE NS_IMETHOD Play(void) = 0;

  /* void pause (); */
  NS_SCRIPTABLE NS_IMETHOD Pause(void) = 0;

  /* void stop (); */
  NS_SCRIPTABLE NS_IMETHOD Stop(void) = 0;

};

  NS_DEFINE_STATIC_IID_ACCESSOR(nsIScriptableWMPPlugin, NS_ISCRIPTABLEWMPPLUGIN_IID)

/* Use this macro when declaring classes that implement this interface. */
#define NS_DECL_NSISCRIPTABLEWMPPLUGIN \
  NS_SCRIPTABLE NS_IMETHOD Play(void); \
  NS_SCRIPTABLE NS_IMETHOD Pause(void); \
  NS_SCRIPTABLE NS_IMETHOD Stop(void); 

/* Use this macro to declare functions that forward the behavior of this interface to another object. */
#define NS_FORWARD_NSISCRIPTABLEWMPPLUGIN(_to) \
  NS_SCRIPTABLE NS_IMETHOD Play(void) { return _to Play(); } \
  NS_SCRIPTABLE NS_IMETHOD Pause(void) { return _to Pause(); } \
  NS_SCRIPTABLE NS_IMETHOD Stop(void) { return _to Stop(); } 

/* Use this macro to declare functions that forward the behavior of this interface to another object in a safe way. */
#define NS_FORWARD_SAFE_NSISCRIPTABLEWMPPLUGIN(_to) \
  NS_SCRIPTABLE NS_IMETHOD Play(void) { return !_to ? NS_ERROR_NULL_POINTER : _to->Play(); } \
  NS_SCRIPTABLE NS_IMETHOD Pause(void) { return !_to ? NS_ERROR_NULL_POINTER : _to->Pause(); } \
  NS_SCRIPTABLE NS_IMETHOD Stop(void) { return !_to ? NS_ERROR_NULL_POINTER : _to->Stop(); } 

#if 0
/* Use the code below as a template for the implementation class for this interface. */

/* Header file */
class nsScriptableWMPPlugin : public nsIScriptableWMPPlugin
{
public:
  NS_DECL_ISUPPORTS
  NS_DECL_NSISCRIPTABLEWMPPLUGIN

  nsScriptableWMPPlugin();

private:
  ~nsScriptableWMPPlugin();

protected:
  /* additional members */
};

/* Implementation file */
NS_IMPL_ISUPPORTS1(nsScriptableWMPPlugin, nsIScriptableWMPPlugin)

nsScriptableWMPPlugin::nsScriptableWMPPlugin()
{
  /* member initializers and constructor code */
}

nsScriptableWMPPlugin::~nsScriptableWMPPlugin()
{
  /* destructor code */
}

/* void play (); */
NS_IMETHODIMP nsScriptableWMPPlugin::Play()
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* void pause (); */
NS_IMETHODIMP nsScriptableWMPPlugin::Pause()
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* void stop (); */
NS_IMETHODIMP nsScriptableWMPPlugin::Stop()
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* End of implementation class template. */
#endif


/* starting interface:    nsIScriptableMplayerPlugin */
#define NS_ISCRIPTABLEMPLAYERPLUGIN_IID_STR "f728830f-1dd1-4444-6666-fb9f414f2465"

#define NS_ISCRIPTABLEMPLAYERPLUGIN_IID \
  {0xf728830f, 0x1dd1, 0x4444, \
    { 0x66, 0x66, 0xfb, 0x9f, 0x41, 0x4f, 0x24, 0x65 }}

class NS_NO_VTABLE NS_SCRIPTABLE nsIScriptableMplayerPlugin : public nsISupports {
 public: 

  NS_DECLARE_STATIC_IID_ACCESSOR(NS_ISCRIPTABLEMPLAYERPLUGIN_IID)

  /* void Play (); */
  NS_SCRIPTABLE NS_IMETHOD Play(void) = 0;

  /* void PlayAt (in double value); */
  NS_SCRIPTABLE NS_IMETHOD PlayAt(double value) = 0;

  /* void Pause (); */
  NS_SCRIPTABLE NS_IMETHOD Pause(void) = 0;

  /* void Stop (); */
  NS_SCRIPTABLE NS_IMETHOD Stop(void) = 0;

  /* void quit (); */
  NS_SCRIPTABLE NS_IMETHOD Quit(void) = 0;

  /* void DoPlay (); */
  NS_SCRIPTABLE NS_IMETHOD DoPlay(void) = 0;

  /* void DoPause (); */
  NS_SCRIPTABLE NS_IMETHOD DoPause(void) = 0;

  /* void FastForward (); */
  NS_SCRIPTABLE NS_IMETHOD FastForward(void) = 0;

  /* void FastReverse (); */
  NS_SCRIPTABLE NS_IMETHOD FastReverse(void) = 0;

  /* void ff (); */
  NS_SCRIPTABLE NS_IMETHOD Ff(void) = 0;

  /* void rew (); */
  NS_SCRIPTABLE NS_IMETHOD Rew(void) = 0;

  /* void rewind (); */
  NS_SCRIPTABLE NS_IMETHOD Rewind(void) = 0;

  /* void Seek (in double value); */
  NS_SCRIPTABLE NS_IMETHOD Seek(double value) = 0;

  /* void Open (in string filename); */
  NS_SCRIPTABLE NS_IMETHOD Open(const char *filename) = 0;

  /* void SetVolume (in double value); */
  NS_SCRIPTABLE NS_IMETHOD SetVolume(double value) = 0;

  /* double GetVolume (); */
  NS_SCRIPTABLE NS_IMETHOD GetVolume(double *_retval NS_OUTPARAM) = 0;

  /* void SetFileName (in string filename); */
  NS_SCRIPTABLE NS_IMETHOD SetFileName(const char *filename) = 0;

  /* void SetIsLooping (in boolean loop); */
  NS_SCRIPTABLE NS_IMETHOD SetIsLooping(PRBool loop) = 0;

  /* boolean GetIsLooping (); */
  NS_SCRIPTABLE NS_IMETHOD GetIsLooping(PRBool *_retval NS_OUTPARAM) = 0;

  /* void SetAutoPlay (in boolean autoPlay); */
  NS_SCRIPTABLE NS_IMETHOD SetAutoPlay(PRBool autoPlay) = 0;

  /* boolean GetAutoPlay (); */
  NS_SCRIPTABLE NS_IMETHOD GetAutoPlay(PRBool *_retval NS_OUTPARAM) = 0;

  /* void SetHREF (in string url); */
  NS_SCRIPTABLE NS_IMETHOD SetHREF(const char *url) = 0;

  /* string GetHREF (); */
  NS_SCRIPTABLE NS_IMETHOD GetHREF(char **_retval NS_OUTPARAM) = 0;

  /* void SetURL (in string url); */
  NS_SCRIPTABLE NS_IMETHOD SetURL(const char *url) = 0;

  /* string GetURL (); */
  NS_SCRIPTABLE NS_IMETHOD GetURL(char **_retval NS_OUTPARAM) = 0;

  /* string GetMIMEType (); */
  NS_SCRIPTABLE NS_IMETHOD GetMIMEType(char **_retval NS_OUTPARAM) = 0;

  /* double getTime (); */
  NS_SCRIPTABLE NS_IMETHOD GetTime(double *_retval NS_OUTPARAM) = 0;

  /* double getDuration (); */
  NS_SCRIPTABLE NS_IMETHOD GetDuration(double *_retval NS_OUTPARAM) = 0;

  /* double getPercent (); */
  NS_SCRIPTABLE NS_IMETHOD GetPercent(double *_retval NS_OUTPARAM) = 0;

  /* boolean isplaying (); */
  NS_SCRIPTABLE NS_IMETHOD Isplaying(PRBool *_retval NS_OUTPARAM) = 0;

  /* void playlistAppend (in string item); */
  NS_SCRIPTABLE NS_IMETHOD PlaylistAppend(const char *item) = 0;

  /* boolean playlistClear (); */
  NS_SCRIPTABLE NS_IMETHOD PlaylistClear(PRBool *_retval NS_OUTPARAM) = 0;

  /* attribute string filename; */
  NS_SCRIPTABLE NS_IMETHOD GetFilename(char * *aFilename) = 0;
  NS_SCRIPTABLE NS_IMETHOD SetFilename(const char * aFilename) = 0;

  /* attribute string src; */
  NS_SCRIPTABLE NS_IMETHOD GetSrc(char * *aSrc) = 0;
  NS_SCRIPTABLE NS_IMETHOD SetSrc(const char * aSrc) = 0;

  /* attribute boolean ShowControls; */
  NS_SCRIPTABLE NS_IMETHOD GetShowControls(PRBool *aShowControls) = 0;
  NS_SCRIPTABLE NS_IMETHOD SetShowControls(PRBool aShowControls) = 0;

  /* attribute boolean fullscreen; */
  NS_SCRIPTABLE NS_IMETHOD GetFullscreen(PRBool *aFullscreen) = 0;
  NS_SCRIPTABLE NS_IMETHOD SetFullscreen(PRBool aFullscreen) = 0;

  /* attribute boolean showlogo; */
  NS_SCRIPTABLE NS_IMETHOD GetShowlogo(PRBool *aShowlogo) = 0;
  NS_SCRIPTABLE NS_IMETHOD SetShowlogo(PRBool aShowlogo) = 0;

  /* readonly attribute long playState; */
  NS_SCRIPTABLE NS_IMETHOD GetPlayState(PRInt32 *aPlayState) = 0;

  /* readonly attribute nsIScriptableWMPPlugin controls; */
  NS_SCRIPTABLE NS_IMETHOD GetControls(nsIScriptableWMPPlugin * *aControls) = 0;

  /* void onClick (in string event); */
  NS_SCRIPTABLE NS_IMETHOD OnClick(const char *event) = 0;

};

  NS_DEFINE_STATIC_IID_ACCESSOR(nsIScriptableMplayerPlugin, NS_ISCRIPTABLEMPLAYERPLUGIN_IID)

/* Use this macro when declaring classes that implement this interface. */
#define NS_DECL_NSISCRIPTABLEMPLAYERPLUGIN \
  NS_SCRIPTABLE NS_IMETHOD Play(void); \
  NS_SCRIPTABLE NS_IMETHOD PlayAt(double value); \
  NS_SCRIPTABLE NS_IMETHOD Pause(void); \
  NS_SCRIPTABLE NS_IMETHOD Stop(void); \
  NS_SCRIPTABLE NS_IMETHOD Quit(void); \
  NS_SCRIPTABLE NS_IMETHOD DoPlay(void); \
  NS_SCRIPTABLE NS_IMETHOD DoPause(void); \
  NS_SCRIPTABLE NS_IMETHOD FastForward(void); \
  NS_SCRIPTABLE NS_IMETHOD FastReverse(void); \
  NS_SCRIPTABLE NS_IMETHOD Ff(void); \
  NS_SCRIPTABLE NS_IMETHOD Rew(void); \
  NS_SCRIPTABLE NS_IMETHOD Rewind(void); \
  NS_SCRIPTABLE NS_IMETHOD Seek(double value); \
  NS_SCRIPTABLE NS_IMETHOD Open(const char *filename); \
  NS_SCRIPTABLE NS_IMETHOD SetVolume(double value); \
  NS_SCRIPTABLE NS_IMETHOD GetVolume(double *_retval NS_OUTPARAM); \
  NS_SCRIPTABLE NS_IMETHOD SetFileName(const char *filename); \
  NS_SCRIPTABLE NS_IMETHOD SetIsLooping(PRBool loop); \
  NS_SCRIPTABLE NS_IMETHOD GetIsLooping(PRBool *_retval NS_OUTPARAM); \
  NS_SCRIPTABLE NS_IMETHOD SetAutoPlay(PRBool autoPlay); \
  NS_SCRIPTABLE NS_IMETHOD GetAutoPlay(PRBool *_retval NS_OUTPARAM); \
  NS_SCRIPTABLE NS_IMETHOD SetHREF(const char *url); \
  NS_SCRIPTABLE NS_IMETHOD GetHREF(char **_retval NS_OUTPARAM); \
  NS_SCRIPTABLE NS_IMETHOD SetURL(const char *url); \
  NS_SCRIPTABLE NS_IMETHOD GetURL(char **_retval NS_OUTPARAM); \
  NS_SCRIPTABLE NS_IMETHOD GetMIMEType(char **_retval NS_OUTPARAM); \
  NS_SCRIPTABLE NS_IMETHOD GetTime(double *_retval NS_OUTPARAM); \
  NS_SCRIPTABLE NS_IMETHOD GetDuration(double *_retval NS_OUTPARAM); \
  NS_SCRIPTABLE NS_IMETHOD GetPercent(double *_retval NS_OUTPARAM); \
  NS_SCRIPTABLE NS_IMETHOD Isplaying(PRBool *_retval NS_OUTPARAM); \
  NS_SCRIPTABLE NS_IMETHOD PlaylistAppend(const char *item); \
  NS_SCRIPTABLE NS_IMETHOD PlaylistClear(PRBool *_retval NS_OUTPARAM); \
  NS_SCRIPTABLE NS_IMETHOD GetFilename(char * *aFilename); \
  NS_SCRIPTABLE NS_IMETHOD SetFilename(const char * aFilename); \
  NS_SCRIPTABLE NS_IMETHOD GetSrc(char * *aSrc); \
  NS_SCRIPTABLE NS_IMETHOD SetSrc(const char * aSrc); \
  NS_SCRIPTABLE NS_IMETHOD GetShowControls(PRBool *aShowControls); \
  NS_SCRIPTABLE NS_IMETHOD SetShowControls(PRBool aShowControls); \
  NS_SCRIPTABLE NS_IMETHOD GetFullscreen(PRBool *aFullscreen); \
  NS_SCRIPTABLE NS_IMETHOD SetFullscreen(PRBool aFullscreen); \
  NS_SCRIPTABLE NS_IMETHOD GetShowlogo(PRBool *aShowlogo); \
  NS_SCRIPTABLE NS_IMETHOD SetShowlogo(PRBool aShowlogo); \
  NS_SCRIPTABLE NS_IMETHOD GetPlayState(PRInt32 *aPlayState); \
  NS_SCRIPTABLE NS_IMETHOD GetControls(nsIScriptableWMPPlugin * *aControls); \
  NS_SCRIPTABLE NS_IMETHOD OnClick(const char *event); 

/* Use this macro to declare functions that forward the behavior of this interface to another object. */
#define NS_FORWARD_NSISCRIPTABLEMPLAYERPLUGIN(_to) \
  NS_SCRIPTABLE NS_IMETHOD Play(void) { return _to Play(); } \
  NS_SCRIPTABLE NS_IMETHOD PlayAt(double value) { return _to PlayAt(value); } \
  NS_SCRIPTABLE NS_IMETHOD Pause(void) { return _to Pause(); } \
  NS_SCRIPTABLE NS_IMETHOD Stop(void) { return _to Stop(); } \
  NS_SCRIPTABLE NS_IMETHOD Quit(void) { return _to Quit(); } \
  NS_SCRIPTABLE NS_IMETHOD DoPlay(void) { return _to DoPlay(); } \
  NS_SCRIPTABLE NS_IMETHOD DoPause(void) { return _to DoPause(); } \
  NS_SCRIPTABLE NS_IMETHOD FastForward(void) { return _to FastForward(); } \
  NS_SCRIPTABLE NS_IMETHOD FastReverse(void) { return _to FastReverse(); } \
  NS_SCRIPTABLE NS_IMETHOD Ff(void) { return _to Ff(); } \
  NS_SCRIPTABLE NS_IMETHOD Rew(void) { return _to Rew(); } \
  NS_SCRIPTABLE NS_IMETHOD Rewind(void) { return _to Rewind(); } \
  NS_SCRIPTABLE NS_IMETHOD Seek(double value) { return _to Seek(value); } \
  NS_SCRIPTABLE NS_IMETHOD Open(const char *filename) { return _to Open(filename); } \
  NS_SCRIPTABLE NS_IMETHOD SetVolume(double value) { return _to SetVolume(value); } \
  NS_SCRIPTABLE NS_IMETHOD GetVolume(double *_retval NS_OUTPARAM) { return _to GetVolume(_retval); } \
  NS_SCRIPTABLE NS_IMETHOD SetFileName(const char *filename) { return _to SetFileName(filename); } \
  NS_SCRIPTABLE NS_IMETHOD SetIsLooping(PRBool loop) { return _to SetIsLooping(loop); } \
  NS_SCRIPTABLE NS_IMETHOD GetIsLooping(PRBool *_retval NS_OUTPARAM) { return _to GetIsLooping(_retval); } \
  NS_SCRIPTABLE NS_IMETHOD SetAutoPlay(PRBool autoPlay) { return _to SetAutoPlay(autoPlay); } \
  NS_SCRIPTABLE NS_IMETHOD GetAutoPlay(PRBool *_retval NS_OUTPARAM) { return _to GetAutoPlay(_retval); } \
  NS_SCRIPTABLE NS_IMETHOD SetHREF(const char *url) { return _to SetHREF(url); } \
  NS_SCRIPTABLE NS_IMETHOD GetHREF(char **_retval NS_OUTPARAM) { return _to GetHREF(_retval); } \
  NS_SCRIPTABLE NS_IMETHOD SetURL(const char *url) { return _to SetURL(url); } \
  NS_SCRIPTABLE NS_IMETHOD GetURL(char **_retval NS_OUTPARAM) { return _to GetURL(_retval); } \
  NS_SCRIPTABLE NS_IMETHOD GetMIMEType(char **_retval NS_OUTPARAM) { return _to GetMIMEType(_retval); } \
  NS_SCRIPTABLE NS_IMETHOD GetTime(double *_retval NS_OUTPARAM) { return _to GetTime(_retval); } \
  NS_SCRIPTABLE NS_IMETHOD GetDuration(double *_retval NS_OUTPARAM) { return _to GetDuration(_retval); } \
  NS_SCRIPTABLE NS_IMETHOD GetPercent(double *_retval NS_OUTPARAM) { return _to GetPercent(_retval); } \
  NS_SCRIPTABLE NS_IMETHOD Isplaying(PRBool *_retval NS_OUTPARAM) { return _to Isplaying(_retval); } \
  NS_SCRIPTABLE NS_IMETHOD PlaylistAppend(const char *item) { return _to PlaylistAppend(item); } \
  NS_SCRIPTABLE NS_IMETHOD PlaylistClear(PRBool *_retval NS_OUTPARAM) { return _to PlaylistClear(_retval); } \
  NS_SCRIPTABLE NS_IMETHOD GetFilename(char * *aFilename) { return _to GetFilename(aFilename); } \
  NS_SCRIPTABLE NS_IMETHOD SetFilename(const char * aFilename) { return _to SetFilename(aFilename); } \
  NS_SCRIPTABLE NS_IMETHOD GetSrc(char * *aSrc) { return _to GetSrc(aSrc); } \
  NS_SCRIPTABLE NS_IMETHOD SetSrc(const char * aSrc) { return _to SetSrc(aSrc); } \
  NS_SCRIPTABLE NS_IMETHOD GetShowControls(PRBool *aShowControls) { return _to GetShowControls(aShowControls); } \
  NS_SCRIPTABLE NS_IMETHOD SetShowControls(PRBool aShowControls) { return _to SetShowControls(aShowControls); } \
  NS_SCRIPTABLE NS_IMETHOD GetFullscreen(PRBool *aFullscreen) { return _to GetFullscreen(aFullscreen); } \
  NS_SCRIPTABLE NS_IMETHOD SetFullscreen(PRBool aFullscreen) { return _to SetFullscreen(aFullscreen); } \
  NS_SCRIPTABLE NS_IMETHOD GetShowlogo(PRBool *aShowlogo) { return _to GetShowlogo(aShowlogo); } \
  NS_SCRIPTABLE NS_IMETHOD SetShowlogo(PRBool aShowlogo) { return _to SetShowlogo(aShowlogo); } \
  NS_SCRIPTABLE NS_IMETHOD GetPlayState(PRInt32 *aPlayState) { return _to GetPlayState(aPlayState); } \
  NS_SCRIPTABLE NS_IMETHOD GetControls(nsIScriptableWMPPlugin * *aControls) { return _to GetControls(aControls); } \
  NS_SCRIPTABLE NS_IMETHOD OnClick(const char *event) { return _to OnClick(event); } 

/* Use this macro to declare functions that forward the behavior of this interface to another object in a safe way. */
#define NS_FORWARD_SAFE_NSISCRIPTABLEMPLAYERPLUGIN(_to) \
  NS_SCRIPTABLE NS_IMETHOD Play(void) { return !_to ? NS_ERROR_NULL_POINTER : _to->Play(); } \
  NS_SCRIPTABLE NS_IMETHOD PlayAt(double value) { return !_to ? NS_ERROR_NULL_POINTER : _to->PlayAt(value); } \
  NS_SCRIPTABLE NS_IMETHOD Pause(void) { return !_to ? NS_ERROR_NULL_POINTER : _to->Pause(); } \
  NS_SCRIPTABLE NS_IMETHOD Stop(void) { return !_to ? NS_ERROR_NULL_POINTER : _to->Stop(); } \
  NS_SCRIPTABLE NS_IMETHOD Quit(void) { return !_to ? NS_ERROR_NULL_POINTER : _to->Quit(); } \
  NS_SCRIPTABLE NS_IMETHOD DoPlay(void) { return !_to ? NS_ERROR_NULL_POINTER : _to->DoPlay(); } \
  NS_SCRIPTABLE NS_IMETHOD DoPause(void) { return !_to ? NS_ERROR_NULL_POINTER : _to->DoPause(); } \
  NS_SCRIPTABLE NS_IMETHOD FastForward(void) { return !_to ? NS_ERROR_NULL_POINTER : _to->FastForward(); } \
  NS_SCRIPTABLE NS_IMETHOD FastReverse(void) { return !_to ? NS_ERROR_NULL_POINTER : _to->FastReverse(); } \
  NS_SCRIPTABLE NS_IMETHOD Ff(void) { return !_to ? NS_ERROR_NULL_POINTER : _to->Ff(); } \
  NS_SCRIPTABLE NS_IMETHOD Rew(void) { return !_to ? NS_ERROR_NULL_POINTER : _to->Rew(); } \
  NS_SCRIPTABLE NS_IMETHOD Rewind(void) { return !_to ? NS_ERROR_NULL_POINTER : _to->Rewind(); } \
  NS_SCRIPTABLE NS_IMETHOD Seek(double value) { return !_to ? NS_ERROR_NULL_POINTER : _to->Seek(value); } \
  NS_SCRIPTABLE NS_IMETHOD Open(const char *filename) { return !_to ? NS_ERROR_NULL_POINTER : _to->Open(filename); } \
  NS_SCRIPTABLE NS_IMETHOD SetVolume(double value) { return !_to ? NS_ERROR_NULL_POINTER : _to->SetVolume(value); } \
  NS_SCRIPTABLE NS_IMETHOD GetVolume(double *_retval NS_OUTPARAM) { return !_to ? NS_ERROR_NULL_POINTER : _to->GetVolume(_retval); } \
  NS_SCRIPTABLE NS_IMETHOD SetFileName(const char *filename) { return !_to ? NS_ERROR_NULL_POINTER : _to->SetFileName(filename); } \
  NS_SCRIPTABLE NS_IMETHOD SetIsLooping(PRBool loop) { return !_to ? NS_ERROR_NULL_POINTER : _to->SetIsLooping(loop); } \
  NS_SCRIPTABLE NS_IMETHOD GetIsLooping(PRBool *_retval NS_OUTPARAM) { return !_to ? NS_ERROR_NULL_POINTER : _to->GetIsLooping(_retval); } \
  NS_SCRIPTABLE NS_IMETHOD SetAutoPlay(PRBool autoPlay) { return !_to ? NS_ERROR_NULL_POINTER : _to->SetAutoPlay(autoPlay); } \
  NS_SCRIPTABLE NS_IMETHOD GetAutoPlay(PRBool *_retval NS_OUTPARAM) { return !_to ? NS_ERROR_NULL_POINTER : _to->GetAutoPlay(_retval); } \
  NS_SCRIPTABLE NS_IMETHOD SetHREF(const char *url) { return !_to ? NS_ERROR_NULL_POINTER : _to->SetHREF(url); } \
  NS_SCRIPTABLE NS_IMETHOD GetHREF(char **_retval NS_OUTPARAM) { return !_to ? NS_ERROR_NULL_POINTER : _to->GetHREF(_retval); } \
  NS_SCRIPTABLE NS_IMETHOD SetURL(const char *url) { return !_to ? NS_ERROR_NULL_POINTER : _to->SetURL(url); } \
  NS_SCRIPTABLE NS_IMETHOD GetURL(char **_retval NS_OUTPARAM) { return !_to ? NS_ERROR_NULL_POINTER : _to->GetURL(_retval); } \
  NS_SCRIPTABLE NS_IMETHOD GetMIMEType(char **_retval NS_OUTPARAM) { return !_to ? NS_ERROR_NULL_POINTER : _to->GetMIMEType(_retval); } \
  NS_SCRIPTABLE NS_IMETHOD GetTime(double *_retval NS_OUTPARAM) { return !_to ? NS_ERROR_NULL_POINTER : _to->GetTime(_retval); } \
  NS_SCRIPTABLE NS_IMETHOD GetDuration(double *_retval NS_OUTPARAM) { return !_to ? NS_ERROR_NULL_POINTER : _to->GetDuration(_retval); } \
  NS_SCRIPTABLE NS_IMETHOD GetPercent(double *_retval NS_OUTPARAM) { return !_to ? NS_ERROR_NULL_POINTER : _to->GetPercent(_retval); } \
  NS_SCRIPTABLE NS_IMETHOD Isplaying(PRBool *_retval NS_OUTPARAM) { return !_to ? NS_ERROR_NULL_POINTER : _to->Isplaying(_retval); } \
  NS_SCRIPTABLE NS_IMETHOD PlaylistAppend(const char *item) { return !_to ? NS_ERROR_NULL_POINTER : _to->PlaylistAppend(item); } \
  NS_SCRIPTABLE NS_IMETHOD PlaylistClear(PRBool *_retval NS_OUTPARAM) { return !_to ? NS_ERROR_NULL_POINTER : _to->PlaylistClear(_retval); } \
  NS_SCRIPTABLE NS_IMETHOD GetFilename(char * *aFilename) { return !_to ? NS_ERROR_NULL_POINTER : _to->GetFilename(aFilename); } \
  NS_SCRIPTABLE NS_IMETHOD SetFilename(const char * aFilename) { return !_to ? NS_ERROR_NULL_POINTER : _to->SetFilename(aFilename); } \
  NS_SCRIPTABLE NS_IMETHOD GetSrc(char * *aSrc) { return !_to ? NS_ERROR_NULL_POINTER : _to->GetSrc(aSrc); } \
  NS_SCRIPTABLE NS_IMETHOD SetSrc(const char * aSrc) { return !_to ? NS_ERROR_NULL_POINTER : _to->SetSrc(aSrc); } \
  NS_SCRIPTABLE NS_IMETHOD GetShowControls(PRBool *aShowControls) { return !_to ? NS_ERROR_NULL_POINTER : _to->GetShowControls(aShowControls); } \
  NS_SCRIPTABLE NS_IMETHOD SetShowControls(PRBool aShowControls) { return !_to ? NS_ERROR_NULL_POINTER : _to->SetShowControls(aShowControls); } \
  NS_SCRIPTABLE NS_IMETHOD GetFullscreen(PRBool *aFullscreen) { return !_to ? NS_ERROR_NULL_POINTER : _to->GetFullscreen(aFullscreen); } \
  NS_SCRIPTABLE NS_IMETHOD SetFullscreen(PRBool aFullscreen) { return !_to ? NS_ERROR_NULL_POINTER : _to->SetFullscreen(aFullscreen); } \
  NS_SCRIPTABLE NS_IMETHOD GetShowlogo(PRBool *aShowlogo) { return !_to ? NS_ERROR_NULL_POINTER : _to->GetShowlogo(aShowlogo); } \
  NS_SCRIPTABLE NS_IMETHOD SetShowlogo(PRBool aShowlogo) { return !_to ? NS_ERROR_NULL_POINTER : _to->SetShowlogo(aShowlogo); } \
  NS_SCRIPTABLE NS_IMETHOD GetPlayState(PRInt32 *aPlayState) { return !_to ? NS_ERROR_NULL_POINTER : _to->GetPlayState(aPlayState); } \
  NS_SCRIPTABLE NS_IMETHOD GetControls(nsIScriptableWMPPlugin * *aControls) { return !_to ? NS_ERROR_NULL_POINTER : _to->GetControls(aControls); } \
  NS_SCRIPTABLE NS_IMETHOD OnClick(const char *event) { return !_to ? NS_ERROR_NULL_POINTER : _to->OnClick(event); } 

#if 0
/* Use the code below as a template for the implementation class for this interface. */

/* Header file */
class nsScriptableMplayerPlugin : public nsIScriptableMplayerPlugin
{
public:
  NS_DECL_ISUPPORTS
  NS_DECL_NSISCRIPTABLEMPLAYERPLUGIN

  nsScriptableMplayerPlugin();

private:
  ~nsScriptableMplayerPlugin();

protected:
  /* additional members */
};

/* Implementation file */
NS_IMPL_ISUPPORTS1(nsScriptableMplayerPlugin, nsIScriptableMplayerPlugin)

nsScriptableMplayerPlugin::nsScriptableMplayerPlugin()
{
  /* member initializers and constructor code */
}

nsScriptableMplayerPlugin::~nsScriptableMplayerPlugin()
{
  /* destructor code */
}

/* void Play (); */
NS_IMETHODIMP nsScriptableMplayerPlugin::Play()
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* void PlayAt (in double value); */
NS_IMETHODIMP nsScriptableMplayerPlugin::PlayAt(double value)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* void Pause (); */
NS_IMETHODIMP nsScriptableMplayerPlugin::Pause()
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* void Stop (); */
NS_IMETHODIMP nsScriptableMplayerPlugin::Stop()
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* void quit (); */
NS_IMETHODIMP nsScriptableMplayerPlugin::Quit()
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* void DoPlay (); */
NS_IMETHODIMP nsScriptableMplayerPlugin::DoPlay()
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* void DoPause (); */
NS_IMETHODIMP nsScriptableMplayerPlugin::DoPause()
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* void FastForward (); */
NS_IMETHODIMP nsScriptableMplayerPlugin::FastForward()
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* void FastReverse (); */
NS_IMETHODIMP nsScriptableMplayerPlugin::FastReverse()
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* void ff (); */
NS_IMETHODIMP nsScriptableMplayerPlugin::Ff()
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* void rew (); */
NS_IMETHODIMP nsScriptableMplayerPlugin::Rew()
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* void rewind (); */
NS_IMETHODIMP nsScriptableMplayerPlugin::Rewind()
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* void Seek (in double value); */
NS_IMETHODIMP nsScriptableMplayerPlugin::Seek(double value)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* void Open (in string filename); */
NS_IMETHODIMP nsScriptableMplayerPlugin::Open(const char *filename)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* void SetVolume (in double value); */
NS_IMETHODIMP nsScriptableMplayerPlugin::SetVolume(double value)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* double GetVolume (); */
NS_IMETHODIMP nsScriptableMplayerPlugin::GetVolume(double *_retval NS_OUTPARAM)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* void SetFileName (in string filename); */
NS_IMETHODIMP nsScriptableMplayerPlugin::SetFileName(const char *filename)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* void SetIsLooping (in boolean loop); */
NS_IMETHODIMP nsScriptableMplayerPlugin::SetIsLooping(PRBool loop)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* boolean GetIsLooping (); */
NS_IMETHODIMP nsScriptableMplayerPlugin::GetIsLooping(PRBool *_retval NS_OUTPARAM)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* void SetAutoPlay (in boolean autoPlay); */
NS_IMETHODIMP nsScriptableMplayerPlugin::SetAutoPlay(PRBool autoPlay)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* boolean GetAutoPlay (); */
NS_IMETHODIMP nsScriptableMplayerPlugin::GetAutoPlay(PRBool *_retval NS_OUTPARAM)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* void SetHREF (in string url); */
NS_IMETHODIMP nsScriptableMplayerPlugin::SetHREF(const char *url)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* string GetHREF (); */
NS_IMETHODIMP nsScriptableMplayerPlugin::GetHREF(char **_retval NS_OUTPARAM)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* void SetURL (in string url); */
NS_IMETHODIMP nsScriptableMplayerPlugin::SetURL(const char *url)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* string GetURL (); */
NS_IMETHODIMP nsScriptableMplayerPlugin::GetURL(char **_retval NS_OUTPARAM)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* string GetMIMEType (); */
NS_IMETHODIMP nsScriptableMplayerPlugin::GetMIMEType(char **_retval NS_OUTPARAM)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* double getTime (); */
NS_IMETHODIMP nsScriptableMplayerPlugin::GetTime(double *_retval NS_OUTPARAM)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* double getDuration (); */
NS_IMETHODIMP nsScriptableMplayerPlugin::GetDuration(double *_retval NS_OUTPARAM)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* double getPercent (); */
NS_IMETHODIMP nsScriptableMplayerPlugin::GetPercent(double *_retval NS_OUTPARAM)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* boolean isplaying (); */
NS_IMETHODIMP nsScriptableMplayerPlugin::Isplaying(PRBool *_retval NS_OUTPARAM)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* void playlistAppend (in string item); */
NS_IMETHODIMP nsScriptableMplayerPlugin::PlaylistAppend(const char *item)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* boolean playlistClear (); */
NS_IMETHODIMP nsScriptableMplayerPlugin::PlaylistClear(PRBool *_retval NS_OUTPARAM)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* attribute string filename; */
NS_IMETHODIMP nsScriptableMplayerPlugin::GetFilename(char * *aFilename)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}
NS_IMETHODIMP nsScriptableMplayerPlugin::SetFilename(const char * aFilename)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* attribute string src; */
NS_IMETHODIMP nsScriptableMplayerPlugin::GetSrc(char * *aSrc)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}
NS_IMETHODIMP nsScriptableMplayerPlugin::SetSrc(const char * aSrc)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* attribute boolean ShowControls; */
NS_IMETHODIMP nsScriptableMplayerPlugin::GetShowControls(PRBool *aShowControls)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}
NS_IMETHODIMP nsScriptableMplayerPlugin::SetShowControls(PRBool aShowControls)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* attribute boolean fullscreen; */
NS_IMETHODIMP nsScriptableMplayerPlugin::GetFullscreen(PRBool *aFullscreen)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}
NS_IMETHODIMP nsScriptableMplayerPlugin::SetFullscreen(PRBool aFullscreen)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* attribute boolean showlogo; */
NS_IMETHODIMP nsScriptableMplayerPlugin::GetShowlogo(PRBool *aShowlogo)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}
NS_IMETHODIMP nsScriptableMplayerPlugin::SetShowlogo(PRBool aShowlogo)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* readonly attribute long playState; */
NS_IMETHODIMP nsScriptableMplayerPlugin::GetPlayState(PRInt32 *aPlayState)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* readonly attribute nsIScriptableWMPPlugin controls; */
NS_IMETHODIMP nsScriptableMplayerPlugin::GetControls(nsIScriptableWMPPlugin * *aControls)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* void onClick (in string event); */
NS_IMETHODIMP nsScriptableMplayerPlugin::OnClick(const char *event)
{
    return NS_ERROR_NOT_IMPLEMENTED;
}

/* End of implementation class template. */
#endif


#endif /* __gen_nsIScriptableMplayerPlugin_h__ */
