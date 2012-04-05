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
volatile int reset;


/**
 * @brief   Funkce vykonana pri prijeti siganulu SIGUSR1 nmebo SIGUSR2.
 * @param   sig Cislo signalu.
 */
void catcher(int sig) {
   if(sig == SIGUSR2) {
      reset = 1;
   }
   else {
      intr = 1;
   }
}

int main(void)/*int argc, char *argv[])*/ {

   pid_t pid;
   struct sigaction sigact;                               /* struktura pro definici cinnost signalu */
   sigset_t block;                                        /* maska */
   sigset_t empty_set;                                    /* prazdna maska */
   char ch = 'A';
   intr = 0;
   reset = 0;
   sigemptyset(&block);                                   /* vyprazdneni mnoziny signalu */
   sigaddset(&block, SIGUSR1);                            /* pridani signalu do mnoziny signalu */
   sigaddset(&block, SIGUSR2);                            /* pridani signalu do mnoziny signalu */
   sigprocmask(SIG_BLOCK, &block, NULL);                  /* nastaveni masky blokovanych signalu */

   sigact.sa_handler = catcher;                           /* nastaveni habdleru signalu na danou fci */
   sigemptyset(&sigact.sa_mask);                          /* zablokovani signalu */
   sigact.sa_flags = 0;
   if(sigaction(SIGUSR1, &sigact, NULL) == -1) {          /* prirazeni kace pri rpijmuti signalu SIGUSR1*/
      perror("CHYBA: pro SIGUSR1  ve fce sigaction()\n");
      return 1;
   }
   if(sigaction(SIGUSR2, &sigact, NULL) == -1) {          /* Prirazeni funkce pr prijmuti signalu SIGUSR2 */
      perror("CHYBA: pro SIGUSR2 ve fci sigaction()\n");
      return 1;
   }
   sigprocmask(SIG_UNBLOCK, &block, NULL);                /* odblokovani signalu */

   pid = fork();                                          /* vytvoreni potomka */
   if(pid < 0) {                                          /* chyba */
      perror("CHYBA: Nepovedlo se vytvori potomka funkci fork()\n");
      return -1;
   }
   else if (pid > 0) {                                    /* pracuje otec */
      printf("Parent (%d): '%c'\n", (int) getpid(), (char) ch);
      ch++;                                               /* posunuti v abecede */
      sigemptyset(&empty_set);
      kill(pid, SIGUSR1);                                 /* poslani signalu synovi */
      while(1) {
         while (sigsuspend(&empty_set) == -1 && errno == EINTR) {
            if(intr)
               break;
         }
         sigprocmask(SIG_UNBLOCK, &block, NULL);
         printf("Press enter...");
         while(getchar() != '\n');                       /* cekam na entr */
         sigprocmask(SIG_BLOCK, &block, NULL);
         if(reset == 1) {                                /* kontrola prichodu signalu SIGUSR2 */
            ch = 'A';
            reset = 0;
         }
         printf("Parent (%d): '%c'\n", (int) getpid(), (char) ch);
         ch++;
         if(ch == '[') {                                 /* kontrola konce abecedy */
            ch = 'A';
         }
         intr = 0;
         kill(pid, SIGUSR1);
      }
   }
   else {                                                 /* syn - pid == 0 */
      sigemptyset(&empty_set);
      while(1) {
         while (sigsuspend(&empty_set) == -1) {
            if(intr)
               break;
         }
         sigprocmask(SIG_BLOCK, &block, NULL);
         if(reset == 1) {                                /* kontrola zda prisel reset na znak */
            ch = 'A';                                    /* nastavim na pocatecni hodnotu */
            reset = 0;
         }
         printf("Child  (%d): '%c'\n", (int) getpid(), (char) ch);
         ch++;                                           /* posun v abecede */
         if(ch == '[') {                                 /* kontrola zda jiz nejsme na konci abecedy */
            ch = 'A';                                    /* nastaveni pocatecni hodnoty */
         }
         intr = 0;
         kill(getppid(), SIGUSR1);                       /* zaslani signalu optci */
      }
   }

   return 0;
}

