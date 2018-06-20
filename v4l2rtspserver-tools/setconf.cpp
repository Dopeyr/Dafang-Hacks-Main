#include<stdio.h>
#include <getopt.h>
#include <cstdlib>
#include "sharedmem.h"

#define SETGETSHAREDMEMORYINT(INT) if (get) printf("%d\n",  INT); else INT = atoi(value);
#define SETGETSHAREDMEMORYSTRING(STR) if (get) printf("%s\n",  STR); else  strcpy(STR,value);
#define SETGETSHAREDMEMORYBOOL(INT) if (get) printf("%s\n",  INT?"true":"false"); else INT= strToBool(value);

int stringToInts(char *str, int region[4])
{
    int i = 0;
    char *pt = strtok (str,",");
    while ((pt != NULL) &&
            i < sizeof(region)) {
        int a = atoi(pt);
        region[i] = a;
        i++;
        pt = strtok (NULL, ",");
    }
    return (i == sizeof(region));
}

bool strToBool(char *str)
{
    if (strcasecmp(str, "true") == 0) return(true);
    if (strcasecmp(str, "on") == 0) return(true);
    if (strcasecmp(str, "false") == 0) return(false);
    if (strcasecmp(str, "off") == 0) return(false);
    if (atoi(str) == 1) return (true);
    return false;
}

void usage(char *command)
{
    fprintf(stderr, "Usage %s -g -k KEY -v VALUE\n", command);
    fprintf(stderr, "Where k to set value, g to get the value\n");
    fprintf(stderr, "\t'f' flip mode set to\n");
    fprintf(stderr, "\t\t'1' -> flip on\n");
    fprintf(stderr, "\t\t'0' -> flip off\n");

    fprintf(stderr, "\t'n' night mode set to\n");
    fprintf(stderr, "\t\t'1' -> night mode on\n");
    fprintf(stderr, "\t\t'0' -> night mode off\n");

    fprintf(stderr, "\t'b' set bitrate to VALUE\n");
    fprintf(stderr, "\t'o' OSD text set to VALUE\n");
    fprintf(stderr, "\t'c' OSD color set VALUE to\n");
    fprintf(stderr, "\t\t'0' for White\n");
    fprintf(stderr, "\t\t'1' for Black\n");
    fprintf(stderr, "\t\t'2' for Red\n");
    fprintf(stderr, "\t\t'3' for Green\n");
    fprintf(stderr, "\t\t'4 for Blue\n");
    fprintf(stderr, "\t\t'5' for Cyan\n");
    fprintf(stderr, "\t\t'6' for Yellow\n");
    fprintf(stderr, "\t\t'7' for Purple\n");

    fprintf(stderr, "\t'x' OSD position Y pos is set to VALUE\n");
    fprintf(stderr, "\t'p' OSD space between char is set to VALUE (can be negative)\n");
    fprintf(stderr, "\t'w' fixed text width (0 variable, 1 fixed)\n");

    fprintf(stderr, "\t'm' motion sensitivity (0 to 4) -1 to deactivate motion\n");
    fprintf(stderr, "\t'z' display a circle when motion detected (-1 deactivated, use the OSD color numbers)\n");
    fprintf(stderr, "\t'r' set detection region (shall be: x0,y0,x1,y1) the server need to be restarted to take into account the new value\n");
    fprintf(stderr, "\t't' set tracking on/off (detection region is not taken into account anymore)\n");
    fprintf(stderr, "\t'u' set time before launching script after no motion (to restore camera position) -1 to deactivate\n");

    fprintf(stderr, "\t'h' set hardware volume (from mic)\n");
    fprintf(stderr, "\t'i' set software volume percentage (X will add X% to the data, from 0 to xxx, -1 to do nothing)\n");
    fprintf(stderr, "\t'q' set set filter number (1 or 2, 0 no filter)\n");
    fprintf(stderr, "\t'l' set set highpass filter on/off\n");


    fprintf(stderr, "Example: to set osd text: %s -k o -v OSDTEXT\n", command);
    fprintf(stderr, "         to get osd text: %s -g o\n", command);

}

void validate_video_setting_or_exit(char *name, char *value) {
    if (strcmp(name, "video-max-qp") == 0 || strcmp(name, "video-min-qp") == 0) {
        if (atoi(value) < 1 || atoi(value) > 51) {
            printf("%s must be between 1 and 51\n", name);
            exit(EXIT_FAILURE);
        }
    }

    if (strcmp(name, "video-quality-level") == 0) {
        if (atoi(value) < 0 || atoi(value) > 7) {
            printf("%s must be between 0 and 7 (0 = best)\n", name);
            exit(EXIT_FAILURE);
        }
    }
}

int main(int argc, char *argv[]) {

    char *key = NULL;
    char *value = NULL;
    bool get = false;

    // Parse Arguments:
    int c;
    while ((c = getopt(argc, argv, "g:k:v:")) != -1) {
        switch (c) {
            case 'g':
                    get = true;
                    // no break on purpose, execute 'k' case
            case 'k':
                if (key == 0) {
                    key = optarg;
                } else {
                    printf("Can not use g and k values at the same time\n");
                    usage(argv[0]);
                    exit(EXIT_FAILURE);
                }
                break;

            case 'v':
                value = optarg;
                break;

            default:
                printf("Invalid option %c\n", c);
                usage(argv[0]);
                exit(EXIT_FAILURE);
        }
    }
    SharedMem &mem = SharedMem::instance();
    shared_conf *conf = mem.getConfig();
    //printf("%d,%d\n", conf->nightmode, conf->flip);
    mem.readConfig();
    //printf("%d,%d\n", conf->nightmode, conf->flip);
    if (strcmp(key, "f") == 0) {
        SETGETSHAREDMEMORYINT(conf->flip );
    } else if (strcmp(key, "n") == 0) {
        SETGETSHAREDMEMORYINT(conf->nightmode);
    } else if (strcmp(key, "b") == 0) {
        SETGETSHAREDMEMORYINT(conf->bitrate);
    } else if (strcmp(key, "o") == 0) {
        SETGETSHAREDMEMORYSTRING(conf->osdTimeDisplay);
    } else if (strcmp(key, "c") == 0) {
        SETGETSHAREDMEMORYINT(conf->osdColor);
    } else if (strcmp(key, "s") == 0) {
        SETGETSHAREDMEMORYINT(conf->osdSize);
    } else if (strcmp(key, "x") == 0) {
        SETGETSHAREDMEMORYINT(conf->osdPosY);
    } else if (strcmp(key, "p") == 0) {
        SETGETSHAREDMEMORYINT(conf->osdSpace);
    } else if (strcmp(key, "w") == 0) {
        SETGETSHAREDMEMORYINT(conf->osdFixedWidth);
    } else if (strcmp(key, "m") == 0) {
        SETGETSHAREDMEMORYINT(conf->sensitivity);
    } else if (strcmp(key, "z") == 0) {
        SETGETSHAREDMEMORYINT(conf->motionOSD);
    } else if (strcmp(key, "r") == 0) {
        if (get) {
            printf("%d,%d,%d,%d\n", conf->detectionRegion[0], conf->detectionRegion[1],conf->detectionRegion[2],conf->detectionRegion[3]);
        } else {
            stringToInts(value, conf->detectionRegion);
        }
    } else if (strcmp(key, "t") == 0) {
        SETGETSHAREDMEMORYBOOL(conf->motionTracking);
    } else if (strcmp(key, "u") == 0) {
        SETGETSHAREDMEMORYINT(conf->motionTimeout);
    } else if (strcmp(key, "h") == 0) {
        SETGETSHAREDMEMORYINT(conf->hardVolume);
    } else if (strcmp(key, "i") == 0) {
        SETGETSHAREDMEMORYINT(conf->softVolume);
    } else if (strcmp(key, "q") == 0) {
        SETGETSHAREDMEMORYINT(conf->filter);
    } else if (strcmp(key, "l") == 0) {
        SETGETSHAREDMEMORYBOOL(conf->highfilter);
    } else if (strcmp(key, "video-max-qp") == 0) {
        validate_video_setting_or_exit(key, value);
        SETGETSHAREDMEMORYINT(conf->maxQp);
    } else if (strcmp(key, "video-min-qp") == 0) {
        validate_video_setting_or_exit(key, value);
        SETGETSHAREDMEMORYINT(conf->minQp);
    } else if (strcmp(key, "video-quality-level") == 0) {
        validate_video_setting_or_exit(key, value);
        SETGETSHAREDMEMORYINT(conf->qualityLvl);
    } else {
        printf("Invalid Argument %s\n", key);
        usage(argv[0]);
        exit(EXIT_FAILURE);
    }
    mem.setConfig();

    return 0;
}

