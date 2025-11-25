/*++

Copyright (c) 1993 - Colorado Memory Systems, Inc.
All Rights Reserved

Module Name:

    drvintr.h

Abstract:


Revision History:          




--*/

/***********************************************************************/
/* Task manager types */
/***********************************************************************/
typedef struct IRQ_struct far *IntPtr;
typedef void (interrupt far *vecptr)();

/***********************************************************************/
/* Task manager externals */
/***********************************************************************/

void far _chain_intr(void (interrupt far *)());
vecptr far _dos_getvect(int);
#define HZ 18
#define PRIBIO 20

/***********************************************************************/
/* Task manager interrupt structure */
/***********************************************************************/

/* IRQ format (See IBM PS/2 interrupt sharing documentation) */

struct  IRQ_struct {
unsigned int    irq_entry;                      /* short jump around entry */
union {
IntPtr                  irq_struct;                     /* pointer to next */
void (interrupt far *irq_fptr)();
}                                       link;
unsigned int    irq_sig;                                /* signature */
unsigned char   irq_flags;                      /* flags */
unsigned int    irq_reset;                      /* short jump to reset code */
unsigned char   irq_reserved[7];        /* reserved */
char                            irq_jump;                       /* long jump op code */
vecptr                  irq_routine;            /* pointer to our interrupt routine */
char                            irq_rst_jump;           /* long jump op code (to reset vector) */
void (far *             irq_rst_vector)();/* pointer to reset vector */
unsigned int    CMS_signature;          /* signature for multiple driver check */
};

/***********************************************************************/
/* Task manager interrupt routines */
/***********************************************************************/

extern  void far _interrupt mt_hardware_start();
extern  void far _interrupt mt_timer_start();
extern  void far _interrupt mt_ctrl_c_start();
extern  void far mt_ctrl_c_sub();
extern  void far _interrupt mt_crit_int_start();
extern  void interrupt far mt_hardware_start();
extern  void interrupt far mt_timer_start();
extern  void interrupt far mt_ctrl_c_start();
extern  void far mt_ctrl_c_sub();
extern  void interrupt far mt_crit_int_start();

extern struct IRQ_struct mt_hardware;
extern struct IRQ_struct mt_timer;
extern struct IRQ_struct mt_ctrl_c;
extern struct IRQ_struct mt_crit_int;
