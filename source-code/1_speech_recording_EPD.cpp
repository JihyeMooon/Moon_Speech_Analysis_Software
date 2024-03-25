// This .cpp file provides speech recording and end point detection (EPD) functions

#include <iostream>
#include <cmath> 
#include <stdio.h>
#include <sstream>
#include <dsound.h>
using namespace std;
typedef enum
{
	NOSILENCE,
	INSILENCE,
	START,
	INSIGNAL,
	END
} EPSTATE;
typedef enum
{
	EP_NONE,
	EP_RESET,
	EP_SILENCE,
	EP_SIGNAL,
	EP_INUTT,
	EP_MAYBEEND,
	EP_ENDOFUTT,
	EP_NOTEND,
	EP_NOSTARTSILENCE
} EPTAG;
int windowsize, minuttlng = 100, zcthresh = 60, numdpnoise = 6, minfriclng = 40, 
maxpause = 150, maxipause = 480, endblip = 20, startblip = 30, minvoicelng = 60, minrise = 15;
EPSTATE epstate = NOSILENCE;
float dpnoise, noise = 0.0,
ave = 0.0, begthresh = 0.0, endthresh = begthresh,
energy = 0.0, maxpeak = 0.0, peakreturn = 0, mnbe = 0;
long scnt = 0, vcnt = 0, evcnt = 0, voicecount = 0,
zccnt = 0, bscnt = 0, startframe = 0, endframe = 0, avescnt = 0, zc = 0; float begfact = 40.0,		// begfact default
endfact = 80.0,		// endfact default
energyfact = 200.0,		// energyfact default
minstartsilence = 2000.0,		// minstartsilence default 
triggerfact = 3.0;		// triggerfact default  
int number = 0, fstart = 0, fend = 0;
bool startsilenceok = false;
bool low = true;
long ncount = 0; 
float *lastdpnoise;
void initendpoint();
int capture(int rate, short *capturebuf, int capturelen, short *data);
const char *gettagname(EPTAG tag); 
void zcpeakpick(short *samples);
EPTAG getendpoint(short *samples);
void averagenoise();
void setnoise();
void main()
{
	WAVEFORMATEX m_WaveFormat;
	m_WaveFormat.wFormatTag = WAVE_FORMAT_PCM; //PCM
	m_WaveFormat.nChannels = 2; 
	m_WaveFormat.nSamplesPerSec = 22050; 
	m_WaveFormat.wBitsPerSample = 32; 
	m_WaveFormat.nBlockAlign = m_WaveFormat.nChannels *  m_WaveFormat.wBitsPerSample / 8; 
	m_WaveFormat.nAvgBytesPerSec = m_WaveFormat.nSamplesPerSec * m_WaveFormat.nBlockAlign; 
	m_WaveFormat.cbSize = 0; 

	FILE *pcm = NULL, *txt2 = NULL, *Data_save = NULL;					short *data = NULL;
	int size, i = 0;									

	fopen_s(&pcm, "sentence.wav", "r");							

	fopen_s(&Data_save, "sentence2.wav", "wb");
	fopen_s(&txt2, "sentence2.txt", "w");							
	fseek(pcm, 0, SEEK_END);							 
	size = ftell(pcm) - 44;								
	size = size / sizeof(short);						
	fseek(pcm, 44, SEEK_SET);
	data = (short*)malloc(sizeof(int)*size);
	short *bufdata = (short*)malloc(sizeof(int)*size);
	int fresult = 0;
	fread(data, size, 4, pcm);
	//cout << size << " - size \n";
	lastdpnoise = new float[numdpnoise];
	for (i = 0; i < numdpnoise; i++)
		lastdpnoise[i] = 0.0;
	initendpoint(); 
	//delete[]lastdpnoise;
	//free(pcm);

	int do_endpoint = 1;
	int rate = 16000, channels = 2;
	double time = 1;
	short *buf;
	int c, r_samples = 0;
	int fstend = 0;
	cout << r_samples << "\n";
	/* output the captured samples */ 
	if (do_endpoint) {
		/* wait for audio event before grabbing samples */ 
		r_samples = capture(rate, bufdata, size, data);
	} 
	fresult = fend - fstart;
	buf = (short*)malloc(sizeof(int)*fresult);
		/* grab all the samples you need directly */
		for (i = 0; i < fresult; i++){
			buf[i] = data[fstart + i];
			fprintf(txt2, "%d\n", buf[i]); 
			//cout << buf[i] << "\t";
	}
		fstend = fend - fstart;
		fstend = fstend * 2;
		cout << "r_samples(captureend) : " << r_samples << "\n";
		cout << "fend-fstart" << fresult << "\n";
		cout << "fend-fstart" << i << "\n";
		cout << "fend" << fend << "\n";
		cout << "fstart" << fstart << "\n";
		if (Data_save != NULL){
			fwrite("RIFF", 4, 1, Data_save);

			size *= 2;
			int save_size = 40 + fresult;
			fwrite(&save_size, sizeof(int), 1, Data_save);
			fwrite("WAVE", 4, 1, Data_save);

			fwrite("fmt ", 4, 1, Data_save);
			int long_temp = 16;
			
			fwrite(&long_temp, sizeof(DWORD), 1, Data_save);
			short int temp = WAVE_FORMAT_PCM;
			fwrite(&temp, sizeof(short int), 1, Data_save);
			fwrite(&m_WaveFormat.nChannels, sizeof(short int), 1, Data_save);
			fwrite(&m_WaveFormat.nSamplesPerSec, sizeof(DWORD), 1, Data_save);
			fwrite(&m_WaveFormat.nAvgBytesPerSec, sizeof(DWORD), 1, Data_save);
			fwrite(&m_WaveFormat.nBlockAlign, sizeof(short int), 1, Data_save);
			fwrite(&m_WaveFormat.wBitsPerSample, sizeof(short int), 1, Data_save);

			fwrite("data", 4, 1, Data_save);
			fwrite(&fstend, sizeof(int), 1, Data_save);

			fwrite(buf, fstend, 1, Data_save);

			
		}
		cout << "fstend " << fstend << "\n";
		fclose(pcm);
		fclose(txt2);
		fclose(Data_save); 
} 
void setnoise()
{
	dpnoise = lastdpnoise[1] = lastdpnoise[0];
	ncount = 2;
} // end endpointer::setnoise
void averagenoise()
{
	long	i;

	for (dpnoise = 0.0, i = ncount - 1; i > 0; i--)
	{
		dpnoise += lastdpnoise[i];
		lastdpnoise[i] = lastdpnoise[i - 1];
	}
	dpnoise = (dpnoise + lastdpnoise[0]) / ncount;
	if (ncount < numdpnoise)
		ncount++;
} // end endpointer::averagenoise 
const char *gettagname(EPTAG tag)
{
	static const char *tagnames[] =	// must match EPTAG enum in endpoint.h
	{
		"NONE",
		"RESET",
		"SILENCE",
		"SIGNAL",
		"INUTT",
		"MAYBEEND",
		"ENDOFUTT",
		"NOTEND",
		"NOSTARTSILENCE"
	};
	long	ntag = long(tag);

	if (ntag < 0 || ntag > long(EP_NOSTARTSILENCE))
		return ("UNKNOWN");
	else
		return (tagnames[ntag]);
} // end endpointer::gettagname

int capture(int rate, short *capturebuf, int capturelen, short *data)
{
	int i = 0;
	// Note: initial silence is WINDOW+2*STEP
	const float STEP = 0.010;	        // step size in sec
	const float WINDOW = 0.016;	// window size in sec
	const long ENDSILENCE = 700;      // duration of end silence in msec 
	const long MINLENGTH = 300;       // minimum utterance in msec

	int framelen, framestep;
	short *frame;
	int framenumber = 0; /* Currently active frame number */
	int framepos = 0;
	int capturepos=0, remaining, captureend = 0;
	EPTAG state = EP_RESET;
	FILE *txt = NULL;
	int count = 0;
	fopen_s(&txt, "frame.txt", "w");
	/* initialize capture */
	framelen = (int)(WINDOW*(float)rate);
	framestep = (int)(STEP*(float)rate);
	frame = (short*)malloc(sizeof(int)*framelen);
	int end = 1;
	while (1) {
		/* Fill the next frame */ 
		while (framepos < framelen && i<=capturelen) {
			frame[framepos++] = data[i];
			//cout << "frame[framepos]" << frame[framepos] << "\n";
			fprintf(txt, "%d\n", frame[framepos]);
			i++;
		}
		framenumber++; 
		windowsize = framelen; 
		/* Process frame through the end point detector */
		EPTAG TAG;
		TAG = getendpoint(frame);// get endpoint tag 
		 
		switch (TAG) {	// determine what to do with this frame  
		case EP_NOSTARTSILENCE:	// error condition --- restart process
			if (TAG == EP_NOSTARTSILENCE)
				cout << "Spoke too early. Wait a bit and try again...";
			initendpoint();
			framenumber = 0; 
			// fall through to RESET

		case EP_RESET:		// false start --- restart recognizer
			// fall through to SILENCE

		case EP_SILENCE:		// not yet start of utterance
			if (state != EP_SILENCE && framenumber > 3) {
				cout << "Waiting for you to speak...";
				state = EP_SILENCE; 
			}
			capturepos = 0; 
			break; 
		case EP_MAYBEEND:		// possible end of utterance
			if (TAG == EP_MAYBEEND) {
				cout << "Probably it’s done " << capturepos << "\n";
				captureend = capturepos; 
				cout << "EP_MAYBEEND i : " << i << "\n";
				fend = i;
			}// fall through to SIGNAL


		case EP_NOTEND:		// the last MAYBE END was NOT the end 
			if (TAG == EP_NOTEND) {
				captureend = 0;
				cout << "It’s not done : " << capturepos << "\n"; 
				cout << "EP_NOTEND i : " << i << "\n";
			}// fall through to SIGNAL

		case EP_INUTT:		// confirmed signal start
			// all data frames before this marked as EP_SIGNAL were part
			// of the actual utterance.  A reset after this point will be
			// due to a rejected signal rather than a false start.
			if (state != EP_INUTT) {
				cout << "EP_INUTT i : " << i << "\n";
				fstart = i;
				cout << "Capturing your speech...";
				state = EP_INUTT; 
				cout << "capturepos : " << capturepos << "\n";
			}
			// fall through to SIGNAL

		case EP_SIGNAL:		// signal frame
			// Copy frame into capture buf.
			remaining = capturelen - capturepos;
			if (remaining > framestep) remaining = framestep;
			if (remaining > 0)
				memcpy(capturebuf + capturepos, frame, remaining*sizeof(*frame));
			capturepos += remaining; 
			// Check for end of capture buf.
			if (capturepos == capturelen) {
				if (captureend == 0) captureend = capturepos;
				cout << "Speech exceeded capture duration. Use -t to increase."; 
				cout << "Too long…  " << capturepos << "\n";
				return captureend;
			}
			break;

		case EP_ENDOFUTT:		// confirmed end of utterance
			// This is a silence frame after the end of signal.  The previous
			// MAYBEEND frame was the actual end of utterance  

			cout << "EP_ENDOFUTT i : " << i << "\n";
			return captureend;
		} 
		framepos = framelen - framestep;
		memmove(frame, frame + framestep, framepos*sizeof(*frame));
		/* Shift the frame overlap to the start of the frame. */
		end++;
		if (i == capturelen){
			break;
		} 
	} 
	return 0;
}
void initendpoint()
{
	long	i;

	epstate = NOSILENCE;
	noise = 0.0;
	ave = 0.0;
	begthresh = 0.0;
	endthresh = begthresh;
	energy = 0.0;
	maxpeak = 0.0;
	scnt = 0;
	vcnt = 0;
	evcnt = 0;
	voicecount = 0;
	zccnt = 0;
	bscnt = 0;
	startframe = 0;
	endframe = 0;
	avescnt = 0;
	startsilenceok = false;
	ncount = 0;
	low = true;
	for (i = 0; i < numdpnoise; i++)
		lastdpnoise[i] = 0.0;
}
EPTAG getendpoint(short *samples)
{
	float	tmp;

	zcpeakpick(samples);			// get zc count and peak energy
	if (peakreturn > maxpeak)
	{
		maxpeak = peakreturn;
		if ((tmp = maxpeak / endfact) > endthresh)
			endthresh = tmp;
	}

	switch (epstate)
	{
	case NOSILENCE:				// start, get background silence
		ave += peakreturn;
		if (++scnt <= 3)
		{					// average 3 frame's worth
			if (scnt == 1)
				setnoise();
			else
				averagenoise();
			if (dpnoise < minstartsilence)
			{
				startsilenceok = true;
				ave += peakreturn;
				avescnt++;
			}
			cout << "test1\t";
			return (EP_SILENCE);
		}
		if (!startsilenceok)
		{
			epstate = START;
			return (EP_NOSTARTSILENCE);
		}
		ave /= avescnt;
		noise = ave;
		begthresh = noise + begfact;
		endthresh = begthresh;
		mnbe = noise * energyfact;
		epstate = INSILENCE;
		return (EP_SILENCE);

	case INSILENCE:
		ave = ((3.0 * ave) + peakreturn) / 4.0;
		//cout << "test2\t"; 
		if (peakreturn > begthresh || zc > zcthresh)
		{					// looks like start of signal
			energy += peakreturn - noise;
			if (zc > zcthresh)
				zccnt++;
			if (peakreturn > begthresh)
				voicecount++;
			if (++vcnt > minrise)
			{
				scnt = 0;
				epstate = START;		// definitely start of signal
			}
			return (EP_SIGNAL);
		}
		else
		{					// still in silence
			//cout << "test3\t"; 
			energy = 0.0;
			if (ave < noise)
			{
				noise = ave;
				begthresh = noise + begfact;
				endthresh = begthresh;
				mnbe = noise * energyfact;
			}
			if (vcnt > 0)
			{			// previous frame was signal
				if (++bscnt > startblip || zccnt == vcnt)
				{			// Oops, no longer in the signal
					noise = ave;
					begthresh = noise * begfact;
					endthresh = begthresh;
					mnbe = noise * energyfact;
					vcnt = 0;
					zccnt = 0;
					bscnt = 0;
					voicecount = 0;
					startframe = 0;
					return (EP_RESET);// not in the signal, ignore previous
				}
				return (EP_SIGNAL);
			}
			zccnt = 0;
			return (EP_SILENCE);
		}

	case START:
		if (peakreturn > begthresh || zc > zcthresh)
		{				// possible start of signal
			energy += peakreturn - noise;
			if (zc > zcthresh)
				zccnt++;
			if (peakreturn > begthresh)
				voicecount++;
			vcnt += scnt + 1;
			scnt = 0;
			if (energy > mnbe || zccnt > minfriclng)
			{
				epstate = INSIGNAL;
				cout << "EP_INUTT\t";
				return (EP_INUTT);
			}
			else{
				cout << "EP_SIGNAL\t";
				return (EP_SIGNAL);
			}
		}
		else
			if (++scnt > maxpause)
			{				// signal went low again, false start
				vcnt = zccnt = voicecount = 0;
				energy = 0.0;
				epstate = INSILENCE;
				ave = ((3.0 * ave) + peakreturn) / 4.0;
				if (ave < noise + begfact)
				{			// lower noise level
					noise = ave;
					begthresh = noise + begfact;
					endthresh = begthresh;
					mnbe = noise * energyfact;
				}
				cout << "EP_RESET\t";
				return (EP_RESET);
			}
			else{ 
				cout << "EP_SIGNAL\t";
				return (EP_SIGNAL);
			}
	case INSIGNAL:
		if (peakreturn > endthresh || zc > zcthresh)
		{				// still in signal
			if (peakreturn > endthresh)
				voicecount++;
			vcnt++;
			scnt = 0;
			cout << "EP_SIGNAL\t";
			return (EP_SIGNAL);
		}
		else
		{				// below end threshold, may be end
			scnt++;
			epstate = END; 
			cout << "EP_MAYBEEND\t";
			return (EP_MAYBEEND);
		}

	case END:
		if (peakreturn > endthresh || zc > zcthresh)
		{				// signal went up again, may not be end
			if (peakreturn > endthresh)
				voicecount++;
			if (++evcnt > endblip)
			{			// back in signal again
				vcnt += scnt + 1;
				evcnt = 0;
				scnt = 0;
				epstate = INSIGNAL;
				cout << "EP_NOTEND\t";
				return (EP_NOTEND);
			}
			else{
				cout << "EP_SIGNAL\t";
				return (EP_SIGNAL);
			}
		}
		else
			if (++scnt > maxipause)
			{				// silence exceeds inter-word pause
				if (vcnt > minuttlng && voicecount > minvoicelng){
					cout << "EP_ENDOFUTT\t";
					return (EP_ENDOFUTT);// end of utterance
				}
				else
				{			// signal is too short
					scnt = vcnt = voicecount = 0;
					epstate = INSILENCE;
					cout << "EP_RESET\t"; 
					return (EP_RESET);	// false utterance, keep looking
				}
			}
			else
			{				// may be an inter-word pause
				if (peakreturn == 0){
					cout << "EP_ENDOFUTT\t";
					return (EP_ENDOFUTT);// zero filler frame
				}
				evcnt = 0; cout << "EP_SIGNAL\t";
				return (EP_SIGNAL);	// assume still in signal
			}
	}
}
void zcpeakpick(short *samples)
{
	long	i;
	float	sum,
		trigger;
	short	*smp;

	for (sum = 0.0, i = 0, smp = samples; i < windowsize; i++, smp++)
		sum += *smp * *smp;
	peakreturn = (sqrt(sum / windowsize));
	lastdpnoise[0] = peakreturn;

	if (ncount == 0)
		dpnoise = peakreturn;			// initial value
	trigger = dpnoise * triggerfact;		// schmidt trigger band

	for (i = 0, zc = 0, smp = samples; i < windowsize; i++, smp++)
	{
		if (low)
		{
			if (*smp > trigger)
			{					// up cross
				zc++;
				low = false;			// search for down cross
			}
		}
		else
		{
			if (*smp < -trigger)
			{					// down cross
				zc++;
				low = true;			// search for up cross
			}
		}
	}
}
