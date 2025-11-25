
---- Install\Proc65_1.SQL    1996/07/18 10:22
---- MSSQL 6.5 ServicePack1
go
use master
go
dump transaction master with no_log

checkpoint
go
set nocount on
set implicit_transactions off
go

declare @vdt varchar(99)
select  @vdt = convert(varchar,getdate(),113)
raiserror('Starting Install\Proc65_1.SQL at  %s',0,1,@vdt) with nowait


print ''
print 'Making sure that updates to system tables are allowed.'
go

declare  @dbcc_current_version	integer
	,@int1			integer
dbcc getvalue('current_version')  --1=4.21A ,400-406=6.0 ,407-?=6.5 (See spt_values 'DBV')
select @dbcc_current_version = @@error

if (     exists (select * from sysobjects where name='sp_configure')
    AND  @dbcc_current_version = (select version from sysdatabases where name='master')
    AND  1 <> (select value from syscurconfigs where config = 102)
   )
	begin						--Query tree compatible
	exec @int1 = sp_configure 'allow updates',1
	if @@error <> 0 or @int1 <> 0
		raiserror('Bad sp_configure exec at top of Proc65_1.SQL, killing spid.'
			,22,127) with log
	reconfigure with override
	end
go

/*
** Make sure server was started in single user mode or that sp_configure was used
** to enable updates to system tables.
*/

if (select value from syscurconfigs where config = 102) <> 1
	raiserror('Cannot run Proc65_1.SQL unless updates to system tables are enabled.  Run sp_configure allow...'
			,22,127) with log
go


execute sp_MS_upd_sysobj_category 1  --Capture now_datetime for use below.

go


/*******************************************************************



	sysmessages



********************************************************************/

print ' '
print 'Insert sysmessages....'

delete sysmessages where error in (8525 ,16965)

insert into sysmessages
	(error ,severity ,dlevel ,description ,langid)
	values
	(8525,16,1,'Distributed transaction completed. Either enlist this session in a new transaction or the NULL transaction.',NULL)
insert into sysmessages values
	(16965,10,0,'Cursor has no longer a valid compiled plan as the underlying table(s) have changed.',NULL)

go


/*******************************************************************



	spt_values TBO



********************************************************************/

print ' '  -- Table Options (In 6.5 see sp_tableoption, session.h)
print 'Insert spt_values type=''TBO'' , sp_tableoption.'
DELETE spt_values where type='TBO'
insert spt_values (name ,number ,type ,low ,high ,status)
	values ('TableOptions' ,0 ,'TBO' ,0 ,1 ,0) -- sysobj.userstat/.sysstat
insert spt_values (name ,number ,type ,low ,high ,status)
	values ('insert row lock' ,1 ,'TBO' ,0 ,1 ,0) -- userstat
insert spt_values (name ,number ,type ,low ,high ,status)
	values ('table lock on bulk load' ,2 ,'TBO' ,0 ,1 ,0) -- userstat
insert spt_values (name ,number ,type ,low ,high ,status)
	values ('pintable' ,512 ,'TBO' ,0 ,1 ,0) -- sysstat
go


/*******************************************************************



	stored procs



********************************************************************/


--Enhanced sp_rename/sp_namecrack_qi, thus these are obsolete in MS SQL 6.5 SP1
if exists (select * from sysobjects
		where sysstat & 0xf = 4
			and name = 'sp_namecrack2')
	drop procedure sp_namecrack2
if exists (select * from sysobjects
		where sysstat & 0xf = 4
			and name = 'sp_namecrack3')
	drop procedure sp_namecrack3

go



if object_id('sp_namecrack_qi') is not null
	drop proc sp_namecrack_qi
raiserror(15339,-1,-1,'sp_namecrack_qi')
go
create procedure sp_namecrack_qi --1996/05/09 16:08
       @WholeQualName   varchar(255)

      ,@CountNumNodes   integer             OUTPUT

--0x0000000D (1+4+8 integer bits ON) means @UnqualName and @QualName2 were DQuoted in @WholeQualName
--Power(2,0) bit is @UnqualName; Power(2,2) bit is @QualName2; Power(2,3) bit is @QualName3 etc.
      ,@BitsQuoted      integer             OUTPUT

      ,@UnqualName      varchar( 30)        OUTPUT  --rightmost name node

      ,@QualName1       varchar( 30) = null OUTPUT  --rightmost qualifier.
      ,@QualName2       varchar( 30) = null OUTPUT
      ,@QualName3       varchar( 30) = null OUTPUT
      ,@QualName4       varchar( 30) = null OUTPUT
      ,@QualName5       varchar( 30) = null OUTPUT
as

--Set ansi_defaults    off
Set ansi_nulls       on
Set ansi_padding     on
Set nocount          on


Declare
       @RetCode            integer
      ,@SetQIStatus        integer  -- 0 means is set off for current spid.
      ,@CurrentByte        char(1)
Declare
       @NameNodeNumFromRightToLeft  integer
      ,@EndPosDQuote                integer
      ,@BeginPosDQuote              integer
      ,@EndPosName                  integer
      ,@BeginPosName                integer
Declare
       @PosInQualName               integer
      ,@SkipNextByte                char(4)
      ,@PrevByteMaybeBeginDQuote    char(4)
      ,@Opt_quoted_identifier       integer

-------------------------------------------------

Select
       @RetCode                     = 0  --  =0 means good  , >0 means bad
Select
       @CountNumNodes      = 0
      ,@BitsQuoted         = 0
      ,@UnqualName         = null
      ,@QualName1          = null
      ,@QualName2          = null
      ,@QualName3          = null
      ,@QualName4          = null
      ,@QualName5          = null
Select
       @EndPosDQuote                = -1
      ,@BeginPosDQuote              = -1
      ,@EndPosName                  = -1
      ,@BeginPosName                = -1


--Select @SetQIStatus = execute sp_help_setopts 'quoted_identifier'
Select   @Opt_quoted_identifier = number
   from  master..spt_values
   where type = 'SOP' and name = 'quoted_identifier'
Select @SetQIStatus = @@options & @Opt_quoted_identifier

---------------------------------------------

Select @PosInQualName = datalength(@WholeQualName)

IF (     @PosInQualName <= 0
   OR    @PosInQualName is null
   )
   begin
   Select @RetCode = @RetCode | 0x0001
   GOTO LABEL_86_RETURN
   end

Select @PosInQualName = @PosInQualName + 1


IF (charindex('"',@WholeQualName) > 0)
   begin
   IF (@SetQIStatus = 0)
      begin
      Select @RetCode = @RetCode | 0x0002
      GOTO LABEL_86_RETURN
      end
   end

Select
       @NameNodeNumFromRightToLeft  = 0
      ,@SkipNextByte                = 'no'
      ,@PrevByteMaybeBeginDQuote    = 'no'


--    Every loop iteration is trying to finalize the
-- interpretation of the previous byte.

-----
WHILE (@PosInQualName > 1)  --Loop thru @WholeQualName bytes, right-to-left.
   begin

   Select @PosInQualName = @PosInQualName - 1

   Select @CurrentByte = substring(@WholeQualName,@PosInQualName,1)


   --------- Special case: Position boundary, last.


   IF (@PosInQualName = datalength(@WholeQualName))
      begin

      IF (@CurrentByte = '.')
         begin
         Select @RetCode = @RetCode | 0x0004
         GOTO LABEL_86_RETURN
         end

      IF (@CurrentByte = '"')
         begin
         Select @EndPosDQuote = @PosInQualName
         Select @EndPosName   = @PosInQualName - 1

         CONTINUE
         end
      ELSE --Presumably current is a letter or digit
         begin
         Select @EndPosName   = @PosInQualName

         CONTINUE
         end
      end --If pos last



   --------- Special case: Position boundary, first.


   IF (@PosInQualName = 1)
      begin

      IF (@CurrentByte = '.')
         begin
         Select @RetCode = @RetCode | 0x0008
         GOTO LABEL_86_RETURN
         end

      IF (@CurrentByte = '"')
         begin

         IF (@EndPosDQuote <= 0)
            begin
            Select @RetCode = @RetCode | 0x0010
            GOTO LABEL_86_RETURN
            end
         ELSE --Could be beginning DQuote
            begin

            IF (@PrevByteMaybeBeginDQuote = 'yes')
               begin
               Select @RetCode = @RetCode | 0x0020
               GOTO LABEL_86_RETURN
               end
            ELSE --Prev byte was not a DQuote
               begin
               Select @BeginPosDQuote = @PosInQualName
               Select @BeginPosName   = @PosInQualName + 1

               GOTO LABEL_34_COMPLETEDNODE
               end
            end
         end
      ELSE --Current is not DQuote (nor a dot)
         begin

         IF (@EndPosDQuote > 0)
            begin
            Select @RetCode = @RetCode | 0x0040
            GOTO LABEL_86_RETURN
            end
         ELSE
            begin

            IF (@EndPosName <= 0)
               begin
               Select @EndPosName = @PosInQualName
               end

            Select @BeginPosName = @PosInQualName

            GOTO LABEL_34_COMPLETEDNODE
            end
         end
      end --If pos 1



   --------- Special case: DQuote char.


   IF (@CurrentByte = '"')
      begin

      IF (@EndPosName <= 0) --Discovering new node
         begin
         Select @EndPosDQuote = @PosInQualName
         Select @EndPosName   = @PosInQualName - 1

         CONTINUE
         end
      ELSE
         begin

         IF (@EndPosDQuote <= 0)
            begin
            Select @RetCode = @RetCode | 0x0080
            GOTO LABEL_86_RETURN
            end
         ELSE
            begin

            IF (@PrevByteMaybeBeginDQuote = 'no')
               begin
               Select @PrevByteMaybeBeginDQuote = 'yes' --toggle
               end
            ELSE
               begin
               Select @PrevByteMaybeBeginDQuote = 'no'
               end

            CONTINUE
            end
         end
      end --If DQoute


   --------- Special case: dot char.


   IF (@CurrentByte = '.')
      begin

      IF (@EndPosName <= 0)
         begin

         IF (substring(@WholeQualName,@PosInQualName+1,1) <> '.')
            begin --Previously examined byte was dot also.
            Select @RetCode = @RetCode | 0x0100
            GOTO LABEL_86_RETURN
            end
         ELSE
            begin

            GOTO LABEL_34_COMPLETEDNODE
            end
         end
      ELSE --Node is in process.
         begin

         IF (@EndPosDQuote >  0)
            begin

            IF (@PrevByteMaybeBeginDQuote = 'yes')
               begin
               Select @BeginPosName = @PosInQualName + 2

               GOTO LABEL_34_COMPLETEDNODE
               end
            ELSE
               begin

               CONTINUE
               end
            end
         ELSE --Normal begin byte determined
            begin
            Select @BeginPosName = @PosInQualName + 1

            GOTO LABEL_34_COMPLETEDNODE
            end
         end
      end --If dot



  ---------------------  Actions for general cases.


   IF (@EndPosName <= 0)
      begin
      Select @EndPosName = @PosInQualName
      end

   CONTINUE



---------------------------------------------------------------

LABEL_34_COMPLETEDNODE:


   Select @NameNodeNumFromRightToLeft =
          @NameNodeNumFromRightToLeft + 1

   IF (@EndPosName <= 0)
      begin
      CONTINUE
      end


   IF (@EndPosDQuote >  0)
      begin
      Select @BitsQuoted = @BitsQuoted |
                      Power(2,@NameNodeNumFromRightToLeft - 1)
      end



   IF      (@NameNodeNumFromRightToLeft - 1 = 0)
      begin
      Select @UnqualName = substring(@WholeQualName
                                    ,@BeginPosName
                                    ,@EndPosName-@BeginPosName+1)
      end
   ELSE IF (@NameNodeNumFromRightToLeft - 1 = 1)
      begin
      Select @QualName1  = substring(@WholeQualName
                                    ,@BeginPosName
                                    ,@EndPosName-@BeginPosName+1)
      end
   ELSE IF (@NameNodeNumFromRightToLeft - 1 = 2)
      begin
      Select @QualName2  = substring(@WholeQualName
                                    ,@BeginPosName
                                    ,@EndPosName-@BeginPosName+1)
      end
   ELSE IF (@NameNodeNumFromRightToLeft - 1 = 3)
      begin
      Select @QualName3  = substring(@WholeQualName
                                    ,@BeginPosName
                                    ,@EndPosName-@BeginPosName+1)
      end
   ELSE IF (@NameNodeNumFromRightToLeft - 1 = 4)
      begin
      Select @QualName4  = substring(@WholeQualName
                                    ,@BeginPosName
                                    ,@EndPosName-@BeginPosName+1)
      end
   ELSE IF (@NameNodeNumFromRightToLeft - 1 = 5)
      begin
      Select @QualName5  = substring(@WholeQualName
                                    ,@BeginPosName
                                    ,@EndPosName-@BeginPosName+1)
      end


   Select
--        @NameNodeNumFromRightToLeft = null
          @EndPosDQuote    = -1
         ,@BeginPosDQuote  = -1
         ,@EndPosName      = -1
         ,@BeginPosName    = -1

   Select @PrevByteMaybeBeginDQuote = 'no'

   end -- >0

------------------------------------------------

Select @CountNumNodes = @NameNodeNumFromRightToLeft

LABEL_86_RETURN:

IF (object_id('tempdb..#ncqinamenodes') is not null)
            drop table #ncqinamenodes

Return @RetCode --sp_namecrack_qi
go
grant execute on sp_namecrack_qi to public
go



if object_id('sp_tableoption') is not null
	drop proc sp_tableoption
raiserror(15339,-1,-1,'sp_tableoption')
go
create procedure sp_tableoption  --1996/07/10 17:16
    @TableNamePattern      character varying(150)

   ,@OptionName            character varying(35)   = null
   ,@OptionValue           character varying(12)   = null
as

Set nocount       on
Set ansi_nulls    on
Set ansi_warnings on

Declare
       @RetCode            integer
      ,@ExecRC             integer
      ,@int1               integer
      ,@int2               integer
Declare
       @CurrentDBName      varchar(30)
      ,@CurrentDBId        integer
      ,@CountMatchedObj    integer
      ,@CountUpdRows2      integer
      ,@IRLString          varchar(35)
      ,@BCPTabLockString   varchar(35)
      ,@PinTabString       varchar(35)
      ,@Status_csr_11      integer
      ,@TabId              integer
      ,@BitLogic           varchar(9)
Declare
       @WOptionName        character varying(35)  -- spt_values.name
      ,@WOptionNum         integer
      ,@charWOptionNum     varchar(12)
      ,@WOptionValue       character varying( 6)  -- T/F
      ,@intOptionValue     integer
Declare
       @UnqualName         varchar(30)
      ,@QualName1          varchar(30)    -- own
      ,@QualName2          varchar(30)    -- db
      ,@QualName3          varchar(30)    -- svr
      ,@QualName4          varchar(30)    -- (Extra)
      ,@QualName5          varchar(30)    -- (Extra)

      ,@WQualName1         varchar(30)
      ,@WQualName2         varchar(30)

-----------------------------

Select
       @RetCode            = 0   --  =0 is good,  >0 is bad.
      ,@CurrentDBName      = db_name()
      ,@CurrentDBId        = db_id()
      ,@CountMatchedObj    = 0
      ,@CountUpdRows2      = 0
      ,@IRLString          = 'insert row lock'
      ,@BCPTabLockString   = 'table lock on bulk load'
      ,@PinTabString       = 'pintable'
      ,@Status_csr_11      = 0  -- 0x01 bit =Declared; 0x02 bit =Open.
      ,@TabId              = null
Select
       @UnqualName         = null
      ,@QualName1          = null
      ,@QualName2          = null
      ,@QualName3          = null
      ,@QualName4          = null
      ,@QualName5          = null

      ,@WQualName1         = null  -- own
      ,@WQualName2         = null  -- db


--------------------- Basic parm edits  -------------------

IF (@TableNamePattern IS null)
   begin
   RaisError(15113,-1,-1,'@TableNamePattern')
   Select @RetCode = @@error
   GOTO LABEL_86_RETURN
   end

IF (@OptionName is null)
   begin
   ------- Report, then bye.

   SELECT
                   val.name   as 'Available Table Options'
                  ,val.status as 'Default Value'
         from      master..spt_values  val
         where     val.type   = 'TBO'
         and       val.number > 0
         order by  val.name

   GOTO LABEL_86_RETURN

   end


--------  Gather spt_values info (only tab opts with 'TBO' rows are docu).


SELECT
             @WOptionName        = min(val.name)
            ,@WOptionNum         = min(val.number)
            ,@int1               = count(*)
      from   master..spt_values  val
      where  val.type            = 'TBO'
      and    val.number          > 0
      and    lower(val.name) like (lower(@OptionName) + '%')

IF (@int1 <> 1)
   begin
   RaisError(15386,-1,-1,@OptionName)
   Select @RetCode = @@error
   GOTO LABEL_86_RETURN
   end

Select @charWOptionNum = convert(varchar,@WOptionNum)


-------- Parse apart the perhaps dots-qualified name.  ---------------


Execute @ExecRC = sp_namecrack_qi
       @TableNamePattern
      ,@int1         OUTPUT  -- count of name nodes in input name.
      ,@int2         OUTPUT  -- bits by position indicating " bounding found.
      ,@UnqualName   OUTPUT
      ,@QualName1    OUTPUT  -- rightmost qualifier (presumably owner).
      ,@QualName2    OUTPUT
      ,@QualName3    OUTPUT
      ,@QualName4    OUTPUT
      ,@QualName5    OUTPUT


IF (@ExecRC <> 0)
   begin
   RaisError(15253,-1,-1,@TableNamePattern,@QualName5)
   Select @RetCode = @@error
   GOTO LABEL_86_RETURN
   end


------------------  Verify database.


Select @WQualName2 = @QualName2

IF (@WQualName2 is null)
   begin
   Select @WQualName2 = @CurrentDBName
   end

IF (@WQualName2 <> @CurrentDBName)
   begin
   RaisError(15387,-1,-1)
   Select @RetCode = @@error
   GOTO LABEL_86_RETURN
   end


------------------  Settle owner.


Select @WQualName1 = @QualName1

IF (@WQualName1 is null)  -- Owner not part of any qualification.
   begin
   Select @WQualName1 = user_name()  -- '%' might not be intuitive.
   end


------------------  Are zero objects matched?


SELECT          @CountMatchedObj = count(*)
      from      sysobjects       sysobj
      where     sysobj.type      = 'U '
      and       user_name(sysobj.uid) like @WQualName1
      and       sysobj.name           like @UnqualName

IF (@CountMatchedObj = 0)
   begin
   RaisError(15388,-1,-1,@TableNamePattern)
   Select @RetCode = @@error
   GOTO LABEL_86_RETURN
   end


-------------  Project all options info into one common format/location.  -----
---- Of course, only one option name per execution.


CREATE table #t1
   (
    ObjId          integer     not null
   ,CurrentValues  integer     not null
   )
CREATE unique
   index    ix2
   on       #t1   (ObjId)


IF (@WOptionName in (@IRLString ,@BCPTabLockString)) -- sysobj.userstat
   begin

   INSERT into #t1
         (ObjId ,CurrentValues)
      SELECT
                sysobj.id  ,sysobj.userstat
         from
                sysobjects       sysobj
         where
                sysobj.type      = 'U '
         and    user_name(sysobj.uid)  like @WQualName1
         and    sysobj.name            like @UnqualName
   end

IF (@WOptionName in (@PinTabString)) -- sysobj.sysstat
   begin

   INSERT into #t1
         (ObjId ,CurrentValues)
      SELECT
                sysobj.id  ,sysobj.sysstat
         from
                sysobjects       sysobj
         where
                sysobj.type      = 'U '
         and    user_name(sysobj.uid)  like @WQualName1
         and    sysobj.name            like @UnqualName
   end


-----------------  Was option value parm specified?  ---------------


Select @WOptionValue = lower(@OptionValue)


IF (@OptionValue is null)
   begin
   --    Report current option settings, then bye.

   SELECT
                CASE
                  When t1.CurrentValues & @WOptionNum <> 0 Then 'True '
                  When t1.CurrentValues & @WOptionNum  = 0 Then 'False'
                END
                              as 'Option Status'
               ,sysobj.name   as 'Table Name'
               ,user_name(sysobj.uid)
                              as 'Owner Name'
         from
                sysobjects    sysobj
               ,#t1           t1
         where  sysobj.type   = 'U '
         and    user_name(sysobj.uid) like @WQualName1
         and    sysobj.name           like @UnqualName
         and    sysobj.id     = t1.ObjId
         order by
                1 ,2 ,3


   GOTO LABEL_86_RETURN

   end


IF (@WOptionValue in ('on' ,'yes'))
   begin
   Select @WOptionValue  = 'true'
   end

IF (@WOptionValue in ('off' ,'no'))
   begin
   Select @WOptionValue  = 'false'
   end


IF (@WOptionValue not in ('true','false'))
   begin
   RaisError(15389,-1,-1,@OptionValue)
   Select @RetCode = @@error
   GOTO LABEL_86_RETURN
   end

Select @intOptionValue =
            CASE @WOptionValue
               When 'true'  Then 1
               When 'false' Then 0
            END

--------------------------  Security  ------------------


IF (@WOptionName = @PinTabString)
   begin

   IF (suser_id() <> 1)
      begin
      RaisError(15392,-1,-1,@WOptionName)
      Select @RetCode = @@error
      GOTO LABEL_86_RETURN
      end
   end


IF (     user_id()   <> 1  --SA & DBO generally have no security restrictions.
   AND   suser_id()  <> 1
   )
   begin

   SELECT
             @int1      = count(*)  --Num of users matching pattern.
      from   sysusers   sysusr
      where  user_name(sysusr.uid) like @WQualName1

   IF (    ---- nonSA/nonDBO users can only affect their own tables.
           (@int1        > 1)
      OR
           (@int1        = 1  AND
            @WQualName1 <> user_name()
           )
      )
      begin
      RaisError(15390,-1,-1,@WOptionName)
      Select @RetCode = @@error
      GOTO LABEL_86_RETURN
      end
   end


------------------  Prepare cursor  -------------------------


IF (@intOptionValue = 0)    --Want it False,
   begin
   Select @BitLogic = ' <> '   --but is True  currently.
   end
ELSE                        --Want it True,
   begin
   Select @BitLogic = '  = '   --but is False currently.
   end


EXECUTE(
'
DECLARE         csr_11_tabid
                insensitive
            cursor for
   SELECT
                sysobj.id
         from   sysobjects    sysobj
               ,#t1           t1
         where  sysobj.type   = ''U ''
         and    user_name(sysobj.uid) like ''' +@WQualName1+ '''
         and    sysobj.name           like ''' +@UnqualName+ '''
         and
                sysobj.id     = t1.ObjId
         and
                t1.CurrentValues & ' +@charWOptionNum+ ' ' +@BitLogic+ ' 0
')
Select @Status_csr_11 = 1

Open csr_11_tabid
Select @Status_csr_11 = @Status_csr_11 | 2


WHILE (11=11)
   begin

   FETCH    next from csr_11_tabid
         into
                @TabId

   IF (@@fetch_status <> 0)
      begin
      Deallocate csr_11_tabid
      Select @Status_csr_11 = 0
      BREAK
      end

   Select @CountUpdRows2 = @CountUpdRows2 + 1


   ---- DBCC rowlock toggles a bit in sysobjects AND
   ----    internally makes the option take effect
   ----    immediately/dynamically without needed MS SQL stop/restart.

   IF (@WOptionName in (@IRLString))
      begin
      DBCC rowlock(@CurrentDBId,@TabId,@intOptionValue) -- sysobj.userstat | 0x0001
      end


   IF (@WOptionName in (@BCPTabLockString))
      begin
      DBCC bcptablock(@CurrentDBId,@TabId,@intOptionValue) -- sysobj.userstat | 0x0002 (session.h)
      end


   IF (@WOptionName in (@PinTabString))
      begin
      IF (@intOptionValue = 0)
         begin
         DBCC unpintable(@CurrentDBId,@TabId)
         end
      ELSE
         begin
         DBCC   pintable(@CurrentDBId,@TabId) -- sysobj.sysstat | 0x0200
         end
      end

   end --loop 11


-----------------  Finalization  -------------------------


LABEL_75_FINAL:

Raiserror(15391,-1,-1,@CountMatchedObj,@WOptionName,@OptionValue,@CountUpdRows2)

LABEL_86_RETURN:

IF (@Status_csr_11 > 0)
   begin
   Deallocate csr_11
   end

IF (object_id('tempdb..#t1') is not null)
   begin
            drop table #t1
   end

Return @RetCode  -- sp_tableoption
go
grant execute on sp_tableoption to public
go



if object_id('sp_fallback_MS_enroll_dev') is not null
	drop proc sp_fallback_MS_enroll_dev
raiserror(15339,-1,-1,'sp_fallback_MS_enroll_dev')
go
create procedure sp_fallback_MS_enroll_dev  --1996/04/10 12:56
    @p_db_dbid         smallint

   ,@p_dev_low          integer           OUTPUT
   ,@p_dev_high         integer           OUTPUT
   ,@p_dev_status       smallint          OUTPUT
   ,@p_dev_name         varchar(30)       OUTPUT
   ,@p_dev_phyname      varchar(127)      OUTPUT
as
/********1*********2*********3*********4*********5*********6*********7**
      This sproc's name contains the substring 'MS',
and thus is undocumented and should NOT be directly
executed by the SA.
      This sproc must be and will be executed on the primary server,
by an RPC issued from a fallback server.
      This sproc does not modify any data.
*********1*********2*********3*********4*********5*********6*********7*/

Set nocount    on
Set ansi_nulls on

Declare
       @RetCode                  integer  -- 0==good ,>0==bad ,<0==no_action
      ,@_error                   integer

-----------------------------------

Select
       @RetCode                  = 0

Select
       @p_dev_low       = null
      ,@p_dev_high      = null
      ,@p_dev_status    = null
      ,@p_dev_phyname   = null

---------------------

SELECT
             @p_dev_name      = min(sysdev.name)
      from
             master..sysdevices     sysdev
            ,master..sysusages      sysusg
            ,master..sysdatabases   sysdb
      where
             sysdev.cntrltype = 0
      and    sysdb.dbid      = @p_db_dbid
      and    sysdb.dbid      = sysusg.dbid
      and
             sysusg.vstart between sysdev.low
                           and     sysdev.high
      and
             @p_dev_name      < sysdev.name


IF (@p_dev_name is null)
   begin
   Goto LABEL_86_RETURN
   end

---------------------

SELECT
             @p_dev_low       = min(sysdev.low)
            ,@p_dev_high      = min(sysdev.high)
            ,@p_dev_status    = min(sysdev.status)
            ,@p_dev_phyname   = min(sysdev.phyname)
      from
             master..sysdevices     sysdev
      where
             @p_dev_name      = sysdev.name


LABEL_86_RETURN:

Return @RetCode -- sp_fallback_MS_enroll_dev
go
grant execute on sp_fallback_MS_enroll_dev to public
go



if object_id('sp_fallback_MS_enroll_usg') is not null
	drop proc sp_fallback_MS_enroll_usg
raiserror(15339,-1,-1,'sp_fallback_MS_enroll_usg')
go
create procedure sp_fallback_MS_enroll_usg  --1996/04/10 12:56
    @p_db_dbid         smallint

   ,@p_usg_segmap       integer        OUTPUT
   ,@p_usg_lstart       integer        OUTPUT
   ,@p_usg_size         integer        OUTPUT
   ,@p_usg_vstart       integer        OUTPUT
as
/********1*********2*********3*********4*********5*********6*********7**
      This sproc's name contains the substring 'MS',
and thus is undocumented and should NOT be directly
executed by the SA.
      This sproc must be and will be executed on the primary server,
by an RPC issued from a fallback server.
      This sproc does not modify any data.
*********1*********2*********3*********4*********5*********6*********7*/

Set nocount    on
Set ansi_nulls on

Declare
       @RetCode                  integer  -- 0==good ,>0==bad ,<0==no_action
      ,@_error                   integer
      ,@num_usg_vstart           numeric(21,0)
      ,@special_vstart           varchar(4)

-----------------------------------

Select
       @RetCode                  = 0
      ,@special_vstart           = 'no'

Select
       @p_usg_segmap    = null
      ,@p_usg_lstart    = null
      ,@p_usg_size      = null

----------------------------------------- Handle vdevno boundary

IF (@p_usg_vstart = -1)
   begin
   Select @special_vstart = 'yes'
   Select @p_usg_vstart = -2147483648  --  -(power(2,32))
   end

Select @num_usg_vstart = convert(numeric(21,0),@p_usg_vstart)

IF (    @special_vstart = 'yes'
   AND  @num_usg_vstart = -2147483648.0
   )
   Select @num_usg_vstart = @num_usg_vstart * 3.0

---------------------

SELECT
             @p_usg_vstart    = min(sysusg.vstart)
      from
             master..sysusages      sysusg
      where
             sysusg.dbid      = @p_db_dbid
      and    @num_usg_vstart  < convert(numeric(21,0),sysusg.vstart)


IF (@p_usg_vstart is null)
   begin
   Goto LABEL_86_RETURN
   end

---------------------

SELECT
             @p_usg_segmap    = min(sysusg.segmap)
            ,@p_usg_lstart    = min(sysusg.lstart)
            ,@p_usg_size      = min(sysusg.size)
      from
             master..sysusages      sysusg
      where
             sysusg.dbid      = @p_db_dbid
      and    @p_usg_vstart    = sysusg.vstart


LABEL_86_RETURN:

Return @RetCode -- sp_fallback_MS_enroll_usg
go
grant execute on sp_fallback_MS_enroll_usg to public
go



if object_id('sp_fallback_MS_verify_ri') is not null
	drop proc sp_fallback_MS_verify_ri
raiserror(15339,-1,-1,'sp_fallback_MS_verify_ri')
go
create procedure sp_fallback_MS_verify_ri  --1996/04/10 12:54
    @pPrimarySvrNamePattern   character varying(44)   = '%'
   ,@pDbNamePattern          character varying(44)   = '%'
as
/********1*********2*********3*********4*********5*********6*********7**
      This sproc should be executed on the fallback server.  Or it
is probably called by sp_fallback_help.
      This sproc is an additional level of safety.  The sp_fallback_%
sprocs use regular transactions where appropriate, but DTC is not
being used (tho perhaps it should be used in _enroll_).
      This sproc checks the internal RefInteg correctness among the
set of tables-

sysdatabases/sysdevices/sysusages.

It also checks the internal RI among the set-

spt_fallback_db/spt_fallback_dev/spt_fallback_usg.

It also checks Active/Inactive fallback status info for consistency.
*********1*********2*********3*********4*********5*********6*********7*/

Set nocount                   on
Set ansi_nulls                on

Declare
       @RetCode                  integer -- =0 is good; >0 is bad.
      ,@Int1                     integer
      ,@BogusDBId                smallint

Declare
       @WSvrName           character varying(30)
      ,@WDBId              smallint
      ,@WLow               integer
      ,@WVstart            integer

Declare
       @PrevWSvrName       character varying(30)
      ,@PrevWDBId          smallint
      ,@PrevWLow           integer
      ,@PrevWVstart        integer

-----------------------------------

Select
       @RetCode                  = 0
      ,@BogusDBId                = -1234


-----------------------------------------------------------------------
--                sys% tables RI as a single set
-----------------------------------------------------------------------


-------- Any sysusg orphans re sysdev?


SELECT       @Int1 = min(sysusg.vstart)
      from
             master..sysusages      sysusg
      where
             not exists
               (
                SELECT   *
                  from   master..sysdevices     sysdev
                  where  sysdev.cntrltype       = 0
                  and    sysusg.vstart between sysdev.low and sysdev.high
               )

IF (@Int1 is not null)
   begin
   RaisError(15366,-1,11,'sysdevices:sysusages.vstart',@Int1)  -- Do NOT internationalize these.
   Select @RetCode = @@error
   --GOTO LABEL_86_RETURN
   end


-------- Any sysusg orphans re sysdb?


SELECT       @Int1 = min(sysusg.vstart)
      from
             master..sysusages      sysusg
      where
             not exists
               (
                SELECT   *
                  from   master..sysdatabases   sysdb
                  where  sysusg.dbid            = sysdb.dbid
               )

IF (@Int1 is not null)
   begin
   RaisError(15366,-1,12,'sysdatabases:sysusages.vstart',@Int1)
   Select @RetCode = @@error
   --GOTO LABEL_86_RETURN
   end


-------- Any sysdev parents with 0 children in sysusg?


---- Error only for sysdevices actively tied to spt_fallback_dev.

SELECT       @Int1 = min(sysdev.low)
      from
             master..sysdevices        sysdev
            ,master..spt_fallback_dev  dev
      where
             dev.xserver_name like @pPrimarySvrNamePattern
      and    sysdev.cntrltype     = 0
      and    sysdev.low           = dev.xfallback_low
      and    not exists
               (
                SELECT   *
                  from   master..sysusages         sysusg
                  where  sysusg.vstart between sysdev.low and sysdev.high
               )

IF (@Int1 is not null)
   begin
   RaisError(15366,-1,13,'sysusages:sysdevices.low',@Int1)
   Select @RetCode = @@error
   --GOTO LABEL_86_RETURN
   end


-------- Any sysdb parents with 0 children in sysusg?


SELECT       @Int1 = min(sysdb.dbid)
      from
             master..sysdatabases      sysdb
      where
             not exists
               (
                SELECT   *
                  from   master..sysusages      sysusg
                  where  sysdb.dbid            = sysusg.dbid
               )

IF (@Int1 is not null)
   begin
   RaisError(15366,-1,14,'sysusages:sysdatabases.dbid',@Int1)
   Select @RetCode = @@error
   --GOTO LABEL_86_RETURN
   end



-----------------------------------------------------------------------
--                spt_fallback_% tables RI as a single set
-----------------------------------------------------------------------


-------- Any usg orphans re dev?


SELECT       @Int1 = min(usg.vstart)
      from
             master..spt_fallback_usg     usg
      where
             usg.xserver_name like @pPrimarySvrNamePattern
      and    not exists
               (
                SELECT   *
                  from   master..spt_fallback_dev     dev
                  where
                         usg.xserver_name    = dev.xserver_name
                  and    usg.vstart between dev.low and dev.high
               )

IF (@Int1 is not null)
   begin
   RaisError(15367,-1,11,'dev:spt_fallback_usg.vstart',@Int1)
   Select @RetCode = @@error
   --GOTO LABEL_86_RETURN
   end


-------- Any usg orphans re db?


SELECT       @Int1 = min(usg.vstart)
      from
             master..spt_fallback_usg     usg
      where
             usg.xserver_name like @pPrimarySvrNamePattern
      and    not exists
               (
                SELECT   *
                  from   master..spt_fallback_db     db
                  where  usg.dbid         = db.dbid
                  and    usg.xserver_name = db.xserver_name
               )

IF (@Int1 is not null)
   begin
   RaisError(15367,-1,12,'db:spt_fallback_usg.vstart',@Int1)
   Select @RetCode = @@error
   --GOTO LABEL_86_RETURN
   end


-------- Any dev parents with 0 children in usg?


SELECT       @Int1 = min(dev.low)
      from
             master..spt_fallback_dev     dev
      where
             dev.xserver_name like @pPrimarySvrNamePattern
      and    not exists
               (
                SELECT   *
                  from   master..spt_fallback_usg     usg
                  where  usg.vstart between dev.low and dev.high
                  and    usg.xserver_name    = dev.xserver_name
               )

IF (@Int1 is not null)
   begin
   RaisError(15367,-1,13,'usg:spt_fallback_dev.low',@Int1)
   Select @RetCode = @@error
   --GOTO LABEL_86_RETURN
   end


-------- Any db parents with 0 children in usg?


SELECT       @Int1 = min(db.dbid)
      from
             master..spt_fallback_db     db
      where
             db.xserver_name like @pPrimarySvrNamePattern
      and    not exists
               (
                SELECT   *
                  from   master..spt_fallback_usg     usg
                  where  usg.dbid         = db.dbid
                  and    usg.xserver_name = db.xserver_name
               )

IF (@Int1 is not null)
   begin
   RaisError(15367,-1,14,'usg:spt_fallback_db.dbid',@Int1)
   Select @RetCode = @@error
   --GOTO LABEL_86_RETURN
   end


------------------------------------------------------------------------
--       spt_fallback_% active/inactive internally consistent as a set?
------------------------------------------------------------------------


SELECT
             @WSvrName      = min(db.xserver_name)
      from
             master..spt_fallback_db     db
      where
             db.xfallback_dbid  is NOT null  -- NOT null
      and
            (exists
               (SELECT      *
                  from      master..spt_fallback_dev  dev
                           ,master..spt_fallback_usg  usg
                  where     dev.xserver_name       = db.xserver_name
                  and       usg.vstart between dev.low and dev.high
                  and       usg.dbid               = db.dbid
                  and       dev.xfallback_low      IS null
               )
            OR
             exists
               (SELECT      *
                  from      master..spt_fallback_usg  usg
                  where     usg.xserver_name       = db.xserver_name
                  and       usg.dbid               = db.dbid
                  and       usg.xfallback_vstart   IS null
               )
            )

IF (@WSvrName is not null)
   begin
   RaisError(15368,-1,11,@WSvrName)
   Select @RetCode = @@error
   --GOTO LABEL_86_RETURN
   end

--------------------------------------- NullNess

SELECT
             @WSvrName      = min(db.xserver_name)
      from
             master..spt_fallback_db     db
      where
             db.xfallback_dbid  IS null  -- IS null
      and
            (exists
               (SELECT      *
                  from      master..spt_fallback_usg  usg
                  where     usg.xserver_name       = db.xserver_name
                  and       usg.dbid               = db.dbid
                  and       usg.xfallback_vstart   is NOT null
               )
            )

IF (@WSvrName is not null)
   begin
   RaisError(15368,-1,12,@WSvrName)
   Select @RetCode = @@error
   --GOTO LABEL_86_RETURN
   end


-----------------------------------------------------------------------
--       spt_fallback_% active entries present in sys% tables?
-----------------------------------------------------------------------


-------- db/sysdb

SELECT
             @WSvrName      = min(db.xserver_name)
      from
             master..spt_fallback_db     db
      where
             db.xfallback_dbid     is not null
      and    db.xfallback_dbid     not in
               (
                SELECT      sysdb.dbid
                  from      master..sysdatabases      sysdb
               )

IF (@WSvrName is not null)
   begin
   RaisError(15369,-1,11,@WSvrName,'db')
   Select @RetCode = @@error
   --GOTO LABEL_86_RETURN
   end


-------- usg/sysusg

SELECT
             @WSvrName      = min(usg.xserver_name)
      from
             master..spt_fallback_usg     usg
      where
             usg.xfallback_vstart   is not null
      and    usg.xfallback_vstart   not in
               (
                SELECT      sysusg.vstart
                  from      master..sysusages      sysusg
               )

IF (@WSvrName is not null)
   begin
   RaisError(15369,-1,12,@WSvrName,'usg')
   Select @RetCode = @@error
   --GOTO LABEL_86_RETURN
   end


-------- dev/sysdev

SELECT
             @WSvrName      = min(dev.xserver_name)
      from
             master..spt_fallback_dev     dev
      where
             dev.xfallback_low      is not null
      and    dev.xfallback_low      not in
               (
                SELECT      sysdev.low
                  from      master..sysdevices     sysdev
                  where     sysdev.cntrltype = 0
               )

IF (@WSvrName is not null)
   begin
   RaisError(15369,-1,13,@WSvrName,'dev')
   Select @RetCode = @@error
   --GOTO LABEL_86_RETURN
   end


-----------------------------------------------------------------------
--       spt_fallback_% illegal PK duplicates / unique violations?
-----------------------------------------------------------------------

---- Even Clus idx's add 16 Kb each minimum.


-------- db dupl?

Declare csr_33_db insensitive cursor for
   SELECT       db.xserver_name
               ,db.dbid
      from      master..spt_fallback_db  db
      order by  db.xserver_name
               ,db.dbid

OPEN csr_33_db


Select
       @WSvrName     = char(0x00)  -- < 0x20 blank
      ,@WDBId        = -1

Select
       @PrevWSvrName = @WSvrName
      ,@PrevWDBId    = @WDBId


WHILE (33=33)
   begin

   FETCH     next
      from   csr_33_db
      into   @WSvrName
            ,@WDBId

   IF (@@fetch_status <> 0)
      begin
      Deallocate csr_33_db
      BREAK
      end

   IF (     @PrevWSvrName  = @WSvrName
      AND   @PrevWDBId     = @WDBId
      )
      begin
      RaisError(15381,-1,-1,@WSvrName,@WDBId)
      Select @RetCode = @@error
      end

   Select
       @PrevWSvrName = @WSvrName
      ,@PrevWDBId    = @WDBId

   end -- loop 33 db dupl


-------- dev dupl?

Declare csr_34_dev insensitive cursor for
   SELECT       dev.xserver_name
               ,dev.low
      from      master..spt_fallback_dev  dev
      order by  dev.xserver_name
               ,dev.low

OPEN csr_34_dev


Select
       @WSvrName     = char(0x00)
      ,@WLow         = -1

Select
       @PrevWSvrName = @WSvrName
      ,@PrevWLow     = @WLow


WHILE (34=34)
   begin

   FETCH     next
      from   csr_34_dev
      into   @WSvrName
            ,@WLow

   IF (@@fetch_status <> 0)
      begin
      Deallocate csr_34_dev
      BREAK
      end

   IF (     @PrevWSvrName  = @WSvrName
      AND   @PrevWLow      = @WLow
      )
      begin
      RaisError(15382,-1,-1,@WSvrName,@WLow)
      Select @RetCode = @@error
      end

   Select
       @PrevWSvrName = @WSvrName
      ,@PrevWLow     = @WLow

   end -- loop 34 dev dupl


-------- usg dupl?

Declare csr_35_usg insensitive cursor for
   SELECT       usg.xserver_name
               ,usg.vstart
      from      master..spt_fallback_usg  usg
      order by  usg.xserver_name
               ,usg.vstart

OPEN csr_35_usg


Select
       @WSvrName     = char(0x00)
      ,@WVstart      = -1

Select
       @PrevWSvrName = @WSvrName
      ,@PrevWVstart  = @WVstart


WHILE (35=35)
   begin

   FETCH     next
      from   csr_35_usg
      into   @WSvrName
            ,@WVstart

   IF (@@fetch_status <> 0)
      begin
      Deallocate csr_35_usg
      BREAK
      end

   IF (     @PrevWSvrName  = @WSvrName
      AND   @PrevWVstart   = @WVstart
      )
      begin
      RaisError(15383,-1,-1,@WSvrName,@WVstart)
      Select @RetCode = @@error
      end

   Select
       @PrevWSvrName = @WSvrName
      ,@PrevWVstart  = @WVstart

   end -- loop 35 usg dupl



--LABEL_86_RETURN:


---------------------------------------------
--    Attempt corrective actions
---------------------------------------------


IF (@RetCode > 0)
   begin

---- Make nonActive any spt_fallback_ rows lacking sys-tables link.

   Select @Int1 = 0

   UPDATE       master..spt_fallback_db
         set    xfallback_dbid   = null
         where  xfallback_dbid   is not null
         and    xfallback_dbid   not in
                  (
                  SELECT sysdb.dbid from master..sysdatabases sysdb
                  )
   Select @Int1 = @Int1 + @@rowcount

   UPDATE       master..spt_fallback_usg
         set    xfallback_vstart = null
         where  xfallback_vstart is not null
         and    xfallback_vstart not in
                  (
                  SELECT sysusg.vstart from master..sysusages sysusg
                  )
   Select @Int1 = @Int1 + @@rowcount

   UPDATE       master..spt_fallback_dev
         set    xfallback_low    = null
         where  xfallback_low    is not null
         and    xfallback_low    not in
                  (
                  SELECT sysdev.low from master..sysdevices sysdev where cntrltype = 0
                  )
   Select @Int1 = @Int1 + @@rowcount

   IF (@Int1 > 0)
      Raiserror(15357,0,1,@Int1)
   end

Return @RetCode -- sp_fallback_MS_verify_ri
go
grant execute on sp_fallback_MS_verify_ri to public
go





if object_id('sp_fallback_activate_svr_db') is not null
	drop proc sp_fallback_activate_svr_db
raiserror(15339,-1,-1,'sp_fallback_activate_svr_db')
go
create procedure sp_fallback_activate_svr_db  --1996/07/10 23:51
    @pPrimarySvrName    character varying(30)   = null
   ,@pDbNamePattern    character varying(44)   = '%'
as
/********1*********2*********3*********4*********5*********6*********7**
      This sproc should be executed on the fallback server
directly (no RPC).
      This sproc copies info from spt_fallback_% tables into
system tables, for db/dev/usg.
*********1*********2*********3*********4*********5*********6*********7*/

-- Declares will not cause "set implicit_transactions on"
--    to increment @@trancount from 0 to 1.
Declare
       @RetCode                  integer

IF (@@trancount > 0)
   begin
   RaisError(15002,-1,-1 , 'sp_fallback_activate_svr_db')
   Select @RetCode = @@error
   GOTO LABEL_86_RETURN
   end

Set implicit_transactions     off

IF (@@trancount > 0)
   begin
   COMMIT TRANSACTION   -- Decrements @@trancount by 1.
   end

Set nocount        on
Set xact_abort     off
Set ansi_defaults  off
Set ansi_nulls     on
Set ansi_padding   on
Set dateformat     ymd

Declare
       @ExecRC                   integer
      ,@_error                   integer
      ,@Int1                     integer
      ,@bin4a                    binary(4)
      ,@bin4b                    binary(4)

Declare
       @ProcStartDtTm            datetime
      ,@CountDbActivated        integer
      ,@val_high_byte            integer

Declare
       @Fallback_dbid            smallint
      ,@FallbackVdevno           smallint
      ,@Fallback_vstart          integer
      ,@Fallback_low             integer
      ,@Fallback_high            integer
      ,@VdevnoFactor             integer
      ,@OfflineBit               smallint  -- sysdatabases.status
      ,@BitDevDeferred           smallint  -- sysdevices.status
      ,@sysdb_name              character varying(30)

Declare
       @xfallback_dbid           smallint

      ,@db_name                 character varying(30)
      ,@db_dbid                 smallint
      ,@db_status               smallint
      ,@db_version              smallint

Declare
       @xfallback_drive          character(2)

      ,@dev_low                  integer
      ,@dev_high                 integer
      ,@dev_status               smallint
      ,@dev_name                 character varying(30)
      ,@dev_phyname              character varying(127)

Declare
       @usg_dbid                 smallint
      ,@usg_segmap               integer
      ,@usg_lstart               integer
      ,@usg_size                 integer
      ,@usg_vstart               integer

      ,@csr5_dev_name            character varying(30)


-----------------------------------

Select
       @RetCode                  = 0
      ,@VdevnoFactor             = 0x01000000  -- 16777216 2Kb pages is max per dev
      ,@OfflineBit               = 512
      ,@BitDevDeferred           = 8192
      ,@ProcStartDtTm            = getdate()


----------------------  only SA  --------------------------------


IF (suser_id() <> 1)  -- must be SA
   begin
   RaisError(15003,-1,-1)
   Select @RetCode = @@error
   GOTO LABEL_86_RETURN
   end

-----------------------  spt_values  ----------------------

SELECT       @val_high_byte = min(val.low)
      from   master..spt_values val
      where  type='E  '
      and    name='int4 high byte'  -- 1 in 6.5 on WinNT 3.51

IF (@val_high_byte <> 1  OR  @val_high_byte is null)
   begin
   RaisError('Invalid ''int4 high byte'' from spt_values. (%d)',11,2,@val_high_byte)
   Select @RetCode = @@error
   GOTO LABEL_86_RETURN
   end

----------------------  Need to lookup one primary svr?  --------------


IF (@pPrimarySvrName is null)
   begin

   SELECT    @Int1   = count(distinct db.xserver_name)
      from   master..spt_fallback_db  db
      where  db.xfallback_dbid        is null

   IF (@Int1 <> 1)
      begin
      RaisError(15344,-1,-1,@Int1)
      Select @RetCode = @@error
      GOTO LABEL_86_RETURN
      end

   SELECT    @pPrimarySvrName = min(db.xserver_name)
      from   master..spt_fallback_db  db
      where  db.xfallback_dbid        is null

   end


IF (@pDbNamePattern is null)
   Select @pDbNamePattern = '%'


-------------- Verify at least one db from primary svr is enrolled.  ------------


IF not exists
   (
   SELECT       *
      from      master..spt_fallback_db  db
      where     db.xserver_name = @pPrimarySvrName
      and       db.name      like @pDbNamePattern
   )
   begin
   RaisError(15345,-1,-1,@@servername,@pPrimarySvrName)
   Select @RetCode = @@error
   GOTO LABEL_86_RETURN
   end


------  Fallback already currently activated for the primary svr/db?  ------
--IF exists (SELECT * from  master..spt_fallback_db db
--                    where db.xfallback_dbid is not null
--                    and   db.xserver_name = @pPrimarySvrName
--                    and   db.name      like @pDbNamePattern
--          )
--   begin
--   RaisError(15346,-1,-1,@@servername,@pPrimarySvrName)
--   Select @RetCode = @@error
--   GOTO LABEL_86_RETURN
--   end


------------------------  Verify db version is a match  ---------------


SELECT
             @Int1            = min(db.version)
      from
             master..spt_fallback_db  db
      where
             db.xserver_name = @pPrimarySvrName
      and    db.name      like @pDbNamePattern

IF exists
   (
   SELECT       *
      from      master..spt_fallback_db  db
      where     db.xserver_name =  @pPrimarySvrName
      and       db.name       like @pDbNamePattern
      and       db.version      <> @Int1
   )
   begin
   RaisError(15347,-1,-1,@pPrimarySvrName)
   Select @RetCode = @@error
   GOTO LABEL_86_RETURN
   end


DBCC getvalue('current_version')
Select @_error = @@error

IF (@_error <> @Int1)
   begin
   RaisError(15348,-1,-1,@pPrimarySvrName,@Int1,@@servername,@_error)
   Select @RetCode = @@error
   GOTO LABEL_86_RETURN
   end


---------------  Look for foreseeable conflicts  --------------

--------

IF exists
      (
      SELECT          *
         from
                      master..spt_fallback_dev     dev
                     ,master..spt_fallback_usg     usg
                     ,master..spt_fallback_db     db
         where
                      dev.xfallback_drive is null             -- Drive info required.
         and          db.xfallback_dbid  is null
         and          db.xserver_name    = @pPrimarySvrName
         and          dev.xserver_name    = @pPrimarySvrName
         and          usg.xserver_name    = @pPrimarySvrName
         and          db.name         like @pDbNamePattern
         and          db.dbid            = usg.dbid
         and          usg.vstart between dev.low and dev.high
      )
   begin
   RaisError(15350,-1,-1)
   Select @RetCode = @@error
   GOTO LABEL_86_RETURN
   end

--------

IF exists
      (
      SELECT          *
         from
                      master..spt_fallback_dev     dev
                     ,master..spt_fallback_usg     usg
                     ,master..spt_fallback_db     db
                     ,master..sysdevices           sysdev
         where
                      dev.name            = sysdev.name        -- Unique?
         and          db.xserver_name    = @pPrimarySvrName
         and          dev.xserver_name    = @pPrimarySvrName
         and          usg.xserver_name    = @pPrimarySvrName
         and          sysdev.cntrltype    = 0
         and          db.name         like @pDbNamePattern
         and          dev.xfallback_low   is null
         and          db.xfallback_dbid  is null
         and          db.dbid            = usg.dbid
         and          usg.vstart between dev.low and dev.high
          )
   begin
   RaisError(15349,-1,-1,@@servername,@pPrimarySvrName,'logical device')
   Select @RetCode = @@error
   GOTO LABEL_86_RETURN
   end

--------

IF exists
      (
      SELECT          *
         from
                      master..spt_fallback_dev     dev
                     ,master..spt_fallback_usg     usg
                     ,master..spt_fallback_db     db
                     ,master..sysdevices           sysdev
         where
                      dev.xfallback_drive + rtrim(substring(dev.phyname,3,127))
                                          = sysdev.phyname     -- Unique?
         and          db.xserver_name    = @pPrimarySvrName
         and          dev.xserver_name    = @pPrimarySvrName
         and          usg.xserver_name    = @pPrimarySvrName
         and          sysdev.cntrltype    = 0
         and          db.name         like @pDbNamePattern
         and          dev.xfallback_low   is null
         and          db.xfallback_dbid  is null
         and          db.dbid            = usg.dbid
         and          usg.vstart between dev.low and dev.high
          )
   begin
   RaisError(15349,-1,-1,@@servername,@pPrimarySvrName,'physical device')
   Select @RetCode = @@error
   GOTO LABEL_86_RETURN
   end



---------------------------  Establish cursors  -----------------------


-------- csr db

DECLARE
                csr_3_db
                insensitive
            cursor for
   SELECT
                db.name
               ,db.dbid
               ,db.status
               ,db.version
      from
                master..spt_fallback_db     db
      where
                db.xserver_name    = @pPrimarySvrName
      and       db.name         like @pDbNamePattern
      and       db.xfallback_dbid  is null
   FOR read only

Open csr_3_db


-------- csr dev

DECLARE
                csr_4_dev
                insensitive
            cursor for
   SELECT
      distinct
                dev.xfallback_drive

               ,dev.low
               ,dev.high
               ,dev.status
               ,dev.name
               ,dev.phyname
      from
                master..spt_fallback_dev     dev
               ,master..spt_fallback_db     db
               ,master..spt_fallback_usg     usg
      where
                dev.xfallback_low   is null
      and       usg.xserver_name    = @pPrimarySvrName
      and       dev.xserver_name    = @pPrimarySvrName
      and       db.xserver_name    = @pPrimarySvrName
      and       db.dbid            = usg.dbid
      and       db.name         like @pDbNamePattern
      and       db.xfallback_dbid  is null
      and       usg.vstart between dev.low and dev.high
   FOR read only

Open csr_4_dev


-------- csr usg (w/dev)

DECLARE
                csr_5_usg
                insensitive
            cursor for
   SELECT
                usg.dbid
               ,usg.segmap
               ,usg.lstart
               ,usg.sizepg
               ,usg.vstart

               ,dev.name
      from
                master..spt_fallback_dev     dev
               ,master..spt_fallback_db     db
               ,master..spt_fallback_usg     usg
      where
                usg.xserver_name    = @pPrimarySvrName
      and       dev.xserver_name    = @pPrimarySvrName
      and       db.xserver_name    = @pPrimarySvrName
      and       db.dbid            = usg.dbid
      and       db.name         like @pDbNamePattern
      and       db.xfallback_dbid  is null
      and       usg.vstart between dev.low and dev.high
   FOR read only

Open csr_5_usg  --Cannot delay this.


-------- Online/db

DECLARE
                csr_6_online
                insensitive
            cursor for
   SELECT
                db.xfallback_dbid
               ,sysdb.name
      from
                master..spt_fallback_db     db
               ,master..sysdatabases         sysdb
      where
                db.xfallback_dbid  = sysdb.dbid
      and       db.xserver_name    = @pPrimarySvrName
      and       db.name         like @pDbNamePattern
      and       db.xdttm_last_ins_upd = @ProcStartDtTm
   FOR read only


---------------------  Work temp # tab for dynamic exec  ---------------

CREATE table #tab1_fb_act_exec (int1 integer not null)


-----------------------  Verify integrity, before txn  ---------------

Execute @ExecRC = sp_fallback_MS_verify_ri

IF (@ExecRC > 0)
   Execute sp_fallback_help

-------------------------------------------------------  Txn

BEGIN TRANSACTION   -- Increments @@trancount by 1.


---------------------  Populate system with  db  info  ---------------------

WHILE (3=3)
   begin


   FETCH
                next
      from
                csr_3_db
      into
                @db_name
               ,@db_dbid
               ,@db_status
               ,@db_version

   IF (@@fetch_status <> 0)
      begin
      Close csr_3_db

      IF (@@fetch_status = -1)
         begin
         BREAK
         end

      RaisError(15351,-1,31,@@fetch_status,'csr_3_')
      Select @RetCode = @@error
      Rollback Transaction
      GOTO LABEL_86_RETURN

      end


----------  Calculate next available dbid on fallback svr.


   Select @Fallback_dbid = null


   IF not exists (SELECT * from master..sysdatabases sysdb
                     where sysdb.dbid=@db_dbid) -- 15591
      begin
      Select @Fallback_dbid = @db_dbid
      end
   ELSE
      begin

      SELECT
                @Fallback_dbid            = min(a_sysdb.dbid) + 1
         from
                master..sysdatabases      a_sysdb
         where
                a_sysdb.dbid             < 32767
         and    not exists
                  (
                   SELECT * from  master..sysdatabases b_sysdb
                            where b_sysdb.dbid = a_sysdb.dbid + 1
                  )
      end


   UPDATE       master..spt_fallback_db
         set
                xdttm_last_ins_upd  = @ProcStartDtTm
               ,xfallback_dbid      = @Fallback_dbid
         where
                xserver_name        = @pPrimarySvrName
         and    dbid                = @db_dbid
         and    name                = @db_name


   INSERT    into   master..sysdatabases
            (
             name
            ,dbid
            ,suid
            ,mode

            ,status
            ,version
            ,logptr
            ,crdate

            ,dumptrdate
            ,category
            )
      values
            (
             @db_name
            ,@Fallback_dbid
            ,1
            ,0

            ,(@db_status | @OfflineBit)  -- Will turn off offline below.
            ,@db_version
            ,0
            ,getdate()

            ,'1944/12/28'
            ,null
            )

   Select @_error = @@error

   IF (@_error <> 0)
      begin
      RaisError(15352,-1,-1,'db ins 1')
      Select @RetCode = @_error  -- More meaningful than @@error.
      Rollback Transaction   -- Resets @@trancount = 0.
      GOTO LABEL_86_RETURN
      end

   end  -- loop 3=3 db



---------------------  Populate system with  dev  info  ---------------------

WHILE (4=4)
   begin


   FETCH
                next
      from
                csr_4_dev
      into
                @xfallback_drive

               ,@dev_low
               ,@dev_high
               ,@dev_status
               ,@dev_name
               ,@dev_phyname

   IF (@@fetch_status <> 0)
      begin
      Close csr_4_dev

      IF (@@fetch_status = -1)
         begin
         BREAK
         end

      RaisError(15351,-1,41,@@fetch_status,'csr_4_')
      Select @RetCode = @@error
      Rollback Transaction
      GOTO LABEL_86_RETURN

      end


----------  Calculate next available vdevno/low on fallback svr.

   Select @FallbackVdevno = null

   SELECT
                @FallbackVdevno = 
                   min(convert(smallint,substring(
                          convert(binary(4),a_sysdev.low),@val_high_byte,1)))
         from
                master..sysdevices             a_sysdev
         where
                a_sysdev.cntrltype  = 0
         and    a_sysdev.low       <> 0

         and    not exists
                  (SELECT  *
                     from  master..sysdevices  b_sysdev
                     where convert(smallint,substring(
                              convert(binary(4),b_sysdev.low),1,1)) = 
                           convert(smallint,substring(
                              convert(binary(4),a_sysdev.low),1,1)) + 1
                     and   b_sysdev.cntrltype = 0
                  )

   IF (    @FallbackVdevno is     null
      OR  (@FallbackVdevno is not null AND @FallbackVdevno >= 255)
      )
      begin
      RaisError('No available ''vdevno''. (%d)',11,1,@FallbackVdevno)
      Select @RetCode = @@error
      Rollback Transaction
      GOTO LABEL_86_RETURN
      end

   Select @FallbackVdevno = @FallbackVdevno + 1


-------- Now can calculate adjusted low & high.

   Select @bin4a = convert(binary(1),@FallbackVdevno)
                 + convert(binary(3),0x000000)

   Select @Fallback_low  = convert(integer,@bin4a)

   Select @Fallback_high = @Fallback_low + (@dev_high - @dev_low)


   UPDATE       master..spt_fallback_dev
         set
                xdttm_last_ins_upd  = @ProcStartDtTm
               ,xfallback_low       = @Fallback_low
         where
                xserver_name        = @pPrimarySvrName
         and    low                 = @dev_low
         and    name                = @dev_name


   INSERT    into   master..sysdevices
            (
             low
            ,high
            ,status
            ,cntrltype

            ,name
            ,phyname
            ,mirrorname
            ,stripeset
            )
      values
            (
             @Fallback_low
            ,@Fallback_high
            ,(2 + @BitDevDeferred)  -- Physical disk ,Deferred
            ,0

            ,@dev_name
            ,(@xfallback_drive + substring(@dev_phyname,3,125))
            ,null
            ,null
            )

   Select @_error = @@error

   IF (@_error <> 0)
      begin
      RaisError(15352,-1,-1,'dev ins 1')
      Select @RetCode = @_error  -- More meaningful than @@error.
      Rollback Transaction   -- Resets @@trancount = 0.
      GOTO LABEL_86_RETURN
      end

   end  -- loop 4=4 dev



---------------------  Populate system with  usg  info  ---------------------

WHILE (5=5)
   begin


   FETCH
                next
      from
                csr_5_usg
      into
                @usg_dbid
               ,@usg_segmap
               ,@usg_lstart
               ,@usg_size
               ,@usg_vstart

               ,@csr5_dev_name

   IF (@@fetch_status <> 0)
      begin
      Close csr_5_usg

      IF (@@fetch_status = -1)
         begin
         BREAK
         end

      RaisError(15351,-1,51,@@fetch_status,'csr_5_')
      Select @RetCode = @@error
      Rollback Transaction
      GOTO LABEL_86_RETURN

      end

-------------------  Lookup fallback dbid  -----------------------


   Select @Fallback_dbid = null

   SELECT
                @Fallback_dbid            = sysdb.dbid
         from
                master..sysdatabases      sysdb
               ,master..spt_fallback_db  db
               ,master..spt_fallback_usg  usg
         where
                usg.vstart                = @usg_vstart
         and    usg.xserver_name          = @pPrimarySvrName
         and    db.xserver_name          = @pPrimarySvrName
         and    db.name               like @pDbNamePattern
         and    db.dbid                  = usg.dbid
         and    db.name                  = sysdb.name


----------  Calculate vstart to match dev.low on fallback svr.


   Select @Fallback_vstart = null

   SELECT
                @Int1 = min(sysdev.low)
         from
                master..sysdevices        sysdev

               ,master..spt_fallback_dev  dev
               ,master..spt_fallback_usg  usg
         where
                dev.xserver_name          = @pPrimarySvrName
         and    usg.vstart                = @usg_vstart
         and    usg.xserver_name          = @pPrimarySvrName
         and
                sysdev.low               <> 0
         and    sysdev.cntrltype          = 0
         and
                dev.xfallback_low         = sysdev.low
         and
                sysdev.name               = dev.name
         and    usg.vstart between dev.low and dev.high

   IF (@Int1 is null)
      begin
      RaisError('Problem calculating new fallback ''vstart'' for old primary vstart=%d.'
               ,16,52,@usg_vstart)
      Select @RetCode = @@error
      Rollback Transaction
      GOTO LABEL_86_RETURN
      end


   Select @bin4a = convert(binary(4),(@usg_vstart & 0x00FFFFFF)) --Mask out vdevno.

   Select @bin4b = convert(binary(4),@Int1)
   Select @bin4a = substring(@bin4b,@val_high_byte,1)
                 + substring(@bin4a,2,3)  --FB vdevno + rest of primary vstart

   Select @Fallback_vstart = convert(integer,@bin4a)


   UPDATE       master..spt_fallback_usg
         set
                xdttm_last_ins_upd  = @ProcStartDtTm
               ,xfallback_vstart    = @Fallback_vstart
         where
                xserver_name        = @pPrimarySvrName
         and    vstart              = @usg_vstart


   INSERT    into   master..sysusages
            (
             dbid
            ,segmap
            ,lstart
            ,size
            ,vstart
            )
      values
            (
             @Fallback_dbid
            ,@usg_segmap
            ,@usg_lstart
            ,@usg_size
            ,@Fallback_vstart
            )

   Select @_error = @@error

   IF (@_error <> 0)
      begin
      RaisError(15352,-1,-1,'usg ins 1')
      Select @RetCode = @_error  -- More meaningful than @@error.
      Rollback Transaction   -- Resets @@trancount = 0.
      GOTO LABEL_86_RETURN
      end

   end  -- loop 5=5 usg


-----------------------  Online & Prepare sysdb's  ------------------


COMMIT TRANSACTION   -- Decrements @@trancount by 1.

Select @CountDbActivated = 0

OPEN csr_6_online  --This open must wait until now.


WHILE ('6a'='6a')
   begin


   FETCH
                next
      from
                csr_6_online
      into
                @xfallback_dbid
               ,@sysdb_name

   IF (@@fetch_status <> 0)
      begin
      Close csr_6_online

      IF (@@fetch_status = -1)
         begin
         BREAK
         end

      RaisError(15351,-1,61,@@fetch_status,'csr_6_ a')
      Select @RetCode = @@error
      Rollback Transaction
      GOTO LABEL_86_RETURN

      end

   Select @CountDbActivated = @CountDbActivated + 1


   Execute @ExecRC = sp_dboption @sysdb_name ,'offline' ,'false'


   DELETE from #tab1_fb_act_exec

   ---- Fix sysdatabases.logptr
   EXECUTE(
   '
   Use ' + @sysdb_name + '

   Declare @ex_logptr integer

   Select          @ex_logptr    = min(idx.first)
         from      sysindexes    idx
                  ,sysobjects    obj
         where     obj.type      = ''S ''
         and       obj.name      = ''syslogs''
         and       idx.id        = obj.id
         and       idx.indid     in (0,1) -- Should be 0 in 4.2-6.5

   INSERT into #tab1_fb_act_exec (int1) values (@ex_logptr)

   --Use master  -- Unnecessary in 6.5, should automatically revert (unlike 6.0).
   ')

   SELECT @Int1 = min(tab1.int1) from #tab1_fb_act_exec tab1

   UPDATE          master..sysdatabases
         set       logptr  = @Int1
         where     name    = @sysdb_name

   end  -- loop 6a=6a online & prepare/db


------------------------  Finalization  -------------------------


-------- Re-Verify integrity, before commit

Execute @ExecRC = sp_fallback_MS_verify_ri

IF (@ExecRC <> 0)
   begin
   RaisError(15352,-1,-1,'activate s d 2')  -- Do NOT internationalize these.
   Select @RetCode = @ExecRC
   Execute sp_fallback_help
   GOTO LABEL_86_RETURN
   end


Raiserror(15374,-1,-1,@CountDbActivated,@pPrimarySvrName)


LABEL_86_RETURN:

IF (object_id('tempdb..#tab1_fb_act_exec') is not null)
            drop table #tab1_fb_act_exec

Deallocate csr_3_db
Deallocate csr_4_dev
Deallocate csr_5_usg
Deallocate csr_6_online

Return @RetCode -- sp_fallback_activate_svr_db
go
grant execute on sp_fallback_activate_svr_db to public
go



dump transaction master with no_log
go



if object_id('sp_rename') is not null
	drop proc sp_rename
raiserror(15339,-1,-1,'sp_rename')
go
CREATE PROCEDURE sp_rename  --1996/05/13 10:28
       @objname    varchar(150)            -- Could be highly qualified.
      ,@newname        varchar( 30)
      ,@objtype  varchar( 13)   = null
as
/*************************************************
DOCUMENTATION (originally written 1996/01 for 6.5):

   [1]  To rename a table, OldName can be passed in totally
unqualified or fully qualified.
   [2]  The SA or DBO can rename objects owner by lesser users,
without the need for SetUser.
   [3]  The Owner portion of a qualified name can usually be
passed in the omitted form (as in MyDb..MyTab or MyTab).  The
typical exception is when the SA/DBO is trying to rename a table
where the OldName is present twice in sysobjects as a table
owned only by two different lesser users; requiring an explicit
owner qualifier in OldName.
   [4]  An unspecified Owner qualifier will default to the
current user if doing so will either resolve what would
otherwise be an ambiguity within ItemType, or will result
in exactly one match.
   [5]  If Database is part of the qualified OldName,
then it must match the current database.  NewName can
never be qualified.
   [6]  Uses sp_namecrack_qi, so name parms can use
quoted_identifiers (1996/05/13).
*************************************************/

Set nocount      on
Set ansi_nulls   on  -- So "(1=null OR 1<>null)" resolves to false.
Set ansi_padding on


Declare
       @OrigParm1ItemType  varchar(13)  -- max length + 1
      ,@WorkParm1ItemType  varchar(13)

Declare
       @RetCode            integer
      ,@ExecRC             integer
      ,@CurrentDb         varchar(30)
      ,@Int1               integer
      ,@Int2               integer
      ,@SetQIStatus        integer
      ,@CountNumNodes      integer
      ,@BitsQuoted         integer

Declare
       @Tab_id             integer
      ,@Tab_uid            smallint
      ,@Tab_name           varchar(30)

      ,@Obj_id             integer
      ,@Obj_uid            smallint
      ,@Obj_type           char(2)
      ,@Obj_type_name      varchar(33)

      ,@Typ_usertype       smallint
      ,@Typ_uid            smallint

      ,@Col_colid          smallint
      ,@Idx_indid          smallint
      ,@Cns_constid        integer
      ,@Cns_obj_uid        smallint

Declare
       @QualName1          varchar(30)
      ,@QualName2          varchar(30)
      ,@QualName3          varchar(30)
      ,@QualName4          varchar(30)
      ,@QualName5          varchar(30)

      ,@WQualName1         varchar(30)
      ,@WQualName2         varchar(30)
      ,@OrigNewName        varchar(30)

Declare
       @UnqualOldName      varchar(30)

--------

Select
       @RetCode         = 0  -- 0=good ,1=bad
      ,@ExecRC          = 1
      ,@UnqualOldName   = null
      ,@CurrentDb      = db_name()

      ,@Tab_id          = null
      ,@Tab_uid         = null
      ,@Tab_name        = null
      ,@Obj_id          = null
      ,@Obj_uid         = null
      ,@Obj_type        = null
      ,@Obj_type_name   = null
      ,@Typ_usertype    = null
      ,@Typ_uid         = null
      ,@Col_colid       = null
      ,@Idx_indid       = null
      ,@Cns_constid     = null
      ,@Cns_obj_uid     = null

      ,@QualName1       = null
      ,@QualName2       = null
      ,@QualName3       = null
      ,@QualName4       = null
      ,@QualName5       = null

      ,@WQualName1      = null
      ,@WQualName2      = null

Select @OrigNewName = @newname

Select @SetQIStatus = @@options & 256 --SOP, 15100

------------------------------------------------------------------------
-------------------  PHASE 10:  Simple parm edits  ---------------------
------------------------------------------------------------------------


-------- Is explicit Parm1ItemType valid?

Select @OrigParm1ItemType = @objtype

IF (@OrigParm1ItemType IS NOT null)
   begin

   Select @Int1 = 0

   IF ('column'         LIKE (lower(@OrigParm1ItemType)+'%'))
      begin
      Select @objtype  = 'Column'
            ,@Int1            = @Int1 + 1
      end
   IF ('index'          LIKE (lower(@OrigParm1ItemType)+'%'))
      begin
      Select @objtype  = 'Index'
            ,@Int1            = @Int1 + 1
      end
   IF ('object'         LIKE (lower(@OrigParm1ItemType)+'%'))
      begin
      Select @objtype  = 'Object'
            ,@Int1            = @Int1 + 1
      end
   IF ('userdatatype'   LIKE (lower(@OrigParm1ItemType)+'%'))
      begin
      Select @objtype  = 'UserDataType'
            ,@Int1            = @Int1 + 1
      end

   IF (@Int1 <> 1)
      begin
      RaisError(15249,-1,-1,@OrigParm1ItemType,@Int1)
      Select @RetCode = 1
      GOTO LABEL_86_RETURN
      end

   end
--ELSE  -- Parm1ItemType IS null
--   begin
--
--   IF (@objname LIKE '%.%')
--      begin
--      RaisError(15247,-1,-1)
--      Select @RetCode = 1
--      GOTO LABEL_86_RETURN
--      end
--   end


IF (@OrigNewName IS null)
   begin
   RaisError(15223,-1,11,'NewName')
   Select @RetCode = 1
   GOTO LABEL_86_RETURN
   end


-------- Parse apart the perhaps dots-qualified name.

IF (@objname IS null)
   begin
   RaisError(15223,-1,-1,'OldName')
   Select @RetCode = 1
   GOTO LABEL_86_RETURN
   end


Execute @ExecRC = sp_namecrack_qi
       @objname
      ,@CountNumNodes      OUTPUT  --two dots means three nodes
      ,@BitsQuoted         OUTPUT  --positional bits (2_powerof_3 bit is @QualName3)
      ,@UnqualOldName      OUTPUT  --rightmost name node
      ,@QualName1          OUTPUT  --qualifier of rightmost name node
      ,@QualName2          OUTPUT
      ,@QualName3          OUTPUT
      ,@QualName4          OUTPUT
      ,@QualName5          OUTPUT

IF (@ExecRC <> 0)
   begin
   RaisError(15253,-1,-1,@objname,@UnqualOldName) --Better if msg said "Error, name cannot be parsed."
   Select @RetCode = 1
   GOTO LABEL_86_RETURN
   end


---------------  Is NewName minimally valid?

Execute @ExecRC = sp_namecrack_qi @OrigNewName
      ,@CountNumNodes      OUTPUT
      ,@BitsQuoted         OUTPUT
      ,@newname            OUTPUT
      ,@WQualName1         OUTPUT --Do not need rest of these values.
      ,@WQualName1         OUTPUT
      ,@WQualName1         OUTPUT
      ,@WQualName1         OUTPUT
      ,@WQualName1         OUTPUT

IF (     @CountNumNodes <> 1
   OR   (@SetQIStatus=0 AND @BitsQuoted<>0)
   )
   begin
   RaisError(15224,-1,15,@newname)
   Select @RetCode = 1
   GOTO LABEL_86_RETURN
   end

IF (   @SetQIStatus = 0
   OR  @BitsQuoted  = 0
   )
   begin
   Execute @ExecRC = sp_validname @newname

   IF (     @ExecRC                    <>    0
      OR    @newname                  LIKE  '%.%'
      OR    substring(@newname,1,1)   =     '#'
      )
      begin
      RaisError(15224,-1,-1,@newname)
      Select @RetCode = 1
      GOTO LABEL_86_RETURN
      end
   end


---------------------------------------------------------------------------
----------------------  PHASE 20:  Settle Parm1ItemType  ------------------
---------------------------------------------------------------------------
---- (Plus gather related info.) ----


------------ syscolumns?

Select @WorkParm1ItemType = 'column'


IF (     lower(@objtype)  = @WorkParm1ItemType
   OR    @OrigParm1ItemType      IS null
   )
   begin

   Select
       @WQualName1 = @QualName1
      ,@WQualName2 = @QualName2  -- Owner, might be modified/defaulted from null.

---- Must handle mytab.col1 even when there are two tables with same name.
   IF (@WQualName2 IS null)  -- owner
      begin
      SELECT
                @Int1         = count(*)
         from
                sysobjects    o1
         where
                o1.type       IN ('U','V')
         and    o1.name       = @WQualName1

      IF (@Int1 > 1)
         begin  -- Resolve owner ambiguity.
         Select @WQualName2 = user_name()
         end
      end


   Select @Int1 = null

   SELECT
             @Int1         = c1.colid
            ,@Col_colid    = c1.colid
      from
             syscolumns    c1
            ,sysobjects    o1
      where
             c1.name       = @UnqualOldName
      and    o1.name       = @WQualName1
      and   (o1.uid        = user_id(@WQualName2)  OR  -- Ensures max of 1 row.
             @WQualName2   IS null
            )
      and    c1.id         = o1.id
      and    o1.id         > 99  -- Avoid system tables/views.


   IF (          (@Int1                   IS     null
            AND   lower(@objtype)  =      @WorkParm1ItemType)
      OR
                 (@Int1                   IS NOT null
            AND   isnull(lower(@objtype) ,@WorkParm1ItemType)
                                          <>     @WorkParm1ItemType)
      )
      begin
      RaisError(15248,-1,-1,@OrigParm1ItemType)
      Select @RetCode = 1
      GOTO LABEL_86_RETURN
      end

   IF (@Int1 IS NOT null)  -- Get related useful info.
      begin
      Select @objtype = @WorkParm1ItemType  -- Common to all sections.

      Select @QualName2 = @WQualName2

      SELECT
                @Tab_id       = o1.id
               ,@Tab_uid      = o1.uid
         from
                sysobjects    o1
         where
                o1.type       IN ('U','V')
         and    o1.name       = @QualName1
         and   (o1.uid        = user_id(@WQualName2)  OR
                @WQualName2   IS null
               )
      end
   end


------------ sysindexes?

Select @WorkParm1ItemType = 'index'


IF (     lower(@objtype)  = @WorkParm1ItemType
   OR    @OrigParm1ItemType      IS null
   )
   begin

   Select
       @WQualName1 = @QualName1
      ,@WQualName2 = @QualName2

   IF (@WQualName2 IS null)  -- owner
      begin
      SELECT
                @Int1         = count(*)
         from
                sysobjects    o1
         where
                o1.type       = 'U'
         and    o1.name       = @WQualName1

      IF (@Int1 > 1)
         begin  -- Resolve owner ambiguity.
         Select @WQualName2 = user_name()
         end
      end


   Select @Int1 = null

   SELECT
             @Int1         = i1.indid
            ,@Idx_indid    = i1.indid
      from
             sysindexes    i1
            ,sysobjects    o1
      where
             i1.name       = @UnqualOldName
      and    o1.name       = @WQualName1
      and   (o1.uid        = user_id(@WQualName2)  OR
             @WQualName2   IS null
            )
      and    i1.id         = o1.id
      and    i1.indid      NOT IN (0,255)
      and    o1.id         > 99


   IF (          (@Int1                   IS     null
            AND   lower(@objtype)  =      @WorkParm1ItemType)
      OR
                 (@Int1                   IS NOT null
            AND   isnull(lower(@objtype) ,@WorkParm1ItemType)
                                          <>     @WorkParm1ItemType)
      )
      begin
      RaisError(15248,-1,-1,@OrigParm1ItemType)
      Select @RetCode = 1
      GOTO LABEL_86_RETURN
      end

   IF (@Int1 IS NOT null)
      begin
      Select @objtype = @WorkParm1ItemType

      Select @QualName2 = @WQualName2

      SELECT
                @Tab_id       = o1.id
               ,@Tab_uid      = o1.uid
         from
                sysobjects    o1
         where
                o1.type       = 'U'
         and    o1.name       = @WQualName1
         and   (o1.uid        = user_id(@WQualName2)  OR
                @WQualName2   IS null
               )

      SELECT
                @Cns_constid     = c2.constid
               ,@Cns_obj_uid     = o1.uid
         from
                sysconstraints   c2
               ,sysindexes       i1
               ,sysobjects       o1
         where
                i1.id            = c2.id
         and    i1.id            = @Tab_id
         and    i1.indid         = @Idx_indid
         and    o1.id            = c2.constid
         and    o1.name          = @UnqualOldName
         and    o1.uid           = @Tab_uid
      end
   end


------------ sysobjects.

Select @WorkParm1ItemType = 'object'


IF (     lower(@objtype)  = @WorkParm1ItemType
   OR    @OrigParm1ItemType      IS null
   )
   begin

   Select
       @WQualName1 = @QualName1

   IF (@WQualName1 IS null)  -- owner
      begin
      SELECT
                @Int1         = count(*)
         from
                sysobjects    o1
         where
                o1.name       = @UnqualOldName

      IF (@Int1 > 1)
         begin  -- Resolve owner ambiguity.
         Select @WQualName1 = user_name()
         end
      end


   Select @Int1 = null

   SELECT
             @Int1         = o1.id
            ,@Obj_id       = o1.id
            ,@Obj_uid      = o1.uid
            ,@Obj_type     = o1.type
      from
             sysobjects    o1
      where
             o1.name       = @UnqualOldName
      and   (o1.uid        = user_id(@WQualName1)  OR
             @WQualName1   IS null
            )
      and    o1.id         > 99  -- Avoid system objects.


   IF (          (@Int1                   IS     null
            AND   lower(@objtype)  =      @WorkParm1ItemType)
      OR
                 (@Int1                   IS NOT null
            AND   isnull(lower(@objtype) ,@WorkParm1ItemType)
                                          <>     @WorkParm1ItemType)
      )
      begin
      RaisError(15248,-1,-1,@OrigParm1ItemType)
      Select @RetCode = 1
      GOTO LABEL_86_RETURN
      end


   IF (@Int1 IS NOT null)
      begin
      Select @objtype = @WorkParm1ItemType

      Select @QualName1 = @WQualName1


      Select @Obj_type_name =
               CASE @Obj_type
                  When 'D ' Then 'Default'
                  When 'F ' Then 'Foreign Key'
                  When 'K ' Then 'Primary/Unique Key'
                  When 'P ' Then 'Procedure'
                  When 'R ' Then 'Rule'
                  When 'RF' Then 'Replication Filter' -- ?
                  When 'TR' Then 'Trigger'
                  When 'V ' Then 'View'
                  Else           ' ? '
               END

--      IF (@Obj_type IN ('D','P','R' ,'RF','TR','V') ) -- 'C' lacks name in syscomments.text
--         begin
--         Raiserror(15336,-1,-1,@Obj_type_name)  -- "dbcc upgradedb(null)" seems to handle these discrepancies.
--         Select @RetCode = 1
--         GOTO LABEL_86_RETURN
--         end


      IF (@Obj_type = 'K')
         begin

         SELECT
                   @Tab_id          = o2.id
                  ,@Tab_uid         = o2.uid
                  ,@Tab_name        = o2.name
            from
                   sysobjects       o1  -- cns=obj
                  ,sysobjects       o2  -- tab
                  ,sysconstraints   c2
            where
                   c2.constid       = @Obj_id
            and    c2.id            = o2.id
            and    c2.constid       = o1.id


         SELECT
                   @Idx_indid       = i1.indid
            from
                   sysindexes       i1
            where
                   i1.id            = @Tab_id
            and    i1.name          = @UnqualOldName

         end
      end
   end


------------ systypes.

Select @WorkParm1ItemType = 'userdatatype'


IF (     lower(@objtype)  = @WorkParm1ItemType
   OR    @OrigParm1ItemType      IS null
   )
   begin
---- systypes.name is unique, without uid.

   Select @Int1 = null

   SELECT
             @Int1         = t1.usertype
            ,@Typ_usertype = t1.usertype
            ,@Typ_uid      = t1.uid
      from
             systypes      t1
      where
             t1.name       = @UnqualOldName
      and   (t1.uid        = user_id(@QualName1)  OR
             @QualName1 IS null
            )
      and    t1.usertype   > 99  -- Avoid system datatypes.


   IF (          (@Int1                   IS     null
            AND   lower(@objtype)  =      @WorkParm1ItemType)
      OR
                 (@Int1                   IS NOT null
            AND   isnull(lower(@objtype) ,@WorkParm1ItemType)
                                          <>     @WorkParm1ItemType)
      )
      begin
      RaisError(15248,-1,-1,@OrigParm1ItemType)
      Select @RetCode = 1
      GOTO LABEL_86_RETURN
      end

   IF (@Int1 IS NOT null)
      begin
      Select @objtype = @WorkParm1ItemType
      end
   end



LABEL_28_AFTERPARM1ITEMTYPE:



-----------------  Valid Parm1ItemType?  ---------------------------


-------- Parm1ItemType determined?

   IF (@objtype IS null)
      begin
      RaisError(15225,-1,-1,@objname,@CurrentDb,@OrigParm1ItemType)
      Select @RetCode = 1
      GOTO LABEL_86_RETURN
      end


-------- Valid combination?

   IF (     lower(@objtype)  IN       ('column','index')
      AND   @objname           NOT LIKE '%.%'
      )
      begin
      RaisError(15332,-1,-1,@objtype)
      Select @RetCode = 1
      GOTO LABEL_86_RETURN
      end



---------------------------------------------------------------------
-------------------  PHASE 30:  More parm edits  --------------------
---------------------------------------------------------------------


---------------  verify any database qualifier matches current db  -----------


IF       (lower(@objtype) IN ('column','index'))
         begin
         IF (isnull(@QualName3,@CurrentDb) <> @CurrentDb)
            begin
            RaisError(15333,-1,-1,@QualName3)
            Select @RetCode = 1
            GOTO LABEL_86_RETURN
            end
         end
ELSE IF  (lower(@objtype) IN ('object','userdatatype'))
         begin
         IF (isnull(@QualName2,@CurrentDb) <> @CurrentDb)
            begin
            RaisError(15333,-1,-1,@QualName2)
            Select @RetCode = 1
            GOTO LABEL_86_RETURN
            end
         end
ELSE
         begin  -- Never will happen.
         RaisError('Error, this procedure has calculated a null or unrecognized ItemType value (%s).   Please give an explicit value for ItemType and retry.'
                  ,11,1,@objtype)
         Select @RetCode = 1
         GOTO LABEL_86_RETURN
         end


-------------------  Validate ownership/authorization issues  ---------------


-------- systypes?

IF (lower(@objtype) = 'userdatatype')
   begin

   IF (     suser_id()  <> 1
      AND   user_id()   <> 1
      AND   @Typ_uid    <> user_id()
      )
      begin
      RaisError(15334,-1,-1)
      Select @RetCode = 1
      GOTO LABEL_86_RETURN
      end
   end


IF (lower(@objtype) = 'object')
   begin

   IF (     suser_id()  <> 1
      AND   user_id()   <> 1
      AND   @Obj_uid    <> user_id()
      )
      begin
      RaisError(15334,-1,-1)
      Select @RetCode = 1
      GOTO LABEL_86_RETURN
      end
   end


IF (lower(@objtype) IN ('index','column'))
   begin

   IF (     suser_id()  <> 1
      AND   user_id()   <> 1
      AND   @Tab_uid    <> user_id()
      )
      begin
      RaisError(15334,-1,-1)
      Select @RetCode = 1
      GOTO LABEL_86_RETURN
      end
   end


-------------------  Catch NewName clashes now before Upd  -----------------


IF (lower(@objtype) = 'column')
   begin

   IF exists (SELECT * from syscolumns c1 where c1.id=@Tab_id
                  and c1.name=@newname and c1.name<>@UnqualOldName
             )
      begin
      RaisError(15335,-1,-1,@newname,@objtype)
      Select @RetCode = 1
      GOTO LABEL_86_RETURN
      end
   end


IF (lower(@objtype) = 'index')
   begin

   IF exists (   (SELECT * from sysindexes i1 where i1.id=@Tab_id
                     and i1.name=@newname and i1.name<>@UnqualOldName)
             )
      begin
      RaisError(15335,-1,-1,@newname,@objtype)
      Select @RetCode = 1
      GOTO LABEL_86_RETURN
      end

   IF (@Cns_constid IS NOT null)
      begin

      IF exists (SELECT * from sysobjects o1 ,sysconstraints c2
                     where c2.constid=@Cns_constid and o1.id<>@Cns_constid
                     and   o1.name=@UnqualOldName  and o1.uid=@Cns_obj_uid
                )
         begin
         RaisError(15335,-1,-1,@newname,@objtype)
         Select @RetCode = 1
         GOTO LABEL_86_RETURN
         end
      end
   end


IF (lower(@objtype) = 'object')
   begin

   IF exists (SELECT * from sysobjects o1
                  where o1.name=@newname and o1.id<>@Obj_id
                  and   o1.uid=@Obj_uid
             )
      begin
      RaisError(15335,-1,-1,@newname,@objtype)
      Select @RetCode = 1
      GOTO LABEL_86_RETURN
      end

---- Indirect duplicates object related?

   IF (@Obj_type IN ('K'))
      begin

      ---- other indexes?
      SELECT
                @Int1            = count(*)
         from
                sysindexes       i1
               ,sysconstraints   c2
         where
                c2.constid       = @Obj_id
         and    i1.name         <> @UnqualOldName
         and    i1.name          = @newname  -- Not worry about 't'ext clashes.
         and    c2.id            = i1.id

      IF (@Int1 > 0)
         begin
         RaisError(15335,-1,-1,@newname,@objtype)
         Select @RetCode = 1
         GOTO LABEL_86_RETURN
         end
      end
   end


IF (lower(@objtype) = 'userdatatype')
   begin

   IF exists (SELECT * from systypes t1
                  where t1.name=@newname and t1.name<>@UnqualOldName
             )
      begin
      RaisError(15335,-1,-1,@newname,@objtype)
      Select @RetCode = 1
      GOTO LABEL_86_RETURN
      end
   end


--------------------------------------------------------------------------
--------------------  PHASE 34:  Cautionary messages  --------------------
--------------------------------------------------------------------------


IF (lower(@objtype) = 'objects')
   begin

   IF exists (SELECT * from sysdepends where depid = @Obj_id)
      begin
      Raiserror(15337,-1,-1)
      end
   end



--------------------------------------------------------------------------
---------------------  PHASE 40:  Update system tables  ------------------
--------------------------------------------------------------------------


-------- Upd systypes

IF (lower(@objtype) = 'userdatatype')
   begin


   BEGIN  TRANSACTION txn_typ1_sp_rename

   UPDATE
             systypes
      set
             name       = @newname
      where
             usertype   = @Typ_usertype

   ---- These Upds should never error, thus do not bother with sysmessages.
   IF (@@error<>0 or @@rowcount<>1)
      begin
      RaisError('Error during Update, issuing Rollback! (%s)',16,1,'txn_typ1,name')
      Rollback Transaction
      Select @RetCode = 1
      GOTO LABEL_86_RETURN
      end

   COMMIT TRANSACTION txn_typ1_sp_rename

   end


-------- Upd sysobjects

IF (lower(@objtype) = 'object')
   begin

   IF (@Obj_type = 'K ')  -- P/U Key
      begin

      BEGIN  TRANSACTION txn_obj2_sp_rename


      UPDATE
                sysindexes
         set
                name       = @newname
         where
                id         = @Tab_id
         and    indid      = @Idx_indid

      IF (@@error<>0 or @@rowcount<>1)
         begin
         RaisError('Error during Update, issuing Rollback! (%s)',16,1,'txn_obj2,k10')
         Rollback Transaction
         Select @RetCode = 1
         GOTO LABEL_86_RETURN
         end


      UPDATE
                sysobjects
         set
                name       = @newname  -- Constraint renamed.
         where
                id         = @Obj_id

      IF (@@error<>0 or @@rowcount<>1)
         begin
         RaisError('Error during Update, issuing Rollback! (%s)',16,1,'txn_obj2,k20')
         Rollback Transaction
         Select @RetCode = 1
         GOTO LABEL_86_RETURN
         end


      UPDATE
                sysobjects
         set
                schema_ver = schema_ver + 1  -- Force recompiles.
         where
                id         = @Tab_id

      IF (@@error<>0 or @@rowcount<>1)
         begin
         RaisError('Error during Update, issuing Rollback! (%s)',16,1,'txn_obj2,k30')
         Rollback Transaction
         Select @RetCode = 1
         GOTO LABEL_86_RETURN
         end


      COMMIT TRANSACTION txn_obj2_sp_rename


      GOTO LABEL_51_AFTERUPDATES  ------------ Jumping.

      end



-------- Default "plain" objects section.


   SELECT
             @Int1      = min(i1.indid)
            ,@Int2      = max(i1.indid)
      from
             sysindexes i1
      where
             i1.id      = @Obj_id


   BEGIN  TRANSACTION txn_obj1_sp_rename


   UPDATE
             sysobjects
      set
             name       = @newname
            ,schema_ver = schema_ver + 1
      where
             id         = @Obj_id

   IF (@@error<>0 or @@rowcount<>1)
      begin
      RaisError('Error during Update, issuing Rollback! (%s)',16,1,'txn_obj1,schema_ver')
      Rollback Transaction
      Select @RetCode = 1
      GOTO LABEL_86_RETURN
      end


   IF (isnull(@Int1,-123) = 0)  -- Tab with no clus Idx.
      begin

      UPDATE
             sysindexes
         set
             name    = @newname
         where
             id      = @Obj_id
         and indid   = 0

      IF (@@error<>0 or @@rowcount<>1)
         begin
         RaisError('Error during Update, issuing Rollback! (%s)',16,1,'txn_obj1,0')
         Rollback Transaction
         Select @RetCode = 1
         GOTO LABEL_86_RETURN
         end
      end


   IF (isnull(@Int2,-123) = 255)  -- 't'ext col in tab.
      begin

      UPDATE
             sysindexes
         set
             name    = 't' + substring(@newname,1,29)
         where
             id      = @Obj_id
         and indid   = 255

      IF (@@error<>0 or @@rowcount<>1)  -- Only 1 indid=255 row for all text+image cols per table.
         begin
         RaisError('Error during Update, issuing Rollback! (%s)',16,1,'txn_obj1,255')
         Rollback Transaction
         Select @RetCode = 1
         GOTO LABEL_86_RETURN
         end
      end


   COMMIT TRANSACTION txn_obj1_sp_rename

   end


-------- Upd sysindexes

IF (lower(@objtype) = 'index')
   begin


   BEGIN  TRANSACTION txn_idx1_sp_rename

   UPDATE
             sysindexes
      set
             name       = @newname
      where
             id         = @Tab_id
      and    indid      = @Idx_indid

   IF (@@error<>0 or @@rowcount<>1)
      begin
      RaisError('Error during Update, issuing Rollback! (%s)',16,1,'txn_idx1,idx name')
      Rollback Transaction
      Select @RetCode = 1
      GOTO LABEL_86_RETURN
      end


   IF (@Cns_constid IS NOT null)
      begin
      UPDATE
             sysobjects
         set
             name       = @newname
         where
             id         = @Cns_constid

      IF (@@error<>0 or @@rowcount<>1)
         begin
         RaisError('Error during Update, issuing Rollback! (%s)',16,1,'txn_idx1,const name')
         Rollback Transaction
         Select @RetCode = 1
         GOTO LABEL_86_RETURN
         end

      UPDATE
             sysobjects
         set
             schema_ver = schema_ver + 1
         where
             id         = @Tab_id

      IF (@@error<>0 or @@rowcount<>1)
         begin
         RaisError('Error during Update, issuing Rollback! (%s)',16,1,'txn_idx1,schema_ver b')
         Rollback Transaction
         Select @RetCode = 1
         GOTO LABEL_86_RETURN
         end
      end


   COMMIT TRANSACTION txn_idx1_sp_rename

   end


-------- Upd syscolumns

IF (lower(@objtype) = 'column')
   begin


   BEGIN  TRANSACTION txn_col1_sp_rename

   UPDATE
             syscolumns
      set
             name       = @newname
      where
             id         = @Tab_id
      and    name       = @UnqualOldName

   IF (@@error<>0 or @@rowcount<>1)
      begin
      RaisError('Error during Update, issuing Rollback! (%s)',16,1,'txn_col1,col name')
      Rollback Transaction
      Select @RetCode = 1
      GOTO LABEL_86_RETURN
      end

   -- Perhaps schema_ver+1 good here.

   COMMIT TRANSACTION txn_col1_sp_rename


   GOTO LABEL_51_AFTERUPDATES

   end


LABEL_51_AFTERUPDATES:


-------------------------  Finalization  -----------------------


Raiserror(15338,-1,-1,@objtype,@newname)


LABEL_86_RETURN:

Return @RetCode --sp_rename
go
grant execute on sp_rename to public
go




if object_id('sp_password') is not null
	drop proc sp_password
raiserror(15339,-1,-1,'sp_password')
go
create procedure sp_password  --1996/05/31 11:06 14841
    @old       varchar(30) = NULL --old (current) password
   ,@new       varchar(30)        --new password
   ,@loginame  varchar(30) = NULL --login whose pw to change
as

set nocount    on
set ansi_nulls on

declare
       @w_suid                integer     --suid of login whose pw to change.
      ,@w_loginame            varchar(30) --name of login whose pw to change.
      ,@must_verify_old_pw    integer
      ,@int2                  integer


-- If in a transaction, disallow this since it might make recovery impossible.

if @@trancount > 0
   begin
   raiserror(15002,-1,-1,'sp_password')
   return (1)
   end


-- Determine name & suid of login whose pw is to change.

if @loginame is null
   begin
   select @w_loginame = suser_name()
   end
else
   begin
   select @w_loginame = @loginame
   end

SELECT       @w_suid       = min(l.suid)
          --,@w_password   = l.password --Encrypted.
      from   master..syslogins l
      where  l.name        = @w_loginame

if @w_suid is null
   begin
   raiserror(15007,-1,-1,@loginame) --No such login name.
   return (1)
   end


-- Enforce that only SA can change pw of other logins.

if       suser_id() <> @w_suid
   AND   suser_id() <> 1
   begin
   raiserror(15210,-1,-1) --Wording might wrongly imply nonSA's cannot specify their own login name; they can.
   return (1)
   end


-- Verify old/present pw was passed in accurately.

select @must_verify_old_pw = 0 --Assume at start, 0 meaning old pw need Not be checked.

if suser_id() = 1 --SA is running this sproc.
   begin
   if @old is not null --Then it has to be accurate.
      begin
      select @must_verify_old_pw = 1
      end
   end
else  --nonSA running this.
   begin
   select @must_verify_old_pw = 1
   end

if @must_verify_old_pw <> 0 --Must verify accuracy of old/present pw.
   begin
   SELECT       @int2      = count(*)
         from   master..syslogins l
         where  l.suid     = @w_suid
         and   (isnull(l.password,' null pw ') = isnull(@old,' null pw ')
            or  pwdcompare(@old,l.password) = 1
               )

   if @int2 = 0
      begin
      raiserror(15211,-1,-1) --Old pw mismatched.
      return (1)
      end
   end


-- Everything is valid, so change the pw and set encrypted flag.

UPDATE       master..syslogins
      set    password = pwdencrypt(@new)
            ,status   = status | 8
      where  suid     = @w_suid

print 'Password changed.'

return (0)
go
grant execute on sp_password to public
go




if object_id('sp_processmail') is not null
	drop proc sp_processmail
raiserror(15339,-1,-1,'sp_processmail')
go
create procedure sp_processmail --1996/06/19 17:30
	@subject varchar(255)=NULL,
	@filetype varchar(3)='txt',
	@separator varchar(3)='tab',
	@set_user varchar(30)='guest',
	@dbuse varchar(30)='master'
as

declare @status int
declare @msg_id varchar(64)
declare @originator varchar(255)
declare @cc_list varchar(255)
declare @msgsubject varchar(255)
declare @query varchar(255)
declare @messages int
declare @mapifailure int
declare @resultmsg varchar(80)
declare @filename varchar(12)
declare @current_msg varchar(64)

select @messages=0
select @mapifailure=0

if @separator='tab' select @separator=CHAR(9)

/* get first message id */
exec @status = master..xp_findnextmsg
		@msg_id=@msg_id output,
		@unread_only='true'

if @status <> 0
	select @mapifailure=1
while (@mapifailure=0)
  begin


    if @msg_id is null break

    exec @status = master..xp_readmail
		@msg_id=@msg_id,
		@originator=@originator output,
		@cc_list=@cc_list output,
		@subject=@msgsubject output,
		@message=@query output,
		@peek='true',		@suppress_attach='true'

    if @status <> 0
	begin
		select @mapifailure=1
		break
	end

    /* get new message id before processing & deleting current */
	select @current_msg=@msg_id
	exec @status = master..xp_findnextmsg
		@msg_id=@msg_id output,
		@unread_only='true'

    	if @status <> 0
	begin
		select @mapifailure=1		break
	end


    if ((@subject IS NULL) OR (@subject=@msgsubject))
    begin
	/* generate random filename */
	select @filename='SQL' + convert(varchar,ROUND(RAND()*100000,0)) + '.' + @filetype

	exec @status = master..xp_sendmail
			@recipients=@originator,
			@copy_recipients=@cc_list,
			@message=@query,
			@query=@query,
			@subject='Query Results',
			@separator=@separator,
			@width=256,
			@attachments=@filename,
			@attach_results='true',
			@no_output='false',
			@echo_error='true',
			@set_user=@set_user,
			@dbuse=@dbuse

	if @status <> 0
		begin
			select @mapifailure=1
			break		
		end
	
	select @messages=@messages+1

	exec master..xp_deletemail @current_msg

    end /* end of xp_sendmail block */
  end  /* end of xp_findnextmsg loop */

  /* finished examining the contents of inbox;  now send results */
  if @mapifailure=0
      	begin
		raiserror(15079,-1,-1,@messages)
		return(1)
	end
  else	
	return(0)
go
---- No GRANT for sp_processmail




if object_id('sp_helpconstraint') is not null
	drop proc sp_helpconstraint
raiserror(15339,-1,-1,'sp_helpconstraint')
go
create procedure sp_helpconstraint --1996/07/02 16:33
    @objname varchar(92)             -- the table to check for constraints
   ,@nomsg   varchar(5) = 'msg'      -- 'nomsg' supresses printing of TBName (sp_help)
as

set nocount on

declare @objid    int           -- the object id of the table
       ,@indid    int           -- the index id of an index
       ,@cnstdes  varchar(255)  -- string to build up index desc
       ,@cnstname varchar(30)   -- name of const. currently under consideration
       ,@tptr     varbinary(16) -- pointer for building text strings.
       ,@i          int
       ,@thiskey    varchar(32)
       ,@cnstid     int
       ,@cnststatus int
       ,@numkeys    int
       ,@rkeyid     int
       ,@fkeyid     int
       ,@dbname     varchar(30)

declare
         @fkey1  int ,@fkey2  int ,@fkey3  int ,@fkey4  int ,@fkey5  int
        ,@fkey6  int ,@fkey7  int ,@fkey8  int ,@fkey9  int ,@fkey10 int
        ,@fkey11 int ,@fkey12 int ,@fkey13 int ,@fkey14 int ,@fkey15 int
        ,@fkey16 int

declare
         @rkey1  int ,@rkey2  int ,@rkey3  int ,@rkey4  int ,@rkey5  int
        ,@rkey6  int ,@rkey7  int ,@rkey8  int ,@rkey9  int ,@rkey10 int
        ,@rkey11 int ,@rkey12 int ,@rkey13 int ,@rkey14 int ,@rkey15 int
        ,@rkey16 int

declare
       @bitDisabled           integer
      ,@bitNotForReplication  integer
--------

select
       @bitDisabled           = 0x4000
      ,@bitNotForReplication  = 0x200000


---- Check to see that the object names are local to the current database.

if      @objname like '%.%.%'
   and  substring(@objname, 1, charindex('.', @objname) - 1) <> db_name()
   begin
   raiserror(15250,-1,-1)
   return (1)
   end


---- Check to see if the table exists and initialize @objid.

select @objid = object_id(@objname)


---- Table does not exist so return.

if @objid is NULL
   begin
   select @dbname=db_name()
   raiserror(15009,-1,-1,@objname,@dbname)
   return (1)
   end


declare cnst_csr insensitive cursor for -- 15574 in 6.5 dynamic unless insensitive
     select   c.constid, c.status, o.name
        from  sysconstraints c, sysobjects o
        where c.id = @objid and o.id = c.constid
        for read only


---- Now check out each constraint, figure out its type and keys and
---- save the info in a temporary table that we'll print out at the end.

create table #spcnsttab
(
    rowid               int           NOT NULL  identity
   ,cnst_type           varchar(48)   NOT NULL   -- 30 for name + text for DEFAULT
   ,cnst_name           varchar(30)   NOT NULL
   ,cnst_nonblank_name  varchar(30)   NOT NULL
   ,cnst_status         integer           NULL
   ,cnst_keys           text              NULL
)

create table #spcnstkeys
(
    cnst_colid int NOT NULL
)

open cnst_csr

fetch cnst_csr into @cnstid, @cnststatus, @cnstname

while @@fetch_status >= 0
   begin

   if ((@cnststatus & 0xf) in (1,2)) -- primary key, unique
      begin

      if ((@cnststatus & 0xf) = 1)
         select @cnstdes = 'PRIMARY KEY'
      else
         select @cnstdes = 'UNIQUE'

      select   @indid = indid
         from  sysindexes
         where sysindexes.name = OBJECT_NAME(@cnstid)
        and sysindexes.id = @objid

      if (@indid > 1)
         select @cnstdes = @cnstdes + ' (non-clustered)'
      else
         select @cnstdes = @cnstdes + ' (clustered)'

      ---- First we'll figure out what the keys are.

      select @i = 1

      while (@i <= 16)
         begin

         select @thiskey = index_col(@objname, @indid, @i)

         if @thiskey is NULL
            goto keysdone

         if @i=1
            begin
            insert into #spcnsttab (cnst_type,cnst_name,cnst_nonblank_name,cnst_keys)
               values (@cnstdes, @cnstname,@cnstname, @thiskey)

            select @tptr = textptr(cnst_keys) from #spcnsttab
            end
         else
            begin
            select @thiskey = ', ' + @thiskey

            if @tptr is not null
               updatetext #spcnsttab.cnst_keys @tptr null null @thiskey
            end

         select @i = @i + 1
         end --loop 16


         ---- When we get here we now have all the keys.

keysdone:

      end
   else   -- not pkey,ukey

      if ((@cnststatus & 0xf) = 3) /* foreign key */
         begin
         select @cnstdes = 'FOREIGN KEY'

         select
                @fkeyid = fkeyid, @rkeyid = rkeyid,
                @fkey1=fkey1, @fkey2=fkey2, @fkey3=fkey3,
                @fkey4=fkey4, @fkey5=fkey5, @fkey6=fkey6,
                @fkey7=fkey7, @fkey8=fkey8, @fkey9=fkey9,
                @fkey10=fkey10, @fkey11=fkey11,
                @fkey12=fkey12, @fkey13=fkey13,
                @fkey14=fkey14, @fkey15=fkey15,
                @fkey16=fkey16,
                @rkey1=rkey1, @rkey2=rkey2, @rkey3=rkey3,
                @rkey4=rkey4, @rkey5=rkey5, @rkey6=rkey6,
                @rkey7=rkey7, @rkey8=rkey8, @rkey9=rkey9,
                @rkey10=rkey10, @rkey11=rkey11,
                @rkey12=rkey12, @rkey13=rkey13,
                @rkey14=rkey14, @rkey15=rkey15,
                @rkey16=rkey16
            from  sysreferences
            where constid = @cnstid

         insert into #spcnstkeys values(@fkey1)
         insert into #spcnstkeys values(@fkey2)
         insert into #spcnstkeys values(@fkey3)
         insert into #spcnstkeys values(@fkey4)
         insert into #spcnstkeys values(@fkey5)
         insert into #spcnstkeys values(@fkey6)
         insert into #spcnstkeys values(@fkey7)
         insert into #spcnstkeys values(@fkey8)
         insert into #spcnstkeys values(@fkey9)
         insert into #spcnstkeys values(@fkey10)
         insert into #spcnstkeys values(@fkey11)
         insert into #spcnstkeys values(@fkey12)
         insert into #spcnstkeys values(@fkey13)
         insert into #spcnstkeys values(@fkey14)
         insert into #spcnstkeys values(@fkey15)
         insert into #spcnstkeys values(@fkey16)

         delete from #spcnstkeys where cnst_colid = 0

         ---- Need a unique index so we can use a cursor.

         create unique index ind1 on #spcnstkeys(cnst_colid)

         execute('declare fkey_curs cursor for
                     select cnst_colid from #spcnstkeys
                     for read only')

         open fkey_curs

         fetch fkey_curs into @i

         select @numkeys=1

         while @@fetch_status >= 0
            begin
            select @thiskey = col_name(@fkeyid, @i)

            ---- No comma for fist key column.

            if @numkeys = 1
               begin
               insert into #spcnsttab (cnst_type,cnst_name,cnst_nonblank_name,cnst_keys)
                  values (@cnstdes, @cnstname,@cnstname, @thiskey)

               select @tptr = textptr(cnst_keys) from #spcnsttab
               end
            else
               begin
               select @thiskey = ', ' + @thiskey

               if @tptr is not null
                  updatetext #spcnsttab.cnst_keys @tptr null null @thiskey
               end

            select @numkeys=@numkeys+1

            fetch fkey_curs into @i
            end --loop key fetch


         ---- When we get here we now have all the keys.

         truncate table #spcnstkeys

         drop index #spcnstkeys.ind1

         deallocate fkey_curs

         insert into #spcnstkeys values(@rkey1)
         insert into #spcnstkeys values(@rkey2)
         insert into #spcnstkeys values(@rkey3)
         insert into #spcnstkeys values(@rkey4)
         insert into #spcnstkeys values(@rkey5)
         insert into #spcnstkeys values(@rkey6)
         insert into #spcnstkeys values(@rkey7)
         insert into #spcnstkeys values(@rkey8)
         insert into #spcnstkeys values(@rkey9)
         insert into #spcnstkeys values(@rkey10)
         insert into #spcnstkeys values(@rkey11)
         insert into #spcnstkeys values(@rkey12)
         insert into #spcnstkeys values(@rkey13)
         insert into #spcnstkeys values(@rkey14)
         insert into #spcnstkeys values(@rkey15)
         insert into #spcnstkeys values(@rkey16)

         delete from #spcnstkeys where cnst_colid = 0

         ---- Need a unique index so we can use a cursor.

         create unique index ind1 on #spcnstkeys(cnst_colid)

         insert into #spcnsttab (cnst_type,cnst_name,cnst_nonblank_name,cnst_keys)
            select
                      ' ' ,' ' ,@cnstname
                     ,'REFERENCES ' + rtrim(db_name(rkeydbid))
                           + '.' + rtrim(
                  (select user_name(uid) from sysobjects where id = @rkeyid
                  )
                                        )
                           + '.'+object_name(@rkeyid) + ' ('
               from  sysreferences
               where constid = @cnstid

         select @tptr = textptr(cnst_keys) from #spcnsttab

         execute('declare rkey_curs cursor for
                     select cnst_colid from #spcnstkeys
                     for read only')

         open rkey_curs

         fetch rkey_curs into @i

         select @numkeys=1

         while @@fetch_status >= 0
            begin
            select @thiskey = col_name(@rkeyid, @i)

            ---- No comma for first key column.

            if @numkeys <> 1
               select @thiskey = ', ' + @thiskey

            if @tptr is not null
               updatetext #spcnsttab.cnst_keys @tptr null null @thiskey

            select @numkeys=@numkeys+1

            fetch rkey_curs into @i
            end --loop


         ---- When we get here we now have all the keys.

         if @tptr is not null
            updatetext #spcnsttab.cnst_keys @tptr null null ')'

         truncate table #spcnstkeys

         drop index #spcnstkeys.ind1

         deallocate rkey_curs
         end
      else
         if ((@cnststatus & 0xf) = 4)    -- check constraint
            begin
            select @i = 1

            select @cnstdes = text from syscomments
               where id = @cnstid and colid = @i

            while @cnstdes is not null
               begin

               if @i=1
                  begin
                  -- Get Table Check constraint
                  insert into #spcnsttab (cnst_type,cnst_name,cnst_nonblank_name,cnst_keys)
                     select   'CHECK Table Level ',@cnstname,@cnstname,' '
                        from  sysconstraints
                        where colid = 0 and constid = @cnstid

                  -- Column Level Check
                  insert into #spcnsttab (cnst_type,cnst_name,cnst_nonblank_name,cnst_keys)
                     select 'CHECK on column ' + col_name(id, colid)
                           ,@cnstname,@cnstname,' '
                        from  sysconstraints
                        where colid > 0 and constid = @cnstid

                  select @tptr = textptr(cnst_keys) from #spcnsttab

                  if @tptr is not null
                  updatetext #spcnsttab.cnst_keys @tptr 0 null null
                  end
               else
                  begin

                  if @tptr is not null
                     updatetext #spcnsttab.cnst_keys @tptr null null @cnstdes
                  end

               select @cnstdes = null

               select @cnstdes = text from syscomments
                  where id = @cnstid and colid = @i

               select @i = @i + 1
               end
            end
         else
            if ((@cnststatus & 0xf) = 5)    -- default
               begin
               select @i = 1

               select @cnstdes = text from syscomments
                  where id = @cnstid and colid = @i

               while @cnstdes is not null
                  begin

                  if @i=1
                     begin
                     insert into #spcnsttab (cnst_type,cnst_name,cnst_nonblank_name,cnst_keys)
                        select 'DEFAULT on column ' + col_name(id, colid)
                              ,@cnstname,@cnstname,' '
                           from  sysconstraints
                           where colid > 0 and constid = @cnstid

                     select @tptr = textptr(cnst_keys) from #spcnsttab

                     if @tptr is not null
                        updatetext #spcnsttab.cnst_keys @tptr 0 null null
                     end
                  else
                     begin
                     if @tptr is not null
                        updatetext #spcnsttab.cnst_keys @tptr null null @cnstdes
                     end

                  select @cnstdes = null

                  select @cnstdes = text from syscomments
                     where id = @cnstid and colid = @i

                  select @i = @i + 1
                  end
               end
            else
               insert into #spcnsttab (cnst_type,cnst_name,cnst_nonblank_name,cnst_keys)
                  values
                     ('*** INVALID TYPE FOUND IN SYSCONSTRAINTS ***'
                     ,'ERROR','ERROR','ERROR')

            fetch cnst_csr into @cnstid, @cnststatus, @cnstname
        end --of major loop


        ---- Find any rules or defaults bound by the sp_bind... method.

        insert into #spcnsttab (cnst_type,cnst_name,cnst_nonblank_name,cnst_keys)
           select 'RULE (bound with sp_bindrule)'
                 ,object_name(c.domain),object_name(c.domain)
                 ,text
              from  syscolumns c,syscomments m
              where c.id = @objid
                and m.id = c.domain
                and c.domain not in
                        (select constid from sysconstraints)

        insert into #spcnsttab (cnst_type,cnst_name,cnst_nonblank_name,cnst_keys)
           select 'DEFAULT (bound with sp_bindefault)'
                 ,object_name(c.cdefault),object_name(c.cdefault)
                 ,text
              from syscolumns c,syscomments m
              where c.id = @objid
                and m.id = c.cdefault
                and c.cdefault not in
                        (select constid from sysconstraints)


        ---- constraint status (type included)

       update #spcnsttab
                set   cnst_status = cs.status
                from  #spcnsttab tt1 ,sysconstraints cs
                where cs.constid = OBJECT_ID( tt1.cnst_name)

        update #spcnsttab
                set   cnst_status = 0
                where cnst_status is null


        if @nomsg <> 'nomsg'
           begin
           select 'Object Name' = @objname
           print ''
           end


        ---- Now print out the contents of the temporary index table.

        if (select count(*) from #spcnsttab) <> 0

           select
                   'constraint_type' = cnst_type
                  ,'constraint_name' = cnst_name

                  ,'status_enabled'      = -- 3=fkey ,4=check
                     CASE
                        When cnst_name = ' ' Then ' '
                        When cnst_status & 0xf in (3,4) and
                             cnst_status & @bitDisabled > 0 and
                             cnst_name <> ' '
                           Then    'Disabled'
                        When cnst_status & 0xf in (3,4) and
                             cnst_status & @bitDisabled = 0 and
                             cnst_name <> ' '
                           Then    'Enabled'
                        Else       '(n/a)'
                     END

                  ,'status_for_replication'  =
                     CASE
                        When cnst_name = ' ' Then ' '
                        When cnst_status & 0xf in (3,4) and
                             cnst_status & @bitNotForReplication > 0 and
                             cnst_name <> ' '
                           Then    'Not_For_Replication'
                        When cnst_status & 0xf in (3,4) and
                             cnst_status & @bitNotForReplication = 0 and
                             cnst_name <> ' '
                           Then    'Is_For_Replication'
                        Else       '(n/a)'
                     END

                  ,'constraint_keys' = cnst_keys
                from      #spcnsttab
                order by  cnst_nonblank_name ,cnst_name desc

        else
           print 'No constraints have been defined for this object.'

        print ''

        if (select count(*) from sysreferences where rkeyid = @objid) <> 0

           select
                   'Table is referenced by ' =
                        db_name(r.fkeydbid) + '.'
                     +  rtrim(
               (select user_name(o.uid) from sysobjects o
                  where o.id = r.fkeyid
               )
                             )
                     + '.' + object_name(r.fkeyid)
                     + ': ' + object_name(r.constid)
               from      sysreferences r
               where     r.rkeyid = @objid
               order by  1
        else
           print 'No foreign keys reference this table.'

deallocate cnst_csr
return (0)
go
grant execute on sp_helpconstraint to public
go



execute sp_MS_upd_sysobj_category 2

go



/***********************************************************************/

declare @dbname varchar(30)
select  @dbname = ' '

while 1=1
	begin

	select		 @dbname = min(name)
		from	 sysdatabases
		where	 @dbname <     name
		and	 status & (32|64|128|256|512|1024) = 0

	if @dbname is null
		break

	print ' '


	execute(
	'
	use ' +@dbname+ '

	declare	 @sa_uid	smallint
		,@sum_rowcount	integer

	update		 sysprotects
		set	 grantor = 1
		where	 grantor is null
		and	 action in (198, 207,222,228,233,235,236) --statement permissions 14947
		and	 action not in (26,193,195,196,197,224)  --object permissions

	select @sum_rowcount = @@rowcount

	select @sa_uid = min(uid) from sysusers where suid=1 and uid<>1  --0 or 1 such SA rows

	if @sa_uid is null
		select @sa_uid = 1

	update		 sysprotects
		set	 grantor = @sa_uid
		where	 grantor is null
		and	 action in (203) --create db statement

	select @sum_rowcount = @sum_rowcount + @@rowcount

	if @sum_rowcount > 0 and ''' +@dbname+ ''' <> ''pubs''
		raiserror(''For db named ' +@dbname+ ', number of null grantors of statement permissions needing update was %d''
			,0,1,@sum_rowcount)
	')

	end --1=1

go




exec sp_configure 'allow updates',0

exec sp_dboption 'master','trunc. log on chkpt.','true'

reconfigure with override
go

dump transaction master with no_log

checkpoint
go
--

