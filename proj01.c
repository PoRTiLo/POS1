
#define _XOPEN_SOURCE
#define _XOPEN_SOURCE_EXTENDED 1 /* XPG 4.2 - needed for WCOREDUMP() */

#include <unistd.h>
#include <stdio.h>
#include <errno.h>
#include <signal.h>

volatile sig_atomic_t intr;

void catcher(int sig) {
   intr = 1;
}

int main(void)/*int argc, char *argv[])*/ {

   pid_t pid;
   struct sigaction sigact;                               /* struktura pro definici cinnost signalu */
   sigset_t block;
   char ch;
   sigemptyset(&block);                                   /* vyprazdneni mnoziny signalu */
   sigaddset(&block, SIGUSR1);                            /* pridani signalu do mnoziny signalu */
   sigprocmask(SIG_BLOCK, &block, NULL);                  /* nastaveni masky blokovanych signalu */

   sigact.sa_handler = catcher;                           /* nastaveni habdleru signalu na danou fci */
   sigemptyset(&sigact.sa_mask);                          /* zablokovani signalu */
   sigact.sa_flags = 0;
   if(sigaction(SIGUSR1, &sigact, NULL) == -1) {
      perror("sigaction()");
      return 1;
   }
   sigprocmask(SIG_UNBLOCK, &block, NULL);                /* odblokovani signalu */
   return 0;
}

