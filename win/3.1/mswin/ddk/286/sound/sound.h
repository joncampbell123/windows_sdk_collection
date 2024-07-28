extern int far PASCAL OpenSound();
extern int far PASCAL CloseSound();
extern int far PASCAL SetVoiceQueueSize(int, int);
extern int far PASCAL SetVoiceNote(int, int, int, int);
extern int far PASCAL SetVoiceAccent(int, int, int, int, int);
extern int far PASCAL SetVoiceEnvelope(int, int, int);
extern int far PASCAL SetSoundNoise(int, int);
extern int far PASCAL SetVoiceSound(int, int, int, int);
extern int far PASCAL StartSound();
extern int far PASCAL StopSound();
extern int far PASCAL WaitSoundState(int);
extern int far PASCAL SyncAllVoices();
extern int far PASCAL CountVoiceNotes(int);
extern long far PASCAL GetThresholdEvent();
extern int far PASCAL GetThresholdStatus();
extern int far PASCAL SetVoiceThreshold(int, int);
 
/* constants used to specify return condition for WaitSoundState */
 
#define S_QUEUEEMPTY      0
#define S_THRESHOLD       1
#define S_ALLTHRESHOLD    2
 
/* constants used to specify accent mode */
 
#define     S_NORMAL      0
#define     S_LEGGATO     1
#define     S_STACCATO    2
 
/* constants used to specify source in SetSoundNoise */
#define     S_PERIOD512   0   /* freq = N/512 high pitch, less coarse hiss */
#define     S_PERIOD1024  1   /* freq = N/1024 */
#define     S_PERIOD2048  2   /* freq = N/2048 low pitch, more coarse hiss */
#define     S_PERIODVOICE 3   /* source is frequency from voice channel (3) */
#define     S_WHITE512    4   /* freq = N/512 high pitch, less coarse hiss */
#define     S_WHITE1024   5   /* freq = N/1024 */
#define     S_WHITE2048   6   /* freq = N/2048 low pitch, more coarse hiss */
#define     S_WHITEVOICE  7   /* source is frequency from voice channel (3) */
 
#define     S_SERDVNA     -1      /* device not available */
#define     S_SEROFM      -2      /* out of memory */
#define     S_SERMACT     -3      /* music active */
#define     S_SERQFUL     -4      /* queue full */
#define     S_SERBDNT     -5      /* invalid note */
#define     S_SERDLN      -6      /* invalid note length */
#define     S_SERDCC      -7      /* invalid note count */
#define     S_SERDTP      -8      /* invalid tempo */
#define     S_SERDVL      -9      /* invalid volume */
#define     S_SERDMD      -10     /* invalid mode */
#define     S_SERDSH      -11     /* invalid shape */
#define     S_SERDPT      -12     /* invalid pitch */
#define     S_SERDFQ      -13     /* invalid frequency */
#define     S_SERDDR      -14     /* invalid duration */
#define     S_SERDSR      -15     /* invalid source */
#define     S_SERDST      -16     /* invalid state */
 