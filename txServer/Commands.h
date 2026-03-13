/* $LastChangedDate: 2015-08-10 16:39:14 -0700 (Mon, 10 Aug 2015) $ */
/* $Rev: 59 $      $Author: peters $ */

// The names of functions that actually do the work...

extern int Radio(char *);
extern int Status();
extern int SetPower(char *);
extern int SetDAC(char *);
extern int ReadADC(char *);
extern int InitSynth(char *);
extern int SetFreq(char *);
extern int GetUDP(void);
extern int Point(char *);
extern int Quit();
extern int Help(char *);

// a structure that contains information on the commands

typedef struct {
	char *name;
	int (*func) ();
	char *doc;
} COMMAND;

COMMAND commands [ ] = {
	{"radio", &Radio, "Toggle radio transmission on/off"},
	{"status", &Status, "Report server and instrument status in plain text"},
	{"setpower", &SetPower, "Turn on and off power switches"},
	{"setdac", &SetDAC, "Set voltage on a DAC"},
	{"readadc", &ReadADC, "Read an ADC. Usage: readadc <mux>"},
	{"initsynth", &InitSynth, "Initialize the Hittite VCO prior to use"},
	{"setfreq", &SetFreq, "Set Local Oscillator Frequency: setfreq <MHz>"},
	{"getudp", &GetUDP, "Get Position from UDP Stream"},
	{"point", &Point, "Set pointing mode, get vector to Ground Station: point <gs1|gs2>"},
        {"quit", &Quit, "Quit server"},
        {"q", &Quit, "Synonym for Quit"},
	{"help", &Help, "Print list of commands or help on specified one"},
	{"?", &Help, "Synonym for help"},
	{(char *)NULL, NULL, (char *)NULL}
};
