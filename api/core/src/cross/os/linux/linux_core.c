
#include <cross/cross_os_api.h>
#include <stdarg.h>

#include <termios.h>
#include <unistd.h>
#include <dirent.h>

#include <unistd.h> 
#include <termios.h> 
#include <sys/ioctl.h>

typedef struct _linux_ffdir
{
    struct dirent **files;
    cross_ffind ffile;
    int32_t idx;
    char full_path[512];
}linux_ffdir,*Plinux_ffdir;

extern void enable_raw_mode();
extern void disable_raw_mode();
extern int32_t kbhit();
extern int32_t getch(void);

void cross_os_printf(const char* fmt, ...)
{
    va_list arg;
    va_start(arg, fmt);
    vprintf(fmt, arg);
    va_end(arg);
}

void cross_os_error(const char* fmt, ...)
{
    va_list arg;
    va_start(arg, fmt);
    vprintf(fmt, arg);
    va_end(arg);
    while(1);
}

int32_t cross_os_get_ch_no_wait(void)
{
	int nchar = -1;
    enable_raw_mode();
    nchar = kbhit();
    disable_raw_mode();
    if(nchar)
    {
        nchar = getch();
		if (nchar == 8)nchar = 0x7F;
    }else nchar = -1;
	return nchar;
}

void cross_os_get_current_path(char* path, int32_t buflen)
{
    char* lastslash = NULL;
    char* ptrpath = getcwd(path,buflen);
    while (*ptrpath != '\0')
    {
        if(*ptrpath == '/')lastslash = ptrpath;
        ptrpath++;
    }

    if (lastslash != NULL)
    {
        lastslash++;
        *lastslash = '\0'; 
    }
}

vatek_result cross_os_findfile_first(hcross_ffind* hffind,const char* path, Pcross_ffind* pfind)
{
    return vatek_unsupport;
}

vatek_result cross_os_findfile_next(hcross_ffind hffind, Pcross_ffind* pfind)
{
    return vatek_unsupport;
}

void cross_os_findfile_close(hcross_ffind hffind)
{

}

/* implement kbhit */

void enable_raw_mode()
{
    struct termios term;
    tcgetattr(0, &term);
    term.c_lflag &= ~(ICANON | ECHO); // Disable echo as well
    tcsetattr(0, TCSANOW, &term);
}

void disable_raw_mode()
{
    struct termios term;
    tcgetattr(0, &term);
    term.c_lflag |= ICANON | ECHO;
    tcsetattr(0, TCSANOW, &term);
}

int32_t kbhit()
{
    int byteswaiting;
    ioctl(0, FIONREAD, &byteswaiting);
    return byteswaiting > 0;
}

int32_t getch(void)
{
    int ch;
    struct termios oldt, newt;
 
    tcgetattr(STDIN_FILENO, &oldt);
    memcpy(&newt, &oldt, sizeof(newt));
    newt.c_lflag &= ~( ECHO | ICANON | ECHOE | ECHOK |
                       ECHONL | ECHOPRT | ECHOKE | ICRNL);
    tcsetattr(STDIN_FILENO, TCSANOW, &newt);
    ch = getchar();
    tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
 
    return ch;
}