#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
//#include <strings.h>

#ifdef __APPLE__
#include <string.h>
#else
#include <strings.h>
#endif

#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <getopt.h>
#include <sys/types.h>          /* See NOTES */
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/ip.h> 
#include <errno.h>
#include <sys/types.h>
#include <sys/wait.h>



void usage(void);

#pragma pack(1)
struct netblock {
unsigned char flags;
unsigned char blhhh;
unsigned char blmmm;
unsigned char bllll;
unsigned char payload[512];
} NETBLOCK;
#pragma pack()

int main(int argc, char **argv)
{
    int dfdn = -1, sfdn = -1, ssfdn = -1, i;
    int p_opt = 0, clilen, childpid = -1, v_opt = 0;
    char *f_opt = NULL, c, *ptr;    
    struct sockaddr_in serv_addr, cli_addr;
    off_t offset =0;

    while ((i = getopt(argc, argv, "vp:f:?")) != -1) 
    {
        switch (i) {
            case 'p':
                    p_opt = atoi (optarg);
                    break;
            case 'f':
                    f_opt = optarg;
                    break;
            case 'v':
                    v_opt = 1;
                    break;
            default :
                    usage();
        }
    }

    if ((f_opt == NULL) || (p_opt == 0))
    {
        usage();
    }
    
    if((dfdn = open(f_opt, O_RDWR)) < 0)
    {
        fprintf(stderr, "*** Can't open disk file %s !\n", f_opt);
        return (1);
    }

    if ((sfdn = socket(AF_INET, SOCK_STREAM, 0)) < 0 )
    {
        fprintf(stderr, "*** Can't open socket !\n");
        return (2);
    }

    bzero((char *) &serv_addr, sizeof(serv_addr));
    serv_addr.sin_family    = AF_INET;
    serv_addr.sin_port      = htons (p_opt);
    serv_addr.sin_addr.s_addr  = htonl(INADDR_ANY);

    if (bind(sfdn, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0 )
    {
        fprintf(stderr, " Can't bind to local address  %d !\n", errno);
        return(3);
    }

    listen(sfdn, 5);
    
    for(;;)
    {
        /* wait for connection */
        clilen = sizeof(cli_addr);
        ssfdn = accept(sfdn, (struct sockaddr *) &cli_addr, &clilen);

        if (ssfdn < 0)
        {
            fprintf(stderr, "*** Accept failed! \n");
            return(4);
        }
     
        if ((childpid = fork()) < 0)
        {
            fprintf(stderr,"*** fork failed! \n");
            return(-1);
        }
        else if (childpid == 0) 
        { /* child */

            close (sfdn);
            fprintf(stderr,"New connection....\n"); 

            for(;;)
            {
                i = read(ssfdn, (char *)&NETBLOCK.flags, sizeof (NETBLOCK));
                if (v_opt)
                {
                     fprintf(stderr,"socket read %d bytes, flags: %02x\n", i, NETBLOCK.flags );  
                }
                if(i == -1)
                {
                   fprintf(stderr,"error occurred %d \n", errno);
                }
                if ( i == 0)  /* EOF */
                {
                    close (ssfdn);
                    return (0);
                }
                if ((i != 4) && (i != sizeof(NETBLOCK)))
                {       
                    fprintf(stderr,"*** Netblock: incorrect packet size!! %d  \n", i);
                    return(5);
                }   
                offset = (((NETBLOCK.blhhh << 16) + (NETBLOCK.blmmm << 8) + NETBLOCK.bllll) * 512);
                lseek (dfdn, offset, SEEK_SET);

                if ( ((NETBLOCK.flags & 0x10) == 0)) 
                { /* write */
    
                    fprintf(stderr,"write %3d bytes, flags: %02x, diskaddress (hex) %02x%02x%02x\n",
                        i, NETBLOCK.flags, NETBLOCK.blhhh, NETBLOCK.blmmm, NETBLOCK.bllll);  
                    if (v_opt)
                    {
                        fprintf(stderr, "wrote 512 bytes to disk\n");
                    }
                    write(dfdn, (char*)&NETBLOCK.payload, 512);
                }
                else
                { /*read */
                    fprintf(stderr,"read  %3d bytes, flags: %02x, diskaddress (hex) %02x%02x%02x\n",
                        i, NETBLOCK.flags, NETBLOCK.blhhh, NETBLOCK.blmmm, NETBLOCK.bllll);  
                    read (dfdn, (char *)&NETBLOCK.payload, 512) ;
                    if (v_opt)
                    {
                        fprintf(stderr, "read 512 bytes from disk\n");
                    }
                    write (ssfdn, (char *)&NETBLOCK.flags, 512 + 4);
                }
            }
        }       
        else
        {
/* parent */
        int status = 0;
        close(ssfdn);
/*        while((childpid != wait(&status)));  */
        }
    }
}

void usage(void)
{

    fprintf(stderr, "usage: Lin4UF  -p port -f <diskfile_image> -v (verbose) \n");
    exit(1);
}

