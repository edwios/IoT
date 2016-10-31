#include "application.h"

#ifndef LM61_h
#define LM61_h
#endif

#define LM61_DEFAULT_TIMEOUT_MS 500
#define LM61_TIMESYNC_DEFAULT_TIMEOUT_MS 10000
#define LM61_HTTP_DEFAULT_TIMEOUT_MS 10000

#define LM61_HTTP_GET   0
#define LM61_HTTP_POST  1
#define LM61_HTTP_HEAD  2

#define GPSXTRA

//#define MODEM_DEBUG

class LM61
{
	public:
		LM61(uint8_t rst, uint8_t pwr);
		bool begin();
		bool init();
		bool poke();
		void end();

        // Socket Send
        uint8_t socketSend(char *s);
		// SIM
		uint8_t unlockSIM(char *pin);

		// Cellular Network
		uint8_t getNetworkStatus(void);
		uint8_t getRSSI(void);
		uint8_t getNetworkMode(void);
		void clearSMS(void);
		
		// Time
		bool readRTC(char *datetime);
		bool enableNetworkTimeSync(bool onoff);
		bool syncNetworkTime(char *s, uint16_t timeout = LM61_TIMESYNC_DEFAULT_TIMEOUT_MS);

		// IMEI
		//uint8_t getIMEI(char *imei);
		// GPRS handling
		bool enableGPRS(bool onoff);
		uint8_t GPRSstate(void);
		void setGPRSNetworkSettings(char *apn, char *username=0, char *password=0);

		// GPS handling
		bool enableGPS(bool onoff);
		int8_t GPSstatus(void);
		int GPSsignal(void);
		uint8_t getGPS(uint8_t arg, char *buffer, uint8_t maxbuff);
		bool getGPS(char *gpsbuffer, float *lat, float *lon, char *gpsdate, char *gpstime, float *speed_kph=0, float *heading=0, float *altitude=0);

		// HTTP high level interface (easier to use, less flexible).
		bool HTTP_GET(char *host, uint16_t port, char *path, uint16_t *status, uint16_t *datalen);
		bool HTTP_POST(char *host, uint16_t port, char *path, char *body, uint16_t *status, uint32_t timeout = LM61_HTTP_DEFAULT_TIMEOUT_MS, bool getJSON = false, char *json = NULL);
		void setUserAgent(char *useragent);
		// HTTPS
		void setHTTPSRedirect(bool onoff);


	private:
		int8_t _rstpin, _pwrpin;
		char replybuffer[255];
		char *apn;
		char *apnusername;
		char *apnpassword;
		char *useragent;
		char *ok_reply;
		char *nonsms_reply;
		bool httpsredirect;

		// HTTP helpers
		bool HTTP_setup(char *url);

		void flushInput();
		uint8_t readline(uint16_t timeout = LM61_DEFAULT_TIMEOUT_MS, bool multiline = false);
		uint8_t getReply(char *send, uint16_t timeout = LM61_DEFAULT_TIMEOUT_MS);
        uint8_t getReplyQuoted(char *prefix, char *suffix, uint16_t timeout = LM61_DEFAULT_TIMEOUT_MS);
		bool sendParseReply(char *tosend, char *toreply, uint16_t *v, char divider = ',', uint8_t index = 0);
		bool parseReply(char *toreply, uint16_t *v, char divider = ',', uint8_t index = 0);
		bool parseReply(char *toreply, float *f, char divider = ',', uint8_t index = 0);
		bool parseReplyQuoted(char *toreply, char *v, int maxlen, char divider = ',', uint8_t index = 0);
		bool sendCheckReply(char *send, char *reply, uint16_t timeout = LM61_DEFAULT_TIMEOUT_MS);
		bool sendCheckReplyStartsWith(char *send, char *reply, uint16_t timeout = LM61_DEFAULT_TIMEOUT_MS);
		bool sendCheckReplyQuoted(char *prefix, char *suffix, char *reply, uint16_t timeout = LM61_DEFAULT_TIMEOUT_MS);
		bool expectReply(char *reply, uint16_t timeout = LM61_DEFAULT_TIMEOUT_MS);
		bool expectReplyStartsWith(char *reply, uint16_t timeout = LM61_DEFAULT_TIMEOUT_MS);
		bool sendExpectReply(char *send, char *reply, uint16_t timeout = LM61_DEFAULT_TIMEOUT_MS);


};

