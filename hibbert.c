/*
 * Copyright (c) 2015, Daniel Wilkins
 *
 * Permission to use, copy, modify, and/or distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
 * WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY
 * SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES 
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
 * ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN
 * CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 */
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <unistd.h>
#include <getopt.h>

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
        ret.charging = false;

        fprintf(stderr, "Warning: unable to open %s\n", BATPATH);
        return ret;
    }
    fscanf(fp, "%ud1", &ret.charging);
#endif
    return ret;
}

void usage(void)
{
    printf("Usage: hibbert [-n -p percentage -t trigger -h -i interval]\nTrigger a script when battery reaches a certain percentage\n-i\tSpecify poll interval (seconds)\n-n\tDon't daemonize.\n-p\tSet percentage to trigger at.\n-t\tSet different trigger (defaults to ~/.hibbertt if it exists, /etc/hibbertt if not)\n");
}

int main(int argc, char *argv[])
{
    bool daemon = true;
    unsigned int triggerpercent = 10;
    unsigned int interval = 60; // 1 minute default poll time
    char const *home = getenv("HOME"); // prevent time of check/time of use
    char const *shell = "/bin/sh -c";
    int triggerlen = strlen(shell) + max(strlen(home) + strlen("/.hibbertt"), strlen("/etc/hibbertt")) + 1;
    char *trigger = malloc(triggerlen);
    *trigger = '\0';
    int lensize;
    int c;
    while ((c = getopt(argc, argv, "nhp:t:i:")) != -1)
    {
	switch (c)
	{
	case 'i':
	    sscanf(optarg, "%ud", &interval);
	    break;
	case 'n':
	    daemon = false;
	    break;
	case 'p':
	    sscanf(optarg, "%ud3", &triggerpercent);
	    break;
	case 't':
	    lensize = strlen(optarg) + 1;
	    if (lensize > triggerlen)
	    {
		triggerlen = lensize;
		trigger = realloc(trigger, triggerlen);
	    }
	    strncpy(trigger, optarg, triggerlen);
	    break;
	case 'h':
	    usage();
	    return 0;
	    break;
	}
    }
    if (daemon)
    {
	int p = fork();
	if (p != 0)
	    exit(1);

	if (setsid() < 0)
	    exit(1);
    }

    while (true)
    {
		struct battstate batt = read_battstate();
        if (!daemon)
            printf("%d %s\n", batt.percentage, batt.charging ? "charging" : "not charging");
		if ((batt.percentage <= triggerpercent) && batt.charging != CHARGING)
		{
			if (*trigger == '\0')
			{
				snprintf(trigger, triggerlen, "%s %s/.hibbertt", shell, home);
				FILE *fp = fopen(trigger, "r");
				if (!fp)
				{
					fprintf(stderr, "Unable to read ~/.hibbertt, trying system trigger\n");
					fp = fopen("/etc/hibbertt", "r");
					if (!fp)
					{
						fprintf(stderr, "Unable to open /etc/hibbertt either! Exiting\n");
						return 1;
					}
					snprintf(trigger, triggerlen, "%s /etc/hibbertt", shell);
				}
			}  			 
		system(trigger);
	}			 
	sleep(interval);
    }
    return 0;
}	
