/*
** FILE NAME:   uccpapp.c
**
** PROJECT:     WLAN
**
** AUTHOR:      Imagination Technologies
**
** DESCRIPTION: Test application to verify firmware loading utility
**				Also includes GRAM read/write tests
**
** NOTICE:      Copyright (C) 2013, Imagination Technologies Ltd.
*/
#define METAG_ALL_VALUES 
#include <metag/machine.inc>   
#include <metag/metagtbi.h> 
#include <MeOS.h> 

#ifdef __UCCP420_28__
#define GRAM_LEN 	0x1C3800
#endif

#ifdef __UCCP420_33__
#define GRAM_LEN 	0x66CC0
#endif

#ifdef __UCCP420_37__
#define GRAM_LEN 	0x66CC0
#endif

#ifdef __UCCP420_38__
#define GRAM_LEN 	0x8D180
#endif

#define GRAM_START	0xB7000000

#ifdef MEM_TEST
#ifdef GRAM_SWEEP
#define GRAM_SIZE 	GRAM_LEN
#else
#define GRAM_SIZE	512
#endif
#endif

#ifdef __ENABLE_MEOS_TIMERTASK__
#define TIMER_STACKSIZE         1024
unsigned int              timstack[TIMER_STACKSIZE];
#endif

static KRN_TASKQ_T        hibQ ; 
static KRN_SCHEDULE_T     sched;
static KRN_TASKQ_T        schedQueues[5];

volatile unsigned int	  buffer[1024];
volatile unsigned int     loopcount;


#ifdef __ENABLE_MEOS_TIMERTASK__
KRN_TIMERISR_T		TISR;
volatile unsigned long jiffies;

int timerISR(void)
{
	jiffies++;
	return 0;
}
#endif

void startOS(void) 
{

   KRN_reset(&sched, schedQueues, 4,0, NULL , 0);
  
   KRN_startOS("Startup task");
#ifdef __ENABLE_MEOS_TIMERTASK__
   KRN_startTimerTask("Timer Task", timstack, TIMER_STACKSIZE, 1000);
#endif
   KRN_priority(NULL, KRN_LOWEST_PRIORITY+1);
#ifdef __ENABLE_MEOS_TIMERTASK__
   KRN_initTimerISR(&TISR, timerISR);
#endif

   DQ_init(&hibQ);
}

int main(void) 
{ 
 unsigned int i;
  #ifdef MEM_TEST
 unsigned int fail =0;
 unsigned int num_runs = 0;
 unsigned char *mem_test_char_ptr = (unsigned char *)GRAM_START;
 unsigned int *mem_test_int_ptr = (unsigned int *)GRAM_START;

    mem_test_start:
  UCC_OUTMSG(" ****** MEM TEST START *******\n");
  UCC_OUTMSG(" UCC_GLOBAL_RAM_CMIF_CTRL =  %x \n",*((unsigned int *)0x0204E804));


  for(i =0;i<GRAM_SIZE;i++)
  {
    mem_test_char_ptr[i] = i;
  }
	
  for(i =0;i<GRAM_SIZE;i++)
  {
     if( mem_test_char_ptr[i] != (i & 0xff))
     {
      fail = 1;
     UCC_OUTMSG("  MEM TEST FAIL char r/w  wr data %x rd data %x address  %x \n", (i & 0xff) ,mem_test_char_ptr[i], &mem_test_char_ptr[i]);
     }
  }
	
  for(i =0;i<(GRAM_SIZE/4);i++)
  {
    mem_test_int_ptr[i] = i | (i<<8) | (i<<16) | (i<<24);
  }
	
  for(i =0;i<(GRAM_SIZE/4);i++)
  {
     if( mem_test_int_ptr[i] != (i | (i<<8) | (i<<16) | (i<<24)))
     {
        fail = 1;
       UCC_OUTMSG("  MEM TEST FAIL integer r/w  wr data %x rd data %x address  %x \n",(i | (i<<8) | (i<<16) | (i<<24)),mem_test_int_ptr[i], &mem_test_int_ptr[i]);
      }
	  
  }

  num_runs++;
	
  if(fail == 0)
	  UCC_OUTMSG(" ****** MEM TEST PASS*******\n");
  else
	  UCC_OUTMSG(" ****** MEM TEST FAIL*******\n");

  if (num_runs == 2)
	  goto done;
	  
  UCC_OUTMSG("Setting UCC_GLOBAL_RAM_CMIF_CTRL to 0x3 \n");
  *((unsigned int *)0x0204E804) = 0x3 << 8;
  fail = 0;
 
  goto mem_test_start;
	done: 
	
#endif	
    /* Start MeOS */
    startOS();

#ifdef __ENABLE_MEOS_TIMERTASK__
    KRN_activateTimerISR(&TISR);
#endif

    while(1)
    {
    	for (i = 0; i < 1024; i++)
    	{
	    if (loopcount % 2 == 0)
		    buffer[i] = 0xAAAAAAAA;
	    else
		    buffer[i] = 0xFFFFFFFF;
    	}
    	loopcount++;

    }
    /* Put this task to sleep */
    /* Should never reach here */
    KRN_hibernate(&hibQ, KRN_INFWAIT);

   return -1;
}
