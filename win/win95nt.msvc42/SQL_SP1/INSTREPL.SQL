
/*
** instrepl.sql			1996/02/13 22:03
**
**
** Copyright Microsoft, Inc. 1994, 1995, 1996
** All Rights Reserved.
** Use, duplication, or disclosure by the United States Government
** is subject to restrictions as set forth in subdivision (c) (1) (ii)
** of the Rights in Technical Data and Computer Software clause
** at CFR 252.227-7013. Microsoft, Inc. One Microsoft Way, Redmond WA
** 98052.
*/

go
use master
go
dump tran master with no_log
go
set nocount on
go

exec sp_configure 'update',1
go
reconfigure with override
go

exec sp_MS_upd_sysobj_category 1 --Capture time for use below.

go

if exists (select * from sysobjects
		where sysstat & 0xf = 4
			and name = 'sp_addarticle')
	drop procedure sp_addarticle
go

if exists (select * from sysobjects
	where sysstat & 0xf = 4
			and name = 'sp_articlecolumn')
	drop procedure sp_articlecolumn
go

if exists (select * from sysobjects
		where sysstat & 0xf = 4
			and name = 'sp_articlefilter')
	drop procedure sp_articlefilter
go

if exists (select * from sysobjects
	where sysstat & 0xf = 4
			and name = 'sp_articletextcol')
	drop procedure sp_articletextcol
go

if exists (select * from sysobjects
	where sysstat & 0xf = 4
			and name = 'sp_textcolstatus')
	drop procedure sp_textcolstatus
go

if exists (select * from sysobjects
		where sysstat & 0xf = 4
			and name = 'sp_articleview')
	drop procedure sp_articleview
go

if exists (select * from sysobjects
		where sysstat & 0xf = 4
			and name = 'sp_addpublication')
	drop procedure sp_addpublication
go

if exists (select * from sysobjects
		where sysstat & 0xf = 4
			and name = 'sp_addpublisher')
	drop procedure sp_addpublisher
go

if exists (select * from sysobjects
		where sysstat & 0xf = 4
			and name = 'sp_addsubscriber')
	drop procedure sp_addsubscriber
go

if exists (select * from sysobjects
		where sysstat & 0xf = 4
			and name = 'sp_addsubscription')
	drop procedure sp_addsubscription
go


IF EXISTS (SELECT * FROM sysobjects
		WHERE sysstat & 0xf = 4
			AND name = 'sp_changearticle')
	DROP PROCEDURE sp_changearticle
go


IF EXISTS (SELECT * FROM sysobjects
		WHERE sysstat & 0xf = 4
			AND name = 'sp_changepublication')
	DROP PROCEDURE sp_changepublication
go

if exists (select * from sysobjects
		where sysstat & 0xf = 4
			and name = 'sp_changesubscriber')
	drop procedure sp_changesubscriber
go

IF EXISTS (SELECT * FROM sysobjects
		WHERE sysstat & 0xf = 4
			AND name = 'sp_changesubscription')
	DROP PROCEDURE sp_changesubscription
go

IF EXISTS (SELECT * FROM sysobjects
		WHERE sysstat & 0xf = 4
			AND name = 'sp_create_distribution_tables')
	DROP PROCEDURE sp_create_distribution_tables
go

dump tran master with no_log
go

if exists (select * from sysobjects
		where sysstat & 0xf = 4
			and name = 'sp_hcchangesubstatus1')
	drop procedure sp_hcchangesubstatus1
go

if exists (select * from sysobjects
		where sysstat & 0xf = 4
			and name = 'sp_hcchangesubstatus2')
	drop procedure sp_hcchangesubstatus2
go

if exists (select * from sysobjects
		where sysstat & 0xf = 4
			and name = 'sp_changesubstatus')
	drop procedure sp_changesubstatus
go

if exists (select * from sysobjects
		where sysstat & 0xf = 4
			and name = 'sp_distcounters')
	drop procedure sp_distcounters
go

if exists (select * from sysobjects
		where sysstat & 0xf = 4
			and name = 'sp_droparticle')
	drop procedure sp_droparticle
go

if exists (select * from sysobjects
		where sysstat & 0xf = 4
			and name = 'sp_droppublication')
	drop procedure sp_droppublication
go

if exists (select * from sysobjects
		where sysstat & 0xf = 4
			and name = 'sp_droppublisher')
	drop procedure sp_droppublisher
go

if exists (select * from sysobjects
		where sysstat & 0xf = 4
			and name = 'sp_dropsubscriber')
	drop procedure sp_dropsubscriber
go

if exists (select * from sysobjects
		where sysstat & 0xf = 4
			and name = 'sp_dropsubscription')
	drop procedure sp_dropsubscription
go

if exists (select * from sysobjects
		where sysstat & 0xf = 4
			and name = 'sp_dsninfo')
	drop procedure sp_dsninfo
go

if exists (select * from sysobjects
		where sysstat & 0xf = 4
			and name = 'sp_enumdsn')
	drop procedure sp_enumdsn
go

if exists (select * from sysobjects
		where sysstat & 0xf = 4
			and name = 'sp_enumfullsubscribers')
	drop procedure sp_enumfullsubscribers
go

if exists (select * from sysobjects
		where sysstat & 0xf = 4
			and name = 'sp_helparticle')
	drop procedure sp_helparticle
go

if exists (select * from sysobjects
		where sysstat & 0xf = 4
			and name = 'sp_helparticlecolumns')
	drop procedure sp_helparticlecolumns
go

if exists (select * from sysobjects
		where sysstat & 0xf = 4
			and name = 'sp_helpdistributor')
	drop procedure sp_helpdistributor
go

if exists (select * from sysobjects
		where sysstat & 0xf = 4
			and name = 'sp_helppublication')
	drop procedure sp_helppublication
go

if exists (select * from sysobjects
		where sysstat & 0xf = 4
			and name = 'sp_helppublicationsync')
	drop procedure sp_helppublicationsync
go

if exists (select * from sysobjects
		where sysstat & 0xf = 4
			and name = 'sp_helpreplicationdb')
	drop procedure sp_helpreplicationdb
go

if exists (select * from sysobjects
		where sysstat & 0xf = 4
			and name = 'sp_helpsubscriberinfo')
	drop procedure sp_helpsubscriberinfo
go

if exists (select * from sysobjects
		where sysstat & 0xf = 4
			and name = 'sp_helpsubscription')
	drop procedure sp_helpsubscription
go

if exists (select * from sysobjects
		where sysstat & 0xf = 4
			and name = 'sp_publishdb')
	drop procedure sp_publishdb
go

if exists (select * from sysobjects
		where sysstat & 0xf = 4
			and name = 'sp_replica')
	drop procedure sp_replica
go

if exists (select * from sysobjects
		where sysstat & 0xf = 4
			and name = 'sp_replsync')
	drop procedure sp_replsync
go

if exists (select * from sysobjects
		where sysstat & 0xf = 4
			and name = 'sp_subscribe')
	drop procedure sp_subscribe
go

if exists (select * from sysobjects
		where sysstat & 0xf = 4
			and name = 'sp_MSuninstall_publishing')
	drop procedure sp_MSuninstall_publishing
go

if exists (select * from sysobjects
		where sysstat & 0xf = 4
			and name = 'sp_unsubscribe')
	drop procedure sp_unsubscribe
go

if exists (select * from sysobjects
		where sysstat & 0xf = 4
			and name = 'xp_dsninfo')
	exec sp_dropextendedproc 'xp_dsninfo'
go

if exists (select * from sysobjects
		where sysstat & 0xf = 4
			and name = 'xp_enumdsn')
	exec sp_dropextendedproc 'xp_enumdsn'
go

CREATE PROCEDURE sp_helpdistributor (
	@distributor varchar(30)  = '%' OUTPUT, /* The distribution server name */
	@distribdb   varchar(30)  = '%' OUTPUT, /* The distribution database script */
        @directory   varchar(255) = '%' OUTPUT, /* The working directory */
        @account     varchar(255) = '%' OUTPUT, /* The Windows NT user account */
	@local varchar(5) = NULL		/* Get local server values */
        ) AS

    /*
    ** Declarations.
    */
    DECLARE @loc_distributor varchar(30)
    DECLARE @loc_distribdb varchar(30)
    DECLARE @loc_directory varchar(255)
    DECLARE @loc_account varchar(255)
    DECLARE @proc varchar(255)

    SET NOCOUNT ON

    /*
    ** If @local flag, get current server's distribution values.
    */
    IF LOWER (@local) = 'local'
	SELECT @loc_distributor = @@SERVERNAME
    /*
    ** Get the distribution server
    */
    ELSE
        BEGIN
	    SELECT @loc_distributor = srvname
	      FROM master..sysservers
	     WHERE srvstatus & 8 <> 0
	
	     IF @@error <> 0
		BEGIN
		    RAISERROR (14071, 16, -1)
		    RETURN (1)
		END
	END

    /*
    ** If remote distribuiton, execute sp_helpdistributor on distribution
    ** server.
    */
    IF @loc_distributor <> @@SERVERNAME
       BEGIN
	    SELECT @proc = RTRIM(@loc_distributor) + '.master..sp_helpdistributor '
            EXECUTE @proc
		@loc_distributor OUTPUT,
		@loc_distribdb OUTPUT,
		@loc_directory OUTPUT,
		@loc_account OUTPUT,
		@local = 'local'
	    IF @@ERROR <> 0
	       RETURN (1)

            GOTO DONE
       END

    /*
    ** Fetch the distribution database name.
    */
    IF (@distributor = '%' AND @distribdb = '%' AND @directory = '%'
	AND @account = '%') OR @distribdb IS NULL
       BEGIN
	    SELECT @proc = 'master..xp_regread '
	    EXECUTE @proc 'HKEY_LOCAL_MACHINE',
			  'SOFTWARE\Microsoft\MSSQLServer\Replication',
			  'DistributionDB',
		    @param = @loc_distribdb OUTPUT
	
	    IF @@ERROR <> 0 RETURN (1)
	END	
    /*
    ** Fetch the distribution working directory.
    */
    IF (@distributor = '%' AND @distribdb = '%' AND @directory = '%'
	AND @account = '%') OR @directory IS NULL
       BEGIN
	    SELECT @proc = 'master..xp_regread '
	    EXECUTE @proc 'HKEY_LOCAL_MACHINE',
			  'SOFTWARE\Microsoft\MSSQLServer\Replication',
			  'WorkingDirectory',
		    @param = @loc_directory OUTPUT
	
	    IF @@ERROR <> 0 RETURN (1)
       END

    /*
    ** Fetch the distribution account name.
    */
    IF (@distributor = '%' AND @distribdb = '%' AND @directory = '%'
	AND @account = '%') OR @account IS NULL
       BEGIN
	    SELECT @proc = 'master..xp_regread '
	    EXECUTE @proc 'HKEY_LOCAL_MACHINE',
			  'SYSTEM\CurrentControlSet\Services\SQLExecutive',
			  'ObjectName',
		    @param = @loc_account OUTPUT
	
	    IF @@ERROR <> 0 RETURN (1)
       END

DONE:

    /*
    ** Return result set if no output parameters
    */

    IF @distributor = '%' AND @distribdb = '%' AND @directory = '%'
	AND @account = '%'

	SELECT 'distributor'           = @loc_distributor,
               'distribution database' = @loc_distribdb,
               'directory'             = @loc_directory,
               'account'               = @loc_account

    /*
    ** Return output parameters if requested.
    */

    IF @distributor IS NULL
    	SELECT @distributor = @loc_distributor
    IF @distribdb IS NULL
    	SELECT @distribdb = @loc_distribdb
    IF @directory IS NULL
    	SELECT @directory = @loc_directory
    IF @account IS NULL
	SELECT @account = @loc_account

    RETURN (0)
go

/*
** Create replication stored procedures.
** Part 2:  create all other stored procedures.
*/

print ''
print 'Creating procedure sp_addpublication.'
go
CREATE PROCEDURE sp_addpublication (
    @publication varchar(30), 	           /* publication name */
    @taskid int,    			   /* associated scheduler task */
    @restricted varchar (10) = 'false',    /* publication security */
    @sync_method varchar(13) = 'native',   /* (bcp) native, (bcp) character */
    @repl_freq varchar(10) = 'continuous', /* continuous, snapshot */
    @description varchar (255) = NULL,     /* publication description */
    @status varchar(8) = 'inactive'        /* publication status; 0=inactive, 1=active */
    ) AS

    SET NOCOUNT ON

    /*
    ** Declarations.
    */

    DECLARE @retcode	int         /* return code value for procedure execution */
    DECLARE @rid bit                /* value for restricted column */
    DECLARE @rfid tinyint           /* identifier for replication frequency */
    DECLARE @publish_bit smallint   /* publication bit (flag) in sysobjects */
    DECLARE @smid tinyint           /* identifier for sync method */
    DECLARE @statid tinyint         /* status id based on @status */
    DECLARE @distributor varchar(30)
    DECLARE @distproc varchar (255)


    SELECT @publish_bit = 32

    /*
    ** Security Check
    ** Only the System Administratr (SA) or the Database Owner (dbo) can
    ** publish a table.
    */

    IF suser_id() <> 1 AND user_id() <> 1
        BEGIN
            RAISERROR (15000, 14, -1)
    	    RETURN (1)
        END

    /*
    ** Check to see if the database has been activated for publication.
    */

    IF (SELECT category & 1
          FROM master..sysdatabases
         WHERE name = DB_NAME()) = 0

	BEGIN
            RAISERROR (14013, 16, -1)
	    RETURN (1)
	END

    /*
    ** Parameter Check: @publication.
    ** The @publication name must conform to the rules for identifiers,
    ** and must not be the keyword 'all'.
    */

    IF @publication IS NULL
        BEGIN
            RAISERROR (14043, 16, -1, 'The publication')
            RETURN (1)
        END

    EXECUTE @retcode = sp_validname @publication

    IF @@ERROR <> 0 OR @retcode <> 0
	RETURN (1)

    IF LOWER (@publication) = 'all'
        BEGIN
            RAISERROR (14034, 16, -1)
            RETURN (1)
        END

    /*
    ** Get distribution server information for remote RPC
    ** task verification.
    */
    EXEC @retcode = sp_helpdistributor @distributor = @distributor OUTPUT
    IF @@error <> 0 OR @retcode <> 0
        BEGIN
	    RAISERROR (14071, 16, -1)
    	    RETURN (1)
	END

    /*
    ** Parameter Check:  @taskid
    ** The @taskid must exists in the systasks table.  The @taskid
    ** must also be unique.
    */
    SELECT @distproc = RTRIM(@distributor) + '.msdb..sp_verifytaskid'
    EXECUTE @retcode = @distproc @taskid = @taskid, @subsystem = 'Sync'
    IF @@ERROR <> 0 or @retcode <> 0
        BEGIN
            RAISERROR (14002, 16, -1, @taskid)
            RETURN (1)
        END

    IF EXISTS (SELECT * FROM syspublications WHERE taskid = @taskid)
        BEGIN
            RAISERROR (14045, 16, -1)
            RETURN (1)
        END

    /*
    ** Parameter Check: @sync_method
    ** The synchronization method must be one of the following:
    **
    **      0  [bcp] native
    **      1  [bcp] character
    */

    SELECT @sync_method = LOWER(@sync_method)
    IF @sync_method IS NULL OR @sync_method NOT IN ('native', 'character', 'bcp native', 'bcp character')
        BEGIN
            RAISERROR (14014, 16, -1)
            RETURN (1)
        END

    IF @sync_method IN ('character', 'bcp character')
        SELECT @smid = 1
    ELSE
        SELECT @smid = 0

    /*
    ** Parameter Check: @repl_freq.
    ** Make sure that the replication frequency is one of the following:
    **
    **  id  frequency
    **  ==  ==========
    **   0  continuous
    **   1  snapshot
    */

    SELECT @repl_freq = LOWER(@repl_freq)
    IF @repl_freq IS NULL OR @repl_freq NOT IN ('continuous', 'snapshot')
        BEGIN
            RAISERROR (14015, 16, -1)
            RETURN (1)
        END

    IF @repl_freq = 'snapshot' SELECT @rfid = 1
    ELSE SELECT @rfid = 0

    /*
    **  Check if the publication already exists.
    */

    IF EXISTS (SELECT * FROM syspublications WHERE name = @publication)
        BEGIN
            RAISERROR (14016, 16, -1, @publication)
    	    RETURN (1)
        END

    /*
    ** Parameter Check:  @restricted.
    */

    IF (@restricted IS NULL) OR (LOWER(@restricted) NOT IN ('true', 'false'))
        BEGIN
            RAISERROR (14017, 16, -1)
    	    RETURN (1)
        END

    IF LOWER(@restricted) = 'true'
    	SELECT @rid = 1
    ELSE
    	SELECT @rid = 0

    /*
    ** Parameter Check:  @status.
    ** The @status value can be:
    **
    **      statid  status
    **      ======  ========
    **           0  inactive
    **           1  active
    */

    IF @status IS NULL OR LOWER(@status) NOT IN ('inactive', 'active')
        BEGIN
            RAISERROR (14012, 16, -1)
            RETURN (1)
        END

    IF LOWER(@status) = 'active' SELECT @statid = 1
    ELSE SELECT @statid = 0

    /*
    **  Add publication to syspublications.
    */

    INSERT syspublications(description, name, repl_freq,
                           restricted, status, sync_method, taskid)
    VALUES (@description, @publication, @rfid, @rid, @statid, @smid, @taskid)

    IF @@ERROR <> 0
        BEGIN
            RAISERROR (14018, 16, -1)
    	    RETURN (1)
        END
go

print ''
print 'Creating procedure sp_changepublication.'
go
CREATE PROCEDURE sp_changepublication (
    @publication varchar(30) = NULL,        /* Publication name */
    @property varchar(15) = NULL,           /* The property to change */
    @value varchar(255) = NULL              /* The new property value */
    ) AS

    SET NOCOUNT ON

    /*
    ** Declarations.
    */

    DECLARE @cmd varchar(255)
    DECLARE @pubid int
    DECLARE @replfreqid tinyint
    DECLARE @restrictedid bit
    DECLARE @retcode int
    DECLARE @statusid tinyint
    DECLARE @syncmethodid tinyint
    DECLARE @taskid int
    DECLARE @distributor varchar(30)
    DECLARE @distproc varchar (255)
    DECLARE @subscribed int
	

    select @subscribed = 1

    /*
    ** Security Check
    ** Only the System Administrator (SA) or the Database Owner (dbo) can
    ** perform this procedure.
    */

    IF suser_id() <> 1 AND user_id() <> 1
        BEGIN
            RAISERROR (15000, 14, -1)
    	    RETURN (1)
        END

    /*
    ** Check to see if the database has been activated for publication.
    */

    IF (SELECT category & 1
          FROM master..sysdatabases
         WHERE name = DB_NAME()) = 0

	BEGIN
            RAISERROR (14013, 16, -1)
	    RETURN (1)
	END

    /*
    ** Parameter Check:  @property.
    ** If the @property parameter is NULL, print the options.
    */

    IF @property IS NULL
        BEGIN
            CREATE TABLE #tab1 (properties varchar(30))
            INSERT INTO #tab1 VALUES ('name')
            INSERT INTO #tab1 VALUES ('description')
            INSERT INTO #tab1 VALUES ('taskid')
            INSERT INTO #tab1 VALUES ('sync_method')
            INSERT INTO #tab1 VALUES ('status')
            INSERT INTO #tab1 VALUES ('repl_freq')
            INSERT INTO #tab1 VALUES ('restricted')
            PRINT ''
            SELECT * FROM #tab1
            RETURN (0)
        END

    /*
    ** Parameter Check:  @publication.
    ** Make sure that the publication exists.
    */

    IF @publication IS NULL
        BEGIN
            RAISERROR (14043, 16, -1, 'The publication')
            RETURN (1)
        END

    EXECUTE @retcode = sp_validname @publication

    IF @@ERROR <> 0 OR @retcode <> 0
	RETURN (1)

    SELECT @pubid = pubid FROM syspublications WHERE name = @publication

    IF @pubid IS NULL
        BEGIN
            RAISERROR (15001, 11, -1, @publication)
    	    RETURN (1)
        END
    ELSE

    /*
    ** Parameter Check:  @property.
    ** Check to make sure that @property is a valid property in
    ** syspublications.
    */

    IF LOWER(@property) NOT IN ('name', 'description', 'taskid', 'sync_method', 'status', 'repl_freq', 'restricted')
        BEGIN
            RAISERROR (14078, 16, -1)
            RETURN (1)
        END

    /*
    ** Change the property.
    */

    IF LOWER(@property) IN ('name', 'description')
        BEGIN

            IF LOWER(@property) = 'name'
                BEGIN

                    IF @value IS NULL
                        BEGIN
                            RAISERROR (14043, 16, -1, 'The publication')
                            RETURN (1)
                        END

                    EXECUTE @retcode = sp_validname @value

                    IF @@ERROR <> 0 OR @retcode <> 0
			RETURN (1)

                    IF EXISTS (SELECT * FROM syspublications WHERE name = @value)
                        BEGIN
                            RAISERROR (14016, 16, -1, @value)
                            RETURN (1)
                        END

                END

            SELECT @cmd = ''
            SELECT @cmd = @cmd + 'UPDATE syspublications '
            SELECT @cmd = @cmd + '   SET ' + @property + ' = ''' + @value + ''''
            SELECT @cmd = @cmd + ' WHERE pubid = ' + STR(@pubid)
            EXECUTE (@cmd)
            IF @@ERROR <> 0 RETURN (1)
        END

    IF LOWER(@property) = 'taskid'
       BEGIN
	    SELECT @taskid = CONVERT(int, @value)
            /*
            ** Get distribution server information for remote RPC
            ** task verification.
            */
            EXEC @retcode = sp_helpdistributor @distributor = @distributor OUTPUT
            IF @@error <> 0 OR @retcode <> 0
               BEGIN
	          RAISERROR (14071, 16, -1)
    	          RETURN (1)
	       END

	    /*
	    ** The @taskid must exists in the systasks table.  The @taskid
	    ** must also be unique.
	    */
	    SELECT @distproc = RTRIM(@distributor) + '.msdb..sp_verifytaskid'
	    EXECUTE @retcode = @distproc @taskid = @taskid,
	       @subsystem = 'Sync'
            IF @@ERROR <> 0 or @retcode <> 0
		BEGIN
		    RAISERROR (14002, 16, -1, @taskid)
		    RETURN (1)
	        END

	    IF EXISTS (SELECT * FROM syspublications WHERE taskid = @taskid)
		BEGIN
		    RAISERROR (14045, 16, -1)
		    RETURN (1)
		END

	    UPDATE syspublications SET taskid = @taskid
	        WHERE pubid = @pubid

            IF @@ERROR <> 0 RETURN (1)
       END

    IF LOWER(@property) = 'sync_method'
        BEGIN

            /*
            ** Check for a valid synchronization method.
            */

            IF LOWER(@value) NOT IN ('native', 'character', 'bcp native', 'bcp character')
                BEGIN
                    RAISERROR (14026, 16, -1)
                    RETURN (1)
                END

            /*
            ** Determine the integer value for the sync_method.
            */

            IF LOWER(@value) IN ('native', 'bcp native')
    	        SELECT @syncmethodid = 0
            ELSE IF LOWER(@value) IN ('character', 'bcp character')
    	        SELECT @syncmethodid = 1

            /*
            ** Update the publication with the new synchronization method.
            */

            UPDATE syspublications
               SET sync_method = @syncmethodid
             WHERE pubid = @pubid

            IF @@ERROR <> 0 RETURN (1)

        END

    IF LOWER(@property) = 'status'
        BEGIN

            /*
            ** Check to make sure that we have a valid status.
            */

            IF LOWER(@value) NOT IN ('active', 'inactive')
                BEGIN
                    RAISERROR (14024, 16, -1)
                    RETURN (1)
                END

            /*
            ** Determine the integer value for the status.
            */

            IF LOWER(@value) = 'active'
    	        SELECT @statusid = 1
            ELSE
    	        SELECT @statusid = 0

            /*
            ** Update the publication with the new status.
            */

            UPDATE syspublications
               SET status = @statusid
             WHERE pubid = @pubid

            IF @@ERROR <> 0 RETURN (1)

        END

    IF LOWER(@property) = 'repl_freq'
        BEGIN

	    /*
	    ** Only unsubscribed publications may have this modified.
	    */
	    IF EXISTS (SELECT * FROM syssubscriptions
		WHERE status <> @subscribed
		AND artid IN (SELECT artid FROM sysarticles where pubid
		   = @pubid))
		BEGIN
		    RAISERROR (14033, 11, -1)
		    RETURN (1)
		END
	
            /*
            ** Check for a valid replication frequency value.
            */

            IF LOWER(@value) NOT IN ('continuous', 'snapshot')
                BEGIN
                    RAISERROR (14015, 16, -1)
                    RETURN (1)
                END

            /*
            ** Determine the integer value for the replication frequency.
            */

            IF LOWER(@value) = 'continuous'
    	        SELECT @replfreqid = 0
            ELSE
    	        SELECT @replfreqid = 1

            /*
            ** Update the publication with the new replication frequency.
            */

            UPDATE syspublications
               SET repl_freq = @replfreqid
             WHERE pubid = @pubid

            IF @@ERROR <> 0 RETURN (1)

        END

    IF LOWER(@property) = 'restricted'
        BEGIN

            /*
            ** Check for a valid restricted value.
            */

            IF LOWER(@value) NOT IN ('true', 'false')
                BEGIN
                    RAISERROR (14017, 16, -1)
                    RETURN (1)
                END

            /*
            ** Determine the integer value for the restricted column.
            */

            IF LOWER(@value) = 'true'
    	        SELECT @restrictedid = 1
            ELSE
    	        SELECT @restrictedid = 0

            /*
            ** Update the publication with the new restriction value.
            */

            UPDATE syspublications
               SET restricted = @restrictedid
             WHERE pubid = @pubid

            IF @@ERROR <> 0 RETURN (1)

        END

    /*
    ** Return succeed.
    */

    RAISERROR (14077, 10, -1)
    RETURN (0)
GO

print ''
print 'Creating procedure sp_changesubscription.'
GO
CREATE PROCEDURE sp_changesubscription (
    @publication varchar(30) = NULL,        /* Publication name */
    @article varchar(30) = NULL,            /* Article name */
    @subscriber varchar(30),  		    /* Subscriber name */
    @property varchar(15) = NULL,           /* The property to change */
    @value varchar(255) = NULL              /* The new property value */
    ) AS

    SET NOCOUNT ON

    /*
    ** Declarations.
    */

    DECLARE @artid int
    DECLARE @inactive tinyint
    DECLARE @pubid int
    DECLARE @retcode int
    DECLARE @srvid int
    DECLARE @subscribed tinyint
    DECLARE @subscriber_bit smallint
    DECLARE @synctypeid int
    DECLARE @none tinyint
    DECLARE @automatic tinyint
    DECLARE @manual tinyint

    /*
    ** Initializations.
    */

    SELECT @inactive = 0        /* Const: subscription status 'inactive' */
    SELECT @subscribed = 1      /* Const: subscription status 'subscribed' */
    SELECT @subscriber_bit = 4  /* Const: subscription server status */
    SELECT @none = 2            /* Const: synchronization type 'none' */
    SELECT @automatic = 1       /* Const: synchronization type 'automatic' */
    SELECT @manual = 0          /* Const: synchronization type 'manual' */

    /*
    ** Security Check.
    */
    IF suser_id() <> 1 AND user_id() <> 1
        BEGIN
            RAISERROR (15000, 14, -1)
            RETURN (1)
        END

    /*
    ** Parameter Check:  @property.
    ** If the @property parameter is NULL, print the options.
    */

    IF @property IS NULL
        BEGIN
            CREATE TABLE #tab1 (properties varchar(30))
            INSERT INTO #tab1 VALUES ('sync_type')
            INSERT INTO #tab1 VALUES ('dest_db')
            SELECT * FROM #tab1
            RETURN (0)
        END

    /*
    ** Parameter Check:  @publication.
    ** Make sure that the publication exists.
    */

    IF @publication IS NULL
        BEGIN
            RAISERROR (14043, 16, -1, 'The publication')
            RETURN (1)
        END

    EXECUTE @retcode = sp_validname @publication

    IF @@ERROR <> 0 OR @retcode <> 0
	RETURN (1)

    SELECT @pubid = pubid FROM syspublications WHERE name = @publication

    IF @pubid IS NULL
        BEGIN
            RAISERROR (15001, 11, -1, @publication)
    	    RETURN (1)
        END
    ELSE

    /*
    ** Check to see that the article exists in sysarticles.
    ** Fetch the article identification number.
    */

    IF @article IS NULL
        BEGIN
            RAISERROR (14043, 16, -1, 'The article')
            RETURN (1)
        END

    EXECUTE @retcode = sp_validname @article

    IF @retcode <> 0
	RETURN (1)

    SELECT @artid = artid
      FROM sysarticles
     WHERE name = @article
       AND pubid = @pubid

    IF @artid IS NULL
        BEGIN
            RAISERROR (15001, 11, -1, @article)
            RETURN (1)
        END

    /*
    ** Parameter Check:  @subscriber.
    ** Check to make sure we have a valid subscriber.
    */

    IF @subscriber IS NULL
        BEGIN
            RAISERROR (14043, 16, -1, 'The subscriber')
            RETURN (1)
        END

    EXECUTE @retcode = sp_validname @subscriber

    IF @retcode <> 0
	RETURN (1)

    SELECT @srvid = srvid
      FROM master..sysservers
     WHERE srvname = @subscriber
       AND (srvstatus & @subscriber_bit) <> 0

    IF @srvid IS NULL
        BEGIN
            RAISERROR (14010, 16, -1)
   	    RETURN (1)
        END

    /*
    ** Check to see if you have a subscription on this publication/article.
    */

    IF NOT EXISTS (SELECT *
                     FROM syssubscriptions
                    WHERE artid = @artid
                      AND srvid = @srvid)
        BEGIN
            RAISERROR (14050, 11, -1)
            RETURN(1)
        END

    /*
    ** Parameter Check:  @property.
    ** Check to make sure that @property is a valid property in
    ** sysarticles.
    */

    IF LOWER(@property) NOT IN ('sync_type', 'dest_db')
        BEGIN
            RAISERROR (14051, 16, -1)
            RETURN (1)
        END

    /*
    ** Change the property.
    */

    IF LOWER(@property) = 'sync_type'
        BEGIN

            /*
            ** Check to make sure that we have a valid sync_type.
            */

            IF LOWER(@value) NOT IN ('manual', 'automatic', 'none')
                BEGIN
                    RAISERROR (14052, 16, -1)
                    RETURN (1)
                END

            /*
            ** Determine the integer value for the sync_type.
            */

	    IF LOWER(@value) = 'automatic'
		SELECT @synctypeid = @automatic
	    ELSE IF LOWER(@value) = 'manual'
		SELECT @synctypeid = @manual
	    ELSE
		SELECT @synctypeid = @none

            /*
            ** Update the subscription with the new sync_type.
            */

            UPDATE syssubscriptions
               SET sync_type = @synctypeid
             WHERE artid = @artid
               AND srvid = @srvid

            IF @@ERROR <> 0
                BEGIN
                    RAISERROR (14053, 16, -1)
                    RETURN (1)
                END

        END

    IF LOWER(@property) = 'dest_db'
        BEGIN

            /*
            ** Check to make sure that we have a valid dest_db.
            */

            EXECUTE @retcode = sp_validname @value

            IF @@ERROR <> 0 OR @retcode <> 0
		RETURN (1)

            /*
            ** Update the subscription with the new destination database.
            */

            IF EXISTS (SELECT *
                         FROM syssubscriptions
                        WHERE artid = @artid
                          AND srvid = @srvid
                          AND status = @inactive)

                BEGIN

                    UPDATE syssubscriptions
                       SET dest_db = @value
                     WHERE artid = @artid
                       AND srvid = @srvid

                    IF @@ERROR <> 0
                        BEGIN
                            RAISERROR (14053, 16, -1)
                            RETURN (1)
                        END
                END

            ELSE
                BEGIN
                    RAISERROR (14007, 16, -1)
                    RETURN (1)
                END
        END

    /*
    ** Return succeed.
    */

    RAISERROR (14054, 10, -1)
    RETURN (0)
go

print ''
print 'Creating procedure sp_helparticle.'
go

CREATE PROCEDURE sp_helparticle (
        @publication varchar(30),         /* The publication name */
        @article varchar(30) = '%',       /* The article name */
	@returnfilter bit = 1             /* Return filter flag */
        ) AS

    SET NOCOUNT ON

    /*
    ** Declarations.
    */

    DECLARE @pubid int
    DECLARE @retcode int
    DECLARE @subscriber_bit smallint

    /*
    ** Initializations.
    */

    SELECT @subscriber_bit = 4

    IF @publication IS NOT NULL
        SELECT @pubid = pubid FROM syspublications WHERE name = @publication

    /*
    ** Create a temporary table to hold all information.
    */

    CREATE TABLE #tab1 (
        artid           int,
        columns         varbinary(32),
        creation_script varchar(127) NULL,
        del_cmd         varchar(255) NULL,
        description     varchar(255) NULL,
        dest_table      varchar(30)  NULL,
        old_filter      int          NULL,
        ins_cmd         varchar(255) NULL,
        name            varchar(30),
        objid           int,
        pubid           int,
        status          tinyint,
        sync_objid      int,
        type            tinyint,
        upd_cmd         varchar(255) NULL,
        source_table    varchar(61)  NULL,      /* converted from objid */
        filter          varchar(61)  NULL,      /* converted from old_filter */
        sync_object     varchar(61)  NULL,      /* converted from sync_objid */
        vpartition      bit          NOT NULL,   /* computed */
	pre_creation_cmd tinyint,
	filter_clause   text	     NULL
    )

    CREATE UNIQUE INDEX idx1 ON #tab1 (name, pubid)

    /*
    ** Parameter Check:  @publication.
    ** Check to make sure that there are some articles
    ** to display.
    */

    IF @publication IS NULL
        BEGIN
            RAISERROR (14043, 16, -1, 'The publication')
            RETURN (1)
        END

    EXECUTE @retcode = sp_validname @publication

    IF @retcode <> 0
	RETURN (1)

    IF NOT EXISTS (SELECT * FROM syspublications WHERE name = @publication)
        BEGIN
            RAISERROR (15001, 11, -1, @publication)
            RETURN (1)
        END

    /*
    ** Parameter Check:  @article.
    ** Check to make sure that the article exists, that it conforms
    ** to the rules for identifiers, and that it isn't NULL.
    */

    IF @article IS NULL
        BEGIN
            RAISERROR (14043, 16, -1, 'The article')
            RETURN (1)
        END

    IF @article <> '%'
        BEGIN

            EXECUTE @retcode = sp_validname @article

            IF @retcode <> 0
		RETURN (1)

            IF NOT EXISTS (SELECT *
                             FROM sysarticles
                            WHERE name = @article
                              AND pubid IN (SELECT pubid
                                              FROM syspublications
                                             WHERE name = @publication))
                BEGIN
                    RAISERROR (15001, 11, -1, @article)
                    RETURN (1)
                END

        END

    /*
    ** If local user show all articles.
    */
    IF @@REMSERVER IS NULL
        BEGIN
	    IF @returnfilter = 1
	    BEGIN
            INSERT INTO #tab1 (artid, columns, creation_script, del_cmd,
                               description, dest_table, old_filter,
                               ins_cmd, name, objid, pubid, status,
                               sync_objid, type, upd_cmd, source_table,
                               filter, vpartition, pre_creation_cmd,
			       filter_clause)
             (SELECT artid, columns, creation_script, del_cmd, a.description,
                     dest_table, filter, ins_cmd, a.name, objid, a.pubid,
                     a.status, sync_objid, type, upd_cmd, NULL, NULL, 0,
		     a.pre_creation_cmd, a.filter_clause
                FROM sysarticles a, syspublications b
               WHERE a.name LIKE @article
                 AND a.pubid = b.pubid
                 AND b.name = @publication)
	    END
	    ELSE
	    BEGIN
            INSERT INTO #tab1 (artid, columns, creation_script, del_cmd,
                               description, dest_table, old_filter,
                               ins_cmd, name, objid, pubid, status,
                               sync_objid, type, upd_cmd, source_table,
                               filter, vpartition, pre_creation_cmd,
			       filter_clause)
             (SELECT artid, columns, creation_script, del_cmd, a.description,
                     dest_table, filter, ins_cmd, a.name, objid, a.pubid,
                     a.status, sync_objid, type, upd_cmd, NULL, NULL, 0,
		     a.pre_creation_cmd, NULL
                FROM sysarticles a, syspublications b
               WHERE a.name LIKE @article
                 AND a.pubid = b.pubid
                 AND b.name = @publication)
	    END
        END

    ELSE
        BEGIN

            /*
            ** Check if remote server is defined as a subscription server.
            */

            IF NOT EXISTS (SELECT *
                             FROM sysservers
                            WHERE srvname = @@REMSERVER
                              AND (srvstatus & @subscriber_bit) <> 0)
                BEGIN
                    RAISERROR (14010, 16, -1)
                    RETURN (1)
                END

            /*
            ** If the publication is public, we can display it.
            */

            IF NOT EXISTS (SELECT *
                             FROM syspublications
                            WHERE name = @publication
                              AND restricted = 0)

                /*
                ** The publication wasn't public, so let's check to see
                ** if there's a restricted publication on which I have
                ** permission.
                */

                IF NOT EXISTS (SELECT *
                                 FROM syssubscriptions a,
                                      sysarticles b,
                                      syspublications c
                                WHERE c.name = @publication
                                  AND c.restricted = 1
                                  AND c.pubid = b.pubid
                                  AND b.name LIKE @article
                                  AND b.artid = a.artid)

                    BEGIN
                        RAISERROR (14011, 16, -1)
                        RETURN (1)
                    END

            /*
            ** Fetch the information into the temporary table.  First, put
            ** in the information about public publications if this publi-
            ** cation is public.  Next, if the publication is restricted,
            ** put in only those articles on which you have been granted
            ** access (sp_addsubscription).
            */

	    IF @returnfilter = 1
	    BEGIN
            INSERT INTO #tab1 (artid, columns, creation_script, del_cmd,
                               description, dest_table, old_filter,
			       ins_cmd, name, objid, pubid, status,
                               sync_objid, type, upd_cmd, source_table,
                               filter, vpartition, pre_creation_cmd,
			       filter_clause)
             (SELECT a.artid, columns, creation_script, del_cmd, a.description,
                     dest_table, filter, ins_cmd, a.name, objid, a.pubid,
                     a.status, sync_objid, type, upd_cmd, NULL, NULL, 0,
		     a.pre_creation_cmd, a.filter_clause
                FROM sysarticles a,
                     syspublications b
               WHERE a.name LIKE @article
                 AND a.pubid = b.pubid
                 AND b.name = @publication
                 AND b.restricted = 0)

            INSERT INTO #tab1 (artid, columns, creation_script, del_cmd,
                               description, dest_table, old_filter,
                               ins_cmd, name, objid, pubid, status,
                               sync_objid, type, upd_cmd, source_table,
                               filter, vpartition, pre_creation_cmd,
			       filter_clause)
             (SELECT a.artid, columns, creation_script, del_cmd, a.description,
                     dest_table, filter, ins_cmd, a.name, objid, a.pubid,
                     a.status, sync_objid, type, upd_cmd, NULL, NULL, 0,
		     a.pre_creation_cmd, a.filter_clause
                FROM sysarticles a,
                     syspublications b,
                     syssubscriptions c,
                     master..sysservers d
               WHERE a.name LIKE @article
                 AND a.pubid = b.pubid
                 AND b.name = @publication
                 AND b.restricted = 1
                 AND a.artid = c.artid
                 AND c.srvid = d.srvid
                 AND d.srvname = @@REMSERVER)
	    END
	    ELSE
       	    BEGIN
            INSERT INTO #tab1 (artid, columns, creation_script, del_cmd,
                               description, dest_table, old_filter,
			       ins_cmd, name, objid, pubid, status,
                               sync_objid, type, upd_cmd, source_table,
                               filter, vpartition, pre_creation_cmd,
			       filter_clause)
             (SELECT a.artid, columns, creation_script, del_cmd, a.description,
                     dest_table, filter, ins_cmd, a.name, objid, a.pubid,
                     a.status, sync_objid, type, upd_cmd, NULL, NULL, 0,
		     a.pre_creation_cmd, NULL
                FROM sysarticles a,
                     syspublications b
               WHERE a.name LIKE @article
                 AND a.pubid = b.pubid
                 AND b.name = @publication
                 AND b.restricted = 0)

            INSERT INTO #tab1 (artid, columns, creation_script, del_cmd,
                               description, dest_table, old_filter,
                               ins_cmd, name, objid, pubid, status,
                               sync_objid, type, upd_cmd, source_table,
                               filter, vpartition, pre_creation_cmd,
			       filter_clause)
             (SELECT a.artid, columns, creation_script, del_cmd, a.description,
                     dest_table, filter, ins_cmd, a.name, objid, a.pubid,
                     a.status, sync_objid, type, upd_cmd, NULL, NULL, 0,
		     a.pre_creation_cmd, NULL
                FROM sysarticles a,
                     syspublications b,
                     syssubscriptions c,
                     master..sysservers d
               WHERE a.name LIKE @article
                 AND a.pubid = b.pubid
                 AND b.name = @publication
                 AND b.restricted = 1
                 AND a.artid = c.artid
                 AND c.srvid = d.srvid
                 AND d.srvname = @@REMSERVER)
	    END
        END

    UPDATE #tab1
       SET source_table = u.name + '.' + o.name
      FROM #tab1, sysobjects o, sysusers u
     WHERE o.id = #tab1.objid
       AND o.uid = u.uid

    UPDATE #tab1
       SET sync_object = sysusers.name + '.' + sysobjects.name
      FROM sysobjects, sysusers
     WHERE sysobjects.id = sync_objid
       AND sysobjects.uid = sysusers.uid

    UPDATE #tab1 SET filter = (SELECT sysusers.name + '.' + sysobjects.name
                                 FROM sysobjects, sysusers
                                WHERE sysobjects.id = #tab1.old_filter
                                  AND sysobjects.uid = sysusers.uid)
      FROM #tab1

    EXECUTE ('DECLARE hC SCROLL CURSOR FOR SELECT name, pubid FROM #tab1')
    OPEN hC
    FETCH hC INTO @article, @pubid
    WHILE (@@fetch_status <> -1)
        BEGIN
            IF EXISTS (SELECT *
                         FROM sysarticles a, syscolumns b
                        WHERE (CONVERT(bit, SUBSTRING(a.columns, CONVERT(tinyint, 32 - FLOOR((colid-1)/8)), 1) & POWER(2, ((colid-1)%8))) = 0
                           OR CONVERT(bit, SUBSTRING(a.columns, CONVERT(tinyint, 32 - FLOOR((colid-1)/8)), 1) & POWER(2, ((colid-1)%8))) IS NULL)
                          AND a.objid = b.id
                          AND a.name = @article
                          AND a.pubid = @pubid)

                UPDATE #tab1
                   SET vpartition = 1
                 WHERE name = @article
                   AND pubid = @pubid

            FETCH hC INTO @article, @pubid
        END
    CLOSE hC
    DEALLOCATE hC

    IF @returnfilter = 1
	    SELECT 'article id'                = artid,
		   'article name'              = name,
		   'base table'                = source_table,
		   'destination table'         = dest_table,
		   'synchronization object'    = sync_object,
		   'type'                      = type,
		   'status'                    = status,
		   'filter'                    = filter,
		   'description'               = description,
		   'insert_command'            = ins_cmd,
		   'update_command'            = upd_cmd,
		   'delete_command'            = del_cmd,
		   'creation script path'      = creation_script,
		   'vertical partition'        = vpartition,
		   'pre_creation_cmd'       = pre_creation_cmd,
		   'filter_clause'	       = filter_clause
	      FROM #tab1
	     ORDER BY 2
    ELSE
	    SELECT 'article id'                = artid,
		   'article name'              = name,
		   'base table'                = source_table,
		   'destination table'         = dest_table,
		   'synchronization object'    = sync_object,
		   'type'                      = type,
		   'status'                    = status,
		   'filter'                    = filter,
		   'description'               = description,
		   'insert_command'            = ins_cmd,
		   'update_command'            = upd_cmd,
		   'delete_command'            = del_cmd,
		   'creation script path'      = creation_script,
		   'vertical partition'        = vpartition,
		   'pre_creation_cmd'       = pre_creation_cmd
	      FROM #tab1
	     ORDER BY 2
    RETURN (0)
go

dump tran master with no_log
go

print ''
print 'Creating procedure sp_articlecolumn.'
go
CREATE PROCEDURE sp_articlecolumn (
        @publication varchar(30),           /* The publication name */
        @article varchar(30),               /* The article name */
        @column varchar(30) = NULL,         /* The column name */
        @operation varchar(4) = 'add'      /* Add or delete a column */
        ) AS

    /*
    ** Declarations.
    */

    DECLARE @bit tinyint                /* Bit offset */
    DECLARE @byte tinyint               /* Byte offset */
    DECLARE @cnt tinyint, @idx tinyint  /* Loop counter, index */
    DECLARE @columns binary(32)         /* Temporary storage for the converted column */
    DECLARE @mask smallint              /* Bit mask to set the bit on */
    DECLARE @newbyte binary(1)          /* New byte to replace old byte with */
    DECLARE @oldbyte binary(1)          /* Temporary storage for original byte */
    DECLARE @pubid int                  /* Publication identification number */
    DECLARE @retcode int                /* Return code for stored procedures */
    DECLARE @zero binary(32)            /* Constant:  0 */
    DECLARE @artid int
    DECLARE @inactive tinyint
    DECLARE @objid int			/* Article base table id */	

    select @inactive = 0

    /*
    ** Security Check
    ** Only the System Administratr (SA) or the Database Owner (dbo) can
    ** perform this procedure.
    */

    IF suser_id() <> 1 AND user_id() <> 1
        BEGIN
            RAISERROR (15000, 14, -1)
    	    RETURN (1)
        END

    /*
    ** Check to see if the database has been activated for publication.
    */

    IF (SELECT category & 1
          FROM master..sysdatabases
         WHERE name = DB_NAME()) = 0

	BEGIN
            RAISERROR (14013, 16, -1)
	    RETURN (1)
	END

    /*
    ** Parameter Check:  @publication.
    ** Make sure that the publication exists and that it conforms to the
    ** rules for identifiers.
    */

    IF @publication IS NULL
        BEGIN
            RAISERROR (14043, 16, -1, 'The publication')
            RETURN (1)
        END

    EXECUTE @retcode = sp_validname @publication

    IF @retcode <> 0
            RETURN (1)

    SELECT @pubid = pubid FROM syspublications WHERE name = @publication

    IF @pubid IS NULL
        BEGIN
            RAISERROR (15001, 11, -1, @publication)
    	    RETURN (1)
        END
    ELSE

    /*
    ** Parameter Check:  @article.
    ** Check to make sure that the article exists in the publication.
    */

    IF @article IS NULL
        BEGIN
            RAISERROR (14043, 16, -1, 'The article')
            RETURN (1)
        END

    EXECUTE @retcode = sp_validname @article

    IF @@ERROR <> 0 OR @retcode <> 0
	RETURN (1)


    /*
    ** Make sure the article exists.
    */
    SELECT @artid = artid FROM sysarticles
       WHERE pubid = @pubid AND name = @article
    IF @artid IS NULL
        BEGIN
            RAISERROR (15001, 11, -1, @article)
            RETURN (1)
        END

    /*
    ** Only unsubscribed articles may be modified.
    */
    IF EXISTS (SELECT * FROM syssubscriptions WHERE artid = @artid
       AND status <> @inactive)
        BEGIN
            RAISERROR (14092, 11, -1)
            RETURN (1)
        END

    /*
    ** Parameter Check:  @column.
    ** Check to make sure that the column exists and conforms to the rules
    ** for identifiers.
    */

    IF @column IS NOT NULL
        BEGIN
            EXECUTE @retcode = sp_validname @column
            IF @@ERROR <> 0 OR @retcode <> 0
		RETURN (1)
        END

    /*
    ** Parameter Check:  @operation.
    ** The operation can be either 'add' or 'drop'.
    */

    IF LOWER(@operation) NOT IN ('add', 'drop')
        BEGIN
            RAISERROR (14019, 16, -1)
            RETURN (1)
        END

    BEGIN TRANSACTION articlecolumn

    /*
    ** Make sure that the columns column is not NULL.
    */

    SELECT @zero = 0x00

    SELECT @columns = columns
      FROM sysarticles
     WHERE artid = @artid

    IF @columns IS NULL
        UPDATE sysarticles
           SET columns = @zero
         WHERE artid = @artid

    SELECT @objid = (SELECT objid FROM sysarticles WHERE artid = @artid)

    /*
    ** If no columns are specified, or if NULL is specified, set all
    ** the bits in the 'columns' column so all columns will be included.
    */

    IF @column IS NULL

        BEGIN

            /*
            ** Fetch the number of columns affected.
            */

            SELECT @cnt = COUNT(*), @idx = 1
              FROM syscolumns
             WHERE id = @objid

            SELECT @columns = @zero

            WHILE @idx <= @cnt
                BEGIN

                    SELECT @byte = CONVERT(tinyint, 32 - FLOOR((@idx-1)/8))
                    SELECT @bit = (@idx-1) % 8

                    IF LOWER(@operation) = 'add'
                        SELECT @mask = POWER(2, @bit)
                    ELSE
                        SELECT @mask = ~POWER(2, @bit)

                    SELECT @oldbyte = SUBSTRING(@columns, @byte, 1)
                    IF @oldbyte IS NULL SELECT @oldbyte = 0x00

                    IF LOWER(@operation) = 'add'
                        SELECT @newbyte = CONVERT(binary(1), ASCII(@oldbyte) | @mask)
                    ELSE
                        SELECT @newbyte = CONVERT(binary(1), ASCII(@oldbyte) & @mask)

                    SELECT @columns = CONVERT(binary(32), STUFF(@columns, @byte, 1, @newbyte))
                    SELECT @idx = @idx + 1


                END

            IF LOWER(@operation) = 'drop'
	       BEGIN
 	          /* Update Text\Image column status as not published */
	          EXECUTE @retcode = sp_articletextcol @artid, NULL,
	             'publish', @operation
	          IF (@@error <> 0 OR @retcode <> 0)
	          BEGIN
	              ROLLBACK TRANSACTION articlecolumn
	              RAISERROR (14020, 16, -1)
	              RETURN (1)
	          END
	       END

            UPDATE sysarticles
               SET columns = @columns
             WHERE name = @article
               AND pubid = @pubid

            IF LOWER(@operation) = 'add'
	       BEGIN
 	          /* Update Text\Image column status as published */
	          EXECUTE @retcode = sp_articletextcol @artid, NULL,
	             'publish', @operation
	          IF (@@error <> 0 OR @retcode <> 0)
	          BEGIN
	              ROLLBACK TRANSACTION articlecolumn
	              RAISERROR (14020, 16, -1)
	              RETURN (1)
	          END
	       END
        END

    ELSE

        BEGIN
                IF EXISTS (SELECT *
                             FROM sysarticles
                            WHERE name = @article
                              AND pubid = @pubid
                              AND columns IS NULL)

                        UPDATE sysarticles
                          SET columns = @zero
                        WHERE name = @article
                          AND pubid = @pubid

                DECLARE @columnid tinyint   /* Columnid-1 = bit to set */

                /*
                ** Get the column id for this column.  We'll use the column id
                ** to determine the bit in the 'columns' column.  The bit we want
                ** is equal to the columnid - 1.
                */

                SELECT @columnid = colid
                  FROM syscolumns
                 WHERE id = @objid AND name = @column

                IF ((@@error <> 0) OR (@columnid IS NULL))
                    BEGIN
                        ROLLBACK TRANSACTION articlecolumn
                        RAISERROR (14020, 16, -1)
                        RETURN (1)
                    END

                /*
                ** Obtain the byte offset and the bit offset, then set the
                ** mask column for the bit we want to turn on.
                */

                SELECT @byte = CONVERT(tinyint, 32 - FLOOR((@columnid-1)/8.0))
                SELECT @bit = (@columnid-1) % 8

                IF LOWER(@operation) = 'add'
                    SELECT @mask = POWER(2, @bit)
                ELSE
                    SELECT @mask = ~POWER(2, @bit)

                /*
                ** Save the columns column in a temporary local variable so we
                ** can twiddle the bit and then put it back into the table.
                */

                SELECT @columns = columns
                  FROM sysarticles
                 WHERE name = @article
                   AND pubid = @pubid

                /*
                ** Fish out the byte we're interested in and save it in a
                ** a temporary local variable.  If it's NULL, just set it
                ** to 0.  Then apply the bitwise operator OR to twiddle the
                ** bit in the old byte and save it in another temporary
                ** local variable @newbyte.
                */

                SELECT @oldbyte = SUBSTRING(@columns, @byte, 1)
                IF @oldbyte IS NULL SELECT @oldbyte = 0x00

                IF LOWER(@operation) = 'add'
                    SELECT @newbyte = CONVERT(binary(1), ASCII(@oldbyte) | @mask)
                ELSE
                    SELECT @newbyte = CONVERT(binary(1), ASCII(@oldbyte) & @mask)

                /*
                ** Stuff the new byte into the varbinary column to replace the
                ** old byte.
                */

                SELECT @columns = CONVERT(varbinary(32), STUFF(@columns+@zero, @byte, 1, @newbyte))

                IF LOWER(@operation) = 'drop'
	           BEGIN
 	              /* Update Text\Image column status as not published */
	              EXECUTE @retcode = sp_articletextcol @artid, @columnid,
	                 'publish', @operation
	              IF (@@error <> 0 OR @retcode <> 0)
	              BEGIN
	                 ROLLBACK TRANSACTION articlecolumn
	                 RAISERROR (14021, 16, -1)
	                 RETURN (1)
	              END
	           END

                /*
                ** Update the sysarticles table.  Set the bit to 1 for the
                ** selected column.
                */

                UPDATE sysarticles
                   SET columns = @columns
                 WHERE name = @article
                   AND pubid = @pubid

                IF @@error <> 0
                    BEGIN
                        ROLLBACK TRANSACTION articlecolumn
                        RAISERROR (14021, 16, -1)
                        RETURN (1)
                    END

                IF LOWER(@operation) = 'add'
	           BEGIN
 	              /* Update Text\Image column status as not published */
	              EXECUTE @retcode = sp_articletextcol @artid, @columnid,
	                 'publish', @operation
	              IF (@@error <> 0 OR @retcode <> 0)
	              BEGIN
	                 ROLLBACK TRANSACTION articlecolumn
	                 RAISERROR (14021, 16, -1)
	                 RETURN (1)
	              END
	           END
        END
    /*
    ** Force the article cache to be refreshed with the new definition.
    */
    EXECUTE sp_replflush

    COMMIT TRANSACTION articlecolumn
go

print ''
print 'Creating procedure sp_helparticlecolumns.'
go
CREATE PROCEDURE sp_helparticlecolumns (
    @publication varchar(30),            /* The publication name */
    @article    varchar(30)              /* The article name */
    ) AS

    /*
    ** Declarations.
    */

    DECLARE @columns binary(32)
    DECLARE @pubid int
    DECLARE @retcode int

    /*
    ** Parameter Check: @article.
    ** The @article name must conform to the rules for identifiers.
    */

    IF @article IS NULL
        BEGIN
            RAISERROR (14043, 16, -1, 'The article')
            RETURN (1)
        END

    EXECUTE @retcode = sp_validname @article

    IF @retcode <> 0
	RETURN (1)

    /*
    ** Parameter Check: @publication.
    ** The @publication name must conform to the rules for identifiers.
    */

    IF @publication IS NULL
        BEGIN
            RAISERROR (14043, 16, -1, 'The publication')
            RETURN (1)
        END

    EXECUTE @retcode = sp_validname @publication

    IF @retcode <> 0
	RETURN (1)

    /*
    ** Get the pubid.
    */

    SELECT @pubid = pubid FROM syspublications WHERE name = @publication

    IF @pubid IS NULL
        BEGIN
            RAISERROR (14027, 11, -1, 'The publication')
            RETURN (1)
        END

    /*
    ** Parameter Check:  @article, @publication.
    ** Check to make sure that the article exists in this publication.
    */

    IF NOT EXISTS (SELECT *
                     FROM sysarticles
                    WHERE pubid = @pubid
                      AND name = @article)
        BEGIN
            RAISERROR (15001, 11, -1, 'The article')
    	    RETURN (1)
        END

    IF @@REMSERVER IS NOT NULL

        /*
        ** Is the publication/article restricted?
        */

        IF EXISTS (SELECT *
                     FROM syspublications
                    WHERE pubid = @pubid
                      AND restricted = 1)

            /*
            ** We have a restricted publication.  Does the subscriber
            ** have access to it?
            */

            IF NOT EXISTS (SELECT *
                             FROM sysarticles a,
                                  syssubscriptions b,
                                  master..sysservers c
                            WHERE c.srvname = @@REMSERVER
                              AND c.srvid = b.srvid
                              AND b.artid = a.artid
                              AND a.name = @article
                              AND a.pubid = @pubid)
                BEGIN
                    RAISERROR (14011, 16, -1)
                    RETURN (1)
                END

    SELECT @columns = columns
      FROM sysarticles
     WHERE name = @article
       AND pubid = @pubid

    SELECT 'column id' = colid,
           'column'    = name,
	   'published' = CONVERT(bit, SUBSTRING(@columns, CONVERT(tinyint, 32 - FLOOR((colid-1)/8)), 1) & POWER(2, ((colid-1)%8)))
      FROM syscolumns
     WHERE id = (SELECT objid
                   FROM sysarticles
                  WHERE name = @article
                    AND pubid = @pubid)
go

print ''
print 'Creating procedure sp_helppublication.'
go

CREATE PROCEDURE sp_helppublication (
        @publication varchar(30) = '%'     /* The publication name */
        ) AS

    SET NOCOUNT ON

    /*
    ** Declarations.
    */

    DECLARE @retcode int
    DECLARE @subscriber_bit smallint

    /*
    ** Initializations.
    */

    SELECT @subscriber_bit = 4

    /*
    ** If local user show all publications.
    */
    IF @@REMSERVER IS NULL
        BEGIN

            /*
            ** Parameter Check:  @publication.
            ** Check to make sure that there are some publications
            ** to display.
            */

            IF @publication IS NULL
                BEGIN
                    RAISERROR (14043, 16, -1, 'The publication')
                    RETURN (1)
                END

            IF @publication <> '%'
                BEGIN

                    EXECUTE @retcode = sp_validname @publication

                    IF @retcode <> 0
			RETURN (1)
                END

            IF @publication <> '%' AND NOT EXISTS (SELECT *
                                                     FROM syspublications
                                                    WHERE name = @publication)
            	BEGIN
                    RAISERROR (15001, 11, -1, @publication)
	    	    RETURN (1)
	        END

            SELECT 'pubid'                  = pubid,
                   'name'                   = name,
                   'restricted'             = restricted,
                   'status'                 = status,
                   'task'                   = taskid,
                   'replication frequency'  = repl_freq,
                   'synchronization method' = sync_method,
                   'description'            = description
              FROM syspublications
             WHERE name LIKE @publication
             ORDER BY name

            IF @@ERROR <> 0 RETURN (1)

        END

    ELSE
        BEGIN

            /*
            ** Check if remote server is defined as a subscription server.
            */

            IF NOT EXISTS (SELECT *
                             FROM sysservers
                            WHERE srvname = @@REMSERVER
                              AND (srvstatus & @subscriber_bit) <> 0)
                BEGIN
                    RAISERROR (14010, 16, -1)
                    RETURN (1)
                END

            /*
            ** Parameter Check:  @publication.
            ** Check to make sure that there are some publications
            ** to display.
            */

            IF @publication IS NULL
                BEGIN
                    RAISERROR (14043, 16, -1, 'The publication')
                    RETURN (1)
                END

            IF @publication <> '%'
                BEGIN

                    EXECUTE @retcode = sp_validname @publication

                    IF @retcode <> 0
			RETURN (1)
                END

            IF @publication <> '%'
                AND NOT EXISTS (SELECT *
                                  FROM syspublications
            		         WHERE restricted = 0
                                   AND name = @publication)
            	AND NOT EXISTS (SELECT *
            		          FROM syspublications a,
                                       syssubscriptions b,
                                       sysarticles c,
                                       master..sysservers d
            			 WHERE a.restricted = 1
                                   AND a.name = @publication
            		           AND a.pubid = c.pubid
                                   AND c.artid = b.artid
            			   AND b.srvid = d.srvid
            			   AND d.srvname =  @@REMSERVER)
                BEGIN
                    RAISERROR (14011, 16, -1)
            	    RETURN (1)
                END

            /*
            ** Display all public publications and all restricted publications
            ** allowed for the server.
            */

            SELECT 'pubid'                  = pubid,
                   'name'                   = name,
                   'restricted'             = restricted,
                   'status'                 = status,
                   'task'                   = taskid,
                   'replication frequency'  = repl_freq,
                   'synchronization method' = sync_method,
                   'description'            = description
              FROM syspublications
             WHERE restricted = 0
               AND name LIKE @publication
             UNION
            SELECT a.pubid, a.name, a.restricted, a.status, a.taskid, a.repl_freq, a.sync_method, a.description
              FROM syspublications a,
                   syssubscriptions b,
                   sysarticles c,
                   master..sysservers d
             WHERE a.restricted = 1
               AND a.name LIKE @publication
               AND a.pubid = c.pubid
               AND c.artid = b.artid
               AND b.srvid = d.srvid
               AND d.srvname =  @@REMSERVER
             ORDER BY name

            IF @@ERROR <> 0 RETURN (1)
        END

    RETURN (0)
go

print ''
print 'Creating procedure sp_helppublicationsync.'
go

CREATE PROCEDURE sp_helppublicationsync (
        @publication varchar(30) 	/* The publication name */
        ) AS

    SET NOCOUNT ON

    /*
    ** Declarations.
    */

    DECLARE @retcode int
    DECLARE @subscriber_bit smallint
    DECLARE @taskid int
    DECLARE @distributor varchar(30)
    DECLARE @distproc varchar (255)

    /*
    ** Initializations.
    */

    SELECT @subscriber_bit = 4

    /*
    ** If local user show all publications.
    */
    IF @@REMSERVER IS NULL
        BEGIN

            /*
            ** Parameter Check:  @publication.
            ** Check to make sure that there are some publications
            ** to display.
            */

            IF @publication IS NULL
                BEGIN
                    RAISERROR (14043, 16, -1, 'The publication')
                    RETURN (1)
                END

            IF @publication = '%'
                BEGIN
                    RAISERROR (14003, 16, -1, 'The publication')
                    RETURN (1)
                END

            EXECUTE @retcode = sp_validname @publication
            IF @@ERROR <> 0 OR @retcode <> 0 RETURN (1)

            IF NOT EXISTS (SELECT * FROM syspublications
               WHERE name = @publication)
               BEGIN
                    RAISERROR (15001, 11, -1, @publication)
	    	    RETURN (1)
	       END

        END

    ELSE
        BEGIN

            /*
            ** Check if remote server is defined as a subscription server.
            */

            IF NOT EXISTS (SELECT *
                             FROM sysservers
                            WHERE srvname = @@REMSERVER
                              AND (srvstatus & @subscriber_bit) <> 0)
                BEGIN
                    RAISERROR (14010, 16, -1)
                    RETURN (1)
                END

            /*
            ** Parameter Check:  @publication.
            ** Check to make sure that there are some publications
            ** to display.
            */

            IF @publication IS NULL
                BEGIN
                    RAISERROR (14043, 16, -1, 'The publication')
                    RETURN (1)
                END

            IF @publication = '%'
                BEGIN
                    RAISERROR (14003, 16, -1, 'The publication')
                    RETURN (1)
                END

            IF  NOT EXISTS (SELECT *
                                  FROM syspublications
            		         WHERE restricted = 0
                                   AND name = @publication)
            	AND NOT EXISTS (SELECT *
            		          FROM syspublications a,
                                       syssubscriptions b,
                                       sysarticles c,
                                       master..sysservers d
            			 WHERE a.restricted = 1
                                   AND a.name = @publication
            		           AND a.pubid = c.pubid
                                   AND c.artid = b.artid
            			   AND b.srvid = d.srvid
            			   AND d.srvname =  @@REMSERVER)
                BEGIN
                    RAISERROR (14011, 16, -1)
            	    RETURN (1)
                END

        END

    /*
    ** Get the publication sync task id
    */
    SELECT @taskid = taskid FROM syspublications WHERE name LIKE @publication
    IF @@ERROR <> 0 RETURN (1)

    /*
    ** Get distribution server information for remote RPC
    ** task verification.
    */
    EXEC @retcode = sp_helpdistributor @distributor = @distributor OUTPUT
    IF @@error <> 0 OR @retcode <> 0
        BEGIN
	    RAISERROR (14071, 16, -1)
    	    RETURN (1)
	END

    /*
    ** The @taskid must exists in the systasks table.
    */
    SELECT @distproc = RTRIM(@distributor) + '.msdb..sp_verifytaskid'
    EXECUTE @retcode = @distproc @taskid = @taskid, @subsystem = 'Sync'
    IF @@ERROR <> 0 or @retcode <> 0
        BEGIN
            RAISERROR (14002, 16, -1, @taskid)
            RETURN (1)
        END

    /*
    ** Return sync task information
    */
    SELECT @distproc = RTRIM(@distributor) + '.msdb..sp_helptask'
    EXECUTE @retcode = @distproc @taskid = @taskid, @mode = 'full'
    IF @@ERROR <> 0 or @retcode <> 0
       RETURN (1)
go

print ''
print 'Creating procedure sp_helpreplicationdb.'
go

CREATE PROCEDURE sp_helpreplicationdb
        @dbname varchar (30) = '%', @type varchar(30) = 'pub'
	AS

    SET NOCOUNT ON

    /*
    ** Declarations.
    */

    DECLARE @retcode int, @typebit int

    if (lower(@type) like 'pub%')
       select @typebit = 1
    else if (lower(@type) like 'sub%')
       select @typebit = 2
    else
    begin
       raiserror(14091,-1,-1)
       return 1
    end

    /*
    ** Parameter Check:  @dbname.
    ** Check to make sure that the database name conforms to the rules
    ** for identifiers.
    */


    IF @dbname <> '%'
       BEGIN
          EXECUTE @retcode = sp_validname @dbname

          IF @@ERROR <> 0 OR @retcode <> 0
		RETURN (1)
       END

    /*
    ** Show databases with this option enabled.
    */

    SELECT name
      FROM sysdatabases
     WHERE name LIKE @dbname
       AND (category & @typebit) <> 0
go


print ''
print 'Creating procedure sp_enumdsn.'
go
CREATE PROCEDURE sp_enumdsn
	AS

    SET NOCOUNT ON

    DECLARE @distributor varchar(30)
    DECLARE @distproc varchar (255)
    DECLARE @retcode int

    /*
    ** Get distribution server information for remote RPC
    ** subscription calls.
    */

    EXEC @retcode = sp_helpdistributor @distributor = @distributor OUTPUT
    IF @@error <> 0 OR @retcode <> 0
        BEGIN
	    RAISERROR (14071, 16, -1)
    	    RETURN (1)
	END

    /*
    ** Call xp_enumdsn
    */
    SELECT @distproc = RTRIM(@distributor) + '.master..xp_enumdsn'
    EXEC @retcode = @distproc
    IF @@error <> 0
        BEGIN
	    RAISERROR (14071, 16, -1)
	    RETURN (1)
	END

go

print ''
print 'Creating procedure sp_enumfullsubscribers.'
go

CREATE PROCEDURE sp_enumfullsubscribers (
        @publication varchar(30) = '%'      /* The publication name */
    ) AS

    /*
    ** Declarations.
    */

    DECLARE @retcode int

    /*
    ** Parameter Check: @publication.
    ** Check to make sure that the publication exists and that it conforms
    ** to the rules for identifiers.
    */

    IF @publication IS NOT NULL
        BEGIN

            IF @publication <> '%'
                BEGIN

                    EXECUTE @retcode = sp_validname @publication

                    IF @retcode <> 0
			RETURN (1)
                END

            IF NOT EXISTS (SELECT * FROM syspublications WHERE name LIKE @publication)
                BEGIN
                    RAISERROR (15001, 11, -1, @publication)
                    RETURN (1)
                END

        END

    ELSE
        BEGIN
            RAISERROR (14043, 16, -1, 'The publication')
            RETURN (1)
        END

    /*
    ** Select all subscribers who subscribe to all articles in the desired
    ** publication.
    */

    SELECT DISTINCT 'subscriber' = sv.srvname
      FROM syspublications p,
           sysarticles s,
           syssubscriptions ss,
           master..sysservers sv
     WHERE p.name LIKE @publication
       AND p.pubid = s.pubid
       AND s.artid = ss.artid
       AND ss.srvid = sv.srvid
       AND NOT EXISTS (SELECT *
                         FROM sysarticles s2
                        WHERE s2.pubid = p.pubid
                          AND NOT EXISTS (SELECT *
                                            FROM syssubscriptions ss2,
                                                 master..sysservers sv2
                                           WHERE s2.artid = ss2.artid
                                             AND ss2.srvid = sv2.srvid
                                             AND sv2.srvid = sv.srvid))
go

print ''
print 'Creating procedure sp_helpsubscriberinfo'
go

CREATE PROCEDURE sp_helpsubscriberinfo
        @subscriber varchar (30)
	AS

    SET NOCOUNT ON

    DECLARE @distributor varchar(30)
    DECLARE @distribdb varchar(30)
    DECLARE @distproc varchar (255)
    DECLARE @retcode int

    /*
    ** Check if subscriber is valid
    */

    IF @subscriber IS NULL
        BEGIN
            RAISERROR (14043, 16, -1, 'The subscriber')
            RETURN (1)
        END

    EXECUTE @retcode = sp_validname @subscriber

    IF @retcode <> 0
	RETURN (1)

    IF NOT EXISTS (SELECT *
                     FROM master..sysservers
                    WHERE srvname = @subscriber)

        BEGIN
	    RAISERROR (14209, 16, -1, @subscriber)
    	    RETURN (1)
        END

    /*
    ** Get distribution server information for remote RPC
    ** subscription calls.
    */

    EXEC @retcode = sp_helpdistributor @distributor = @distributor OUTPUT,
                                       @distribdb   = @distribdb OUTPUT

    IF @@error <> 0 OR @retcode <> 0 OR @distribdb IS NULL OR @distributor IS NULL
        BEGIN
	    RAISERROR (14071, 16, -1)
    	    RETURN (1)
	END

    /*
    ** Retrieve MSsubscriber_info
    */

    SELECT @distproc = RTRIM(@distributor) + '.' +
        RTRIM(@distribdb) + '..sp_MShelp_subscriber_info '

    EXEC @retcode = @distproc @@SERVERNAME, @subscriber

    IF @@error <> 0
        BEGIN
	    RAISERROR (14071, 16, -1)
	    RETURN (1)
	END

    IF @retcode <> 0
        BEGIN
	    RAISERROR (14085, 16, -1)
	    RETURN (1)
	END

go

print ''
print 'Creating procedure sp_helpsubscription.'
go

CREATE PROCEDURE sp_helpsubscription
	@publication varchar (30) = '%',    /* The publication name */
	@article varchar (30) = '%',        /* The article name */
	@subscriber varchar (30) = '%'      /* The subscriber name */
	AS

    SET NOCOUNT ON

    /*
    ** Declarations.
    */

    DECLARE @retcode int
    DECLARE @subscriber_bit smallint


    /*
    ** Initializations.
    */
    SELECT @subscriber_bit = 4

    /*
    ** Parameter Check:  @subscriber.
    ** If remote server, limit the view to the remote server's subscriptions.
    ** Make sure that the name isn't NULL.
    */

    IF @@REMSERVER IS NOT NULL SELECT @subscriber = @@REMSERVER

    IF @subscriber IS NULL
        BEGIN
            RAISERROR (14043, 16, -1, 'The subscriber')
            RETURN (1)
        END

    /*
    ** Parameter Check:  @subscriber.
    ** Check if remote server is defined as a subscription server, and
    ** that the name conforms to the rules for identifiers.
    */

    IF @subscriber <> '%'
        BEGIN

            EXECUTE @retcode = sp_validname @subscriber

            IF @retcode <> 0
		RETURN (1)

            IF NOT EXISTS (SELECT *
                             FROM sysservers
                            WHERE srvname = @subscriber
                              AND (srvstatus & @subscriber_bit) <> 0)
                BEGIN
                    RAISERROR (14010, 16, -1)
                    RETURN (1)
                END
        END

    /*
    ** Parameter Check:  @publication.
    ** If the publication name is specified, check to make sure that it
    ** conforms to the rules for identifiers and that the publication
    ** actually exists.  Disallow NULL.
    */

    IF @publication IS NULL
        BEGIN
            RAISERROR (14043, 16, -1, 'The publication')
            RETURN (1)
        END

    IF @publication <> '%'
        BEGIN

            EXECUTE @retcode = sp_validname @publication

            IF @retcode <> 0
		RETURN (1)

            IF NOT EXISTS (SELECT * FROM syspublications WHERE name LIKE @publication)
                BEGIN
                   IF @publication = '%'
                       RAISERROR (14008, 11, -1)
                   ELSE
                       RAISERROR (15001, 11, -1, @publication)
                   RETURN (1)
                END

        END

    /*
    ** Parameter Check:  @article.
    ** If the article name is specified, check to make sure that it
    ** conforms to the rules for identifiers and that the article
    ** actually exists.  Disallow NULL.
    */

    IF @article IS NULL
        BEGIN
            RAISERROR (14043, 16, -1, 'The article')
            RETURN (1)
        END

    IF @article <> '%'
        BEGIN

            EXECUTE @retcode = sp_validname @article

            IF @retcode <> 0
		RETURN (1)

            IF NOT EXISTS (SELECT *
                             FROM sysarticles
                            WHERE name = @article
                              AND pubid IN (SELECT pubid
                                              FROM syspublications
                                             WHERE name LIKE @publication))
                BEGIN
                    RAISERROR (15001, 11, -1, @article)
                    RETURN (1)
                END

        END

    /*
    ** Get subscriptions
    */

    SELECT 'subscriber'           = ss.srvname,
           'publication'          = pub.name,
           'article'              = art.name,
           'destination database' = sub.dest_db,
           'subscription status'  = sub.status,
           'synchronization type' = sub.sync_type
      FROM syssubscriptions sub,
           sysservers ss,
           syspublications pub,
           sysarticles art
     WHERE ss.srvname LIKE @subscriber
       AND sub.srvid = ss.srvid
       AND pub.name LIKE @publication
       AND art.name LIKE @article
       AND art.pubid = pub.pubid
       AND sub.artid = art.artid
     ORDER BY subscriber, publication, article
go

print ''
print 'Creating procedure sp_replica.'
go

CREATE PROCEDURE sp_replica (
        @tabname varchar(92),     /* The table being replicated */
        @replicated varchar(5)    /* True or false */
        ) AS

    /*
    ** Declarations.
    */

    DECLARE @db varchar(30)
    DECLARE @id int
    DECLARE @object varchar(30)
    DECLARE @owner varchar(30)
    DECLARE @replica_bit int
    DECLARE @retcode int
    DECLARE @site varchar(30)

    /*
    ** Initializations.
    */

    SELECT @replica_bit = 256

    /*
    ** Parameter Check: @tabname.
    ** Check to make sure that the table exists and that it conforms to the
    ** rules for identifiers.
    */

    IF @tabname IS NULL
        BEGIN
            RAISERROR (14043, 16, -1, 'The table')
            RETURN (1)
        END

    EXECUTE sp_namecrack @tabname,
                         @site OUTPUT,
                         @db OUTPUT,
                         @owner OUTPUT,
                         @object OUTPUT

    EXECUTE @retcode = sp_validname @object

    IF @@ERROR <> 0 OR @retcode <> 0
	return(1)

    IF @owner IS NOT NULL
        BEGIN

            EXECUTE @retcode = sp_validname @owner

            IF @@ERROR <> 0 OR @retcode <> 0
		RETURN (1)
        END

    IF @db IS NOT NULL
        BEGIN

            EXECUTE @retcode = sp_validname @db

            IF @@ERROR <> 0 OR @retcode <> 0
		RETURN (1)

            IF @db <> DB_NAME()
                BEGIN
                    RAISERROR (14004, 16, -1, @object)
                    RETURN (1)
                END

        END

    SELECT @id = OBJECT_ID (@tabname)
    IF @id IS NULL
        BEGIN
            RAISERROR (15001, 11, -1, @tabname)
            RETURN (1)
        END

    IF NOT EXISTS (SELECT * FROM sysobjects WHERE id = @id AND type = 'U')
        BEGIN
            RAISERROR (14028, 11, -1)
            RETURN (1)
        END

    /*
** Parameter Check: @replicated.
    ** Check to make sure that the value is {true | false}.
    */

    IF LOWER(@replicated) NOT IN ('true', 'false')
        BEGIN
            RAISERROR (14081, 16, -1)
            RETURN (1)
        END

    /*
    ** Set the category bit on or off depending upon @replicated.
    */

    IF LOWER(@replicated) IN ('true')	/* Turn on bit */
	UPDATE sysobjects
           SET category = category | @replica_bit
         WHERE id = @id
           AND type = 'U'
    ELSE				/* Turn off bit */
 	UPDATE sysobjects
           SET category = category & ~@replica_bit
         WHERE id = @id
           AND type = 'U'

    IF @@ERROR <> 0
        BEGIN
            RAISERROR (14083, 16, -1)
            RETURN (1)
        END

    IF LOWER(@replicated) IN ('true')
        PRINT 'The object was successfully marked as a replicated object.'
    ELSE
        PRINT 'The object was successfully unmarked as a replicated object.'

    RETURN (0)
go

print ''
print 'Creating procedure sp_articlefilter.'
go
create procedure sp_articlefilter
	@publication varchar(30),   		/* publication name */
	@article varchar(30), 			/* article name */
	@filter_name varchar (92) = NULL, 	/* name of filter procedure*/
	@filter_clause text = ''       		/* article's filter clause */
		
as
	
	declare @pubid smallint
	declare @table_name varchar (30)
	declare @user_name varchar (30)
	declare @qualified_table_name varchar (61)
	declare @filter_id int
	declare @type tinyint		
	declare @previous_proc varchar (30)
	declare @retcode int
	declare @site varchar(30)
	declare @db varchar(30)
	declare @owner varchar(30)
	declare @object varchar(30)
	declare @artid int
	declare @inactive tinyint


	select @inactive = 0

	/*
	** Security Check.
	** Only the System Administrator (SA) or the Database Owner (dbo) can
	** add an article view.
	*/
	if suser_id() <> 1 and user_id() <> 1
	   begin
              RAISERROR (15000, 14, -1)
	      return (1)
           end

	/*
	** Parameter Check:  @publication.
	** Make sure that the publication exists and that it conforms to the
	** rules for identifiers.
	*/
	if @publication is null
           begin
              RAISERROR (14043, 16, -1, 'The publication')
              return (1)
           END

	execute @retcode = sp_validname @publication
	if @retcode <> 0
            RETURN (1)

	select @pubid = pubid from syspublications where name = @publication

        if @pubid is null
           begin
            RAISERROR (15001, 11, -1, @publication)
    	    return (1)
           end

	/*
	** Parameter Check:  @article.
	** Check to make sure that the article exists in the publication.
	*/

	if @article is null
	   begin
              RAISERROR (14043, 16, -1, 'The article')
              return (1)
           end

        execute @retcode = sp_validname @article
        if @retcode <> 0
	   return (1)

	/*
	** Get the article information.
	*/
	select @artid = art.artid, @table_name = so.name, @type = art.type,
	   @filter_id = art.filter, @user_name = USER_NAME(so.uid)
	   from sysarticles art, sysobjects so
	   where art.pubid = @pubid
	   and art.name = @article
	   and art.objid = so.id

	/*
	** Fail if there is no article information.
	*/
	if @artid is null
	   begin
              RAISERROR (15001, 11, -1, @article)
    	      return (1)
	   end

        /*
        ** Only unsubscribed articles may be modified.
        */
        if exists (select * from syssubscriptions where artid = @artid
          and status <> @inactive)
          begin
	     RAISERROR (14092, 11, -1)
             RETURN (1)
          end

	/*
	** Make sure a valid @filter_name was provided and it is
	** a valid name.
	*/
	if datalength(@filter_clause) > 1
	   begin
     		/*
		** Make sure a valid @filter_name was provided and it is
		** a valid name.
		*/
		if @filter_name is null
		   begin
		      RAISERROR (14043, 16, -1, 'The filter_name')
		      return (1)
		   end

		execute sp_namecrack @filter_name,
		   @site OUTPUT,
		   @db OUTPUT,
		   @owner OUTPUT,
		   @object OUTPUT

		execute @retcode = sp_validname @object
		if @retcode <> 0
		   return (1)
	   end

	/*
	** If the article has a generated filter (not manually created), then
	** drop the current filter before creating the new one.
	*/
	if ((@type & 0x3) <> 0x3) and @filter_id <> 0
	   begin
 	      select @previous_proc = object_name (@filter_id)
	      if @previous_proc is not null and
	         exists (select * from sysobjects where name = @previous_proc
	            and type = 'RF')
		 begin
	            exec ('drop procedure ' + @previous_proc)
		    if @@error <> 0
		       return (1)
		 end

	   end

	/*
	** make an owner qualified table name for these operations name
	*/

	select @qualified_table_name = @user_name + '.' + @table_name

	/*
	** If there is a @filter_clause, create the new filter and
	** update the article filter id and filter_clause.
	**/
	if datalength(@filter_clause) > 1
	   begin
		exec ('create procedure ' + @object +
			' for replication as ' +
			'if exists (select * from ' + @qualified_table_name +
			' where ' + @filter_clause +
			') return 1 else return 0')
		if @@error <> 0
		   return (1)

		select @filter_id = id  from sysobjects where name = @object
			and type = 'RF'
		if @filter_id is null or @filter_id = 0
		   begin
		      RAISERROR (15001, 11, -1, @object)
		      return (1)
		   end

		/*
		** Update article
		*/
		update sysarticles set filter = @filter_id,
		   filter_clause = @filter_clause
		   where pubid = @pubid
		      and name = @article
	   end
	else
		/*
		** Clear the filter id and filter_clause.
		*/
		update sysarticles set filter = 0,
		   filter_clause = NULL
		   where pubid = @pubid
		      and name = @article

        /*
        ** Force the article cache to be refreshed with the new definition.
        */
        EXECUTE sp_replflush
go

print ''
print 'Creating procedure sp_articletextcol.'
go
CREATE PROCEDURE sp_articletextcol (
    @artid int,
    @colid tinyint = NULL,
    @type varchar(10),      /* 'publish', 'nonsqlsub' */
    @operation varchar(5))  /* 'add', 'drop' */
    AS

    /*
    ** Declarations.
    */
    DECLARE @cmd char(255)
    DECLARE @cmd1 char(255)
    DECLARE @columns binary(32)	/* Temporary storage for the converted column */
    DECLARE @tabid int		/* Article base table id */
    DECLARE @retcode int
    DECLARE @status bit
    DECLARE @image tinyint		/* Constant: 0x22 */
    DECLARE @text tinyint		/* Constant: 0x23 */
    DECLARE @publish tinyint		/* Constant: 0x10 */
    DECLARE @nonsqlsub tinyint		/* Constant: 0x20 */

    /* Constants */
    SELECT @image = 0x22
    SELECT @text = 0x23
    SELECT @publish = 0x10
    SELECT @nonsqlsub = 0x20

    /*
    ** Security Check
    ** Only the System Administratr (SA) or the Database Owner (dbo)
    ** can execute this procedure.
    */
    IF suser_id() <> 1 AND user_id() <> 1
        BEGIN
            RAISERROR (14093, 14, -1)
    	    RETURN (1)
        END

    SELECT @tabid = objid FROM sysarticles WHERE artid = @artid

    SELECT @cmd = @cmd + 'DECLARE hCarttextcol CURSOR FOR'

    IF @colid IS NULL
       BEGIN
       SELECT @cmd = @cmd + ' SELECT colid FROM syscolumns, sysarticles'
       SELECT @cmd = @cmd + ' WHERE artid = ' + CONVERT(varchar(10), @artid)
       SELECT @cmd = @cmd + ' AND id = ' + CONVERT(varchar(10), @tabid)
       SELECT @cmd = @cmd + ' AND (syscolumns.type = 0x22 OR syscolumns.type = 0x23)'
       SELECT @cmd1 = @cmd1 + ' AND CONVERT(bit, SUBSTRING(columns,'
       SELECT @cmd1 = @cmd1 + ' CONVERT(tinyint, 32 - FLOOR((colid-1)/8)),'
       SELECT @cmd1 = @cmd1 + ' 1) & POWER(2, ((colid-1)%8))) = 1'
       EXECUTE (@cmd + @cmd1)
       END
    ELSE
       BEGIN
       SELECT @cmd = @cmd + ' SELECT colid FROM syscolumns WHERE id ='
       SELECT @cmd = @cmd + ' ' + CONVERT(varchar(10), @tabid)
       SELECT @cmd = @cmd + ' AND colid =' + CONVERT(varchar(10), @colid)
       SELECT @cmd = @cmd + ' AND (type = 0x22 OR type = 0x23)'
       EXECUTE (@cmd)
       END


    /* Process each Text\Image column in the article */
    OPEN hCarttextcol
    FETCH hCarttextcol INTO @colid
    WHILE (@@fetch_status <> -1)
    BEGIN

       IF LOWER(@operation) = 'add'
          BEGIN
          IF LOWER(@type) = 'publish'
             UPDATE syscolumns
                SET status = status | @publish
	        WHERE id = @tabid
      	        AND colid = @colid
	  ELSE
	     UPDATE syscolumns
                SET status = status | @nonsqlsub
	        WHERE id = @tabid
      	        AND colid = @colid
	  END
       ELSE /* drop */
          BEGIN
	  /*
	  ** Is there another non-sql server subscription on the column?
	  ** Or another article publishing the column?
	  */
	  EXEC @retcode = sp_textcolstatus @artid, @tabid, @colid,
	     @type, @status OUTPUT

	  IF @@error <> 0 OR @retcode <> 0
	     BEGIN
	        CLOSE hCarttextcol
		DEALLOCATE hCarttextcol
		RETURN (1)
	     END

	  IF (@status = 0)
	     BEGIN
	     IF LOWER(@type) = 'publish'
	        /* Clear 'publish' bit */
                UPDATE syscolumns
                   SET status = status & ~@publish
	           WHERE id = @tabid
      	           AND colid = @colid
	      ELSE
	        /* Clear 'non-sql server subscription' bit */
                UPDATE syscolumns
                   SET status = status & ~@nonsqlsub
	           WHERE id = @tabid
      	           AND colid = @colid
             END
          END
       FETCH hCarttextcol INTO @colid
    END

    CLOSE hCarttextcol
    DEALLOCATE hCarttextcol

GO

print ''
print 'Creating procedure sp_textcolstatus.'
go
CREATE PROCEDURE sp_textcolstatus (
    @artid int,
    @tabid int,
    @colid int,
    @type varchar (10),  	/* 'publish', 'nonsqlsub' */
    @status bit OUTPUT)
    AS

    /*
    ** Declarations.
    */
    DECLARE @cmd char(255)
    DECLARE @artid2 int
    DECLARE @columns binary(32)
    DECLARE @replicate tinyint		/* Constant: 0x10 */
    DECLARE @nonsqlsub tinyint		/* Constant: 0x20 */
    DECLARE @image tinyint		/* Constant: 0x22 */
    DECLARE @text tinyint		/* Constant: 0x23 */

    /* Constants */
    SELECT @image = 0x22
    SELECT @text = 0x23

    SELECT @status = 0

    /*
    ** Security Check
    ** Only the System Administratr (SA) or the Database Owner (dbo)
    ** can execute this procedure.
    */
    IF suser_id() <> 1 AND user_id() <> 1
        BEGIN
            RAISERROR (14093, 14, -1)
    	    RETURN (1)
        END

    IF LOWER(@type) = 'nonsqlsub'
    BEGIN
    /*
    ** Check all active or subscribed articles for the TEXT/IMAGE column.
    */	
    SELECT @cmd = @cmd + 'DECLARE hC4 CURSOR FOR '
    SELECT @cmd = @cmd + ' SELECT sub.artid FROM sysarticles art, syssubscriptions sub'
    SELECT @cmd = @cmd + '  WHERE art.objid = ' + CONVERT(varchar(10), @tabid)
    SELECT @cmd = @cmd + '  AND art.artid <> ' + CONVERT(varchar(10), @artid)
    SELECT @cmd = @cmd + '  AND sub.artid = art.artid'
    SELECT @cmd = @cmd + '  AND sub.status = 1 OR sub.status = 2'
    END
    ELSE
    BEGIN
    /*
    ** Check all articles for the TEXT/IMAGE column.
    */	
    SELECT @cmd = @cmd + 'DECLARE hC4 CURSOR FOR '
    SELECT @cmd = @cmd + ' SELECT artid FROM sysarticles '
    SELECT @cmd = @cmd + '  WHERE objid = ' + CONVERT(varchar(10), @tabid)
    SELECT @cmd = @cmd + '  AND artid <> ' + CONVERT(varchar(10), @artid)
    END

    EXECUTE (@cmd)
    OPEN hC4
    FETCH hC4 INTO @artid2
    WHILE (@@fetch_status <> -1)
    BEGIN
       SELECT @columns = columns FROM sysarticles WHERE artid = @artid2

       IF EXISTS (SELECT * FROM syscolumns
          WHERE id = @tabid
          AND colid = @colid
	  AND CONVERT(bit, SUBSTRING(@columns, CONVERT(tinyint,
	     32 - FLOOR((colid-1)/8)), 1) & POWER(2, ((colid-1)%8))) = 1
	  AND (type = @image OR type = @text))
          BEGIN
	     SELECT @status = 1	
	     GOTO CLEANUP
          END

       FETCH hC4 INTO @artid2
    END

CLEANUP:
    CLOSE hC4
    DEALLOCATE hC4

    RETURN (0)
GO

print ''
print 'Creating procedure sp_articleview.'
go
create procedure sp_articleview
	@publication varchar(30),        /* Publication name */
	@article varchar(30),     	 /* Article name */
	@view_name varchar (92) = NULL,  /* View name */
	@filter_clause text = ''       	 /* Article's filter clause */
		
as
	declare @pubid smallint
	declare @table_name varchar (30)
	declare @user_id int
	declare @user_name varchar (30)
	declare @qualified_table_name varchar (61)
	declare @columns varbinary (32)
	declare @name varchar (30)
	declare @col_clause1 varchar (255)
	declare @col_clause2 varchar (255)
	declare @retcode int
	declare @view_id int
	declare @type tinyint
	declare @table_id int
	declare @previous_view varchar (30)
	declare @colid int
	declare @site varchar(30)
	declare @db varchar(30)
	declare @owner varchar(30)
	declare @object varchar(30)
	declare @artid int
	declare @inactive tinyint


	select @inactive = 0

	/*
	** Security Check.
	** Only the System Administrator (SA) or the Database Owner (dbo) can
	** add an article view.
	*/
	if suser_id() <> 1 and user_id() <> 1
	   begin
              RAISERROR (15000, 14, -1)
	      return (1)
           end

	/*
	** Parameter Check:  @publication.
	** Make sure that the publication exists and that it conforms to the
	** rules for identifiers.
	*/
	if @publication is null
           begin
              RAISERROR (14043, 16, -1, 'The publication')
              return (1)
           END

	execute @retcode = sp_validname @publication
	if @retcode <> 0
            RETURN (1)

	select @pubid = pubid from syspublications where name = @publication
        if @pubid is null
           begin
            RAISERROR (15001, 11, -1, @publication)
    	    return (1)
           end

	/*
	** Parameter Check:  @article.
	** Check to make sure that the article exists in the publication.
	*/

	if @article is null
	   begin
              RAISERROR (14043, 16, -1, 'The article')
              return (1)
           end

        execute @retcode = sp_validname @article
        if @retcode <> 0
	   return (1)

	/*
	** Get the article information.
	*/
	select @artid = art.artid, @table_name = so.name,
	   @user_id = uid, @user_name = USER_NAME(so.uid),
	   @columns = art.columns, @type = art.type,
	   @view_id = art.sync_objid, @table_id = art.objid
	   from sysarticles art, sysobjects so
	   where art.pubid = @pubid
	   and art.name = @article
	   and art.objid = so.id

	/*
	** Fail if there is no article information.
	*/
	if @artid is null
	   begin
              RAISERROR (15001, 11, -1, @article)
    	      return (1)
	   end

        /*
        ** Only unsubscribed articles may be modified.
        */
        if exists (select * from syssubscriptions where artid = @artid
          and status <> @inactive)
          begin
	     RAISERROR (14092, 11, -1)
             RETURN (1)
          end

	
	/*
	** Create a table of all the articles columns.
	*/
	create table #tmp (colid int, name varchar(30), published bit)
	if @@error <> 0
	   return (1)

	create unique index ind1 on #tmp (colid)
	if @@error <> 0
	   begin
		drop table #tmp
		return (1)
	   end

	insert into #tmp select colid, name,
		convert(bit, substring(@columns, convert(tinyint,
			32 - floor((colid-1)/8)), 1) & POWER(2, ((colid-1)%8)))
		from syscolumns
		where id = (select id from sysobjects where name = @table_name and
		   uid = @user_id and type = 'U')

	/* Break out the specified view name and get the non-ownerqual'd name, then validate that. */
	execute sp_namecrack @view_name, @site OUTPUT, @db OUTPUT, @owner OUTPUT, @object OUTPUT
	execute @retcode = sp_validname @object
	if @retcode <> 0
		return (1)

	/* If no non-published columns, we'll select all and avoid the 510-byte limit on column strings. */
	if not exists (select * from #tmp where published = 0) begin
		select @col_clause1 = null
		select @col_clause2 = null
		goto CreateView
	end

	/*
	** Construct the column list based on all published columns in the
	** article.
	*/
	execute ('declare hC scroll cursor for select colid, name from #tmp
		where published = 1')
	open hC
	fetch hC into @colid, @name
	while (@@fetch_status <> -1)
        begin

	    if @col_clause1 is null or
		((datalength(@name) + datalength(@col_clause1) + 2) < 255)
		if @col_clause1 is null
			select @col_clause1 = @name
		else
			select @col_clause1 = @col_clause1 + ', ' + @name
		
	    else if @col_clause2 is null or
		((datalength(@name) + datalength(@col_clause2) + 2) < 255)
		begin
			if @col_clause2 is null
				select @col_clause2 = ','+ @name
			else
				select @col_clause2 = @col_clause2 + ', ' +
					@name
		end
	    else
    		/*
		** The procedure only support ~510 bytes for the column list
		*/
		begin
			RAISERROR (14039, 16, -1)
			close hC
			deallocate hC
			drop table #tmp
			return (1)
		end
	    fetch hC into @colid, @name
        end			

	close hC
	deallocate hC

CreateView:
	/*
	** If the article has a generated view (not manually created), then
	** drop the current view before creating the new one.
	*/
	if ((@type & 0x5) <> 0x5) and @view_id <> 0
	   and @view_id <> @table_id
	   begin
 	      select @previous_view = object_name (@view_id)
	      if @previous_view is not null and
                 exists (select * from sysobjects where name = @previous_view
	            and type = 'V')
	         exec ('drop view ' + @previous_view)
	   end

	/*
	** If a view is going to be created. Make sure a valid @view_name
	** was provided.
	*/
	if @col_clause1 is not null or @col_clause2 is not null
	   begin
	      if @view_name is null
	         begin
	            RAISERROR (14043, 16, -1, 'The view_name')
	            return (1)
	         end
	    end

	/*
	** make an owner qualified table name for these operations name
	*/

	select @qualified_table_name = @user_name + '.' + @table_name

	/*
	** Construct and execute the view creation command.
	*/
	if @col_clause2 is not null
	   begin
		if datalength(@filter_clause) > 1
			exec ('create view ' + @object + ' as select ' +
				@col_clause1 + @col_clause2 + ' from ' +
				@qualified_table_name + ' where ' + @filter_clause)
		else
			exec ('create view ' + @object + ' as select ' +
				@col_clause1 + @col_clause2 + ' from ' +
				@qualified_table_name)
		if @@error <> 0
		   return (1)
	   end
	else if @col_clause1 is not null
	  begin

		if datalength(@filter_clause) > 1
			exec ('create view ' + @object + ' as select ' +
				@col_clause1 + ' from ' +  @qualified_table_name +
				' where ' + @filter_clause)
		else
			exec ('create view ' + @object + ' as select ' +
				@col_clause1 + ' from ' +  @qualified_table_name)
		if @@error <> 0
		   return (1)
	   end
	else
		begin
			if datalength(@filter_clause) > 1
				exec ('create view ' + @object + ' as select * from ' +
					@qualified_table_name + ' where ' + @filter_clause)
			if @@error <> 0
			   return (1)

		end
	/*
	** Update the article's sync_objid with the new view or the base
	** table id.
	*/
	if @col_clause1 is null and datalength(@filter_clause) = 1
	   select @view_id = object_id(@qualified_table_name)
	else
	   begin
	      select @view_id = id from sysobjects where name = @object and
	         type = 'V'
	      if @view_id is null or @view_id = 0
	         begin
	            RAISERROR (15001, 11, -1, @object)
	            return (1)	
	         end
	   end

	/* Update article definition */
	update sysarticles set sync_objid = @view_id where
	   pubid = @pubid and
	   name = @article

	/*
	** Set new sync_objid and @filter_clause value
	*/
	if datalength(@filter_clause) > 1
	   update sysarticles set sync_objid = @view_id,
	      filter_clause = @filter_clause
	      where pubid = @pubid
	      and name = @article
	else
	   update sysarticles set sync_objid = @view_id,
	      filter_clause = NULL
	      where pubid = @pubid
	      and name = @article

       drop table #tmp

       /*
       ** Force the article cache to be refreshed with the new definition.
       */
       EXECUTE sp_replflush
go

dump tran master with no_log
go

print ''
print 'Creating procedure sp_addarticle.'
go
CREATE PROCEDURE sp_addarticle
    @publication varchar(30),               /* publication name */
    @article varchar(30), 		    /* article name */
    @source_table varchar (92), 	    /* table name */
    @destination_table varchar (30) = NULL, /* destination table name */
    @vertical_partition char(5) = 'false',  /* vertical partition */
    @type varchar (30) = 'logbased',	    /* article type */
    @filter varchar (92) = NULL,	    /* stored procedure used to filter table */
    @sync_object varchar (92) = NULL,	    /* view or table used for synchronization */
    @ins_cmd varchar (255) = 'SQL',	    /* insert format string */
    @del_cmd varchar (255) = 'SQL',	    /* delete format string */
    @upd_cmd varchar (255) = 'SQL',	    /* update format string */
    @creation_script varchar (127) = NULL,  /* article schema script */
    @description varchar (255) = NULL,	    /* article description */
    @pre_creation_cmd varchar(10) = 'drop', /* 'none', 'drop', 'delete', 'truncate' */
    @filter_clause text	= ''		    /* where clause */
    AS

    SET NOCOUNT ON

    /*
    ** Declarations.
    */

    DECLARE @accessid smallint
    DECLARE @db varchar(30)
    DECLARE @filterid int
    DECLARE @object varchar(30)
    DECLARE @owner varchar(30)
    DECLARE @pubid int
    DECLARE @publish_bit smallint
    DECLARE @retcode int
    DECLARE @site varchar(30)
    DECLARE @syncid int
    DECLARE @tabid int
    DECLARE @typeid smallint
    DECLARE @pkkey varchar (30)
    DECLARE @i int
    DECLARE @indid int
    DECLARE @precmdid int

    SELECT @publish_bit = 32

    /*
    ** Security Check.
    ** Only the System Administrator (SA) or the Database Owner (dbo) can
    ** add an article to a publication.
    */
    IF suser_id() <> 1 AND user_id() <> 1
	BEGIN
            RAISERROR (15000, 14, -1)
	    RETURN (1)
        END

    /*
    ** Parameter Check: @article.
    ** The @article name cannot be NULL and must conform to the rules
    ** for identifiers.
    */

    IF @article IS NULL
        BEGIN
            RAISERROR (14043, 16, -1, 'The article')
            RETURN (1)
        END

    EXECUTE @retcode = sp_validname @article

    IF @retcode <> 0
	return(1)

    if LOWER(@article) = 'all'
        BEGIN
            RAISERROR (14032, 16, -1, @article)
            RETURN (1)
        END

    /*
    ** Parameter Check: @publication.
    ** The @publication name cannot be NULL and must conform to the rules
    ** for identifiers.
    */

    IF @publication IS NULL
        BEGIN
            RAISERROR (14043, 16, -1, 'The publication')
            RETURN (1)
        END

    EXECUTE @retcode = sp_validname @publication

    IF @retcode <> 0
	RETURN (1)

    /*
    ** Parameter Check: @source_table.
    ** Check to see that the @source_table is local, that it conforms
    ** to the rules for identifiers, and that it is a table, and not
    ** a view or another database object.
    */

    IF @source_table IS NULL
        BEGIN
            RAISERROR (14043, 16, -1, 'The source table')
            RETURN (1)
        END

    EXECUTE sp_namecrack @source_table,
                         @site OUTPUT,
                         @db OUTPUT,
                         @owner OUTPUT,
                         @object OUTPUT

    IF @source_table LIKE '%.%.%' AND @db <> DB_NAME()
       BEGIN
          RAISERROR (14004, 16, -1, 'The source table')
	  RETURN (1)
       END

    EXECUTE @retcode = sp_validname @object

    IF @retcode <> 0
	RETURN (1)

    /*
    **  Get the id of the @source_table
    */

    SELECT @tabid = id FROM sysobjects WHERE id = OBJECT_ID(@source_table)

    IF @tabid IS NULL
        BEGIN
            RAISERROR (14027, 11, -1, 'The source table')
    	    RETURN (1)
        END

    /*
    ** Make sure that the table name specified is a table and not a view.
    */

    IF NOT EXISTS (SELECT *
                     FROM sysobjects
		    WHERE id = (SELECT OBJECT_ID(@source_table))
		      AND type = 'U')
	BEGIN
            RAISERROR (14028, 16, -1)
            RETURN (1)
        END

    /*
    ** Parameter Check:  @destination_table.
    ** If the destination table is not specified, assume it's the same
    ** as the source table.  Make sure that the table name is not qualified.
    */

    IF @destination_table LIKE '%.%.%'
	BEGIN
            RAISERROR (14001, 16, -1)
	    RETURN (1)
	END

    IF @destination_table LIKE '%.%'
	BEGIN
            RAISERROR (14044, 16, -1, 'destination table')
	    RETURN (1)
	END

    IF @destination_table IS NULL
        SELECT @destination_table = @source_table

    EXECUTE sp_namecrack @destination_table,
                         @site OUTPUT,
                         @db OUTPUT,
                         @owner OUTPUT,
                         @object OUTPUT

    EXECUTE @retcode = sp_validname @object

    IF @retcode <> 0
	RETURN (1)

    /*
    ** Parameter Check: @vertical_partition
    ** Check to make sure that the vertical partition is either TRUE or FALSE.
    */

    SELECT @vertical_partition = LOWER(@vertical_partition)
    IF @vertical_partition NOT IN ('true', 'false')
        BEGIN
            RAISERROR (14029, 16, -1)
            RETURN (1)
        END

    /*
    ** Parameter Check: @filter
    ** Make sure that the filter is a valid stored procedure.
    */
    IF @filter IS NOT NULL
        BEGIN

            EXECUTE sp_namecrack @filter,
                                 @site OUTPUT,
                                 @db OUTPUT,
                                 @owner OUTPUT,
                                 @object OUTPUT

	    IF @filter LIKE '%.%.%' AND @db <> DB_NAME()
               BEGIN
                  RAISERROR (14004, 16, -1, 'The filter')
		  RETURN (1)
	       END


            EXECUTE @retcode = sp_validname @object

            IF @retcode <> 0
		RETURN (1)

	    /*
	    ** Get the id of the @filter
	    */
            select @filterid = id from sysobjects where
		id = OBJECT_ID(@filter) and type = 'RF'
            IF @filterid IS NULL
	        BEGIN
                    RAISERROR (14027, 11, -1, 'The filter')
                    RETURN (1)
                END
        END
    ELSE
        select @filterid = 0

    /*
    ** Get the pubid.
    */

    SELECT @pubid = pubid FROM syspublications WHERE name = @publication

    IF @pubid IS NULL
        BEGIN
            RAISERROR (14027, 11, -1, 'The publication')
            RETURN (1)
        END

    /*
    ** Parameter Check:  @article, @publication.
    ** Check if the article already exists in this publication.
    */

    IF EXISTS (SELECT *
                 FROM sysarticles
                WHERE pubid = @pubid
                  AND name = @article)
        BEGIN
            RAISERROR (14030, 16, -1, @article, @publication)
    	    RETURN (1)
        END

    /*
    ** Set the typeid.  The default type is logbased.  Anything else is
    ** currently undefined (reserved for future use).
    **
    **      @typeid     type
    **      =======     ========
    **            1     logbased
    **		  3     logbased manualfilter
    **		  5     logbased manualview
    **		  7     logbased manualboth
    */

    IF LOWER(@type) NOT IN ('logbased', 'logbased manualfilter', 'logbased manualview', 'logbased manualboth')
       BEGIN
            RAISERROR (14023, 16, -1)
            RETURN (1)
       END

    IF LOWER(@type) = 'logbased'
       SELECT @typeid = 1
    ELSE IF LOWER(@type) = 'logbased manualfilter'
       SELECT @typeid = 3
    ELSE IF LOWER(@type) = 'logbased manualview'
       SELECT @typeid = 5
    ELSE IF LOWER(@type) = 'logbased manualboth'
       SELECT @typeid = 7

    /*
    ** Set the precmdid.  The default type is 'drop'.
    **
    **      @precmdid   pre_creation_cmd
    **      =========   ================
    **            0     none
    **		  1     drop
    **		  2     delete
    **		  3     truncate
    */
    IF LOWER(@pre_creation_cmd) NOT IN ('none', 'drop', 'delete', 'truncate')
       BEGIN
          RAISERROR (14061, 16, -1)
          RETURN (1)
       END

    /*
    ** Determine the integer value for the pre_creation_cmd.
    */
    IF LOWER(@pre_creation_cmd) = 'none'
       SELECT @precmdid = 0
    ELSE IF LOWER(@pre_creation_cmd) = 'drop'
       SELECT @precmdid = 1
    ELSE IF LOWER(@pre_creation_cmd) = 'delete'
       SELECT @precmdid = 2
    ELSE IF LOWER(@pre_creation_cmd) = 'truncate'
       SELECT @precmdid = 3

    IF @sync_object IS NULL
    	select @syncid = @tabid
    ELSE
    	BEGIN

	    /*
            ** Parameter Check: @sync_object.
	    ** Check to see that the sync_object is local and that it
            ** conforms to the rules for identifiers.
	    */

            EXECUTE sp_namecrack @sync_object,
                                 @site OUTPUT,
                                 @db OUTPUT,
                                 @owner OUTPUT,
                                 @object OUTPUT

	    IF @sync_object LIKE '%.%.%' AND @db <> DB_NAME()
               BEGIN
                  RAISERROR (14004, 16, -1, 'The synchronization object')
		  RETURN (1)
	       END


            EXECUTE @retcode = sp_validname @object

            IF @retcode <> 0
		RETURN (1)

	    /*
	    **  Get the id of the @sync_object
	    */

	    SELECT @syncid = id FROM sysobjects WHERE id = OBJECT_ID(@sync_object)

	    IF @syncid IS NULL
		BEGIN
                    RAISERROR (14027, 11, -1, 'The synchronization object')
		    RETURN (1)
		END

	    /*
	    ** Make sure the sync object specified is a table or a view.
	    */

	    IF NOT EXISTS (SELECT * FROM sysobjects
					WHERE id = (SELECT OBJECT_ID(@sync_object))
					AND (type = 'U' or
					     type = 'V'))
		BEGIN
                    RAISERROR (14031, 16, -1)
		    RETURN (1)
		END
	END

    /*
    ** Make sure there is a primary key on the source table.
    */
    IF NOT EXISTS (SELECT * FROM sysconstraints WHERE id = @tabid and
    	(status & 0x1) <> 0) 	/* PK status */
       BEGIN
          RAISERROR (14088, 16, -1, @source_table)
	  RETURN (1)
       END

    /*
    **  Add article to sysarticles and update sysobjects category bit.
    */
    BEGIN TRAN sp_addarticle
    	INSERT sysarticles (columns, creation_script, del_cmd, description,
                            dest_table, filter, filter_clause, ins_cmd, name,
			    objid, pre_creation_cmd, pubid,
                            status, sync_objid, type, upd_cmd)
        VALUES (0, @creation_script, @del_cmd, @description, @destination_table,
                @filterid, @filter_clause, @ins_cmd, @article, @tabid,
		@precmdid, @pubid, 0, @syncid, @typeid, @upd_cmd)

    	IF @@ERROR <> 0
            BEGIN
    		ROLLBACK TRAN sp_addarticle
    		RETURN (1)
    	    END

    	UPDATE sysobjects
           SET category = category | @publish_bit
         WHERE id = (SELECT objid
                       FROM sysarticles
                      WHERE name = @article
                        AND pubid = @pubid)

    	IF @@ERROR <> 0
    	    BEGIN
    		ROLLBACK TRAN sp_addarticle
		RETURN (1)
	    END

        /*
        ** Set all bits to '1' in the columns column to include all columns.
        */

        IF @vertical_partition = 'false'
	   BEGIN
	      EXECUTE @retcode  = sp_articlecolumn @publication, @article
	      IF @@ERROR <> 0 OR @retcode <> 0
	         BEGIN
	 	    ROLLBACK TRAN sp_addarticle
		    RETURN (1)
	         END
	   END
        /*
	** Set all bits to '1' for all columns in the primary key.
	*/
        ELSE
	   BEGIN
		SELECT @indid = indid FROM sysindexes
			WHERE id = @tabid
			AND (status & 2048) <> 0	/* PK index */
		/*
		**  First we'll figure out what the keys are.
		*/
		SELECT @i = 1

		WHILE (@i <= 16)
		   BEGIN
		      SELECT @pkkey = INDEX_COL(@source_table, @indid, @i)
		      if @pkkey is NULL
		         goto DONE

		      EXECUTE @retcode  = sp_articlecolumn @publication,
				@article, @pkkey, 'add'
		      IF @@ERROR <> 0 OR @retcode <> 0
		         BEGIN
			    ROLLBACK TRAN sp_addarticle
			    RETURN (1)
			 END

			select @i = @i + 1
		   END
	   END

DONE:
    COMMIT TRAN sp_addarticle
go

print ''
print 'Creating procedure sp_addpublisher.'
go

CREATE PROCEDURE sp_addpublisher (
	@publisher varchar (30),	  /* publisher server name */
	@type varchar (5) = NULL	  /* NULL or 'dist' */
        ) AS

    /*
    ** Declarations.
    */

    DECLARE @distaccount varchar(255)
    DECLARE @proc varchar (255)
    DECLARE @retcode int
    DECLARE @privilege varchar (30)

    /*
    ** Parameter Check:  @publisher.
    ** Check to make sure that the publisher is not NULL and that it
    ** conforms to the rules for identifiers.
    */

    IF @publisher IS NULL
        BEGIN
            RAISERROR (14043, 16, -1, 'The publisher')
            RETURN (1)
        END

    EXECUTE @retcode = sp_validname @publisher

    IF @@ERROR <> 0 OR @retcode <> 0
	RETURN (1)


    /*
    ** Perform special logic if defining a publisher for a distribution
    ** server.
    */
    IF LOWER(@type) = 'dist'
	BEGIN
		/* Check if publisher is already defined. */
		IF EXISTS (SELECT *
			 FROM master..sysservers
			WHERE srvname = @publisher
			AND srvstatus &  16 <> 0)

		BEGIN
			RAISERROR (14074, 16, -1, @publisher)
			RETURN (1)
		END

		IF NOT EXISTS (SELECT *
			     FROM master..sysservers
			    WHERE srvname = @publisher)

		/* Add the server if it does not exist. */
		BEGIN
		    EXECUTE @retcode = sp_addserver @publisher
			IF @@error <> 0 OR @retcode <> 0
			BEGIN
			    RAISERROR (14075, 16, -1)
			    RETURN (1)
			END
		END

		/*
		** Set the server option to indicate that this is a
		** distribution publisher.
		*/
		EXECUTE @retcode = sp_serveroption @publisher, 'dpub', true
		IF @@error <> 0 OR @retcode <> 0
		BEGIN
			RAISERROR (14075, 16, -1)
			RETURN (1)
		END

		/* Add remotelogin enabling the 'sa' of the publisher to
		** RPC for distribution information.
		*/
		IF EXISTS (SELECT *
                  FROM sysremotelogins srl,
	               sysservers ss
                 WHERE ss.srvname = @publisher
	           AND srl.remoteserverid = ss.srvid
	           AND srl.remoteusername = 'sa'
		   AND suid = 16383) 	/* 'repl_subscriber' */
	        BEGIN
		   EXECUTE @retcode = sp_dropremotelogin @publisher, repl_subscriber, sa
		   IF @@error <> 0 OR @retcode <> 0
		   BEGIN
			RAISERROR (14075, 16, -1)
			RETURN (1)
		   END
		END

		EXECUTE @retcode = sp_addremotelogin @publisher, sa, sa
		IF @@error <> 0 OR @retcode <> 0
		   BEGIN
		      RAISERROR (14075, 16, -1)
		      RETURN (1)
		   END

		EXECUTE @retcode = sp_remoteoption @publisher, sa, sa, trusted, true
		IF @@error <> 0 OR @retcode <> 0
	           BEGIN
		      RAISERROR (14075, 16, -1)
		      RETURN (1)
		   END

        	/* Add remotelogin enabling the 'probe' of the publisher to
		** RPC for distribution counter information.
		*/
		IF NOT EXISTS (SELECT *
                  FROM sysremotelogins srl,
	               sysservers ss
                 WHERE ss.srvname = @publisher
	           AND srl.remoteserverid = ss.srvid
	           AND srl.remoteusername = 'probe'
		   AND srl.suid = 10)	/* 'probe' */
		BEGIN
		   EXECUTE @retcode = sp_addremotelogin @publisher, probe, probe
		   IF @@error <> 0 OR @retcode <> 0
		   BEGIN
			RAISERROR (14075, 16, -1)
			RETURN (1)
		   END
		END

   	        RETURN (0)
	END

    /*
    ** Check to make sure that the publisher doesn't already exist.
    */
    IF EXISTS (SELECT * FROM master..sysservers
              WHERE srvname = @publisher
                 AND srvstatus & 2 <> 0)
	BEGIN
		RAISERROR (14074, 16, -1, @publisher)
		RETURN (1)
	END

    /*
    ** The server may already be listed in master..sysservers, but might
    ** not be marked as a publisher yet.  If it's not in
    ** master..sysservers, let's add it first.
    */

    IF NOT EXISTS (SELECT *
                     FROM master..sysservers
                    WHERE srvname = @publisher)

    	BEGIN
    	    EXECUTE @retcode = sp_addserver @publisher
    	    IF @@error <> 0 OR @retcode <> 0
    	        BEGIN
                    RAISERROR (14075, 16, -1)
    	            RETURN (1)
    	        END
    	END

    /*
    ** Fetch the publisher's distributor account.
    */

    SELECT @proc = RTRIM(@publisher) + '.master..sp_helpdistributor '
    EXECUTE @retcode = @proc @account = @distaccount OUTPUT
    IF @@error <> 0 OR @retcode <> 0 OR @distaccount IS NULL
        BEGIN
            RAISERROR (14071, 16, -1)
            RETURN (1)
        END

    /*
    ** Set the server option to indicate that this is a publisher.
    */
    EXECUTE @retcode = sp_serveroption @publisher, 'pub', true
    IF @@error <> 0 OR @retcode <> 0
        BEGIN
            RAISERROR (14075, 16, -1)
            RETURN (1)
        END

    /*
    ** If @distaccount = 'LocalSystem' assume 'admin' privilege
    */
    IF @distaccount = 'LocalSystem'
       RETURN (0)

    /*
    ** Check if @distaccount has admin or repl privilege already.
    */
    EXECUTE @retcode = master.dbo.xp_logininfo @distaccount, 'all',
       @privilege = @privilege output
    IF @@error <> 0 OR @retcode <> 0
        BEGIN
            RAISERROR (14076, 10, -1, @distaccount)
            RETURN (0)
        END

    IF @privilege = 'admin' OR @privilege = 'repl'
       RETURN  (0)

    /*
    ** Grant replication privilege to the distributor NT account.
    */
    EXECUTE @retcode = master.dbo.xp_grantlogin @distaccount, repl
    IF @@error <> 0 OR @retcode <> 0
        BEGIN
            RAISERROR (14076, 10, -1, @distaccount)
            RETURN (0)
        END

go

print ''
print 'Creating procedure sp_addsubscriber.'
go

CREATE PROCEDURE sp_addsubscriber (
	@subscriber varchar (30),
	@type tinyint = 0,
	@login varchar (30) = NULL,
	@password varchar (30) = NULL,
	@commit_batch_size int = 100,
	@status_batch_size int = 100,
	@flush_frequency int = 0,
	@frequency_type int = 4,
	@frequency_interval int = 1,
	@frequency_relative_interval int = 1,
	@frequency_recurrence_factor int = 0,
	@frequency_subday int = 4,
	@frequency_subday_interval int = 5,
	@active_start_time_of_day int = 0,
	@active_end_time_of_day int = 235959,
	@active_start_date int = 0,
	@active_end_date int = 99991231,
	@description varchar (255) = NULL
        ) AS

    DECLARE @distributor varchar(30)
    DECLARE @distribdb varchar(30)
    DECLARE @distproc varchar (255)
    DECLARE @retcode int
    DECLARE @dsn_subscriber tinyint

    select @dsn_subscriber = 1	/* Const: subscriber type 'dsn' */	

    /*
    ** Parameter Check:  @subscriber.
    ** Check to make sure that the subscriber doesn't already exist, and
    ** that the name is a valid non-null identifier.
    */

    IF @subscriber IS NULL
        BEGIN
            RAISERROR (14043, 16, -1, 'The subscriber')
            RETURN (1)
        END

    EXECUTE @retcode = sp_validname @subscriber

    IF @@ERROR <> 0 OR @retcode <> 0
	RETURN (1)

    if LOWER(@subscriber) = 'all'
        BEGIN
            RAISERROR (14032, 16, -1, @subscriber)
            RETURN (1)
        END

	IF EXISTS (SELECT *
                 FROM master..sysservers
                WHERE srvname = @subscriber
                  AND srvstatus & 4 <> 0)

        BEGIN
	    RAISERROR (14040, 16, -1, @subscriber)
    	    RETURN (1)
        END


    /*
    **  If no MSsubscriber_info parameters skip RPC code.
    */

    IF @frequency_type = -1
        GOTO ADDSUB

    /*
    ** Get distribution server information for remote RPC
    ** subscription calls.
    */

    EXEC @retcode = sp_helpdistributor @distributor = @distributor OUTPUT,
                                       @distribdb   = @distribdb   OUTPUT

    IF @@error <> 0
        BEGIN
	     RAISERROR (14071, 16, -1)
    	     RETURN (1)
	 END

    IF @retcode <> 0 OR @distribdb IS NULL OR @distributor IS NULL
        BEGIN
            RAISERROR (14071, 16, -1)
            RETURN (1)
        END

    /*
    ** Add subscriber to distribution sysservers, if distribution
    ** server is remote.
    */

    If @distributor <> @@SERVERNAME
        BEGIN

	    SELECT @distproc = RTRIM(@distributor) + '.master..sp_addserver '
	    EXEC @distproc @server = @subscriber, @duplicate_ok = 'duplicate_ok'

	    /*
	    ** Assume distributor already existed if execute failed. Check
	    ** @@error for non-procedure errors.
	    */
	    IF @@error <> 0
                BEGIN
	            RAISERROR (14042, 16, -1)
	            RETURN (1)
	        END
	END

    /*
    ** Insert information into MSsubscriber_info
    */
    SELECT @distproc = RTRIM(@distributor) + '.' + RTRIM(@distribdb) + '..sp_MSadd_subscriber_info '
    EXEC @retcode = @distproc
    	 @@SERVERNAME,
         @subscriber,
	 @type,
	 @login,
	 @password,
	 @commit_batch_size,
	 @status_batch_size,
	 @flush_frequency,
	 @frequency_type,
	 @frequency_interval,
	 @frequency_relative_interval,
	 @frequency_recurrence_factor,
	 @frequency_subday,
	 @frequency_subday_interval,
	 @active_start_time_of_day,
	 @active_end_time_of_day,
	 @active_start_date,
	 @active_end_date,
	 @description = @description

    IF @@error <> 0 OR @retcode <> 0
        BEGIN
	    RAISERROR (14042, 16, -1)
	    RETURN (1)
	END

ADDSUB:

    /*
    ** The server may already be listed in master..sysservers, but might
    ** not be marked as a subscriber yet.  If it's not in
    ** master..sysservers, let's add it first.
    */

    IF NOT EXISTS (SELECT *
                         FROM master..sysservers
                        WHERE srvname = @subscriber)

	EXECUTE @retcode = sp_addserver @subscriber

        IF @@error <> 0 OR @retcode <> 0
            BEGIN
	        RAISERROR (14042, 16, -1)
                RETURN (1)
            END

    /*
    ** Set the server option to indicate this is a subscriber.
    */

    EXECUTE @retcode = sp_serveroption @subscriber, 'sub', true

    IF @@error <> 0 OR @retcode <> 0
        BEGIN
	   RAISERROR (14042, 16, -1)
           RETURN (1)
        END

    /*
    ** Set the server option to indicate this is a DSN subscriber.
    */
    if @type = @dsn_subscriber
       BEGIN
          EXECUTE @retcode = sp_serveroption @subscriber, 'dsn', true
          IF @@error <> 0 OR @retcode <> 0
             BEGIN
	         RAISERROR (14042, 16, -1)
                 RETURN (1)
             END
       END

     /*
     ** Setup remotelogin for subscribing server 'sa' account, if
     ** one does not already exist.
     **/

     IF EXISTS (SELECT *
                  FROM sysremotelogins srl,
	               sysservers ss
                 WHERE ss.srvname = @subscriber
	           AND srl.remoteserverid = ss.srvid
	           AND (srl.remoteusername = 'sa'
	            OR (srl.remoteusername IS NULL AND srl.suid = -1)))
	 BEGIN
       	     RETURN (0)
	 END

     EXECUTE @retcode = sp_addremotelogin @subscriber, repl_subscriber, sa

     IF @@error <> 0 OR @retcode <> 0
        BEGIN
	    RAISERROR (14042, 16, -1)
	    RETURN (1)
	END

    EXECUTE @retcode = sp_remoteoption @subscriber, repl_subscriber, sa, trusted, true

    IF @@error <> 0 OR @retcode <> 0
        BEGIN
            RAISERROR (14042, 16, -1)
            RETURN (1)
        END
go

/*
** Create replication stored procedures.
** Part 1:  create codependent procedures.
*/

print ''
print 'Creating procedure sp_hcchangesubstatus1.'
go
CREATE PROCEDURE sp_hcchangesubstatus1
        @publication varchar(30) = '%',
        @article varchar(30) = '%',
        @subscriber varchar(30) = '%'
        AS

    DECLARE hCsubstatus CURSOR FOR
        SELECT sub.artid,
               art.objid,
               sub.srvid,
               ss.srvname,
               sub.dest_db,
               sub.status,
	       ss.srvstatus,
	       pub.repl_freq
          FROM syssubscriptions sub,
               sysarticles art,
               syspublications pub,
               sysservers ss
         WHERE pub.name LIKE @publication
           AND art.name LIKE @article
           AND ss.srvname LIKE @subscriber
           AND sub.srvid = ss.srvid
           AND sub.artid = art.artid
           AND art.pubid = pub.pubid
	 FOR READ ONLY

go

print ''
print 'Creating procedure sp_hcchangesubstatus2.'
go
CREATE PROCEDURE sp_hcchangesubstatus2
        @publication varchar(30) = '%',
        @article varchar(30) = '%',
        @subscriber varchar(30) = '%',
        @previous_status varchar(30),
        @prevstatid tinyint
        AS

    DECLARE hCsubstatus CURSOR FOR
        SELECT sub.artid,
               art.objid,
               sub.srvid,
               ss.srvname,
               sub.dest_db,
               sub.status,
	       ss.srvstatus,
	       pub.repl_freq
          FROM syssubscriptions sub,
               sysarticles art,
               syspublications pub,
               sysservers ss
         WHERE pub.name LIKE @publication
           AND art.name LIKE @article
           AND ss.srvname LIKE @subscriber
           AND sub.srvid = ss.srvid
           AND sub.artid = art.artid
           AND art.pubid = pub.pubid
     	   AND sub.status = @prevstatid
	 FOR READ ONLY
go

print ''
print 'Creating procedure sp_changesubstatus.'
go

CREATE PROCEDURE sp_changesubstatus (
	@publication varchar (30) = '%',    /* publication name */
	@article varchar (30) = '%',        /* article name */
	@subscriber varchar(30) = '%',      /* subscriber name */
	@status varchar(30),                /* subscription status */
	@previous_status varchar(30)=NULL   /* previous subscription status */
	) AS

    SET NOCOUNT ON

    DECLARE @inactive tinyint
    DECLARE @subscribed tinyint
    DECLARE @active tinyint
    DECLARE @public tinyint
    DECLARE @replicate_bit smallint
    DECLARE @subscriber_bit smallint
    DECLARE @msg varchar(255)
    DECLARE @prevstatid tinyint
    DECLARE @artid int
    DECLARE @tabid int
    DECLARE @srvid smallint
    DECLARE @statusid tinyint
    DECLARE @distributor varchar(30)
    DECLARE @distribdb varchar(30)
    DECLARE @distproc varchar (255)
    DECLARE @pub_db varchar(30)
    DECLARE @dest_db varchar (30)
    DECLARE @sub_name varchar (30)
    DECLARE @sub_status tinyint
    DECLARE @sub_ts binary (8)
    DECLARE @sub_type smallint
    DECLARE @cmd0 varchar (255)
    DECLARE @cmd1 varchar (255)
    DECLARE @cmd2 varchar (255)
    DECLARE @cmd3 varchar (255)
    DECLARE @retcode int
    DECLARE @dsn_bit smallint
    DECLARE @repl_freq tinyint

    /*
    ** Initializations.
    */

    SELECT @inactive = 0        /* Const: subscription status 'inactive' */
    SELECT @subscribed = 1      /* Const: subscription status 'subscribed' */
    SELECT @active = 2          /* Const: subscription status 'active' */
    SELECT @public = 0          /* Const: publication status 'public' */
    SELECT @replicate_bit = 64  /* Const: replication bit in sysobjects=0x40 */
    SELECT @subscriber_bit = 4  /* Const: subscription server status */
    SELECT @pub_db = DB_NAME()
    SELECT @dsn_bit = 32

    /*
    ** Parameter Check:  @publication
    ** Check to make sure that the publication exists, that it's not NULL,
    ** and that it conforms to the rules for identifiers.
    */

    IF @publication IS NULL
        BEGIN
            RAISERROR (14043, 16, -1, 'The publication')
            RETURN (1)
        END

    IF @publication <> '%'
        BEGIN
            EXECUTE @retcode = sp_validname @publication
            IF @@ERROR <> 0 OR @retcode <> 0
		RETURN (1)
        END

    IF NOT EXISTS (SELECT * FROM syspublications WHERE name LIKE @publication)
        BEGIN
	    IF @publication = '%'
                RAISERROR (14008, 11, -1)
	    ELSE
                RAISERROR (15001, 11, -1, @publication)
	    RETURN (1)
        END

    /*
    ** Parameter Check:  @article
    ** Check to make sure that the article exists, that it's not null,
    ** and that it conforms to the rules for identifiers.
    */

    IF @article IS NULL
        BEGIN
            RAISERROR (14043, 16, -1, 'The article')
            RETURN (1)
        END

    IF @article <> '%'
        BEGIN
            EXECUTE @retcode = sp_validname @article
            IF @@ERROR <> 0 OR @retcode <> 0
		RETURN (1)
        END

    IF NOT EXISTS (SELECT *
                     FROM sysarticles a,
                          syspublications b
	            WHERE a.name LIKE @article
                      AND a.pubid = b.pubid
                      AND b.name LIKE @publication)

        BEGIN
	    IF @article = '%'
                RAISERROR (14009, 11, -1, @publication)
	    ELSE
                RAISERROR (15001, 11, -1, @article)
	    RETURN (1)
        END

    /*
    ** Parameter Check:  @subscriber
    ** Check to make sure that the subscriber exists, that it is not NULL,
    ** and that it conforms to the rules for identifiers.
    */

    IF @subscriber IS NULL
        BEGIN
            RAISERROR (14043, 16, -1, 'The subscriber')
            RETURN (1)
        END

    IF @subscriber <> '%'
        BEGIN
            EXECUTE @retcode = sp_validname @subscriber
            IF @@ERROR <> 0 OR @retcode <> 0
		RETURN (1)
        END

    IF NOT EXISTS (SELECT *
                     FROM master..sysservers
                    WHERE srvname LIKE @subscriber
                      AND (srvstatus & 4) <> 0)

        BEGIN
            IF @subscriber ='%'
                RAISERROR (14064, 11, -1)
	    ELSE
                RAISERROR (14063, 11, -1)
            RETURN (1)
        END

    /*
    ** Parameter Check: @status.
    ** Set the @statusid according to the @status value.  Values can be
    ** any of the following:
    **
    **      status      statusid
    **      =========   ========
    **      inactive           0
    **      subscribed         1
    **      active             2
    */

    IF LOWER(@status) NOT IN ('active', 'subscribed', 'inactive')
        BEGIN
            RAISERROR (14065, 16, -1)
	    RETURN (1)
        END

    IF LOWER(@status) IN ('active')
        SELECT @statusid = @active
    ELSE IF LOWER(@status) IN ('subscribed')
        SELECT @statusid = @subscribed
    ELSE
        SELECT @statusid = @inactive

    /*
    ** Parameter Check: @previous_status.
    ** Set the @prevstatid according to the @previous_status value.
    ** Values can be any of the following:
    **
    **      previous_status      prevstatid
    **      ===============      ==========
    **      inactive                      0
    **      subscribed                    1
    **      active                        2
    */

    IF @previous_status IS NOT NULL
        BEGIN
            IF LOWER(@previous_status) NOT IN ('active', 'subscribed', 'inactive')
                BEGIN
                    RAISERROR (14066, 16, -1)
	            RETURN (1)
                END

            IF LOWER(@status) = LOWER(@previous_status)
                BEGIN
                    RAISERROR (14067, 16, -1)
	            RETURN (1)
                END

            IF LOWER(@previous_status) IN ('active')
                SELECT @prevstatid = @active
            ELSE IF LOWER(@previous_status) IN ('subscribed')
                SELECT @prevstatid = @subscribed
            ELSE
               SELECT @prevstatid = @inactive
        END

    /*
    ** Get distribution server information for remote RPC
    ** subscription calls.
    */

    EXEC @retcode = sp_helpdistributor @distributor = @distributor OUTPUT,
    	                               @distribdb = @distribdb OUTPUT

    IF @@ERROR <> 0
        BEGIN
            RAISERROR (14071, 16, -1)
    	    RETURN (1)
	 END

    IF @retcode <> 0 OR @distribdb IS NULL OR @distributor IS NULL
        BEGIN
            RAISERROR (14071, 16, -1)
            RETURN (1)
        END

    BEGIN TRANSACTION changesubstatus

        /*
        ** Declare cursor containing subscriptions to be updated.
        */

        IF @previous_status IS NOT NULL
            EXECUTE sp_hcchangesubstatus2 @publication, @article, @subscriber, @previous_status, @prevstatid
        ELSE
            EXECUTE sp_hcchangesubstatus1 @publication, @article, @subscriber

        OPEN hCsubstatus

        FETCH hCsubstatus INTO @artid, @tabid, @srvid, @sub_name, @dest_db,
	   @sub_status, @sub_type, @repl_freq

        WHILE (@@fetch_status <> -1)
            BEGIN
        	/*
        	** If current status is same as new status, do nothing.
        	*/

        	IF @sub_status = @statusid
        	    BEGIN
        	        FETCH hCsubstatus INTO @artid, @tabid, @srvid, @sub_name,
			   @dest_db, @sub_status, @sub_type, @repl_freq
        		CONTINUE
        	    END

		/*
		** Update syssubscription status
		*/
	        UPDATE syssubscriptions
                   SET status = @statusid
                   FROM syssubscriptions sub,
                       sysarticles art,
                       syspublications pub
                   WHERE pub.name LIKE @publication
                        AND art.artid = @artid
                        AND sub.srvid = @srvid
                        AND sub.artid = @artid
                        AND art.pubid = pub.pubid
		if @@ERROR <> 0
               	    BEGIN
               		CLOSE hCsubstatus
               		DEALLOCATE hsubstatus
               		ROLLBACK TRANSACTION changesubstatus
                        RAISERROR (14053, 16, -1)
               	        RETURN (1)
               	    END

		/*
		** Get timestamp of subscription.
		*/
		SELECT @sub_ts = timestamp FROM
		       syssubscriptions sub,
                       sysarticles art,
                       syspublications pub
                   WHERE pub.name LIKE @publication
                        AND art.artid = @artid
                        AND sub.srvid = @srvid
                        AND sub.artid = @artid
                        AND art.pubid = pub.pubid
		IF @sub_ts IS NULL
                    BEGIN
               		CLOSE hCsubstatus
               		DEALLOCATE hCsubstatus
               		ROLLBACK TRANSACTION changesubstatus
                        RAISERROR (14053, 16, -1)
               	        RETURN (1)
               	    END

        	/*
        	** If activating subscription, update sysarticles, sysobjects and
        	** MSjob_subscriptions.
        	*/
                IF @statusid = @active
                    BEGIN

                	/*
                	** Update status of article to show it has been activated.
                	*/
                	UPDATE sysarticles SET status = status | 1 WHERE artid = @artid
                	IF @@ERROR <> 0
                	    BEGIN
                		CLOSE hCsubstatus
                		DEALLOCATE hCsubstatus
                		ROLLBACK TRANSACTION changesubstatus
                                RAISERROR (14069, 16, -1)
                	        RETURN (1)
                	    END

                        /*
                        ** Turn the replication flag on for this object in the
                        ** sysobjects table (make it logbased).
                        */

                	if @repl_freq = 0
                	  BEGIN
                		UPDATE sysobjects
                           	SET category = category | @replicate_bit
                	 	WHERE id = (SELECT objid
                                       	FROM sysarticles
                                      	WHERE artid = @artid)
                      END

                		IF @@ERROR <> 0
                	   	 BEGIN
                			CLOSE hCsubstatus
                			DEALLOCATE hCsubstatus
                			ROLLBACK TRANSACTION changesubstatus
                          	      RAISERROR (14068, 16, -1)
                	        	RETURN (1)
                	    	END

                    END

		/*
		** Update status of all Text\Image columns if
		** subscriber is non-SQL Server.
		*/
		IF (@sub_type & @dsn_bit) <> 0
		   BEGIN
                      IF @statusid = @subscribed OR @statusid = @active
		        BEGIN
			     EXEC @retcode = sp_articletextcol @artid, NULL,
				'nonsqlsub', 'add'
                	     IF @@ERROR <> 0 OR @retcode <> 0
                	        BEGIN
                		   CLOSE hCsubstatus
                		   DEALLOCATE hCsubstatus
                		   ROLLBACK TRANSACTION changesubstatus
                                   RAISERROR (14068, 16, -1)
                	           RETURN (1)
                	        END
		        END
		     ELSE IF @statusid = @inactive
		        BEGIN
			     EXEC @retcode = sp_articletextcol @artid, NULL,
				'nonsqlsub', 'drop'
			     IF @@ERROR <> 0 OR @retcode <> 0
				BEGIN
				   CLOSE hCsubstatus
				   DEALLOCATE hCsubstatus
				   ROLLBACK TRANSACTION changesubstatus
				   RAISERROR (14068, 16, -1)
				   RETURN (1)
				END
		        END
		   END

               	/*
        	** If deactivating subscription, update sysarticles, sysobjects and
        	** MSjob_subscriptions.
        	*/

                IF @statusid <> @active AND @sub_status = @active
                    BEGIN
		        /*
			** Set the article status to 'inactive' if there are
			** no other active subscriptions on it.
			*/
			IF NOT EXISTS (SELECT * FROM syssubscriptions WHERE
			   artid = @artid AND status = @active)
			   BEGIN
                              UPDATE sysarticles SET status = status & ~1 WHERE
			         artid = @artid
                              IF @@ERROR <> 0
                                  BEGIN
                		     CLOSE hCsubstatus
                		     DEALLOCATE hCsubstatus
                		     ROLLBACK TRANSACTION changesubstatus
                                     RAISERROR (14069, 16, -1)
                                     RETURN (1)
                	          END
			   END

		        /*
			** Set the object replication bits  to 'inactive' if
			** there are no other active subscriptions on the
			** table.
			*/
			IF NOT EXISTS (SELECT * FROM syssubscriptions WHERE
			   artid IN (SELECT sa.artid FROM sysarticles sa, syspublications sp WHERE
				sa.objid = @tabid and sa.pubid = sp.pubid and sp.repl_freq = 0) AND status = @active)
			   BEGIN
                	      UPDATE sysobjects
                                 SET category = category & ~@replicate_bit
                                 WHERE id = (SELECT objid
                                      FROM sysarticles WHERE artid= @artid)
			       IF @@ERROR <> 0
			          BEGIN
                		     CLOSE hCsubstatus
                		     DEALLOCATE hCsubstatus
				     ROLLBACK TRANSACTION changesubstatus
				     RAISERROR (14068, 16, -1)
				     RETURN (1)
				  END
			   END

                    END

                /*
                ** Add the active subscription to the distributor's
                ** subscriptions table if changing status from @inactive
                */
                IF @sub_status = @inactive
                    BEGIN
                	SELECT @distproc = RTRIM(@distributor) + '.' + RTRIM(@distribdb) + '..sp_MSadd_subscription '
                	EXEC @retcode = @distproc @@SERVERNAME, @pub_db, @sub_name, @artid, @dest_db, @statusid, @sub_ts
                	IF @@ERROR <> 0 OR @retcode <> 0
                	    BEGIN
               		        CLOSE hCsubstatus
               		        DEALLOCATE hCsubstatus
                		ROLLBACK TRANSACTION changesubstatus
                                RAISERROR (14070, 16, -1)
                		RETURN (1)
                	    END
                    END
		ELSE
		    BEGIN
		       /*
		       ** Drop the deactivated subscription from the distributor's
		       ** subscriptions table.
		       */
                       IF @statusid = @inactive
                          BEGIN
                	     SELECT @distproc = RTRIM(@distributor) + '.' + RTRIM(@distribdb) + '..sp_MSdrop_subscription '
                	     EXEC @retcode = @distproc @@SERVERNAME, @pub_db, @sub_name,  @artid, @dest_db
                	     IF @@ERROR <> 0 OR @retcode <> 0
                	        BEGIN
                		   CLOSE hCsubstatus
               		           DEALLOCATE hCsubstatus
                		   ROLLBACK TRANSACTION changesubstatus
                                   RAISERROR (14070, 16, -1)
                		   RETURN (1)
                	        END
			  END
                       /*
                       ** Update subscription status and timestamp in distributor's
                       ** subscriptions table.
                       */
 		       ELSE
		          BEGIN
              	             SELECT @distproc = RTRIM(@distributor) + '.' + RTRIM(@distribdb) + '..sp_MSupdate_subscription '
                             EXEC @retcode = @distproc @@SERVERNAME, @pub_db, @sub_name, @artid, @statusid, @sub_ts
                             IF @@ERROR <> 0 OR @retcode <> 0
                                BEGIN
                		   CLOSE hCsubstatus
                		   DEALLOCATE hCsubstatus
                	           ROLLBACK TRANSACTION changesubstatus
                                   RAISERROR (14070, 16, -1)
                	           RETURN (1)
                               END
		          END
		    END

	        /*
		** Set internal object replication bit  to 'inactive' if
		** there are no other active subscriptions on the
		** table.
		*/
		IF @statusid = @inactive AND @sub_status = @active AND
		   NOT EXISTS (SELECT * FROM syssubscriptions WHERE
		      artid IN (SELECT artid FROM sysarticles WHERE
		      objid = @tabid) AND status = @active)
		   BEGIN
                      /* Turn off object replication */
                      SELECT @cmd1 = 'exec sp_replstatus ' + CONVERT(varchar(10), @tabid) + ', 0'
                      EXEC (@cmd1)
                   END

                /* Turn on object replication */
	        IF @statusid = @active
		   BEGIN
                     SELECT @cmd1 = 'exec sp_replstatus ' + CONVERT(varchar(10), @tabid) + ', 1'
                     EXEC (@cmd1)
		   END

                /*
                ** Get next row.
                */
                FETCH hCsubstatus INTO @artid, @tabid, @srvid, @sub_name, @dest_db,
		   @sub_status, @sub_type, @repl_freq

            END

        CLOSE hCsubstatus
        DEALLOCATE hCsubstatus

    /*
    ** Force the article cache to be refreshed.
    */
    EXECUTE sp_replflush
    COMMIT TRANSACTION changesubstatus
go

print ''
print 'Creating procedure sp_addsubscription.'
go

CREATE PROCEDURE sp_addsubscription (
	@publication varchar (30),		/* publication name */
	@article varchar (30) = 'all',		/* article name */
	@subscriber varchar(30),		/* subscriber name */
	@destination_db varchar (30) = NULL,	/* destination database */
	@sync_type varchar (15) = 'automatic',	/* subscription sync type */
	@status varchar(30) = NULL              /* subscription status */
	) AS

    SET NOCOUNT ON

    /*
    ** Declarations.
    */

    DECLARE @artid int
    DECLARE @pre_creation_cmd tinyint
    DECLARE @none tinyint
    DECLARE @automatic tinyint
    DECLARE @cmd varchar(255)
    DECLARE @cmd2 varchar(255)
    DECLARE @inactive tinyint
    DECLARE @manual tinyint
    DECLARE @pubid int
    DECLARE @retcode int
    DECLARE @srvid smallint
    DECLARE @srvstatus smallint
    DECLARE @subscriber_bit smallint
    DECLARE @sync_typeid tinyint
    DECLARE @dsn_bit smallint
    DECLARE @truncate tinyint
    DECLARE @sync_method tinyint
    DECLARE @char_bcp tinyint

    /*
    ** Initializations.
    */

    SELECT @none = 2            /* Const: synchronization type 'none' */
    SELECT @automatic = 1       /* Const: synchronization type 'automatic' */
    SELECT @manual = 0          /* Const: synchronization type 'manual' */
    SELECT @inactive = 0        /* Const: subscription status 'inactive' */
    SELECT @subscriber_bit = 4  /* Const: subscription server status */
    SELECT @dsn_bit = 32  	/* Const: ODBC DSN server status */
    SELECT @truncate = 3	/* Const: truncate pre-creation command */
    SELECT @char_bcp = 1	/* Const: character bcp sync method */

    /*
    ** Security Check.
    ** Only the System Administrator (SA) or the Database Owner (dbo) can
    ** add an article to a publication.
    */

    IF suser_id() <> 1 AND user_id() <> 1
	BEGIN
            RAISERROR (15000, 14, -1)
	    RETURN (1)
        END

    /*
    ** Parameter Check: @subscriber.
    ** Check if the server exists and that it is a subscription server.
    */

    IF @subscriber IS NULL
        BEGIN
            RAISERROR (14043, 16, -1, 'The subscriber')
            RETURN (1)
        END

    EXECUTE @retcode = sp_validname @subscriber

    IF @retcode <> 0
	RETURN (1)

    SELECT @srvid = srvid, @srvstatus = srvstatus
      FROM master..sysservers
     WHERE srvname = @subscriber
       AND (srvstatus & @subscriber_bit) <> 0

    IF @srvid IS NULL
        BEGIN
            RAISERROR (14010, 16, -1)
   	    RETURN (1)
        END

    /*
    ** Parameter Check: @publication.
    ** Check to make sure that the publication exists and that it conforms
    ** to the rules for identifiers.
    */

    IF @publication IS NOT NULL
        BEGIN

            EXECUTE @retcode = sp_validname @publication

            IF @retcode <> 0
		RETURN (1)

            IF NOT EXISTS (SELECT * FROM syspublications WHERE name = @publication)
                BEGIN
                    RAISERROR (15001, 11, -1, @publication)
                    RETURN (1)
                END

        END

    SELECT @pubid = pubid, @sync_method = sync_method
       FROM syspublications WHERE name = @publication

    IF @pubid IS NULL
        BEGIN
            RAISERROR (14043, 16, -1, 'The publication')
            RETURN (1)
        END

    /*
    ** If the subscriber is an ODBC DSN, only allow subscriptions to
    ** publications with a character mode bcp sync_method.
    */
    IF (@srvstatus & @dsn_bit) <> 0 AND @sync_method <> @char_bcp
       BEGIN
            RAISERROR (14095, 16, -1, @publication, @subscriber)
            RETURN (1)
        END

    /*
    ** Parameter Check:  @article
    ** Check to make sure that the article exists, is not NULL, and
    ** conforms to the rules for identifiers.
    */

    IF LOWER(@article) = 'all'
    	/*
	** Get all articles in the publication that are not subscribed to
	** by the @subscriber
	*/
        BEGIN
            SELECT @cmd = ''
            SELECT @cmd = @cmd + 'DECLARE hCx CURSOR FOR '
            SELECT @cmd = @cmd + ' SELECT DISTINCT a.name '
            SELECT @cmd = @cmd + '   FROM sysarticles a, syspublications b '
            SELECT @cmd = @cmd + '  WHERE a.pubid = b.pubid '
            SELECT @cmd = @cmd + '    AND b.name = ''' + @publication + ''''
	    SELECT @cmd2 = ' AND NOT EXISTS (SELECT * from syssubscriptions s '
	    SELECT @cmd2 = @cmd2 + ' WHERE s.artid = a.artid AND s.status <> 0 AND s.srvid = '
	    SELECT @cmd2 = @cmd2 + CONVERT(varchar(10), @srvid) + ')' + ' FOR READ ONLY'
            EXECUTE (@cmd + @cmd2)
            OPEN hCx
            FETCH hCx INTO @article
            WHILE (@@fetch_status <> -1)
                BEGIN
                    EXECUTE sp_addsubscription @publication    = @publication,
                                               @article        = @article,
			                       @subscriber     = @subscriber,
                                               @destination_db = @destination_db,
                                               @sync_type      = @sync_type
                    FETCH hCx INTO @article
                END
            CLOSE hCx
            DEALLOCATE hCx
            RETURN (0)
        END

    SELECT @artid = artid, @pre_creation_cmd = pre_creation_cmd
      FROM sysarticles
     WHERE name = @article
       AND pubid = @pubid

    IF @article IS NOT NULL
        BEGIN
            EXECUTE @retcode = sp_validname @article
            IF @retcode <> 0
		RETURN (1)

	    IF NOT EXISTS (SELECT *
                             FROM sysarticles
                            WHERE artid = @artid
                              AND pubid = @pubid)
                BEGIN
                    RAISERROR (15001, 11, -1, @article)
                    RETURN (1)
                END
        END

    IF @artid IS NULL
        BEGIN
            RAISERROR (14043, 16, -1, 'The article')
            RETURN (1)
        END

    /*
    ** If the subscriber is an ODBC DSN, do not allow subscriptions to
    ** articles with a "truncate" pre_creation_cmd.
    */
    IF (@srvstatus & @dsn_bit) <> 0 AND @pre_creation_cmd = @truncate
       BEGIN
            RAISERROR (14094, 16, -1, @article, @subscriber)
            RETURN (1)
        END

   /*
   ** Parameter Check: @sync_type.
   ** Set sync_typeid based on the @sync_type specified.
   **
   **   sync_typeid     sync_type
   **   ===========     =========
   **             0     manual
   **             1     automatic
   **             2     none
   */

   IF LOWER(@sync_type) NOT IN ('automatic', 'manual', 'none')
       BEGIN
           RAISERROR (14052, 16, -1)
           RETURN (1)
       END

   IF LOWER(@sync_type) = 'automatic'
	SELECT @sync_typeid = @automatic
   ELSE IF LOWER(@sync_type) =  'manual'
        SELECT @sync_typeid = @manual
   ELSE
        SELECT @sync_typeid = @none


    /*
    ** If it's an automatic sync, ensure that none of the articles
    ** being subscribed to have the 'owner qualified' status bit set
    */

    IF @sync_typeid = @automatic
    BEGIN
        IF EXISTS ( SELECT * FROM sysarticles
                    WHERE artid = @artid AND pubid = @pubid AND
                    4 = (status & 4) )
        BEGIN
            RAISERROR( 14098, 16, -1 )
            RETURN (1)
        END

    END

    /*
    ** Parameter Check: @destination_db.
    ** Set @destination_db to current database if not specified.  Make
    ** sure that the @destination_db conforms to the rules for identifiers.
    */

    IF @destination_db IS NULL SELECT @destination_db = DB_NAME()

    EXECUTE @retcode = sp_validname @destination_db

    IF @retcode <> 0
	RETURN (1)

    /*
    ** Bug 12850:
    ** Make sure that the creation_script is specified if pre_creation_cmd is "drop"
    ** Note that at this point, @article cannot be 'all'.
    */
    if exists (select * from sysarticles where
    		name = @article AND
    		pre_creation_cmd = 1 AND
    		creation_script is null)
    	BEGIN
    		RAISERROR (14096, 16, -1)	
    		RETURN (1)
    	END


    /*
    ** Add subscription to syssubscriptions
    */
    BEGIN TRAN addsubscription

    /*
    ** If no subscription exists, add it to syssubscriptions.
    */
    IF NOT EXISTS (SELECT *
                     FROM syssubscriptions
                    WHERE srvid = @srvid
                      AND artid = @artid)
        BEGIN
	   INSERT syssubscriptions (artid,
                                    srvid,
                                    dest_db,
                                    status,
                                    sync_type,
                                    timestamp)
	   VALUES (@artid,
                   @srvid,
                   @destination_db,
                   @inactive,
                   @sync_typeid,
                   NULL)

	   IF @@ERROR <> 0
	       BEGIN
	           ROLLBACK TRAN  addsubscription
                   RAISERROR (14057, 16, -1)
	           RETURN (1)
	       END
        END
    ELSE
        BEGIN
    	   /*
	   ** If is a 'restricted' publication the subscription may already
	   ** exist.
	   */
	   IF EXISTS (SELECT * FROM syspublications WHERE pubid = @pubid
                 AND restricted = 0)
           BEGIN
              ROLLBACK TRAN  addsubscription
              RAISERROR (14058, 16, -1)
              RETURN (1)
           END
	END

    /*
    ** If the @status was not provided determine the default value.
    ** If the @sync_type = 'none' then the subscription defaults to 'active'.
    ** Else the subscription defaults to 'subscribed'.
    */
    IF @status IS NULL
       BEGIN
          IF @sync_typeid = @none	
	     SELECT @status = 'active'
	  ELSE
	     SELECT @status = 'subscribed'
	END

    /*
    ** Set publication subscription status.
    */
    EXEC @retcode = sp_changesubstatus
	@publication = @publication,
	@article     = @article,
	@subscriber  = @subscriber,
	@status      = @status

    IF @@error <> 0 OR @retcode <> 0
	BEGIN
	   ROLLBACK TRAN  addsubscription
           RAISERROR (14057, 16, -1)
	   RETURN (1)
	END

    COMMIT TRAN addsubscription
go

print ''
print 'Creating procedure sp_changearticle.'
GO
CREATE PROCEDURE sp_changearticle (
    @publication varchar(30) = NULL,        /* Publication name */
    @article varchar(30) = NULL,            /* Article name */
    @property varchar(20) = NULL,           /* The property to change */
    @value varchar(255) = NULL              /* The new property value */
    ) AS

    SET NOCOUNT ON

    /*
    ** Declarations.
    */

    DECLARE @artid int
    DECLARE @cmd1 varchar(255)
    DECLARE @cmd2 varchar(255)
    DECLARE @db varchar(30)
    DECLARE @filter int
    DECLARE @object varchar(30)
    DECLARE @owner varchar(30)
    DECLARE @pubid int
    DECLARE @retcode int
    DECLARE @site varchar(30)
    DECLARE @sync_objid int
    DECLARE @typeid tinyint
    DECLARE @precmdid tinyint
    DECLARE @inactive tinyint


    select @inactive = 0

    /*
    ** Security Check
    ** Only the System Administrator (SA) or the Database Owner (dbo) can
    ** perform this procedure.
    */

    IF suser_id() <> 1 AND user_id() <> 1
        BEGIN
            RAISERROR (15000, 14, -1)
    	    RETURN (1)
        END

    /*
    ** Check to see if the database has been activated for publication.
    */

    IF (SELECT category & 1
          FROM master..sysdatabases
         WHERE name = DB_NAME()) = 0

	BEGIN
            RAISERROR (14013, 16, -1)
	    RETURN (1)
	END

    /*
    ** Parameter Check:  @property.
    ** If the @property parameter is NULL, print the options.
    */

    IF @property IS NULL
        BEGIN
            CREATE TABLE #tab1 (properties varchar(30))
            INSERT INTO #tab1 VALUES ('name')
            INSERT INTO #tab1 VALUES ('description')
            INSERT INTO #tab1 VALUES ('sync_object')
            INSERT INTO #tab1 VALUES ('type')
            INSERT INTO #tab1 VALUES ('ins_cmd')
            INSERT INTO #tab1 VALUES ('del_cmd')
            INSERT INTO #tab1 VALUES ('upd_cmd')
            INSERT INTO #tab1 VALUES ('filter')
            INSERT INTO #tab1 VALUES ('dest_table')
            INSERT INTO #tab1 VALUES ('creation_script')
            INSERT INTO #tab1 VALUES ('pre_creation_cmd')
            INSERT INTO #tab1 VALUES ('status')
            PRINT ''
            SELECT * FROM #tab1
            RETURN (0)
        END

    /*
    ** Parameter Check:  @property.
    ** Check to make sure that @property is a valid property in
    ** sysarticles.
    */
    IF @property IS NULL OR LOWER(@property) NOT IN ('name',
                                                     'description',
                                                     'sync_object',
                                                     'type',
                                                     'ins_cmd',
                                                     'del_cmd',
                                                     'upd_cmd',
                                                     'filter',
                                                     'dest_table',
                                                     'creation_script',
                                                     'pre_creation_cmd',
                                                     'status')
        BEGIN
            RAISERROR (14022, 16, -1)
            RETURN (1)
        END


    /*
    ** Parameter Check:  @publication.
    ** Make sure that the publication exists.
    */

    IF @publication IS NULL
        BEGIN
            RAISERROR (14043, 16, -1, 'The publication')
            RETURN (1)
        END

    EXECUTE @retcode = sp_validname @publication

    IF @@ERROR <> 0 OR @retcode <> 0
	RETURN (1)

    SELECT @pubid = pubid FROM syspublications WHERE name = @publication

    IF @pubid IS NULL
        BEGIN
            RAISERROR (15001, 11, -1, @publication)
    	    RETURN (1)
        END
    ELSE

    /*
    ** Check to see that the article exists in sysarticles.
    ** Fetch the article identification number.
    */

    IF @article IS NULL
        BEGIN
            RAISERROR (14043, 16, -1, 'The article')
            RETURN (1)
        END

    EXECUTE @retcode = sp_validname @article

    IF @retcode <> 0
	RETURN (1)

    SELECT @artid = artid
      FROM sysarticles
     WHERE name = @article
       AND pubid = @pubid
    IF @artid IS NULL
        BEGIN
            RAISERROR (15001, 11, -1, @article)
            RETURN (1)
        END

    /*
    ** Only unsubscribed articles may be modified.
    */
    IF EXISTS (SELECT * FROM syssubscriptions WHERE artid = @artid
       AND status <> @inactive)
        BEGIN
            RAISERROR (14092, 11, -1)
            RETURN (1)
        END

    /*
    ** Change the property.
    */

    IF LOWER(@property) IN ('name', 'description', 'ins_cmd', 'del_cmd', 'upd_cmd', 'dest_table', 'creation_script')
        BEGIN

            IF LOWER(@property) = 'name'
                BEGIN

                    IF @value IS NULL
                        BEGIN
                            RAISERROR (14043, 16, -1, 'The article')
                            RETURN (1)
                        END

                    EXECUTE @retcode = sp_validname @value

                    IF @@ERROR <> 0 OR @retcode <> 0
			RETURN (1)

                    IF EXISTS (SELECT *
                                 FROM syspublications a, sysarticles b
                                WHERE b.name = @value
                                  AND a.pubid = b.pubid
                                  AND a.name = @publication)
                        BEGIN
                            RAISERROR (14016, 16, -1, @value)
                            RETURN (1)
                        END

                END

            /*
            ** Check the validity of the destination table.  NULL should
            ** get converted to the source table name.  Destination table
            ** names can be owner qualified, but not database qualified.
            */

            IF LOWER(@property) = 'dest_table'
                BEGIN
                    IF @value IS NULL
                        SELECT @value = object_name(objid)
                          FROM sysarticles
                         WHERE artid = @artid
                           AND pubid = @pubid
                    IF @value LIKE '%.%.%' OR @value LIKE '%.%'
                        BEGIN
                            EXECUTE sp_namecrack @value,
                                                 @site OUTPUT,
                                                 @db OUTPUT,
                                                 @owner OUTPUT,
                                                 @object OUTPUT
                            IF @db IS NOT NULL
                                BEGIN
                                    RAISERROR (14079, 16, -1)
                                    RETURN (1)
                                END
                            EXECUTE @retcode = sp_validname @object
                            IF @@ERROR <> 0 OR @retcode <> 0
				RETURN (1)
                        END
                    ELSE
                        BEGIN
                            EXECUTE @retcode = sp_validname @value
                            IF @@ERROR <> 0 OR @retcode <> 0
				RETURN (1)
                        END
                END

            SELECT @cmd1 = 'UPDATE sysarticles '
		IF @value IS NULL
		BEGIN
	            SELECT @cmd1 = @cmd1 + '   SET ' + @property + ' = NULL'
		    SELECT @cmd2 = ' WHERE artid = ' + STR(@artid)
		    SELECT @cmd2 = @cmd2 + '   AND pubid = ' + STR(@pubid)
		    EXECUTE (@cmd1 + @cmd2)
		END
		ELSE
		BEGIN
		    SELECT @cmd1 = @cmd1 + '   SET ' + @property + ' = '''
		    SELECT @cmd2 = ''' WHERE artid = ' + STR(@artid)
		    SELECT @cmd2 = @cmd2 + '   AND pubid = ' + STR(@pubid)
		    EXECUTE (@cmd1 + @value + @cmd2)
		END
            IF @@ERROR <> 0 RETURN (1)
        END

    IF LOWER(@property) = 'sync_object'
        BEGIN

            /*
            ** Check for a valid synchronization object.
            */

            IF @value IS NULL
                BEGIN
                    RAISERROR (14043, 16, -1, 'The synchronization object')
                    RETURN (1)
                END

            IF @value LIKE '%.%.%' OR @value LIKE '%.%'
                BEGIN
                    EXECUTE sp_namecrack @value,
                                         @site OUTPUT,
                                         @db OUTPUT,
                                         @owner OUTPUT,
                                         @object OUTPUT

                    EXECUTE @retcode = sp_validname @object

                    IF @@ERROR <> 0 OR @retcode <> 0
			RETURN (1)

                END

            ELSE
                BEGIN

                    EXECUTE @retcode = sp_validname @value

                    IF @@ERROR <> 0 OR @retcode <> 0
			RETURN (1)
                END

            SELECT @sync_objid = OBJECT_ID(@value)
            IF @sync_objid IS NULL
                BEGIN
                    RAISERROR (15001, 11, -1, @value)
                    RETURN (1)
                END

            IF NOT EXISTS (SELECT *
                             FROM sysobjects
                            WHERE type IN ('U', 'V')
                              AND id = @sync_objid)

                BEGIN
                    RAISERROR (14031, 16, -1)
                    RETURN (1)
                END

            /*
            ** Update the article with the new synchronization object.
            */

            UPDATE sysarticles
               SET sync_objid = @sync_objid
             WHERE artid = @artid
               AND pubid = @pubid

            IF @@ERROR <> 0 RETURN (1)

        END

    IF LOWER(@property) = 'type'
        BEGIN

            /*
            ** Check to make sure that we have a valid type.
            */

	    IF LOWER(@value) NOT IN ('logbased', 'logbased manualfilter', 'logbased manualview', 'logbased manualboth')
                BEGIN
                    RAISERROR (14023, 16, -1)
                    RETURN (1)
                END

            /*
            ** Determine the integer value for the type.
            */
	    IF LOWER(@value) = 'logbased'
		SELECT @typeid = 1
	    ELSE IF LOWER(@value) = 'logbased manualfilter'
		SELECT @typeid = 3
	    ELSE IF LOWER(@value) = 'logbased manualview'
		SELECT @typeid = 5
	    ELSE IF LOWER(@value) = 'logbased manualboth'
		SELECT @typeid = 7

            /*
            ** Update the article with the new type.
            */

            UPDATE sysarticles
               SET type = @typeid
             WHERE artid = @artid
               AND pubid = @pubid

            IF @@ERROR <> 0 RETURN (1)

        END

    IF LOWER(@property) = 'filter'
        BEGIN

            /*
            ** Check for a valid filter value.
            */

            IF @value IS NOT NULL
                BEGIN

                    IF @value LIKE '%.%.%' OR @value LIKE '%.%'
                        BEGIN
                            EXECUTE sp_namecrack @value,
                                                 @site OUTPUT,
                                                 @db OUTPUT,
                                                 @owner OUTPUT,
                                                 @object OUTPUT

                            EXECUTE @retcode = sp_validname @object

                            IF @@ERROR <> 0 OR @retcode <> 0
				RETURN (1)

                        END

                    ELSE
                        BEGIN

                            EXECUTE @retcode = sp_validname @value

                            IF @@ERROR <> 0 OR @retcode <> 0
				RETURN (1)
                        END
                END

            SELECT @filter = OBJECT_ID(@value)

            IF @value IS NOT NULL
                BEGIN

                    IF @filter IS NULL
                        BEGIN
                            RAISERROR (15001, 11, -1, @value)
                            RETURN (1)
                        END

                    IF NOT EXISTS (SELECT *
                                     FROM sysobjects
                                    WHERE type = 'RF'
                                      AND id = @filter)

                        BEGIN
                            RAISERROR (14049, 16, -1)
                            RETURN (1)
                        END

                END

            IF @value IS NULL SELECT @filter = 0

            /*
            ** Update the article with the new filter.
            */

            UPDATE sysarticles
               SET filter = @filter
             WHERE artid = @artid
               AND pubid = @pubid

            IF @@ERROR <> 0 RETURN (1)

        END

    IF LOWER(@property) = 'pre_creation_cmd'
        BEGIN

            /*
            ** Check to make sure that we have a valid pre_creation_cmd.
            */

            IF LOWER(@value) NOT IN ('none', 'drop', 'delete', 'truncate')
                BEGIN
                    RAISERROR (14061, 16, -1)
                    RETURN (1)
                END

            /*
            ** Determine the integer value for the type.
            */

            IF LOWER(@value) = 'none'
    	        SELECT @precmdid = 0
            ELSE IF LOWER(@value) = 'drop'
    	        SELECT @precmdid = 1
            ELSE IF LOWER(@value) = 'delete'
    	        SELECT @precmdid = 2
            ELSE IF LOWER(@value) = 'truncate'
    	        SELECT @precmdid = 3

            /*
            ** Update the article with the new pre_creation_cmd.
            */
            UPDATE sysarticles
               SET pre_creation_cmd = @precmdid
             WHERE artid = @artid
               AND pubid = @pubid

            IF @@ERROR <> 0 RETURN (1)

        END

	IF LOWER(@property) = 'status'
        BEGIN
            /*
            ** Check to make sure that we have a valid type.
            */

            IF LOWER(@value) NOT IN ('not owner qualified', 'owner qualified', 'no column names', 'include column names' )
                BEGIN
                    RAISERROR (14097, 16, -1)
                    RETURN (1)
                END

            /*
            ** Determine the integer value for the type.
            */
		IF LOWER(@value) = 'not owner qualified'
			UPDATE sysarticles 
			SET status = status & ~4
			WHERE artid = @artid
                          AND pubid = @pubid

		ELSE IF LOWER(@value) = 'owner qualified'
		    UPDATE sysarticles 
			SET status = status | 4
			WHERE artid = @artid
                          AND pubid = @pubid
			 
		ELSE IF LOWER(@value) = 'no column names'
		    UPDATE sysarticles 
			SET status = status & ~8
			WHERE artid = @artid
                          AND pubid = @pubid
			 
		ELSE IF LOWER(@value) = 'include column names'
		    UPDATE sysarticles 
			SET status = status | 8
			WHERE artid = @artid
                          AND pubid = @pubid
			 
        IF @@ERROR <> 0 RETURN (1)

	END

    /*
    ** Force the article cache to be refreshed with the new definition.
    */
    EXECUTE sp_replflush

    /*
    ** Return succeed.
    */

    RAISERROR (14025, 10, -1)
    RETURN (0)
go

print ''
print 'Creating procedure sp_changesubscriber.'
go
CREATE PROCEDURE sp_changesubscriber (
	@subscriber varchar (30),
	@type tinyint = 0,
	@login varchar (30) = NULL,
	@password varchar (30) = NULL,
	@commit_batch_size int = NULL,
	@status_batch_size int = NULL,
	@flush_frequency int = NULL,
	@frequency_type int = NULL,
	@frequency_interval int = NULL,
	@frequency_relative_interval int = NULL,
	@frequency_recurrence_factor int = NULL,
	@frequency_subday int = NULL,
	@frequency_subday_interval int = NULL,
	@active_start_time_of_day int = NULL,
	@active_end_time_of_day int = NULL,
	@active_start_date int = NULL,
	@active_end_date int = NULL,
	@description varchar (255) = NULL
        ) AS

    DECLARE @distributor varchar(30)
    DECLARE @distribdb varchar(30)
    DECLARE @distproc varchar (255)
    DECLARE @msg varchar(255)
    DECLARE @retcode int

    /*
    ** Check to make sure that the subscriber doesn't already exist.
    */
    IF NOT EXISTS (SELECT *
                 FROM master..sysservers
                WHERE srvname = @subscriber)
        BEGIN
            SELECT @msg = 'The server ''' + @subscriber +
		''' is not a valid subscriber.'
            PRINT @msg
            RETURN (0)
        END

    /*
    ** Get distribution server information for remote RPC
    ** subscription calls.
    */
    EXEC @retcode = sp_helpdistributor @distributor = @distributor OUTPUT,
        @distribdb = @distribdb OUTPUT
    IF @@ERROR <> 0 OR @retcode <> 0
        BEGIN
    	     PRINT 'Unable to update distribution database MSsubscriberinfo table.'
    	     RETURN (1)
	 END

    /*
    ** Update MSsubscriber_info
    */
    SELECT @distproc = RTRIM(@distributor) + '.' +
        RTRIM(@distribdb) + '..sp_MSupdate_subscriber_info '
    EXEC @retcode = @distproc
        @@SERVERNAME,
        @subscriber,
	@type,
	@login,
	@password,
	@commit_batch_size,
	@status_batch_size,
	@flush_frequency,
	@frequency_type,
	@frequency_interval,
	@frequency_relative_interval,
	@frequency_recurrence_factor,
	@frequency_subday,
	@frequency_subday_interval,
	@active_start_time_of_day,
	@active_end_time_of_day,
	@active_start_date,
	@active_end_date,
	@description = @description
    IF @@ERROR <> 0 OR @retcode <> 0
        BEGIN
            SELECT @msg = 'The server ''' + @subscriber +
		''' is not a valid subscriber.'
            PRINT @msg
	    RETURN (1)
	END
go

print ''
print 'Creating procedure sp_distcounters'
go
CREATE PROCEDURE sp_distcounters
    AS

    SET NOCOUNT ON

    /*
    ** Declarations.
    */
    DECLARE @distributor varchar(30)
    DECLARE @distribdb varchar(30)
    DECLARE @distproc varchar (255)
    DECLARE @retcode int

    /*
    ** Get distribution server information for remote RPC
    ** subscription calls.  If no distribution information, assume
    ** replication is not being used.
    */
    EXEC @retcode = sp_helpdistributor @distributor = @distributor OUTPUT,
                                       @distribdb = @distribdb OUTPUT
    IF @@error <> 0 OR @retcode <> 0 OR @distributor IS NULL OR
       @distribdb IS NULL
	RETURN (1)

    /*
    ** Request counters from Distribution Server
    */
    SELECT @distproc = RTRIM(@distributor) + '.' + RTRIM(@distribdb) +
	'..sp_MSdistribution_counters '
    EXEC @retcode = @distproc @@SERVERNAME
go

print ''
print 'Creating procedure sp_droparticle.'
go

CREATE PROCEDURE sp_droparticle(
        @publication varchar(30),     /* The publication name */
        @article varchar(30)          /* The article name */

        ) AS

    /*
    ** Declarations.
    */

    DECLARE @cmd varchar(255)
    DECLARE @objid int
    DECLARE @pubid int
    DECLARE @publish_bit smallint
    DECLARE @retcode int
    DECLARE @filter_name varchar (30)
    DECLARE @view_name varchar (30)
    DECLARE @type tinyint

    /*
    ** Initializations.
    */

    SELECT @publish_bit = 32    /* Const: publishing server bit */

    /*
    ** Security Check.
    ** Only the System Administrator (SA) or the Database Owner (dbo) can
    ** drop an article from a publication.
    */
    IF suser_id() <> 1 AND user_id() <> 1
	BEGIN
            RAISERROR (15000, 14, -1)
	    RETURN (1)
        END

    /*
    ** Get the @pubid.
    */

    SELECT @pubid = pubid FROM syspublications WHERE name = @publication

    /*
    ** Parameter Check:  @article.
    ** If the @article is 'all', drop all articles for the specified
    ** publication (@publication).
    */

    IF LOWER(@article) = 'all'
        BEGIN
            SELECT @cmd = ''
            SELECT @cmd = @cmd + 'DECLARE hC SCROLL CURSOR FOR '
            SELECT @cmd = @cmd + ' SELECT DISTINCT name '
            SELECT @cmd = @cmd + '   FROM sysarticles '
            SELECT @cmd = @cmd + '  WHERE pubid = ' + CONVERT(varchar(10), @pubid)
            EXECUTE (@cmd)
            OPEN hC
            FETCH hC INTO @article
            WHILE (@@fetch_status <> -1)
                BEGIN
                    EXECUTE sp_droparticle @publication, @article
                    FETCH hC INTO @article
                END
            CLOSE hC
            DEALLOCATE hC
            RETURN (0)
        END

    /*
    ** Parameter Check: @article.
    ** The @article name must conform to the rules for identifiers.
    */

    IF @article IS NULL
        BEGIN
            RAISERROR (14043, 16, -1, 'The article')
            RETURN (1)
        END

    EXECUTE @retcode = sp_validname @article

    IF @retcode <> 0
	RETURN (1)

    /*
    ** Parameter Check: @publication.
    ** The @publication name must conform to the rules for identifiers.
    */

    IF @publication IS NULL
        BEGIN
            RAISERROR (14043, 16, -1, 'The publication')
            RETURN (1)
        END

    EXECUTE @retcode = sp_validname @publication

    IF @retcode <> 0
	RETURN (1)

    IF NOT EXISTS (SELECT * FROM syspublications WHERE name = @publication)
        BEGIN
            RAISERROR (15001, 11, -1, @publication)
            RETURN (1)
        END

    /*
    ** Ascertain the existence of the article.
    */

    IF NOT EXISTS (SELECT *
                     FROM sysarticles
                    WHERE name = @article
                      AND pubid = @pubid)
        BEGIN
            RAISERROR (15001, 11, -1, @article)
	    RETURN (1)
        END

    /*
    ** Check to make sure that there are no subscriptions on the article.
    */

    IF EXISTS (SELECT *
                 FROM syssubscriptions, sysarticles
                WHERE sysarticles.name = @article
                  AND sysarticles.pubid = @pubid
                  AND sysarticles.artid = syssubscriptions.artid)
        BEGIN
            RAISERROR (14046, 16, -1)
            RETURN (1)
        END

    /*
    **  Delete article from sysarticles and clear publish bit in
    **  sysobjects.
    */

    BEGIN TRAN droparticle

        /*
        ** Retrieve the object id of the underlying table.
        */

        SELECT @objid = objid, @type = type
          FROM sysarticles
         WHERE name = @article
           AND pubid = @pubid

        /*
        ** If this article is the only one that references this object,
        ** then we can safely turn off the publish bit in sysobjects.
        */

        IF NOT EXISTS (SELECT *
                         FROM sysarticles
                        WHERE objid = @objid
                          AND NOT (name = @article AND pubid = @pubid))
            BEGIN
                UPDATE sysobjects SET category = category & ~@publish_bit
	         WHERE id = (SELECT objid
	        	       FROM sysarticles
                              WHERE name = @article
                                AND pubid = @pubid)

	        IF @@ERROR <> 0
	            BEGIN
	        	ROLLBACK TRAN
                        RAISERROR (14047, 16, -1, 'the article')
	        	RETURN (1)
	            END
            END

        /*
        ** Drop article view if not logbased manualview (type = 5)
        */
	IF (@type & 5) = 1
	   BEGIN	
		SELECT @view_name = sysobjects.name
			FROM sysarticles, sysobjects
		 WHERE sysarticles.name = @article
		   AND pubid = @pubid
		   AND sync_objid = sysobjects.id
		   AND sysobjects.type = 'V'
	   END

        /*
        ** Drop article filter if not logbased manualfilter (type = 3)
        */
	IF (@type & 3) = 1
	   BEGIN	
		SELECT @filter_name = sysobjects.name
			FROM sysarticles, sysobjects
		 WHERE sysarticles.name = @article
		   AND pubid = @pubid
		   AND filter = sysobjects.id
		   AND sysobjects.type = 'RF'

	   END

	/*
	** Drop all article columns.  This is done to force all Text\Image
	** column status to be updated.
	*/
	EXECUTE @retcode  = sp_articlecolumn @publication, @article,
	   @operation = 'drop'
        IF @@ERROR <> 0 OR @retcode <> 0
	   BEGIN
	      ROLLBACK TRAN droparticle
	      RETURN (1)
	   END

        /*
        ** Remove the row from sysarticles.
        */
    	DELETE
          FROM sysarticles
         WHERE name = @article
           AND pubid = @pubid

	IF @@ERROR <> 0
	    BEGIN
	  	ROLLBACK TRAN
                RAISERROR (14047, 16, -1, 'the article')
		RETURN (1)
	    END

    COMMIT TRAN droparticle

    IF @view_name IS NOT NULL
       exec ('drop view ' + @view_name)

    IF @filter_name IS NOT NULL
       exec ('drop procedure ' + @filter_name)

    /*
    ** Force the article cache to be refreshed.
    */
    EXECUTE sp_replflush
go

print ''
print 'Creating procedure sp_droppublication.'
go
CREATE PROCEDURE sp_droppublication(
        @publication varchar(30)       /* The publication name */
        ) AS

    /*
    ** Declarations.
    */

    DECLARE @article varchar(30)
    DECLARE @cmd varchar(255)
    DECLARE @retcode int
    DECLARE @taskid int
    DECLARE @distributor varchar(30)
    DECLARE @distribdb varchar(30)
    DECLARE @distproc varchar (255)

    /*
    ** Only the System Administrator (SA) or the Database Owner (dbo) can drop
    ** a publication.
    */

    IF suser_id() <> 1 AND user_id() <> 1
	BEGIN
            RAISERROR (15000, 14, -1)
	    RETURN (1)
        END

    /*
    ** Parameter Check:  @publication.
    ** If the @publication is 'all', drop all publications.  Otherwise,
    ** make sure the @publication is a valid non-null identifier.
    */

    IF LOWER(@publication) = 'all'
        BEGIN
            SELECT @cmd = ''
            SELECT @cmd = @cmd + 'DECLARE hC1 SCROLL CURSOR FOR '
            SELECT @cmd = @cmd + ' SELECT DISTINCT name '
            SELECT @cmd = @cmd + '   FROM syspublications '
            SELECT @cmd = @cmd + '  WHERE pubid NOT IN '
            SELECT @cmd = @cmd + '(SELECT pubid FROM sysarticles WHERE artid IN '
            SELECT @cmd = @cmd + '(SELECT artid FROM syssubscriptions))'
            EXECUTE (@cmd)
            OPEN hC1
            FETCH hC1 INTO @publication
            WHILE (@@fetch_status <> -1)
                BEGIN
                    EXECUTE sp_droppublication @publication
                    FETCH hC1 INTO @publication
                END
            CLOSE hC1
            DEALLOCATE hC1
            RETURN (0)
        END

    IF @publication IS NULL
        BEGIN
            RAISERROR (14003, 16, -1)
            RETURN (1)
        END

    EXECUTE @retcode = sp_validname @publication

    IF @retcode <> 0
	RETURN (1)

    /*
    ** Ascertain the existence of the publication and get the taskid.
    */
    SELECT @taskid = taskid
                     FROM syspublications
                    WHERE name = @publication
    IF @taskid IS NULL
        BEGIN
            RAISERROR (15001, 11, -1, @publication)
	    RETURN (1)
        END

    /*
    ** Check to make sure that there are no subscriptions on the publication.
    */

    IF EXISTS (SELECT *
                 FROM syssubscriptions a, sysarticles b, syspublications c
                WHERE c.name = @publication
                  AND c.pubid = b.pubid
                  AND b.artid = a.artid)
        BEGIN
            RAISERROR (14005, 16, -1)
            RETURN (1)
        END

    /*
    ** Delete all articles from the publication.
    */

    SELECT @cmd = ''
    SELECT @cmd = @cmd + 'DECLARE hC2 SCROLL CURSOR FOR '
    SELECT @cmd = @cmd + ' SELECT DISTINCT name '
    SELECT @cmd = @cmd + '   FROM sysarticles '
    SELECT @cmd = @cmd + '  WHERE pubid = (SELECT pubid '
    SELECT @cmd = @cmd + '   FROM syspublications '
    SELECT @cmd = @cmd + '  WHERE name = ''' + @publication + ''')'
    EXECUTE (@cmd)
    OPEN hC2
    FETCH hC2 INTO @article
    WHILE (@@fetch_status <> -1)
        BEGIN
            EXECUTE sp_droparticle @publication, @article
            FETCH hC2 INTO @article
        END
    CLOSE hC2
    DEALLOCATE hC2

    /*
    ** Delete publication from syspublications.
    */

    DELETE
      FROM syspublications
     WHERE name = @publication

    IF @@ERROR <> 0
        BEGIN
            RAISERROR (14006, 16, -1)
	    RETURN (1)
        END

    /*
    ** Get distribution server information for remote RPC call.
    */
    EXEC @retcode = sp_helpdistributor @distributor = @distributor OUTPUT,
                                       @distribdb = @distribdb OUTPUT

    IF @@ERROR <> 0 OR  @retcode <> 0
        BEGIN
            RAISERROR (14071, 16, -1)
            RETURN (1)
        END

    /*
    ** Delete sync task of Publication.
    */
    SELECT @distproc = RTRIM(@distributor) + '.msdb..sp_droptask'
    EXECUTE @distproc @id = @taskid
go


print ''
print 'Creating procedure sp_droppublisher.'
go
CREATE PROCEDURE sp_droppublisher (
	@publisher varchar (30),   	  /* publisher server name */
	@type varchar (5) = NULL	  /* NULL or 'dist' */
        ) AS

    DECLARE @distaccount varchar(127)
    DECLARE @proc varchar (255)
    DECLARE @retcode int
    DECLARE @privilege varchar (30)

    /*
    ** Parameter Check:  @publisher.
    ** Check to make sure that the publisher exists, that the name isn't
    ** NULL, and that the name conforms to the rules for identifiers.
    */

    IF @publisher IS NULL
        BEGIN
            RAISERROR (14043, 16, -1, 'The publisher')
            RETURN (1)
        END

    EXECUTE @retcode = sp_validname @publisher

    IF @retcode <> 0
	RETURN (1)

    /*
    ** Perform special logic if dropping a publisher for a distribution
    ** server.
    */
    IF LOWER(@type) = 'dist'
    	BEGIN
	    IF NOT EXISTS (SELECT *
		FROM master..sysservers
                WHERE srvname = @publisher
                AND srvstatus & 16 <> 0)

	    BEGIN
		RAISERROR (14080, 11, -1)
	        RETURN (1)
	    END

	    EXECUTE @retcode = sp_serveroption @publisher, 'dpub', false
	    IF @@ERROR <> 0 OR @retcode <> 0 RETURN (1)

            IF EXISTS (SELECT * FROM master..sysremotelogins
	       WHERE remoteserverid = (SELECT srvid FROM master..sysservers
	       WHERE srvname = @publisher)
	       AND remoteusername = 'sa'
	       AND suid = 1) 	/* 'sa' */
	    BEGIN
	       EXECUTE @retcode = sp_dropremotelogin @publisher, sa, sa
	       IF @@ERROR <> 0 OR @retcode <> 0 RETURN (1)
	    END

            IF EXISTS (SELECT * FROM master..sysremotelogins
	       WHERE remoteserverid = (SELECT srvid FROM master..sysservers
	       WHERE srvname = @publisher)
	       AND remoteusername = 'probe'
	       AND suid = 10) 	/* 'probe' */
	    BEGIN
	       EXECUTE @retcode = sp_dropremotelogin @publisher, probe, probe
	       IF @@ERROR <> 0 OR @retcode <> 0 RETURN (1)
	    END

	    RETURN (0)
	END

    /*
    ** Make sure the server is defined as a 'publisher'.
    */
    IF NOT EXISTS (SELECT *
                     FROM master..sysservers
                    WHERE srvname = @publisher
                      AND srvstatus & 2 <> 0)

        BEGIN
            RAISERROR (14080, 11, -1)
            RETURN (1)
        END

    /*
    ** Turn off the server option to indicate that this is a publisher.
    */
    EXECUTE @retcode = sp_serveroption @publisher, 'pub', false
    IF @@ERROR <> 0 OR @retcode <> 0 RETURN (1)

    /*
    ** Fetch the publisher's distributor account.
    */

    SELECT @proc = RTRIM(@publisher) + '.master..sp_helpdistributor '
    EXEC @retcode = @proc @account = @distaccount OUTPUT
    IF @@ERROR <> 0 OR @retcode <> 0
        BEGIN
            RAISERROR (14071, 16, -1)
            RETURN (1)
        END

    /*
    ** If @distaccount = 'LocalSystem' assume 'admin' privilege
    */
    IF @distaccount = 'LocalSystem'
       RETURN (0)

    /*
    ** If @distaccount has 'admin' privilege, do not revoke
    */
    EXECUTE @retcode = master.dbo.xp_logininfo @distaccount, 'all',
       @privilege = @privilege output
    IF @@error <> 0 OR @retcode <> 0 RETURN (1)

    IF @privilege = 'admin'
       RETURN  (0)

    /*
    ** Revoke replication privilege to the distributor NT account.
    */
    EXEC @retcode = master.dbo.xp_revokelogin @distaccount
    IF @@ERROR <> 0 OR @retcode <> 0 RETURN (1)
go

print ''
print 'Creating procedure sp_dropsubscriber.'
go
CREATE PROCEDURE sp_dropsubscriber (
	@subscriber varchar (30)        /* The name of the subscriber */
        ) AS

    SET NOCOUNT ON

    /*
    ** Declarations.
    */

    DECLARE @retcode int

    /*
    ** Security Check.
    ** Only the System Administrator (SA) or the Database Owner (dbo) can
    ** drop a subscriber
    */
    IF suser_id() <> 1 AND user_id() <> 1
	BEGIN
            RAISERROR (15000, 14, -1)
	    RETURN (1)
        END

    /*
    ** Parameter Check:  @subscriber.
    ** Check to make sure that the subscriber exists.
    */

    IF @subscriber IS NULL
        BEGIN
            RAISERROR (14043, 16, -1, 'The subscriber')
            RETURN (1)
        END

    EXECUTE @retcode = sp_validname @subscriber

    IF @retcode <> 0
	RETURN (1)

    IF NOT EXISTS (SELECT *
                     FROM master..sysservers
                    WHERE srvname = @subscriber
                      AND srvstatus & 4 <> 0)

        BEGIN
            RAISERROR (14048, 16, -1, @subscriber)
            RETURN (1)
        END

    /*
    ** Drop the remote logins associated with this server.
    */

    IF EXISTS (SELECT * FROM master..sysremotelogins
	WHERE remoteserverid = (SELECT srvid FROM master..sysservers
		   WHERE srvname = @subscriber)
	   AND remoteusername = 'sa'
	   AND suid = 16383) 	/* 'repl_subscriber' */
	BEGIN
	    EXECUTE @retcode = sp_dropremotelogin @subscriber,
		'repl_subscriber', 'sa'
	    IF @@ERROR <> 0
               BEGIN
		   ROLLBACK TRANSACTION
                   RAISERROR (14047, 16, -1, @subscriber)
                   RETURN (1)
               END

	    IF @retcode <> 0
		BEGIN
		   ROLLBACK TRANSACTION
		   RETURN (1)
		END
	END

    /*
    ** Turn off the subscriber server option.
    */
    EXECUTE @retcode = sp_serveroption @subscriber, 'sub', false
    IF @@ERROR <> 0
        BEGIN
           ROLLBACK TRANSACTION
           RAISERROR (14047, 16, -1, @subscriber)
           RETURN (1)
        END

    IF @retcode <> 0
        BEGIN
           ROLLBACK TRANSACTION
           RETURN (@retcode)
        END

    RAISERROR (14062, 10, -1)
go

print ''
print 'Creating procedure sp_dropsubscription.'
go
CREATE PROCEDURE sp_dropsubscription (
	@publication varchar (30) = NULL,   /* The publication name */
	@article varchar (30) = NULL,       /* The article name */
	@subscriber varchar (30)            /* The subscriber name */
	) AS

    /*
    ** Declarations.
    */

    DECLARE @subscriber_bit smallint
    DECLARE @cmd varchar(255)
    DECLARE @srvid smallint
    DECLARE @artid int
    DECLARE @retcode int
    DECLARE @active tinyint

    /*
    ** Initializations.
    */
    SET NOCOUNT ON
    SELECT @subscriber_bit = 4  /* Const: subscription server status */
    SELECT @active = 2          /* Const: subscription status 'active' */

    /*
    ** Security Check.
    ** Only the System Administrator (SA) or the Database Owner (dbo) can
    ** add an article to a publication.
    */

    IF suser_id() <> 1 AND user_id() <> 1
	BEGIN
            RAISERROR (15000, 14, -1)
	    RETURN (1)
        END

    /*
    ** If the @subscriber is 'all', the user wants to cancel all subscriptions
    ** to the specified article(s).
    */

    IF LOWER(@subscriber) = 'all'
        BEGIN
            SELECT @cmd = ''
            SELECT @cmd = @cmd + 'DECLARE hC1 SCROLL CURSOR FOR '
            SELECT @cmd = @cmd + ' SELECT DISTINCT srvname '
            SELECT @cmd = @cmd + '   FROM master..sysservers a, syssubscriptions b '
            SELECT @cmd = @cmd + '  WHERE srvstatus & ' + CONVERT(char(1), @subscriber_bit) + ' <> 0 '
            SELECT @cmd = @cmd + '    AND a.srvid = b.srvid '
            EXECUTE (@cmd)
            OPEN hC1
            FETCH hC1 INTO @subscriber
            WHILE (@@fetch_status <> -1)
                BEGIN
                    EXECUTE sp_dropsubscription @publication = 'all',
			                        @subscriber  = @subscriber
                    FETCH hC1 INTO @subscriber
                END
            CLOSE hC1
            DEALLOCATE hC1
            RETURN (0)
        END

    /*
    ** Parameter Check: @subscriber.
    ** Check if the server exists and that it is a subscription server.
    */

    IF @subscriber IS NULL
        BEGIN
            RAISERROR (14043, 16, -1, 'The subscriber')
            RETURN (1)
        END

    EXECUTE @retcode = sp_validname @subscriber

    IF @retcode <> 0
	RETURN (1)

    SELECT @srvid = srvid
      FROM sysservers
     WHERE srvname = @subscriber
       AND (srvstatus & @subscriber_bit) <> 0

    IF @srvid IS NULL
        BEGIN
            RAISERROR (14010, 16, -1)
    	    RETURN (1)
        END

    /*
    ** If the @publication is 'all', the user wants to cancel all subscriptions
    ** for all publications associated with the specified @subscriber.
    */

    IF LOWER(@publication) = 'all'
        BEGIN
            SELECT @cmd = ''
            SELECT @cmd = @cmd + 'DECLARE hC2 SCROLL CURSOR FOR '
            SELECT @cmd = @cmd + ' SELECT DISTINCT a.name '
            SELECT @cmd = @cmd + '   FROM syspublications a, '
            SELECT @cmd = @cmd + '        sysarticles b, '
            SELECT @cmd = @cmd + '        syssubscriptions c '
            SELECT @cmd = @cmd + '  WHERE c.srvid = ' + CONVERT(char(10), @srvid)
            SELECT @cmd = @cmd + '    AND a.pubid = b.pubid '
            SELECT @cmd = @cmd + '    AND b.artid = c.artid '
            EXECUTE (@cmd)
            OPEN hC2
            FETCH hC2 INTO @publication
            WHILE (@@fetch_status <> -1)
                BEGIN
                    EXECUTE sp_dropsubscription @publication = @publication,
                                                @article     = 'all',
			                        @subscriber  = @subscriber
                    FETCH hC2 INTO @publication
                END
            CLOSE hC2
            DEALLOCATE hC2
            RETURN (0)
        END

    /*
    ** Parameter Check: @publication.
    ** Check to make sure that the publication exists and that it conforms
    ** to the rules for identifiers.
    */

    IF @publication IS NULL
        BEGIN
            RAISERROR (14043, 16, -1, 'The publication')
            RETURN (1)
        END

    EXECUTE @retcode = sp_validname @publication

    IF @retcode <> 0
	RETURN (1)

    IF NOT EXISTS (SELECT * FROM syspublications WHERE name = @publication)
        BEGIN
            RAISERROR (15001, 11, -1, @publication)
            RETURN (1)
        END

    /*
    ** If the @article is 'all', the user wants to cancel all
    ** subscriptions on this publisher associated with the given @subscriber
    ** and @publication.
    */

    IF LOWER(@article) = 'all'
        BEGIN
            SELECT @cmd = ''
            SELECT @cmd = @cmd + 'DECLARE hC3 SCROLL CURSOR FOR '
            SELECT @cmd = @cmd + 'SELECT DISTINCT art.name '
            SELECT @cmd = @cmd + 'FROM sysarticles art, '
            SELECT @cmd = @cmd + 'syssubscriptions sub, '
            SELECT @cmd = @cmd + 'syspublications pub '
            SELECT @cmd = @cmd + 'WHERE sub.srvid = ' + CONVERT(char(10), @srvid)
            SELECT @cmd = @cmd + 'AND sub.artid = art.artid '
            SELECT @cmd = @cmd + 'AND art.pubid = pub.pubid '
            SELECT @cmd = @cmd + 'AND pub.name = ''' + @publication + ''''
            EXECUTE (@cmd)
            OPEN hC3
            FETCH hC3 INTO @article
            WHILE (@@fetch_status <> -1)
                BEGIN
                    EXECUTE sp_dropsubscription @publication,
                                                @article,
			                        @subscriber
                    FETCH hC3 INTO @article
                END
            CLOSE hC3
            DEALLOCATE hC3
            RETURN (0)
        END

    /*
    ** Parameter Check: @article
    ** Check if the article exists.
    */

    IF @article IS NULL
        BEGIN
            RAISERROR (14043, 16, -1, 'The article')
            RETURN (1)
        END

    EXECUTE @retcode = sp_validname @article

    IF @retcode <> 0
	RETURN (1)

    SELECT @artid = artid
      FROM sysarticles art, syspublications pub
     WHERE pub.name = @publication
       AND art.name = @article
       AND art.pubid = pub.pubid

    IF @artid IS NULL
        BEGIN
            RAISERROR (15001, 11, -1, @article)
	    RETURN (1)
        END

    /*
    ** Check if the subscription exists.
    */

    IF NOT EXISTS (SELECT *
                     FROM syssubscriptions
                    WHERE srvid = @srvid
                      AND artid = @artid)
	BEGIN
            RAISERROR (14055, 11, -1, @article, @publication, @subscriber)
            RETURN (1)
	END

    BEGIN TRANSACTION dropsubscription

        /*
        ** Change the status of the subscription to 'inactive'.
        */

        EXECUTE @retcode = sp_changesubstatus @publication,
                                              @article,
		                              @subscriber,
                                              @status = 'inactive'
	    IF @@ERROR <> 0 OR @retcode <> 0
	    BEGIN
	        ROLLBACK TRANSACTION dropsubscription
	        RETURN (1)
	    END
        /*
        ** Remove subscription from syssubscriptions.
        */

	DELETE syssubscriptions
	 WHERE artid = @artid
	   AND srvid = @srvid

	IF @@ERROR <> 0
	    BEGIN
		ROLLBACK TRANSACTION dropsubscription
		RAISERROR (14055, 16, -1, @article, @publication, @subscriber)
		RETURN (1)
	    END

    COMMIT TRANSACTION dropsubscription


go

print ''
print 'Creating procedure sp_dsninfo.'
go
CREATE PROCEDURE sp_dsninfo
        @dsn varchar (30),
	@infotype varchar (30) = NULL,
	@login varchar (30) = NULL,
	@password varchar (30) = NULL
	AS

    SET NOCOUNT ON

    DECLARE @distributor varchar(30)
    DECLARE @distproc varchar (255)
    DECLARE @retcode int

    /*
    ** Get distribution server information for remote RPC
    ** subscription calls.
    */

    EXEC @retcode = sp_helpdistributor @distributor = @distributor OUTPUT
    IF @@error <> 0 OR @retcode <> 0
        BEGIN
	    RAISERROR (14071, 16, -1)
    	    RETURN (1)
	END

    /*
    ** Call xp_dsninfo
    */
    SELECT @distproc = RTRIM(@distributor) + '.master..xp_dsninfo '
    EXEC @retcode = @distproc @dsn, @infotype, @login, @password
    IF @@error <> 0
        BEGIN
	    RAISERROR (14071, 16, -1)
	    RETURN (1)
	END

go

print ''
print 'Creating procedure sp_publishdb.'
go
CREATE PROCEDURE sp_publishdb @dbname varchar(30),@value varchar (5)
	AS

    DECLARE @retcode int
    DECLARE @distributor varchar(30)
    DECLARE @distribdb varchar(30)
    DECLARE @distproc varchar (255)
    DECLARE @taskname varchar (40)

    /*
    ** Construct Log Reader task name.
    */
    select @taskname = @@SERVERNAME + '_' + @dbname

    /*
    ** Get distribution server information for remote RPC call.
    */
    EXECUTE @retcode = sp_helpdistributor @distributor = @distributor OUTPUT,
       @distribdb   = @distribdb OUTPUT

    IF @@ERROR <> 0 or @retcode <> 0
        BEGIN
           IF LOWER(@value) = 'true'
	     RAISERROR (14036, 16, -1)
	   ELSE
	     RAISERROR (14038, 16, -1)
	   RETURN (1)
	END

    /*
    ** Enable the database for publishing.
    */
    IF @value = 'true'
	begin
	    /*
	    ** Check if the database has already been enabled.
	    */
	    IF EXISTS (SELECT * FROM sysdatabases
	    	WHERE name = @dbname
	    	AND (category & 1) <> 0)
			BEGIN
				RAISERROR (14035, 10, -1, @dbname)
				RETURN (0)
			END

	    /*
	    ** Schedule Log Reader task for the database
	    */
	    SELECT @distproc = RTRIM(@distributor) + '.msdb..sp_addtask'
	    EXECUTE @retcode = @distproc
		@taskname,
		@subsystem = 'LogReader',
		@server = @@SERVERNAME,
		@databasename = @dbname,
		@freqtype = 64,		/* Auto-Start */
		@retryattempts = 1440,	/* Every minute for 24 hours */
		@retrydelay = 1,	
		@command = '-b100 -c100 -i1000 -t100',
		@enabled = 1,
		@loghistcompletionlevel = 0
	    IF @@ERROR <> 0 or @retcode <> 0
			BEGIN
			    RETURN (1)
			END

	    /*
            ** Add the repl_subscriber user account to the database.
	    */
	    IF NOT EXISTS (SELECT * FROM sysusers WHERE name = 'repl_subscriber')
	        EXECUTE sp_adduser 'repl_subscriber'
	END

else	/* Disable the database for publishing. */
	begin
  	    /*
	    ** Check if the database is enabled for publishing.
    	    */
	    IF NOT EXISTS (SELECT * FROM sysdatabases
    		WHERE name = @dbname
    		AND (category & 1) <> 0)
			BEGIN
				RAISERROR (14013, 10, -1)
				return (0)
			END
            /*
    	    ** Delete logreader task, continue if drop is not successful
    	    */
            SELECT @distproc = RTRIM(@distributor) + '.msdb..sp_droptask'
    	    EXECUTE @distproc @name = @taskname

	    /*
    	    ** Remove all subscriptions in the database.
       	    */
	    EXEC sp_dropsubscription @publication = 'all',
	        @article = 'all', @subscriber = 'all'

    	    /*
    	    ** Remove all publications and articles in the database.
    	    */
	    EXEC sp_droppublication @publication = 'all'

	    /*
	    ** Remove all published database jobs from the distribution
	    ** database.
	    */
            SELECT @distproc = RTRIM(@distributor) + '.' +
                RTRIM(@distribdb) + '..sp_MSremove_published_jobs '
            EXEC @distproc @@SERVERNAME, @dbname

        /*
        ** Remove the repl_subscriber user account from the database.
        */

	    IF EXISTS (SELECT * FROM sysusers WHERE name = 'repl_subscriber')
	       exec sp_dropuser 'repl_subscriber'

        /*
        ** Publishing shutdown, remove all xacts pending distribution
        */

		/* ensure we can get in as logreader */

		EXEC sp_replflush

		/* unmark all xacts marked for replication */

	    EXEC ( "USE " + @dbname + " exec sp_repldone 0, 0, NULL, 0, 0, 1" )
	    EXEC ( "USE " + @dbname + " checkpoint" )

		/* release our hold on the db as logreader */

		EXEC sp_replflush
	end

	return(0)
go

print ''
print 'Creating procedure sp_subscribe.'
go
CREATE PROCEDURE sp_subscribe (
	@publication varchar(30),	      /* publication name */
	@article varchar(30) = 'all',	      /* article name */
	@destination_db varchar (30) = NULL,  /* subscriber database */
	@sync_type varchar (15) = 'automatic' /* subscription sync type */
	) AS

    SET NOCOUNT ON

    /*
    ** Declarations.
    */

    DECLARE @artid int
    DECLARE @none tinyint
    DECLARE @automatic tinyint
    DECLARE @cmd varchar(255)
    DECLARE @inactive tinyint
    DECLARE @manual tinyint
    DECLARE @pubid int
    DECLARE @restricted bit
    DECLARE @retcode int
    DECLARE @srvid smallint
    DECLARE @sync_typeid smallint
    DECLARE @subscriber_bit smallint

    /*
    ** Security Check
    ** Only the System Administratr (SA) or the Database Owner (dbo) or
    ** Replication subscriber (repl_subscriber) can subscribe to an article.
    */

    IF suser_id() <> 1 AND user_id() <> 1 AND user_id() <> 16383
        BEGIN
            RAISERROR (14093, 14, -1)
    	    RETURN (1)
        END

    /*
    ** Initializations.
    */

    SELECT @none = 2            /* Const: synchronization type 'none' */
    SELECT @automatic = 1       /* Const: synchronization type 'automatic' */
    SELECT @manual = 0          /* Const: synchronization type 'manual' */
    SELECT @restricted = 1      /* Const: security option 'restricted' */
    SELECT @inactive = 0        /* Const: subscription status 'inactive' */
    SELECT @subscriber_bit = 4  /* Const: subscription server status */

    /*
    ** Check to make sure that this procedure is not being executed on the
    ** publisher (locally).
    */

    IF @@REMSERVER IS NULL
        BEGIN
            RAISERROR (14073, 16, -1)
            RETURN (1)
        END

    SELECT @srvid = srvid
      FROM master..sysservers
     WHERE srvname = @@REMSERVER
       AND (srvstatus & @subscriber_bit) <> 0

    IF @srvid IS NULL
        BEGIN
            RAISERROR (14010, 16, -1)
   	    RETURN (1)
        END

    /*
    ** Parameter Check: @publication.
    ** Check to make sure that the publication exists and that it conforms
    ** to the rules for identifiers.
    */

    IF @publication IS NOT NULL
        BEGIN

            EXECUTE @retcode = sp_validname @publication

            IF @retcode <> 0
		RETURN (1)

            IF NOT EXISTS (SELECT * FROM syspublications WHERE name = @publication)
                BEGIN
                    RAISERROR (15001, 11, -1, @publication)
                    RETURN (1)
                END

        END

    SELECT @pubid = pubid FROM syspublications WHERE name = @publication

    IF @pubid IS NULL
        BEGIN
            RAISERROR (14043, 16, -1, 'The publication')
            RETURN (1)
        END

    /*
    ** Parameter Check:  @article
    ** Check to make sure that the article exists, is not NULL, and
    ** conforms to the rules for identifiers.
    */

    IF LOWER(@article) = 'all'
    	/*
	** Get all articles in the publication that are not subscribed to
	** by the @subscriber
	*/
        BEGIN

            IF EXISTS (SELECT *
                         FROM syspublications
                        WHERE name = @publication
                          AND restricted = 1)

                BEGIN

                    SELECT DISTINCT 'name' = a.name
                      INTO #tab1
                      FROM sysarticles a,
                           syspublications b,
                           syssubscriptions c,
                           master..sysservers d
                     WHERE a.pubid = b.pubid
                       AND b.name = @publication
                       AND a.artid = c.artid
                       AND c.srvid = d.srvid
		       AND c.status = @inactive
                       AND d.srvname = @@REMSERVER

                    CREATE UNIQUE INDEX idx1 ON #tab1 (name)
                    SELECT @cmd = 'DECLARE hCx SCROLL CURSOR FOR SELECT name FROM #tab1'

                END

            ELSE

                BEGIN

                    SELECT DISTINCT 'name' = a.name
                      INTO #tab2
                      FROM sysarticles a, syspublications b
                     WHERE a.pubid = b.pubid
                       AND b.name = @publication
		       AND NOT EXISTS (SELECT * from syssubscriptions s
		          WHERE s.artid = a.artid
			     AND s.srvid = @srvid)

                    CREATE UNIQUE INDEX idx1 ON #tab2 (name)
                    SELECT @cmd = 'DECLARE hCx SCROLL CURSOR FOR SELECT name FROM #tab2'

                END

            /*
            SELECT @cmd = ''
            SELECT @cmd = @cmd + 'DECLARE hCx SCROLL CURSOR FOR '
            SELECT @cmd = @cmd + ' SELECT DISTINCT a.name '
            SELECT @cmd = @cmd + '   FROM sysarticles a, syspublications b '
            SELECT @cmd = @cmd + '  WHERE a.pubid = b.pubid '
            SELECT @cmd = @cmd + '    AND b.name = ''' + @publication + ''''
            */

            EXECUTE (@cmd)
            OPEN hCx
            FETCH hCx INTO @article
            WHILE (@@fetch_status <> -1)
                BEGIN
                    EXECUTE sp_subscribe @publication    = @publication,
                                         @article        = @article,
                                         @destination_db = @destination_db,
                                         @sync_type      = @sync_type
                    FETCH hCx INTO @article
                END
            CLOSE hCx
            DEALLOCATE hCx
            RETURN (0)
        END

    IF @article IS NULL
        BEGIN
            RAISERROR (14043, 16, -1, 'The article')
            RETURN (1)
        END

    SELECT @artid = artid
      FROM sysarticles
     WHERE name = @article
       AND pubid = @pubid

    EXECUTE @retcode = sp_validname @article
    IF @retcode <> 0
	RETURN (1)

    IF NOT EXISTS (SELECT *
                     FROM sysarticles
                    WHERE artid = @artid
                      AND pubid = @pubid)
        BEGIN
            RAISERROR (15001, 11, -1, @article)
            RETURN (1)
        END

    IF @artid IS NULL
        BEGIN
            RAISERROR (14043, 16, -1, 'The article')
            RETURN (1)
        END

    /*
    ** Parameter Check: @sync_type.
    ** Set sync_typeid based on the @sync_type specified.
    **
    **   sync_typeid     sync_type
    **   ===========     =========
    **             0     manual
    **             1     automatic
    **             2     none
    */

    IF LOWER(@sync_type) NOT IN ('automatic', 'manual', 'none')
        BEGIN
            RAISERROR (14052, 16, -1)
            RETURN (1)
        END

    IF LOWER(@sync_type) IN ('automatic')
        SELECT @sync_typeid = @automatic
    ELSE IF LOWER(@sync_type) IN ('manual')
        SELECT @sync_typeid = @manual
    ELSE
        SELECT @sync_typeid = @none

    /*
    ** If it's an automatic sync, ensure that none of the articles
    ** being subscribed to have the 'owner qualified' status bit set
    */

    IF @sync_typeid = @automatic
    BEGIN
        IF EXISTS ( SELECT * FROM sysarticles
                    WHERE artid = @artid AND pubid = @pubid AND
                    4 = (status & 4) )
        BEGIN
            RAISERROR( 14098, 16, -1 )
            RETURN (1)
        END

    END


    /*
    ** Parameter Check: @destination_db.
    ** Set @destination_db to current database if not specified.  Make
    ** sure that the @destination_db conforms to the rules for identifiers.
    */

    IF @destination_db IS NULL SELECT @destination_db = DB_NAME()

    EXECUTE @retcode = sp_validname @destination_db

    IF @retcode <> 0
	RETURN (1)

    /*
    ** Bug 12850:
    ** Make sure that the creation_script is specified if pre_creation_cmd is "drop"
    ** Note that at this point, @article cannot be all.
    */

    if exists (select * from sysarticles where
    		name = @article AND
    		pre_creation_cmd = 1 AND
    		creation_script is null)
    	BEGIN
    		RAISERROR (14096, 16, -1)	
    		RETURN (1)
    	END
    

	BEGIN TRAN subscribe

        /*
        ** If 'public' publication, add the subscription.  Anyone can subscribe
        ** to a 'public' publication without intervention from the SA or DBO
        ** of the publishing server.
        */

        IF EXISTS (SELECT *
        	     FROM syspublications
        	    WHERE pubid = @pubid
        	      AND restricted = 0)
            BEGIN

                /*
                ** If the subscription already exists, don't add it.
                */

                IF EXISTS (SELECT *
                  	         FROM syssubscriptions
                	        WHERE artid = @artid
                	          AND srvid = @srvid)
                    BEGIN
                        ROLLBACK TRAN subscribe
                        RAISERROR (14058, 16, -1)
                        RETURN (1)
                    END

                /*
                ** The subscription doesn't exist, so let's add it to
                ** syssubscriptions.
                */

                INSERT syssubscriptions VALUES (@artid,
                				    @srvid,
                				    @destination_db,
                				    @inactive,
                				    @sync_typeid,
                				    NULL)

                IF @@ERROR <> 0
	            BEGIN
	        	ROLLBACK TRAN subsribe
                        RAISERROR (14057, 16, -1)
	        	RETURN (1)
	            END

            END

        /*
        ** If 'restricted' publication, update the subscription.  A restricted
        ** subscription must already exist in syssubscriptions.  It must be
        ** created by the SA or DBO on the publishing server using
        ** sp_addsubscription.  All we need to do is set the status to
        ** 'subscribe'.
        */

        ELSE
            BEGIN

        	/*
        	** First, make sure that the SA or DBO on the publishing
        	** server has created the subscription on the restricted
        	** publication.
        	*/

        	IF NOT EXISTS (SELECT *
        			 FROM syssubscriptions sub,
                                      syspublications pub,
        			      sysarticles art
        			WHERE art.artid = @artid
        			  AND pub.pubid = @pubid
        			  AND sub.srvid = @srvid
        			  AND sub.artid = art.artid
        			  AND pub.restricted = @restricted)
        			  /* AND sub.sync_type = @sync_typeid) */
        	    BEGIN
        		ROLLBACK TRAN subscribe
                        RAISERROR (14072, 16, -1)
        		RETURN (1)
        	    END

        	/*
        	** A subscription exists for the restricted publication.
        	*/

        	UPDATE syssubscriptions
        	   SET dest_db   = @destination_db,
        	       sync_type = @sync_typeid
        	 WHERE srvid = @srvid
        	   AND artid = @artid

        	IF @@ERROR <> 0
        	    BEGIN
        	        ROLLBACK TRAN subscribe
                        RAISERROR (14057, 16, -1)
        		RETURN (1)
        	    END
            END

        /*
        ** If a sync is required then update the subscription status to
	** 'subscribed'. Else, set the subscription status to 'active'.
        */
        if @sync_typeid = @none
		EXEC @retcode = sp_changesubstatus
			@publication = @publication,
                        @article     = @article,
                        @subscriber  = @@REMSERVER,
                        @status      = 'active'
	else
		EXEC @retcode = sp_changesubstatus
			@publication = @publication,
                        @article     = @article,
                        @subscriber  = @@REMSERVER,
                        @status      = 'subscribed'

        IF @@ERROR <> 0 OR @retcode <> 0
            BEGIN
        	   ROLLBACK TRAN subscribe
                   RAISERROR (14057, 16, -1)
        	   RETURN (1)
            END

    COMMIT TRAN subscribe
go

print ''
print 'Creating procedure sp_MSuninstall_publishing.'
go
CREATE PROCEDURE sp_MSuninstall_publishing

AS

	DECLARE @cmd varchar(255)
	DECLARE @name varchar(30)
	DECLARE @distname varchar(30)
	DECLARE @distdb varchar(30)
	DECLARE @proc varchar(255)

	/*
	** Unpublish all published databases.
	*/
        SELECT @cmd = ''
        SELECT @cmd = @cmd + 'DECLARE hC CURSOR FOR '
        SELECT @cmd = @cmd + 'SELECT name FROM master..sysdatabases '
        SELECT @cmd = @cmd + 'WHERE (category & 1) <> 0'
        EXECUTE (@cmd)
        OPEN hC
        FETCH hC INTO @name
        WHILE (@@fetch_status <> -1)
            BEGIN
    		SELECT @name
                EXECUTE sp_dboption @name, published, FALSE
                FETCH hC INTO @name
            END
        CLOSE hC
        DEALLOCATE hC

	/*
	** Drop all subscribers
	*/
        SELECT @cmd = ''
        SELECT @cmd = @cmd + 'DECLARE hC CURSOR FOR '
        SELECT @cmd = @cmd + 'SELECT srvname FROM master..sysservers '
        SELECT @cmd = @cmd + 'WHERE (srvstatus & 4) <> 0'
        EXECUTE (@cmd)
        OPEN hC
        FETCH hC INTO @name
        WHILE (@@fetch_status <> -1)
            BEGIN
                EXECUTE sp_dropsubscriber @name
                FETCH hC INTO @name
            END
        CLOSE hC
        DEALLOCATE hC

	/*
	** Remove the publish option on the server
	*/
	EXECUTE sp_serveroption @@SERVERNAME, pub, FALSE

	/*
	** Remove the publish option on the server
	*/
	IF @distname IS NOT NULL
		EXECUTE sp_serveroption @@SERVERNAME, pub, FALSE

	/*
	** Get the distribution server name
	*/
        SELECT @distname = srvname FROM master..sysservers
		WHERE (srvstatus & 8) <> 0

	/*
	** If this is a distribution server drop the distribution database
	** and remove distribution registry entry
	*/
	IF @distname = @@SERVERNAME
	BEGIN
		/*
		** If there are distribution publishers, do not drop the
		** distribution database
		*/
		IF EXISTS (SELECT * FROM master..sysservers where
			(srvstatus & 16) <> 0)
		BEGIN
		   RETURN (0)
		END

		/*
		** Get the distribution database name from the Registry
		*/
		SELECT @proc = 'master..xp_regread '
		EXECUTE @proc 'HKEY_LOCAL_MACHINE',
			  'SOFTWARE\Microsoft\MSSQLServer\Replication',
			  'DistributionDB',
		    @param = @distdb OUTPUT

		/*
		** Drop the distribution database
		*/
		EXECUTE ('DROP DATABASE ' + @distdb)

		/*
		** Clear the distribution registry values
		*/
		EXECUTE master.dbo.xp_regwrite 'HKEY_LOCAL_MACHINE',
                                   'SOFTWARE\Microsoft\MSSQLServer\Replication',
                                   'DistributionDB',REG_SZ, ''
	END		

	/*
	** Remove the distribution option on the server
	*/
	IF @distname IS NOT NULL
		EXECUTE sp_serveroption @distname, dist, FALSE

       /* 
       ** Drop all remote servers that have the DSN option set 
       */
        SELECT @cmd = ''
        SELECT @cmd = @cmd + 'DECLARE hC CURSOR FOR '
        SELECT @cmd = @cmd + 'SELECT srvname FROM master..sysservers '
        SELECT @cmd = @cmd + 'WHERE (srvstatus & 32) <> 0'
        EXECUTE (@cmd)
        OPEN hC
        FETCH hC INTO @name
        WHILE (@@fetch_status <> -1)
            BEGIN
                EXECUTE sp_dropserver @name
                FETCH hC INTO @name
            END
        CLOSE hC
        DEALLOCATE hC
GO

print ''
print 'Creating procedure sp_unsubscribe.'
go
CREATE PROCEDURE sp_unsubscribe (
	@publication varchar(30) = NULL,       /* publication name */
	@article varchar(30) = NULL            /* article name */
	) AS

    SET NOCOUNT ON

    /*
    ** Declarations.
    */

    DECLARE @cmd varchar(255)
    DECLARE @pubid int
    DECLARE @artid int
    DECLARE @public tinyint
    DECLARE @srvid smallint
    DECLARE @subscriber_bit smallint
    DECLARE @retcode int
    DECLARE @active tinyint

    /*
    ** Security Check
    ** Only the System Administratr (SA) or the Database Owner (dbo) or
    ** Replication subscriber (repl_subscriber) can unsubscribe from an article.
    */

    IF suser_id() <> 1 AND user_id() <> 1 AND user_id() <> 16383
        BEGIN
            RAISERROR (14093, 14, -1)
    	    RETURN (1)
        END

    /*
    ** Initializations.
    */

    SELECT @public = 0          /* Const: security = 'public'. */
    SELECT @subscriber_bit = 4  /* Const: subscription server status */
    SELECT @active = 2          /* Const: subscription status 'active' */

    /*
    ** Check to make sure that this procedure is not being executed on the
    ** publisher (locally).
    */

    IF @@REMSERVER IS NULL
        BEGIN
            RAISERROR (14073, 16, -1)
            RETURN (1)
        END

    /*
    ** Check if the server exists and that it is a subscription server.
    */

    SELECT @srvid = srvid
	             FROM sysservers
                    WHERE srvname = @@REMSERVER
                      AND (srvstatus & @subscriber_bit) <> 0
    IF @srvid IS NULL
        BEGIN
            RAISERROR (14010, 16, -1)
	    RETURN (1)
        END

    /*
    ** If the @publication is 'all' the user wants to cancel all
    ** @publication @article subscriptions.
    */

    IF LOWER(@publication) = 'all'
        BEGIN

            SELECT DISTINCT a.name
              INTO #unsubscribe1
              FROM syspublications a,
                   sysarticles b,
                   syssubscriptions c
             WHERE c.srvid = @srvid
               AND c.artid = b.artid
               AND b.pubid = a.pubid

            CREATE UNIQUE INDEX idx1 ON #unsubscribe1 (name)

            SELECT @cmd = 'DECLARE hC1 SCROLL CURSOR FOR SELECT * FROM #unsubscribe1'
            EXECUTE (@cmd)

            OPEN hC1
            FETCH hC1 INTO @publication

            WHILE (@@fetch_status <> -1)
                BEGIN
                    EXECUTE sp_unsubscribe @publication, 'all'
                    FETCH hC1 INTO @publication
                END
            CLOSE hC1
            DEALLOCATE hC1
            RETURN (0)
        END

    /*
    ** Parameter Check:  @publication.
    ** Check to make sure that the publication exists.
    */

    IF @publication IS NOT NULL
        BEGIN

            EXECUTE @retcode = sp_validname @publication

            IF @retcode <> 0
		RETURN (1)

            IF NOT EXISTS (SELECT * FROM syspublications WHERE name = @publication)
                BEGIN
                    RAISERROR (15001, 11, -1, @publication)
                    RETURN (1)
                END

        END

    SELECT @pubid = pubid FROM syspublications WHERE name = @publication

    IF @pubid IS NULL
        BEGIN
            RAISERROR (14043, 16, -1, 'The publication')
            RETURN (1)
        END

    /*
    ** If the @article is 'all', the user wants to cancel all subscriptions
    ** for the publication.
    */

    IF LOWER(@article) = 'all'
        BEGIN

            SELECT a.name
              INTO #unsubscribe2
              FROM sysarticles a,
                   syssubscriptions b
             WHERE a.pubid = @pubid
               AND b.srvid = @srvid
               AND b.artid = a.artid

            CREATE UNIQUE INDEX idx1 ON #unsubscribe2 (name)

            SELECT @cmd = 'DECLARE hC2 SCROLL CURSOR FOR SELECT * FROM #unsubscribe2'
            EXECUTE (@cmd)

            OPEN hC2
            FETCH hC2 INTO @article

            WHILE (@@fetch_status <> -1)
                BEGIN
                    EXECUTE sp_unsubscribe @publication, @article
                    FETCH hC2 INTO @article
                END
            CLOSE hC2
            DEALLOCATE hC2
            RETURN (0)
        END

    /*
    ** Parameter Check:  @article.
    ** Check to make sure that the article exists.
    */

    SELECT @artid = artid
      FROM sysarticles
     WHERE name = @article
       AND pubid = @pubid

    IF @article IS NOT NULL
        BEGIN
            EXECUTE @retcode = sp_validname @article
            IF @retcode <> 0
		RETURN (1)

	    IF NOT EXISTS (SELECT *
                             FROM sysarticles
                            WHERE artid = @artid
                              AND pubid = @pubid)
                BEGIN
                    RAISERROR (15001, 11, -1, @article)
                    RETURN (1)
                END
        END

    IF @artid IS NULL
        BEGIN
            RAISERROR (14043, 16, -1, 'The article')
            RETURN (1)
        END

    IF NOT EXISTS (SELECT *
                     FROM syssubscriptions a,
                          sysarticles b,
                          syspublications c
                    WHERE a.srvid = @srvid
                      AND a.artid = b.artid
                      AND b.pubid = c.pubid
                      AND c.pubid = @pubid
                      AND a.status <> 0)
        BEGIN
            RAISERROR (14050, 11, -1)
            RETURN (1)
        END

    BEGIN TRAN unsubscribe

        /*
        ** Change the status of the subscription to 'inactive', then delete
        ** subscription row.  If the subscription is a public subscription,
        ** it can safely be deleted.
        */

        /*
        ** Change the status of the subscription to 'inactive'.
        */

        EXECUTE @retcode  = sp_changesubstatus @publication = @publication,
        			               @article     = @article,
                                               @subscriber  = @@REMSERVER,
                                               @status      = 'inactive'

        IF @@ERROR <> 0 OR @retcode <> 0
            BEGIN
                ROLLBACK TRAN unsubscribe
                RAISERROR (14056, 16, -1)
                RETURN (1)
            END

        /*
        ** If the publication is 'public', delete the subscription.
        */

        IF EXISTS (SELECT *
                     FROM syspublications
                    WHERE pubid = @pubid
                      AND restricted = @public)
            BEGIN

                DELETE syssubscriptions
                 WHERE srvid = @srvid
                   AND artid = @artid

        	IF @@ERROR <> 0
        	    BEGIN
        	        ROLLBACK TRAN unsubscribe
                        RAISERROR (14056, 16, -1)
        	        RETURN (1)
        	    END
            END

    COMMIT TRAN unsubscribe

go

print ''
print 'Creating procedure sp_create_distribution_tables.'
go
CREATE PROCEDURE sp_create_distribution_tables
AS

BEGIN
   IF NOT EXISTS (SELECT * FROM sysobjects WHERE name = 'MSjobs' and type = 'U')
   BEGIN
      /****************************************************************************/
      PRINT ''
      PRINT 'Creating Table:  dbo.MSjobs'
      PRINT ''
      /****************************************************************************/
      CREATE TABLE MSjobs
      (
      publisher_id smallint,
      publisher_db varchar (  30 ) ,
      job_id int,
      type tinyint, /* 0- sql cmd 1-noop cmd 2-sql script 3/4-bcp cmd 5-manual sync*/
      xactid_page int,
      xactid_row smallint,
      xactid_ts binary (  8 ) ,
      entry_time datetime
      )

      /* Set category bit to reflect MS objects */
      UPDATE sysobjects SET category = category | 2 WHERE sysstat & 0xf = 3 AND
         name = 'MSjobs'

      PRINT ''
      PRINT 'Creating Clustered Index: ucMSjobs'
      PRINT ''
      CREATE UNIQUE CLUSTERED INDEX ucMSjobs ON dbo.MSjobs
         (publisher_db, publisher_id, job_id)
   END

   IF NOT EXISTS (SELECT * FROM sysobjects WHERE name = 'MSjob_commands' and type = 'U')
   BEGIN
      /****************************************************************************/
      PRINT ''
      PRINT 'Creating Table:  dbo.MSjob_commands'
      PRINT ''
      /****************************************************************************/
      CREATE TABLE MSjob_commands (
      publisher_id smallint,
      publisher_db varchar (  30 ) ,
      job_id int,
      command_id int,
      art_id int,
      incomplete bit,
      command varchar (  255 ) NULL
      )
      /* Set category bit to reflect MS objects */
      UPDATE sysobjects SET category = category | 2 WHERE sysstat & 0xf = 3 AND
         name = 'MSjob_commands'

      PRINT ''
      PRINT 'Creating Clustered Index: ucMSjob_commands'
      PRINT ''
      CREATE UNIQUE CLUSTERED INDEX ucMSjob_commands ON dbo.MSjob_commands
         (publisher_db, publisher_id, job_id, command_id)
   END

   IF NOT EXISTS (SELECT * FROM sysobjects WHERE name = 'MSsubscriber_jobs' and type = 'U')
   BEGIN
      /****************************************************************************/
      PRINT ''
      PRINT 'Creating Table:  dbo.MSsubscriber_jobs'
      PRINT ''
      /****************************************************************************/
      CREATE TABLE MSsubscriber_jobs
      (
      publisher_id smallint,
      publisher_db varchar (30),
      job_id int,
      subscriber_id smallint,
      subscriber_db varchar (  30 ) ,
      command_id int
      )

      /* Set category bit to reflect MS objects */
      UPDATE sysobjects SET category = category | 2 WHERE sysstat & 0xf = 3 AND
      	name = 'MSsubscriber_jobs'

      PRINT ''
      PRINT 'Creating Clusteredd Index: ucMSsubscriber_jobs'
      PRINT ''
      CREATE UNIQUE CLUSTERED INDEX ucMSsubscriber_jobs ON dbo.MSsubscriber_jobs
         (publisher_db, publisher_id, subscriber_db, subscriber_id, job_id, command_id)

      PRINT ''
      PRINT 'Creating NonClustered Index: ncMSsubscriber_jobs'
      PRINT ''
      CREATE NONCLUSTERED INDEX ncMSsubscriber_jobs ON dbo.MSsubscriber_jobs
         (publisher_db, publisher_id, job_id)
   END


   IF NOT EXISTS (SELECT * FROM sysobjects WHERE name = 'MSsubscriber_status' and type = 'U')
   BEGIN
      /****************************************************************************/
      PRINT ''
      PRINT 'Creating Table:  dbo.MSsubscriber_status'
      PRINT ''
      /****************************************************************************/
      CREATE TABLE MSsubscriber_status
      (
      publisher_id smallint,
      publisher_db varchar (  30 ) ,
      job_id int,
      subscriber_id smallint,
      subscriber_db varchar (  30 ) ,
      completion_time datetime,
      delivery_latency int,
      delivered_jobs int,
      delivery_rate int,
      status int
      )

      /* Set category bit to reflect MS objects */
      UPDATE sysobjects SET category = category | 2 WHERE sysstat & 0xf = 3 AND
         name = 'MSsubscriber_status'

      PRINT ''
      PRINT 'Creating Clustered Index: ucMSsubscriber_status'
      PRINT ''
      CREATE UNIQUE CLUSTERED INDEX ucMSsubscriber_status on dbo.MSsubscriber_status
       (publisher_db, publisher_id, subscriber_db, subscriber_id, job_id)

   END

   IF NOT EXISTS (SELECT * FROM sysobjects WHERE name = 'MSsubscriber_info' and type = 'U')
   BEGIN
      /****************************************************************************/
      PRINT ''
      PRINT 'Creating Table:  dbo.MSsubscriber_info'
      PRINT ''
      /****************************************************************************/
      CREATE TABLE MSsubscriber_info
      (
      publisher  varchar (30),
      subscriber  varchar (30),
      type tinyint,           /* 0: MS SQL Server 1: ODBC Data Source */
      login varchar(30) NULL,
      password varchar(30) NULL,
      commit_batch_size int,
      status_batch_size int,
      flush_frequency int,
      frequency_type int,
      frequency_interval int,
      frequency_relative_interval int,
      frequency_recurrence_factor int,
      frequency_subday int,
      frequency_subday_interval int,
      active_start_time_of_day int,
      active_end_time_of_day int,
      active_start_date int,
      active_end_date int,
      retryattempts int,
      retrydelay int,
      description varchar(255) NULL,
      )

      /* Set category bit to reflect MS objects */
      UPDATE sysobjects SET category = category | 2 WHERE sysstat & 0xf = 3 AND
	 name = 'MSsubscriber_info'

      PRINT ''
      PRINT 'Creating Clustered Index: ucMSsubscriber_info'
      PRINT ''
      CREATE UNIQUE CLUSTERED INDEX ucMSsubscriber_info ON dbo.MSsubscriber_info
	 (publisher, subscriber)
   END
   ELSE
   BEGIN
      IF NOT EXISTS (select * from syscolumns
		where name = 'description'
		and id=object_id('MSsubscriber_info'))
      BEGIN
         /****************************************************************************/
         PRINT ''
         PRINT 'Alter Table:  dbo.MSsubscriber_info'
         PRINT ''
         /****************************************************************************/
         ALTER TABLE MSsubscriber_info ADD description varchar (255) NULL
	 UPDATE MSsubscriber_info SET description = 'SQL Server 6.0'
      END
   END

   IF NOT EXISTS (SELECT * FROM sysobjects WHERE name = 'MSjob_subscriptions' and type = 'U')
   BEGIN
      /****************************************************************************/
      PRINT ''
      PRINT 'Creating Table:  dbo.MSjob_subscriptions'
      PRINT ''
      /****************************************************************************/
      CREATE TABLE MSjob_subscriptions
      (
      publisher varchar ( 30 ),
      publisher_id smallint,
      publisher_db varchar (  30 ) ,
      subscriber varchar ( 30 ),
      subscriber_id smallint,
      art_id int,
      subscriber_db varchar (  30 ) ,
      status tinyint,
      ts binary (8)
      )

      /* Set category bit to reflect MS objects */
      UPDATE sysobjects SET category = category | 2 WHERE sysstat & 0xf = 3 AND
	 name = 'MSjob_subscriptions'

      PRINT ''
      PRINT 'Creating Clustered Index: ucMSsubscriptions'
      PRINT ''
      CREATE UNIQUE CLUSTERED INDEX ucMSsubscriptions ON dbo.MSjob_subscriptions
	 (publisher_db, publisher_id, art_id, subscriber_id)
   END
END
GO



if exists (select * from sysobjects
		where sysstat & 0xf = 4
			and name = 'sp_repldone')
	exec sp_dropextendedproc 'sp_repldone'
go

if exists (select * from sysobjects
		where sysstat & 0xf = 4
			and name = 'sp_repltrans')
	exec sp_dropextendedproc 'sp_repltrans'
go

if exists (select * from sysobjects
		where sysstat & 0xf = 4
			and name = 'sp_replcmds')
	exec sp_dropextendedproc 'sp_replcmds'
go

if exists (select * from sysobjects
		where sysstat & 0xf = 4
			and name = 'sp_replcounters')
	exec sp_dropextendedproc 'sp_replcounters'
go

if exists (select * from sysobjects
		where sysstat & 0xf = 4
			and name = 'sp_replflush')
	exec sp_dropextendedproc 'sp_replflush'
go

if exists (select * from sysobjects
		where sysstat & 0xf = 4
			and name = 'sp_replstatus')
	exec sp_dropextendedproc 'sp_replstatus'
go

/*
** Add extended stored procedures for replication support.
*/
sp_addextendedproc  'sp_repldone', 'repldone extended procedure'
go

sp_addextendedproc  'sp_repltrans', 'repltrans extended procedure'
go

sp_addextendedproc  'sp_replcounters', 'replcounters extended procedure'
go

sp_addextendedproc  'sp_replcmds', 'replcmds extended procedure'
go

sp_addextendedproc  'sp_replflush', 'replflush extended procedure'
go

sp_addextendedproc  'sp_replstatus', 'replstatus extended procedure'
go

print ''
print 'Adding logins and users for replication.'
print ''
go
/*
**  Add the login and user named 'repl_subscriber'.  These are used when a
**  replication publisher.  Use the maximum user id for the login id.
*/
if not exists (select * from syslogins where name = 'repl_subscriber')
	insert into syslogins (suid,status,accdate,totcpu,totio,spacelimit,
			timelimit,resultlimit,dbname,name,password,language)
	values (16383, 9, getdate(), 0, 0, 0, 0, 0,'master','repl_subscriber',
			pwdencrypt(convert(char(30),@@dbts)),NULL)
go

if not exists (select * from sysusers where name = 'repl_subscriber')
	exec sp_adduser 'repl_subscriber'
go

/*
**  Add the login and user named 'repl_publisher'.  This is used when a
**  replication subscriber.
*/
if not exists (select * from syslogins where name = 'repl_publisher')
	insert into syslogins (suid,status,accdate,totcpu,totio,spacelimit,
			timelimit,resultlimit,dbname,name,password,language)
	values (16382, 9, getdate(), 0, 0, 0, 0, 0,'master','repl_publisher',
			pwdencrypt(convert(char(30),@@dbts)),NULL)
go

/*
** Add xp_enum_dsn extended procedure
*/
sp_addextendedproc 'xp_dsninfo','xpsql60.dll'
go

/*
** Add xp_enum_dsn extended procedure
*/
sp_addextendedproc 'xp_enumdsn','xpsql60.dll'
go

grant execute on sp_addarticle to public
go
grant execute on sp_addpublication to public
go
grant execute on sp_addsubscription to public
go
grant execute on sp_articlecolumn to public
go
grant execute on sp_articlefilter to public
go
grant execute on sp_articletextcol to public
go
grant execute on sp_articleview to public
go
grant execute on sp_changearticle to public
go
grant execute on sp_changepublication to public
go
grant execute on sp_changesubscription to public
go
grant execute on sp_distcounters to public
go
grant execute on sp_droparticle to public
go
grant execute on sp_droppublication to public
go
grant execute on sp_dropsubscription to public
go
grant execute on sp_helparticle to public
go
grant execute on sp_helparticlecolumns to public
go
grant execute on sp_helpdistributor to public
go
grant execute on sp_helppublication to public
go
grant execute on sp_helppublicationsync to public
go
grant execute on sp_helpreplicationdb to public
go
grant execute on sp_helpsubscription to public
go
grant execute on sp_helpsubscriberinfo to public
go
grant execute on sp_replcounters to probe
go
grant execute on sp_replstatus to public
go
grant execute on sp_subscribe to public
go
grant execute on sp_textcolstatus to public
go
grant execute on sp_unsubscribe to public
go

sp_configure 'allow updates',0
go
reconfigure with override
go

print ''
print 'Checking objects created by instrepl.sql.'
go

--obsolete   exec sp_check_objects 'repl'
exec sp_MS_upd_sysobj_category 2  --set sysobjects.category | 2 based on crdate.
go

print ''
print 'instrepl.sql completed successfully.'
go

dump tran master with no_log
go
checkpoint
go
-- - -----
