/*****************************************************************/ 
/**		     Microsoft LAN Manager			**/ 
/**	       Copyright(c) Microsoft Corp., 1987-1991		**/ 
/*****************************************************************/ 

/********************************************************************
 *								    *
 *  About this file ...  NCB.H					    *
 *								    *
 *  This file contains information about NCBs.  Use this file	    *
 *  in conjunction with NETBIOS.H and the NetBIOS APIs to write	    *
 *  programs that interact with the network via the NetBIOS	    *
 *  protocol.
 *								    *
 *								    *
 *  NOTE:  You must include NETCONS.H before this file, since this  *
 *	   file	depends on values defined in NETCONS.H.		    *
 *								    *
 ********************************************************************/

#ifndef NETNCB_INCLUDED

#define NETNCB_INCLUDED 

/**************************************************************** 
 *								*
 *	  	Data structure templates			*
 *								*
 ****************************************************************/


#define NCBNAMSZ	NETBIOS_NAME_LEN    /* absolute length of a net name	*/

/*
 * Network Control Block 
 */

struct ncb {
    unsigned char   ncb_command;	    /* command code		    */
    unsigned char   ncb_retcode;	    /* return code		    */
    unsigned char   ncb_lsn;		    /* local session number	    */
    unsigned char   ncb_num;		    /* number of our network name   */
    char far *	    ncb_buffer; 	    /* address of message buffer    */
    unsigned short  ncb_length; 	    /* size of message buffer	    */
    char	    ncb_callname[NCBNAMSZ]; /* blank-padded name of remote  */
    char	    ncb_name[NCBNAMSZ];     /* our blank-padded netname     */
    unsigned char   ncb_rto;		    /* rcv timeout/retry count	    */
    unsigned char   ncb_sto;		    /* send timeout/sys timeout     */
    unsigned long   ncb_post;		    /* Async notification handle    */
    unsigned char   ncb_lana_num;	    /* lana (adapter) number	    */
    unsigned char   ncb_cmd_cplt;	    /* 0xff => commmand pending     */
    unsigned char   ncb_reserve[14];	    /* reserved, used by BIOS	    */
};	/* ncb */

typedef struct ncb NCB;

struct _astatbuf {
    unsigned char	adapter_address[6];
    unsigned char	rev_major;
    unsigned char	reserved0;
    unsigned char	adapter_type;
    unsigned char	rev_minor;
    unsigned short	duration;
    unsigned short	frmr_recv;
    unsigned short	frmr_xmit;

    unsigned short	iframe_recv_err;

    unsigned short	xmit_aborts;
    unsigned long	xmit_success;
    unsigned long	recv_success;

    unsigned short	iframe_xmit_err;

    unsigned short	recv_buff_unavail;
    unsigned short	t1_timeouts;
    unsigned short	ti_timeouts;
    unsigned long	reserved1;
    unsigned short	free_ncbs;
    unsigned short	max_cfg_ncbs;
    unsigned short	max_ncbs;
    unsigned short	xmit_buf_unavail;
    unsigned short	max_dgram_size;
    unsigned short	pending_sess;
    unsigned short	max_cfg_sess;
    unsigned short	max_sess;
    unsigned short	max_sess_pkt_size;
    unsigned short	name_count;
};

typedef struct _astatbuf ASTATBUF;
typedef struct _astatbuf *PASTATBUF;

/*
 *	The following macro gives a pointer to the names data.
 *	It takes a "struct _astatbuf *" and returns a "char *".
 */

#define ASTAT_NAME_BUFFER(x)	((char *)(x)     + sizeof(struct _astatbuf))
#define ASTAT_NAME_BUFFER_F(x)	((char far *)(x) + sizeof(struct _astatbuf))


struct _namebuf {
    unsigned char	name[NCBNAMSZ];
    unsigned char	name_num;
    unsigned char	name_flags;
};

typedef struct _namebuf NAMEBUF;
typedef struct _namebuf *PNAMEBUF;


struct _sessbufhdr {
    unsigned char	sess_name;
    unsigned char	num_sess;
    unsigned char	rcv_dg_outstanding;
    unsigned char	rcv_any_outstanding;
};

typedef struct _sessbufhdr SESSBUFHDR;
typedef struct _sessbufhdr *PSESSBUFHDR;

struct _sessbuf {
    unsigned char	lsn;
    unsigned char	state;
    unsigned char	local_name[NCBNAMSZ];
    unsigned char	remote_name[NCBNAMSZ];
    unsigned char	rcvs_outstanding;
    unsigned char	sends_outstanding;
};

typedef struct _sessbuf SESSBUF;
typedef struct _sessbuf *PSESSBUF;


/**************************************************************** 
 *								*
 *	  	Special values and constants			*
 *								*
 ****************************************************************/

/*
 *	NCB Command codes
 */

#define NCBCALL 	0x10		/* NCB CALL			    */
#define NCBLISTEN	0x11		/* NCB LISTEN			    */
#define NCBHANGUP	0x12		/* NCB HANG UP			    */
#define NCBSEND 	0x14		/* NCB SEND			    */
#define NCBRECV 	0x15		/* NCB RECEIVE			    */
#define NCBRECVANY	0x16		/* NCB RECEIVE ANY		    */
#define NCBCHAINSEND	0x17		/* NCB CHAIN SEND		    */
#define NCBDGSEND	0x20		/* NCB SEND DATAGRAM		    */
#define NCBDGRECV	0x21		/* NCB RECEIVE DATAGRAM 	    */
#define NCBDGSENDBC	0x22		/* NCB SEND BROADCAST DATAGRAM	    */
#define NCBDGRECVBC	0x23		/* NCB RECEIVE BROADCAST DATAGRAM   */
#define NCBADDNAME	0x30		/* NCB ADD NAME 		    */
#define NCBDELNAME	0x31		/* NCB DELETE NAME		    */
#define NCBRESET	0x32		/* NCB RESET			    */
#define NCBASTAT	0x33		/* NCB ADAPTER STATUS		    */
#define NCBSSTAT	0x34		/* NCB SESSION STATUS		    */
#define NCBCANCEL	0x35		/* NCB CANCEL			    */
#define NCBADDGRNAME	0x36		/* NCB ADD GROUP NAME		    */
#define NCBUNLINK	0x70		/* NCB UNLINK			    */
#define NCBSENDNA	0x71		/* NCB SEND NO ACK		    */
#define NCBCHAINSENDNA	0x72		/* NCB CHAIN SEND NO ACK	    */
#define NCBLANSTALERT	0x73		/* NCB LAN STATUS ALERT 	    */

#define NCBCALLNIU	0x74		/* UB special			    */
#define NCBRCVPKT	0x78		/* UB special			    */
#define NCBFINDNAME	0x78		/* as defined by IBM NetBIOS	    */
#define NCBTRACE	0x79		/* NCB TRACE (DOS only)		    */

#define ASYNCH		0x80		/* high bit set == asynchronous     */

/*
 *	NCB Return codes
 */

#define NRC_GOODRET	0x00	/* good return				     */
#define NRC_BUFLEN	0x01	/* illegal buffer length		     */
#define NRC_BFULL	0x02	/* buffers full, no receive issued	     */
#define NRC_ILLCMD	0x03	/* illegal command			     */
#define NRC_CMDTMO	0x05	/* command timed out			     */
#define NRC_INCOMP	0x06	/* message incomplete, issue another command */
#define NRC_BADDR	0x07	/* illegal buffer address		     */
#define NRC_SNUMOUT	0x08	/* session number out of range		     */
#define NRC_NORES	0x09	/* no resource available		     */
#define NRC_SCLOSED	0x0a	/* session closed			     */
#define NRC_CMDCAN	0x0b	/* command cancelled			     */
#define NRC_DMAFAIL	0x0c	/* PC DMA failed			     */
#define NRC_DUPNAME	0x0d	/* duplicate name			     */
#define NRC_NAMTFUL	0x0e	/* name table full			     */
#define NRC_ACTSES	0x0f	/* no deletions, name has active sessions    */
#define NRC_INVALID	0x10	/* name not found or no valid name	     */
#define NRC_LOCTFUL	0x11	/* local session table full		     */
#define NRC_REMTFUL	0x12	/* remote session table full		     */
#define NRC_ILLNN	0x13	/* illegal name number			     */
#define NRC_NOCALL	0x14	/* no callname				     */
#define NRC_NOWILD	0x15	/* cannot put * in NCB_NAME		     */
#define NRC_INUSE	0x16	/* name in use on remote adapter	     */
#define NRC_NAMERR	0x17	/* called name cannot == name or name #      */
#define NRC_SABORT	0x18	/* session ended abnormally		     */
#define NRC_NAMCONF	0x19	/* name conflict detected		     */
#define NRC_IFBUSY	0x21	/* interface busy, IRET before retrying      */
#define NRC_TOOMANY	0x22	/* too many commands outstanding, retry later*/
#define NRC_BRIDGE	0x23	/* ncb_bridge field not 00 or 01	     */
#define NRC_CANOCCR	0x24	/* command completed while cancel occurring  */
#define NRC_RESNAME	0x25	/* reserved name specified		     */
#define NRC_CANCEL	0x26	/* command not valid to cancel		     */
#define NRC_DUPENV	0x30	/* Name defined by anther local process      */
#define NRC_MULT	0x33	/* multiple requests for same session	     */
#define NRC_ENVNOTDEF	0x34	/* Environment undefined. RESET must be issued */
#define NRC_MAXAPPS	0x36	/* max number of applications exceeded	     */
#define NRC_NORESOURCES 0x38	/* requested resources are not available     */
#define NRC_INVADDRESS	0x39	/* invalid ncb address or length > segment   */
#define NRC_RESETATINT	0x3A	/* RESET may not be submitted at int time    */
#define NRC_INVDDID	0x3B	/* invalid NCB DDID			     */
#define NRC_LOCKFAIL	0x3C	/* lock of user area failed		     */
#define NRC_SYSTEM	0x40	/* system error 			     */
#define NRC_ROM 	0x41	/* ROM checksum failure 		     */
#define NRC_RAM 	0x42	/* RAM test failure			     */
#define NRC_DLF 	0x43	/* digital loopback failure		     */
#define NRC_ALF 	0x44	/* analog loopback failure		     */
#define NRC_IFAIL	0x45	/* interface failure			     */
#define NRC_ADPTMALFN	0x50	/* network adapter malfunction		     */
#define NRC_INTERR	0xf9	/* driver internal error	             */
#define NRC_ADAPCHECK	0xfa	/* adapter check			     */
#define NRC_NODRIVER	0xfb	/* netbios 3.0 driver not loaded             */
#define NRC_ADAPCLOSE	0xfd	/* unexpected adapter closure		     */

#define NRC_PENDING	0xff	/* asynchronous command is not yet finished  */

    /* main user entry point for NetBIOS 3.0*/
API_FUNCTION
NetBios(struct ncb far *);

/*
 *	Maximum datagram size
 */

#define MAX_DG_SIZE 512


/*
 *      Masks for names returned in buffer by NCBASTAT
 */

#define NAME_FLAGS_MASK	0x87

#define	GROUP_NAME	0x80
#define UNIQUE_NAME	0x00

#define REGISTERING	0x00
#define REGISTERED	0x04
#define DEREGISTERED	0x05
#define DUPLICATE	0x06
#define DUPLICATE_DEREG	0x07

/*
 *      Sessions states for sessions returned by NCBSSTAT
 */

#define	LISTEN_OUTSTANDING	0x01
#define	CALL_PENDING		0x02
#define SESSION_ESTABLISHED	0x03
#define HANGUP_PENDING		0x04
#define	HANGUP_COMPLETE		0x05
#define	SESSION_ABORTED		0x06

#endif /* NETNCB_INCLUDED */
