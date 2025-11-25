
/*
** U_Tables.CQL    1996/07/10 22:33
**
** (From old Install\InstMsgs.SQL)
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

declare @vdt varchar(99)
select  @vdt = convert(varchar,getdate(),113)
raiserror('Starting Install\SptValue.SQL at  %s',0,1,@vdt) with nowait
raiserror('This file creates all the ''SPT_'' tables.',0,1)
go


/****************  No system sprocs, thus 'allow updates' unneeded for this file.
print ''
print 'Making sure that updates to system tables are allowed.'
-go

declare  @dbcc_current_version	integer
	,@int1			integer
dbcc getvalue('current_version')
select @dbcc_current_version = @@error

if (     exists (select * from sysobjects where name='sp_configure')
    AND  @dbcc_current_version = (select version from sysdatabases where name='master')
    AND  1 <> (select value from syscurconfigs where config = 102)
   )
	begin						--Query tree compatible
	exec @int1 = sp_configure 'allow updates',1
	if @@error <> 0 or @int1 <> 0
		raiserror('Bad sp_configure exec at top of SptValue.SQL, killing spid.'
			,22,127) with log
	reconfigure with override
	end
-go


-- Make sure server was started in single user mode or that sp_configure was used
-- to enable updates to system tables.

if (select value from syscurconfigs where config = 102) <> 1
	raiserror('Cannot run SptValue.SQL unless updates to system tables are enabled. Shutdown server and restart with the ''-m'' option or use sp_configure to enable updates to system tables.'
		,22,127) with log
-go
****************/

------------------------------------------------------------------
go
exec sp_MS_upd_sysobj_category 1
go
------------------------------------------------------------------

print ''
print 'Dropping tables that will be (re)created by this script.'
go

if exists (select * from sysobjects
			where name = 'spt_committab'
				and sysstat & 0xf = 3)
	begin
	print 'Dropping TB spt_committab ...'
	drop table spt_committab
	end

if exists (select * from sysobjects
			where name = 'spt_monitor'
				and sysstat & 0xf = 3)
	begin
	print 'Dropping TB spt_monitor ...'
	drop table spt_monitor
	end

if exists (select * from sysobjects
		where name = 'spt_values'
			and sysstat & 0xf = 3)
	begin
	print 'Dropping TB spt_values ...'
	drop table spt_values
	end

--------------------------------------------------
if exists (select * from sysobjects
		where name = 'spt_fallback_usg'
			and sysstat & 0xf = 3)
	begin
	print 'Dropping TB spt_fallback_usg ...'
	drop table spt_fallback_usg
	end

if exists (select * from sysobjects
		where name = 'spt_fallback_dev'
			and sysstat & 0xf = 3)
	begin
	print 'Dropping TB spt_fallback_dev ...'
	drop table spt_fallback_dev
	end

if exists (select * from sysobjects
		where name = 'spt_fallback_db'
			and sysstat & 0xf = 3)
	begin
	print 'Dropping TB spt_fallback_dab ...'
	drop table spt_fallback_db
	end
go

dump tran master with no_log
go



------------------------------------------------------------------
------------------------------------------------------------------


raiserror(15339,-1,-1,'spt_committab')
go
CREATE TABLE	spt_committab
(
	commid		int		NOT NULL,	/* id used to refer to transaction */
	start		datetime	NOT NULL,	/* time transaction started */
	lastchange	datetime	NOT NULL,	/* last time this row was updated */
	totnum		int		NOT NULL,	/*
							** number of servers
							** initially involved in xact
							*/
	outnum		int		NOT NULL,	/*
							** number of servers
							** who still have not completed
							*/
	status		char(1)		NOT NULL,	/* 'a'- abort, 'c'- commit, 'b'- begin */
	applname	varchar(30)	NOT NULL,	/* application name */
	xactname	varchar(30)	NOT NULL,	/* transaction name */
	password	varchar(30)	NULL 		/* password protection for updating */
)
go

print 'Creating Unique Clustered index on spt_committab ...'
go
create unique clustered index commitclust on spt_committab(commid)
go


---------------------------------------

raiserror(15339,-1,-1,'spt_monitor')
go

create table spt_monitor
(
	lastrun		datetime	NOT NULL,
	cpu_busy	int		NOT NULL,
	io_busy		int		NOT NULL,
	idle		int		NOT NULL,
	pack_received	int		NOT NULL,
	pack_sent	int		NOT NULL,
	connections	int		NOT NULL,
	pack_errors	int		NOT NULL,
	total_read	int		NOT NULL,
	total_write 	int		NOT NULL,
	total_errors 	int		NOT NULL
)
go


---------------------------------------

raiserror(15339,-1,-1,'spt_values')
go
create table spt_values
(
name	varchar(35)	    NULL,
number	int		NOT NULL,
type	char(3)		NOT NULL, --Make these unique to aid GREP (e.g. SOP, not SET or S).
low	int		    NULL,
high	int		    NULL,
status	int		    NULL  DEFAULT 0
)
go


print ''
print 'Creating indexes on table spt_values.'
go
print '   Unique Clustered ....'
go

-- 'J','S','P'  challenge uniqueness.
create Unique Clustered index spt_valuesclust on spt_values(type ,number ,name)
go

print '   (nonUnique) Nonclustered ....'
go

create Nonclustered index ix2_spt_values_nu_nc on spt_values(number, type)
go


----+----1----+----2----+----3----+----4----+----5----+----6----+----7----+

raiserror(15339,-1,-1,'spt_fallback_db')
go
create table spt_fallback_db
   (
    xserver_name           character varying(30)      not null
   ,xdttm_ins              datetime                   not null
   ,xdttm_last_ins_upd     datetime                   not null
   ,xfallback_dbid         smallint                       null

   ,name                   character varying(30)      not null
   ,dbid                   smallint                   not null
   ,status                 smallint                   not null
   ,version                smallint                   not null
   )
go

/****
--Nice-to-have the uniqueness check, but extra 16 Mb in master db too precious.
create unique Clustered
   index    idx1
   on       spt_fallback_db
      (
       xserver_name
      ,name
      )
****/
go

----+----1----+----2----+----3----+----4----+----5----+----6----+----7----+

raiserror(15339,-1,-1,'spt_fallback_dev')
go
create table spt_fallback_dev
   (
    xserver_name           character varying(30)      not null
   ,xdttm_ins              datetime                   not null
   ,xdttm_last_ins_upd     datetime                   not null
   ,xfallback_low          integer                        null
   ,xfallback_drive        character(2)                   null

   ,low                    integer                    not null
   ,high                   integer                    not null
   ,status                 smallint                   not null
   ,name                   character varying(30)      not null
   ,phyname                character varying(127)     not null
   )
go

/****
create unique Clustered
   index    idx1
   on       spt_fallback_dev
      (
       xserver_name
      ,name
      )
****/
go

----+----1----+----2----+----3----+----4----+----5----+----6----+----7----+

raiserror(15339,-1,-1,'spt_fallback_usg')
go
create table spt_fallback_usg
   (
    xserver_name           character varying(30)      not null
   ,xdttm_ins              datetime                   not null
   ,xdttm_last_ins_upd     datetime                   not null
   ,xfallback_vstart       integer                        null

   ,dbid                   smallint                   not null
   ,segmap                 integer                    not null
   ,lstart                 integer                    not null
   ,sizepg                 integer                    not null
   ,vstart                 integer                    not null
   )
go

/****
create unique Clustered
   index    idx1
   on       spt_fallback_usg
      (
       xserver_name
      ,dbid
      ,lstart
      )
****/
go


------------------------------------------------------------------
------------------------------------------------------------------

raiserror('Granting on spt_ tables ...',0,1)
go

grant select on spt_values  to public
grant select on spt_monitor to public

grant select on spt_fallback_db to public
grant select on spt_fallback_dev to public
grant select on spt_fallback_usg to public
go



------------------------------------------------------------------
------------------------------------------------------------------


raiserror('Now inserting one row into table spt_monitor',0,1)
go

insert into spt_monitor
	select
	lastrun = getdate(),
	cpu_busy = @@cpu_busy,
	io_busy = @@io_busy,
	idle = @@idle,
	pack_received = @@pack_received,
	pack_sent = @@pack_sent,
	connections = @@connections,
	pack_errors = @@packet_errors,
	total_read = @@total_read,
	total_write = @@total_write,
	total_errors = @@total_errors
go



-- Caution, 'Z' is used by sp_helpsort, though no 'Z' rows are inserted by this file.

print ''
print 'Now Inserting rows into table spt_values ...'
go

print ''
print 'Adding status bit descriptions for sysservers (''A'').'
go
insert into spt_values (name, number, type)
	values ('SYSSERVERS TYPES', -1, 'A')
insert into spt_values (name, number, type)
	values ('rpc', 1, 'A')
insert into spt_values (name, number, type)
	values ('pub', 2, 'A')
insert into spt_values (name, number, type)
	values ('sub', 4, 'A')
insert into spt_values (name, number, type)
	values ('dist', 8, 'A')
insert into spt_values (name, number, type)
	values ('dpub', 16, 'A')
insert into spt_values (name, number, type)
	values ('dsn', 32, 'A')
insert into spt_values (name, number, type)
	values ('fallback', 64, 'A')
go

print ''
print 'Adding translations for ''yes'', ''no'', etc.'
go
insert spt_values (name, number, type)
	values ('YES OR NO', -1, 'B')
insert spt_values (name, number, type)
	values ('no', 0, 'B')
insert spt_values (name, number, type)
	values ('yes', 1, 'B')
insert spt_values (name, number, type)
	values ('none', 2, 'B')
go


print ''
print 'Adding configuration options...'
go
insert spt_values(name, number, type)
        values ('CONFIGURATION OPTIONS', -1, 'C') -- 1=Dynamic, 2=Advanced

insert spt_values(name, number, type, low, high,status)
	values ('recovery interval', 101, 'C', 1, 32767 ,1)

insert spt_values(name, number, type, low, high,status)
	values ('allow updates', 102, 'C', 0, 1 ,1)

insert spt_values(name, number, type, low, high,status)
	values ('user connections', 103, 'C', 5, @@max_connections ,0)

insert spt_values(name, number, type, low, high,status)
	values ('memory', 104, 'C', 2800, 1048576 ,0)

insert spt_values(name, number, type, low, high,status)
	values ('open databases', 105, 'C', 5, 32767 ,0)

insert spt_values(name, number, type, low, high,status)
	values ('locks', 106, 'C', 5000, 2147483647 ,0)

insert spt_values(name, number, type, low, high,status)
	values ('open objects', 107, 'C', 100, 2147483647 ,0)

insert spt_values(name, number, type, low, high,status)
	values ('procedure cache', 108, 'C', 1, 99 ,0)

insert spt_values(name, number, type, low, high,status)
	values ('fill factor', 109, 'C', 0, 100 ,0)

insert spt_values(name, number, type, low, high,status)
	values ('database size', 111, 'C', 2, 10000 ,0)

insert spt_values(name, number, type, low, high,status)
	values ('media retention', 112, 'C', 0, 365 ,0)

insert spt_values(name, number, type, low, high,status)
	values ('recovery flags', 113, 'C', 0, 1 ,0)

insert spt_values(name, number, type, low, high,status)
	values ('nested triggers', 115, 'C', 0, 1 ,1)

insert spt_values(name, number, type, low, high,status)
	values ('remote access', 117, 'C', 0, 1 ,0)

insert spt_values(name, number, type, low, high,status)
	values ('default language', 124, 'C', 0, 9999 ,1)

insert spt_values(name, number, type, low, high,status)
	values ('language in cache', 125, 'C', 3, 100 ,0)


insert spt_values (name, number, type, low,high,status)
	values ('tempdb in ram (MB)', 501, 'C', 0, 2044 ,0)

insert spt_values(name, number, type, low, high,status)
	values ('max async IO', 502, 'C', 1, 255 ,0)

insert into spt_values(name, number, type, low, high,status)
	values ('max worker threads', 503, 'C', 10, 1024 ,1)

insert spt_values(name, number, type, low, high,status)
	values ('network packet size', 505, 'C', 512, 32767 ,1)

insert into spt_values(name, number, type, low, high,status)
	values ('RA worker threads', 508, 'C', 0, 255 ,0)

insert spt_values(name, number, type, low, high,status)
	values ('show advanced options', 518, 'C', 0, 1 ,1)

insert into spt_values(name, number, type, low, high,status)
	values ('LE threshold percent', 521, 'C', 1, 100 ,1)

insert into spt_values(name, number, type, low, high,status)
	values ('LE threshold maximum', 523, 'C', 2, 500000 ,1)


insert into spt_values(name, number, type, low, high,status)
	values ('backup threads', 540, 'C', 0, 32 ,0)

insert into spt_values(name, number, type, low, high,status)
	values ('backup buffer size', 541, 'C', 1, 10 ,1)


insert into spt_values (name ,number ,type ,low ,high ,status)
	values ('remote proc trans', 542, 'C', 0, 1, 0)

insert into spt_values (name ,number ,type ,low ,high ,status)
	values ('remote conn timeout', 543, 'C', -1, 32767, 0)



insert into spt_values (name ,number ,type ,low ,high ,status)
	values ('time slice', 1110, 'C', 50, 1000, 3)

insert into spt_values (name ,number ,type ,low ,high ,status)
	values ('remote sites', 1119, 'C', 0, 256, 2)



insert spt_values (name, number, type, low, high,status)
	values ('default sortorder id', 1123, 'C', 0, 255 ,2)

insert spt_values(name, number, type, low, high,status)
	values ('hash buckets', 1504, 'C', 4999, 265003 ,2)

insert spt_values(name, number, type, low, high,status)
	values ('sort pages', 1505, 'C', 64, 511 ,3)

insert spt_values(name, number, type, low, high,status)
	values ('max lazywrite IO', 1506, 'C', 1, 255 ,3)


insert into spt_values(name, number, type, low, high,status)
	values ('RA slots per thread', 1509, 'C', 1, 255 ,2)

insert into spt_values(name, number, type, low, high,status)
	values ('RA pre-fetches', 1510, 'C', 1, 1000 ,3)

insert into spt_values(name, number, type, low, high,status)
	values ('RA delay', 1511, 'C', 0, 500 ,3)

insert into spt_values(name, number, type, low, high,status)
	values ('RA cache miss limit', 1512, 'C', 1, 255 ,3)

insert into spt_values(name, number, type, low, high,status)
	values ('RA cache hit limit', 1513, 'C', 1, 255 ,3)


insert spt_values(name, number, type, low, high,status)
	values ('spin counter', 1514, 'C', 1, 2147483647 ,3)

insert spt_values(name, number, type, low, high,status)
	values ('free buffers', 1515, 'C', 20, 524288 ,3)

insert spt_values(name, number, type, low, high,status)
	values ('SMP concurrency', 1516, 'C', -1, 64 ,2)

insert spt_values(name, number, type, low, high,status)
	values ('priority boost', 1517, 'C', 0, 1 ,2)

insert spt_values(name, number, type, low, high,status)
	values ('remote login timeout', 1519, 'C', 0, 2147483647 ,3)

insert spt_values(name, number, type, low, high,status)
	values ('remote query timeout', 1520, 'C', 0, 2147483647 ,3)


insert into spt_values(name, number, type, low, high,status)
	values ('LE threshold minimum', 1522, 'C', 2, 500000 ,3)

insert into spt_values(name, number, type, low, high,status)
	values ('logwrite sleep (ms)', 1530, 'C', -1, 500 ,1)

insert into spt_values(name, number, type, low, high,status)
	values ('cursor threshold', 1531, 'C', -1, 2147483647 ,3)


insert spt_values(name, number, type, low, high,status)
	values ('set working set size', 1532, 'C', 0, 1 ,2)

insert spt_values(name, number, type, low, high,status)
	values ('resource timeout', 1533, 'C', 5, 2147483647 ,3)

insert spt_values(name, number, type, low, high,status)
	values ('user options'  --Most on/off style SET options
          ,1534, 'C', 0
          ,4095 ,1)  --Max is 0xfff=4095 (except for incompatibilities!)

insert spt_values(name, number, type, low, high,status)
	values ('affinity mask', 1535, 'C', 0, 2147483647, 2)

insert spt_values(name, number, type, low, high,status)
	values ('max text repl size', 1536, 'C', 0, 2147483647, 1)
go


print ''
print 'Adding database status bits.'
go
/*
**  If you add a bit here make sure you add the value to the value
**	of the ALL SETTABLE option if it is settable with sp_dboption.
*/

insert spt_values (name, number, type)
	values ('DATABASE STATUS', -1, 'D')
insert spt_values (name, number, type)
	values ('select into/bulkcopy', 4, 'D')
insert spt_values (name, number, type)
	values ('trunc. log on chkpt.', 8, 'D')
insert spt_values (name, number, type)
	values ('no chkpt on recovery', 16, 'D')
insert spt_values (name, number, type)
	values ('loading', 32, 'D')  --Had been "don't recover".
insert spt_values (name, number, type)
	values ('pre recovery', 64, 'D')
insert spt_values (name, number, type)
	values ('recovering', 128, 'D')
insert spt_values (name, number, type)
	values ('not recovered', 256, 'D')  --suspect
insert into spt_values(name, number, type, low, high)
	values ('offline', 512, 'D', 0, 1)
insert spt_values (name, number, type)
	values ('read only', 1024, 'D')
insert spt_values (name, number, type)
	values ('dbo use only', 2048, 'D')
insert spt_values (name, number, type)
	values ('single user', 4096, 'D')
insert spt_values (name, number, type)
	values ('ANSI null default', 16384, 'D')
insert spt_values (name, number, type)
	values ('emergency mode', 32768, 'D')

/* This bit comes from sysdatabases.category not sysdatabases.status. */
insert spt_values (name, number, type)
	values ('published', 65536, 'D')
insert spt_values (name, number, type)
	values ('subscribed', 131072, 'D')

/* Sum of bits of all settable options, update when adding such options
** or modifying existing options to be settable. */
insert spt_values (name, number, type)
	values ('ALL SETTABLE OPTIONS', 220700, 'D')
go



print 'Add release/version data, re  sysdatabases.version  ...'
go
insert into spt_values (name ,number ,type,low,high)
	values ('SYSDATABASES.VERSION', -1, 'DBV',-1,-1) --dbcc getvalue('current_version')
insert into spt_values (name ,number ,type,low,high)
	values ('4.2' ,199307 ,'DBV',1,1)  --WinNT version
insert into spt_values (name ,number ,type,low,high)
	values ('6.0' ,199506 ,'DBV',400,406) --Betas thru Release range was 400-406.
declare @sysdbver int
dbcc getvalue('current_version')
select @sysdbver = @@error
insert into spt_values (name ,number ,type,low,high)
	values ('6.5' ,199604 ,'DBV',407,408) --,@sysdbver)
go



print ''
print 'Setting machine type.'
go
/*
**  Set the machine type
**  spt_values.low is the number of bytes in a page for the
**  particular machine.
*/
insert spt_values (name, number, type, low)
	values ('SQLSERVER HOST TYPE', -1, 'E', 0)
go

print ''
print 'Adding platform specific entries.'
go
/*
**  Set the platform specific entries.
**  spt_values.low is the number of bytes in a page.
*/
insert into spt_values (name, number, type, low)
	values ('WINDOWS/NT', 1, 'E', 2048)

/* Value to set and clear the high bit for int datatypes for os/2.
** Would like to enter -2,147,483,648 to avoid byte order issues, but
** the server won't take it, even in exponential notation.
*/
insert into spt_values (name, number, type, low)
	values ('int high bit', 2, 'E', 0x80000000)

/* Value which gives the byte position of the high byte for int datatypes for
** os/2.  This value was changed from 4 (the usual Intel 80x86 order) to 1
** when binary convert routines were changed to reverse the byte order.  So
** this value is accurate ONLY when ints are converted to binary datatype.
*/
insert into spt_values (name, number, type, low)
	values ('int4 high byte', 3, 'E', 1)
go

print ''
print 'Adding status bit descriptions for sysremotelogins.'
go
insert spt_values (name, number, type)
	values ('SYSREMOTELOGINS TYPES', -1, 'F')
insert spt_values (name, number, type)
	values ('', 0, 'F')
insert spt_values (name, number, type)
	values ('trusted', 1, 'F')
go

insert spt_values (name, number, type)
	values ('GENERAL MISC. STRINGS', -1, 'G')
insert spt_values (name, number, type)
	values ('SQL Server Internal Table', 0, 'G')
go

print ''
print 'Adding descriptions for status bits in sysindexes.'
go
insert spt_values (name, number, type)
	values ('INDEX TYPES', -1, 'I')
insert spt_values (name, number, type)
	values ('nonclustered', 0, 'I')
insert spt_values (name, number, type)
	values ('ignore duplicate keys', 1, 'I')
insert spt_values (name, number, type)
	values ('unique', 2, 'I')
insert spt_values (name, number, type)
	values ('ignore duplicate rows', 4, 'I')
insert spt_values (name, number, type)
	values ('clustered', 16, 'I')
insert spt_values (name, number, type)
	values ('allow duplicate rows', 64, 'I')
go

print ''
print 'Adding values for referential integrity.'
go
insert into spt_values (name, number, type, low, high)
	values ('primary key', 2048, 'I', 0, 1)
insert into spt_values (name, number, type, low, high)
	values ('unique key', 4096, 'I', 0, 1)
go

print ''
print 'Adding listing of physical types that are compatible.'
go
insert spt_values (name, number, type)
	values ('COMPATIBLE TYPES', -1, 'J')
insert spt_values (name, number, low, type)
	values ('binary', 1, 45, 'J')
insert spt_values (name, number, low, type)
	values ('varbinary', 1, 37, 'J')
insert spt_values (name, number, low, type)
	values ('bit', 2, 50, 'J')
insert spt_values (name, number, low, type)
	values ('char', 3, 47, 'J')
insert spt_values (name, number, low, type)
	values ('varchar', 3, 39, 'J')
insert spt_values (name, number, low, type)
	values ('datetime', 4, 61, 'J')
insert spt_values (name, number, low, type)
	values ('datetimn', 4, 111, 'J')
insert spt_values (name, number, low, type)
	values ('smalldatetime', 4, 58, 'J')
insert spt_values (name, number, low, type)
	values ('float', 5, 62, 'J')
insert spt_values (name, number, low, type)
	values ('floatn', 5, 109, 'J')
insert spt_values (name, number, low, type)
	values ('real', 5, 59, 'J')
insert spt_values (name, number, low, type)
	values ('int', 6, 56, 'J')
insert spt_values (name, number, low, type)
	values ('intn', 6, 38, 'J')
insert spt_values (name, number, low, type)
	values ('smallint', 6, 52, 'J')
insert spt_values (name, number, low, type)
	values ('tinyint', 6, 48, 'J')
insert spt_values (name, number, low, type)
	values ('money', 7, 60, 'J')
insert spt_values (name, number, low, type)
	values ('moneyn', 7, 110, 'J')
insert spt_values (name, number, low, type)
	values ('smallmoney', 7, 122, 'J')
go

print ''
print 'Adding bit description for syskeys.type.'
go
insert spt_values (name, number, type)
	values ('SYSKEYS TYPES', -1, 'K')
insert spt_values (name, number, type)
	values ('primary', 1, 'K')
insert spt_values (name, number, type)
	values ('foreign', 2, 'K')
insert spt_values (name, number, type)
	values ('common', 3, 'K')
go

print ''
print 'Adding values for syslocks.type'
go
insert spt_values(name, number, type)
  values ('LOCK TYPES', -1, 'L')
insert spt_values(name, number, type)
  values ('Ex_table', 1, 'L')
insert spt_values(name, number, type)
  values ('Sh_table', 2, 'L')
insert spt_values(name, number, type)
  values ('Ex_intent', 3, 'L')
insert spt_values(name, number, type)
  values ('Sh_intent', 4, 'L')
insert spt_values(name, number, type)
  values ('Ex_page', 5, 'L')
insert spt_values(name, number, type)
  values ('Sh_page', 6, 'L')
insert spt_values(name, number, type)
  values ('Update_page', 7, 'L')
insert spt_values(name, number, type)
  values ('Ex_extent', 8, 'L')
insert spt_values(name, number, type)
  values ('Update_extent', 9, 'L')
insert spt_values(name, number, type)
  values ('ShTab_ExIntent', 10, 'L')
insert spt_values(name, number, type)
  values ('Next_extent', 11, 'L')
insert spt_values(name, number, type)
  values ('Prev_extent', 12, 'L')
insert spt_values(name, number, type)
  values ('Insert_page', 13, 'L')
insert spt_values(name, number, type)
  values ('Link_page', 14, 'L')
insert spt_values(name, number, type)
  values ('Ex_table-blk', 256+1, 'L')
insert spt_values(name, number, type)
  values ('Sh_table-blk', 256+2, 'L')
insert spt_values(name, number, type)
  values ('Ex_intent-blk', 256+3, 'L')
insert spt_values(name, number, type)
  values ('Sh_intent-blk', 256+4, 'L')
insert spt_values(name, number, type)
  values ('Ex_page-blk', 256+5, 'L')
insert spt_values(name, number, type)
  values ('Sh_page-blk', 256+6, 'L')
insert spt_values(name, number, type)
  values ('Update_page-blk', 256+7, 'L')
insert spt_values(name, number, type)
  values ('Ex_extent-blk', 256+8, 'L')
insert spt_values(name, number, type)
  values ('Update_extent-blk', 256+9, 'L')
insert spt_values(name, number, type)
  values ('ShTab_ExIntent-blk', 256+10, 'L')
insert spt_values(name, number, type)
  values ('Next_extent-blk', 256+11, 'L')
insert spt_values(name, number, type)
  values ('Prev_extent-blk', 256+12, 'L')
insert spt_values(name, number, type)
  values ('Insert_page-blk', 256+13, 'L')
insert spt_values(name, number, type)
  values ('Link_page-blk', 256+14, 'L')
go

/*
******************************************
StarFighter Lock Description Strings
******************************************
*/

insert spt_values(name, number, type)
  values ('SF LOCK TYPES', -1, 'SFL')
insert spt_values(name, number, type)
  values ('Extent Lock - Exclusive', 8, 'SFL')
insert spt_values(name, number, type)
  values ('Extent Lock - Update', 9, 'SFL')
insert spt_values(name, number, type)
  values ('Extent Lock - Next', 11, 'SFL')
insert spt_values(name, number, type)
  values ('Extent Lock - Previous', 12, 'SFL')
go

print ''
print 'Adding descriptions for object types.'
go
/*
**  These values define the object type.  The number made from the low
**  4 bits in sysobjects.sysstats indicates the type of object.
*/
insert spt_values (name, number, type)
	values ('OBJECT TYPES', -1, 'O')
insert spt_values (name, number, type)
	values ('system table', 1, 'O')
insert spt_values (name, number, type)
	values ('view', 2, 'O')
insert spt_values (name, number, type)
	values ('user table', 3, 'O')
insert spt_values (name, number, type)
	values ('stored procedure',4, 'O')
/* no number 5 */
insert spt_values (name, number, type)
	values ('default', 6, 'O')
insert spt_values (name, number, type)
	values ('rule', 7, 'O')
insert spt_values (name, number, type)
	values ('trigger', 8, 'O')
insert spt_values (name, number, type)
	values ('replication filter stored procedure', 12, 'O')
go


print ''
print 'Adding bit position information  ''P''  (helpful with sysprotects.columns).'
go

insert spt_values(low, high, number, type) values (1, 1, 0, 'P')
/*
**insert spt_values(name, number, type)
**	values ('BIT POSITIONS', -1, 'P')
**go
*/
insert spt_values(low, high, number, type) values (1, 2, 1, 'P')
insert spt_values(low, high, number, type) values (1, 4, 2, 'P')
insert spt_values(low, high, number, type) values (1, 8, 3, 'P')
insert spt_values(low, high, number, type) values (1, 16, 4, 'P')
insert spt_values(low, high, number, type) values (1, 32, 5, 'P')
insert spt_values(low, high, number, type) values (1, 64, 6, 'P')
insert spt_values(low, high, number, type) values (1, 128, 7, 'P')
go

/* 8 - 15 */
insert spt_values(low, high, number, type)
select
	(select max(low) from spt_values where type = 'P' and number
	between 0 and 7) + 1 + (number / 8),high,
	number + 1 + (select max(number) from spt_values where type = 'P'
	and number between 0 and 7), 'P'
	from spt_values
	where type = 'P' and number between 0 and 7
go
/* 16 - 31 */
insert spt_values(low, high, number, type)
select
	(select max(low) from spt_values where type = 'P' and number
	between 0 and 15) + 1 + (number / 8), high, number + 1 +
	(select max(number) from spt_values where type = 'P' and number
	between 0 and 15), 'P'
	from spt_values
	where type = 'P' and number between 0 and 15
go
/* 32 - 63 */
insert spt_values(low, high, number, type)
select
	(select max(low) from spt_values where type = 'P' and number
	between 0 and 63) + 1 + (number / 8), high, number + 1 +
	(select max(number) from spt_values where type = 'P' and number
	between 0 and 63), 'P'
	from spt_values
	where type = 'P' and number between 0 and 63
go
/* 64 - 127 */
insert spt_values(low, high, number, type)
select
	(select max(low) from spt_values where type = 'P' and number
	between 0 and 127) + 1 + (number / 8), high, number + 1 +
	(select max(number) from spt_values where type = 'P' and number
	between 0 and 127), 'P'
	from spt_values
	where type = 'P' and number between 0 and 127
go
/* 128 - 255 */
insert spt_values(low, high, number, type)
select
	(select max(low) from spt_values where type = 'P' and number
	between 0 and 127) + 1 + (number / 8), high, number + 1 +
	(select max(number) from spt_values where type = 'P' and number
	between 0 and 127), 'P'
	from spt_values
	where type = 'P' and number between 0 and 127
go


print ''
print 'Adding bit descriptions for sysobjects.userstat (''R'').'
go
/*
**  These values translate the object type's userstat bits.  If the high
**  bit is set for a sproc, then it's a report.
*/
insert spt_values (name, number, type)
	values ('REPORT TYPES', -1, 'R')
insert spt_values (name, number, type)
	values ('', 0, 'R')
insert spt_values (name, number, type)
	values (' (rpt)', -32768, 'R')
go

print ''
print 'Adding descriptions for sysusages.segmap.'
go
insert into spt_values (name, type, number)
	values ('SYSUSAGES SEGMAP', 'S', -1)
insert into spt_values (name, type, number)
	values ('data only', 'S', 0)
insert into spt_values (name, type, number)
	values ('data only', 'S', 1)
insert into spt_values (name, type, number)
	values ('data only', 'S', 2)
insert into spt_values (name, type, number)
	values ('data only', 'S', 3)
insert into spt_values (name, type, number)
	values ('log only', 'S', 4)
insert into spt_values (name, type, number)
	values ('data only', 'S', 5)
insert into spt_values (name, type, number)
	values ('data only', 'S', 6)
insert into spt_values (name, type, number)
	values ('data and log', 'S', 7)
go


raiserror('Adding type=''SOP'' rows for SET Options status info.   See sp_help_setopts, @@options, and config=1534 (''user options'').',0,1)
go
--status&1=1 means configurable via 'user options'.
insert into spt_values (name ,number ,type ,status) values
      ('@@OPTIONS' ,-1 ,'SOP' ,0)
go
insert into spt_values (name ,number ,type ,status) values
      ('disable_def_cnst_check'  ,1 ,'SOP' ,1)

insert into spt_values (name ,number ,type ,status) values
      ('implicit_transactions'   ,2 ,'SOP' ,1)

insert into spt_values (name ,number ,type ,status) values
      ('cursor_close_on_commit'  ,4 ,'SOP' ,1)

insert into spt_values (name ,number ,type ,status) values
      ('ansi_warnings'           ,8 ,'SOP' ,1)

insert into spt_values (name ,number ,type ,status) values
      ('ansi_padding'            ,16 ,'SOP' ,1)

insert into spt_values (name ,number ,type ,status) values
      ('ansi_nulls'              ,32 ,'SOP' ,1)

insert into spt_values (name ,number ,type ,status) values
      ('arithabort'              ,64 ,'SOP' ,1)

insert into spt_values (name ,number ,type ,status) values
      ('arithignore'             ,128 ,'SOP' ,1)

insert into spt_values (name ,number ,type ,status) values
      ('quoted_identifier'       ,256 ,'SOP' ,1)

insert into spt_values (name ,number ,type ,status) values
      ('nocount'                 ,512 ,'SOP' ,1)

--Mutually exclusive when ON.
insert into spt_values (name ,number ,type ,status) values
      ('ansi_null_dflt_on'       ,1024 ,'SOP' ,1)
insert into spt_values (name ,number ,type ,status) values
      ('ansi_null_dflt_off'      ,2048 ,'SOP' ,1)
go


print ''
print 'Adding sysprotects.action AND protecttype values...'
go
insert spt_values(name, number, type)
  values ('SYSPROTECTS.ACTION', -1, 'T')
insert spt_values(name, number, type)
  values ('References', 26, 'T')
insert spt_values(name, number, type)
  values ('Select', 193, 'T')          ----action
insert spt_values(name, number, type)
  values ('Insert', 195, 'T')  --Covers BCPin and LoadTable.
insert spt_values(name, number, type)
  values ('Delete', 196, 'T')
insert spt_values(name, number, type)
  values ('Update', 197, 'T')
insert spt_values(name, number, type)
  values ('Create Table', 198, 'T')
insert spt_values(name, number, type)
  values ('Create Database', 203, 'T')

insert spt_values(name, number, type)
  values ('Grant_WGO', 204, 'T')
insert spt_values(name, number, type)
  values ('Grant', 205, 'T')           ----protecttype
insert spt_values(name, number, type)
  values ('Revoke', 206, 'T')

insert spt_values(name, number, type)
  values ('Create View', 207, 'T')
insert spt_values(name, number, type)
  values ('Create Procedure', 222, 'T')
insert spt_values(name, number, type)
  values ('Execute', 224, 'T')
insert spt_values(name, number, type)
  values ('Dump Database', 228, 'T')
insert spt_values(name, number, type)
  values ('Create Default', 233, 'T')
insert spt_values(name, number, type)
  values ('Dump Transaction', 235, 'T')
insert spt_values(name, number, type)
  values ('Create Rule', 236, 'T')

go


print ''  -- Table Options (In 6.5 see sp_tableoption)
print 'Adding type=''TBO'' row for TaBle Options.'
go
insert spt_values (name ,number ,type ,low ,high ,status)
	values ('TableOptions' ,0 ,'TBO' ,0 ,1 ,0) -- sysobj.userstat/.sysstat
insert spt_values (name ,number ,type ,low ,high ,status)
	values ('insert row lock' ,1 ,'TBO' ,0 ,1 ,0) -- userstat
insert spt_values (name ,number ,type ,low ,high ,status)
	values ('table lock on bulk load' ,2 ,'TBO' ,0 ,1 ,0) -- userstat 6.5 SP1 new
insert spt_values (name ,number ,type ,low ,high ,status)
	values ('pintable' ,512 ,'TBO' ,0 ,1 ,0) -- sysstat 6.5 SP1 change

go


print ''
print 'Add bit description for sysdevices.status.'
go
insert spt_values (name, number, type)
	values ('SYSDEVICES STATUS', -1, 'V')
insert spt_values (name, number, type)
	values ('default disk', 1, 'V')
insert spt_values (name, number, type)
	values ('physical disk', 2, 'V')
insert spt_values (name, number, type)
	values ('logical disk', 4, 'V')
insert spt_values (name, number, type)
	values ('skip header', 8, 'V')
insert spt_values (name, number, type)
	values ('dump device', 16, 'V')
insert spt_values (name, number, type)
	values ('serial writes', 32, 'V')
insert spt_values (name, number, type)
	values ('device mirrored', 64, 'V')
insert spt_values (name, number, type)
	values ('reads mirrored', 128, 'V')
insert spt_values (name, number, type)
	values ('half-mirror only', 256, 'V')
insert spt_values (name, number, type)
	values ('mirror enabled', 512, 'V')
insert into spt_values(name, number, type, low, high)
	values ('read only', 4096, 'V', 0, 1)
insert into spt_values(name, number, type, low, high)
	values ('deferred', 8192, 'V', 0, 1)
go

update statistics spt_values
go

------------------------------------------------------------------
go
exec sp_MS_upd_sysobj_category 2
go
------------------------------------------------------------------

/********
if exists (select * from sysobjects where name = 'sp_configure'
			and sysstat & 0xf = 4)
	begin
		exec sp_configure 'allow updates',0
		reconfigure with override
	end
********/
go

declare @vdt varchar(99)
select  @vdt = convert(varchar,getdate(),113)
raiserror('Finishing Install\SptValue.SQL at  %s',0,1,@vdt)
go

dump tran master with no_log
go
checkpoint
go
print 'End of .SQL file.'
go
-- -

