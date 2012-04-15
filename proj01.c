/**
 *       @file  proj01.c
 *      @brief  1. projekt do predmetu Pokrocile operacni systemy.
 *      Úlohou otce a syna je postupnì vypisovat rostoucí písmena abecedy (cyklicky A - Z) 
 *      tak, aby se otec se synem pravidelnì støídali. Støídání zajistìte vzájemným 
 *      zasíláním signálu SIGUSR1 - otec po¹le synovi signál v¾dy po vypsání znaku, stejnì 
 *      tak syn otci. Otec pøed vypsáním znaku nejdøíve zobrazí výzvu a poèká na stisk 
 *      klávesy, co¾ ov¹em uèiní a¾ po pøíjmu signálu SIGUSR1 od synovského procesu. Pro 
 *      vstup z klávesnice staèí pou¾ít funkci getchar() a reagovat jen na klávesu Enter. 
 *      První zaèíná otec (místo pid vypisujte skuteèné èíslo pøíslu¹ného procesu).
 *
 * Detailed description starts here.
 *
 *     @author  Bc. Jaroslav Sendler (xsendl00), xsendl00@stud.fit.vutbr.cz
 *
 *   @internal
 *     Created  04/05/2012
 *     Company  FIT-VUT, Brno
 *   Copyright  Copyright (c) 2012, Bc. Jaroslav Sendler
 *
 * =====================================================================================
 */


#define _XOPEN_SOURCE
#define _XOPEN_SOURCE_EXTENDED 1 /* XPG 4.2 - needed for WCOREDUMP() */

#include <unistd.h>
#include <stdio.h>
#include <errno.h>
#include <signal.h>

volatile int intr;
/*volatile int reset;*/
int ch;                                                    /* znak abaecedy */ 

/**
 * @brief   Funkce vykonana pri prijeti siganulu SIGUSR1 nmebo SIGUSR2.
 * @param   sig Cislo signalu.
 */
void catcher(int sig) {
   if(sig == SIGUSR2) {                                  /* prisel signal USR2 */
      ch = (int)'A';                                     /* resetuji abecedu */
   }
   else {                                                /* prisel signal USR1 */
      intr = 1;
   }
   return;
}

/**
 * @brief   Posunese v abecede o jeden znak dale, Pripdane zacne od zacatku.
 */   
void inc(void) {
   ch++;
   if(ch > 'Z') {                                        /* jsme an konci abecedy */
      ch = (int)'A';                                     /* pocatek abecedy */   
   }
   return;
}

int main(void)/*int argc, char *argv[])*/ {

   pid_t pid;
   struct sigaction sigact;                               /* struktura pro definici cinnost signalu */
   sigset_t block;                                        /* maska */
   sigset_t empty_set;                                    /* prazdna maska */
   ch = (int)'A';
   intr = 0;
   sigemptyset(&block);                                   /* vyprazdneni mnoziny signalu */
   sigaddset(&block, SIGUSR1);                            /* pridani signalu do mnoziny signalu */
   sigaddset(&block, SIGUSR2);                            /* pridani signalu do mnoziny signalu */
   sigprocmask(SIG_BLOCK, &block, NULL);                  /* nastaveni masky blokovanych signalu */

   sigact.sa_handler = catcher;                           /* nastaveni habdleru signalu na danou fci */
/*   sigemptyset(&sigact.sa_mask);*/                          /* zablokovani signalu */
   sigact.sa_mask = block;
   sigact.sa_flags = 0;
   if(sigaction(SIGUSR1, &sigact, NULL) == -1) {          /* prirazeni kace pri rpijmuti signalu SIGUSR1*/
      perror("CHYBA: pro SIGUSR1  ve fce sigaction()\n");
      return(1);
   }
   if(sigaction(SIGUSR2, &sigact, NULL) == -1) {          /* Prirazeni funkce pr prijmuti signalu SIGUSR2 */
      perror("CHYBA: pro SIGUSR2 ve fci sigaction()\n");
      return(1);
   }
/*   sigprocmask(SIG_UNBLOCK, &block, NULL); */               /* odblokovani signalu */

   pid = fork();                                          /* vytvoreni potomka */
   if(pid < 0) {                                          /* chyba */
      perror("CHYBA: Nepovedlo se vytvori potomka funkci fork()\n");
      return(-1);
   }
   else if (pid > 0) {                                    /* pracuje otec */
      int c;
      printf("Parent (%d): '%c'\n", (int) getpid(), (char) ch);
      inc();                                              /* posunuti v abecede */
      sigemptyset(&empty_set);
      if(kill(pid, SIGUSR1) == -1) {                      /* poslani signalu synovi */
         perror("CHYBA:%s\n");
         return -1;
      }
      while(1) {
         while( (sigsuspend(&empty_set) == -1) && errno == EINTR) {
            if(intr)
               break;
         }
         sigprocmask(SIG_UNBLOCK, &block, NULL);
         printf("Press enter...");
         while(( c = getchar()) != '\n');                 /* cekam na entr */
         sigprocmask(SIG_BLOCK, &block, NULL);
         printf("Parent (%d): '%c'\n", (int) getpid(), (char) ch);
         inc();                                           /* posun v abecede */
         intr = 0;
         if(kill(pid, SIGUSR1) == -1) {
            perror("CHYBA:%s\n");
            return -1;
         }
      }
   }
   else {                                                 /* syn - pid == 0 */
      sigemptyset(&empty_set);
      while(1) {
         while((sigsuspend(&empty_set) == -1) && errno == EINTR) {
            if(intr)
               break;
         }
         sigprocmask(SIG_BLOCK, &block, NULL);           /* blokovani */
         printf("Child  (%d): '%c'\n", (int) getpid(), (char) ch);
         inc();                                          /* inkrementace, posun v abecede */
         intr = 0;
         if(kill(getppid(), SIGUSR1) == -1) {            /* zaslani signalu otci */
            perror("CHYBA:%s\n");
            return -1;
         }
      }
   }

   return 0;
}

