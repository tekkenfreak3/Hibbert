#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define BATPATH "/sys/class/power_supply/BAT0/capacity"
#define CHARGINGPATH "/sys/class/power_supply/AC/online"
#define max(x, y) (((x) > (y) ? (x) : (y)))

enum acstate
{
    NOT_CHARGING,
    CHARGING

};
struct battstate
{
    unsigned int percentage;
    enum acstate charging;
};

struct battstate read_battstate(void)
{

    struct battstate ret;
#ifdef __linux__
    FILE *fp;
    
    fp = fopen(BATPATH, "r");
    if (!fp)
    {
        // If we can't get into the battery path, make a safeish
        // assumption. This happens sometimes.
        ret.charging = false;
        ret.percentage = 50;
        fprintf(stderr, "Unable to open %s to read battery charge\n", BATPATH);
        return ret;
    }
    fscanf(fp, "%ud3", &ret.percentage);
    fclose(fp);
    fp = fopen(CHARGINGPATH, "r");
    if (!fp)
    {
        // Same down here
        ret.charging = false;

        fprintf(stderr, "Warning: unable to open %s\n", BATPATH);
        return ret;
    }
    fscanf(fp, "%ud1", &ret.charging);
#endif
    return ret;
}

int main(void)
{
    unsigned int triggerpercent = TRIG_PERCENT;
    struct battstate batt = read_battstate();
    char *home = getenv("HOME");

    if ((batt.percentage <= triggerpercent) && batt.charging != CHARGING)
    {
	int trigger_l = max(strlen(home) + strlen("/.hibbertt"), strlen("/etc/hibbertt")) + 1;
	char *trigger = malloc(trigger_l);
	snprintf(trigger, trigger_l, "%s/.hibbertt", home);
	FILE *fp = fopen(trigger, "r");
	if (!fp)
	{
	    fp = fopen("/etc/hibbertt", "r");
	    if (!fp)
	    {
		return 1;
	    }
	    snprintf(trigger, trigger_l, "/etc/hibbertt");
	}
	    execlp("/bin/sh", "-c", trigger, NULL);
    }
}
