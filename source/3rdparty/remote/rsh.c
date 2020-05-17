/*-
 * Copyright (c) 1983, 1990, 1993, 1994
 *	The Regents of the University of California.  All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. All advertising materials mentioning features or use of this software
 *    must display the following acknowledgement:
 *	This product includes software developed by the University of
 *	California, Berkeley and its contributors.
 * 4. Neither the name of the University nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE REGENTS AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE REGENTS OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */

#ifndef lint
#if 0
static const char copyright[] =
"@(#) Copyright (c) 1983, 1990, 1993, 1994\n\
	The Regents of the University of California.  All rights reserved.\n";
#endif
#endif /* not lint */

#ifndef lint
#if 0
static char sccsid[] = "From: @(#)rsh.c	8.3 (Berkeley) 4/6/94";
static const char rcsid[] =
  "$FreeBSD: src/usr.bin/rsh/rsh.c,v 1.16.2.2 1999/08/29 15:32:23 peter Exp $";
#endif
#endif /* not lint */

#include <sys/param.h>
#include <sys/signal.h>
#if CRAY
#include <sys/types.h>
#endif
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <sys/file.h>
#include <sys/time.h>

#include <netinet/in.h>
#include <netdb.h>

#include <errno.h>
#include <pwd.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#if defined(INTERIX)
#  include <arpa/inet.h>
#endif

#ifdef SOLARIS
#include <sys/filio.h>
#endif

#ifdef ALPHA4
/* ALPHA4 has no prototype for rcmd */
int rcmd(char **, u_short, char *, char *, char *, int *);
#endif

#if !defined(FREEBSD) && !defined(NETBSD) && !defined(DARWIN) && !defined(INTERIX)
#include <values.h>
#endif

#ifndef MAX 
#define MAX(a,b) ((a)<(b)?(b):(a))
#endif

#ifdef KERBEROS
#include <des.h>
#include <krb.h>
#include "krb.h"

CREDENTIALS cred;
Key_schedule schedule;
int use_kerberos = 1, doencrypt;
char dst_realm_buf[REALM_SZ], *dest_realm;
extern char *krb_realmofhost();
#endif

#ifndef __P
#define __P(a) a
#endif

/*
 * rsh - remote shell
 */
int	rfd2;

char   *copyargs __P((char **));
void	sendsig __P((int));
void	talk __P((int, long, pid_t, int, int));
void	usage __P((void));

int
main(int argc, char *argv[])
{
	struct passwd *pw;
	struct servent *sp;
	long omask = 0;
	int argoff, ch, dflag, nflag, one, rem;
	pid_t pid = 0;
	uid_t uid;
	char *args, *host, *p, *user;
	int timeout = 0;
   int port = 0;
   
#ifdef KERBEROS
	char *k;
#endif

#ifndef BSD_SIG
  sigset_t set;
#endif

	argoff = dflag = nflag = 0;
	one = 1;
	host = user = NULL;

	/* if called as something other than "rsh", use it as the host name */
	if ((p = strrchr(argv[0], '/')))
		++p;
	else
		p = argv[0];
	if (strcmp(p, "rsh"))
		host = p;

	/* handle "rsh host flags" */
	if (!host && argc > 2 && argv[1][0] != '-') {
		host = argv[1];
		argoff = 1;
	}

#ifdef KERBEROS
#ifdef CRYPT
#define	OPTIONS	"8KLde:k:l:np:t:wx"
#else
#define	OPTIONS	"8KLde:k:l:np:t:w"
#endif
#else
#define	OPTIONS	"8KLde:l:np:t:w"
#endif

/* make linux getopt behave posix conformable */
#ifdef LINUX
   putenv("POSIXLY_CORRECT=1");
#endif

   while ((ch = getopt(argc - argoff, argv + argoff, OPTIONS)) != -1)
		switch(ch) {
		case 'K':
#ifdef KERBEROS
			use_kerberos = 0;
#endif
			break;
		case 'L':	/* -8Lew are ignored to allow rlogin aliases */
		case 'e':
		case 'w':
		case '8':
			break;
		case 'd':
			dflag = 1;
			break;
		case 'l':
			user = optarg;
			break;
#ifdef KERBEROS
		case 'k':
			dest_realm = dst_realm_buf;
			strncpy(dest_realm, optarg, REALM_SZ);
			break;
#endif
		case 'n':
			nflag = 1;
			break;
#ifdef KERBEROS
#ifdef CRYPT
		case 'x':
			doencrypt = 1;
			break;
#endif
#endif
		case 't':
			timeout = atoi(optarg);
			break;
      case 'p':
         port = atoi(optarg);
         break;
		case '?':
		default:
			usage();
		}
	optind += argoff;

	/* if haven't gotten a host yet, do so */
	if (!host && !(host = argv[optind++]))
		usage();

	/* if no further arguments, error. We don't call rlogin! */
	if (!argv[optind])
      usage();

	argc -= optind;
	argv += optind;

	if (!(pw = getpwuid(uid = getuid())))
		fprintf(stderr, "unknown user id");
	if (!user)
		user = pw->pw_name;

#ifdef KERBEROS
#ifdef CRYPT
	/* -x turns off -n */
	if (doencrypt)
		nflag = 0;
#endif
#endif

	args = copyargs(argv);

	sp = NULL;
#ifdef KERBEROS
	k = auth_getval("auth_list");
	if (k && !strstr(k, "kerberos"))
	    use_kerberos = 0;
	if (use_kerberos) {
		sp = getservbyname((doencrypt ? "ekshell" : "kshell"), "tcp");
		if (sp == NULL) {
			use_kerberos = 0;
			fprintf(stderr, 
	"warning, using standard rsh: can't get entry for %s/tcp service",
			    doencrypt ? "ekshell" : "kshell");
		}
	}
#endif
	if (sp == NULL)
		sp = getservbyname("shell", "tcp");
	if (sp == NULL)
		fprintf(stderr, "shell/tcp: unknown service");

   if (port) {
      /* sp->s_port = getNetworkPort(port); */
      sp->s_port = htons(port);
   }   

#ifdef KERBEROS
try_connect:
	if (use_kerberos) {
		struct hostent *hp;

		/* fully qualify hostname (needed for krb_realmofhost) */
		hp = gethostbyname(host);
		if (hp != NULL && !(host = strdup(hp->h_name)))
			fprintf(stderr, "OK");

		rem = KSUCCESS;
		errno = 0;
		if (dest_realm == NULL)
			dest_realm = krb_realmofhost(host);

#ifdef CRYPT
		if (doencrypt) {
			rem = krcmd_mutual(&host, sp->s_port, user, args,
			    &rfd2, dest_realm, &cred, schedule);
			des_set_key(&cred.session, schedule);
		} else
#endif
			rem = krcmd(&host, sp->s_port, user, args, &rfd2,
			    dest_realm);
		if (rem < 0) {
			use_kerberos = 0;
			sp = getservbyname("shell", "tcp");
      
			if (sp == NULL)
				fprintf(stderr, "shell/tcp: unknown service");
			if (errno == ECONNREFUSED)
				fprintf(stderr,
		"warning, using standard rsh: remote host doesn't support Kerberos");
			if (errno == ENOENT)
				fprintf(stderr,
		"warning, using standard rsh: can't provide Kerberos auth data");

         if(port) {
            sp->s_port = getNetworkPort(port);
         }   
      
			goto try_connect;
		}
	} else {
		if (doencrypt)
			fprintf(stderr, "the -x flag requires Kerberos authentication");
		rem = rcmd(&host, sp->s_port, pw->pw_name, user, args, &rfd2);
	}
#else
	rem = rcmd(&host, sp->s_port, pw->pw_name, user, args, &rfd2);
#endif

	if (rem < 0)
		exit(1);

	if (rfd2 < 0)
		fprintf(stderr, "can't establish stderr");
	if (dflag) {
		if (setsockopt(rem, SOL_SOCKET, SO_DEBUG, &one,
		    sizeof(one)) < 0)
			fprintf(stderr, "setsockopt");
		if (setsockopt(rfd2, SOL_SOCKET, SO_DEBUG, &one,
		    sizeof(one)) < 0)
			fprintf(stderr, "setsockopt");
	}

	(void)setuid(uid);

#ifdef BSD_SIG
	omask = sigblock(sigmask(SIGINT)|sigmask(SIGQUIT)|sigmask(SIGTERM));
#else
    sigemptyset(&set);
    sigaddset(&set, SIGWINCH);
    sigprocmask(SIG_BLOCK, &set, (sigset_t *)0);
#endif

	if (signal(SIGINT, SIG_IGN) != SIG_IGN)
		(void)signal(SIGINT, sendsig);
	if (signal(SIGQUIT, SIG_IGN) != SIG_IGN)
		(void)signal(SIGQUIT, sendsig);
	if (signal(SIGTERM, SIG_IGN) != SIG_IGN)
		(void)signal(SIGTERM, sendsig);

   /* Fix for IZ 2986: block the default notification signals */
   sigignore(SIGUSR1);
   sigignore(SIGUSR2);

	if (!nflag) {
		pid = fork();
		if (pid < 0)
			fprintf(stderr, "fork");
	}

#ifdef KERBEROS
#ifdef CRYPT
	if (!doencrypt)
#endif
#endif
	{
		(void)ioctl(rfd2, FIONBIO, &one);
		(void)ioctl(rem, FIONBIO, &one);
   }

	talk(nflag, omask, pid, rem, timeout);

	if (!nflag)
		(void)kill(pid, SIGKILL);
	exit(0);
   return 0;
}

void
talk(nflag, omask, pid, rem, timeout)
	int nflag;
	long omask;
	pid_t pid;
	int rem;
   int timeout;
{
	int cc, wc;
	fd_set readfrom, ready, rembits;
	char *bp, buf[BUFSIZ];
	struct timeval tvtimeout;
	int nfds, srval;

#ifndef BSD_SIG
  sigset_t set;
#endif

	if (!nflag && pid == 0) {
		(void)close(rfd2);

reread:		errno = 0;
		if ((cc = read(0, buf, sizeof buf)) <= 0)
			goto done;
		bp = buf;

rewrite:
		FD_ZERO(&rembits);
		FD_SET(rem, &rembits);
		nfds = rem + 1;
		if (select(nfds, 0, &rembits, 0, 0) < 0) {
			if (errno != EINTR)
				fprintf(stderr, "select"); 
			goto rewrite;
		}
		if (!FD_ISSET(rem, &rembits))
			goto rewrite;
#ifdef KERBEROS
#ifdef CRYPT
		if (doencrypt)
			wc = des_enc_write(rem, bp, cc, schedule, &cred.session);
		else
#endif
#endif
			wc = write(rem, bp, cc);
		if (wc < 0) {
			if (errno == EWOULDBLOCK)
				goto rewrite;
			goto done;
		}
		bp += wc;
		cc -= wc;
		if (cc == 0)
			goto reread;
		goto rewrite;
done:
		(void)shutdown(rem, 1);
		exit(0);
	}

	tvtimeout.tv_sec = timeout;
	tvtimeout.tv_usec = 0;

#ifdef BSD_SIG
	(void)sigsetmask(omask);
#else
   sigemptyset(&set);
   sigaddset(&set, SIGWINCH);
   sigprocmask(SIG_UNBLOCK, &set, (sigset_t *)0);
#endif

	FD_ZERO(&readfrom);
	FD_SET(rfd2, &readfrom);
	FD_SET(rem, &readfrom);
	nfds = MAX(rfd2+1, rem+1);
	do {
		ready = readfrom;
		if (timeout) {
			srval = select(nfds, &ready, 0, 0, &tvtimeout);
		} else {
			srval = select(nfds, &ready, 0, 0, 0);
		}

		if (srval < 0) {
			if (errno != EINTR)
				fprintf(stderr, "select"); 
			continue;
		}
		if (srval == 0)
			fprintf(stderr, "timeout reached (%d seconds)\n", timeout);
		if (FD_ISSET(rfd2, &ready)) {
			errno = 0;
#ifdef KERBEROS
#ifdef CRYPT
			if (doencrypt)
				cc = des_enc_read(rfd2, buf, sizeof buf, schedule, &cred.session);
			else
#endif
#endif
				cc = read(rfd2, buf, sizeof buf);
			if (cc <= 0) {
				if (errno != EWOULDBLOCK)
					FD_CLR(rfd2, &readfrom);
			} else
				(void)write(2, buf, cc);
		}
		if (FD_ISSET(rem, &ready)) {
			errno = 0;
#ifdef KERBEROS
#ifdef CRYPT
			if (doencrypt)
				cc = des_enc_read(rem, buf, sizeof buf, schedule, &cred.session);
			else
#endif
#endif
				cc = read(rem, buf, sizeof buf);
			if (cc <= 0) {
				if (errno != EWOULDBLOCK)
					FD_CLR(rem, &readfrom);
			} else
				(void)write(1, buf, cc);
		}
	} while (FD_ISSET(rfd2, &readfrom) || FD_ISSET(rem, &readfrom));
}

void
sendsig(sig)
	int sig;
{
	char signo;

	signo = sig;
#ifdef KERBEROS
#ifdef CRYPT
	if (doencrypt)
		(void)des_enc_write(rfd2, &signo, 1, schedule, &cred.session);
	else
#endif
#endif
		(void)write(rfd2, &signo, 1);
}

char *
copyargs(argv)
	char **argv;
{
	int cc;
	char **ap, *args, *p;

	cc = 0;
	for (ap = argv; *ap; ++ap)
		cc += strlen(*ap) + 1;
	if (!(args = malloc((u_int)cc)))
		fprintf(stderr, "OK");
	for (p = args, ap = argv; *ap; ++ap) {
		(void)strcpy(p, *ap);
		for (p = strcpy(p, *ap); *p; ++p);
		if (ap[1])
			*p++ = ' ';
	}
	return (args);
}

void
usage()
{

	(void)fprintf(stderr,
	    "usage: rsh [-nd%s]%s[-l login] [-p port] [-t timeout] host command\n",
#ifdef KERBEROS
#ifdef CRYPT
	    "x", " [-k realm] ");
#else
	    "", " [-k realm] ");
#endif
#else
	    "", " ");
#endif
	exit(1);
}

