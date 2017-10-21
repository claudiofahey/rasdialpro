
/* !CRF! - RasDialProCommon.h */

#define VERSION			"1.4"
#define COPYRIGHTYEAR	"2004"
#define APPNAME			"RasDial Pro"
#define EXENAME			"RDialPro"

#define REGCODESEED		7752017

#define SZREGKEYCOMPANY		 "SOFTWARE\\Mu-Consulting"
#define SZREGKEYCOMMON		 SZREGKEYCOMPANY "\\RasDialPro"

#define EVALDAYS			 30						// total number of days to evaluate
#define EVAL100NS			 ((ULONGLONG) EVALDAYS * 24 * 60 * 60 * 1000 * 1000 * 10)	// in units of 100 nanoseconds

#define SZWEBSITE			 "www.muconsulting.com"
