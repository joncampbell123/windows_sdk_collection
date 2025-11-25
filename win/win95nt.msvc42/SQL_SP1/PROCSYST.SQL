
--MS SQL 6.5 ServicePack revisions included.
/*
** ProcSyst.CQL		1996/07/10 21:52
**
** (From old InstProc.SQL.  See also Configur.SQL.  See also ProcSyst.HR.)
** (qtoken.exe combines .CQL & .HR to output .SQL)
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
dump transaction master with no_log
go
checkpoint
go
set nocount on
set implicit_transactions off
go

declare @vdt varchar(99)
select  @vdt = convert(varchar,getdate(),113)
raiserror('Starting Install\ProcSyst.SQL at  %s',0,1,@vdt) with nowait
go

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
		raiserror('Bad sp_configure exec at top of ProcSyst.SQL, killing spid.'
			,22,127) with log
	reconfigure with override
	end
go

/*
** Make sure server was started in single user mode or that sp_configure was used
** to enable updates to system tables.
*/

if (select value from syscurconfigs where config = 102) <> 1
	raiserror('Cannot run ProcSyst.SQL unless updates to system tables are enabled.  Shutdown server and restart with the ''-m'' option or use sp_configure to enable updates to system tables.'
			,22,127) with log
go


---- 1996/04/18 16:03   14947
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
		and	 action in (198, 207,222,228,233,235,236) --statement permissions
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

---- End, of sysprotects.grantor update for statement permissions.
go


exec sp_MS_upd_sysobj_category 1  --Capture now_datetime for use below.

go

select count(*) ,'= the number of system stored procs in master..sysobjects'
	from sysobjects where type='P ' and name like 'sp[_]%'

print ''
print 'Dropping procedures that will be (re)created.'
go

--Enhanced sp_rename/sp_namecrack_qi, thus these are obsolete in MS SQL 6.5 ServicePack1.
if exists (select * from sysobjects
		where sysstat & 0xf = 4
			and name = 'sp_namecrack2')
	drop procedure sp_namecrack2
if exists (select * from sysobjects
		where sysstat & 0xf = 4
			and name = 'sp_namecrack3')
	drop procedure sp_namecrack3


go
if exists (select * from sysobjects
		where sysstat & 0xf = 4
			and name = 'sp_a_count_bits_on')
		     drop procedure sp_a_count_bits_on

if exists (select * from sysobjects
		where sysstat & 0xf = 4
			and name = 'sp_abort_xact')
	drop procedure sp_abort_xact

if exists (select * from sysobjects
		where sysstat & 0xf = 4
			and name = 'sp_addalias')
	drop procedure sp_addalias

if exists (select * from sysobjects
		where sysstat & 0xf = 4
			and name = 'sp_addextendedproc')
	drop procedure sp_addextendedproc

if exists (select * from sysobjects
		where sysstat & 0xf = 4
			and name = 'sp_addgroup')
	drop procedure sp_addgroup

if exists (select * from sysobjects
		where sysstat & 0xf = 4
			and name = 'sp_addlanguage')
	drop procedure sp_addlanguage

if exists (select * from sysobjects
		where sysstat & 0xf = 4
			and name = 'sp_addlogin')
	drop procedure sp_addlogin

if exists (select * from sysobjects
		where sysstat & 0xf = 4
			and name = 'sp_addmessage')
	drop procedure sp_addmessage

if exists (select * from sysobjects
		where sysstat & 0xf = 4
			and name = 'sp_addremotelogin')
	drop procedure sp_addremotelogin

if exists (select * from sysobjects
		where sysstat & 0xf = 4
			and name = 'sp_addsegment')
	drop procedure sp_addsegment
go

dump transaction master with no_log
go

if exists (select * from sysobjects
		where sysstat & 0xf = 4
			and name = 'sp_addserver')
	drop procedure sp_addserver

if exists (select * from sysobjects
		where sysstat & 0xf = 4
			and name = 'sp_addtype')
	drop procedure sp_addtype

if exists (select * from sysobjects
		where sysstat & 0xf = 4
			and name = 'sp_addumpdevice')
	drop procedure sp_addumpdevice

if exists (select * from sysobjects
		where sysstat & 0xf = 4
			and name = 'sp_adduser')
	drop procedure sp_adduser

if exists (select * from sysobjects
                where sysstat & 0xf = 4
                        and name = 'sp_altermessage')
        drop procedure sp_altermessage

if exists (select * from sysobjects
		where sysstat & 0xf = 4
			and name = 'sp_bindefault')
	drop procedure sp_bindefault

if exists (select * from sysobjects
		where sysstat & 0xf = 4
			and name = 'sp_bindrule')
	drop procedure sp_bindrule

if exists (select * from sysobjects
		where sysstat & 0xf = 4
			and name = 'sp_blockcnt')
	drop procedure sp_blockcnt

if exists (select * from sysobjects
		where sysstat & 0xf = 4
			and name = 'sp_change_users_login')
		     drop procedure sp_change_users_login

if exists (select * from sysobjects
		where sysstat & 0xf = 4
			and name = 'sp_changedbowner')
	drop procedure sp_changedbowner

if exists (select * from sysobjects
		where sysstat & 0xf = 4
			and name = 'sp_changegroup')
	drop procedure sp_changegroup

if exists (select * from sysobjects
		where sysstat & 0xf = 4
			and name = 'sp_checknames')
	drop procedure sp_checknames

if exists (select * from sysobjects
		where sysstat & 0xf = 4
			and name = 'sp_chklangparam')
	drop procedure sp_chklangparam

if exists (select * from sysobjects
		where sysstat & 0xf = 4
			and name = 'sp_commit_xact')
	drop procedure sp_commit_xact

if exists (select * from sysobjects
		where sysstat & 0xf = 4
			and name = 'sp_setlangalias')
	drop procedure sp_setlangalias

if exists (select * from sysobjects
		where sysstat & 0xf = 4
			and name = 'sp_commonkey')
	drop procedure sp_commonkey
go

if exists (select * from sysobjects
		where sysstat & 0xf = 4
			and name = 'sp_configure')
	drop procedure sp_configure

if exists (select * from sysobjects
		where sysstat & 0xf = 4
			and name = 'sp_create_removable')
	drop procedure sp_create_removable

if exists (select * from sysobjects
		where sysstat & 0xf = 4
			and name = 'sp_certify_removable')
	drop procedure sp_certify_removable

if exists (select * from sysobjects
		where sysstat & 0xf = 4
			and name = 'sp_check_removable')
	drop procedure sp_check_removable

if exists (select * from sysobjects
		where sysstat & 0xf = 4
			and name = 'sp_cnst_csr')
	drop procedure sp_cnst_csr

if exists (select * from sysobjects
		where sysstat & 0xf = 4
			and name = 'sp_coalesce_fragments')
	drop procedure sp_coalesce_fragments

if exists (select * from sysobjects
		where sysstat & 0xf = 4
			and name = 'sp_dboption')
	drop procedure sp_dboption

if exists (select * from sysobjects
		where sysstat & 0xf = 4
			and name = 'sp_dbinstall')
	drop procedure sp_dbinstall

if exists (select * from sysobjects
		where sysstat & 0xf = 4
			and name = 'sp_dbremove')
	drop procedure sp_dbremove

if exists (select * from sysobjects
		where sysstat & 0xf = 4
			and name = 'sp_defaultdb')
	drop procedure sp_defaultdb

if exists (select * from sysobjects
		where sysstat & 0xf = 4
			and name = 'sp_defaultlanguage')
	drop procedure sp_defaultlanguage

if exists (select * from sysobjects
		where sysstat & 0xf = 4
			and name = 'sp_depends')
	drop procedure sp_depends

if exists (select * from sysobjects
		where sysstat & 0xf = 4
			and name = 'sp_devcreate')
	drop procedure sp_devcreate

if exists (select * from sysobjects
		where sysstat & 0xf = 4
			and name = 'sp_devoption')
	drop procedure sp_devoption

if exists (select * from sysobjects
		where sysstat & 0xf = 4
			and name = 'sp_diskdefault')
	drop procedure sp_diskdefault
go

if exists (select * from sysobjects
		where sysstat & 0xf = 4
			and name = 'sp_dropalias')
	drop procedure sp_dropalias

if exists (select * from sysobjects
		where sysstat & 0xf = 4
			and name = 'sp_dropdevice')
	drop procedure sp_dropdevice

if exists (select * from sysobjects
		where sysstat & 0xf = 4
			and name = 'sp_dropdumpdevice')
	drop procedure sp_dropdumpdevice

if exists (select * from sysobjects
		where sysstat & 0xf = 4
			and name = 'sp_dropextendedproc')
	drop procedure sp_dropextendedproc
go

dump transaction master with no_log
go
if exists (select * from sysobjects
		where sysstat & 0xf = 4
			and name = 'sp_dropgroup')
	drop procedure sp_dropgroup

if exists (select * from sysobjects
		where sysstat & 0xf = 4
			and name = 'sp_dropkey')
	drop procedure sp_dropkey

if exists (select * from sysobjects
		where sysstat & 0xf = 4
			and name = 'sp_droplanguage')
	drop procedure sp_droplanguage

if exists (select * from sysobjects
		where sysstat & 0xf = 4
			and name = 'sp_droplogin')
	drop procedure sp_droplogin

if exists (select * from sysobjects
		where sysstat & 0xf = 4
			and name = 'sp_dropmessage')
	drop procedure sp_dropmessage

if exists (select * from sysobjects
		where sysstat & 0xf = 4
			and name = 'sp_droptype')
	drop procedure sp_droptype

if exists (select * from sysobjects
		where sysstat & 0xf = 4
			and name = 'sp_dropuser')
	drop procedure sp_dropuser

if exists (select * from sysobjects
		where sysstat & 0xf = 4
			and name = 'sp_dropremotelogin')
	drop procedure sp_dropremotelogin

if exists (select * from sysobjects
		where sysstat & 0xf = 4
			and name = 'sp_dropsegment')
	drop procedure sp_dropsegment

if exists (select * from sysobjects
		where sysstat & 0xf = 4
			and name = 'sp_dropserver')
	drop procedure sp_dropserver
go

if exists (select * from sysobjects
		where sysstat & 0xf = 4
			and name = 'sp_extendsegment')
	drop procedure sp_extendsegment
go

if exists (select * from sysobjects where type='P ' and
		 name='sp_fallback_activate_svr')
	drop procedure sp_fallback_activate_svr
if exists (select * from sysobjects where type='P ' and
		 name='sp_fallback_activate_svr_db')
	drop procedure sp_fallback_activate_svr_db

if exists (select * from sysobjects where type='P ' and
		 name='sp_fallback_deactivate_svr')
	drop procedure sp_fallback_deactivate_svr
if exists (select * from sysobjects where type='P ' and
		 name='sp_fallback_deactivate_svr_db')
	drop procedure sp_fallback_deactivate_svr_db

if exists (select * from sysobjects where type='P ' and
		 name='sp_fallback_enroll_svr_db')
	drop procedure sp_fallback_enroll_svr_db

if exists (select * from sysobjects where type='P ' and
		 name='sp_fallback_help')
	drop procedure sp_fallback_help

if exists (select * from sysobjects where type='P ' and
		 name='sp_fallback_help_db_dev')
	drop procedure sp_fallback_help_db_dev

if exists (select * from sysobjects where type='P ' and
		 name='sp_fallback_MS_enroll_db')
	drop procedure sp_fallback_MS_enroll_db

if exists (select * from sysobjects where type='P ' and
		 name='sp_fallback_MS_enroll_dev')
	drop procedure sp_fallback_MS_enroll_dev

if exists (select * from sysobjects where type='P ' and
		 name='sp_fallback_MS_enroll_usg')
	drop procedure sp_fallback_MS_enroll_usg

if exists (select * from sysobjects where type='P ' and
		 name='sp_fallback_MS_sel_fb_svr')
	drop procedure sp_fallback_MS_sel_fb_svr

if exists (select * from sysobjects where type='P ' and
		 name='sp_fallback_MS_verify_ri')
	drop procedure sp_fallback_MS_verify_ri

if exists (select * from sysobjects where type='P ' and
		 name='sp_fallback_permanent_svr')
	drop procedure sp_fallback_permanent_svr

if exists (select * from sysobjects where type='P ' and
		 name='sp_fallback_upd_dev_drive')
	drop procedure sp_fallback_upd_dev_drive

if exists (select * from sysobjects where type='P ' and
		 name='sp_fallback_withdraw_svr_db')
	drop procedure sp_fallback_withdraw_svr_db
go

if exists (select * from sysobjects
		where sysstat & 0xf = 4
			and name = 'sp_fixindex')
	drop procedure sp_fixindex

if exists (select * from sysobjects
		where sysstat & 0xf = 4
			and name = 'sp_foreignkey')
	drop procedure sp_foreignkey
go

dump transaction master with no_log
go

if exists (select * from sysobjects
		where sysstat & 0xf = 4
			and name = 'sp_help')
	drop procedure sp_help

if exists (select * from sysobjects
		where sysstat & 0x0f = 4
			and name = 'sp_help_revdatabase')
		     drop procedure sp_help_revdatabase

if exists (select * from sysobjects
		where sysstat & 0xf = 4
			and name = 'sp_helpconstraint')
	drop procedure sp_helpconstraint

if exists (select * from sysobjects
		where sysstat & 0xf = 4
			and name = 'sp_helpdb')
	drop procedure sp_helpdb

if exists (select * from sysobjects
		where sysstat & 0xf = 4
			and name = 'sp_helpdevice')
	drop procedure sp_helpdevice

if exists (select * from sysobjects
		where sysstat & 0xf = 4
			and name = 'sp_helpextendedproc')
	drop procedure sp_helpextendedproc

if exists (select * from sysobjects
		where sysstat & 0xf = 4
			and name = 'sp_helpgroup')
	drop procedure sp_helpgroup

if exists (select * from sysobjects
		where sysstat & 0xf = 4
			and name = 'sp_helpindex')
	drop procedure sp_helpindex

if exists (select * from sysobjects
		where sysstat & 0xf = 4
			and name = 'sp_helpjoins')
	drop procedure sp_helpjoins

if exists (select * from sysobjects
		where sysstat & 0xf = 4
			and name = 'sp_helpkey')
	drop procedure sp_helpkey
go

dump transaction master with no_log
go

if exists (select * from sysobjects
		where sysstat & 0xf = 4
			and name = 'sp_helplanguage')
	drop procedure sp_helplanguage

if exists (select * from sysobjects
		where sysstat & 0xf = 4
			and name = 'sp_helplog')
	drop procedure sp_helplog

if exists (select * from sysobjects
		where sysstat & 0xf = 4
			and name = 'sp_helplogins')
		     drop procedure sp_helplogins

if exists (select * from sysobjects
		where sysstat & 0xf = 4
			and name = 'sp_helprotect')
	drop procedure sp_helprotect

if exists (select * from sysobjects
		where sysstat & 0xf = 4
			and name = 'sp_helptext')
	drop procedure sp_helptext

if exists (select * from sysobjects
		where sysstat & 0xf = 4
			and name = 'sp_helpuser')
	drop procedure sp_helpuser

if exists (select * from sysobjects
		where sysstat & 0xf = 4
			and name = 'sp_helpremotelogin')
	drop procedure sp_helpremotelogin

if exists (select * from sysobjects
		where sysstat & 0xf = 4
			and name = 'sp_helpsegment')
	drop procedure sp_helpsegment

if exists (select * from sysobjects
		where sysstat & 0xf = 4
			and name = 'sp_helpserver')
	drop procedure sp_helpserver

if exists (select * from sysobjects
		where sysstat & 0xf = 4
			and name = 'sp_helpsort')
	drop procedure sp_helpsort

if exists (select * from sysobjects
		where sysstat & 0xf = 4
			and name = 'sp_helpstartup')
	drop procedure sp_helpstartup

if exists (select * from sysobjects
		where sysstat & 0xf = 4
			and name = 'sp_lock')
	drop procedure sp_lock

if exists (select * from sysobjects
		where sysstat & 0xf = 4
			and name = 'sp_lock2')
	drop procedure sp_lock2

if exists (select * from sysobjects
		where sysstat & 0xf = 4
			and name = 'sp_lockinfo')
	drop procedure sp_lockinfo
go

dump transaction master with no_log
go

if exists (select * from sysobjects
		where sysstat & 0xf = 4
			and name = 'sp_logdevice')
	drop procedure sp_logdevice

if exists (select * from sysobjects
		where sysstat & 0xf = 4
			and name = 'sp_lookup')
	drop procedure sp_lookup

if exists (select * from sysobjects
		where sysstat & 0xf = 4
			and name = 'sp_makestartup')
	drop procedure sp_makestartup

if exists (select * from sysobjects
		where sysstat & 0xf = 4
			and name = 'sp_markreport')
	drop procedure sp_markreport

if exists (select * from sysobjects
		where sysstat & 0xf = 4
			and name = 'sp_monitor')
	drop procedure sp_monitor

if exists (select * from sysobjects
		where sysstat & 0xf = 4
			and name = 'sp_namecrack')
	drop procedure sp_namecrack
if exists (select * from sysobjects
		where sysstat & 0xf = 4
			and name = 'sp_namecrack_qi')
	drop procedure sp_namecrack_qi

if exists (select * from sysobjects
		where sysstat & 0xf = 4
			and name = 'sp_objectsegment')
	drop procedure sp_objectsegment

if exists (select * from sysobjects
		where sysstat & 0xf = 4
			and name = 'sp_password')
	drop procedure sp_password

if exists (select * from sysobjects
		where sysstat & 0xf = 4
			and name = 'sp_placeobject')
	drop procedure sp_placeobject

if exists (select * from sysobjects
		where sysstat & 0xf = 4
			and name = 'sp_primarykey')
	drop procedure sp_primarykey


if exists (select * from sysobjects
		where sysstat & 0xf = 4
			and name = 'sp_probe_xact')
	drop procedure sp_probe_xact

if exists (select * from sysobjects
		where sysstat & 0xf = 4
			and name = 'sp_processinfo')
	drop procedure sp_processinfo

if (exists (select * from sysobjects
		where sysstat & 0xf = 4
			and name = 'sp_processmail'))
	drop procedure sp_processmail
go

if exists (select * from sysobjects
		where sysstat & 0xf = 4
			and name = 'sp_recompile')
	drop procedure sp_recompile

if exists (select * from sysobjects
		where sysstat & 0xf = 4
			and name = 'sp_remoteoption')
	drop procedure sp_remoteoption

if exists (select * from sysobjects
		where sysstat & 0xf = 4
			and name = 'sp_remove_xact')
	drop procedure sp_remove_xact

if exists (select * from sysobjects
		where sysstat & 0xf = 4
			and name = 'sp_rename')
	drop procedure sp_rename

if exists (select * from sysobjects
		where sysstat & 0xf = 4
			and name = 'sp_renamedb')
	drop procedure sp_renamedb

if exists (select * from sysobjects
		where sysstat & 0xf = 4
			and name = 'sp_scan_xact')
	drop procedure sp_scan_xact

if exists (select * from sysobjects
		where sysstat & 0xf = 4
			and name = 'sp_setnetname')
	drop procedure sp_setnetname

if exists (select * from sysobjects
		where sysstat & 0xf = 4
			and name = 'sp_serveroption')
	drop procedure sp_serveroption

if exists (select * from sysobjects
		where sysstat & 0xf = 4
			and name = 'sp_spaceused')
	drop procedure sp_spaceused

if exists (select * from sysobjects
		where sysstat & 0xf = 4
			and name = 'sp_sqlexec')
	drop procedure sp_sqlexec

if exists (select * from sysobjects
		where sysstat & 0xf = 4
			and name = 'sp_start_xact')
	drop procedure sp_start_xact

if exists (select * from sysobjects
		where sysstat & 0xf = 4
			and name = 'sp_stat_xact')
	drop procedure sp_stat_xact
go

if exists (select * from sysobjects
		where sysstat & 0xf = 4
			and name = 'sp_tableoption')
	drop procedure sp_tableoption

if exists (select * from sysobjects
		where sysstat & 0xf = 4
			and name = 'sp_tempdbspace')
	drop procedure sp_tempdbspace

if exists (select * from sysobjects
		where sysstat & 0xf = 4
			and name = 'sp_unbindefault')
	drop procedure sp_unbindefault

if exists (select * from sysobjects
		where sysstat & 0xf = 4
			and name = 'sp_unbindrule')
	drop procedure sp_unbindrule

if exists (select * from sysobjects
		where sysstat & 0xf = 4
			and name = 'sp_unmakestartup')
	drop procedure sp_unmakestartup


if exists (select * from sysobjects where type='P ' and
                 name='sp_user_counter1')
        drop procedure sp_user_counter1
if exists (select * from sysobjects where type='P ' and
                 name='sp_user_counter2')
        drop procedure sp_user_counter2
if exists (select * from sysobjects where type='P ' and
                 name='sp_user_counter3')
        drop procedure sp_user_counter3
if exists (select * from sysobjects where type='P ' and
                 name='sp_user_counter4')
        drop procedure sp_user_counter4
if exists (select * from sysobjects where type='P ' and
                 name='sp_user_counter5')
        drop procedure sp_user_counter5
if exists (select * from sysobjects where type='P ' and
                 name='sp_user_counter6')
        drop procedure sp_user_counter6
if exists (select * from sysobjects where type='P ' and
                 name='sp_user_counter7')
        drop procedure sp_user_counter7
if exists (select * from sysobjects where type='P ' and
                 name='sp_user_counter8')
        drop procedure sp_user_counter8
if exists (select * from sysobjects where type='P ' and
                 name='sp_user_counter9')
        drop procedure sp_user_counter9
if exists (select * from sysobjects where type='P ' and
                 name='sp_user_counter10')
        drop procedure sp_user_counter10
go
if exists (select * from sysobjects where type='P ' and
                 name='sp_userdefcounters')
        drop procedure sp_userdefcounters
go


if exists (select * from sysobjects
		where sysstat & 0xf = 4
			and name = 'sp_validaltlang')
	drop procedure sp_validaltlang

if exists (select * from sysobjects
		where sysstat & 0xf = 4
			and name = 'sp_validlang')
	drop procedure sp_validlang

if exists (select * from sysobjects
		where sysstat & 0xf = 4
			and name = 'sp_validname')
	drop procedure sp_validname

if exists (select * from sysobjects
		where sysstat & 0xf = 4
			and name = 'sp_who')
	drop procedure sp_who

if exists (select * from sysobjects
		where sysstat & 0xf = 4
			and name = 'sp_who2')
	drop procedure sp_who2
go

if exists ( select * from sysobjects
		where sysstat & 0xf = 4
			and name = 'MS_sqlctrs_users' )
    drop proc MS_sqlctrs_users
go

dump transaction master with no_log
go
print ''
go


raiserror('Special command to fix up freeoff on page 24 in all databases: dbcc dbr- (m-, f-)',0,1)
go
dbcc dbrepair (master, fixsysindex) --Why located in this file ?!
go



---------------------------------------------------------------
---------------------------------------------------------------
raiserror('Creating the general purpose System Stored Procedures ....',0,1)
---------------------------------------------------------------
---------------------------------------------------------------

select count(*) ,'= the number of system stored procs in master..sysobjects'
	from sysobjects where type='P ' and name like 'sp[_]%'

raiserror(' ',0,1)
go


raiserror(15339,-1,-1,'sp_user_counter1 - N')
go

create proc sp_user_counter1 as
select 0
go
create proc sp_user_counter2 as
select 0
go
create proc sp_user_counter3 as
select 0
go
create proc sp_user_counter4 as
select 0
go
create proc sp_user_counter5 as
select 0
go
create proc sp_user_counter6 as
select 0
go
create proc sp_user_counter7 as
select 0
go
create proc sp_user_counter8 as
select 0
go
create proc sp_user_counter9 as
select 0
go
create proc sp_user_counter10 as
select 0
go

raiserror(15339,-1,-1,'sp_userdefcounters')
go
create proc sp_userdefcounters as
exec sp_user_counter1
exec sp_user_counter2
exec sp_user_counter3
exec sp_user_counter4
exec sp_user_counter5
exec sp_user_counter6
exec sp_user_counter7
exec sp_user_counter8
exec sp_user_counter9
exec sp_user_counter10
go


raiserror(15339,-1,-1,'sp_blockcnt')
go
--perfmon
create procedure sp_blockcnt as
select blockedusers=count(*) from sysprocesses where blocked <> 0
go



raiserror(15339,-1,-1,'sp_tempdbspace')
go
--new query to watch max tempdbspace from perfmon
create proc sp_tempdbspace as

declare @dbsize dec(15,0)
declare @freespace dec(15,0)
declare @spaceused dec(15,0)

select @dbsize = sum(convert(dec(15),size))
               from master..sysusages
               where dbid = db_id('tempdb')

select  database_name = 'tempdb',
               database_size = (@dbsize / 512),
	       spaceused=(@dbsize/512-(@dbsize/512 -
                                 (select sum(convert(dec(15),reserved))
                from tempdb..sysindexes
	where indid in (0, 1, 255))/512))
go



raiserror(15339,-1,-1,'sp_devoption')
go
create procedure sp_devoption
@devname varchar(30) = NULL,		-- device name to change
@optname varchar(20) = NULL,		-- option name to turn on/off
@optvalue varchar(10) = NULL,		-- true or false
@override varchar(8)=	NULL
as
declare @statvalue int
declare @optcount int

select @optname=lower(@optname)
select @optvalue=lower(@optvalue)
select @override=lower(@override)

/*
** If no parameters were supplied, list settable options.
*/
if @devname is null
 	begin
		select 'Settable device options:' = name
			from master.dbo.spt_values
			where type = 'V'
				and number in (4096,8192)
			order by name
		return (0)
	end

/*
** Verify that device exists.
*/
if not exists (select * from sysdevices where name=@devname)
	begin
		raiserror(15012,-1,-1,@devname)
		return (1)
	end

/*
** If no option was supplied, display current settings.
*/
if @optname is null
	begin
		select 'The following options are set:' = v.name
			from spt_values v, sysdevices d
			where d.name=@devname
				and v.type = 'V'
				and (v.number & d.status)=v.number
				and v.number in (4096,8192)
		return(0)
	end

/*
** Can't change settings for 'master' device.
*/
if @devname = 'master'
	begin
		raiserror(15133,-1,-1)
		return (1)
	end

/*
**  Use @optname and try to find the right option.
**  If there isn't just one, print appropriate diagnostics and return.
*/
select @statvalue = min(number), @optcount = count(*)
	from master.dbo.spt_values
	where lower(name) like '%' + @optname + '%'
		and type = 'V'
		and number in (4096,8192)

if @optcount = 0
	begin
		raiserror(15013,-1,-1,@optname)
		select 'Settable device options:' = name
			from master.dbo.spt_values
			where type = 'V'
				and number in (4096,8192)
			order by name
		return (1)
	end

/*
**  If more than one option like @optname, show the duplicates and return.
*/
if @optcount > 1
begin
	raiserror(15135,-1,-1,@optname)
	select duplicate_options = name
		from master.dbo.spt_values
		where lower(name) like '%' + @optname + '%'
			and type = 'V'
			and number in (4096,8192)
	return (1)
end

/*
**  Check optvalue.
*/
if @optvalue not in ('true', 'false') or @optvalue is null
	begin
		raiserror(15136,-1,-1)
		return (1)
	end

/*
**  Check @override.
*/
if @override <> 'override' and @override is not null
	begin
		raiserror(15281,-1,-1,@override)
		return(1)
	end

/*
**  Can only supply @override if setting 'read only' device option.
*/
if @override is not null and not exists
	(select * from spt_values where type = 'V' and
		lower(name) like '%' + @optname + '%'
		and number = 4096)
	begin
		raiserror(15282,-1,-1)
		return(1)
	end

/*
**  Only the SA can execute the update part of this procedure.
*/
if suser_id() <> 1
	begin
		raiserror(15137,-1,-1)
		return (1)
	end

/*
**  If we're in a transaction, disallow this since it might make recovery
**  impossible.
*/

if @@trancount > 0
	begin
		raiserror(15002,-1,-1,'sp_devoption')
		return (1)
	end

/*
**  Get the number which is the bit value to set
*/
select @statvalue = number
	from master.dbo.spt_values
	where lower(name) = @optname and type = 'V'
		and number in (4096,8192)

/*
** If setting device to 'read only', make certain that all databases
** on that device have been set to 'read only' or that user specified
** override.
*/
if @statvalue = 4096 and isnull(@override,'no override') <> 'override'
	and @optvalue='true'
	and exists (select * from sysdatabases d, sysusages u, sysdevices v
					where  d.dbid = u.dbid
					and u.vstart between v.low and v.high
					and v.name = @devname
					and (d.status&1024)=0)
	begin
		raiserror(15138,-1,-1)
		select distinct 'Writable database(s)' = d.name
			from sysdatabases d, sysusages u, sysdevices v
				where  d.dbid = u.dbid
				and u.vstart between v.low and v.high
				and v.name = @devname
				and (d.status&1024)=0
		return (1)
	end

if @optvalue='true'
	update sysdevices set status=status|@statvalue where name=@devname
else if lower(@optvalue)='false'
	update sysdevices set status=status & (~@statvalue) where name=@devname

print 'Device option set.'
return (0)
go


raiserror(15339,-1,-1,'sp_dboption')
go
create procedure sp_dboption  -- 1996/03/15 12:51
@dbname varchar(30) = NULL,     /* database name to change */
@optname varchar(35) = NULL,  /* option name to turn on/off */
@optvalue varchar(10) = NULL  /* true or false */
as

set nocount    on
set ansi_nulls on

declare @dbid int         /* dbid of the database */
declare @dbuid int           /* id of the owner of the database */
declare @statvalue int     /* number of option */
declare @optcount int      /* number of options like @optname */
declare @allopts int    /* bit map off all options that can be
            ** set by sp_dboption. */
declare @devname varchar(30)
declare @cur_dbname varchar(30)
declare @pmsg varchar(255)
declare @ret_code int
declare @int1 int
 ,@opt_trunclogonchkpt integer
 ,@fulloptname varchar(35)
 ,@orig_db_status smallint
 ,@opt_bit_singleuser integer

/*
**  If no @dbname given, just list the possible dboptions.
**  Only certain status bits may be set or cleared by sp_dboption.
*/

/*
** Get bitmap of all options that can be set by sp_dboption.
*/
select @allopts=number from master.dbo.spt_values where type = 'D'
   and name = 'ALL SETTABLE OPTIONS'

if @dbname is null
begin
   select 'Settable database options:' = name
      from master.dbo.spt_values
      where type = 'D'
         and number & @allopts <> 0
         and number not in (-1,@allopts)  /* Eliminate non-option entries */
      order by name
   return (0)
end

/*
**  Verify the database name and get info
*/
select @dbid = dbid, @dbuid = suid ,@orig_db_status = status
   from sysdatabases
      where name = @dbname

/*
**  If @dbname not found, say so and list the databases.
*/
if @dbid is null
   begin
      raiserror(15010,-1,-1,@dbname)
      print ''
      select 'Available databases:' = name
         from sysdatabases
      return (1)
   end

/*
** If no option was supplied, display current settings.
*/
if @optname is null
   begin
      select 'The following options are set:' = v.name
         from master.dbo.spt_values v, sysdatabases d
            where d.name=@dbname
               and number & @allopts <> 0
               and number not in (-1,@allopts)  /* Eliminate non-option entries */
               and v.type = 'D'
               and ((v.number & d.status)=v.number
                  or d.category & (v.number/0x10000) <> 0)

      return(0)
   end

if lower(@optvalue) not in ('true', 'false') and @optvalue is not null
   begin
      raiserror(15241,-1,-1)
      return (1)
   end

/*
**  Use @optname and try to find the right option.
**  If there isn't just one, print appropriate diagnostics and return.
*/
select @statvalue = min(number), @optcount = count(*) ,@fulloptname = min(name)
      from master.dbo.spt_values
      where lower(name) like '%' + lower(@optname) + '%'
         and type = 'D'
         and number & @allopts <> 0
         and number not in (-1,@allopts)  /* Eliminate non-option entries */

/*
**  If no option, show the user what the options are.
*/
if @optcount = 0
   begin
      raiserror(15011,-1,-1,@optname)
      print ''

      select 'Settable database options:' = name
         from master.dbo.spt_values
         where type = 'D'
            and number & @allopts <> 0
            and number not in (-1,@allopts)  /* Eliminate non-option entries */
         order by name

      return (1)
   end

/*
**  If more than one option like @optname, show the duplicates and return.
*/
if @optcount > 1
   begin
      raiserror(15242,-1,-1,@optname)
      print ''

      select duplicate_options = name
         from master.dbo.spt_values
         where name like '%' + @optname + '%'
            and type = 'D'
            and number & @allopts <> 0
            and number not in (-1,@allopts)  /* Eliminate non-option entries */
      return (1)
   end

select @opt_trunclogonchkpt = min(number)
   from master..spt_values
   where type='D  ' and name='trunc. log on chkpt.'
/*
**  You can not change any of the options in master, except the 'trunc log on
**  checkpoint' option (8).  If the user tries to do so tell them they can't.
*/
if (@dbid = 1 and @statvalue <> @opt_trunclogonchkpt and @optvalue is not null)
   begin
      raiserror(15243,-1,-1,@optname)
      return (1)
   end

/*
**  Only the SA or the dbo of @dbname can execute the update part
**  of this procedure so check.
*/
if suser_id() <> 1 and suser_id() <> @dbuid and @optvalue is not null
   begin
      raiserror(15244,-1,-1)
      return (1)
   end

/*
**  If we're in a transaction, disallow this since it might make recovery
**  impossible.
*/
if @@trancount > 0 and @optvalue is not null
   begin
      raiserror(15002,-1,-1,'sp_dboption')
      return (1)
   end

select @cur_dbname=db_name()  /* Keep track of where we are. */


/*
**  Just want to see current setting of specified option.
*/
if @optvalue is null
begin
      select       @int1   = min(number)
         from      master.dbo.spt_values
         where     name LIKE '%' + @optname + '%'


      select OptionName = v.name

            ,CurrentSetting =
               CASE
                  When ( (v.number & d.status) = v.number
                       or d.category & (v.number/0x10000) <> 0
                       )
                     Then 'ON'
                  When NOT
                       ( (v.number & d.status) = v.number
                       or d.category & (v.number/0x10000) <> 0
                       )
                     Then 'off'
               END

         from master.dbo.spt_values v, sysdatabases d
            where d.name=@dbname
               and v.number & @allopts <> 0
               and v.number not in (-1,@allopts)   /* Eliminate non-option entries */
               and v.type = 'D'
               and v.number = @int1 and @int1 is not null

   return (0)
end


---- Pre-detect 'single user' conflicts.
select @opt_bit_singleuser = min(number) from master..spt_values
  where type='D  ' and name='single user'
if (@fulloptname           ='single user') --13834
   begin
   select @int1 = count(*) from master..sysprocesses
      where @dbname = db_name(dbid) and spid<>@@spid
   if (@int1 > 0 and
	(@orig_db_status & @opt_bit_singleuser > 0 or
	 lower(@optvalue) = 'true')
      )
      begin
      raiserror(15089,-1,-1,@fulloptname)
      return (1)
      end
   end


/*
**  Now update sysdatabases.
*/
if lower(@optvalue) = 'true'
begin
   /*
   **  If this is the option to make the database read only
   **  or to take it offline, we need to do some checking first.
   **  Unless it's the master db, no one can be using it.
   **  If it's the master db, only the SA may be using it.
   */
   if @statvalue in (512,1024)
      and (select count(*) from sysprocesses
         where dbid = @dbid
         and suid <> 1) > 0
      begin
         raiserror(15069,-1,-1)
         return (1)
      end

   /*
   ** If we're setting the database 'offline' - use 'dbcc dbcontrol'
   ** rather than setting the status bit directly.
   */
   if @statvalue = 512  -- 512 ON means db is Offline.
   begin

      if (0 = (@statvalue & (select status from master..sysdatabases where name=@dbname)))
         begin  -- Currently db is Online.

         dbcc dbcontrol(@dbname,offline)
         if @@error>0
            begin
               raiserror(15245,-1,-1)
               return (1)
            end
         else
            print 'Database is now offline'

         end
      else
         print 'Database already is offline.'

      /*
      ** Now close all open devices that belong solely to this database
      ** or are shared only by other offline databases.
      */
      exec('declare c1 cursor for
         select distinct v.name
         from master.dbo.sysdevices v, master.dbo.sysusages u, master.dbo.sysdatabases d
         where d.name = '''+
         @dbname+
         ''' and d.dbid = u.dbid
         and u.vstart between v.low and v.high
         and v.status & 2 = 2
         and v.status & 8192 = 0  -- Not already closed/deferred.
         and not exists '+
         '(select * from master.dbo.sysdevices v2,
               master.dbo.sysusages u2,
               master.dbo.sysdatabases d2
            where v2.name = v.name
            and d2.dbid = u2.dbid
            and vstart between v2.low and v2.high
            and d2.dbid <> d.dbid
            and d2.status & 512 <> 512)')

      open c1
      fetch c1 into @devname

      while @@fetch_status >= 0
      begin
         select @pmsg='Closing device '''+@devname+''' and marking it ''deferred''.'
         print @pmsg
         dbcc devcontrol(@devname,offline)
         exec sp_devoption @devname,deferred,true
         fetch c1 into @devname
      end
      deallocate c1
      /*
      ** Don't need to continue and set status bit or 'checkpoint'
      ** the database if option was 'offline' since the dbcc
      ** command does.
      */
      return(0)   /* Don't need to continue and 'checkpoint' db for this
            ** option. */
   end

   if @statvalue = 65536   /* We're publishing the database. */
      begin
         exec ('use ' + @dbname + ' exec sp_publishdb ' + @dbname
		+ ', ''true''')
         update sysdatabases
             set category = category | 1
             where name = @dbname
             print 'Database successfully published.'
             return(0)
      end

   if @statvalue = 131072  /* Allowing subscriptions in the database. */
      begin
         /* Add repl_publisher, dbo alias */

         exec ('use ' + @dbname + '
                if not exists (select * from sysalternates where
            suid = 16382) '+ /* repl_publisher id */
            'insert into sysalternates (suid, altsuid)
               values (16382, 1)'
              )

         update sysdatabases set category = category | 2
            where name = @dbname
         print 'Database successfully enabled for subscriptions.'
         return (0)
      end

   /* All other options get handled identically. */
   update sysdatabases set status = status | @statvalue
      where dbid = @dbid
end

/*
**  We want to set the requested option off.
*/

else

begin
   if @statvalue = 512     /* We're bringing it online. */
   begin
        /*
      ** Open all closed devices that this database uses.
      */
      exec('declare c1 cursor for
         select distinct v.name
         from master.dbo.sysdevices v, master.dbo.sysusages u, master.dbo.sysdatabases d
         where d.name = '''+
         @dbname+
         ''' and d.dbid = u.dbid
         and u.vstart between v.low and v.high
         and v.status & 2 = 2
         and v.status & 8192 > 0')

      open c1
      fetch c1 into @devname

      while @@fetch_status >= 0
      begin
         select @pmsg='Opening device '''+@devname+''' and marking it as non-deferred.'
         print @pmsg
         dbcc devcontrol(@devname,online)
         if @@error not in (0,5151,5155)
            begin
                                        deallocate c1
               raiserror(15246,-1,-1,@devname)
               return (1)
            end
         exec sp_devoption @devname,deferred,false
         fetch c1 into @devname
      end
      deallocate c1


      if (0 = (@statvalue & (select status from master..sysdatabases where name=@dbname)))
         -- Currently db is Online.
         print 'Database already is online.'
      else
         begin
         dbcc dbcontrol(@dbname,online)
         if @@error>0
            begin
               return (1)
            end
         else
            print 'Database is now online.'
         end

      return(0)
   end

   if @statvalue = 65536   /* We're disabling publishing for the database. */
      begin
         exec ('use ' + @dbname + ' exec sp_publishdb ' + @dbname
		+ ', ''false''')
         update sysdatabases
		set category = category & ~1
                where name = @dbname
	 print 'Database no longer published.'
      end

   if @statvalue = 131072  /* We're disabling subscriptions to database. */
      begin
         /* Drop repl_publisher, dbo alias */

         exec ('use ' + @dbname + '
                if     exists (select * from sysalternates where
            suid = 16382) '+ /* repl_publisher id */
            'delete sysalternates where suid = 16382'
              )

         update sysdatabases set category = category & ~2
            where name = @dbname
         print 'Database no longer enabled for subscriptions.'
         return (0)
      end

   /* All other options get handled identically. */
   update sysdatabases set status = status & ~@statvalue
      where dbid = @dbid
end

/*
**  CHECKPOINT the database that was changed.
*/
print 'CHECKPOINTing database that was changed.'

exec('use ' + @dbname + ' checkpoint')
select @int1 = @@error
if (@int1 <> 0)
	begin
	update		 master..sysdatabases
		set	 status = @orig_db_status
		where	 dbid   = @dbid
	end

return (0)
go



raiserror(15339,-1,-1,'sp_coalesce_fragments')
go
create procedure sp_coalesce_fragments  --1996/02/07 18:03
	 @DBNamePattern  varchar(44) = '%'
as

set nocount                on
set cursor_close_on_commit off

declare
	@RetCode int,
	@c1_vstart int,
	@c1_size int,
	@c1_dbid smallint

declare
	@2size int,
	@2segmap int,
	@countusagesdel int,
	@_rowcount int

select	@countusagesdel = 0
	,@RetCode = 0 --Assume good (1=bad)

if suser_id() <> 1
	begin
	raiserror(15003,-1,-1)  --must be SA
	select @RetCode = 1
	goto LABEL_86RETURN
	end

if @DBNamePattern is null
	begin
	select @DBNamePattern = ' n u l l'
	end

if not exists (select * from master.dbo.sysdatabases
		where name like @DBNamePattern)
	begin
	raiserror(15329,-1,-1)
	select @RetCode = 1
	goto LABEL_86RETURN
	end

---- Cursor to find a preceding contiguous fragment.

execute('declare c1 scroll cursor for
	select u1.vstart ,u1.size ,u1.dbid
	from master.dbo.sysusages u1
	where exists (select * from master.dbo.sysusages u2
			where u2.dbid = u1.dbid
			and u2.vstart = u1.vstart+u1.size
			and u2.lstart = u1.lstart+u1.size
			and u2.vstart between
				(select d1.low
					from master.dbo.sysdevices d1
					where u1.vstart between
						d1.low and d1.high
					and d1.status&2=2)
			and
				(select d2.high
					from master.dbo.sysdevices d2
					where u1.vstart between
						d2.low and d2.high
					and d2.status&2=2)
			)
	and db_name(u1.dbid) like ''' + @DBNamePattern + '''
	and u1.vstart > 0'
	)

-- Each loop iteration finds a sysusages row for which there is a
-- contiguous fragment at a higher vstart.   Then it gathers info on that
-- higher vstart row, and uses that info to delete the higher vstart
-- row and update the first row.

while 1=1
	begin

	open  c1
	fetch c1 into @c1_vstart,@c1_size ,@c1_dbid

	if @@fetch_status <> 0
		BREAK


	select @2size=null ,@2segmap=null

	select @2size=min(size) ,@2segmap=min(segmap)
		  from  master.dbo.sysusages
		  where vstart = @c1_vstart + @c1_size
		  and   dbid   = @c1_dbid

	select @_rowcount = @@rowcount

	if (@2size is null  OR  @2segmap is null)
		begin
		raiserror(15314,-1,-1,@c1_dbid,@c1_vstart) --corrupt sysusages
		select @RetCode = 1
		BREAK
		end


	select @countusagesdel = @countusagesdel + 1


BEGIN TRANSACTION txn_1a_sp_coalesce_fragments

	delete from sysusages
		  where vstart = @c1_vstart + @c1_size
		  and   dbid   = @c1_dbid

	if @@error <> 0
		begin
		raiserror('Inside txn_1a_, Del failed, will Rollback (1a1)!',17,127)
		ROLLBACK TRANSACTION --Even a @@trancount=3 would be reduced to =0.
		select @RetCode = 1
		GOTO LABEL_85DEALLOCC1
		end

	update sysusages
		set
			size   = size   + @2size,
			segmap = segmap | @2segmap
		where current of c1

	if @@error <> 0
		begin
		raiserror('Inside txn_1a_, Upd failed, will Rollback (1a2)!',17,127)
		ROLLBACK TRANSACTION --Even a @@trancount=3 would be reduced to =0.
		select @RetCode = 1
		GOTO LABEL_85DEALLOCC1
		end

COMMIT TRANSACTION txn_1a_sp_coalesce_fragments


	close c1  --force refreshing open
		
 	end --of loop


raiserror(15315,-1,-1,@countusagesdel)

LABEL_85DEALLOCC1:

deallocate c1

LABEL_86RETURN:

return @RetCode
go


---- Top_Fallback ("Failover")


raiserror(15339,-1,-1,'sp_fallback_MS_enroll_db')
go
create procedure sp_fallback_MS_enroll_db  --1996/04/10 12:54
    @pFallbackSvrName   varchar(30)
   ,@p_db_name         varchar(30)

   ,@p_db_dbid         smallint       OUTPUT
   ,@p_db_status       smallint       OUTPUT
   ,@p_db_version      smallint       OUTPUT
as
/********1*********2*********3*********4*********5*********6*********7**
      This sproc's name contains the substring 'MS',
and thus is undocumented and should NOT be directly
executed by the SA.
      This sproc must be and will be executed on the primary server,
by an RPC issued from a fallback server.
      This sproc does not modify any data.
      This sproc contains extra nondb edits, e.g. for sysservers.
*********1*********2*********3*********4*********5*********6*********7*/

Set nocount    on
Set ansi_nulls on

Declare
       @RetCode                  integer  -- 0==good ,>0==bad ,<0==no_action
      ,@_error                   integer
      ,@int1                     integer
      ,@vc1                      varchar(30)
      ,@FBBit                    integer

-----------------------------------

Select
       @RetCode                  = 0


Select @FBBit = 64

--SELECT       @FBBit = min(val.number)
--      from   master..spt_values  val
--      where  val.type = 'A  '
--      and    val.name = 'fallback'


------------------------  Ensure fallback svr has been added  --------------
/***
IF (@@servername <> @pFallbackSvrName)
   begin

   SELECT
             @int1         = count(*)
            ,@vc1          = min(syssvr.srvname)
      from
             master..sysservers     syssvr
      where
            (     syssvr.srvname   = @pFallbackSvrName
             and  syssvr.srvstatus & @FBBit > 0
             and  syssvr.srvid    <> 0
            )

   IF (     @int1 <> 1
      OR    @vc1  <> @pFallbackSvrName
      )
      begin
      RaisError(15359,-1,-1,@pFallbackSvrName)
      Select @RetCode = 1
      GOTO LABEL_86_RETURN
      end
   end
***/

---------------------

SELECT
             @p_db_dbid      = min(sysdb.dbid)
            ,@p_db_status    = min(sysdb.status)
            ,@p_db_version   = min(sysdb.version)
      from
             master..sysdatabases   sysdb
      where
             sysdb.name      = @p_db_name


LABEL_86_RETURN:

Return @RetCode
go



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



raiserror(15339,-1,-1,'sp_fallback_MS_sel_fb_svr')
go
create procedure sp_fallback_MS_sel_fb_svr  --1996/02/14 10:44
    @pFallbackSvrName    character varying(30)   OUTPUT
as
/********1*********2*********3*********4*********5*********6*********7**
      This sproc should be executed on the primary server.
      This sproc is used by ODBC & DBLib when first connecting.
      This sproc will assign a null to the output parm in the
beginning, but will try to fill the parm with the name of any one of
the fallback servers defined in sysservers, for this primary server.
Ideally there will be only one fallback server defined for any one
primary server which might crash.
*********1*********2*********3*********4*********5*********6*********7*/

Set nocount                   on
Set ansi_nulls                on

Declare
       @FBBit                 integer

-----------------------------------

SELECT       @FBBit = min(val.number)
      from   master..spt_values  val
      where  val.type = 'A  '
      and    name     = 'fallback'

-------------------------------------

SELECT       @pFallbackSvrName   = min(syssvr.srvnetname) -- sp_setnetname
      from
             master..sysservers  syssvr
      where
             syssvr.srvstatus    & @FBBit > 0


Return 0
go



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



raiserror(15339,-1,-1,'sp_fallback_help')
go
create procedure sp_fallback_help  --1996/02/17 22:03
    @pPrimarySvrName character varying(30)   = '%'
as
/********1*********2*********3*********4*********5*********6*********7**
      This sproc should be executed on the fallback server.
*********1*********2*********3*********4*********5*********6*********7*/

Set nocount    on
Set ansi_nulls on

Declare
       @RetCode                  integer
      ,@ExecRC                   integer
      ,@AllIntegGoodRC           integer
      ,@charMaxLenPhyname        character varying(12)

--------------------------------------

Select
       @RetCode                  = 0
      ,@AllIntegGoodRC           = 0

--------------------------- Integrities?

Execute @ExecRC = sp_fallback_MS_verify_ri

IF (@ExecRC <> 0)
   begin
   Select @AllIntegGoodRC = @ExecRC
   Select @RetCode        = @ExecRC
   end

------------------------------------------ db

EXECUTE(
'
Set nocount off

SELECT
             ''Databases''          as ''InfoType ''
            ,CASE
               When db.xfallback_dbid is not null Then ''YES''
               When db.xfallback_dbid is     null Then ''no''
             END
                                    as ''Active''
            ,db.xserver_name       as ''PrimaryServerName''
            ,db.name               as ''DatabaseName''
            ,db.xdttm_ins          as ''DateEnrolled''
            ,db.xdttm_last_ins_upd as ''DateLastUpd''
            ,db.xfallback_dbid     as ''FallbackDBId''
            ,db.dbid               as ''PrimaryDBId''
      from
             master..spt_fallback_db  db
      where
             db.xserver_name   LIKE ''' + @pPrimarySvrName + '''
      order by
             db.xserver_name
            ,db.name
')


----------------------------------------- dev

SELECT
             @charMaxLenPhyname
                  = isnull(convert(varchar,max(datalength(dev.phyname))),'0')
      from
             master..spt_fallback_dev   dev


IF convert(integer,@charMaxLenPhyname) <
                       Datalength('PhynameOnPrimaryServer')
   Select          @charMaxLenPhyname  =
       convert(varchar,Datalength('PhynameOnPrimaryServer'))



EXECUTE(
'
Set nocount off

SELECT
             ''Devices''            as ''InfoType ''
            ,CASE
               When dev.xfallback_low is not null Then ''YES''
               When dev.xfallback_low is     null Then ''no''
             END
                                    as ''Active''
            ,dev.xserver_name       as ''PrimaryServerName''
            ,dev.name               as ''LogicalDeviceName''
            ,dev.xdttm_ins          as ''DateEnrolled''
            ,dev.xdttm_last_ins_upd as ''DateLastUpd''
            ,dev.xfallback_drive    as ''FBDrive''
            ,substring(dev.phyname,1,' + @charMaxLenPhyname + ')
                                    as ''PhynameOnPrimaryServer''
            ,dev.xfallback_low      as ''FallbackLow''
      from
             master..spt_fallback_dev  dev
      where
             dev.xserver_name   LIKE ''' + @pPrimarySvrName + '''
      order by
             dev.xserver_name
            ,dev.name
')


-----------------------------------------  usg

IF (@AllIntegGoodRC = 0)
   begin

   EXECUTE(
   '
   Set nocount off

   SELECT
             ''Usages''             as ''InfoType ''
            ,CASE
               When usg.xfallback_vstart is not null Then ''YES''
               When usg.xfallback_vstart is     null Then ''no''
             END
                                    as ''Active''
            ,usg.xserver_name       as ''PrimaryServerName''
            ,db.name               as ''DatabaseName''
            ,dev.name               as ''LogicalDeviceName''
            ,usg.xfallback_vstart   as ''FallbackVstart''
            ,usg.xdttm_ins          as ''DateEnrolled''
            ,usg.xdttm_last_ins_upd as ''DateLastUpd''
            ,usg.dbid               as ''DBIdOnPrimaryServer''
            ,usg.vstart             as ''VstartOnPrimaryServer''
      from
             master..spt_fallback_usg  usg
            ,master..spt_fallback_db  db
            ,master..spt_fallback_dev  dev
      where
             usg.xserver_name   LIKE ''' + @pPrimarySvrName + '''
      and    usg.dbid = db.dbid
      and    usg.vstart between dev.low and dev.high
      order by
             usg.xserver_name
            ,usg.vstart
   ')
   end
ELSE  ---- Integrity problem, play report generation safe.
   begin

   EXECUTE(
   '
   Set nocount off

   SELECT
             ''Usages''             as ''InfoType ''
            ,CASE
               When usg.xfallback_vstart is not null Then ''YES''
               When usg.xfallback_vstart is     null Then ''no''
             END
                                    as ''Active''
            ,usg.xserver_name       as ''PrimaryServerName''
            ,usg.xfallback_vstart   as ''FallbackVstart''
            ,usg.xdttm_ins          as ''DateEnrolled''
            ,usg.xdttm_last_ins_upd as ''DateLastUpd''
            ,usg.dbid               as ''DBIdOnPrimaryServer''
            ,usg.vstart             as ''VstartOnPrimaryServer''
      from
             master..spt_fallback_usg  usg
      where
             usg.xserver_name   LIKE ''' + @pPrimarySvrName + '''
      order by
             usg.xserver_name
            ,usg.vstart
   ')
   end


IF (@RetCode > 0)
   begin
   SELECT 'sysdb',dbid,name     from master..sysdatabases  order by dbid
   SELECT 'sysusg',dbid,vstart   from master..sysusages     order by dbid,vstart
   SELECT 'sysdev',low,high,name from master..sysdevices
            where cntrltype=0                               order by low
   end


Return @RetCode
go



raiserror(15339,-1,-1,'sp_fallback_help_db_dev')
go
create procedure sp_fallback_help_db_dev  --1996/03/26 16:04
    @pPrimarySvrName    varchar(30) = null
   ,@pFallbackDrive     varchar( 2) = null
   ,@pDbName            varchar(30) = null
as
/********1*********2*********3*********4*********5*********6*********7**
   Sometimes necessary to run sp_fallback_upd_dev_drive before
running this, to set values matching @pFallbackDrive.

EXAMPLES (too late for 6.5 BooksOnline):
   exec sp_fallback_help_db_dev 'PriSvr2' ,'G:'
   exec sp_fallback_help_db_dev 'PriSvr2' ,'G:' ,null
   exec sp_fallback_help_db_dev 'PriSvr2' ,null ,'DbSaucer'
   exec sp_fallback_help_db_dev 'PriSvr2' ,'G:' ,'DbSaucer' --Two independent reports.
   exec sp_fallback_help_db_dev '%'       ,'%'  ,'%'
*********1*********2*********3*********4*********5*********6*********7*/

Set nocount    on
Set ansi_nulls on

Declare
       @RetCode                  integer  -- 0==good ,>0==bad
      ,@_error                   integer
      ,@int1                     integer
      ,@vc1                      varchar(30)

-----------------------------------

Select
       @RetCode                  = 0

--------------------  Edit parms  --------------------

IF (           @pPrimarySvrName  is null
   OR    (     @pFallbackDrive   is null
         AND   @pDbName          is null
         )
   )
   begin
   RaisError(15393,11,1)
   Select @RetCode = @@error
   GOTO LABEL_86_RETURN
   end


IF (@pFallbackDrive is not null)
   begin

   SELECT
                db.xserver_name     as 'PrimarySvrName'
               ,dev.xfallback_drive as 'FallbackDrive'
               ,db.name             as 'DbName'
               ,dev.name            as 'DevName'
         from
                master..spt_fallback_db      db
               ,master..spt_fallback_dev     dev
               ,master..spt_fallback_usg     usg
         where
                usg.dbid            = db.dbid
         and    usg.vstart between dev.low and dev.high
         and    db.xserver_name     = usg.xserver_name
         and    db.xserver_name     = dev.xserver_name
         and    db.xserver_name     like @pPrimarySvrName
         and    dev.xfallback_drive like @pFallbackDrive
         order by
                db.xserver_name
               ,dev.xfallback_drive
               ,db.name
               ,dev.name
   end


IF (@pDbName is not null)
   begin

   SELECT
                db.xserver_name     as 'PrimarySvrName'
               ,db.name             as 'DbName'
               ,dev.name            as 'DevName'
         from
                master..spt_fallback_db      db
               ,master..spt_fallback_dev     dev
               ,master..spt_fallback_usg     usg
         where
                usg.dbid            = db.dbid
         and    usg.vstart between dev.low and dev.high
         and    db.xserver_name     = usg.xserver_name
         and    db.xserver_name     = dev.xserver_name
         and    db.xserver_name  like @pPrimarySvrName
         and    db.name          like db.name
         order by
                db.xserver_name
               ,db.name
               ,dev.name
   end


LABEL_86_RETURN:

Return @RetCode
go



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



raiserror(15339,-1,-1,'sp_fallback_activate_svr')
go
create procedure sp_fallback_activate_svr  --1996/02/14 12:22
    @pPrimarySvrName    character varying(30)   = null
as
/********1*********2*********3*********4*********5*********6*********7**
      This sproc is a mere shell for the newer _db extension version.
      This 6.5_beta name is still in shipped version only to
assist Compaq who was shown this name.  DEC & WinNT fallback
offerings will be shown only the _db extension version.  This mere
shell sproc will likely be discontinued in post 6.5 release.

      This sproc should be executed on the fallback server
directly (no RPC).
*********1*********2*********3*********4*********5*********6*********7*/

-- Declares will not cause "set implicit_transactions on"
--    to increment @@trancount from 0 to 1.
Declare
       @RetCode                  integer

IF (@@trancount > 0)
   begin
   RaisError(15002,-1,-1,'sp_fallback_activate_svr')
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
Set dateformat     ymd

Declare
       @ExecRC                   integer

Execute @ExecRC = sp_fallback_activate_svr_db @pPrimarySvrName ,'%'

Select @RetCode = @ExecRC

LABEL_86_RETURN:

Return @RetCode
go



raiserror(15339,-1,-1,'sp_fallback_deactivate_svr_db')
go
create procedure sp_fallback_deactivate_svr_db  --1996/02/29 11:07
    @pPrimarySvrName    character varying(30)   = null
   ,@pDbNamePattern    character varying(44)   = '%'
as
/********1*********2*********3*********4*********5*********6*********7**
      This sproc should be executed on the fallback server
(no RPC).
      This sproc will delete rows from system tables, and make
a will update the spt_fallback_% tables.
      As far as possible, this sproc is designed to overcome
small activation discrepancies and achieve the intended result.
*********1*********2*********3*********4*********5*********6*********7*/

Declare
       @RetCode                  integer

IF (@@trancount > 0)
   begin
   RaisError(15002,-1,-1,'sp_fallback_deactivate_svr_db')
   Select @RetCode = @@error
   GOTO LABEL_86_RETURN
   end

Set implicit_transactions     off

IF (@@trancount > 0)
   begin
   COMMIT TRANSACTION   -- Decrements @@trancount by 1.
   end

Set nocount                   on
Set ansi_nulls                on
Set cursor_close_on_commit    off
Set xact_abort                off

Declare
       @ExecRC                   integer
      ,@_error                   integer
      ,@ProcStartDtTm            datetime
      ,@Int1                     integer
      ,@BitDbOffline            integer

Declare
       @xfallback_dbid           smallint

      ,@db_name                 character varying(30)
      ,@db_dbid                 smallint

-----------------------------------

Select
       @RetCode                  = 0
      ,@ProcStartDtTm            = getdate()
      ,@BitDbOffline            = 512


----------------------  only SA  --------------------------------


IF (suser_id() <> 1)  -- must be SA
   begin
   RaisError(15003,-1,-1)
   Select @RetCode = @@error
   GOTO LABEL_86_RETURN
   end

----------------------  Need to lookup one primary svr?  --------------


IF (@pPrimarySvrName is null)
   begin

   SELECT    @Int1   = count(distinct db.xserver_name)
      from   master..spt_fallback_db  db
      where  db.xfallback_dbid        is not null

   IF (@Int1 <> 1)
      begin
      RaisError(15344,-1,-1,@Int1)
      Select @RetCode = @@error
      GOTO LABEL_86_RETURN
      end

   SELECT    @pPrimarySvrName = min(db.xserver_name)
      from   master..spt_fallback_db  db
      where  db.xfallback_dbid        is not null

   end

------------------  Handle db parm  ------------------------

IF (@pDbNamePattern is null)
   Select @pDbNamePattern = '%'


------  Fallback already currently deactivated for the primary server?

IF not exists (SELECT * from  master..spt_fallback_db db
                    where db.xfallback_dbid is not null
                    and   db.xserver_name = @pPrimarySvrName
                    and   db.name      like @pDbNamePattern
              )
   begin
   RaisError(15353,-1,-1,@@servername,@pPrimarySvrName)
   Select @RetCode = @@error
   GOTO LABEL_86_RETURN
   end


---------------  Calculate a list of possible sysdev's to delete  -----------


---- Capture devs containing to-be-deact dbs.
Create table #1dev_deact
   (low             integer     not null
   ,xfallback_low   integer     not null
   ,name            varchar(30) not null
   )

INSERT into #1dev_deact
               (low ,xfallback_low ,name)
   SELECT
         distinct
                dev.low ,dev.xfallback_low ,dev.name
         from   master..spt_fallback_dev  dev
               ,master..spt_fallback_usg  usg
               ,master..spt_fallback_db  db
         where  db.dbid           = usg.dbid
         and    usg.vstart between dev.low and dev.high
         and    db.xserver_name = usg.xserver_name
         and    db.xserver_name = dev.xserver_name
         and
                db.xserver_name = @pPrimarySvrName
         and    db.name      like @pDbNamePattern
         and    db.xfallback_dbid   is not null
         and    usg.xfallback_vstart is not null
         and    dev.xfallback_low    is not null


---- Capture devs containing dbs other than dbs to-be-deact.
Create table #2sysdev_othersuse
   (low        integer     not null
   ,name       varchar(30) not null
   )

INSERT into #2sysdev_othersuse
               (low ,name)
   SELECT
         distinct
                sysdev.low ,sysdev.name
         from   master..sysdevices        sysdev
               ,master..sysusages         sysusg
               ,master..sysdatabases      sysdb
         where  sysdb.dbid      = sysusg.dbid
         and    sysdev.cntrltype = 0
         and    sysusg.vstart between sysdev.low and sysdev.high
         and    not exists
                  (SELECT      *
                     from      master..spt_fallback_db  db
                     where     db.xserver_name    = @pPrimarySvrName
                     and       db.name      like    @pDbNamePattern
                     and       db.xfallback_dbid  is not null
                     and       db.xfallback_dbid  = sysdb.dbid
                  )


---- Remove some devs from the list of devs to-be-deact.
DELETE          #1dev_deact
      where     xfallback_low in
                  (SELECT low from #2sysdev_othersuse
                  )

---------------------------  Establish cursors  -----------------------


-------- csr sysdb

DECLARE
                csr_11_db
                insensitive
            cursor for
   SELECT
                db.xfallback_dbid
               ,db.dbid
               ,db.name
      from
                master..spt_fallback_db     db
      where
                db.xserver_name    = @pPrimarySvrName
      and       db.name         like @pDbNamePattern
      and       db.xfallback_dbid  is not null
   FOR read only


---------------------  Take each relevant db offline  ------------------


OPEN csr_11_db


WHILE ('11a'='11a')
   begin


   FETCH
                next
      from
                csr_11_db
      into
                @xfallback_dbid
               ,@db_dbid
               ,@db_name

   IF (@@fetch_status <> 0)
      begin
      Close csr_11_db  -- Will use this again.
      BREAK
      end

----------

   IF not exists   (
         SELECT       *
            from      master..sysdatabases   sysdb
            where     sysdb.dbid = @db_dbid
            and       sysdb.status & @BitDbOffline = 0  --currently is online
                  )
      begin

      CONTINUE

      end


   Execute @ExecRC = sp_dboption @db_name ,'offline' ,'true' --Also defers unshared sysdev.

   IF (@ExecRC <> 0)
      begin
      RaisError(15355,-1,-1,@db_name)
      Select @RetCode = @@error
      GOTO LABEL_86_RETURN
      end

   end -- loop 11a db

---------------------------------------------- txn

BEGIN TRANSACTION  -- @@trancount++;


---------------------  Delete sys% tables rows  ---------------


OPEN csr_11_db


WHILE ('11b'='11b')
   begin


   FETCH
                next
      from
                csr_11_db
      into
                @xfallback_dbid
               ,@db_dbid
               ,@db_name

   IF (@@fetch_status <> 0)
      begin
      Close csr_11_db
      BREAK
      end


-------- Handle usg.

   DELETE       master..sysusages
         where
                dbid    = @xfallback_dbid


   UPDATE       master..spt_fallback_usg
         set
                xdttm_last_ins_upd  = @ProcStartDtTm
               ,xfallback_vstart    = null
         where
                xserver_name        = @pPrimarySvrName
         and    dbid                = @db_dbid


-------- Handle db.

   DELETE       master..sysdatabases
         where
                dbid    = @xfallback_dbid


   UPDATE       master..spt_fallback_db
         set
                xdttm_last_ins_upd  = @ProcStartDtTm
               ,xfallback_dbid      = null
         where
                xfallback_dbid      = @xfallback_dbid

   end -- loop 11b db


--------------------  Del all corresponding dev  ---------------------


---- Del sysdevs whose children were all just deactivated.

DELETE       master..sysdevices
      where
             cntrltype = 0
      and    name in
               (SELECT name from #1dev_deact)
      and    low  in
               (SELECT xfallback_low from #1dev_deact)
      and    not exists
               (SELECT      *
                  from      master..sysusages   sysusg
                  where     sysusg.vstart between
                              master..sysdevices.low  and
                              master..sysdevices.high
               )

UPDATE       master..spt_fallback_dev
      set
             xdttm_last_ins_upd  = @ProcStartDtTm
            ,xfallback_low       = null
      where
             xserver_name        = @pPrimarySvrName
      and    xfallback_low  is not null
      and
             low in (SELECT low from #1dev_deact)


------------------------  Finalization  -------------------------


----------  Re-Verify integrity, before commit

Execute @ExecRC = sp_fallback_MS_verify_ri

IF (@ExecRC <> 0)
   begin
   RaisError(15352,-1,-1,'deactivate s d 2')  -- Do NOT internationalize these.
   Select @RetCode = @ExecRC
   Execute sp_fallback_help
   Rollback Transaction
   GOTO LABEL_86_RETURN
   end


COMMIT TRANSACTION


Raiserror(15356,-1,-1,@pPrimarySvrName)


LABEL_86_RETURN:

Deallocate csr_11_db

IF (object_id('tempdb..#1dev_deact') is not null)
            drop table #1dev_deact
IF (object_id('tempdb..#2sysdev_othersuse') is not null)
            drop table #2sysdev_othersuse

Return @RetCode
go



raiserror(15339,-1,-1,'sp_fallback_deactivate_svr')
go
create procedure sp_fallback_deactivate_svr  --1996/02/14 12:12
    @pPrimarySvrName    character varying(30)   = null
as
/********1*********2*********3*********4*********5*********6*********7**
      This sproc is a mere shell for the newer _db extension version.
      This 6.5_beta name is still in shipped version only to
assist Compaq who was shown this name.  DEC & WinNT fallback
offerings will be shown only the _db extension version.  This mere
shell sproc will likely be discontinued in post 6.5 release.

      This sproc should be executed on the fallback server
directly (no RPC).
*********1*********2*********3*********4*********5*********6*********7*/

Declare
       @RetCode                  integer

IF (@@trancount > 0)
   begin
   RaisError(15002,-1,-1,'sp_fallback_deactivate_svr')
   Select @RetCode = @@error
   GOTO LABEL_86_RETURN
   end

Set implicit_transactions     off

IF (@@trancount > 0)
   begin
   COMMIT TRANSACTION   -- Decrements @@trancount by 1.
   end

Set nocount                   on
Set ansi_nulls                on
Set cursor_close_on_commit    off
Set xact_abort                off

Declare
       @ExecRC                   integer


Execute @ExecRC = sp_fallback_deactivate_svr_db @pPrimarySvrName ,'%'

Select @RetCode = @ExecRC

LABEL_86_RETURN:

Return @RetCode
go



raiserror(15339,-1,-1,'sp_fallback_enroll_svr_db')
go
create procedure sp_fallback_enroll_svr_db  --1996/02/29 11:02
    @pPrimarySvrName  character varying(30)  -- Name of any one primary svr.
   ,@pDbName         character varying(30)  -- Local db, on primary svr.
as
/********1*********2*********3*********4*********5*********6*********7**
Fallback/Failover:
      This sproc must be executed on the fallback server,
as it will issue RPCs to the primary server to
execute some other sprocs there.
*********1*********2*********3*********4*********5*********6*********7*/

-- Declares will not cause "set implicit_transactions on"
--    to increment @@trancount from 0 to 1.
Declare
       @RetCode                  integer

IF (@@trancount > 0)
   begin
   RaisError(15002,-1,-1,'sp_fallback_enroll_svr_db')
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
Set dateformat     ymd

Declare
       @ExecRC                   integer
      ,@_error                   integer
      ,@_rowcount                integer
      ,@int1                     integer
      ,@vc1                      character varying(100)
      ,@ProcStartDtTm            datetime

Declare
       @db_dbid                 smallint
      ,@db_status               smallint
      ,@db_version              smallint

      ,@usg_segmap               integer
      ,@usg_lstart               integer
      ,@usg_size                 integer
      ,@usg_vstart               integer

      ,@dev_low                  integer
      ,@dev_high                 integer
      ,@dev_status               smallint
      ,@dev_name                 character varying(30)
      ,@dev_phyname              character varying(127)


--------

Select
       @RetCode                  = 0
      ,@ProcStartDtTm            = getdate()


----------------------  only SA  --------------------------------

IF (suser_id() <> 1)  -- must be SA
   begin
   RaisError(15003,-1,-1)
   Select @RetCode = @@error
   GOTO LABEL_86_RETURN
   end


--------------------  Edit primary svr parm  ---------------

IF (@pPrimarySvrName is null)
   begin
   RaisError(15344,-1,-1,0)
   Select @RetCode = @@error
   GOTO LABEL_86_RETURN
   end

-----------------------  Ensure db not already enrolled  --------------

IF exists
      (SELECT * from  master..spt_fallback_db
                where xserver_name = @pPrimarySvrName
                and   name         = @pDbName
      )
   begin
   RaisError(15380,-1,-1,@pDbName,@pPrimarySvrName)
   Select @RetCode = @@error
   GOTO LABEL_86_RETURN
   end


-----------------------  Exclude special db's  ---------------------

IF (lower(@pDbName) in ('master','model','tempdb','msdb'))
   begin
   RaisError(15385,-1,-1,@pDbName)
   Select @RetCode = @@error
   GOTO LABEL_86_RETURN
   end



-------------------  Ensure integrity is clean to start  ----------------

Execute @ExecRC = sp_fallback_MS_verify_ri

IF (@ExecRC <> 0)
   begin
   RaisError(15352,-1,-1,'enroll verify 1')
   Select @RetCode = @@error
   GOTO LABEL_86_RETURN
   end


-------------------  Simplify usages  ------------------------------

Execute sp_coalesce_fragments '%'


Select @vc1 = @pPrimarySvrName + '...sp_coalesce_fragments'  -- RPC

Execute @ExecRC = @vc1 '%'

Select @_error = @@error

IF (     @ExecRC  <> 0
   OR    @@error  <> 0
   )
   begin
   RaisError(15360,-1,-1,@pPrimarySvrName)
   Select @RetCode = @@error
   GOTO LABEL_86_RETURN
   end


-------------------  RPC to gather db info from primary  ----------------


Select @vc1 = @pPrimarySvrName + '...sp_fallback_MS_enroll_db'

Execute @ExecRC = @vc1
             @@servername
            ,@pDbName
               ,@db_dbid     OUTPUT
               ,@db_status   OUTPUT
               ,@db_version  OUTPUT

Select @_error = @@error

IF (     @ExecRC  <> 0
   OR    @@error  <> 0
   )
   begin
   RaisError(15362,-1,-1,'database')
   Select @RetCode = @@error
   GOTO LABEL_86_RETURN
   end

IF (@db_dbid is null)
   begin
   RaisError(15361,-1,-1,@pDbName,@pPrimarySvrName)
   Select @RetCode = @@error
   GOTO LABEL_86_RETURN
   end


------------------  Get internal db version number  ------------------

DBCC getvalue('current_version')
Select @_error = @@error

IF (@_error <> @db_version)
   begin
   RaisError(15364,-1,-1,@pPrimarySvrName,@pDbName
                        ,@db_version,@@servername,@_error)
   Select @RetCode = @@error
   GOTO LABEL_86_RETURN
   end

---------------------------------------------- txn


BEGIN TRANSACTION


------------------------  Store db info  ------------------------


INSERT       into   master..spt_fallback_db
            (
             xserver_name
            ,xdttm_ins
            ,xdttm_last_ins_upd
            ,xfallback_dbid

            ,name
            ,dbid
            ,status
            ,version
            )
      values
            (
             @pPrimarySvrName
            ,@ProcStartDtTm
            ,@ProcStartDtTm
            ,null

            ,@pDbName
            ,@db_dbid
            ,@db_status
            ,@db_version
            )

Select @_error = @@error

IF (@_error <> 0)
   begin
   Select @RetCode = @_error
   Rollback Transaction
   GOTO LABEL_86_RETURN
   end


---------  Handle self-enrollment db.

IF (@pPrimarySvrName = @@servername)
   begin

   UPDATE       master..spt_fallback_db
      set       xfallback_dbid   = dbid
      where     xdttm_ins        = xdttm_last_ins_upd
      and       xdttm_ins        = @ProcStartDtTm

   Select @_error = @@error

   IF (@_error <> 0)
      begin
      Select @RetCode = @_error
      Rollback Transaction
      GOTO LABEL_86_RETURN
      end
   end


----------------  RPC to get usg info  --------------------------------


Select @vc1 = @pPrimarySvrName + '...sp_fallback_MS_enroll_usg'

Select @usg_vstart = -1

WHILE ('usg1'='usg1')
   begin

   Execute @ExecRC = @vc1
                @db_dbid
                  ,@usg_segmap  OUTPUT
                  ,@usg_lstart  OUTPUT
                  ,@usg_size    OUTPUT
                  ,@usg_vstart  OUTPUT

   Select @_error = @@error

   IF (     @ExecRC   > 0
      OR    @_error  <> 0
      )
      begin
      RaisError(15362,-1,-1,'usages')
      Select @RetCode = @@error
      Rollback Transaction
      GOTO LABEL_86_RETURN
      end

   IF (@usg_lstart is null)
      begin
      BREAK
      end


   INSERT    into   master..spt_fallback_usg
            (
             xserver_name
            ,xdttm_ins
            ,xdttm_last_ins_upd
            ,xfallback_vstart

            ,dbid
            ,segmap
            ,lstart
            ,sizepg
            ,vstart
            )
      values
            (
             @pPrimarySvrName
            ,@ProcStartDtTm
            ,@ProcStartDtTm
            ,null

            ,@db_dbid
            ,@usg_segmap
            ,@usg_lstart
            ,@usg_size
            ,@usg_vstart
            )

   Select @_error = @@error

   IF (@_error <> 0)
      begin
      Select @RetCode = @_error
      Rollback Transaction
      GOTO LABEL_86_RETURN
      end

   end  --loop usg1


---------  Handle self-enrollment usg.

IF (@pPrimarySvrName = @@servername)
   begin

   UPDATE       master..spt_fallback_usg
      set       xfallback_vstart = vstart
      where     xdttm_ins        = xdttm_last_ins_upd
      and       xdttm_ins        = @ProcStartDtTm

   Select @_error = @@error

   IF (@_error <> 0)
      begin
      Select @RetCode = @_error
      Rollback Transaction
      GOTO LABEL_86_RETURN
      end
   end


----------------  RPC to get dev info  --------------------------------


Select @vc1 = @pPrimarySvrName + '...sp_fallback_MS_enroll_dev'

Select @dev_name = char(0x00)

WHILE ('dev2'='dev2')
   begin

   Execute @ExecRC = @vc1
                @db_dbid
                  ,@dev_low      OUTPUT
                  ,@dev_high     OUTPUT
                  ,@dev_status   OUTPUT
                  ,@dev_name     OUTPUT
                  ,@dev_phyname  OUTPUT

   Select @_error = @@error

   IF (     @ExecRC   > 0
      OR    @_error  <> 0
      )
      begin
      RaisError(15362,-1,-1,'devices')
      Select @RetCode = @@error
      Rollback Transaction
      GOTO LABEL_86_RETURN
      end

   IF (@dev_high is null)
      begin
      BREAK
      end


   IF exists
         (SELECT      *
            from      master..spt_fallback_dev  dev
            where     dev.xserver_name    = @pPrimarySvrName
            and       dev.low             = @dev_low
         )
      begin
      CONTINUE
      end


   INSERT    into   master..spt_fallback_dev
            (
             xserver_name
            ,xdttm_ins
            ,xdttm_last_ins_upd
            ,xfallback_drive

            ,low
            ,high
            ,status
            ,name
            ,phyname
            )
      values
            (
             @pPrimarySvrName
            ,@ProcStartDtTm
            ,@ProcStartDtTm
            ,null

            ,@dev_low
            ,@dev_high
            ,@dev_status
            ,@dev_name
            ,@dev_phyname
            )

   Select @_error = @@error

   IF (@_error <> 0)
      begin
      Select @RetCode = @_error
      Rollback Transaction
      GOTO LABEL_86_RETURN
      end

   end  --loop dev2


---------  Handle self-enrollment dev.

IF (@pPrimarySvrName = @@servername)
   begin

   UPDATE       master..spt_fallback_dev
      set       xfallback_low    = low
               ,xfallback_drive  = substring(phyname,1,2)
      where     xdttm_ins        = xdttm_last_ins_upd
      and       xdttm_ins        = @ProcStartDtTm

   Select @_error = @@error

   IF (@_error <> 0)
      begin
      Select @RetCode = @_error
      Rollback Transaction
      GOTO LABEL_86_RETURN
      end
   end


------------------  Re-ensure integrity  --------------------

Execute @ExecRC = sp_fallback_MS_verify_ri

IF (@ExecRC <> 0)
   begin
   Select @RetCode = 1
   Rollback Transaction
   GOTO LABEL_86_RETURN
   end


--------------------

COMMIT TRANSACTION

Raiserror(15375,-1,-1,@pDbName,@pPrimarySvrName)

LABEL_86_RETURN:

Return @RetCode
go



raiserror(15339,-1,-1,'sp_fallback_permanent_svr')
go
create procedure sp_fallback_permanent_svr  --1996/01/27 15:12
    @pPrimarySvrName character varying(30)   = null
   ,@pGeneralOptions integer                 = 0
as
/********1*********2*********3*********4*********5*********6*********7**
      This sproc should be executed on the fallback server.
*********1*********2*********3*********4*********5*********6*********7*/

Set nocount    on
Set ansi_nulls on

Declare
       @RetCode                  integer
      ,@ExecRC                   integer
      ,@_error                   integer
      ,@Int1                     integer
---------
Select
       @RetCode                  = 0

-------------------------  Edit override parm  --------------------

IF (@pGeneralOptions is null)
   Select @pGeneralOptions = 0

IF (@pGeneralOptions & 1 = 0)  -- Bit 1 ON means "Yes I really want to do this."
   begin
   RaisError(15376,-1,-1)
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


-----------------  Verify the one svr is active  -----------------


IF not exists
   (
   SELECT       *
      from      master..spt_fallback_db  db
      where     db.xserver_name    = @pPrimarySvrName
      and       db.xfallback_dbid  is not null
   )
   begin
   RaisError(15370,-1,-1,@pPrimarySvrName)
   Select @RetCode = @@error
   GOTO LABEL_86_RETURN
   end

IF exists
   (
   SELECT       *
      from      master..spt_fallback_db  db
      where     db.xserver_name    = @pPrimarySvrName
      and       db.xfallback_dbid  is     null
   )
   begin
   RaisError(15384,-1,-1,@pPrimarySvrName)
   Select @RetCode = @@error
   GOTO LABEL_86_RETURN
   end


-----------------------  Verify integrity, before txn  ---------------


Execute @ExecRC = sp_fallback_MS_verify_ri

IF (@ExecRC <> 0)
   begin
   RaisError(15352,-1,-1,'permanent 1')  -- Do NOT internationalize these.
   Select @RetCode = @ExecRC
   Execute sp_fallback_help
   GOTO LABEL_86_RETURN
   end


-----------------  Simply delete from the spt_fallback_% tables  ------------

BEGIN TRANSACTION


DELETE       master..spt_fallback_usg
      where  xserver_name     = @pPrimarySvrName


DELETE       master..spt_fallback_db
      where  xserver_name     = @pPrimarySvrName


DELETE       master..spt_fallback_dev
      where  xserver_name     = @pPrimarySvrName


-----------------------  Verify integrity, before commit  ---------------


Execute @ExecRC = sp_fallback_MS_verify_ri

IF (@ExecRC <> 0)
   begin
   RaisError(15352,-1,-1,'permanent 2')  -- Do NOT internationalize these.
   Select @RetCode = @ExecRC
   Execute sp_fallback_help
   Rollback Transaction
   GOTO LABEL_86_RETURN
   end


COMMIT TRANSACTION

Raiserror(15377,-1,-1,@pPrimarySvrName,@@servername)

LABEL_86_RETURN:

Return @RetCode
go



raiserror(15339,-1,-1,'sp_fallback_upd_dev_drive')
go
create procedure sp_fallback_upd_dev_drive  --1996/03/20 16:15
    @pPrimarySvrName    character varying(30)
   ,@pPrimaryDrive      character(2)
   ,@pFallbackDrive     character(2)
as
/********1*********2*********3*********4*********5*********6*********7**
      This sproc should be executed on the fallback server.
*********1*********2*********3*********4*********5*********6*********7*/

Set nocount    on
Set ansi_nulls on

Declare
       @RetCode                  integer  -- 0==good ,>0==bad
      ,@_error                   integer
      ,@_rowcount                integer
      ,@ProcStartDtTm            datetime

-------------------------------------

Select
       @RetCode                  = 0
      ,@ProcStartDtTm            = getdate()


----------------------  only SA  --------------------------------


IF (suser_id() <> 1)  -- must be SA
   begin
   RaisError(15003,-1,-1)
   Select @RetCode = @@error
   GOTO LABEL_86_RETURN
   end


--------------------  Foresee name conflicts  --------------------

IF exists
   (
   SELECT       *
      from      master..sysdevices        sysdev
               ,master..spt_fallback_dev  dev
      where     dev.xserver_name          = @pPrimarySvrName
      and       dev.xfallback_low        is null
      and       lower(substring(dev.phyname,1,2)) =
                lower(@pPrimaryDrive)
      and       sysdev.cntrltype = 0
      and      (@pFallbackDrive + substring(dev.phyname,3,125))
                                          = sysdev.phyname
   )
   begin
   RaisError(15371,-1,-1,@pFallbackDrive,@@servername)
   --Select @RetCode = @@error
   --GOTO LABEL_86_RETURN
   end


------------------------  Upd to new drive  ------------------------


UPDATE       master..spt_fallback_dev
      set
             xdttm_last_ins_upd        = @ProcStartDtTm
            ,xfallback_drive           = @pFallbackDrive
      where
             xserver_name              = @pPrimarySvrName
      and    substring(phyname,1,2)    = @pPrimaryDrive
      and    xfallback_low            is null


Select @_error = @@error ,@_rowcount = @@rowcount

IF (@_error <> 0)
   begin
   RaisError(15352,-1,-1,'sp_fallback_upd_dev_drive 1')
   Select @RetCode = @_error
   GOTO LABEL_86_RETURN
   end

Raiserror(15372,-1,-1,@_rowcount)


LABEL_86_RETURN:

Return @RetCode

go



raiserror(15339,-1,-1,'sp_fallback_withdraw_svr_db')
go
create procedure sp_fallback_withdraw_svr_db  --1996/02/21 14:44
    @pPrimarySvrName    character varying(30)
   ,@pDbNamePattern    character varying(44)   = '%'
as
/********1*********2*********3*********4*********5*********6*********7**
      This sproc should be executed on the fallback server.
*********1*********2*********3*********4*********5*********6*********7*/

Set nocount    on
Set ansi_nulls on

Declare
       @RetCode                  integer  -- 0==good ,>0==bad
      ,@ExecRC                   integer
      ,@_error                   integer
      ,@_rowcount                integer

Declare
       @CountDbWithdrawn        integer
      ,@CountDbStillEnrolled    integer

Declare
       @db_name                 character varying(30)
      ,@db_dbid                 smallint

-----------------------------------

Select
       @RetCode                  = 0
      ,@CountDbWithdrawn        = 0
      ,@CountDbStillEnrolled    = 0


----------------------  only SA  --------------------------------


IF (suser_id() <> 1)  -- must be SA
   begin
   RaisError(15003,-1,-1)
   Select @RetCode = @@error
   GOTO LABEL_86_RETURN
   end

-----------------------  Verify integrity, before txn  ---------------

Execute @ExecRC = sp_fallback_MS_verify_ri

IF (@ExecRC > 0)
   Execute sp_fallback_help

----------------  Verify fallback is inactive for this primary  --------------


IF exists
      (SELECT * from  master..spt_fallback_db  db
                where db.xserver_name    = @pPrimarySvrName
                and   db.xfallback_dbid  is not null
                and   db.name         like @pDbNamePattern
      )
   begin
   RaisError(15346,-1,-1,@@servername,@pPrimarySvrName)
   Select @RetCode = @@error
   GOTO LABEL_86_RETURN
   end



BEGIN TRANSACTION


----------------  Capture list of relevant dev's  ------------------

Create table #dev36
   (low        integer     not null
   ,name       varchar(30) not null
   )

INSERT into #dev36
               (    low     ,name)
   SELECT
         distinct
                dev.low ,dev.name
         from   master..spt_fallback_db  db
               ,master..spt_fallback_usg  usg
               ,master..spt_fallback_dev  dev
         where  db.dbid         = usg.dbid
         and    usg.vstart between dev.low and dev.high
         and    db.xserver_name = usg.xserver_name
         and    db.xserver_name = dev.xserver_name
         and    db.xserver_name = @pPrimarySvrName
         and    db.name like @pDbNamePattern
         and    db.xfallback_dbid  is     null
         and    dev.xfallback_low   is     null


------------------------  Del db, governs the loop  ----------------------

----------  Cursor, thru db's

DECLARE
             csr_7_db
             insensitive
         cursor for
   SELECT
             db.name
            ,db.dbid
      from
             master..spt_fallback_db     db
      where
             db.xserver_name    =    @pPrimarySvrName
      and    db.name            like @pDbNamePattern
      and    db.xfallback_dbid is null
   FOR read only

OPEN csr_7_db


WHILE (7=7)
   begin


   FETCH
                next
      from
                csr_7_db
      into
                @db_name
               ,@db_dbid

   IF (@@fetch_status <> 0)
      begin
      Deallocate csr_7_db
      BREAK
      end


---- Del usg.

   DELETE       master..spt_fallback_usg
         where
                xserver_name   = @pPrimarySvrName
         and    dbid           = @db_dbid


---- Del db.

   DELETE       master..spt_fallback_db
         where
                xserver_name   = @pPrimarySvrName
         and    dbid           = @db_dbid


   Select @CountDbWithdrawn = @CountDbWithdrawn + 1

   end -- loop 7=7 db


---------------------  Clean up dev  ---------------------


---- Del appropriate dev's.

DELETE          master..spt_fallback_dev
         where
                xserver_name        = @pPrimarySvrName
         and    low not in
                  (
                   SELECT      b_dev.low
                     from      master..spt_fallback_dev  b_dev
                              ,master..spt_fallback_usg  b_usg
                     where     b_usg.vstart between b_dev.low and b_dev.high
                     and       b_dev.xserver_name  = b_usg.xserver_name
                     and       b_dev.xserver_name  = @pPrimarySvrName
                  )
         and    low     in
                  (SELECT #dev36.low from #dev36)


------------------------  Finalization  -----------------------


----------  Verify integrity, before commit

Execute @ExecRC = sp_fallback_MS_verify_ri

IF (@ExecRC <> 0)
   begin
   RaisError(15352,-1,-1,'withdraw 2')  -- Do NOT internationalize these.
   Select @RetCode = @ExecRC
   Execute sp_fallback_help
   Rollback Transaction
   GOTO LABEL_86_RETURN
   end


COMMIT TRANSACTION


--------  FYI How many remain enrolled?


SELECT
             @CountDbStillEnrolled    = count(*)
      from
             master..spt_fallback_db  db
      where
             db.xserver_name          = @pPrimarySvrName


Raiserror(15373,-1,-1,@pPrimarySvrName,@CountDbWithdrawn
                     ,@CountDbStillEnrolled)


LABEL_86_RETURN:

IF (object_id('tempdb..#dev36') is not null)
            drop table #dev36

Return @RetCode
go


---- Bottom_Fallback ("Failover")


raiserror(15339,-1,-1,'sp_validname')
go
CREATE PROCEDURE sp_validname --1996/03/04 13:02
    @name   varchar(30)
AS
/***********************************************************
      This SP can screen out some bad names for things
like syslogins.name.

      This SP cannot aggressively challenge names
values, due to the lack of a code page for the Japanese
(1995/09/11).   Thus all bytes >0x7F will be treated
as DBCS lead bytes (trail byte values not checked).

      Datalength:    Returns number of 8-bit bytes,
not number of characters.

      Substring:     Parm number 2 (of 3) represents the
Nth character (DBCS and/or SBCS), not the Nth 8-bit
byte.   Parm 3 is the number of characters to return,
not the number of bytes to return.

      MS SQL 6.0 identifiers must not exceed 30
8-bit bytes.
***********************************************************/

Set nocount on

--------  This sproc verifies the name is a legal identifier, DBCS or SBCS.

Declare
    @current               int
   ,@last                  int

Declare
    @curchar               tinyint
   ,@atsign                tinyint
   ,@underscore            tinyint
   ,@poundsign             tinyint
   ,@dollarsign            tinyint
   ,@big_A                 tinyint
   ,@small_a               tinyint
   ,@big_Z                 tinyint
   ,@small_z               tinyint
   ,@digit_0               tinyint
   ,@digit_9               tinyint

Declare
    @alpha_s_1             tinyint
   ,@alpha_s_2             tinyint
   ,@alpha_s_3             tinyint

   ,@alpha_2_start         tinyint
   ,@alpha_2_end           tinyint

   ,@alpha_3_start         tinyint
   ,@alpha_3_end           tinyint
   ,@alpha_4_start         tinyint
   ,@alpha_4_end           tinyint
   ,@alpha_5_start         tinyint
   ,@alpha_5_end           tinyint
   ,@alpha_6_start         tinyint
   ,@alpha_6_end           tinyint
   ,@alpha_7_start         tinyint
   ,@alpha_7_end           tinyint

Declare
    @binname               varbinary(30)
   ,@recognized_cp_yn      char(1)
   ,@charset_name          varchar(30)


----------------------------------------


--------  Ensure that the name specified is non-null.


IF (@name is null)
   begin
   raiserror (15004,-1,-1)
   return (1)
   end

Select  @binname = convert(varbinary,@name)


--------  Assign ANSI char ranges.


Select
    @atsign       = ascii('@')
   ,@underscore   = ascii('_')
   ,@poundsign    = ascii('#')
   ,@dollarsign   = ascii('$')
   ,@big_A        = ascii('A')
   ,@small_a      = ascii('a')
   ,@big_Z        = ascii('Z')
   ,@small_z      = ascii('z')
   ,@digit_0      = ascii('0')
   ,@digit_9      = ascii('9')


--------  Get the Character Set ID (csid).


SELECT
             @charset_name    = Min(cs1.name)
      from
             master..syscharsets    cs1  --static row
            ,master..syscharsets    cs2  --row genned by install
      where
             cs1.id           = cs2.csid
      and
             cs1.type         = 1001
      and    cs2.type         = 2001
      and
             cs2.id           =
                  (SELECT      value
                     from      master..sysconfigures
                     where     config  = 1123  --default sortorder id
                  )



--------  Define character hex ranges ("codepoints")  ---------------


----  SBCS values >=0x80 are just for looks, will be treated as DBCS later.


Select @recognized_cp_yn    = 'N'


IF @charset_name = 'iso_1'
   begin
   Select @recognized_cp_yn = 'Y'

   Select
          @alpha_2_start = 0xC0, @alpha_2_end = 0xD6
         ,@alpha_3_start = 0xD8, @alpha_3_end = 0xF6
         ,@alpha_4_start = 0xF8, @alpha_4_end = 0xFF
         ,@alpha_5_start = 0x41, @alpha_5_end = 0x5A
         ,@alpha_6_start = 0x61, @alpha_6_end = 0x7A
         ,@alpha_7_start = 0x61, @alpha_7_end = 0x7A
         ,@alpha_s_1 = 0x41, @alpha_s_2 = 0x41, @alpha_s_3 = 0x41
   end


IF @charset_name = 'cp850'
   begin
   Select @recognized_cp_yn = 'Y'

   Select
          @alpha_2_start = 0x80, @alpha_2_end = 0x9D
         ,@alpha_3_start = 0xA0, @alpha_3_end = 0xA5
         ,@alpha_4_start = 0xB5, @alpha_4_end = 0xB7
         ,@alpha_5_start = 0xC6, @alpha_5_end = 0xC7
         ,@alpha_6_start = 0xD0, @alpha_6_end = 0xD8
         ,@alpha_7_start = 0xE0, @alpha_7_end = 0xED
         ,@alpha_s_1 = 0xDE, @alpha_s_2 = 0xF4, @alpha_s_3 = 0xF5
   end



IF @charset_name = 'cp437'
   begin
   Select @recognized_cp_yn = 'Y'

   select @alpha_2_start = 0x80, @alpha_2_end = 0x9A,
                   @alpha_3_start = 0xA0, @alpha_3_end = 0xA5,
                   @alpha_4_start = 0xE0, @alpha_4_end = 0xEE,
                   @alpha_5_start = 0x41, @alpha_5_end = 0x5A,
                   @alpha_6_start = 0x61, @alpha_6_end = 0x7A,
                   @alpha_7_start = 0x61, @alpha_7_end = 0x7A,
                   @alpha_s_1 = 0x9E, @alpha_s_2 = 0x41, @alpha_s_3 = 0x41
   end


IF @charset_name = 'cp1250'
   begin
   Select @recognized_cp_yn = 'Y'

   select @alpha_2_start = 0x9C, @alpha_2_end = 0x9F,
                   @alpha_3_start = 0xBE, @alpha_3_end = 0xFF,
                   @alpha_4_start = 0x41, @alpha_4_end = 0x5A,
                   @alpha_5_start = 0x41, @alpha_5_end = 0x5A,
                   @alpha_6_start = 0x61, @alpha_6_end = 0x7A,
                   @alpha_7_start = 0x8C, @alpha_7_end = 0x8F,
                   @alpha_s_1 = 0x41, @alpha_s_2 = 0x41, @alpha_s_3 = 0x41
   end


IF @charset_name = 'cp1251'
   begin
   Select @recognized_cp_yn = 'Y'

   select @alpha_2_start = 0x9C, @alpha_2_end = 0x9F,
                   @alpha_3_start = 0xA1, @alpha_3_end = 0xA3,
                   @alpha_4_start = 0xB2, @alpha_4_end = 0xFF,
                   @alpha_5_start = 0x41, @alpha_5_end = 0x5A,
                   @alpha_6_start = 0x61, @alpha_6_end = 0x7A,
                   @alpha_7_start = 0x8C, @alpha_7_end = 0x90,
                   @alpha_s_1 = 0x41, @alpha_s_2 = 0x41, @alpha_s_3 = 0x41
   end


IF @charset_name = 'cp1253'
   begin
   Select @recognized_cp_yn = 'Y'

   select @alpha_2_start = 0xBE, @alpha_2_end = 0xD1,
                   @alpha_3_start = 0xD3, @alpha_3_end = 0xFE,
                   @alpha_4_start = 0x41, @alpha_4_end = 0x5A,
                   @alpha_5_start = 0x41, @alpha_5_end = 0x5A,
                   @alpha_6_start = 0x61, @alpha_6_end = 0x7A,
                   @alpha_7_start = 0xB8, @alpha_7_end = 0xBA,
                   @alpha_s_1 = 0xA2, @alpha_s_2 = 0xBC, @alpha_s_3 = 0x41
   end


IF @charset_name = 'cp1254'
   begin
   Select @recognized_cp_yn = 'Y'

   select @alpha_2_start = 0xD8, @alpha_2_end = 0xFF,
                   @alpha_3_start = 0x9F, @alpha_3_end = 0x9F,
                   @alpha_4_start = 0x41, @alpha_4_end = 0x5A,
                   @alpha_5_start = 0x41, @alpha_5_end = 0x5A,
                   @alpha_6_start = 0x61, @alpha_6_end = 0x7A,
                   @alpha_7_start = 0xC0, @alpha_7_end = 0xD6,
                   @alpha_s_1 = 0x83, @alpha_s_2 = 0x8A, @alpha_s_3 = 0x9A
   end


IF @charset_name = 'cp1255'
   begin
   Select @recognized_cp_yn = 'Y'

   select @alpha_2_start = 0x41, @alpha_2_end = 0x5A,
                   @alpha_3_start = 0x41, @alpha_3_end = 0x5A,
                   @alpha_4_start = 0x41, @alpha_4_end = 0x5A,
                   @alpha_5_start = 0x41, @alpha_5_end = 0x5A,
                   @alpha_6_start = 0x61, @alpha_6_end = 0x7A,
                   @alpha_7_start = 0xE0, @alpha_7_end = 0xFA,
                   @alpha_s_1 = 0x41, @alpha_s_2 = 0x41, @alpha_s_3 = 0x41
   end


IF @charset_name = 'cp1256'
   begin
   Select @recognized_cp_yn = 'Y'

   select @alpha_2_start = 0x80, @alpha_2_end = 0x9A,
                   @alpha_3_start = 0xA0, @alpha_3_end = 0xA5,
                   @alpha_4_start = 0xE0, @alpha_4_end = 0xEE,
                   @alpha_5_start = 0x41, @alpha_5_end = 0x5A,
                   @alpha_6_start = 0x61, @alpha_6_end = 0x7A,
                   @alpha_7_start = 0x61, @alpha_7_end = 0x7A,
                   @alpha_s_1 = 0x41, @alpha_s_2 = 0x41, @alpha_s_3 = 0x41
   end


IF @charset_name = 'cp1257'
   begin
   Select @recognized_cp_yn = 'Y'

   select @alpha_2_start = 0x9C, @alpha_2_end = 0x9F,
                   @alpha_3_start = 0xC0, @alpha_3_end = 0xFE,
                   @alpha_4_start = 0x41, @alpha_4_end = 0x5A,
                   @alpha_5_start = 0x41, @alpha_5_end = 0x5A,
                   @alpha_6_start = 0x61, @alpha_6_end = 0x7A,
                   @alpha_7_start = 0x8C, @alpha_7_end = 0x8F,
                   @alpha_s_1 = 0x41, @alpha_s_2 = 0x41, @alpha_s_3 = 0x41
   end



--------  If no code page?


IF (@recognized_cp_yn = 'N')  --Allow anything.
   begin

   Select
          @alpha_2_start = 0x00, @alpha_2_end = 0xFF
         ,@alpha_3_start = 0x00, @alpha_3_end = 0xFF
         ,@alpha_4_start = 0x00, @alpha_4_end = 0xFF
         ,@alpha_5_start = 0x00, @alpha_5_end = 0xFF
         ,@alpha_6_start = 0x00, @alpha_6_end = 0xFF
         ,@alpha_7_start = 0x00, @alpha_7_end = 0xFF
         ,@alpha_s_1 = 0x41, @alpha_s_2 = 0x41, @alpha_s_3 = 0x41
   end



----------------------------------------------------------------------
--             Validate the first byte (not first character).
----------------------------------------------------------------------


Select @curchar = convert(tinyint,substring(@binname, 1, 1))


IF (not (   (@curchar between @alpha_2_start and @alpha_2_end)
         or (@curchar between @alpha_3_start and @alpha_3_end)
         or (@curchar between @alpha_4_start and @alpha_4_end)
         or (@curchar between @alpha_5_start and @alpha_5_end)
         or (@curchar between @alpha_6_start and @alpha_6_end)
         or (@curchar between @alpha_7_start and @alpha_7_end)

         or (@curchar >= @big_A    and  @curchar <= @big_Z)
         or (@curchar >= @small_a  and  @curchar <= @small_z)

         or  @curchar  = @alpha_s_1
         or  @curchar  = @alpha_s_2
         or  @curchar  = @alpha_s_3
         or  @curchar  = @poundsign
         or  @curchar  = @underscore

         or  @curchar >= 0x80  --DBCS lead byte a possibility.
        )
   )
   begin
   raiserror(15005,-1,-1,@name)
   return (1)
   end



----------------------------------------------------------------------
--          Now validate remaining bytes (DBCS lead byte sensitive).
----------------------------------------------------------------------


Select
    @current   = 1
   ,@last      = datalength(@name)


-- Now check the rest of the characters.

-------------------------
WHILE (@current <= @last)
   begin

   select @curchar = convert(tinyint,substring(@binname, @current, 1))


   if (    @current > 1
       AND
            (not (  (@curchar between @alpha_2_start and @alpha_2_end)
                 or (@curchar between @alpha_3_start and @alpha_3_end)
                 or (@curchar between @alpha_4_start and @alpha_4_end)
                 or (@curchar between @alpha_5_start and @alpha_5_end)
                 or (@curchar between @alpha_6_start and @alpha_6_end)
                 or (@curchar between @alpha_7_start and @alpha_7_end)

                 or (@curchar >= @big_A    and  @curchar <= @big_Z)
                 or (@curchar >= @small_a  and  @curchar <= @small_z)

                 or  @curchar  = @alpha_s_1
                 or  @curchar  = @alpha_s_2
                 or  @curchar  = @alpha_s_3
                 or  @curchar  = @poundsign
                 or  @curchar  = @underscore

                 or  @curchar >= 0x80  --DBCS

                 or  @curchar  = @atsign
                 or  @curchar  = @dollarsign
                 or (@curchar >= @digit_0  and  @curchar <= @digit_9)
                 )
            )
      )
      begin
      raiserror(15006,-1,-1,@name)
      return (1)
      end


   IF (convert(varbinary(1),@curchar) >= 0x80)  --Admit could be SBCS Greek >=0x80, but...
      Select @current = @current + 2
   ELSE
      Select @current = @current + 1

   end  --loop

Return (0)
go



raiserror(15339,-1,-1,'sp_validlang')
go
create procedure sp_validlang
@name	varchar(30)
as

/* Check to see if this language is in Syslanguages. */
if exists (select * from syslanguages where name = @name)
	or @name = 'us_english'		/* us_english is always valid. */
	return(0)

raiserror(15033,-1,-1,@name)
return (1)
go


raiserror(15339,-1,-1,'sp_chklangparam')
go
create procedure sp_chklangparam
@namelist varchar(251),		/* month or day name list */
@msgparam varchar(30),		/* name of list, for error messages */
@numnames int,			/* correct number of names in the list */
@maxnamelen int			/* maximum length of any name in list */
as

	/*
	** sp_chklangparam is called by sp_addlanguage to check the lists of
	** full month names, short month names, and day names.  It returns a status
	** indicating whether any errors were found:
	**	0 - name list is valid
	**	1 - spaces were found, which are not allowed
	**	2 - not enough names in the list (must be exactly @numnames of them)
	**	3 - too many names in the list (must be exactly @numnames of them)
	**	4 - some name(s) are too long
	**	5 - some name(s) have non-aphabetic characters
	*/

	declare @totallen		int
	declare @mthlen			int
	declare @mthname		varchar(251)
	declare @msg			varchar(255)
	declare @csid			tinyint
	declare @csname			varchar(30)
	declare @alpha_0_start		tinyint
	declare @alpha_0_end		tinyint
	declare @alpha_1_start		tinyint
	declare @alpha_1_end		tinyint
	declare @alpha_2_start		tinyint
	declare @alpha_2_end		tinyint
	declare @alpha_3_start		tinyint
	declare @alpha_3_end		tinyint
	declare @alpha_4_start		tinyint
	declare @alpha_4_end		tinyint
	declare @alpha_5_start		tinyint
	declare @alpha_5_end		tinyint
	declare @alpha_6_start		tinyint
	declare @alpha_6_end		tinyint
	declare @alpha_7_start		tinyint
	declare @alpha_7_end		tinyint
	declare @alpha_s_1		tinyint
	declare @alpha_s_2		tinyint
	declare @alpha_s_3		tinyint
	declare @alpha_s_4		tinyint
	declare @curchar		tinyint
	declare @next			int

	/* Get the Character Set id (csid) */
	select @csid = csid from master..syscharsets
		where id = (select value from master..sysconfigures
			where config = 1123)

	/* Get the Character Set Name (csname) */
	select @csname = name from master..syscharsets
		where id = @csid and type < 2000

	/* Make sure the list has no blanks. */
	if ( charindex( ' ', @namelist ) <> 0 )
	begin
		raiserror(15057,-1,-1,@msgparam)
		return (1)
	end

	if @csid = 1
	    /* Set up the alphabetic codepoint ranges for iso_1. */
	    select @alpha_0_start = 0x41, @alpha_0_end = 0x5A,
		   @alpha_1_start = 0x61, @alpha_1_end = 0x7A,
		   @alpha_2_start = 0xC0, @alpha_2_end = 0xD6,
		   @alpha_3_start = 0xD8, @alpha_3_end = 0xF6,
	           @alpha_4_start = 0xF8, @alpha_4_end = 0xFF,
	           @alpha_5_start = 0x41, @alpha_5_end = 0x5A,
	           @alpha_6_start = 0x61, @alpha_6_end = 0x7A,
	           @alpha_7_start = 0x61, @alpha_7_end = 0x7A,
		   @alpha_s_1 = 0x41, @alpha_s_2 = 0x41, @alpha_s_3 = 0x41,
		   @alpha_s_4 = 0x2D

	if @csid = 2
	    /* Set up the alphabetic codepoint ranges for cp850. */
	    select @alpha_0_start = 0x41, @alpha_0_end = 0x5A,
		   @alpha_1_start = 0x61, @alpha_1_end = 0x7A,
		   @alpha_2_start = 0x80, @alpha_2_end = 0x9D,
		   @alpha_3_start = 0xA0, @alpha_3_end = 0xA5,
	           @alpha_4_start = 0xB5, @alpha_4_end = 0xB7,
	           @alpha_5_start = 0xC6, @alpha_5_end = 0xC7,
	           @alpha_6_start = 0xD0, @alpha_6_end = 0xD8,
	           @alpha_7_start = 0xE0, @alpha_7_end = 0xED,
		   @alpha_s_1 = 0xDE, @alpha_s_2 = 0xF4, @alpha_s_3 = 0xF5,
		   @alpha_s_4 = 0x2D

	if @csid = 3
	    /* Set up the alphabetic codepoint ranges for cp437. */
	    select @alpha_0_start = 0x41, @alpha_0_end = 0x5A,
		   @alpha_1_start = 0x61, @alpha_1_end = 0x7A,
		   @alpha_2_start = 0x80, @alpha_2_end = 0x9A,
		   @alpha_3_start = 0xA0, @alpha_3_end = 0xA5,
	           @alpha_4_start = 0xE0, @alpha_4_end = 0xEE,
	           @alpha_5_start = 0x41, @alpha_5_end = 0x5A,
	           @alpha_6_start = 0x61, @alpha_6_end = 0x7A,
	           @alpha_7_start = 0x61, @alpha_7_end = 0x7A,
		   @alpha_s_1 = 0x9E, @alpha_s_2 = 0x41, @alpha_s_3 = 0x41,
		   @alpha_s_4 = 0x2D

	if @csid = 4
	    /* Set up the alphabetic codepoint ranges for cp1250. */
	    select @alpha_0_start = 0x41, @alpha_0_end = 0x5A,
		   @alpha_1_start = 0x61, @alpha_1_end = 0x7A,
	       @alpha_2_start = 0x9C, @alpha_2_end = 0x9F,
		   @alpha_3_start = 0xBE, @alpha_3_end = 0xFF,
	           @alpha_4_start = 0x41, @alpha_4_end = 0x5A,
	           @alpha_5_start = 0x41, @alpha_5_end = 0x5A,
	           @alpha_6_start = 0x61, @alpha_6_end = 0x7A,
	           @alpha_7_start = 0x8C, @alpha_7_end = 0x8F,
		   @alpha_s_1 = 0x41, @alpha_s_2 = 0x41, @alpha_s_3 = 0x41,
		   @alpha_s_4 = 0x41

	if @csid = 5
	    /* Set up the alphabetic codepoint ranges for cp1251. */
	    select @alpha_0_start = 0x41, @alpha_0_end = 0x5A,
		   @alpha_1_start = 0x61, @alpha_1_end = 0x7A,
	       @alpha_2_start = 0x9C, @alpha_2_end = 0x9F,
		   @alpha_3_start = 0xA1, @alpha_3_end = 0xA3,
	           @alpha_4_start = 0xB2, @alpha_4_end = 0xFF,
	           @alpha_5_start = 0x41, @alpha_5_end = 0x5A,
	           @alpha_6_start = 0x61, @alpha_6_end = 0x7A,
	           @alpha_7_start = 0x8C, @alpha_7_end = 0x90,
		   @alpha_s_1 = 0x41, @alpha_s_2 = 0x41, @alpha_s_3 = 0x41,
		   @alpha_s_4 = 0x41

	if @csid = 6
	    /* Set up the alphabetic codepoint ranges for cp1253. */
	    select @alpha_0_start = 0x41, @alpha_0_end = 0x5A,
		   @alpha_1_start = 0x61, @alpha_1_end = 0x7A,
           @alpha_2_start = 0xBE, @alpha_2_end = 0xD1,
		   @alpha_3_start = 0xD3, @alpha_3_end = 0xFE,
	           @alpha_4_start = 0x41, @alpha_4_end = 0x5A,
	           @alpha_5_start = 0x41, @alpha_5_end = 0x5A,
	           @alpha_6_start = 0x61, @alpha_6_end = 0x7A,
	           @alpha_7_start = 0xB8, @alpha_7_end = 0xBA,
		   @alpha_s_1 = 0xA2, @alpha_s_2 = 0xBC, @alpha_s_3 = 0x41,
		   @alpha_s_4 = 0x41

	if @csid = 7
	    /* Set up the alphabetic codepoint ranges for cp1254. */
	    select @alpha_0_start = 0x41, @alpha_0_end = 0x5A,
		   @alpha_1_start = 0x61, @alpha_1_end = 0x7A,
	       @alpha_2_start = 0xD8, @alpha_2_end = 0xFF,
		   @alpha_3_start = 0x9F, @alpha_3_end = 0x9F,
	           @alpha_4_start = 0x41, @alpha_4_end = 0x5A,
	           @alpha_5_start = 0x41, @alpha_5_end = 0x5A,
	           @alpha_6_start = 0x61, @alpha_6_end = 0x7A,
	           @alpha_7_start = 0xC0, @alpha_7_end = 0xD6,
		   @alpha_s_1 = 0x83, @alpha_s_2 = 0x8A, @alpha_s_3 = 0x9A,
		   @alpha_s_4 = 0x41

	if @csid = 8
	    /* Set up the alphabetic codepoint ranges for cp1255. */
	    select @alpha_0_start = 0x41, @alpha_0_end = 0x5A,
		   @alpha_1_start = 0x61, @alpha_1_end = 0x7A,
           @alpha_2_start = 0x41, @alpha_2_end = 0x5A,
		   @alpha_3_start = 0x41, @alpha_3_end = 0x5A,
	           @alpha_4_start = 0x41, @alpha_4_end = 0x5A,
	           @alpha_5_start = 0x41, @alpha_5_end = 0x5A,
	           @alpha_6_start = 0x61, @alpha_6_end = 0x7A,
	           @alpha_7_start = 0xE0, @alpha_7_end = 0xFA,
		   @alpha_s_1 = 0x41, @alpha_s_2 = 0x41, @alpha_s_3 = 0x41,
		   @alpha_s_4 = 0x41

	if @csid = 9
	    /* Set up the alphabetic codepoint ranges for cp1256. */
	    select @alpha_0_start = 0x41, @alpha_0_end = 0x5A,
		   @alpha_1_start = 0x61, @alpha_1_end = 0x7A,
	      @alpha_2_start = 0x80, @alpha_2_end = 0x9A,
		   @alpha_3_start = 0xA0, @alpha_3_end = 0xA5,
	           @alpha_4_start = 0xE0, @alpha_4_end = 0xEE,
	           @alpha_5_start = 0x41, @alpha_5_end = 0x5A,
	           @alpha_6_start = 0x61, @alpha_6_end = 0x7A,
	           @alpha_7_start = 0x61, @alpha_7_end = 0x7A,
		   @alpha_s_1 = 0x41, @alpha_s_2 = 0x41, @alpha_s_3 = 0x41,
		   @alpha_s_4 = 0x41

	if @csid = 10
	    /* Set up the alphabetic codepoint ranges for cp1257. */
	    select @alpha_0_start = 0x41, @alpha_0_end = 0x5A,
		   @alpha_1_start = 0x61, @alpha_1_end = 0x7A,
	       @alpha_2_start = 0x9C, @alpha_2_end = 0x9F,
		   @alpha_3_start = 0xC0, @alpha_3_end = 0xFE,
	           @alpha_4_start = 0x41, @alpha_4_end = 0x5A,
	           @alpha_5_start = 0x41, @alpha_5_end = 0x5A,
	           @alpha_6_start = 0x61, @alpha_6_end = 0x7A,
	           @alpha_7_start = 0x8C, @alpha_7_end = 0x8F,
		   @alpha_s_1 = 0x41, @alpha_s_2 = 0x41, @alpha_s_3 = 0x41,
		   @alpha_s_4 = 0x41

	select @totallen = datalength( @namelist )
	while ( @totallen > 0 )
	begin
		/* Move to the next name in the list. */
		select @namelist = right( @namelist, @totallen )

		/* If this is not the last name in the list then there must
		** be another comma.
		*/
		if ( @numnames > 1 )
		begin
			select @mthlen = charindex( ',', @namelist ) - 1
			if ( @mthlen < 0 )
			begin
				raiserror(15058,-1,-1,@msgparam)
				return(2)
			end
		end

		/* This is the last name in the list, so there cannot be
		** another comma.  This name is all remaining characters
		** in the list.
		*/
		else
		begin
			if ( charindex( ',', @namelist ) <> 0 )
			begin
				raiserror(15059,-1,-1,@msgparam)
				return(3)
			end
			select @mthlen = @totallen
		end

		/* Check the name length. */
		if ( @mthlen > @maxnamelen )
		begin
			raiserror(15283,-1,-1,@msgparam)
			return(4)
		end

		/*
		** Check to see that the names contain only true
		** Alphabetic characters; not even DIGITs are allowed.
		*/
		select @next = 1
		while @next <= @mthlen
		begin
		    select @curchar = convert(binary(1),
			substring(@namelist, @next, 1))
		    if (not
		    (
		       (@curchar between @alpha_0_start and @alpha_0_end)
		    or (@curchar between @alpha_1_start and @alpha_1_end)
		    or (@curchar between @alpha_2_start and @alpha_2_end)
		    or (@curchar between @alpha_3_start and @alpha_3_end)
		    or (@curchar between @alpha_4_start and @alpha_4_end)
		    or (@curchar between @alpha_5_start and @alpha_5_end)
		    or (@curchar between @alpha_6_start and @alpha_6_end)
		    or (@curchar between @alpha_7_start and @alpha_7_end)
		    or @curchar = @alpha_s_1
		    or @curchar = @alpha_s_2
		    or @curchar = @alpha_s_3
		    or @curchar = @alpha_s_4
		    )
		    )
		    begin
    			raiserror(15060,-1,-1,@msgparam,@csname)
			return(5)
		    end

		    select @next = @next + 1
		end

		/* Move to the next name in the list. */
		select @numnames = @numnames - 1
		select @totallen = @totallen - @mthlen - 1
	end

	/* No problems were found with the name list. */
	return (0)
go



raiserror(15339,-1,-1,'sp_namecrack')
go
create procedure sp_namecrack
@name	varchar(123),
@site	varchar(30) = null output,
@db	varchar(30) = null output,
@owner	varchar(30) = null output,
@object	varchar(30) = null output
as
--Used in InstRepl.SQL

select @site = null, @db = null, @owner = null, @object = null

/*
**  Is there a site name?
*/
if @name like '%.%.%.%'
begin
	select @site = substring(@name, 1, charindex('.', @name) - 1)
	select @name = substring(@name, charindex('.', @name) + 1, 125)
end

/*
**  Is there a database name?
*/
if @name like '%.%.%'
begin
	select @db = substring(@name, 1, charindex('.', @name) - 1)
	select @name = substring(@name, charindex('.', @name) + 1, 125)
end

/*
**  Is there a owner?
*/
if @name like '%.%'
begin
	select @owner = substring(@name, 1, charindex('.', @name) - 1)
	select @name = substring(@name, charindex('.', @name) + 1, 125)
end

select @object = @name

return (0)
go



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



raiserror(15339,-1,-1,'sp_addalias')
go
create procedure sp_addalias
@loginame varchar(30),		/* the name of the pretender */
@name_in_db varchar(30)		/* who the pretender wants to pretend to be */
as

declare @suid	int		/* the suid of the pretender */
declare @asuid	int		/* the suid of the person to impersonate */
declare	@name	varchar(30)
declare @dbname sysname

/*
**  You must be SA or the dbo to execute this sproc.
*/
if suser_id() <> 1 and user_id() <> 1
	begin
		raiserror(15000,-1,-1)
		return (1)
	end

/*
**  Make sure that the pretender has an account.
*/
select @suid = suid from syslogins where name = @loginame

if @suid is NULL
	begin
		raiserror(15007,-1,-1,@loginame)
		return (1)
	end

/*
**  Get the suid of the person we want to pretend to be.
**  All user ids are <= 16383 and group ids are > 16383.
*/
select @asuid = suid from sysusers where name = @name_in_db and uid <= 16383

/*
**  Does the user to be impersonated exist in the current database?
*/
if @asuid is NULL
	begin
		raiserror(15008,-1,-1,@name_in_db)
		return (1)
	end

/*
**  Is the login to be aliased already a user in the current db?
*/
if exists (select * from sysusers where suid = @suid)
	begin
		select @name = name,@dbname = db_name() from sysusers
			where suid = @suid
		raiserror(15278,-1,-1,@loginame,@name,@dbname)
		return (1)
	end

/*
**  Is the person already aliased to a user?
*/
if exists (select * from sysalternates where suid = @suid)
	begin
		raiserror(15022,-1,-1)
		return (1)
	end

/*
**  Add the alias.
*/
insert into sysalternates (suid, altsuid)
	values (@suid, @asuid)

raiserror(15340,-1,-1)

return (0)
go


raiserror(15339,-1,-1,'sp_addextendedproc')
go
create procedure sp_addextendedproc
@functname varchar(62),		/* (owner.)name of function to call */
@dllname varchar(255)		/* name of DLL containing function */
as
/*
**  If we're in a transaction, disallow the addition of the
**  extended stored procedure.
*/
if @@trancount > 0
begin
	raiserror(15002,-1,-1,'sp_addextendedproc')
	return (1)
end

/*
** Create the extended procedure mapping.
*/
dbcc addextendedproc( @functname, @dllname)
return (0)
go


raiserror(15339,-1,-1,'sp_addgroup')
go
create procedure sp_addgroup
@grpname varchar(30)			/* new group name */
as

declare @gid int			/* group id */

/*
**  You must be SA or the dbo to execute this sproc.
*/
if suser_id() <> 1 and user_id() <> 1
	begin
		raiserror(15000,-1,-1)
		return (1)
	end

/*
**  Check to see that the @grpname is valid.
*/
declare @returncode	int
execute @returncode = sp_validname @grpname
if @returncode <> 0
	return(1)

/*
**  See if the new group name is already being used as a user or group name.
*/
select @gid = uid from sysusers where name = @grpname

/*
**  User ids are > 0 and  <= 16383 and group ids are > 16383.
*/
if @gid is not null
begin
	if @gid > 0 and @gid <= 16383
		raiserror(15023,-1,-1,@grpname)
	else
		raiserror(15024,-1,-1,@grpname)

	return (1)
end

/*
**  Now get the group id for the new group.  Look for the first available
**  hole > 16383.
*/
select @gid = min(s1.uid)+1 from sysusers s1
	where uid < 32767	/* Don't cause arithmetic overflow. */
	and uid >= 16383
	and not exists
		(select * from sysusers s2
			where s2.uid = s1.uid+1)

/*
**  This is the first group.
*/
if @gid is null
	select @gid = 16384

/*
**  Create the group.
*/
insert into sysusers (uid, suid, gid, name, environ)
values (@gid, -@gid, @gid, @grpname, '')

raiserror(15341,-1,-1)

return (0)
go



raiserror(15339,-1,-1,'sp_addlogin')
go
CREATE PROCEDURE sp_addlogin  -- 1996/03/06 14:04

    @loginame         varchar(30)
   ,@passwd          varchar(30) = Null
   ,@defdb         varchar(30) = 'master'
   ,@deflanguage   varchar(30) = Null
   ,@loginsuid         smallint    = Null
AS

Set nocount on

Declare
    @RetCode               int
   ,@IntA                  int

   ,@High2Int              smallint
   ,@NewSUID               smallint
   ,@LowRegSUID            smallint

   ,@charLowRegSUID        varchar(12)
   ,@WorkName              varchar(30)


--------  Initial values.

Select
    @RetCode               = 1   -- 0=Good ,1=Bad.
   ,@NewSUID               = Null
   ,@High2Int              = 32767
   ,@LowRegSUID            = 10

Select
    @charLowRegSUID        = convert(varchar,@LowRegSUID)


----------------------------------------------------------------------
----------------------------------------------------------------------
--*         Simple validations of input parms.
----------------------------------------------------------------------
----------------------------------------------------------------------


--------  An open txn risks improper recovery.

IF (@@trancount > 0)
   begin
   RaisError(15002,-1,-1,'sp_addlogin')
   GOTO LABEL_86RETURN
   end


--------  Only SA can run this.

IF (suser_id() <> 1)
   begin
   RaisError(15003,-1,-1)
   GOTO LABEL_86RETURN
   end


--------  Verify login name is valid.

Execute @IntA = sp_validname @loginame

IF (@IntA <> 0)
   begin
   --msg handle by called sproc.
   GOTO LABEL_86RETURN
   end


--------  Validate default language.

IF (@deflanguage IS NOT Null)
   begin
   Execute @IntA = sp_validlang @deflanguage

   IF (@IntA <> 0)
      begin
      --msg handle by called sproc.
      GOTO LABEL_86RETURN
      end
   end

--------  Verify login name is new.

IF (exists (select * from syslogins where name = @loginame))
   begin
   RaisError(15025,-1,-1,@loginame)
   GOTO LABEL_86RETURN
   end


--------  Verify the default database name is known.

IF NOT (exists (select * from sysdatabases where name = @defdb))
   begin
   RaisError(15010,-1,-1,@defdb)
   GOTO LABEL_86RETURN
   end


----------------------------------------------------------------------
----------------------------------------------------------------------
--*         Parms are valid, try to add login.
----------------------------------------------------------------------
----------------------------------------------------------------------


--------  Did the executioner supply an suid value?  -----------

IF (@loginsuid IS NOT Null)
   begin

   IF (@loginsuid <= 1)
      begin
      RaisError(15301,-1,-1 ,@loginsuid)
      GOTO LABEL_86RETURN
      end


   Select    @IntA      = min(suid)
            ,@WorkName  = min(name)
      from   syslogins
      where  suid       = @loginsuid

   IF (@IntA IS NOT Null)
      begin
      RaisError(15296,-1,-1 ,@loginsuid ,@WorkName)
      GOTO LABEL_86RETURN
      end


   Select @NewSUID = @loginsuid

   GOTO LABEL_07INSERT

   end


--------  Try the faster way of generating a new suid.  --------------

-- 16382=repl_publisher  ,16383=repl_subscriber


SELECT
             @IntA      = Max(suid)
      from
             syslogins
      where
      suid       < 16299  --avoids repl_%


IF     EXISTS (
               SELECT   *
                 from   syslogins
                 where  suid=(@IntA+1)
              )
   Select @IntA = @High2Int  --Acts as a defeating switch.


IF (@IntA < @High2Int)  --True almost always.
   begin

   Select @NewSUID = @IntA + 1

   IF (      @NewSUID < @LowRegSUID)
      Select @NewSUID = @LowRegSUID

   GOTO LABEL_07INSERT

   end


--------  Instead, scan for an available new suid number.  ------------

SELECT     --Find lowest in-use suid where the next number is available.
                @NewSUID = min(log1.suid)
      from
                syslogins  log1
      where
                log1.suid  < @High2Int  --Avoid arithmetic overflow.
      and       log1.suid >= @LowRegSUID
      and
                NOT exists    --Faster than plain cursor loop.
                  (
                   SELECT  log2.*
                     from  syslogins   log2
                     where log2.suid   = log1.suid + 1
                  )



IF (@NewSUID IS Null)
   begin
   RaisError(15297,-1,-1 ,@loginame)
   GOTO LABEL_86RETURN
   end
ELSE
   begin
   Select   @NewSUID = @NewSUID + 1

   GOTO LABEL_07INSERT

   end


----------------------------------------------------------------------
--*            We have a good new suid number.
----------------------------------------------------------------------

--------  Yes, a good suid number is in hand, add login.


   --Logic can never hit this raiserror/goto, precautionary.
RaisError('Bad logic in sp_addlogin, should never hit this statement (a1).' ,16,127)
GOTO LABEL_86RETURN



LABEL_07INSERT:   --@NewSUID must be set properly by now.



INSERT
         into
                syslogins
            (
             suid
            ,status
            ,accdate

            ,totcpu ,totio ,spacelimit ,timelimit ,resultlimit

            ,dbname
            ,name
            ,password
            ,language
            )
         Values
            (
             @NewSUID
            ,8          -- 0x08 bit means pw encrypt new alogorithm
            ,getdate()

            ,0 ,0 ,0 ,0 ,0

            ,@defdb
            ,@loginame
            ,pwdencrypt(@passwd)
            ,@deflanguage
            )


IF (@@error = 0)
   begin
   Raiserror(15298,-1,-1)
   Select @RetCode   = 0
   end


----------------------------------------------------------------------
--*               Finalization.
----------------------------------------------------------------------


LABEL_86RETURN:


Return @RetCode
go



raiserror(15339,-1,-1,'sp_change_users_login')
go
CREATE PROCEDURE sp_change_users_login -- 1995/12/22 13:52

    @Action               varchar(10)  = Null

   ,@UserNamePattern      varchar(40)  = Null
   ,@LoginName            varchar(30)  = Null
AS

Set nocount on

Declare
    @RetCode               integer
   ,@exspRetCode           integer
   ,@FixMode               character(5)
   ,@IntA                  integer
   ,@IntB                  integer
   ,@CountFixesByUpd       integer
   ,@CountFixesByAddLogin  integer

   ,@DBName                varchar(30)

Declare
    @110Name               varchar(30)
   ,@110UID                smallint
   ,@110SUID               smallint
   ,@LoginSUID             smallint


---------------  Initial values.


Select
    @DBName                = db_name()
   ,@CountFixesByUpd       = 0
   ,@CountFixesByAddLogin  = 0
   ,@RetCode               = 0  -- 0=Good ,1=Bad.


---------------  Validate the action  -----------------------


IF (          @Action            IS     Null
    OR
        (     @Action            IS NOT Null
         AND  Upper(@Action) NOT IN ('REPORT','UPDATE_ONE','AUTO_FIX')
        )
   )
   begin
   RaisError(15286,-1,-1,@Action)
   Select @RetCode = 1
   GOTO LABEL_86RETURN
   end

Select @Action = Upper(@Action)


---------------  Exclude certain special logins  ------------------


IF (     lower(@LoginName) IN
      ('sa' ,'probe' ,'repl_publisher' ,'repl_subscriber')
    AND        @LoginName  IS NOT Null
   )
   begin
   RaisError(15287,-1,-1,@LoginName)
   Select @RetCode = 1
   GOTO LABEL_86RETURN
   end


---------------  Only the SA can run certain actions  -----------------


IF (     suser_id() <> 1
    AND  @Action   IN ('AUTO_FIX')
   )
   begin
   RaisError(15288,-1,-1,'SA',@Action)
   Select @RetCode = 1
   GOTO LABEL_86RETURN
   end


---------------  Only SA or DBO can run certain actions  -------------


IF (     (suser_id() <> 1  AND  user_id() <> 1)
    AND  @Action   IN ('UPDATE_ONE')
   )
   begin
   RaisError(15288,-1,-1,'SA or DBO',@Action)
   Select @RetCode = 1
   GOTO LABEL_86RETURN
   end


-----------  Pre-existing open txn not allowed (recovery problemmatic) ----------


IF (@@trancount > 0)
   begin
   RaisError(15289,-1,-1)
   Select @RetCode = 1
   GOTO LABEL_86RETURN
   end


---------------- Validate parm value combinations  -----------------


IF (@Action   = 'REPORT')
   begin

   IF (     @UserNamePattern IS NOT Null
       OR   @LoginName       IS NOT Null
      )
      begin
      RaisError(15290,-1,-1,@Action,@UserNamePattern,@LoginName)
      Select @RetCode = 1
      GOTO LABEL_86RETURN
      end
   end


IF (@Action   = 'UPDATE_ONE')
   begin

   IF (     @UserNamePattern IS     Null
       OR   @LoginName       IS     Null
      )
      begin
      RaisError(15290,-1,-1,@Action,@UserNamePattern,@LoginName)
      Select @RetCode = 1
      GOTO LABEL_86RETURN
      end
   end


IF (@Action   = 'AUTO_FIX')
   begin

   IF (     @UserNamePattern IS     Null
       OR   @LoginName       IS NOT Null
      )
      begin
      RaisError(15290,-1,-1,@Action,@UserNamePattern,@LoginName)
      Select @RetCode = 1
      GOTO LABEL_86RETURN
      end
   end



----------------------------------------------------------------------
----------------------------------------------------------------------
--                      ACTIONS
----------------------------------------------------------------------
----------------------------------------------------------------------


IF (@Action   =    'UPDATE_ONE')
   GOTO LABEL_ACTION_UPDATE_ONE

IF (@Action   =    'AUTO_FIX')
   GOTO LABEL_ACTION_AUTO_FIX

IF (@Action   =    'REPORT')
   GOTO LABEL_ACTION_REPORT


----------------------------------------------------------------------
--                      Action = REPORT
----------------------------------------------------------------------


LABEL_ACTION_REPORT:



SELECT
             UserName   = u1.name
            ,UserSUID   = u1.suid
      from
             sysusers   u1
      where
             u1.suid    > 0
      and
             NOT EXISTS
               (
                SELECT     l1.*
                  from     master.dbo.syslogins   l1
                  where    l1.suid     = u1.suid
               )
      order by
             u1.name



GOTO LABEL_77FINALIZATION



----------------------------------------------------------------------
--                      Action = UPDATE_ONE
----------------------------------------------------------------------


LABEL_ACTION_UPDATE_ONE:



IF (NOT EXISTS (SELECT * from sysusers where name = @UserNamePattern))
   begin
   RaisError(15291,-1,-1,'User',@UserNamePattern)
   Select @RetCode = 1
   GOTO LABEL_86RETURN
   end


SELECT
             @LoginSUID    = Min(suid)
      from
             master.dbo.syslogins
      where
             name          = @LoginName
      and    name     NOT IN
                        ('sa' ,'probe'
                        ,'repl_publisher' ,'repl_subscriber'
                        )
      and    suid          > 0


IF (@LoginSUID IS Null)
   begin
   RaisError(15291,-1,-1,'Login',@LoginName)
   Select @RetCode = 1
   GOTO LABEL_86RETURN
   end


UPDATE
             sysusers
      set
             suid    = @LoginSUID
      where
             name    = @UserNamePattern
      and    uid     > 0
      and    suid    > 0
      and    suid   <> @LoginSUID



IF (@@error = 0  AND  @@rowcount = 1)
   Select @CountFixesByUpd = 1

Raiserror(15295,-1,-1,@CountFixesByUpd)


GOTO LABEL_77FINALIZATION



----------------------------------------------------------------------
--                      Action = AUTO_FIX
----------------------------------------------------------------------


LABEL_ACTION_AUTO_FIX:



---------------  Cursor 110  -----------------


EXECUTE(
'
DECLARE Cursor110_Users Cursor
   -- Insensitive Scroll
   For
SELECT
             u1.name
            ,u1.uid
            ,u1.suid
      from
             sysusers               u1
      where
             name    LIKE ''' + @UserNamePattern + '''
      and
             suid     > 0  --Bypass Groups, Public, Guest etc.
      and
             name  NOT IN
                     (''sa'' ,''probe''
                     ,''repl_publisher'' ,''repl_subscriber''
                     )
      and
             NOT Exists
               (
                SELECT     *
                  from     master.dbo.syslogins   l1
                  where    l1.suid  = u1.suid
               )
'
)


OPEN Cursor110_Users



---------------
WHILE (110=110)         ---------  LOOP 110: thru Orphan Users  -----------
   begin


   FETCH
                Next
         from
                Cursor110_Users
         into
                @110Name
               ,@110UID
               ,@110SUID


   IF (@@fetch_status <> 0)
      begin
      DEALLOCATE Cursor110_Users
      BREAK  --loop 110
      end


-----------  Handle case where Name already in use.

   SELECT
                @LoginSUID    = Min(suid)
         from
                master.dbo.syslogins   l1
         where
                l1.name    = @110Name



   IF (@LoginSUID IS Null)
      begin

      ---- Simple sp_addlogin, to get a common L/U name.

      SELECT
                @IntA = isnull( (max(suid) + 1) ,11)
         from
                sysusers
         where
                suid between 2 and 15999

      SELECT
                @IntB = max(suid) + 1
         from
                syslogins
         where
                suid between 2 and 15999
         and    suid >= @IntA

      IF (@IntB IS Null)
         Select @IntB = @IntA


      Execute @exspRetCode = sp_addlogin
                                 @110Name ,Null ,@DBName ,default ,@IntB


      SELECT
                   @LoginSUID = min(suid)
            from
                   master.dbo.syslogins
            where
                   name=@110Name


      IF @LoginSUID IS Null
         begin
         RaisError('Very odd failure of sp_addlogin (user=%s), aborting.',16,1,@110Name)
         Deallocate Cursor110_Users
         Select @RetCode = 1
         GOTO LABEL_86RETURN
         end


      Select @FixMode = '1AddL'
      Raiserror(15293,-1,-1,@110Name)
      end
   ELSE
      begin
      Select @FixMode = '2UpdU'
      Raiserror(15292,-1,-1,@110Name)
      end



   IF EXISTS (SELECT * from sysusers where suid=@LoginSUID)
      begin
      RaisError(15331,-1,-1,@110Name,@LoginSUID)
      Select @RetCode = 1
      -- GOTO LABEL_86RETURN

      CONTINUE  -- loop 110
      end



-------------  Upd Users & Alternates.


   BEGIN TRANSACTION txn_1a_sp_change_users_login


   UPDATE
                sysusers
         set
                suid       = @LoginSUID
         where
                name       = @110Name

   IF @@error <> 0
      begin
      RaisError('Inside txn_1a_, Upd failed, will Rollback (1a1)!',17,127)
      ROLLBACK TRANSACTION --Name not specified.
      Select @RetCode = 1
      GOTO LABEL_86RETURN
      end


   UPDATE
                sysalternates
         set
                suid       = @LoginSUID
         where
                suid       = @110SUID

   IF @@error <> 0
      begin
      RaisError('Inside txn_1a_, Upd failed, will Rollback (1a2)!',17,127)
      ROLLBACK TRANSACTION --Name not specified.
      Select @RetCode = 1
      GOTO LABEL_86RETURN
      end


   COMMIT TRANSACTION txn_1a_sp_change_users_login



   IF (@FixMode = '1AddL')
      begin
      Select @CountFixesByAddLogin = @CountFixesByAddLogin + 1
      end
   IF (@FixMode = '2UpdU')
      begin
      Select @CountFixesByUpd = @CountFixesByUpd + 1
      end



   end  --loop 110



Raiserror(15295,-1,-1,@CountFixesByUpd)
Raiserror(15294,-1,-1,@CountFixesByAddLogin)


GOTO LABEL_77FINALIZATION



------------------  Finalization  ------------------


LABEL_77FINALIZATION:


LABEL_86RETURN:


Return @RetCode
go



raiserror(15339,-1,-1,'sp_addmessage')
go
create procedure sp_addmessage
@msgnum int = null,		-- Number of new message.
@severity smallint = null,	-- Severity of new message.
@msgtext varchar(255) = null,	-- Text of new message.
@lang varchar(30) = null,       -- language (name) of new message
@with_log varchar(5) = 'FALSE', -- Whether the message will ALWAYS go to the NT event log
@replace varchar(7) = null	-- Optional parameter to specify that
				-- existing message with same number
				-- should be overwritten.
as
declare @retcode int
declare @langid smallint
declare @dlevel smallint

if @msgnum is null or @severity is null or @msgtext is null
	begin
		raiserror(15071,-1,-1)
		return(1)
	end

/*
** User defined messages must be > 50000.
*/
if @msgnum <= 50000
	begin
		raiserror(15040,-1,-1)
		return(1)
	end

/*
** Valid severity range for user defined messges is 1 to 25.
*/
if @severity not between 1 and 25
	begin
		raiserror(15041,-1,-1)
		return(1)
	end

/*
** Only the SA may add messages with severity > 18 or which set the WITH_LOG option to 'true'
*/
if suser_id() <> 1 and ((@severity > 18) or (rtrim(upper(@with_log)) = 'TRUE'))
	begin
		raiserror(15042,-1,-1)
		return (1)
	end

/*
** Verify the language
*/
if @lang is not null
	begin
		exec @retcode = sp_validlang @lang
		if @retcode <>  0
			return(1)
	end
else
	select @lang = @@language

/*
** Get langid from syslanguages; us_english won't exist, so use 0.
*/
select @langid = isnull((select langid from syslanguages where name = @lang),0)

/*
** @with_log must be 'TRUE' or 'FALSE'
*/
if (upper(@with_log) not in ('TRUE', 'FALSE'))
	begin
		raiserror(15271,-1,-1)
		return (1)
	end

/*
** Set the dlevel bit accordingly
*/
if (rtrim(upper(@with_log)) = 'TRUE')
        select @dlevel = 0x80
else
        select @dlevel = 0x0

/*
** If we're adding a non-us_english message, make sure the us_english version already exists.
*/
if (@langid not in (0, null)) and not exists (select * from sysmessages where error=@msgnum and langid in (0, null))
	begin
		raiserror(15279,-1,-1,@lang)
		return(1)
	end

/*
** If we're adding a non-us_english message, make sure that the severity matches that of the us_english version 
*/
if (@langid not in (0, null)) and not exists (select * from sysmessages where error=@msgnum and severity=@severity and langid in (0, null))
	begin
		declare @us_english_severity smallint
		select @us_english_severity = severity from sysmessages where error=@msgnum and langid in (0, null)
		raiserror(15304,-1,-1,@lang,@us_english_severity)
		return(1)
	end

/*
**  Does this message already exist, and if so are we REPLACEing it?
*/
if (select count(*) from sysmessages where error=@msgnum and langid=@langid) > 0
	if lower(@replace) = 'replace'
		begin
			raiserror(15342,-1,-1)
			delete from sysmessages where error = @msgnum and langid = @langid
			/*
			** If we're REPLACEing a us_english message, make sure any non-us_english messages get updated with the same severity
			*/
			if (@langid in (0, null))
				begin
					update sysmessages set severity = @severity
					where error = @msgnum and langid not in (0, null)
				end
		end
	else
		begin
			/*
			** The 'replace' option wasn't specified and a
			** msg. with the number already exists.
			*/
			raiserror(15043,-1,-1)
			return(1)
		end

/*
**  Create the message.
*/
insert into sysmessages(error,severity,description,dlevel,langid)
	values (@msgnum,@severity,@msgtext,@dlevel,@langid)

if (@@error = 0)
   raiserror(15343,-1,-1)

return (0)
go



raiserror(15339,-1,-1,'sp_addumpdevice')
go
create procedure sp_addumpdevice -- 1995/09/07 12:01
@devtype varchar(10),      /* disk, tape, or diskette */
@logicalname   varchar(30),      /* logical name of the device */
@physicalname  varchar(127),     /* physical name of the device */
@cntrltype  smallint = null,  /* controller type - ignored. */
@devstatus  varchar(40) = 'noskip'  /* device characteristics */
as

declare @status smallint      /* status bits for device */
declare @returncode int

/*
**  An open txn might jeopardize a recovery.
*/
if @@trancount > 0
   begin
      raiserror(15002,-1,-1,'sp_addumpdevice')
      return (1)
   end

/*
**  You must be SA to execute this sproc.
*/
if suser_id() <> 1
   begin
      raiserror(15003,-1,-1)
      return (1)
   end

/*
**  Check to see that the @logicalname is valid.
*/
exec @returncode = sp_validname @logicalname
if @returncode <> 0
   return(1)

select @devtype=lower(@devtype)

/*
**  Check out the @devtype.
*/
if @devtype not in ('disk', 'tape', 'diskette', 'pipe')
   begin
      raiserror(15044,-1,-1,@devtype)
      return (1)
   end

/*
**  Check the args are not NULL.
*/
if @logicalname is null
   begin
      raiserror(15045,-1,-1)
      return(1)
   end

if @physicalname is null
   begin
      raiserror(15046,-1,-1)
      return(1)
   end

/*
**  Make sure physical file name would be unique among devices.
*/
if exists (select * from sysdevices where phyname = @physicalname)
   begin
      raiserror(15061,-1,-1,@physicalname)
      return (1)
   end

/*
**  Prohibit certain special english words from being logical names.
*/
if (@logicalname IN ('disk' ,'diskette' ,'tape' ,'floppy'))
   begin
      raiserror(15285,-1,-1,@logicalname)
      return (1)
   end

/*
**  Make sure that a device with @logicalname doesn't already exist.
*/
if exists (select * from sysdevices where name = @logicalname)
   begin
      raiserror(15026,-1,-1,@logicalname)
      return (1)
   end

/*
**  Always turn on the dump status bit.
*/
select @status = 16

/*
**  If @devtype is a tape then check to see if devstatus is 'skip'.
*/
if @devtype = 'tape'
   begin
      if @devstatus not in ('noskip','skip')
         begin
            raiserror(15047,-1,-1)
            return (1)
         end

      if @devstatus = 'skip' select @status = @status | 8
   end

/*
**  If a disk then the cntrltype = 2
*/
if @devtype = 'disk'
   begin
      insert into sysdevices
         (low, high, status, cntrltype, name, phyname)
         values
         (0, 0, @status, 2, @logicalname, @physicalname)
      print '''Disk'' device added.'
   end

/*
**  If a diskette then the cntrltype in (3,4)
*/
if @devtype = 'diskette'
   begin
      insert into sysdevices
         (low, high, status, cntrltype, name, phyname)
         values
         (0, 0, @status, 3, @logicalname, @physicalname)
      print '''Diskette'' device added.'
   end

/*
**  Tape device.
*/
if @devtype = 'tape'
   begin
      insert into sysdevices
         (low, high, status, cntrltype, name, phyname)
         values
         (0, 0, @status, 5, @logicalname,@physicalname)
      print '''Tape'' device added.'
   end

/*
** Pipe.
*/
if @devtype = 'pipe'
   begin
      insert into sysdevices
         (low, high, status, cntrltype, name, phyname)
         values
         (0, 0, @status, 6, @logicalname,@physicalname)
      print '''Pipe'' device added.'
   end

return (0)
go



raiserror(15339,-1,-1,'sp_adduser')
go
create procedure sp_adduser
@loginame varchar(30),			-- user's login name in syslogins
@name_in_db varchar(30) = NULL,		-- user's name to add to current db
@grpname varchar(30) = NULL		-- group to put new user in
as

declare @suid		int		-- user's system id
declare @grpid		int		-- group id of group to put user in
declare @uid		int		-- new user's id
declare @returncode	int

/*
**  You must be SA or the dbo to execute this sproc.
*/
if suser_id() <> 1 and user_id() <> 1
begin
	raiserror(15000,-1,-1)
	return (1)
end

/*
**  If no new user name is given, use the login name.
*/
if @name_in_db is null
	select @name_in_db = @loginame

/*
**  Check to see that the @name_in_db is valid.
*/
exec @returncode = sp_validname @name_in_db
if @returncode <> 0
	return(1)

/*
**  Check to see if the user already exists in the database.
*/
if exists (select * from sysusers where name = @name_in_db)
	begin
		raiserror(15023,-1,-1,@name_in_db)
		return (1)
	end

/*
** The repl_subscriber user can only be mapped to the repl_subscriber
** login.
*/
if (@loginame='repl_subscriber' or @name_in_db='repl_subscriber')
	and @loginame<>@name_in_db
		begin
			raiserror(15070,-1,-1)
			return(1)
		end

/*
**  Check to see that the user has a login name.
**  We'll also initialize @grpid to 0 while we're here.
*/
select @suid = suid, @grpid = 0 from syslogins where name = @loginame

/*
** If the login doesn't exist and it isn't the special case
** where user was specified as 'guest', return an error.
*/
if @suid is null and (@name_in_db <> 'guest' or @loginame <> 'guest')
	begin
		raiserror(15007,-1,-1,@loginame)
		return (1)
	end

/*
** Can't map guest user to a real login.
*/
if @suid is not null and @name_in_db = 'guest' and @loginame <> 'guest'
	begin
		raiserror(15062,-1,-1)
		return (1)
	end

/*
**  If they specified 'guest' and we got to here, add the guest user.
*/
if @name_in_db = 'guest' and @loginame = 'guest'
	begin
		/*
		**  Add the guest user and return.
		*/
		insert into sysusers (uid, suid, gid, name)
			values (2, -1, 0, 'guest')
		print 'The ''guest'' user has been added.'
		return (0)
	end

/*
**  See if the login already has an account under a different user name.
**  That is, is the user's suid already in the sysusers table.
*/
if exists (select * from sysusers where suid = @suid)
	begin
		raiserror(15063,-1,-1)
		return (1)
	end

/*
**  See if the user is known in the database already with an alias.
**  That is, does the user's suid appear in the sysalternates table.
*/
if exists (select * from sysalternates where suid = @suid)
	begin
		raiserror(15022,-1,-1)
		return (1)
	end

/*
**  If a group name is given, check to see that it is valid.
**  User ids are > 0 and <= 16383 and group ids are > 16383.
*/
if @grpname IS NOT NULL
begin
	select @grpid = -1
	select @grpid = gid from sysusers
	where name = @grpname and (uid > 16383 or uid = 0)

	if @grpid = -1
	begin
		raiserror(15014,-1,-1,@grpname)
		return (1)
	end
end

/*
**  Calculate the first available uid.
*/
if @name_in_db='repl_subscriber'
	select @uid=16383
else
	if (select count(*) from sysusers where uid between 3 and 16382) = 0
		select @uid=3
	else
		select @uid=min(s1.uid)+1 from sysusers s1
		where uid < 16382	/* Don't go into uid range of groups. */
		and uid > 1
		and not exists
			(select * from sysusers s2
			where s2.uid = s1.uid+1)

/* Error if no more user id's available */
if @uid is null
	begin
		raiserror(15065,-1,-1)
		return (1)
	end


insert into sysusers (uid, suid, gid, name)
	values (@uid, @suid, @grpid, @name_in_db)

print 'New user added.'

return (0)
go


raiserror(15339,-1,-1,'sp_addremotelogin')
go
create procedure sp_addremotelogin
@remoteserver	varchar(30),		/* name of remote server */
@loginame varchar(30) = NULL,		/* user's remote name */
@remotename varchar(30) = NULL		/* user's local user name */
as

/*
**  If we're in a transaction, disallow this since it might make recovery
**  impossible.
*/
if @@trancount > 0
	begin
		raiserror(15002,-1,-1,'sp_addremotelogin')
		return (1)
	end

/*
**  Check that the server name is valid.
*/
if not exists (select * from sysservers
		where srvname = @remoteserver)
	begin
		raiserror(15015,-1,-1,@remoteserver)
		return (1)
	end

/*
**  There are three cases to handle.
**
**	1) if only @remoteserver is given then a entry is made in
**		sysremotelogins that means anyone that doesn't have
**		an exact of mapped match in sysremotelogins will use
**		their remotename as their local name and it will be looked
**		up in syslogins.
**
**	2) if @remotename is omitted then it means that anyone from the
**		remote server logging in that doesn't have a complete
**		match in sysremotelogins will be mapped to @loginame.
**
**	3) if @remotename and @loginame are given then it is a straight
**		remote login for sysremotelogins.
*/

/*
**  Case 1:  Only @remoteserver given.
*/
if @loginame is null and @remotename is null
begin
	/*
	**  Check that there is not already an entry for local mapping.
	*/
	if exists (select *
		from sysremotelogins l, sysservers s
			where l.remoteserverid = s.srvid
				and s.srvname = @remoteserver
				and l.remoteusername is null)
	begin
		raiserror(15066,-1,-1,@remoteserver)
		return (1)
	end

	/*
	**  Add the entry.
	*/
	insert into sysremotelogins
		(remoteserverid, remoteusername, suid, status)
	select srvid, null, -1, 0
		from sysservers
			where srvname = @remoteserver
	print 'New remote login created.'
	return (0)

end

/*
**  Check that the @loginame is valid.  These is needed for both
**  case 2 and 3.
*/
if not exists (select * from syslogins where name = @loginame)
	begin
		raiserror(15067,-1,-1,@loginame)
		return (1)
	end

/*
**  Check to make sure that there is not already a @remotename for
**  the @remoteserver.
*/
if exists (select *
		from sysremotelogins l, sysservers s
			where l.remoteserverid = s.srvid
			and s.srvname = @remoteserver
			and l.remoteusername = @remotename)
	begin
		select @remotename = isnull(@remotename,'NULL')
		raiserror(15068,-1,-1,@remotename,@remoteserver)
		return (1)
	end

/*
**  Case 2: We want to make an entry into sysremotelogins that will map
**	any non-exact matches to a particular local user.
*/
if @remotename is null
begin
	/*
	**  Check that there is not already an entry for local mapping.
	*/
	if exists (select *
		from sysremotelogins l, sysservers s
			where l.remoteserverid = s.srvid
			and s.srvname = @remoteserver
			and l.remoteusername is null)
		begin
			raiserror(15066,-1,-1,@remoteserver)
			return (1)
		end

	/*
	**  Go ahead and make the entry.
	*/
	insert into sysremotelogins
		(remoteserverid, remoteusername, suid, status)
	select srvid, null, suser_id(@loginame), 0
		from sysservers
				where srvname = @remoteserver

	print 'New remote login created.'
	return (0)
end

/*
**  Case 3:  All the parameters have been supplied.  All we need to check
**	is that the entry isn't already in sysremotelogins.
**	We've verified the @remoteserver and @loginame above.
*/
if @loginame IS NOT NULL and @remotename IS NOT NULL
begin
	/*
	**  Make sure that the @loginame and @remotename are a
	**  unique combination.
	*/
	if exists (select *
		from sysremotelogins l, sysservers s
			where l.remoteusername = @remotename
				and l.remoteserverid = s.srvid
				and s.srvname = @remoteserver
				and l.suid = suser_id(@loginame))
	begin
		raiserror(15068,-1,-1,@remotename,@remoteserver)
		return (1)
	end

	/*
	**  Go ahead the do the insert.
	*/
	insert into sysremotelogins
		(remoteserverid, remoteusername, suid, status)
	select srvid, @remotename, suser_id(@loginame), 0
		from sysservers
			where srvname = @remoteserver
	print 'New remote login created.'
	return (0)
end

/*
**  We got here because the syntax was incorrect.
*/
raiserror(15072,-1,-1)
return (1)
go


raiserror(15339,-1,-1,'sp_addsegment')
go
create procedure sp_addsegment
@segname	varchar(30),	/* segment name */
@devname	varchar(30)	/* device name to put segment on */
as

declare @dbuid		smallint	/* id of the owner of the database */
declare @segbit		int		/* this is the bit to turn on in sysusages */
declare @retcode	int
declare @dbname		varchar(30)

/*
**  If we're in a transaction, disallow this since it might make recovery
**  impossible.
*/
if @@trancount > 0
	begin
		raiserror(15002,-1,-1,'sp_addsegment')
		return (1)
	end

/*
**  You must be SA or the dbo to execute this sproc.
*/
if suser_id() <> 1 and user_id() <> 1
	begin
		raiserror(15000,-1,-1)
		return (1)
	end

/*
**  See if the device exists.
*/
if not exists (select * from sysdevices where name like @devname)
	begin
		raiserror(15012,-1,-1,@devname)
		return (1)
	end

/*
**	Check that the device is a database device.
*/

if not exists (select * from sysdevices
		where name like @devname
			and cntrltype in (0,7))
	begin
		raiserror(15035,-1,-1,@devname)
		return (1)
	end

/*
**  Now see if the @dbname uses the @devname
*/
if not exists (select *
		from sysusages u, sysdevices d
			where d.name = @devname
				and u.vstart between d.low and d.high
				and u.dbid = db_id())
begin
	select @dbname=db_name()
	raiserror(15073,-1,-1,@devname,@dbname)
	return (1)
end

/*
**  Check to see if the device is marked as a log device.
**  If so, return error.
*/
if exists (select * from sysusages u, sysdevices d
		where d.name = @devname
		and u.vstart between d.low and d.high
		and u.dbid = db_id()
		and segmap = 4)
	begin
		raiserror(15074,-1,-1,@devname)
		return (1)
	end

/*
** Check for valid identifier.
*/
execute @retcode = sp_validname @segname
if @retcode <> 0
	return (1)

/*
**  Now go ahead and define the new segment and add it to the segmap
**  of sysusages.
**  NOTE: Don't update master..sysusages and syssegments as a xact since
**  	it could cause problems for recovery.
*/

/*
**  Check that @segname doesn't already exist.
*/
if exists (select * from syssegments holdlock where name = @segname)
begin
	raiserror(15027,-1,-1,@segname)
	return (1)
end

/*
**  Figure out the next segment number to use.
**  Segment number may be 0-31.
*/
select @segbit = 3
while @segbit < 32
begin
	/*
	**  Did we find one?
	*/
	if exists (select *
			from syssegments
				where segment = @segbit)
	begin
		select @segbit = @segbit + 1
	end

	/*
	**  We found an opening so break out.
	*/
	else break

end

if @segbit >= 32
begin
	raiserror(15075,-1,-1)
	return (1)
end

/*
**  Add the new segment.
*/
insert into syssegments (segment, name, status)
	values (@segbit, @segname, 0)

/*
**  Now set the segments on @devname sysusages.
*/
if (@segbit < 31)
	select @segbit = power(2, @segbit)
else
	/*
	**  Since this is segment 31, power(2, 31) will overflow
	**  since segmap is an int.  We'll grab the machine-dependent
	**  bit mask from spt_values to set the right bit.
	*/
	select @segbit = low
		from master.dbo.spt_values
			where type = 'E'
				and number = 2
update sysusages
	set segmap = segmap | @segbit
		from sysusages u,
			sysdevices d
		where d.name = @devname
			and u.vstart between d.low and d.high
			and u.dbid = db_id()

/*
**  Now we need to activate the new segment map.
*/
select @dbname = db_name()
dbcc dbrepair (@dbname, remap)
print 'Segment created.'

return (0)
go


raiserror(15339,-1,-1,'sp_addserver')
go
create procedure sp_addserver  --1996/02/28 10:45
    @server          varchar(30)          --server name
   ,@local           varchar(10) = NULL   --local server, or fallback
   ,@duplicate_ok    varchar(13) = NULL   --duplicate_ok
as

declare @srvid          smallint
       ,@fallback_bit   integer

---- If we are in a transaction, disallow this since it might make
----   recovery impossible.

if @@trancount > 0
   begin
      raiserror(15002,-1,-1,'sp_addserver')
      return (1)
   end

---- Server names must be unique so check.

if exists (select * from sysservers where srvname = @server)
   begin
      if @duplicate_ok = 'duplicate_ok'
         return (0)
      raiserror(15028,-1,-1,@server)
      return (1)
   end

---- Check to see that the @server is valid.

declare @returncode   int
execute @returncode = sp_validname @server
if @returncode <> 0
   return(1)


if (@local is not null)
   select @local = lower(@local)

---- If this is the not the local server, then get the first available srvid.

if (@local is null)
   begin
      ---- If no servers are found, use #1.
      select @srvid = isnull(min(srvid)+1,1)
         from sysservers s1
         where srvid < 32767   -- Avoid arithmetic overflow.
         and not exists
            (select * from sysservers s2
            where s2.srvid = s1.srvid+1)

      insert into sysservers
         (srvid, srvstatus, srvname, srvnetname)
         values
         (@srvid, 1, @server, @server)

   GOTO LABEL_75_DONE
   end

---- Handle fallback server.

if (@local='fallback')
   begin

   select @fallback_bit = min(val.number) from master..spt_values val
               where val.type='A  ' and val.name='fallback'

   ---- Ensure max of 1 fallback server.
   if exists (select * from master..sysservers
                where srvstatus & @fallback_bit > 0)
      begin
      RaisError(15378,-1,-1)
      return(1)
      end

   select @srvid = isnull(min(srvid)+1,1)
      from sysservers s1
      where srvid < 32767  -- Avoid arithmetic overflow.
      and not exists
         (select * from sysservers s2
         where s2.srvid = s1.srvid+1)

   insert into sysservers
      (srvid, srvstatus, srvname, srvnetname)
      values
      (@srvid, (1+ @fallback_bit), @server, @server)

   GOTO LABEL_75_DONE
   end

---- If @local = 'local' then this is the local server and its srvid = 0.

if (@local = 'local')
   begin
      if exists (select * from sysservers where srvid = 0)
         begin
            raiserror(15090,-1,-1)
            return (1)
         end

      insert into sysservers
         (srvid, srvstatus, srvname, srvnetname)
         values (0, 0, @server, @server)

   GOTO LABEL_75_DONE
   end

RaisError(15379,-1,-1,@local)  -- error, unrecognized.

LABEL_75_DONE:

print 'Server added.'
return (0)
go


dump transaction master with no_log
go


raiserror(15339,-1,-1,'sp_addtype')
go
create procedure sp_addtype
@typename varchar(30),		-- name of user-defined type
@phystype varchar(30),		-- physical system type of user-defined type
@nulltype varchar(8) = null	-- nullability of new type
as

declare @len int		-- length of user type
declare @type tinyint		-- typeid of physical type
declare @tlen tinyint		-- length of physical type
declare @typeid smallint	-- user typeid of physical type
declare @var bit		-- is physical type variable length?
declare @nonull bit		-- default is getansinull()
declare @nullegal bit		-- does physical type allow NULLs?
declare @prec int		-- precision of the datatype
declare @scale int		-- scale of the datatype
declare @tprec tinyint		-- precision of the datatype read from systypes
declare @tscale tinyint		-- scale of the datatype read from systypes

select @nulltype = lower(@nulltype)

/*
**  Types based on 'bit' cannot be nullable.
*/
if lower(@phystype) = 'bit'
	if @nulltype is null	-- If user didn't specify nullability,
				-- make sure it doesn't get set to nullable
				-- by getansinull()
		select @nulltype = 'not null'
	else			-- Also make sure that user doesn't try
				-- to explicitly set it to nullable.
		if @nulltype = 'null'
			begin
				raiserror(15276,-1,-1)
				return(1)
			end

/*
**  Should the user type allow NULLs?
*/
if @nulltype is null
	select @nonull = abs(getansinull()-1)
else if @nulltype = 'null'
	select @nonull = 0
else if @nulltype in ('not null','nonull')
	select @nonull = 1
else
	begin
		raiserror(15085,-1,-1)
		return (1)
	end

/*
**  Check to see that the @typename is valid.
*/
declare @returncode int
execute @returncode = sp_validname @typename
if @returncode <> 0
	return(1)

/*
**  Check to see if the user type already exists or a system type
**  whose name = lower(@typename) (or a synomym) already exists.
*/
if exists (select * from systypes where name = @typename)
		or lower(@typename)
		in ('character','character varying','char varying',
			'integer','dec','binary varying')

	begin
		raiserror(15029,-1,-1,@typename)
		return (1)
	end

/*
** Make physical typename all lower case to ensure case insensitivity.
*/
select @phystype = lower(@phystype)

/*
** Can't supply length with sysname type.
*/
if @phystype like 'sysname%(%'
	begin
		raiserror(15270,-1,-1)
		return(1)
	end

/*
** initialize the length to be NULL first.
*/
select @len = NULL

/*
** If precision and scale were given with the type - extract them
*/
if @phystype like '_%(_%,_%)'
begin
	select @prec = convert(int, substring(@phystype,
		charindex('(',@phystype) + 1,
		charindex(',',@phystype) - 1 - charindex('(',@phystype)))

	select @scale = convert(int, substring(@phystype,
		charindex(',',@phystype) + 1,
		charindex(')',@phystype) - 1 - charindex(',',@phystype)))
	/*
	** Extract the physical type name
	*/
	select @phystype = substring(@phystype, 1,
		   charindex('(', @phystype) - 1)
end
else

/*
**  If a length was given with the user datatype, extract it.
*/
if @phystype like '_%(%)'
begin
	select @len = convert(int, substring(@phystype,
		charindex('(',@phystype) + 1,
		charindex(')',@phystype) - 1 - charindex('(',@phystype)))

	/*
	** Extract the physical type name
	*/
	select @phystype = substring(@phystype, 1,
		   charindex('(', @phystype) - 1)
end

if @phystype='character'
	select @phystype='char'
else if @phystype='character varying'
	select @phystype='varchar'
else if @phystype='char varying'
	select @phystype='varchar'
else if @phystype='integer'
	select @phystype='int'
else if @phystype='dec'
	select @phystype='decimal'
else if @phystype='binary varying'
	select @phystype='varbinary'

/*
**  Make sure that the physical type exists and get its characteristics.
**  System physical types have a usertype < 100 and are owned by the
**  dbo (userid = 1).
*/
select @type = type, @tlen = length, @typeid = usertype,
	@var = variable, @nullegal = allownulls, @tprec = prec,
	@tscale = scale
from systypes
	where usertype < 100 and name = @phystype and uid = 1

if @type is null
begin
	raiserror(15036,-1,-1,@phystype)
	return (1)
end

/*
**  Disallow user-defined datatypes on timestamps.  This is done because
**  a timestamp is not a basic type but is really a binary.  There is,
**  therefore, no way to tell if a user-defined datatype is mapped to
**  a binary or a timestamp.  Timestamps can't have rules or defaults.
*/
if @phystype = 'timestamp'
begin
	raiserror(15038,-1,-1)
	return (1)
end

/*
**  Check if the NULL status of the user type is consistent with the NULL status
**  of the physical type.  Here are the possible cases.
**
**		   physical type
**		  NULLs	  NONULLs
**	        -----------------
** user	NULLs	|  ok	|  no
** type NONULLs	|  ok	|  ok
*/
if @nonull = 0 and @nullegal = 0
	begin
		raiserror(15037,-1,-1,@phystype)
		return (1)
	end

/*
**  We'll use the variable @nullegal when we update the systypes table
**  so we need to set it to reflect if NULLs are allowed (@nonull = 0)
**  or NO NULLs are allowed (@nonull = 1).
*/
if @nonull = 0
	select @nullegal = 1
else
	select @nullegal = 0


/* Decide about precision, scale, length
** First check from NUMERIC, DECIMAL
*/
if (@typeid = 24) or  (@typeid = 10)
begin
	/* Type is NUMERIC or DECIMAL */

	if @len > 0
		begin
			/* Length is really the precision
			** Since no scale is specified then scale
			** is minimum(Default, precision). Default = 4
			*/
			select @prec = @len
			select @scale = 0
		end
	else
		if (@prec is NULL)
			begin
				select @prec = 18
				select @scale = 0
			end

	if (@prec > 38) or (@prec < 1)
		begin
			raiserror(15086,-1,-1)
			return (1)
		end

	if (@scale > @prec) or (@scale < 0)
		begin
			/*
			** Illegal scale specified -- must be less than precision
			** and positive.
			*/
			raiserror(15087,-1,-1)
			return (1)
		end

	/* Compute length from precision */
	if (@prec <= 2)
		select @len = 2

	if (@prec > 2) and (@prec <= 4)
		select @len = 3

	if (@prec > 4) and (@prec <= 7)
		select @len = 4

	if (@prec > 7) and (@prec <= 9)
		select @len = 5

	if (@prec > 9) and (@prec <= 12)
		select @len = 6

	if (@prec > 12) and (@prec <= 14)
		select @len = 7

	if (@prec > 14) and (@prec <= 16)
		select @len = 8

	if (@prec > 16) and (@prec <= 19)
		select @len = 9

	if (@prec > 19) and (@prec <= 21)
		select @len = 10

	if (@prec > 21) and (@prec <= 24)
		select @len = 11

	if (@prec > 24) and (@prec <= 26)
		select @len = 12

	if (@prec > 26) and (@prec <= 28)
		select @len = 13

	if (@prec > 28) and (@prec <= 31)
		select @len = 14

	if (@prec > 31) and (@prec <= 33)
		select @len = 15

	if (@prec > 33) and (@prec <= 36)
		select @len = 16

	if (@prec > 36) and (@prec <= 38)
		select @len = 17
end
else

/*
**  Typeids 1 (char), 2 (varchar), 3 (binary) and 4 (varbinary) are the
**  only ones which allow a length to be specified.
*/
if @typeid > 4
	begin
		/*
		**  We can't use a length and we got one.
		*/
		if @len > 0
			begin
				raiserror(15088,-1,-1)
				return (1)
			end

		/*
		**  Use the fixed length of the physical type.
		*/
		select @len = @tlen
		select @prec = @tprec
		select @scale = @tscale
	end
else
	begin
		/*
		**  We need a length and we didn't get one.
		*/
		if @len is null
			begin
				raiserror(15091,-1,-1)
				return (1)
			end

		if @len <= 0 or @len > 255
			begin
				raiserror(15092,-1,-1)
				return (1)
			end

		select @prec = @len
		select @scale = @tscale
	end

/*
**  Finally, get the maximum existing user type so we use it + 1 for this
**  new type.
*/
select @typeid = max(usertype)
	from systypes

/*
**  There are no user defined types yet so use the first number (101).
*/
if @typeid < 100
	select @typeid = 100

insert systypes (uid, variable, type, length, tdefault,
	domain, name, usertype, allownulls, prec, scale)
select user_id(), @var, @type, @len, 0, 0, @typename, @typeid + 1, @nullegal,
	  @prec, @scale

print 'Type added.'

return (0)
go


raiserror(15339,-1,-1,'sp_addlanguage')
go
create procedure sp_addlanguage
@language varchar(30),
@alias varchar(30) = NULL,
@months varchar(251),
@shortmons varchar(119),
@days varchar(216),
@datefmt char(3),
@datefirst tinyint
as

declare @msg varchar(250)
declare @nextlangid smallint
declare @returncode int

/*  Check to see if the language exists. */
if exists (select * from syslanguages where name = @language)
	begin
		raiserror(15039,-1,-1,@language)
		return (1)
	end

/* Check to see that the list of full month names is valid. */
execute @returncode = sp_chklangparam @months, 'full month', 12, 20
if @returncode <> 0
	return (1)

/* Check to see that the list of short month names is valid. */
execute @returncode = sp_chklangparam @shortmons, 'short month', 12, 9
if @returncode <> 0
	return (1)

/* Check to see that the list of day names is valid. */
execute @returncode = sp_chklangparam @days, 'day', 7, 30
if @returncode <> 0
	return (1)

/* Check to see that the @datefmt is valid. */
if (@datefmt <> 'mdy' and @datefmt <> 'dmy' and @datefmt <> 'ymd' and @datefmt <> 'ydm'
	and @datefmt <> 'myd' and @datefmt <> 'dym')
begin
	raiserror(15093,-1,-1,@datefmt)
	return (1)
end

/*
**  Check to see that the @datefirst is valid.
*/
if (@datefirst < 1 or @datefirst > 7)
begin
	select @msg = convert(char(3), @datefirst)
	raiserror(15094,-1,-1,@msg)
	return (1)
end

/*
**  If the user didn't specify the alias name, the alias name is same as
**  the official name.
*/
if @alias is null
begin
	select @alias =  @language
end

/*  Check to see if the alias exists. */

if exists (select * from syslanguages where alias = @alias)
	begin
		raiserror(15034,-1,-1,@alias)
		return (1)
	end

if @language = 'us_english'
begin
	/*  The language id of us_english is 0. */
	insert syslanguages (langid, dateformat, datefirst, upgrade,
		name, alias, months, shortmonths, days)
	select 0, @datefmt, @datefirst, 0,
		@language, @alias, @months, @shortmons, @days
end
else
begin

	if (select max(langid) from syslanguages) is null
		select @nextlangid = 1
	else
		select @nextlangid = (select max(langid)+1
		from syslanguages)

	insert into syslanguages
		(langid, dateformat, datefirst, upgrade, name, alias,
			months, shortmonths, days)
	select @nextlangid, @datefmt, @datefirst, 0,
		@language, @alias, @months, @shortmons, @days
end


/* If the insert failed, say so. */
if @@error <> 0
	begin
		raiserror(15095,-1,-1)
		return(1)
	end

print 'New language inserted.'
go


raiserror(15339,-1,-1,'sp_altermessage')
go
create procedure sp_altermessage
@message_id       int,
@parameter        varchar(30),
@parameter_value  varchar(5)
as
begin
  declare @rows_affected  varchar(16)
  declare @msg            varchar(128)

  select @parameter = upper(@parameter)
  select @parameter_value = upper(@parameter_value)

  /*
  **  You must be SA or the dbo to execute this sproc.
  */
  if (suser_id() <> 1)
  begin
          raiserror(15003, 16, 1)
          return (1)
  end

  /*
  ** Does this message exist?
  */
  if (not exists (select * from master.dbo.sysmessages
                  where error = @message_id))
  begin
          raiserror(15179,-1,-1,@message_id)
          return (1)
  end

  /*
  ** Is Parameter 'WITH_LOG'?
  */
  if (@parameter <> 'WITH_LOG')
  begin
	raiserror(15176,-1,-1)
	return (1)
  end

  /*
  ** Is ParameterValue TRUE or FALSE?
  */
  if (@parameter_value not in ('TRUE', 'FALSE'))
  begin
          raiserror(15277,-1,-1)
          return (1)
  end

  /*
  ** Determine the number of messages affected
  */
  select @rows_affected = convert(varchar(16), count(*)) from master.dbo.sysmessages
       where error = @message_id
  select @msg = @rows_affected + ' Messages altered.'

  /*
  ** Turn dlevel bit 7 on or off
  */
  if (@parameter_value = 'TRUE')
  begin
          update master.dbo.sysmessages
                  set dlevel = dlevel | 0x80
                          where error = @message_id
          if (@@error = 0)
          begin
                  if (convert(int, @rows_affected) = 1) PRINT 'Message altered.'
                  if (convert(int, @rows_affected) > 1) PRINT @msg
          end
  end
  else
  if (@parameter_value = 'FALSE')
  begin
          update master.dbo.sysmessages
                  set dlevel = dlevel & 0x7FFFFF7F
                          where error = @message_id
          if (@@error = 0)
          begin
                  if (convert(int, @rows_affected) = 1) PRINT 'Message altered.'
                  if (convert(int, @rows_affected) > 1) PRINT @msg
          end
  end
  return (0)
end
go


raiserror(15339,-1,-1,'sp_defaultlanguage')
go
create procedure sp_defaultlanguage
@loginame varchar(30),			/* login name of the user */
@language varchar(30) = NULL		/* default language for the new user */
as

declare @msg varchar(250)
declare @returncode int

/*
**  Only the system administrator (sa) can run this command.
**  Check to make sure the executor is the sa.
*/
if (suser_id() <> 1 and suser_id() <> suser_id(@loginame))
begin
	raiserror(15096,-1,-1,@loginame,@loginame)
	return (1)
end

/*
**  Check to see that the @language is valid.
*/
if @language is not null
begin
	execute @returncode = sp_validlang @language
	if @returncode <> 0
		return(1)
end

/*
**  Make sure the login already exists.
*/
if not exists (select * from syslogins where name = @loginame)
	begin
		raiserror(15007,-1,-1,@loginame)
		return (1)
	end

/*
**  Set the default language for this user.
*/
update syslogins
	set language = @language
	 	where name = @loginame

select @msg =  @loginame + '''s default language is changed to ' + @language
print @msg

return (0)
go


raiserror(15339,-1,-1,'sp_helplanguage')
go
create procedure sp_helplanguage
@language varchar(30) = NULL
as

/* Print all languages if the user didn't give the language name. */
if @language is null
begin
	if exists (select * from syslanguages)
		select * from syslanguages
	else
		print 'No alternate languages are available.'

	/* Find out whether us_english is there or not. */
	if not exists (select * from syslanguages
			where name = 'us_english')
		print 'us_english is always available, even though it is not in syslanguages.'

	return (0)
end

/*  Report information on this language. */
if exists (select * from syslanguages where name = @language)
	begin
		select * from syslanguages where name = @language
		return (0)
	end

if exists (select * from syslanguages where alias = @language)
	begin
		select * from syslanguages where alias = @language
		return (0)
	end

/* Couldn't find this language. */
if @language = 'us_english'
	begin
		print 'us_english is always available, even though it is not in syslanguages.'
		return (0)
	end
else
	begin
		raiserror(15033,-1,-1,@language)
		return (1)
	end
go


raiserror(15339,-1,-1,'sp_droplanguage')
go
create procedure sp_droplanguage
@language varchar(30),
@dropmessages varchar(30) = NULL
as

declare @langid smallint
declare @dropmsgs int


/*  Check to see if the language exists. */
if not exists (select * from syslanguages where name = @language)
	begin
		raiserror(15033,-1,-1,@language)
		return (1)
	end

/* Get language id from syslanguages. */
select @langid = (select langid
			from syslanguages
				where name = @language)

/*  Check to see if 'dropmessages' is requested. */
select @dropmsgs = 0
if @dropmessages is null
	begin
		if exists (select * from sysmessages where langid = @langid)
			begin
				/*
				**  Cannot drop a language if the language has associated
				**  entries in the sysmessages.
				*/	
				raiserror(15097,-1,-1,@language)
				return (1)
			end
	end
else
	begin
		if @dropmessages <> 'dropmessages'
			begin
				raiserror(15098,-1,-1)
				return (1)
			end

		select @dropmsgs = 1
	end

/*
**  Drop the language and delete messages from Sysmessages if there is any.
*/
if @dropmsgs = 1
	delete sysmessages where langid = @langid

delete syslanguages
	where langid = @langid

print 'Language deleted.'

go


raiserror(15339,-1,-1,'sp_setlangalias')
go
create procedure sp_setlangalias
@language varchar(30),
@alias varchar(30)
as

declare @returncode int

/*  Check to see if the language exists. */
exec @returncode = sp_validlang @language
if @returncode <> 0
	return (1)

/*  Check to see if the alias exists. */

if exists (select * from syslanguages where alias = @alias)
	begin
		raiserror(15034,-1,-1,@alias)
		return (1)
	end

/* Reset the alternate language name. */
update syslanguages set alias = @alias where name = @language

/* If the update failed, say so. */
if @@error <> 0
	begin
		raiserror(15099,-1,-1)
		return(1)	
	end

print 'Language alias reset.'
go


raiserror(15339,-1,-1,'sp_bindefault')
go
create procedure sp_bindefault
@defname varchar(92),			/* name of the default */
@objname varchar(61),			/* table or usertype name */
@futureonly varchar(15) = NULL		/* flag to indicate extent of binding */
as

declare @defid int			/* id of the default to bind */
declare @futurevalue varchar(15)	/* the value of @futureonly that causes
					** the binding to be limited */

set nocount on

select @futurevalue = 'futureonly'	/* initialize @futurevalue */

/*
**  When a default or rule is bound to a user-defined datatype, it is also
**  bound, by default, to any columns of the user datatype that are currently
**  using the existing default or rule as their default or rule.  This default
**  action may be overridden by setting @futureonly = @futurevalue when the
**  procedure is invoked.  In this case existing columns with the user
**  datatype won't have their existing default or rule changed.
*/

/*
**  Check to see that the object names are local to the current database.
*/
if (@objname like '%.%.%')
   or
   (@defname like '%.%.%' and
	substring(@defname, 1, charindex('.', @defname) - 1) <> db_name())
begin
	raiserror(15076,-1,-1)
	return (1)
end

/*
**  Check that the @futureonly argument, if supplied, is correct.
*/
if (@futureonly IS NOT NULL)
	if (@futureonly <> @futurevalue)
		begin
			raiserror(15100,-1,-1)
			return (1)
		end

/*
**  Check to see that the default exists and get its id.
*/
select @defid = (select id from sysobjects
			where id = object_id(@defname)
				and sysstat & 0xf = 6)	-- default object

if @defid is NULL
	begin
		raiserror(15016,-1,-1,@defname)
		return (1)
	end

/*
**  If @objname is of the form tab.col then we are binding to a column.
**  Otherwise its a datatype.  In the column case, we need to extract
**  and verify the table and column names and make sure the user owns
**  the table that is getting the default bound. We also need to ensure
**  that we don't overwrite any DRI style defaults.
*/
if @objname like '%.%'
begin
	declare @tabname varchar(30)		/* name of table */
	declare @colname varchar(30)		/* name of column */

	/*
	**  Get the table name out.
	*/
	select @tabname = substring(@objname, 1, charindex('.', @objname) - 1)
	select @colname = substring(@objname, charindex('.', @objname) + 1, 61)


	/*
	**  If the column type is timestamp, disallow the bind.
	**  Defaults can't be bound to timestamp columns.
	*/
	if exists (select *
			from syscolumns c
		        where c.id = object_id(@tabname)
			and c.name = @colname
			and c.usertype = 80)
	begin
		raiserror(15101,-1,-1)
		return (1)
	end

	/*
	**  If the column category is identity, disallow the bind.
	**  Defaults can't be bound to identity columns.
	*/
	if exists (select *
			from syscolumns c
		        where c.id = object_id(@tabname)
			and c.name = @colname
			and c.status & 128 = 128)
	begin
		raiserror(15102,-1,-1)
		return (1)
	end


        /*
	**  Check to see if the column was created with or altered
        **  to have a DRI style default value.
	*/
	if exists (select *
			from syscolumns c,sysconstraints t
		        where c.id = object_id(@tabname)
			and c.name = @colname
                        and t.id = c.id
                        and t.colid = c.colid
                        and t.status & 0xf = 5)
	begin
		raiserror(15103,-1,-1)
		return (1)
	end

        update syscolumns
		set cdefault = @defid
			from syscolumns, sysobjects
		where syscolumns.id = object_id(@tabname)
			and syscolumns.name = @colname
			and sysobjects.id = object_id(@tabname)
			and uid = user_id()
			and sysobjects.sysstat & 0xf = 3      /* user table */

	/*
	**  Did the bind happen?
	*/
	if @@rowcount <> 1
	begin
		raiserror(15104,-1,-1,@tabname,@colname)
		return (1)
	end

	/*
	**  Since binding a default is a schema change, update schema count
	**  for the object in the sysobjects table.
	*/
	update sysobjects
		set schema_ver = schema_ver + 1
			from sysobjects
				where id = object_id(@tabname)
					and uid = user_id()

	print 'Default bound to column.'

end
else
begin
	/*
	**  We're binding to a user type.  In this case, the @objname
	**  is really the name of the user datatype.
	**  When we bind to a user type, any existing columns get changed
	**  to the new binding unless their current binding is not equal
	**  to the current binding for the usertype or if they set the
	**  @futureonly parameter to @futurevalue.
	*/
	declare @olddefault int	/* current default for type */

	/*
	**  Get the current default for the datatype.
	*/
	select @olddefault = tdefault
		from systypes
	where name = @objname
		and uid = user_id()
		and usertype > 100

	if @olddefault is null
		begin
			raiserror(15105,-1,-1)
			return (1)
		end

	update systypes
		set tdefault = @defid
			from systypes
		where name = @objname
			and uid = user_id()
			and usertype > 100


	print 'Default bound to datatype.'

	/*
	**  Now see if there are any columns with the usertype that
	**  need the new binding.
	*/
	if isnull(@futureonly, '') <> @futurevalue
		and exists (select *
			from syscolumns, systypes
				where syscolumns.usertype = systypes.usertype
					and systypes.name = @objname
					and systypes.usertype > 100
					and systypes.uid = user_id()
					and (syscolumns.cdefault = @olddefault
					or syscolumns.cdefault = 0 ) )
	begin
		/*
		**  Update syscolumns with new binding.
		*/
		update syscolumns
			set cdefault = systypes.tdefault
				from syscolumns, systypes
			where syscolumns.usertype = systypes.usertype
				and systypes.name = @objname
				and systypes.usertype > 100
				and systypes.uid = user_id()
                                and (syscolumns.cdefault = @olddefault
                                          or  syscolumns.cdefault = 0 )

		/*
		**  Update the table schema to indicate that something
		**  has changed in the table's schema.
		*/
		update sysobjects
			set schema_ver = schema_ver + 1
				from sysobjects, syscolumns, systypes
			where sysobjects.id = syscolumns.id
				and syscolumns.usertype = systypes.usertype
				and systypes.name = @objname
				and systypes.usertype > 100
				and systypes.uid = user_id()
                                and (syscolumns.cdefault = @olddefault
                                          or  syscolumns.cdefault = 0 )

		print 'The new default has been bound to columns(s) of the specified user datatype.'
	end
end

return (0)
go


raiserror(15339,-1,-1,'sp_bindrule')
go
create procedure sp_bindrule
@rulename varchar(92),			/* name of the rule */
@objname varchar(61),			/* table or usertype name */
@futureonly varchar(15) = NULL		/* column name */
as

declare @ruleid int			/* id of the rule to bind */
declare @futurevalue varchar(15)	/* the value of @futureonly that causes
					** the binding to be limited */

set nocount on

select @futurevalue = 'futureonly'	/* initialize @futurevalue */

/*
**  When a default or rule is bound to a user-defined datatype, it is also
**  bound, by default, to any columns of the user datatype that are currently
**  using the existing default or rule as their default or rule.  This default
**  action may be overridden by setting @futureonly = @futurevalue when the
**  procedure is invoked.  In this case existing columns with the user
**  datatype won't have their existing default or rule changed.
*/

/*
**  Check to see that the object names are local to the current database.
*/
if (@objname like '%.%.%')
   or
   (@rulename like '%.%.%' and
	substring(@rulename, 1, charindex('.', @rulename) - 1) <> db_name())
	begin
		raiserror(15077,-1,-1)
		return (1)
	end

/*
**  Check that the @futureonly argument, if supplied, is correct.
*/
if (@futureonly IS NOT NULL)
	begin
		if (@futureonly <> @futurevalue)
			begin
				raiserror(15106,-1,-1)
				return (1)
			end
	end

/*
**  Check to see that the rule exists and get its id.
*/
select @ruleid = (select id from sysobjects
			where id = object_id(@rulename)
				and sysstat & 0xf = 7)	-- rule object

if @ruleid is NULL
	begin
		raiserror(15017,-1,-1,@rulename)
		return (1)
	end

/*
**  If @objname is of the form tab.col then we are binding to a column.
**  Otherwise its a datatype.  In the column case, we need to extract
**  and verify the table and column names and make sure the user owns
**  the table that is getting the rule bound.
*/
if @objname like '%.%'
begin
	declare @tabname varchar(30)		/* name of table */
	declare @colname varchar(30)		/* name of column */

	/*
	**  Get the table name out.
	*/
	select @tabname = substring(@objname, 1, charindex('.', @objname) - 1)
	select @colname = substring(@objname, charindex('.', @objname) + 1, 61)

	/*
	**  If the column type is image, text, or timestamp, disallow the bind.
	**  Rules can't be bound to image, text, or timestamp columns.
	**  The qualification to check for types is a little strange because
	**  text and image are real, basic data types while timestamp is not.
	**  Timestamp is really a binary.  The types are checked in case
	**  there is a user-defined datatype that is an image or text.
	**  User-defined datatypes mapping to timestamp are not allowed
	**  by sp_addtype.
	*/
	if exists (select *
			from sysobjects o, syscolumns c
		where c.id = object_id(@tabname)
			and c.name = @colname
			and c.name = @colname
			and o.id = object_id(@tabname)
			and o.sysstat & 0xf = 3
			and (c.type in (35, 34) or c.usertype = 80))
		begin
			raiserror(15107,-1,-1)
			return (1)
		end

	update syscolumns
		set domain = @ruleid
			from syscolumns, sysobjects
		where syscolumns.id = object_id(@tabname)
			and syscolumns.name = @colname
			and sysobjects.id = object_id(@tabname)
			and uid = user_id()
			and sysobjects.sysstat & 0xf = 3      /* user table */

	/*
	**  Did the bind happen?
	*/
	if @@rowcount <> 1
	begin
		raiserror(15104,-1,-1,@tabname,@colname)
		return (1)
	end

	/*
	**  Since binding a rule is a schema change, update schema count
	**  for the object in the sysobjects table.
	*/
	update sysobjects set schema_ver = schema_ver + 1
		from sysobjects
		where id = object_id(@tabname)
			and uid = user_id()

	print 'Rule bound to table column.'

end
else
begin
	/*
	**  We're binding to a user type.  In this case, the @objname
	**  is really the name of the user datatype.
	**  When we bind to a user type, any existing columns get changed
	**  to the new binding unless their current binding is not equal
	**  to the current binding for the usertype or if they set the
	**  @futureonly parameter to @futurevalue.
	*/
	declare @oldrule int			/* current rule for type */

	/*
	**  If the column type is image, text, or timestamp, disallow the bind.
	**  Rules can't be bound to image or text columns.
	*/
	if exists (select * from systypes
			where name = @objname
				and (type in (35, 34) or usertype = 80))
		begin
			raiserror(15107,-1,-1)
			return (1)
		end

	/*
	**  Get the current rule for the datatype.
	*/
	select @oldrule = domain from systypes
	where name = @objname
		and uid = user_id()
		and usertype > 100

	if @oldrule is null
		begin
			raiserror(15105,-1,-1)
			return (1)
		end

	update systypes set domain = @ruleid
			from systypes
		where name = @objname
			and uid = user_id()
			and usertype > 100


	print 'Rule bound to datatype.'

	/*
	**  Now see if there are any columns with the usertype that
	**  need the new binding.
	*/
	if isnull(@futureonly, '') <> @futurevalue
		and exists (select *
			from syscolumns, systypes
				where syscolumns.usertype = systypes.usertype
					and systypes.name = @objname
					and systypes.usertype > 100
					and systypes.uid = user_id()
                                        and (syscolumns.domain = @oldrule
                                          or  syscolumns.domain = 0 )     )
	begin
		print 'The new rule has been bound to column(s) of the specified user datatype.'
		/*
		**  Update syscolumns with new binding.
		*/
		update syscolumns
			set domain = systypes.domain
				from syscolumns, systypes
			where syscolumns.usertype = systypes.usertype
				and systypes.name = @objname
				and systypes.usertype > 100
				and systypes.uid = user_id()
                                and (syscolumns.domain = @oldrule
                                  or  syscolumns.domain = 0 )

		/*
		**  Update the table schema to indicate that something
		**  has changed in the table's schema.
		*/
		update sysobjects
			set schema_ver = schema_ver + 1
				from sysobjects, syscolumns, systypes
			where sysobjects.id = syscolumns.id
				and syscolumns.usertype = systypes.usertype
				and systypes.name = @objname
				and systypes.usertype > 100
				and systypes.uid = user_id()
                                and (syscolumns.domain = @oldrule
                                  or  syscolumns.domain = 0 )
	end
end

return (0)
go


raiserror(15339,-1,-1,'sp_changedbowner')
go
create procedure sp_changedbowner
@loginame varchar(30),         /* login to become dbo */
@map      varchar(10) = NULL   /* True to map aliases, else drop  */
as

declare @suid    smallint
declare @oldsuid smallint

/*
**  Only the SA can change the owner of a database.
*/
if suser_id() <> 1
begin
	raiserror(15108,-1,-1)
	return(1)
end

/*
**  Can't change the owner of the master database.
*/
if db_id() = 1
begin
	raiserror(15109,-1,-1)
	return(1)
end

/*
**  Make sure that @loginame exists and has a login.
*/
select @suid = suid from syslogins where name = @loginame

if @suid is null
	begin
		raiserror(15007,-1,-1,@loginame)
		return(1)
	end

/*
**  Make sure that @loginame isn't already a user or alias in the database.
*/
if exists (select * from sysusers where suid = @suid)
	begin
		raiserror(15110,-1,-1)
		return(1)
	end

if exists (select * from sysalternates where suid = @suid)
	begin
		raiserror(15111,-1,-1)
		return(1)
	end

/*
** find old (current) dbo's suid
*/
select @oldsuid = suid
	from sysusers
		where uid = 1

begin transaction

/*
**  Now change the suid of the owner of the database to the suid of @loginame.
*/
update sysusers
	set suid = @suid
		where uid = 1

/*
** if the user requested that aliases be mapped to new dbo, do that.
*/
if lower(@map) = 'true'
begin

	if exists (select *
			from sysalternates
				where altsuid = @oldsuid)
	begin
		update sysalternates
			set altsuid = @suid
				where altsuid = @oldsuid

		print 'The dependent aliases were mapped to the new dbo.'
	end
end
/* else drop the aliases to the old dbo */
else
begin

	if exists (select *
			from sysalternates
				where altsuid = @oldsuid)
	begin
		delete from sysalternates
			where altsuid = @oldsuid

		print 'The dependent aliases were dropped.'
	end
end


/*
**  Reflect the new owner of the database in sysdatabases.
*/
update sysdatabases
	set suid = @suid
		where dbid = db_id()

commit transaction

/*
**  Checkpoint the database so that the in-memory data structure for the
**  database will be consistent with sysdatabases.
*/
checkpoint

print 'Database owner changed.'

return (0)
go


raiserror(15339,-1,-1,'sp_changegroup')
go
create procedure sp_changegroup
@grpname varchar(30),			/* group name */
@username varchar(30)			/* user name to add to group */
as

declare @gid int			/* group id */
declare @uid int			/* user id */

/*
**  You must be SA or the dbo to execute this sproc.
*/
if suser_id() <> 1 and user_id() <> 1
	begin
		raiserror(15000,-1,-1)
		return (1)
	end

/*
**  See if the group name exists.
*/
select @gid = uid from sysusers where name = @grpname
	and (uid > 16383 or uid = 0)

/*
**  If no such group, quit.
**  User ids are <= 16383 and group ids are > 16383 or = 0.
*/
if @gid is null
	begin
		raiserror(15014,-1,-1,@grpname)
		return (1)
	end

/*
**  See if the user name exists.
*/
select @uid = uid from sysusers where name = @username
	and (uid <= 16383 and uid > 0)

/*
**  If no such user in the database, quit.
*/
if @uid is null
	begin
		raiserror(15008,-1,-1,@username)
		return (1)
	end

/*
**  Everything is consistent so change the group.
*/
update sysusers set gid = @gid
	from sysusers where uid = @uid

/*
**  We need to invalidate the protection cache since objects have
**  changed ownership.  This command will invalidate the current
**  protection cache so when protections are checked the new and
**  correct protections will be used.
*/
grant all to null

print 'Group changed.'

return (0)
go


raiserror(15339,-1,-1,'sp_checknames')
go
create procedure sp_checknames
@mode varchar(20) = NULL		/* mode of operation; e.g. 'silent' */
as

declare @msilent	int		/* set to 1 if 'silent' mode is on */
declare @ret_val	int		/* set to 1 if we find funny char */
declare @codepoint	tinyint		/* set to 1 if we find funny char */
declare @dbname		varchar(30)	/* holds database name */
declare @msg		varchar(90)	/* used for messages to user */
declare @pat		varchar(132)	/* holds the pattern to search for */

set nocount on

if (@mode like '%help%')
begin
	print 'sp_checknames is used to search for non 7-bit ASCII characters'
	print 'in several important columns of system tables.  The following'
	print 'columns are searched:'
	print ' '
	print '    In ''master'':'
	print '        sysdatabases.name'
	print '        sysdevices.name'
	print '        syslogins.name'
	print '        syslogins.dbname'
	print '        sysremotelogins.remoteusername'
	print '        sysservers.srvname'
	print '        sysservers.srvnetname'
	print ' '
	print '    In all databases:'
	print '        syscolumns.name'
	print '        sysindexes.name'
	print '        sysobjects.name'
	print '        syssegments.name'
	print '        systypes.name'
	print '        sysusers.name'
	print ' '

	return (0)
end

/*
**  First, initialize return value, and set up mode variables:
*/
select @ret_val = 0

if (@mode like '%silent%')
	select @msilent = 1
else
	select @msilent = 0


/*
**  Now, initialize the pattern string we will search for:
*/
select @pat = '%[', @codepoint = 127
while (@codepoint < 255)
begin
	select @codepoint = @codepoint + 1
	select @pat = @pat + char(@codepoint)
end
select @pat = @pat + ']%'


/*
**  Get the database name we are in:
*/
select @dbname = db_name()

if (@msilent = 0)
begin
	print ' '
	select @msg = 'Looking for non 7-bit ASCII characters in the system ' +
			'tables of database:  ''' + @dbname + ''''
	print @msg
	print ' '
end


/*
**  Look through these only if in the master database:
*/
if (@dbname = 'master')
begin
    if exists (select name from sysdatabases
		    where name like @pat)
    begin
	if (@msilent = 1)
	    return (1)

	select @ret_val = 1
	print ' '
	print '==============================================================='
	print 'Table.Column name:  ''sysdatabases.name'''
	print ' '
	print 'The following database names contain non 7-bit ASCII characters.'
	print 'If you wish to change their names, use ''sp_renamedb'':'
	print ' '
	select dbid,name from sysdatabases
			where name like @pat
    end

    if exists (select name from sysdevices where name like @pat)
    begin
	if (@msilent = 1)
	    return (1)

	select @ret_val = 1
	print ' '
	print '==============================================================='
	print 'Table.Column name:  ''sysdevices.name'''
	print ' '
	print 'The following device names contain non 7-bit ASCII characters.'
	print 'If you wish to change their names, use ''UPDATE'':'
        print ' '
	select name from sysdevices where name like @pat
    end

    if exists (select name from syslogins where name like @pat)
    begin
	if (@msilent = 1)
	    return (1)

	select @ret_val = 1
	print ' '
	print '==============================================================='
	print 'Table.Column name:  ''syslogins.name'''
        print ' '
	print 'The following login names contain non 7-bit ASCII characters.'
	print 'If you wish to change these names, use ''sp_droplogin'' and'
	print '''sp_addlogin'':'
        print ' '
	select suid,name from syslogins
			where name like @pat
    end

    if exists (select dbname from master.dbo.syslogins
		    where dbname like @pat)
    begin
	if (@msilent = 1)
	    return (1)

	select @ret_val = 1
	print ' '
	print '==============================================================='
	print 'Table.Column name:  ''syslogins.dbname'''
	print ' '
	print 'The following logins have default database names that contain'
	print 'non 7-bit ASCII characters.  If you wish to change them use'
	print '''sp_defaultdb'':'
        print ' '
	select suid,name,dbname from master.dbo.syslogins
			where dbname like @pat
    end

    if exists (select remoteusername from master.dbo.sysremotelogins
		    where remoteusername like @pat)
    begin
	if (@msilent = 1)
	    return (1)

	select @ret_val = 1
	print ' '
	print '==============================================================='
	print 'Table.Column name:  ''sysremotelogins.remoteusername'''
        print ' '
	print 'The following remote login names contain non 7-bit ASCII'
	print 'characters.  If you wish to change these names, use'
	print '''sp_dropremotelogin'' and ''sp_addremotelogin'':'
        print ' '
	select remoteserverid,remoteusername from master.dbo.sysremotelogins
			where remoteusername like @pat
    end

    if exists (select srvname from master.dbo.sysservers
		    where srvname like @pat)
    begin
	if (@msilent = 1)
	    return (1)

	select @ret_val = 1
	print ' '
	print '==============================================================='
	print 'Table.Column name:  ''sysservers.srvname'''
        print ' '
	print 'The following server names contain non 7-bit ASCII characters.'
	print 'If you wish to change their names, use ''sp_dropserver'' and'
	print '''sp_addserver'':'
        print ' '
	select srvid,srvname from master.dbo.sysservers
			where srvname like @pat
    end

    if exists (select srvnetname from master.dbo.sysservers
		    where srvnetname like @pat)
    begin
	if (@msilent = 1)
	    return (1)

	select @ret_val = 1
	print ' '
	print '==============================================================='
	print 'Table.Column name:  ''sysservers.srvnetname'''
        print ' '
	print 'The following servers have ''initialization file'' names that contain'
	print 'non 7-bit ASCII characters.  If you wish to change these names,'
	print 'use ''UPDATE'':'
        print ' '
	select srvid,srvname,srvnetname from master.dbo.sysservers
			where srvnetname like @pat
    end

end


/*
**  For *ALL* databases, we want to look through these:
*/
if exists (select name from dbo.syscolumns
	    where name like @pat)
begin
    if (@msilent = 1)
	return (1)

    select @ret_val = 1
    print ' '
    print '==============================================================='
    print 'Table.Column name:  ''syscolumns.name'''
    print ' '
    print 'The following column and parameter names contain non 7-bit ASCII'
    print 'characters.  If you wish to change these names, use ''sp_rename'':'
    print ' '
    select objname=o.name,colname=c.name from dbo.syscolumns c, dbo.sysobjects o
		where c.name like @pat and o.id = c.id
end

if exists (select name from dbo.sysindexes
	    where name like @pat
	    	  and indid > 0)
begin
    if (@msilent = 1)
	return (1)

    select @ret_val = 1
    print ' '
    print '==============================================================='
    print 'Table.Column name:  ''sysindexes.name'''
    print ' '
    print 'The following index names contain non 7-bit ASCII characters.'
    print 'If you wish to change these names, use ''UPDATE'':'
    print ' '
    select id,indid,name from dbo.sysindexes
		where name like @pat
		and indid > 0
end

if exists (select name from dbo.sysobjects
	    where name like @pat)
begin
    if (@msilent = 1)
	return (1)

    select @ret_val = 1
    print ' '
    print '==============================================================='
    print 'Table.Column name:  ''sysobjects.name'''
    print ' '
    print 'The following object names contain non 7-bit ASCII characters.'
    print 'If you wish to change these names, use ''sp_rename'':'
    print ' '
    select owner=u.name,o.name from dbo.sysobjects o,dbo.sysusers u
		where o.name like @pat and o.uid=u.uid
end

if exists (select name from dbo.syssegments
	    where name like @pat)
begin
    if (@msilent = 1)
	return (1)

    select @ret_val = 1
    print ' '
    print '==============================================================='
    print 'Table.Column name:  ''syssegments.name'''
    print ' '
    print 'The following segment names contain non 7-bit ASCII characters.'
    print 'If you wish to change these names, use ''UPDATE'':'
    print ' '
    select segment,name from dbo.syssegments
		where name like @pat
end

if exists (select name from dbo.systypes
	    where name like @pat)
begin
    if (@msilent = 1)
	return (1)

    select @ret_val = 1
    print ' '
    print '==============================================================='
    print 'Table.Column name:  ''systypes.name'''
    print ' '
    print 'The following datatype names contain non 7-bit ASCII characters.'
    print 'If you wish to change these names, use ''sp_rename'':'
    print ' '
    select name from dbo.systypes
		where name like @pat
end

if exists (select name from dbo.sysusers
	    where name like @pat)
begin
    if (@msilent = 1)
	return (1)

    select @ret_val = 1
    print ' '
    print '==============================================================='
    print 'Table.Column name:  ''sysusers.name'''
    print ' '
    print 'The following user or group names contain non 7-bit ASCII'
    print 'characters.  If you wish to change these names, use ''UPDATE'':'
    print ' '
    select suid,uid,name from dbo.sysusers
		where name like @pat
end

if (@ret_val = 0  and  @msilent = 0)
begin
    select @msg = 'Good news!  Database ''' + @dbname + ''' has no obj/user/etc.'
    print @msg
    print 'names that contain non 7-bit ASCII characters.'
end

return (@ret_val)
go

dump transaction master with no_log
go

raiserror(15339,-1,-1,'sp_start_xact')
go
create procedure sp_start_xact
	@applname varchar(30),
	@xactname varchar(30),
	@count	  int,
	@password varchar(30) = NULL
AS

set nocount on

BEGIN
	BEGIN TRANSACTION
		INSERT spt_committab VALUES
		(
			0,
			getdate(),
			getdate(),
			@count,
			@count,
			'b',
			@applname,
			@xactname,
			@password
		)

		UPDATE spt_committab
			set commid = 1 + (select max(commid) from spt_committab)
			where commid = 0

		SELECT max(commid) from spt_committab
	COMMIT TRANSACTION
END
go


raiserror(15339,-1,-1,'sp_commit_xact')
go
create procedure sp_commit_xact
	@commid	int,
	@password varchar(30) = NULL
AS
if exists (select *
	from spt_committab
	where commid = @commid
	and password = @password)
begin
	/*
	**  If status is aborted, return a 1 to indicate a failure.
	*/
	if exists (select *
			from spt_committab
				where commid = @commid
					and status = 'a')
	begin
		select 1
		return (1)
	end

	/*  Mark the xact as committed. */
	UPDATE spt_committab
		SET status = 'c', lastchange = getdate()
		WHERE commid = @commid
	select 0
	return (0)
end
else
begin
	/* Error so return (1) */
	raiserror 30000 'Commit service xact id doesn''t exist.'
	select 1
	return (1)
end
go


raiserror(15339,-1,-1,'sp_abort_xact')
go
CREATE PROCEDURE sp_abort_xact
	@commid	int,
	@password varchar(30) = NULL
AS
if exists (select *
	from spt_committab
	where commid = @commid
	and password = @password)
begin
	UPDATE spt_committab
		SET status = 'a' , lastchange = getdate()
		WHERE commid = @commid
END
else
begin
	raiserror 30001 'Commit service xact id doesn''t exist.'
end
go


raiserror(15339,-1,-1,'sp_remove_xact')
go
CREATE PROCEDURE sp_remove_xact
	@commid	int,
	@count	int,
	@password varchar(30) = NULL
AS
if exists (select *
	from spt_committab
	where commid = @commid
	and password = @password)
begin
	/*
	** Decrement outnum by @count.
	*/
	UPDATE spt_committab
		SET outnum = outnum - @count, lastchange = GETDATE()
		WHERE commid = @commid

	/*
	**  Delete the row if outnum < 1
	*/
	DELETE spt_committab
		WHERE commid = @commid
			and outnum < 1

end
else
begin
	raiserror 30002 'Commit service xact id doesn''t exist.'
end
go


raiserror(15339,-1,-1,'sp_stat_xact')
go
create procedure sp_stat_xact
	@commid	int
AS
BEGIN
	IF EXISTS
	(
		SELECT status
			FROM spt_committab
			WHERE commid = @commid
	)
		SELECT status
			FROM spt_committab
			WHERE commid = @commid
	ELSE
		SELECT status = 'u'
END
go


raiserror(15339,-1,-1,'sp_probe_xact')
go
create procedure sp_probe_xact
	@commid	int
AS
BEGIN
	declare @stat	char(1)

	begin tran
		IF EXISTS ( SELECT status
				FROM spt_committab
					WHERE commid = @commid)
		begin
			SELECT @stat = status
				FROM spt_committab
					WHERE commid = @commid

			if (@stat <> 'c')
			begin
				update spt_committab
					set status = 'a'
						where commid = @commid
				select status = 'a'
			end
			else select status = @stat
		end
		else select status = 'u'
	commit tran

END
go


raiserror(15339,-1,-1,'sp_processinfo')
go
create procedure sp_processinfo
as
/* ====================================================*/
/* sp_processinfo - returns detailed lock              */
/* information for the server                          */
/* Microsoft SQL Server 	               */
/* ====================================================*/

declare @lkdbnm varchar(32),@lkdbid smallint,@lkobjid int,@UserDBID smallint,@usrdbnm varchar(32),@User_ID smallint, @grpid smallint, @stmt varchar(255)

/* ===========================================================*/
/* build temp table with processes and their associated locks */
/* ===========================================================*/

select
	'FullName'	= case when p.spid > 9 then lo.name else 'System' end,'UserID'= p.uid,
	'UName'	= '                                ','ProcessID'= p.spid,'Status'= p.status,
	'DBID'		= p.dbid,'DbName'= d.name,'Command'= p.cmd,'Host'= p.hostname,'Application'= p.program_name,
	'Blocking'	= case when l.type >= 256 then 100 else 0 end,'Blockedby'= p.blocked,'LockType'	= l.type,
	'LKObjDBID'	= l.dbid,'LKObjDB'	= '                                ','LKObjID'	= l.id,
	'LKObj'		= case when l.id = 0 then 
			(select name from master..spt_values where number = l.type and type = 'SFL')
			else '                                                                                     ' end,
	'LockedPage'	= l.page,'GroupID'	= p.gid,'GName'	= '                                ','CPUUsage'	= p.cpu,
	'PhysicalIO'	= p.physical_io,'HostProcess'	= p.hostprocess
into #info from master..sysprocesses p,	master..syslocks l,master..syslogins lo (nolock),master..sysdatabases d (nolock)
where	p.spid 	*= l.spid and p.suid = lo.suid and p.dbid= d.dbid

/* ======================= */
/* flag blocking processes */	
/* ======================= */

update #info set Blocking = 1 where ProcessID in (select Blockedby from #info where Blockedby > 0)

/* =============================================*/
/* get list of locked object IDs */
/* =============================================*/

exec ('declare c1 cursor for select distinct LockedDBName = d.name, LKObjID , LKObjDBID from #info, master..sysdatabases d
where LKObjDBID = d.dbid  and LKObjID > 0 FOR READ ONLY')

open c1

fetch c1 into @lkdbnm, @lkobjid, @lkdbid

while @@fetch_status >= 0
   begin
	/* set database.owner.name of locked objects in #info */
	select @stmt ='Update #info set LKObj=' + '''' + @lkdbnm + '''' + '+ ''.'' + u.name + ''.'' + o.name' + 
 	',LKObjDB=' + ''''+@lkdbnm + '''' + ' from '+@lkdbnm+'..sysobjects o,'+@lkdbnm+'..sysusers u ' +
	'where o.id='+convert(char(10),@lkobjid)+ 
	' and LKObjID=o.id' +  
	' and u.uid=o.uid and LKObjDBID=' + convert(char,@lkdbid)  
	exec(@stmt )
	/* next please */
	fetch c1 into @lkdbnm, @lkobjid, @lkdbid
   end
deallocate c1

/* =============================================*/
/* resolve UserID, GroupID for all processes    */
/* get a list of each database we need to visit */
/* =============================================*/

exec ('declare c1 cursor for select distinct DBID, DbName from #info FOR READ ONLY')

open c1

fetch c1 into  @UserDBID, @usrdbnm

while @@fetch_status >= 0
   begin
	/* set user/group in #info */
	select @stmt = 'Update #info set GName=g.name,UName=u.name from '+@usrdbnm+'..sysusers u,'+
	@usrdbnm+'..sysusers g,#info '+	'where u.uid=UserID and g.uid=GroupID and DBID=' + convert(char(10),@UserDBID)
	exec(@stmt )
	/* next please */
	fetch c1 into  @UserDBID, @usrdbnm
   end
deallocate c1

/* ====== return the #info table ===== */
SELECT
	FullName , ProcessID, 	Status,	DName = DbName,	Command,Host, Application, Blockedby,LockType,LKObj,
	GName = GName,CPUUsage, PhysicalIO, HostProcess,UName,Blocking
FROM #info (nolock) ORDER BY FullName, ProcessID

return(0)
GO

raiserror(15339,-1,-1,'sp_scan_xact')
go
create procedure sp_scan_xact
	@commid	int = NULL
as
BEGIN
	IF	@commid = -1 or @commid is NULL

		SELECT commid,start,lastchange,totnum,outnum,
		       status,applname,xactname
		FROM spt_committab
	ELSE
		SELECT commid,start,lastchange,totnum,outnum,
		       status,applname,xactname
		FROM spt_committab
		WHERE @commid = commid
END
go


raiserror(15339,-1,-1,'sp_commonkey')
go
create procedure sp_commonkey
@tabaname  varchar(92),		/* name of first table in the key */
@tabbname varchar(92),		/* name of second table in the key */
@col1a     varchar(30), 	/* first column name of first table */
@col1b     varchar(30), 	/* first column name of second table */
@col2a     varchar(30) = NULL,
@col2b     varchar(30) = NULL,
@col3a     varchar(30) = NULL,
@col3b     varchar(30) = NULL,
@col4a     varchar(30) = NULL,
@col4b     varchar(30) = NULL,
@col5a     varchar(30) = NULL,
@col5b     varchar(30) = NULL,
@col6a     varchar(30) = NULL,
@col6b     varchar(30) = NULL,
@col7a     varchar(30) = NULL,
@col7b     varchar(30) = NULL,
@col8a     varchar(30) = NULL,
@col8b     varchar(30) = NULL
as

declare @objida int		/* id of table we are doing */
declare @objidb int		/* id of table with primary key */
declare @uida smallint		/* owner of the first table */
declare @cnt    int		/* how many columns are in the key */

declare @key1a int		/* colids of the first table */
declare @key2a int
declare @key3a int
declare @key4a int
declare @key5a int
declare @key6a int
declare @key7a int
declare @key8a int

declare @key1b int		/* colids of the second table */
declare @key2b int
declare @key3b int
declare @key4b int
declare @key5b int
declare @key6b int
declare @key7b int
declare @key8b int

/*
**  Check to see that the tabnames are local.
*/
if @tabaname like '%.%.%'
	and substring(@tabaname, 1, charindex('.', @tabaname) - 1) <> db_name()
	begin
		raiserror(15078,-1,-1)
		return (1)
	end

if @tabbname like '%.%.%'
	and substring(@tabbname, 1, charindex('.', @tabbname) - 1) <> db_name()
	begin
		raiserror(15078,-1,-1)
		return (1)
	end

/*
**  See if we can find the objects.  They must be system tables, user tables,
**  or views.  The low 4 bits of sysobjects.sysstat indicate what the
**  object type is -- it's more reliable than using sysobjects.type which
**  could change.
*/
select @objida = id, @uida = uid
	from sysobjects
		where id = object_id(@tabaname)
			and (sysstat & 0xf = 1		/* system table */
				or sysstat & 0xf = 2	/* view */
				or sysstat & 0xf = 3)	/* user table */
/*
**  If either of the tables don't exist, quit.
*/
if @objida is NULL
	begin
		raiserror(15112,-1,-1)
		return (1)
	end

select @objidb = id
	from sysobjects
		where id = object_id(@tabbname)
			and sysstat & 0xf in (1,2,3)

if @objidb is NULL
	begin
		raiserror(15113,-1,-1)
 		return(1)
	end

/*
**  Only the owner of the table can define its common keys.
*/
if @uida <> user_id()
	begin
		raiserror(15114,-1,-1)
		return (1)
	end

/*
**  Now check to see that the first key columns exist and have compatible types.
*/
select @cnt = 1, @key1a = a.colid, @key1b = b.colid
	from syscolumns a, syscolumns b, master.dbo.spt_values y,
		master.dbo.spt_values z
		where a.name = @col1a
			and a.id = @objida
			and b.name = @col1b
			and b.id = @objidb
			and y.type = 'J'
			and a.type = y.low
			and z.type = 'J'
			and b.type = z.low
			and y.number = z.number
if @key1a is null
	begin
		raiserror(15115,-1,-1)
		return (1)
	end

if @col2a IS NOT NULL
begin
	select @cnt = @cnt + 1, @key2a = a.colid, @key2b = b.colid
		from syscolumns a, syscolumns b, master.dbo.spt_values y,
			master.dbo.spt_values z
			where a.name = @col2a
				and a.id = @objida
				and b.name = @col2b
				and b.id = @objidb
				and y.type = 'J'
				and a.type = y.low
				and z.type = 'J'
				and b.type = z.low
				and y.number = z.number
	if @key2a is null
		begin
			raiserror(15116,-1,-1)
			return (1)
		end
end
else goto keys_ok

if @col3a IS NOT NULL
begin
	select @cnt = @cnt + 1, @key3a = a.colid, @key3b = b.colid
		from syscolumns a, syscolumns b, master.dbo.spt_values y,
			master.dbo.spt_values z
			where a.name = @col3a
				and a.id = @objida
				and b.name = @col3b
				and b.id = @objidb
				and y.type = 'J'
				and a.type = y.low
				and z.type = 'J'
				and b.type = z.low
				and y.number = z.number
	if @key3a is null
		begin
			raiserror(15117,-1,-1)
			return (1)
		end
end
else goto keys_ok

if @col4a IS NOT NULL
begin
	select @cnt = @cnt + 1, @key4a = a.colid, @key4b = b.colid
		from syscolumns a, syscolumns b, master.dbo.spt_values y,
			master.dbo.spt_values z
			where a.name = @col4a
				and a.id = @objida
				and b.name = @col4b
				and b.id = @objidb
				and y.type = 'J'
				and a.type = y.low
				and z.type = 'J'
				and b.type = z.low
				and y.number = z.number
	if @key4a is null
		begin
			raiserror(15118,-1,-1)
			return (1)
		end
end
else goto keys_ok

if @col5a IS NOT NULL
begin
	select @cnt = @cnt + 1, @key5a = a.colid, @key5b = b.colid
		from syscolumns a, syscolumns b, master.dbo.spt_values y,
			master.dbo.spt_values z
			where a.name = @col5a
				and a.id = @objida
				and b.name = @col5b
				and b.id = @objidb
				and y.type = 'J'
				and a.type = y.low
				and z.type = 'J'
				and b.type = z.low
				and y.number = z.number
	if @key5a is null
		begin
			raiserror(15119,-1,-1)
			return (1)
		end
end
else goto keys_ok

if @col6a IS NOT NULL
begin
	select @cnt = @cnt + 1, @key6a = a.colid, @key6b = b.colid
		from syscolumns a, syscolumns b, master.dbo.spt_values y,
			master.dbo.spt_values z
			where a.name = @col6a
				and a.id = @objida
				and b.name = @col6b
				and b.id = @objidb
				and y.type = 'J'
				and a.type = y.low
				and z.type = 'J'
				and b.type = z.low
				and y.number = z.number
	if @key6a is null
		begin
			raiserror(15120,-1,-1)
			return (1)
		end
end
else goto keys_ok

if @col7a IS NOT NULL
begin
	select @cnt = @cnt + 1, @key7a = a.colid, @key7b = b.colid
		from syscolumns a, syscolumns b, master.dbo.spt_values y,
			master.dbo.spt_values z
			where a.name = @col7a
				and a.id = @objida
				and b.name = @col7b
				and b.id = @objidb
				and y.type = 'J'
				and a.type = y.low
				and z.type = 'J'
				and b.type = z.low
				and y.number = z.number
	if @key7a is null
		begin
			raiserror(15121,-1,-1)
			return (1)
		end
end
else goto keys_ok

if @col8a IS NOT NULL
begin
	select @cnt = @cnt + 1, @key8a = a.colid, @key8b = b.colid
		from syscolumns a, syscolumns b, master.dbo.spt_values y,
			master.dbo.spt_values z
			where a.name = @col8a
				and a.id = @objida
				and b.name = @col8b
				and b.id = @objidb
				and y.type = 'J'
				and a.type = y.low
				and z.type = 'J'
				and b.type = z.low
				and y.number = z.number
	if @key8a is null
		begin
			raiserror(15122,-1,-1)
			return (1)
		end
end

/*
**  If we made it this far then all the columns for the common key are ok.
**  Everything is consistent so add the common key to syskeys.
*/
keys_ok:

insert syskeys (id, type, depid, keycnt, size, key1, key2, key3, key4, key5,
	key6, key7, key8, depkey1, depkey2, depkey3, depkey4, depkey5,
	depkey6, depkey7, depkey8)
values (@objida, 3, @objidb, @cnt, 0, @key1a, @key2a, @key3a, @key4a, @key5a,
	@key6a, @key7a, @key8a, @key1b, @key2b, @key3b, @key4b, @key5b,
	@key6b, @key7b, @key8b)

print 'New common key added.'

return (0)
go


raiserror(15339,-1,-1,'sp_configure')
go
CREATE PROCEDURE sp_configure  --1996/01/03 13:02

    @configname   varchar(35) = NULL   -- option name to configure
   ,@configvalue  int         = NULL   -- new configuration value
as

declare
    @confignum                int   --Num of the opt to be configured
   ,@configcount              int   --Num of options like @configname
-- ,@value_in_sysconfigures   int   --For one option, its value bef Upd
   ,@show_advance             int   --Enable/supress actions on "advanced" opts

/*
**  Determine @maxnumber based on advance option in syscurconfigs.
*/
if (select value from syscurconfigs where config = 518) = 1
   select @show_advance = 1   -- Display advanced options
else
   select @show_advance = 0   -- Don't display advanced options

/*
**  Make certain that max user info. reflects any addpak upgrades.
*/
if (select high from master.dbo.spt_values where number=103 and type='C')
   <> @@max_connections

   update master.dbo.spt_values
      set high = @@max_connections
      where number = 103
         and type='C'

/*
**  If no option name is given, the procedure will just print out all the
**  options and their values.
*/
if @configname is NULL
   begin
      select name, minimum = low, maximum = high,
         config_value = c.value,
         run_value = syscurconfigs.value
      from master.dbo.spt_values, sysconfigures c, syscurconfigs
      where type = 'C'
         and number = c.config
         and number = syscurconfigs.config
         and ((c.status/2) | @show_advance) & 1 = @show_advance
      order by lower(name)

      return (0)
   end

set nocount on

/*
**  Use @configname and try to find the right option.
**  If there isn't just one, print appropriate diagnostics and return.
*/
select @configcount = count(*)
   from master.dbo.spt_values v ,sysconfigures c
   where v.name like '%' + @configname + '%' and v.type = 'C'
      and v.number = c.config
      and ((c.status/2) | @show_advance) & 1 = @show_advance

/*
**  If no option, show the user what the options are.
*/
if @configcount = 0
   begin
      raiserror (15123,-1,-1,@configname)

      print ''
      print 'Valid configuration options are:'

      /*
      ** Show the user what the options are.
      */
      select name, minimum = low, maximum = high,
         config_value = c.value,
         run_value = syscurconfigs.value
      from master.dbo.spt_values, sysconfigures c, syscurconfigs
      where type = 'C'
         and number = c.config
         and number = syscurconfigs.config
         and ((c.status/2) | @show_advance) & 1 = @show_advance

      return (1)
   end

/*
**  If more than one option like @configname, show the duplicates and return.
*/
if @configcount > 1
   begin
      raiserror (15124,-1,-1,@configname)
      print ''

      select duplicate_options = name
      from master.dbo.spt_values,sysconfigures c
      where name like '%' + @configname + '%'
         and type = 'C'
         and number = c.config
         and ((c.status/2) | @show_advance) & 1 = @show_advance

      return (1)
   end
else
   /* There must be exactly one, so get the full name. */
   select @configname = name --,@value_in_sysconfigures = c.value
      from master.dbo.spt_values,sysconfigures c
      where name like '%' + @configname + '%' and type = 'C'
         and number = c.config
         and ((c.status/2) | @show_advance) & 1 = @show_advance

/*
** If @configvalue is NULL, just show the current state of the option.
*/
if @configvalue is null
begin
   select name, minimum = low, maximum = high,
      config_value = c.value,
      run_value = syscurconfigs.value
   from master.dbo.spt_values, sysconfigures c, syscurconfigs
   where type = 'C'
      and number *= c.config
      and number *= syscurconfigs.config
      and name like '%' + @configname + '%'
      and ((c.status/2) | @show_advance) & 1 = @show_advance
   return (0)
end

/*
**  Only the SA can execute the update part of this procedure so check.
*/
if suser_id() <> 1
   begin
      raiserror(15125,-1,-1)
      return (1)
   end

/*
**  Now get the configuration number.
*/
select @confignum = number
   from master.dbo.spt_values,sysconfigures c
   where type = 'C'
      and (@configvalue between low and high or @configvalue = 0)
      and name like '%' + @configname + '%'
      and number = c.config
      and ((c.status/2) | @show_advance) & 1 = @show_advance

/*
**  If this is the number of devices configuration parameter,
**  we want to make sure that it's not being set to lower than the
**  number of devices in sysdevices.
*/
if @confignum = 116
begin
   /*
   **  Get the max vdevno.
   */
   declare @maxvdevno int
   select @maxvdevno = max(convert(tinyint, substring(convert(binary(4),
      d.low), v.low, 1)))
   from sysdevices d, master.dbo.spt_values v
   where v.type = 'E'
      and v.number = 3

   /*
   **  If @configvalue = 0, then set it to the current number of devices.
   */
   if @configvalue = 0
   select @configvalue = @maxvdevno + 1
   if (select low from master.dbo.spt_values
         where type = 'C'
         and number = 116) > @configvalue
      begin
         select @configvalue = low
            from master.dbo.spt_values
            where type = 'C'
               and number = 116
      end

   if ((@maxvdevno + 1) > @configvalue)
      begin
         raiserror(15126,-1,-1)
         return (1)
      end
end

/*
**  If this is the number of default language, we want to make sure
**  that the new value is a valid language id in Syslanguages.
*/
if @confignum = 124
   begin
   if not exists (select * from syslanguages
         where langid = @configvalue)
      begin
         /* 0 is default language, us_english */
         if @configvalue <> 0
            begin
               raiserror(15127,-1,-1)
               return (1)
            end
      end
   end

/*
**  If this is the number of kernel language, we want to make sure
**  that the new value is a valid language id in Syslanguages.
*/
if @confignum = 132
   begin
   if not exists (select * from syslanguages
         where langid = @configvalue)
      begin
         /* 0 is default language, us_english */
         if @configvalue <> 0
            begin
               raiserror(15028,-1,-1)
               return (1)
            end
      end
   end

/*
**  "user options" should not try to set incompatible options/values.
*/
if @confignum = 1534  --"user options"
   begin

   if (@configvalue & (1024+2048) = (1024+2048)) --ansi_null_default_on/off
      begin
      raiserror(15303,-1,-1,@configvalue)
      return (1)
      end
   end

/*
**  Although the @configname is good, @configvalue wasn't in range.
*/
if @confignum is NULL
   begin
   raiserror(15129,-1,-1,@configvalue,@configname)
   return (1)
   end

--Msg 15002, but in 6.5 allow this inside a txn (not check @@trancount) #12828.

/*
**  Now update sysconfigures.
*/
update sysconfigures set value = @configvalue
   where config = @confignum

/*
** If this is memory parameter, we need to calculate a
** new value for number of free buffers.
*/
if @confignum = 104
   begin
   print ''
   print 'Recalculating free buffer setting.'
   update sysconfigures set value = @configvalue/20
      where config = 1515
   end

print 'Configuration option changed. Run the RECONFIGURE command to install.'

return (0)
go



raiserror(15339,-1,-1,'sp_dbinstall')
go
create procedure sp_dbinstall --1995/09/25 16:42

@dbname		varchar(30) = null,	-- name of db
@logical	varchar(30) = null,	-- logical name of device
@physical	varchar(255) = null,	-- physical name of device
@size		int = null,		-- size of device in Meg.
@dev_type	varchar(6) = null,	-- Device can be 'system' or 'data'.
@location	varchar(255) = null	-- Disk location for writable
					-- fragment.  Mandatory for
					-- system device, optional for
					-- data devices.					
as
        declare @vdevno int
	declare @dbid int
	declare @sysdb_version int
	declare @start int

	declare @filesize int
	declare @retcode int
	declare @lstart int
	declare @cmd varchar(255)
	declare @segmap int	

	/*
	** Make sure all parameters were supplied, if not print a syntax diagram.
	*/
	if	@dbname is null
		or @logical is null
		or @physical is null
		or @size is null
		or @dev_type is null
		begin
			raiserror(15130,-1,-1)
			return(1)
		end

	/* Check to see if a valid database name was supplied. */
	exec @retcode = sp_validname @dbname
	if @retcode <> 0
		return(1)

	/* Check to see if a valid device name was supplied. */
	exec @retcode = sp_validname @logical
	if @retcode <> 0
		return(1)

	select @dev_type=lower(@dev_type)

	/* Check to see if device type is valid. */
	if @dev_type not in ('system','data')
		begin
			raiserror(15048,-1,-1,@dev_type)
			return(1)
		end

	/*
        **  Check to see if we're defining the initial device or adding
	**  devices to complete an installation.
	*/

	if @dev_type = 'system'
		begin
			/* We're creating the db, so make sure it doesn't already exist. */
			if exists (select * from sysdatabases where name = @dbname)
				begin
					raiserror(15049,-1,-1)
					return(1)
				end

			/* make sure that new location is specified. */
			if @location is null
				begin
					raiserror(15052,-1,-1)
					return(1)
				end
		end
	else
		begin
			/*
			** We're appending data devices, so make sure db
			** already exists.
			*/
			if not exists (select * from sysdatabases where name = @dbname)
				begin
					raiserror(15050,-1,-1)
					return(1)
				end
		end

	/* Databases must occupy at least 1 meg on a device.
	** Have to trust the user for just how much.
	*/
	if @size < 1
		begin
			raiserror(15051,-1,-1)
			return(1)
		end

	/*
	** Make sure that devices with the supplied logical or physical
	** names don't already exist.
	*/
	if exists (select * from sysdevices where name = @logical)
		begin
			raiserror(15026,-1,-1,@logical)
			return(1)
		end


	if exists (select * from sysdevices where phyname = isnull(@location,' o k'))
		begin
			raiserror(15030,-1,-1,@location)
			return(1)
		end

	/* Get the dbid. */
	if @dev_type = 'system'	/* Initial install, need to calculate one. */
		begin
			/* Get first available database number. */

                        select @dbid = min(dbid)+1
			from sysdatabases d1
			where dbid < 32767	/* Avoid arithmetic overflow if
						** dbid = max smallint exists. */
			and not exists
				(select * from sysdatabases d2
				where d2.dbid = d1.dbid+1)
		end
	else	/* Appending device frags. to existing db., get its dbid. */
		select @dbid = dbid from sysdatabases where name = @dbname


	dbcc getvalue('current_version')  --internal/system schema
	select @sysdb_version = @@error


	/* Get first available device number. */
	select @vdevno = null
	select @vdevno = (min(low)/0x01000000)+1
		from sysdevices d1
		where low/0x01000000 between 0 and 254
		and not exists
			(select * from sysdevices d2
				where d2.low/0x01000000 = (d1.low/0x01000000)+1)

	if @vdevno is null
		begin
			raiserror(15054,-1,-1)
			return(1)
		end

	/* Calculate first virtual page number for device. */
	select  @start = 0x01000000 * @vdevno

	/* Get the file size. */
	exec('dbcc devcontrol('''+@physical+''',filesize) with no_infomsgs')
	select @filesize = @@error
	if @filesize = 0 /* => file can't be opened. */
		begin
			raiserror(15055,-1,-1,@physical)
			return(1)
		end

	if @location is not null
		begin
			select @cmd = 'copy '+@physical+' '+@location
			select @cmd = 'xp_cmdshell '''+@cmd+''''
			exec (@cmd)
			if @@error <> 0
				begin
					print 'Couldn''t move device file'
					return(1)
				end
			select @physical = @location
		end

	begin tran

		if @dev_type = 'system'		-- Don't mark system device as 'read only'
			insert sysdevices
				(low,high,status,cntrltype,name,phyname,mirrorname)
				values
				(@start, @start+@filesize-1, 8194, 0, @logical, @physical, NULL)
		else	-- It must be a data fragment, so mark the device 'read only'
			insert sysdevices
				(low,high,status,cntrltype,name,phyname,mirrorname)
				values
				(@start, @start+@filesize-1,12290, 0, @logical, @physical, NULL)

    		if @@error <> 0
    			begin
				raiserror(15056,-1,-1,'sysdevices')
		    		goto failed
		    	end

                if @dev_type = 'system'	-- New db.
                        begin
        	               select @lstart = 0

                               insert into sysdatabases
			        (name,dbid,suid,mode,status,version,logptr,crdate,dumptrdate,category)
			        values
    		                (@dbname, @dbid, 1, 0, 512, @sysdb_version, 0, getdate(), getdate(), 0 )

    		               if @@error <> 0
    			                begin
    				                raiserror(15056,-1,-1,'sysdatabases')
		    		                goto failed
                                        end

                        end
                else	/* We're appending, so get the last fragment and
        	        ** go from there. */
                        select @lstart = lstart+size from sysusages
                	where dbid=@dbid
        		and lstart = (select max(lstart) from sysusages
        				where dbid=@dbid)

		/*
		** If it's the data device, assume that it's static.
		** Else, it is a system device and we must map the
		** system and log segments to  it.
		*/
		if @dev_type = 'data'
			select @segmap = 0	/* No allocations. */
		else
			/* It's the system device */
			select @segmap = 5	/* System + log */

		insert sysusages
			(dbid,segmap,lstart,size,vstart)
			values
			(@dbid, @segmap, @lstart, @size*512, @start )
    		if @@error <> 0
    			begin
    				raiserror(15056,-1,-1,'sysusages')
		    		goto failed
		    	end

		dbcc dbrepair (@dbname,remap) with no_infomsgs

		/* If we made it to here, we're okay. */

		goto succeeded

		failed:
			begin
				rollback tran
				return(1)
			end

		succeeded:

	commit tran

	print ''
	if @dev_type = 'system'
		begin
	                print 'System device installed.'
			print 'Use sp_dbinstall to install data device(s).'
		end
	else
		begin
			print 'Data device installed.'
			print 'Use sp_dbinstall to install additional data device(s).  Or, if installation is complete, use sp_dboption to bring the database online.'
		end

return(0)
go


dump transaction master with no_log
go



raiserror(15339,-1,-1,'sp_dbremove')
go
create procedure sp_dbremove
@dbname varchar(30) = null,
@dropdev varchar(10) = null
as

	declare @dbid int
	declare @devname varchar(30)
	declare @pmsg varchar(255)
	declare @physname varchar(255)

	declare @cmd1 varchar(255)
	declare @cmd2 varchar(255)
	declare @cmd3 varchar(255)

	if @dbname is null
		begin
			raiserror(15131,-1,-1)
			return(1)
		end

	if lower(@dropdev) <> 'dropdev' and @dropdev is not null
		begin
			raiserror(15131,-1,-1)
			return(1)
		end

	/* Check to see if database exists. */
	select @dbid = null
	select @dbid = dbid from sysdatabases where name=@dbname
	if @dbid is null
		begin
			raiserror(15010,-1,-1,@dbname)
			return(1)
		end

	/* Make sure no one is in the db. */
	if (select count(*) from sysprocesses where dbid = @dbid) > 0
		begin
			raiserror(15069,-1,-1)
			return (1)
		end

	if lower(@dropdev) = 'dropdev'
		begin
			print 'Dropping any devices used only by this database.'

			select @cmd1 = 'declare c1 cursor for
				select distinct d.name from sysdevices d,
					master.dbo.sysusages u
				where u.dbid = '+convert(varchar(5),@dbid)

			select @cmd2 = ' and u.segmap >= 0
 	  			and u.vstart between d.low and d.high
 	  			and d.status & 2 = 2	/* Physical devices only. */
 	  			and not exists
 	  				(select * from master.dbo.sysusages u2
 	  					where u2.dbid <> u.dbid'
 	  		select @cmd3 =			' and u2.vstart between d.low and d.high)'
			exec(@cmd1+@cmd2+@cmd3)

			open c1
			fetch c1 into @devname

			if @@fetch_status < 0
				begin
					print 'This database shares all of its devices with other databases.'
					print '-- no devices will be dropped.'
				end

			while @@fetch_status >= 0
				begin
					select @pmsg = 'Dropping device: '+@devname
					/*
					** Get the physical file name before
					** delete the row from sysdevices.
					*/
					select @physname = phyname
						from sysdevices
						where name = @devname

					/*
					** Release file handle for physical device.
					*/

					dbcc devcontrol(@devname,offline)
                                                if @@error = 0
                                                        begin
					                        select @pmsg = 'File: '''+@physname+''' closed.'
                        					print @pmsg
                                                        end

					delete from sysdevices where name = @devname

					fetch c1 into @devname
				end

			exec('deallocate c1')
		end

	update sysdatabases set status = 256 where dbid=@dbid
	dbcc dbrepair(@dbname,dropdb,noinit)
	print 'Database removed.'

	return(0)
go


raiserror(15339,-1,-1,'sp_devcreate')
go
create procedure sp_devcreate
@dbname varchar(30),
@logical varchar(30),
@physical varchar(255),
@size int
as

declare @retcode int,
	@vdevno int,
	@exec_str varchar(255),
	@size_in_blocks int

exec @retcode = sp_validname @logical
if @retcode <> 0
	return(1)

if @physical is null
	begin
		raiserror (15267,-1,-1,@logical)
		return(1)
	end

if @size is null
	begin
		raiserror (15268,-1,-1,@logical)
		return(1)
	end

if @size < 1
	begin
		raiserror (15262,-1,-1)
		return(1)
	end

/* Get next available device number for this segment. */
select @vdevno = null
select @vdevno = (min(low)/0x01000000)+1
	from sysdevices d1
	where low/0x01000000 between 0 and 254
		and not exists
			(select * from sysdevices d2
			where d2.low/0x01000000 =
				(d1.low/0x01000000)+1)

if @vdevno is null or @@error <> 0
	begin
		raiserror(15054,-1,-1)
		exec sp_dbremove @dbname,dropdev
		return(1)
	end

select @size_in_blocks = @size * 512.

disk init
	name = @logical,
	physname = @physical,
	size = @size_in_blocks,
	vdevno = @vdevno

if @@error <> 0
	begin
		raiserror(15263,-1,-1,'data')
		exec sp_dbremove @dbname,dropdev
		return(1)
	end

select @exec_str = 'alter database '+@dbname+' on '+@logical+
			' = '+convert(varchar(5),@size)
exec(@exec_str)

if @@error <> 0
	begin
		raiserror(15264,-1,-1,'data')
		exec sp_dbremove @dbname,dropdev
		return(1)
	end

/* Set sysusages.segmap to 'data' for data fragment. */
update sysusages set segmap = 2
	where vstart =
		(select low from sysdevices where name = @logical)

if @@error <> 0
	begin
		raiserror(15265,-1,-1)
		exec sp_dbremove @dbname,dropdev
		return(1)
	end

dbcc dbrepair (@dbname,remap) with no_infomsgs
go



raiserror(15339,-1,-1,'sp_create_removable')
go
create procedure sp_create_removable

@dbname		varchar(30) = null,	/* name of db */
@syslogical	varchar(30) = null,	/* logical name of system device */
@sysphysical	varchar(255) = null,	/* physical name of system device */
@syssize	int = null,		/* size of sys device in Meg. */
@loglogical	varchar(30) = null,	/* logical name of log device */
@logphysical	varchar(255) = null,	/* physical name of log device */
@logsize	int = null,		/* size of log device in Meg. */
@datalogical1	varchar(30) = null,	/* logical name of data device */
@dataphysical1	varchar(255) = null,	/* physical name of data device */
@datasize1	int = null,		/* size of data device in Meg. */
@datalogical2	varchar(30) = null,	/* logical name of data device */
@dataphysical2	varchar(255) = null,	/* physical name of data device */
@datasize2	int = null,		/* size of data device in Meg. */
@datalogical3	varchar(30) = null,	/* logical name of data device */
@dataphysical3	varchar(255) = null,	/* physical name of data device */
@datasize3	int = null,		/* size of data device in Meg. */
@datalogical4	varchar(30) = null,	/* logical name of data device */
@dataphysical4	varchar(255) = null,	/* physical name of data device */
@datasize4	int = null,		/* size of data device in Meg. */
@datalogical5	varchar(30) = null,	/* logical name of data device */
@dataphysical5	varchar(255) = null,	/* physical name of data device */
@datasize5	int = null,		/* size of data device in Meg. */
@datalogical6	varchar(30) = null,	/* logical name of data device */
@dataphysical6	varchar(255) = null,	/* physical name of data device */
@datasize6	int = null,		/* size of data device in Meg. */
@datalogical7	varchar(30) = null,	/* logical name of data device */
@dataphysical7	varchar(255) = null,	/* physical name of data device */
@datasize7	int = null,		/* size of data device in Meg. */
@datalogical8	varchar(30) = null,	/* logical name of data device */
@dataphysical8	varchar(255) = null,	/* physical name of data device */
@datasize8	int = null,		/* size of data device in Meg. */
@datalogical9	varchar(30) = null,	/* logical name of data device */
@dataphysical9	varchar(255) = null,	/* physical name of data device */
@datasize9	int = null,		/* size of data device in Meg. */
@datalogical10	varchar(30) = null,	/* logical name of data device */
@dataphysical10	varchar(255) = null,	/* physical name of data device */
@datasize10	int = null,		/* size of data device in Meg. */
@datalogical11	varchar(30) = null,	/* logical name of data device */
@dataphysical11	varchar(255) = null,	/* physical name of data device */
@datasize11	int = null,		/* size of data device in Meg. */
@datalogical12	varchar(30) = null,	/* logical name of data device */
@dataphysical12	varchar(255) = null,	/* physical name of data device */
@datasize12	int = null,		/* size of data device in Meg. */
@datalogical13	varchar(30) = null,	/* logical name of data device */
@dataphysical13	varchar(255) = null,	/* physical name of data device */
@datasize13	int = null,		/* size of data device in Meg. */
@datalogical14	varchar(30) = null,	/* logical name of data device */
@dataphysical14	varchar(255) = null,	/* physical name of data device */
@datasize14	int = null,		/* size of data device in Meg. */
@datalogical15	varchar(30) = null,	/* logical name of data device */
@dataphysical15	varchar(255) = null,	/* physical name of data device */
@datasize15	int = null,		/* size of data device in Meg. */
@datalogical16	varchar(30) = null,	/* logical name of data device */
@dataphysical16 varchar(255) = null,	/* physical name of data device */
@datasize16	int = null		/* size of data device in Meg. */
	
as

declare @vdevno int,
	@size_in_blocks int,
	@retcode int,
	@exec_str varchar(255),
	@numdevs int

if suser_id() <> 1	-- Make sure that it's the SA executing this.
	begin
		raiserror(15003,-1,-1)
		return(1)	
	end

if @dbname is null
	or @syslogical is null
	or @sysphysical is null
	or @syssize is null
	or @loglogical is null
	or @logphysical is null
	or @logsize is null
	or @datalogical1 is null
	or @dataphysical1 is null
	or @datasize1 is null
		begin
			raiserror (15261,-1,-1)
			return (1)
		end

if exists (select * from sysdatabases where name = @dbname)
	begin
		raiserror(15032,-1,-1,@dbname)
		return(1)
	end

/* Check to verify that valid sizes were supplied for required devices. */
if @syssize < 1 or @logsize < 1 or @datasize1 < 1
	begin
		raiserror (15262,-1,-1)
		return(1)
	end

/* Check to see if a valid database name was supplied. */
exec @retcode = sp_validname @dbname
if @retcode <> 0
	return(1)

/* valid syslogical? */
exec @retcode = sp_validname @syslogical
if @retcode <> 0
	return(1)

/* valid loglogical? */
exec @retcode = sp_validname @loglogical
if @retcode <> 0
	return(1)

/* valid datalogical1? */
exec @retcode = sp_validname @datalogical1
if @retcode <> 0
	return(1)

/* Create the necessary devices. */

/* Get first available device number for the system device. */
select @vdevno = null
select @vdevno = (min(low)/0x01000000)+1
	from sysdevices d1
	where low/0x01000000 between 0 and 254
		and not exists
			(select * from sysdevices d2
			where d2.low/0x01000000 = (d1.low/0x01000000)+1)

if @vdevno is null or @@error <> 0
	begin
		raiserror(15054,-1,-1)
		return(1)
	end

select @size_in_blocks = @syssize * 512.

disk init
	name = @syslogical,
	physname = @sysphysical,
	size = @size_in_blocks,
	vdevno = @vdevno

if @@error <> 0
	begin
		raiserror(15263,-1,-1,'system')
		return(1)
	end

/* Create the database's system device segment. */
select @exec_str = 'create database '+ @dbname+' on '+@syslogical+
		' = '+convert(varchar(5),@syssize)
exec(@exec_str)

if @@error <> 0
	begin
		raiserror(15264,-1,-1,'system')
		return(1)
	end

/* Get next available device number for the first data segment. */
select @vdevno = null
select @vdevno = (min(low)/0x01000000)+1
	from sysdevices d1
	where low/0x01000000 between 0 and 254
		and not exists
			(select * from sysdevices d2
			where d2.low/0x01000000 = (d1.low/0x01000000)+1)

if @vdevno is null or @@error <> 0
	begin
		raiserror(15054,-1,-1)
		exec sp_dbremove @dbname,dropdev
		return(1)
	end

select @size_in_blocks = @datasize1 * 512.

disk init
	name = @datalogical1,
	physname = @dataphysical1,
	size = @size_in_blocks,
	vdevno = @vdevno

if @@error <> 0
	begin
		raiserror(15263,-1,-1,'data')
		exec sp_dbremove @dbname,dropdev
		return(1)
	end

select @exec_str = 'alter database '+@dbname+' on '+@datalogical1+
			' = '+convert(varchar(5),@datasize1)
exec(@exec_str)

if @@error <> 0
	begin
		raiserror(15264,-1,-1,'data')
		exec sp_dbremove @dbname,dropdev
		return(1)
	end

/* Set sysusages.segmap to 'data only' for data fragment. */
update sysusages set segmap = 2
	where vstart =
		(select low from sysdevices where name = @datalogical1)

if @@error <> 0
	begin
		raiserror(15265,-1,-1)
		exec sp_dbremove @dbname,dropdev
		return(1)
	end

dbcc dbrepair (@dbname,remap) with no_infomsgs

/* Check out optional data devices. */

if @datalogical2 is not null
	begin
		exec @retcode = sp_devcreate @dbname,@datalogical2,@dataphysical2,@datasize2
		if @retcode <> 0
			begin
				raiserror(15269,-1,-1,@datalogical2)
				exec sp_dbremove @dbname,dropdev
				return(1)
			end
		select @numdevs = 2
	end
else goto no_more_devs

if @datalogical3 is not null
	begin
		exec @retcode = sp_devcreate @dbname,@datalogical3,@dataphysical3,@datasize3
		if @retcode <> 0
			begin
				raiserror(15269,-1,-1,@datalogical3)
				exec sp_dbremove @dbname,dropdev
				return(1)
			end
		select @numdevs = @numdevs + 1
	end
else goto no_more_devs

if @datalogical4 is not null
	begin
		exec @retcode = sp_devcreate @dbname,@datalogical4,@dataphysical4,@datasize4
		if @retcode <> 0
			begin
				raiserror(15269,-1,-1,@datalogical4)
				exec sp_dbremove @dbname,dropdev
				return(1)
			end
		select @numdevs = @numdevs + 1
	end
else goto no_more_devs

if @datalogical5 is not null
	begin
		exec @retcode = sp_devcreate @dbname,@datalogical5,@dataphysical5,@datasize5
		if @retcode <> 0
			begin
				raiserror(15269,-1,-1,@datalogical5)
				exec sp_dbremove @dbname,dropdev
				return(1)
			end
		select @numdevs = @numdevs + 1
	end
else goto no_more_devs

if @datalogical6 is not null
	begin
		exec @retcode = sp_devcreate @dbname,@datalogical6,@dataphysical6,@datasize6
		if @retcode <> 0
			begin
				raiserror(15269,-1,-1,@datalogical6)
				exec sp_dbremove @dbname,dropdev
				return(1)
			end
		select @numdevs = @numdevs + 1
	end
else goto no_more_devs

if @datalogical7 is not null
	begin
		exec @retcode = sp_devcreate @dbname,@datalogical7,@dataphysical7,@datasize7
		if @retcode <> 0
			begin
				raiserror(15269,-1,-1,@datalogical7)
				exec sp_dbremove @dbname,dropdev
				return(1)
			end
		select @numdevs = @numdevs + 1
	end
else goto no_more_devs

if @datalogical8 is not null
	begin
		exec @retcode = sp_devcreate @dbname,@datalogical8,@dataphysical8,@datasize8
		if @retcode <> 0
			begin
				raiserror(15269,-1,-1,@datalogical8)
				exec sp_dbremove @dbname,dropdev
				return(1)
			end
		select @numdevs = @numdevs + 1
	end
else goto no_more_devs

if @datalogical9 is not null
	begin
		exec @retcode = sp_devcreate @dbname,@datalogical9,@dataphysical9,@datasize9
		if @retcode <> 0
			begin
				raiserror(15269,-1,-1,@datalogical9)
				exec sp_dbremove @dbname,dropdev
				return(1)
			end
		select @numdevs = @numdevs + 1
	end
else goto no_more_devs

if @datalogical10 is not null
	begin
		exec @retcode = sp_devcreate @dbname,@datalogical10,@dataphysical10,@datasize10
		if @retcode <> 0
			begin
				raiserror(15269,-1,-1,@datalogical10)
				exec sp_dbremove @dbname,dropdev
				return(1)
			end
		select @numdevs = @numdevs + 1
	end
else goto no_more_devs

if @datalogical11 is not null
	begin
		exec @retcode = sp_devcreate @dbname,@datalogical11,@dataphysical11,@datasize11
		if @retcode <> 0
			begin
				raiserror(15269,-1,-1,@datalogical11)
				exec sp_dbremove @dbname,dropdev
				return(1)
			end
		select @numdevs = @numdevs + 1
	end
else goto no_more_devs

if @datalogical12 is not null
	begin
		exec @retcode = sp_devcreate @dbname,@datalogical12,@dataphysical12,@datasize12
		if @retcode <> 0
			begin
				raiserror(15269,-1,-1,@datalogical12)
				exec sp_dbremove @dbname,dropdev
				return(1)
			end
		select @numdevs = @numdevs + 1
	end
else goto no_more_devs

if @datalogical13 is not null
	begin
		exec @retcode = sp_devcreate @dbname,@datalogical13,@dataphysical13,@datasize13
		if @retcode <> 0
			begin
				raiserror(15269,-1,-1,@datalogical13)
				exec sp_dbremove @dbname,dropdev
				return(1)
			end
		select @numdevs = @numdevs + 1
	end
else goto no_more_devs

if @datalogical14 is not null
	begin
		exec @retcode = sp_devcreate @dbname,@datalogical14,@dataphysical14,@datasize14
		if @retcode <> 0
			begin
				raiserror(15269,-1,-1,@datalogical14)
				exec sp_dbremove @dbname,dropdev
				return(1)
			end
		select @numdevs = @numdevs + 1
	end
else goto no_more_devs

if @datalogical15 is not null
	begin
		exec @retcode = sp_devcreate @dbname,@datalogical15,@dataphysical15,@datasize15
		if @retcode <> 0
			begin
				raiserror(15269,-1,-1,@datalogical15)
				exec sp_dbremove @dbname,dropdev
				return(1)
			end
		select @numdevs = @numdevs + 1
	end
else goto no_more_devs

if @datalogical16 is not null
	begin
		exec @retcode = sp_devcreate @dbname,@datalogical16,@dataphysical16,@datasize16
		if @retcode <> 0
			begin
				raiserror(15269,-1,-1,@datalogical16)
				exec sp_dbremove @dbname,dropdev
				return(1)
			end
		select @numdevs = @numdevs + 1
	end

no_more_devs:

/* Get next available device number for log segment. */
select @vdevno = null
select @vdevno = (min(low)/0x01000000)+1
	from sysdevices d1
	where low/0x01000000 between 0 and 254
		and not exists
			(select * from sysdevices d2
			where d2.low/0x01000000 = (d1.low/0x01000000)+1)

if @vdevno is null or @@error <> 0
	begin
		raiserror(15054,-1,-1)
		exec sp_dbremove @dbname,dropdev
		return(1)
	end

select @size_in_blocks = @logsize * 512.

disk init
	name = @loglogical,
	physname = @logphysical,
	size = @size_in_blocks,
	vdevno = @vdevno

if @@error <> 0
	begin
		raiserror(15263,-1,-1,'log')
		exec sp_dbremove @dbname,dropdev
		return(1)
	end

select @exec_str = 'alter database '+@dbname+' on '+@loglogical+
			' = '+convert(varchar(5),@logsize)
exec(@exec_str)

if @@error <> 0
	begin
		raiserror(15264,-1,-1,'log')
		exec sp_dbremove @dbname,dropdev
		return(1)
	end

/* Set sysusages.segmap to 'log' for log fragment. */
update sysusages set segmap = 4
	where vstart =
		(select low from sysdevices where name = @loglogical)

if @@error <> 0
	begin
		raiserror(15265,-1,-1)
		exec sp_dbremove @dbname,dropdev
		return(1)
	end

dbcc dbrepair (@dbname,remap) with no_infomsgs

/* Set sysusages.segmap to 'system' for system fragment. */
update sysusages set segmap = 1
	where vstart =
		(select low from sysdevices where name = @syslogical)

if @@error <> 0
	begin
		raiserror(15265,-1,-1)
		exec sp_dbremove @dbname,dropdev
		return(1)
	end

dbcc dbrepair (@dbname,remap) with no_infomsgs

return(0)

go


raiserror(15339,-1,-1,'sp_defaultdb')
go
create procedure sp_defaultdb
@loginame varchar(30),			/* login name of the user */
@defdb varchar(30) 			/* default db for the user */
as

declare @suid int		/* the suid to use to qualify the change */

/*
**  If we're in a transaction, disallow this since it might make recovery
**  impossible.
*/
if @@trancount > 0
	begin
		raiserror(15002,-1,-1,'sp_defaultdb')
		return (1)
	end

/*
**  Check that the account exists.
*/
if not exists (select * from syslogins where name = @loginame)
	begin
		raiserror(15007,-1,-1,@loginame)
		return (1)
	end

/*
**  Check that the database name is valid.
*/
if not exists (select * from sysdatabases where name = @defdb)
	begin
		raiserror(15010,-1,-1,@defdb)
		return (1)
	end

/*
**  Both the owner of the account (@loginame) and the SA can run this
**  command.  If the SA is running it, set @suid to the suid of @loginame.
*/
select @suid = suser_id()
if @suid = 1
	begin
		/*
		**  Get the suid for @loginame.  We know it exists because we
		**  checked above.
		*/
		select @suid = suid from syslogins where name = @loginame
	end

/*
**  Change the database
*/
update syslogins set dbname = @defdb
	where suid = @suid
	and name = @loginame

/*
**  If nothing got updated then it means that the the user tried to change
**  someone else's default.
*/
if @@rowcount < 1
	begin
		raiserror(15132,-1,-1)
		return (1)
	end

print 'Default database changed.'

return (0)
go


raiserror(15339,-1,-1,'sp_depends')
go
create procedure sp_depends  --1996/03/15 12:51
@objname varchar(92)		/* the object we want to check */
as

declare @objid int			/* the id of the object we want */
declare @found_some bit			/* flag for dependencies found */
declare @dbname varchar(30)

/*
**  Make sure the @objname is local to the current database.
*/
if @objname like '%.%.%' and
	substring(@objname, 1, charindex('.', @objname) - 1) <> db_name()
	begin
		raiserror(15250,-1,-1)
		return (1)
	end

/*
**  See if @objname exists.
*/
select @objid = object_id(@objname), @dbname=db_name()
if @objid is null
	begin
		raiserror(15009,-1,-1,@objname,@dbname)
		return (1)
	end

/*
**  Initialize @found_some to indicate that we haven't seen any dependencies.
*/
select @found_some = 0

set nocount on

/*
**  Print out the particulars about the local dependencies.
*/
if exists (select *
		from sysdepends
			where id = @objid)
begin
	print 'In the current database the specified object references the following:'
	select		 'name' = substring((s6.name + '.' + o1.name), 1, 40),
			 type = substring(v2.name, 1, 16),
			 updated = substring(u4.name, 1, 7),
			 selected = substring(w5.name, 1, 8)
		from	 sysobjects		o1
			,master.dbo.spt_values	v2
			,sysdepends		d3
			,master.dbo.spt_values	u4
			,master.dbo.spt_values	w5 --11667
			,sysusers		s6
		where	 o1.id = d3.depid
		and	 o1.sysstat & 0xf = v2.number and v2.type = 'O'
		and	 u4.type = 'B' and u4.number = d3.resultobj
		and	 w5.type = 'B' and w5.number = d3.readobj|d3.selall
		and	 d3.id = @objid
		and	 o1.uid = s6.uid

	select @found_some = 1
end

/*
**  Now check for things that depend on the object.
*/
if exists (select *
		from sysdepends
			where depid = @objid)
begin
	print 'In the current database the specified object is referenced by the following:'
	select distinct 'name' = substring((s.name + '.' + o.name), 1, 40),
		type = substring(v.name, 1, 16)
			from sysobjects o, master.dbo.spt_values v, sysdepends d,
				sysusers s
			where o.id = d.id
				and o.sysstat & 0xf = v.number and v.type = 'O'
				and d.depid = @objid
				and o.uid = s.uid
	select @found_some = 1
end

/*
**  Did we find anything in sysdepends?
*/
if @found_some = 0
	print 'Object doesn''t reference any object and no objects reference it.'

set nocount off

return (0)
go


raiserror(15339,-1,-1,'sp_diskdefault')
go
create procedure sp_diskdefault
@logicalname	varchar(30),		/* logical name of the device */
@defstatus	varchar(15)		/* turn on or off */
as

/*
**  If we're in a transaction, disallow this since it might make recovery
**  impossible.
*/
if @@trancount > 0
	begin
           raiserror(15002,-1,-1,'sp_diskdefault')
	   return (1)
	end

/*
**  Only the SA can run this sproc.
*/
if suser_id() <> 1
	begin
		raiserror(15003,-1,-1)
		return (1)
	end

/*
**  Make sure that a device with @logicalname exists.
*/
if not exists (select * from sysdevices where name = @logicalname)
	begin
		raiserror(15012,-1,-1,@logicalname)
		return (1)
	end

/*
**  Make sure that it is a database disk and not a dump device.
*/
if exists (select * from sysdevices
		where name = @logicalname
			and status & 16 = 16)
	begin
		raiserror(15035,-1,-1,@logicalname)
		return (1)
	end

/*
**  Make sure that the database disk is NOT a RAM device.
*/
if exists (select *
		from sysdevices
		where name = @logicalname
			and status & 2048 = 2048 )
	begin
		raiserror(15139,-1,-1)
		return (1)
	end

if @defstatus = 'defaulton'
	begin
		update sysdevices set status = status | 1
			where name = @logicalname
		return (0)
	end

if @defstatus = 'defaultoff'
	begin
		update sysdevices set status = status & ~1
			where name = @logicalname
		return (0)
	end

/*
**  @defstatus must be 'defaulton' or 'defaultoff'
*/
raiserror(15140,-1,-1)

return (1)
go

dump transaction master with no_log
go


raiserror(15339,-1,-1,'sp_dropalias')
go
create procedure sp_dropalias
@loginame varchar(30)		/* account name of the user with the alias */
as

/*
**  You must be SA or the dbo to execute this sproc.
*/
if suser_id() <> 1 and user_id() <> 1
begin
	raiserror(15000,-1,-1)
	return (1)
end

/*
**  Check to make sure that the @loginame has an account.
*/
if (select suser_id(@loginame)) is null
begin
	raiserror(15007,-1,-1,@loginame)
	return (1)
end

/*
**  Delete the alias, if any, from sysalternates.
*/
delete from sysalternates
	where suid = suser_id(@loginame)

/*
**  If nothing happened (rowcount = 0), there was no alias.
*/
if @@rowcount > 0
	begin
		print 'Alias user dropped.'
		return (0)
	end
else
	begin
		raiserror(15134,-1,-1)
		return (1)
	end
go


raiserror(15339,-1,-1,'sp_dropdevice')
go
create procedure sp_dropdevice
@logicalname	varchar(30),		-- logical name of the device
@delfile	varchar(7) = null	-- optional param. to delete disk file
as

declare @physname varchar(255)
declare @ctrltype smallint
declare @msg varchar(255)
declare @cmd varchar(255)

/*
** See if user specified something for @delfile and, if so, validate it.
*/
if @delfile is not null
	begin
		select @delfile = lower(@delfile)

		if @delfile <> 'delfile'
			begin
				raiserror(15216,-1,-1,@delfile)
				return(1)
			end
	end

/*
**  If we're in a transaction, disallow this since it might make recovery
**  impossible.
*/
if @@trancount > 0
	begin
		raiserror(15002,-1,-1,'sp_dropdevice')
		return (1)
	end

/*
**  Only the system administrator (SA) can run this command.
**  Check to make sure the executor is the sa.
*/
if suser_id() <> 1
	begin
		raiserror(15003,-1,-1)
		return (1)
	end

/*
**  Check and make sure that the device actually exists.
*/
if not exists (select * from sysdevices where name = @logicalname)
	begin
		raiserror(15012,-1,-1,@logicalname)
		return (1)
	end

/*
**  Check and make sure that no database is using the device.
**  If so, print out who is using it and exit.
*/
if exists (select *
	from sysdatabases s, sysusages u, sysdevices d,
		master.dbo.spt_values p
	where s.dbid = u.dbid
		and u.segmap >= 0
		and d.low <= u.size + u.vstart
		and d.high >= u.size + u.vstart - 1
		and d.status & 2 = 2
		and p.type = 'S'
		and d.name = @logicalname)
begin
	raiserror(15141,-1,-1)

	select distinct 'Device used by database(s):' = s.name
		from sysdatabases s, sysusages u, sysdevices d,
			master.dbo.spt_values p
		where s.dbid = u.dbid
			and u.segmap >= 0
			and d.low <= u.size + u.vstart
			and d.high >= u.size + u.vstart - 1
			and d.status & 2 = 2
			and p.type = 'S'
			and p.number >= 0
			and d.name = @logicalname
	order by s.name

	return (1)
end

select @physname = phyname,@ctrltype = cntrltype
		from sysdevices
		where name = @logicalname

if @ctrltype <> 6	-- Can't release file handle for 'pipe' device.
	begin
		/*
		** Release file handle for physical devices.
		*/
		dbcc devcontrol(@logicalname,offline)

		if @@error in (0,5154)	-- 5154 => Device is not open.
			begin
				select @msg = 'File: '''+@physname+''' closed.'
				print @msg

				/*
				** Drop the device.
				*/
				delete from sysdevices
					where name = @logicalname
				print 'Device dropped.'

				if @delfile = 'delfile'
					begin
						select @cmd = 'del '+@physname
						select @cmd = 'xp_cmdshell '''+@cmd+''''
						exec (@cmd)

						/* See if the delete was successful. */
						exec('dbcc devcontrol(''' +
							@physname +
							''',filesize) with no_infomsgs')
						if @@error <> 0 /* => file can't be opened. */
							begin
								print 'Couldn''t delete device file'
								return(1)
							end
						else
							print 'Physical file deleted.'
					end
			end
		else return(1)
	end

return (0)
go


raiserror(15339,-1,-1,'sp_dropextendedproc')
go
create procedure sp_dropextendedproc
@functname varchar(62) -- name of function  (30-->62 1996/01/31)
as
/*
**  If we're in a transaction, disallow the dropping of the
**  extended stored procedure.
*/
if @@trancount > 0
	begin
		raiserror(15002,-1,-1,'sp_dropextendedproc')
		return (1)
	end

/*
** Drop the extended procedure mapping.
*/
dbcc dropextendedproc( @functname )
return (0)
go


raiserror(15339,-1,-1,'sp_dropgroup')
go
create procedure sp_dropgroup
@grpname varchar(30)		/* group to be dropped */
as

declare @gid int		/* group id of the group to be dropped */

/*
**  You must be SA or the dbo to execute this sproc.
*/
if suser_id() <> 1 and user_id() <> 1
	begin
		raiserror(15000,-1,-1)
		return (1)
	end

/*
**  See if the group exists.
**  User ids are <= 16383 and group ids are > 16383.
*/
select @gid = (select uid
	from sysusers
		where name = @grpname
			and (uid > 16383 or uid = 0))

if @gid is null
	begin
		raiserror(15014,-1,-1,@grpname)
		return (1)
	end

/*
**  Can't drop the group public.
*/
if @gid = 0
	begin
		raiserror(15142,-1,-1)
		return (1)
	end

/*
**  Check to see that nobody is in the group.  If so, return.
*/
if (select count(*) from sysusers where gid = @gid and uid <= 16383) <> 0
	begin
		raiserror(15144,-1,-1)

		/*
		**  Show who is in the group.
		*/
		select name from sysusers
			where gid = @gid
			and uid <= 16383

		return (1)
	end

/*
**  Drop the group.
**  Also drop any references to the group in the sysprotects table.
*/
begin transaction

	delete from sysusers
		where uid = @gid

	delete from sysprotects
		where uid = @gid

commit transaction

print 'Group has been dropped.'

return (0)
go


raiserror(15339,-1,-1,'sp_dropkey')
go
create procedure sp_dropkey
@keytype varchar(10), 			/* type of key to drop */
@tabname varchar(92),			/* table with the key */
@deptabname varchar(92) = null		/* dependent table */
as

declare @id int				/* id of @tabname */
declare @uid smallint			/* owner of @tabname */
declare @depid int			/* id of related table */
declare @depuid smallint		/* owner of @related table */
declare @dbname varchar(30)

/*
**  First make sure that the key type is ok.
*/
if lower(@keytype) not in ('primary', 'foreign', 'common')
	begin
		raiserror(15145,-1,-1)
		return (1)
	end

/*
**  Check to see that the tabname is local.
*/
if @tabname like '%.%.%'
	and substring(@tabname, 1, charindex('.', @tabname) - 1) <> db_name()
	begin
		raiserror(15078,-1,-1)
		return (1)
	end

if @deptabname like '%.%.%'
	and substring(@deptabname, 1, charindex('.', @deptabname) - 1) <> db_name()
	begin
		raiserror(15078,-1,-1)
		return (1)
	end

/*
**  Get the ids of the @tabname and @deptabname.
*/
select @id = id, @depid = object_id(@deptabname), @uid = uid
		from sysobjects
			where id = object_id(@tabname)
if @id is null
	begin
		select @dbname = db_name()
		raiserror(15009,-1,-1,@tabname,@dbname)
		return (1)
	end

if @uid <> user_id()
	begin
		raiserror(15146,-1,-1)
		return (1)
	end

/*
**  If primary key, just drop it.
*/
if lower(@keytype) = 'primary'
	begin
		delete from syskeys where id = @id and type = 1

		if @@rowcount = 0
			begin
				raiserror(15147,-1,-1)
				return (1)
			end

		else
			print 'Primary key for the table or view dropped.'

		/*
		**  Check to see if there are any foreign keys dependent on the
		**  primary key.  If so -- drop them.
		*/
		delete from syskeys where depid = @id and type = 2

		if @@rowcount <> 0
			print 'Dependent foreign keys were also dropped.'

		return (0)
	end

/*
**  It's either a foreign or common key so we need to verify the
**  existence of the @deptabname.
*/
if @depid is null
	begin
		/*
		**  Was the @deptabname supplied?
		*/
		if @deptabname is null
			begin
				raiserror(15148,-1,-1)
				return (1)
			end

		/*
		**  It was supplied but it doesn't exist.
		*/
		raiserror(15081,-1,-1)
		return (1)
	end


/*
**  If foreign key, get rid of it.
*/
if lower(@keytype) = 'foreign'
	begin
		/*
		**  Get rid of the foreign key entry.
		*/
		delete from syskeys
			where type = 2
			and id = @id
			and depid = @depid

		if @@rowcount = 0
			begin
				raiserror(15149,-1,-1)
				return (1)
			end

		print 'Foreign key dropped.'

		return (0)
	end

/*
**  Key type must be common so just get rid of the common keys
**  with the right ids and depids.  Since whenever a common key is defined
**  it is added to both of the tables involved, we'll get rid of both of
**  those entries.
*/
delete from syskeys
	where type = 3
		and id = @id
		and depid = @depid

if @@rowcount = 0
	begin
		raiserror(15150,-1,-1)
		return (1)
	end

/*
**  Now get rid of the inverse common key entries.
*/
delete from syskeys
	where type = 3
		and id = @depid
		and depid = @id

print 'Common keys dropped.'

return (0)
go


raiserror(15339,-1,-1,'sp_droplogin')
go
create procedure sp_droplogin -- 1995/10/24 18:21

@loginame sysname		-- name of login to drop
as

set nocount on

declare @login_suid  smallint
       ,@char_login_suid  varchar(6)
declare @username sysname
declare @dbname	sysname

/*
**  If we're in a transaction, disallow this since it might make recovery
**  impossible.
*/
if @@trancount > 0
	begin
		raiserror(15002,-1,-1,'sp_droplogin')
		return (1)
	end

/*
**  Only the system administrator (SA) can run this command.
**  Check to make sure the executor is the sa.
*/
if suser_id() <> 1
	begin
		raiserror(15003,-1,-1)
		return (1)
	end

/*
**  If login exists, capture the corresponding suid.
*/
select @login_suid = min(suid)
   from master..syslogins
   where name = @loginame

if @login_suid is null
   begin
   raiserror(15007,10,-1,@loginame)
   return(1)
   end

select @char_login_suid = convert(varchar,@login_suid)

/*
**  This work table will hold all problems found.
*/
create table #db_list
(
	dbname sysname not null,
	user_name sysname null,
	map_type varchar (5) null
)

/*
**  Check to see if any databases are owned by the login.
*/

insert into #db_list (dbname)
	select name from sysdatabases where suid = @login_suid

if @@rowcount <> 0
	begin
		raiserror(15174,-1,-1,@loginame)
		select 'Databases owned by login:' = dbname from #db_list
		return(1)
	end

/*
**  Check to see if the login is mapped to a user in any database.
*/

declare db_name_cursor cursor for select name from sysdatabases
open db_name_cursor

fetch db_name_cursor into @dbname

while @@fetch_status >= 0
	begin
		exec ('use ' + @dbname + ' ' +
		      'insert into #db_list (dbname,user_name,map_type)
			select '''+@dbname+''', name, ''user''
			from sysusers
			where suid = ' + @char_login_suid + '
			union
			select '''+@dbname+''',u.name,''alias''	
			from sysusers u, sysalternates a
			where ' + @char_login_suid + ' = a.suid
				and a.altsuid = u.suid
			')

		--In 6.5 DB context auto reverts back, immediately after dynam exec().

		fetch db_name_cursor into @dbname
	end

if (select count(*) from #db_list) <> 0
	begin
		raiserror(15175,-1,-1,@loginame)

		select
			'Database name:' = dbname,
			'User name:' = user_name,
			'Mapping type:' = map_type
		from #db_list

		deallocate db_name_cursor

		return (1)
	end


deallocate db_name_cursor

/*
** Finally, check if the login owns tasks in msdb since we don't want the tasks to
** get inadvertently re-assigned when a new login is created with the same suid.
*/
if exists (select * from master.dbo.sysdatabases where name = 'msdb')
begin
	create table #retval (taskcount int not null)
	exec ('exec msdb.dbo.sp_MScheckforownedtasks ''#retval'', ''' + @loginame + '''')
	if exists (select taskcount from #retval where taskcount > 0)
	begin	
		declare @task_count int
		select @task_count = taskcount from #retval
		raiserror(14248, -1, -1, @task_count)
		return (1)
	end
end

/*
**  Delete the login, cascading to certain children first.
*/
delete from sysremotelogins where suid = @login_suid
delete from syslogins       where suid = @login_suid

/*
** Check @@rowcount when it works
*/
if @@rowcount > 0
	begin
		raiserror('Login dropped.',0,1)
		return (0)
	end
else
	begin
		raiserror(15007,10,-1,@loginame)
		return (1)
	end
go



raiserror(15339,-1,-1,'sp_dropmessage')
go
create procedure sp_dropmessage
@msgnum int = null,		-- Number of message to drop.
@lang varchar(30) = null	-- Language of message to drop (or 'ALL')
as
declare @retcode int
declare @langid smallint

/*
** If no message id, show usage
*/
if @msgnum is null
	begin
		raiserror(15177,-1,-1)
		return (1)
	end

/*
** Message id must be > 50000
*/
if @msgnum < 50001
	begin
		raiserror(15178,-1,-1)
		return(1)
	end

if (select count(*) from sysmessages where error=@msgnum) = 0
	begin
		raiserror(15179,-1,-1,@msgnum)
		return(1)
	end

/*
** Verify the language
*/
if @lang is null
	select @lang = @@language
if upper(@lang) <> 'ALL'
begin
	begin
		exec @retcode = sp_validlang @lang
		if @retcode <>  0
			return(1)
	end
	/*
	** Get langid from syslanguages; us_english won't exist, so use 0.
	*/
	select @langid = isnull((select langid from syslanguages where name = @lang),0)
end

/*
** The us_english version must be the last one to be dropped
*/
if (@langid = 0) and (select count(*) from sysmessages where error = @msgnum) > 1
begin
	raiserror(15280,-1,-1)
	return(1)
end

/*
**  Drop the message.
*/
if upper(@lang) = 'ALL'
	delete from sysmessages where error = @msgnum
else
	delete from sysmessages where error = @msgnum and langid = @langid

print 'Message dropped.'
return (0)
go


raiserror(15339,-1,-1,'sp_droptype')
go
create procedure sp_droptype
@typename varchar(30)			/* the user type to drop */
as

declare @typeid smallint		/* the typeid of the usertype to drop */

/*
**  Initialize @typeid so we can tell if we can't find it.
*/
select @typeid = 0

/*
**  Find the user type with @typename.  It must be a user type (usertype > 99)
**  and it must be owned by the person (or dbo) running the procedure.
*/
select @typeid = usertype
	from systypes
		where (uid = user_id() or user_id() = 1)
			and name = @typename
			and usertype > 99

if @typeid = 0
	begin
		raiserror(15105,-1,-1)
		return (1)
	end

/*
**  Check to see if the type is being used.  If it is, it can't be dropped.
*/
if exists (select * from syscolumns where usertype = @typeid)
	begin
		raiserror(15180,-1,-1)

		/*
		**  Show where it's being used.
		*/
		select object = o.name, type = o.type, owner = u.name,
			column = c.name, datatype = t.name
		from syscolumns c, systypes t, sysusers u, sysobjects o
		where c.usertype = @typeid
			and t.usertype = @typeid
			and o.uid = u.uid
			and c.id = o.id
		order by object, column

		return (1)
	end

/*
**  Everything is consistent so drop the type.
*/
delete from systypes where usertype = @typeid

print 'Type has been dropped.'

return (0)
go


raiserror(15339,-1,-1,'sp_dropuser')
go
create procedure sp_dropuser
@name_in_db varchar(30)			/* user name to drop */
as

declare @suid int				/* suid of the user */
declare @uid int				/* uid of the user */
declare @objectcount int		/* count of objects user owns */
declare @typecount int			/* count of types user owns */
declare	@userdropped int		/* flag to indicate user was dropped */
declare @privcount int                  /* count of occurrences as Grantor */

/*
**  You must be SA or the dbo to execute this sproc.
*/
if suser_id() <> 1 and user_id() <> 1
	begin
		raiserror(15000,-1,-1)
		return (1)
	end

/*
**  See if the user exists in the database.
**  User ids are <= 16383 and group ids are > 16383.
*/
select @uid = uid, @suid = suid
	from sysusers
		where name = @name_in_db
			and uid <= 16383 and uid > 0

/*
**  No such user so return.
*/
if @uid is null
	begin
		raiserror(15008,-1,-1,@name_in_db)
		return (1)
	end

/*
**  Can't drop database owner.
*/
if @uid = 1
	begin
		raiserror(15181,-1,-1)
		return (1)
	end

/*
**  Trouble if guest gets dropped from master or tempdb, so check.
*/
if (@name_in_db = 'guest' and (db_id() = 1 or db_id() = 2))
	begin
		raiserror(15182,-1,-1)
		return(1)
	end

/*
**  If the user owns any objects or datatypes or is a grantor,
**  and we're not forcing the drop, return without doing anything.
*/
select @objectcount = count(*)
	   from sysobjects
		   where uid = @uid
select @typecount = count(*)
	from systypes
		where uid = @uid
select @privcount = count(*)
	from sysprotects
	where grantor = @uid


if @objectcount > 0 or @typecount > 0 or @privcount > 0
begin
	if @objectcount > 0
		begin
			raiserror(15183,-1,-1)
			select name, type
				from sysobjects
					where uid = @uid
		end

	if @typecount > 0
		begin
			raiserror(15184,-1,-1)
			select user_type = a.name, physical_type = b.name
				from systypes a, systypes b
					where a.uid = @uid
						and a.type = b.type
						and b.usertype < 100
		end

	if @privcount > 0
		begin
			raiserror(15284,-1,-1)
			select 'Grantee'=user_name(uid) ,'Object'=object_name(id)
				from sysprotects
				where grantor = @uid
		end

	return (1)
end


/*
**  Drop the user.
**  Also drop any references to the user in the sysprotects table.
**  If anyone is aliased to the user, drop them also.
*/
begin transaction

	delete from sysusers
		where suid = @suid
	select @userdropped = @@rowcount

	delete from sysprotects
		where uid = @uid

	/*
	**  Drop any dependent aliases.
	*/
	if exists (select *
			from sysalternates
				where altsuid = @suid)
	begin
		delete from sysalternates
			where altsuid = @suid

		print 'The dependent aliases were also dropped.'
	end

if @userdropped = 1
	commit transaction
else rollback transaction

print 'User has been dropped from current database.'

return (0)
go

dump transaction master with no_log
go

raiserror(15339,-1,-1,'sp_dropremotelogin')
go
create procedure sp_dropremotelogin
@remoteserver	varchar(30),		/* name of remote server */
@loginame varchar(30) = NULL,		/* user's remote name */
@remotename varchar(30) = NULL		/* user's local user name */
as

declare @suid	smallint

/*
**  If we're in a transaction, disallow this since it might make recovery
**  impossible.
*/
if @@trancount > 0
	begin
		raiserror(15002,-1,-1,'sp_dropremotelogin')
		return (1)
	end

/*
**  If @loginame is NULL then we want to set @suid = -1. Otherwise get
**  it real value.
*/
if @loginame is null
	select @suid = -1
else
	select @suid = suser_id(@loginame)

/*
**  Delete the remote login.
*/
delete sysremotelogins
	from sysremotelogins l, sysservers s
	where l.remoteserverid = s.srvid
		and s.srvname = @remoteserver
		and l.remoteusername = @remotename
		and l.suid = @suid

/*
** Check @@rowcount when it works
*/
if @@rowcount > 0
	begin
		print 'Remote login dropped.'
		return (0)
	end
else
	begin
		raiserror(15185,-1,-1,@remotename,@loginame,@remoteserver)
		return (1)
	end
go


raiserror(15339,-1,-1,'sp_dropsegment')
go
create procedure sp_dropsegment
@segname	varchar(30),		/* segment name */
@device		varchar(30) = null	/* device name */
as

declare @dbuid	smallint	/* id of the owner of the database */
declare @segbit	int		/* this is the bit to turn on in sysusages */
declare	@msg	varchar(200)
declare @dbname varchar(30)

/*
**  If we're in a transaction, disallow this since it might make recovery
**  impossible.
*/
if @@trancount > 0
	begin
		raiserror(15002,-1,-1,'sp_dropsegment')
		return (1)
	end

/*
**  You must be SA or the dbo to execute this sproc.
*/
if suser_id() <> 1 and user_id() <> 1
	begin
		raiserror(15000,-1,-1)
		return (1)
	end

/*
**  There are two drop cases:
**	1) dropping the segment
**	2) dropping a segment reference to a device
**  The sproc does things differently depending on which we're doing.
*/

/*
**  Check to see if the segment exists.
*/
if not exists (select * from syssegments where name = @segname)
	begin
		raiserror(15018,-1,-1,@segname)
		return (1)
	end

/*
**  Get the segment number of @segname.
*/
select @segbit = segment
	from syssegments
		where name = @segname

/*
**  Now convert the segment number to the right bit for segmap.
*/
if (@segbit < 31)
	select @segbit = power(2, @segbit)
else
	/*
	**  Since this is segment 31, power(2, 31) will overflow
	**  since segmap is an int.  We'll grab the machine-dependent
	**  bit mask from spt_values to clear the right bit.
	*/
	select @segbit = low
		from master.dbo.spt_values
		where type = 'E'
			and number = 2

/*
**  Don't allow the segments 'default', 'system', or 'logsegment' to
**  be dropped completely.
*/
if @segname in ('system', 'default', 'logsegment')
	begin
		if @device is null
			begin
				raiserror(15186,-1,-1,@segname)
				return (1)
			end

		/*
		**  For the case where we are dropping the logsegment from a
		**  device, the logic below works OK.  If this is the last
		**  device for the the segment, it won't let the segment be
		**  dropped.
		*/
	end


/*
**  If we are dropping the segment, check to see that the segment isn't
**  being used by anyone.
*/
if @device is null
	begin
		/*
		**  Check to see if the segment is being used.
		*/
		if exists (select *
			from sysindexes i, syssegments s
				where s.name = @segname
					and i.segment = s.segment)
			begin
				raiserror(15187,-1,-1,@segname)
				return (1)
			end
	end

/*
**  If we are dropping the segment reference, check to see that arguments are
**  reasonable.
*/
if @device IS NOT NULL
begin
	/*
	**  Now see if the database even uses the @device
	*/
	if not exists (select *
			from sysusages u, sysdevices d
			where d.name = @device
				and u.vstart between d.low and d.high
				and u.dbid = db_id())
		begin
			select @dbname=db_name()
			raiserror(15073,-1,-1,@device,@dbname)
			return (1)
		end

	/*
	**  Now check that the segment references the device.
	*/
	if not exists (select *
			from sysusages u, sysdevices d
			where u.segmap & @segbit = @segbit
				and d.name = @device
				and d.low <= u.size + u.vstart
				and d.high >= u.size + u.vstart - 1
				and u.dbid = db_id()
				and d.status & 2 = 2)
		begin
			raiserror(15188,-1,-1,@segname,@device)
			return (1)
		end

	/*
	**  Now check to see if this is the last unique device reference for the
	**  segment.  If it is then we don't want to drop the reference.
	*/
	select distinct d.name into #temptable
		from sysusages u, sysdevices d
		where u.segmap & @segbit = @segbit
			and d.low <= u.size + u.vstart
			and d.high >= u.size + u.vstart - 1
			and u.dbid = db_id()
			and d.status & 2 = 2

	if (select count(*) from #temptable) <= 1
		begin
			raiserror(15189,-1,-1,@segname)
			return (1)
		end

	drop table #temptable
end

/*
**  If we're dropping the segment, go ahead drop the segment and
**  clear its bit in sysusages.
*/
if @device is null
begin
	/*
	**  NOTE: Don't update master..sysusages and syssegments as a xact since
	**  	it could cause problems for recovery.
	*/

	delete syssegments where name = @segname

	/*
	**  Now clear the segment from device in sysusages.
	*/
	update sysusages set segmap = segmap & (~@segbit)
		from sysusages u, sysdevices d
		where d.low <= u.size + u.vstart
			and d.high >= u.size + u.vstart - 1
			and u.dbid = db_id()
			and d.status & 2 = 2
end

/*
**  We want to just clear the segment reference from the device.
*/
else
begin
	update sysusages set segmap = segmap & (~@segbit)
		from sysusages u, sysdevices d
		where d.low <= u.size + u.vstart
			and d.high >= u.size + u.vstart - 1
			and u.dbid = db_id()
			and d.status & 2 = 2
			and d.name = @device

end

/*
**  Now we need to activate the new segment map.
*/
select @dbname = db_name()
dbcc dbrepair (@dbname, remap)

if @device is null
	print 'Segment dropped.'
else print 'Segment reference to device dropped.'

/*
**  If there are no more segment references to the device,
**  print a warning.
*/
if exists (select *
		from sysusages
			where segmap = 0
				and dbid = db_id())
begin
	declare @nosegs	varchar(255),	/* list of unreferenced devices */
		@curdevice varchar(30),
		@i int

	select @curdevice = min(d.name)
		from sysusages u, sysdevices d
		where d.low <= u.size + u.vstart
			and d.high >= u.size + u.vstart - 1
			and u.dbid = db_id()
			and d.status & 2 = 2
			and segmap = 0
	select @nosegs = @curdevice, @i = 1
	while (@curdevice IS NOT NULL)
	begin
		select @curdevice = min(d.name)
			from sysusages u, sysdevices d
			where d.low <= u.size + u.vstart
				and d.high >= u.size + u.vstart - 1
				and u.dbid = db_id()
				and d.status & 2 = 2
				and segmap = 0
				and d.name > @curdevice
		if @curdevice IS NOT NULL
		begin
			select @nosegs = @nosegs + ', ' + @curdevice
			select @i = @i + 1
		end
	end

	/*
	**  Get the device names.
	*/
	select @msg =  'WARNING: There are no longer any segments referencing '
	if @i > 1
		select @msg = @msg + 'devices '''
	else select @msg = @msg + 'device '''
	select @msg = @msg + @nosegs + '''. '
	if @i > 1
		select @msg = @msg + 'These devices '
	else select @msg = @msg + 'This device '
	select @msg = @msg + 'will no longer be used for space allocation.'
	print @msg
end
return (0)
go


raiserror(15339,-1,-1,'sp_dropserver')
go
create procedure sp_dropserver
@server	varchar(30),			/* server name */
@droplogins	char(10) = NULL		/* drop all related logins? */
as

declare	@msg varchar(200)
declare	@distributor varchar(30)
declare	@distribdb varchar(30)
declare	@distproc varchar(255)

/*
**  If we're in a transaction, disallow this since it might make recovery
**  impossible.
*/
if @@trancount > 0
	begin
		raiserror(15002,-1,-1,'sp_dropserver')
		return (1)
	end

/*
**  Check to see if the server exists.
*/
if not exists (select * from sysservers where srvname = @server)
	begin
		raiserror(15015,-1,-1,@server)
		return (1)
	end

/*
**  Check to see if there are any related logins in sysremotelogins.
*/
if @droplogins is null
	begin
		if exists (select * from sysremotelogins l,
				sysservers s
			where s.srvid = l.remoteserverid
				and s.srvname = @server)
			begin
				raiserror(15190,-1,-1,@server)
				return (1)
			end
	end

/*
**  If @droplogins is true then drop any associated logins
*/
else if lower(@droplogins) = 'droplogins'
begin
	begin transaction dropserver

		delete sysremotelogins
			from sysremotelogins l,
				sysservers s
			where s.srvid = l.remoteserverid
				and s.srvname = @server
		if @@rowcount > 0
		begin
			select @msg = 'Remote logins for remote server '''
				+ @server + ''' have been dropped.'
			print @msg
		end

		delete sysservers where srvname = @server

		print 'Server dropped.'

	commit transaction dropserver

	return (0)
end

/*
**  Bad argument to @droplogins.
*/
else
	begin
		raiserror(15191,-1,-1)
		return (1)
	end

/*
** Drop server information from distribution database MSsubscriber_info table
*/
exec sp_helpdistributor @distributor = @distributor OUTPUT,
   @distribdb = @distribdb OUTPUT

if @distributor is not null and @distribdb is not null
  begin
     select @distproc = RTRIM(@distributor) + '.' + RTRIM(@distribdb) +
        '..sp_MSdrop_subscriber_info '
     exec @distproc @@SERVERNAME, @server
  end

/*
**  Drop the server.
*/
delete sysservers where srvname = @server

print 'Server dropped.'

return (0)
go


raiserror(15339,-1,-1,'sp_extendsegment')
go
create procedure sp_extendsegment
@segname varchar(30),	/* segment name */
@devname varchar(30)	/* device name to put segment on */
as

declare @dbuid  smallint        /* id of the owner of the database */
declare @segbit	int		/* this is the bit to turn on in sysusages */
declare	@msg	char(100)
declare @dbname varchar(30)
declare @retcode	int

/*
**  If we're in a transaction, disallow this since it might make recovery
**  impossible.
*/
if @@trancount > 0
	begin
		raiserror(15002,-1,-1,'sp_extendsegment')
		return (1)
	end

/*
**  You must be SA or the dbo to execute this sproc.
*/
if suser_id() <> 1 and user_id() <> 1
	begin
		raiserror(15000,-1,-1)
		return (1)
	end

/*
**  Check to see if the segment exists.
*/
if not exists (select * from syssegments where name = @segname)
	begin
		raiserror(15018,-1,-1,@segname)
		return (1)
	end

/*
**  See if the device exists.
*/
if not exists (select * from sysdevices where name like @devname)
	begin
		raiserror(15012,-1,-1,@devname)
		return (1)
	end

/*
**  Now see if the @dbname uses the @devname
*/

if not exists (select * from sysusages u, sysdevices d
			where d.name = @devname
				and u.vstart between d.low and d.high
				and u.dbid = db_id())
	begin
		select @dbname=db_name()
		raiserror(15073,-1,-1,@devname,@dbname)
		return (1)
	end

/*
**  Check to see if the device is marked as a log device.
**  If so, print error.
*/
if exists (select * from sysusages u, sysdevices d
		where d.name = @devname
			and u.vstart between d.low and d.high
			and u.dbid = db_id()
			and segmap = 4)
	begin
		raiserror(15074,-1,-1,@devname)
		return (1)
	end

/*
** Check for valid identifier.
*/
execute @retcode = sp_validname @segname
if @retcode <> 0
	return (1)

/*
**  Now go ahead and define the new segment and add it to the segmap
**  of sysusages.
**  NOTE: Don't update master..sysusages and syssegments as a xact since
**  	it could cause problems for recovery.
*/

/*
**  Get the segment number for @segname.
*/
select @segbit = segment
	from syssegments
		where name = @segname

/*
**  If @segbit is 2 then it's the logsegment and the logsegment
**  doesn't share devices with other segments.  If this is the original
**  device, we must maintain a data segment since logical page 0
**  resides here, so exit.  If not, go ahead and make the change but
**  don't OR the @segbit in but just set segmap to 4.
*/
if @segbit = 2
begin
	/*
	**  First check to see if this is the original device
	*/
	if exists (select * from sysusages u, sysdevices d
			where d.name = @devname
				and u.vstart between d.low and d.high
				and u.dbid = db_id()
				and lstart = 0)
		begin
			raiserror(15192,-1,-1)
			return (1)
		end

	else
		update sysusages set segmap = 4
			from sysusages u, sysdevices d
			where d.name = @devname
				and u.vstart between d.low and d.high
				and u.dbid = db_id()
end

else
begin
	if (@segbit < 31)
		select @segbit = power(2, @segbit)
	else
		/*
		**  Since this is segment 31, power(2, 31) will
		**  overflow since segmap is an int.  We'll grab the
		**  machine-dependent bit mask from spt_values to set
		**  the right bit.
		*/
		select @segbit = low
			from master.dbo.spt_values
			where type = 'E'
				and number = 2
	/*
	**  Add the segment to @devname in sysusages.
	*/
	update sysusages set segmap = segmap | @segbit
			from sysusages u, sysdevices d
			where d.name = @devname
				and u.vstart between d.low and d.high
				and u.dbid = db_id()
end

/*
**  Now we need to activate the new segment map.
*/
select @dbname = db_name()
dbcc dbrepair (@dbname, remap)
print 'Segment extended.'

/*
**  Get the segment number for @segname.
*/
select @segbit = segment
	from syssegments
		where name = @segname

/*
**  If we are extending the logsegment, print out a message about what it
**  means to do so.
*/
if @segbit = 2
	begin
		select @msg = 'Device ''' + @devname
			+ ''' is now exclusively used by '''
			+ @segname + '''.'
		print @msg
	end

return (0)
go


raiserror(15339,-1,-1,'sp_fixindex')
go
create procedure sp_fixindex
		@dbname		varchar(30),
		@tabname	varchar(30),  		/* system table name */
		@indid		int					/* index id value    */
as
	/*
	**	Description:	allow the SA to force a drop and then a
	**			create index on system catalogs.
	**
	**	Usage:		sp_fixindex  database, systemcatalog, ind_id
	**
	**	Note:		before using this procedure the database has to
	**			be in single user mode. The sp_dboption has to
	**			be used for user databases, and update of
	**			sysdatabases for master.
	**
	*/

	declare	@ramboix smallint			/* mask for rambo indx*/
	declare @badlist  varchar(255)   /* name list of all tables that should
					      not have any indexes on them */
	/*
	**	Initialize name lists. These will be used in catching
	**	any attempts to fix indexes on fake tables or tables
	**	that have no indexes on them.
	*/
	select @badlist = 'syslocks, sysprocesses, syscurconfig,
			   syssegments, syslogckp'

	/*
	**	Make sure we are 'fixing' a system catalog.
	*/
	if @tabname NOT LIKE 'sys%'
		begin
			raiserror(15193,-1,-1)
			return (1)
		end

	/*
	**	Make sure that we are doing this on somenthing that
	**	has indexes (or real tables).
	*/
	if @badlist LIKE  '%' + @tabname + '%'
		begin
			raiserror(15194,-1,-1)
			return (1)
		end

	/*
	**	At this point we are sure that the table has an index
	**	of requested type, so all there is to be done is to
	**	set the ramboix bit for this object unless we try to
	**	fix Sysobjects.
	*/
	if @tabname <> 'sysobjects'
	begin
		/* Use decimal integer equivalent of hex 1000 to avoid
		** byte order issues.
		*/
		select @ramboix = 4096
		update sysobjects set sysstat = sysstat | @ramboix
		where name = @tabname

		checkpoint
	end

	/*
	**	Call dbcc to do the job.
	*/
	dbcc dbrepair(@dbname, REPAIRINDEX, @tabname, @indid)

	/*
	**	Processing successfully done, so turn off the ramboix
	**	bit if not sysobjects, and tell the user that all is fine.
	*/
	if @tabname <> 'sysobjects'
	begin
		update sysobjects set sysstat = sysstat & ~@ramboix
		where name = @tabname

		checkpoint
	end

	return (0)
go


raiserror(15339,-1,-1,'sp_foreignkey')
go
create procedure sp_foreignkey
@tabname  varchar(92),		/* name of table that we are doing */
@pktabname varchar(92),		/* name of table with primary key */
@col1     varchar(30), 		/* column name of table we are doing */
@col2     varchar(30) = NULL,
@col3     varchar(30) = NULL,
@col4     varchar(30) = NULL,
@col5     varchar(30) = NULL,
@col6     varchar(30) = NULL,
@col7     varchar(30) = NULL,
@col8     varchar(30) = NULL
as

declare @objida int	/* id of table we are doing */
declare @objidb int	/* id of table with primary key */
declare @uid smallint	/* id of owner of the table */
declare @cnt    int	/* how many columns are in foreign key */

declare @pkey1 int	/* colids of the foreign key */
declare @pkey2 int
declare @pkey3 int
declare @pkey4 int
declare @pkey5 int
declare @pkey6 int
declare @pkey7 int
declare @pkey8 int

declare @fkey1 int	/* colids of the primary key */
declare @fkey2 int
declare @fkey3 int
declare @fkey4 int
declare @fkey5 int
declare @fkey6 int
declare @fkey7 int
declare @fkey8 int

/*
**  Check to see that the tabname is local.
*/
if @tabname like '%.%.%'
	and substring(@tabname, 1, charindex('.', @tabname) - 1) <> db_name()
	begin
		raiserror(15078,-1,-1)
		return (1)
	end

if @pktabname like '%.%.%'
	and substring(@pktabname, 1, charindex('.', @pktabname) - 1) <> db_name()
	begin
		raiserror(15078,-1,-1)
		return (1)
	end

/*
**  See if we can find the objects.  They must be a system table, user table,
**  or view.  The low 4 bits of sysobjects.sysstat indicate what the
**  object type is -- it's more reliable than using sysobjects.type which
**  could change.
*/
select @objida = id, @uid = uid
	from sysobjects
		where id = object_id(@tabname)
			and (sysstat & 0xf = 1	/* system table */
				or sysstat & 0xf = 2	/* view */
				or sysstat & 0xf = 3)	/* user table */
/*
**  If either of the tables don't exist, quit.
*/
if @objida is NULL
	begin
		raiserror(15151,-1,-1)
		return (1)
	end

select @objidb = id
	from sysobjects
		where id = object_id(@pktabname)
			and (sysstat & 0xf = 1
				or sysstat & 0xf = 2
				or sysstat & 0xf = 3)
if @objidb is NULL
	begin
		raiserror(15152,-1,-1)
		return (1)
	end

/*
**  Only the owner of the table can define its foreign keys.
*/
if @uid <> user_id()
	begin
		raiserror(15153,-1,-1)
		return (1)
	end

/*
**  Now check to see that the foreign key columns exist.
*/
select @cnt = 1, @fkey1 = colid
	from syscolumns
		where name = @col1
			and id = @objida
if @fkey1 is null
	begin
		raiserror(15154,-1,-1)
		return (1)
	end

if @col2 IS NOT NULL
	begin
		select @cnt = @cnt + 1, @fkey2 = colid
			from syscolumns
			where name = @col2
			and id = @objida

		if @fkey2 is NULL
			begin
				raiserror(15155,-1,-1)
				return (1)
			end
	end
else goto foreign_ok

if @col3 IS NOT NULL
	begin
		select @cnt = @cnt + 1, @fkey3 = colid
			from syscolumns
			where name = @col3
			and id = @objida

		if @fkey3 is NULL
			begin
				raiserror(15156,-1,-1)
				return (1)
			end
	end
else goto foreign_ok

if @col4 IS NOT NULL
	begin
		select @cnt = @cnt + 1, @fkey4 = colid
			from syscolumns
			where name = @col4
			and id = @objida

		if @fkey4 is NULL
			begin
				raiserror(15157,-1,-1)
				return (1)
			end
	end
else goto foreign_ok

if @col5 IS NOT NULL
	begin
		select @cnt = @cnt + 1, @fkey5 = colid
			from syscolumns
			where name = @col5
			and id = @objida

		if @fkey5 is NULL
			begin
				raiserror(15158,-1,-1)
				return (1)
			end
	end
else goto foreign_ok

if @col6 IS NOT NULL
	begin
		select @cnt = @cnt + 1, @fkey6 = colid
			from syscolumns
			where name = @col6
			and id = @objida

		if @fkey6 is NULL
			begin
				raiserror(15159,-1,-1)
				return (1)
			end
	end
else goto foreign_ok

if @col7 IS NOT NULL
	begin
		select @cnt = @cnt + 1, @fkey7 = colid
			from syscolumns
			where name = @col7
			and id = @objida

		if @fkey7 is NULL
			begin
				raiserror(15160,-1,-1)
				return (1)
			end
	end
else goto foreign_ok

if @col8 IS NOT NULL
	begin
		select @cnt = @cnt + 1, @fkey8 = colid
			from syscolumns
			where name = @col8
			and id = @objida

		if @fkey8 is NULL
			begin
				raiserror(15161,-1,-1)
				return (1)
			end
	end


/*
**  If we made it this far then all the columns for the foreign key are ok.
*/
foreign_ok:

/*
**  Now let's check out the primary key that the foreign key is on.
**  There must be the same number of columns in the key and the
**  base types of the columns must agree.
*/
select @pkey1 = key1, @pkey2 = key2, @pkey3 = key3, @pkey4 = key4,
	@pkey5 = key5, @pkey6 = key6, @pkey7 = key7, @pkey8 = key8
		from syskeys
	where id = @objidb
		and type = 1
		and keycnt = @cnt

/*
**  If @pkey1 is null then there is no such primary key or the number of
**  columns in the primary key are not the same as the number of columns
**  in the foreign key.
*/
if @pkey1 is null
begin
	if exists (select * from syskeys where id = @objidb and type = 1)
		raiserror(15162,-1,-1)
	else
		raiserror(15163,-1,-1)

	return (1)
end

/*
**  Since we have the right number of columns in the foreign and primary keys,
**  check that their base datatypes agree.
*/
if not exists (select *
		from syscolumns a, syscolumns b, master.dbo.spt_values y,
			master.dbo.spt_values z
		where a.colid = @fkey1
			and a.id = @objida
			and b.colid = @pkey1
			and b.id = @objidb
			and y.type = 'J'
			and a.type = y.low
			and z.type = 'J'
			and b.type = z.low
			and y.number = z.number)
	begin
		raiserror(15164,-1,-1)
		return (1)
	end

if @fkey2 IS NOT NULL and not exists (select *
		from syscolumns a, syscolumns b, master.dbo.spt_values y,
			master.dbo.spt_values z
		where a.colid = @fkey2
			and a.id = @objida
			and b.colid = @pkey2
			and b.id = @objidb
			and y.type = 'J'
			and a.type = y.low
			and z.type = 'J'
			and b.type = z.low
			and y.number = z.number)
	begin
		raiserror(15165,-1,-1)
		return (1)
	end
else goto doinsert

if @fkey3 IS NOT NULL and not exists (select *
		from syscolumns a, syscolumns b, master.dbo.spt_values y,
			master.dbo.spt_values z
		where a.colid = @fkey3
			and a.id = @objida
			and b.colid = @pkey3
			and b.id = @objidb
			and y.type = 'J'
			and a.type = y.low
			and z.type = 'J'
			and b.type = z.low
			and y.number = z.number)
	begin
		raiserror(15166,-1,-1)
		return (1)
	end
else goto doinsert

if @fkey4 IS NOT NULL and not exists (select *
		from syscolumns a, syscolumns b, master.dbo.spt_values y,
			master.dbo.spt_values z
		where a.colid = @fkey4
			and a.id = @objida
			and b.colid = @pkey4
			and b.id = @objidb
			and y.type = 'J'
			and a.type = y.low
			and z.type = 'J'
			and b.type = z.low
			and y.number = z.number)
	begin
		raiserror(15167,-1,-1)
		return (1)
	end

if @fkey5 IS NOT NULL and not exists (select *
		from syscolumns a, syscolumns b, master.dbo.spt_values y,
			master.dbo.spt_values z
		where a.colid = @fkey5
			and a.id = @objida
			and b.colid = @pkey5
			and b.id = @objidb
			and y.type = 'J'
			and a.type = y.low
			and z.type = 'J'
			and b.type = z.low
			and y.number = z.number)
	begin
		raiserror(15168,-1,-1)
		return (1)
	end
else goto doinsert

if @fkey6 IS NOT NULL and not exists (select *
		from syscolumns a, syscolumns b, master.dbo.spt_values y,
			master.dbo.spt_values z
		where a.colid = @fkey6
			and a.id = @objida
			and b.colid = @pkey6
			and b.id = @objidb
			and y.type = 'J'
			and a.type = y.low
			and z.type = 'J'
			and b.type = z.low
			and y.number = z.number)
	begin
		raiserror(15169,-1,-1)
		return (1)
	end
else goto doinsert

if @fkey7 IS NOT NULL and not exists (select *
		from syscolumns a, syscolumns b, master.dbo.spt_values y,
			master.dbo.spt_values z
		where a.colid = @fkey7
			and a.id = @objida
			and b.colid = @pkey7
			and b.id = @objidb
			and y.type = 'J'
			and a.type = y.low
			and z.type = 'J'
			and b.type = z.low
			and y.number = z.number)
	begin
		raiserror(15170,-1,-1)
		return (1)
	end
else goto doinsert

if @fkey8 IS NOT NULL and not exists (select *
		from syscolumns a, syscolumns b, master.dbo.spt_values y,
			master.dbo.spt_values z
		where a.colid = @fkey8
			and a.id = @objida
			and b.colid = @pkey8
			and b.id = @objidb
			and y.type = 'J'
			and a.type = y.low
			and z.type = 'J'
			and b.type = z.low
			and y.number = z.number)
	begin
		raiserror(15171,-1,-1)
		return (1)
	end

/*
**  Everything is consistent so add the foreign key to syskeys.
*/
doinsert:

insert syskeys (id, type, depid, keycnt, size, key1, key2, key3, key4, key5,
	key6, key7, key8, depkey1, depkey2, depkey3, depkey4, depkey5,
	depkey6, depkey7, depkey8)
values (@objida, 2, @objidb, @cnt, 0, @fkey1, @fkey2, @fkey3, @fkey4, @fkey5,
	@fkey6, @fkey7, @fkey8, @pkey1, @pkey2, @pkey3, @pkey4, @pkey5,
	@pkey6, @pkey7, @pkey8)

print 'New foreign key added.'

return (0)
go



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

dump transaction master with no_log
go


raiserror(15339,-1,-1,'sp_helpdb')
go
create procedure sp_helpdb  --1995/12/20 15:34 #12755
@dbname varchar(30) = NULL			/* database name to change */
as

declare @showdev	bit
declare @allopts	int

set nocount on

/*
**  If no database name given, get 'em all.
*/
if @dbname is null
	select @dbname = '%', @showdev = 0
else select @showdev = 1

/*
**  See if the database exists
*/
if not exists (select * from sysdatabases where name like @dbname)
	begin
		raiserror(15010,-1,-1,@dbname)
		return (1)
	end

/*
** Get bitmap of all options that can be set by sp_dboption.
*/
select @allopts=number from master.dbo.spt_values
	where type = 'D' and name = 'ALL SETTABLE OPTIONS'

/*
**  Since we examine the status bits in sysdatabase and turn them
**  into english, we need a temporary table to build the descriptions.
*/
create table #spdbdesc
(
	dbid	smallint	null,
	dbdesc	varchar(175)	null
)

/*
**  Initialize #spdbdesc from sysdatabases
*/
insert into #spdbdesc (dbid)
		select dbid from sysdatabases
			where name like @dbname

/*
**  Now for each dbid in #spdbdesc, build the database status
**  description.
*/
declare @curdbid smallint	/* the one we're currently working on */
declare @dbdesc varchar(175)	/* the total description for the db */
declare @bitdesc varchar(35)	/* the bit description for the db */

/*
**  Set @curdbid to the first dbid.
*/
select @curdbid = min(dbid) from #spdbdesc

while @curdbid IS NOT NULL
begin
	/*
	**  Initialize @dbdesc.
	*/
	select @dbdesc = ''

	/*
	** First check bits in sysdatabases.status.
	*/

	/*
	**  Check select into/bulk copy bit (4)
	*/
	select @bitdesc = null
	select @bitdesc = v.name
		from master.dbo.spt_values v, sysdatabases d
		where d.dbid = @curdbid
			and v.type = 'D'
			and d.status & v.number = 4
			and v.number <> @allopts       /* all options */
	if @bitdesc IS NOT NULL
	begin
		if @dbdesc <> ''
			select @dbdesc = @dbdesc + ', ' +  @bitdesc
		else select @dbdesc = @bitdesc
	end

	/*
	**  Check truncate log on checkpoint bit (8)
	*/
	select @bitdesc = null
	select @bitdesc = v.name
		from master.dbo.spt_values v, sysdatabases d
		where d.dbid = @curdbid
			and v.type = 'D'
			and d.status & v.number = 8
			and v.number <> @allopts       /* all options */
	if @bitdesc IS NOT NULL
	begin
		if @dbdesc <> ''
			select @dbdesc = @dbdesc + ', ' +  @bitdesc
		else select @dbdesc = @bitdesc
	end

	/*
	**  Check no checkpoint on recovery bit (16)
	*/
	select @bitdesc = null
	select @bitdesc = v.name
		from master.dbo.spt_values v, sysdatabases d
		where d.dbid = @curdbid
			and v.type = 'D'
			and d.status & v.number = 16
			and v.number <> @allopts /* all options */
	if @bitdesc IS NOT NULL
	begin
		if @dbdesc <> ''
			select @dbdesc = @dbdesc + ', ' +  @bitdesc
		else select @dbdesc = @bitdesc
	end

	/*
	**  Check don't recover bit (32)
	*/
	select @bitdesc = null
	select @bitdesc = v.name
		from master.dbo.spt_values v, sysdatabases d
		where d.dbid = @curdbid
			and v.type = 'D'
			and d.status & v.number = 32
			and v.number <> @allopts       /* all options */
	if @bitdesc IS NOT NULL
	begin
		if @dbdesc <> ''
			select @dbdesc = @dbdesc + ', ' +  @bitdesc
		else select @dbdesc = @bitdesc
	end

	/*
	**  Check not recovered only bit (256)
	*/
	select @bitdesc = null
	select @bitdesc = v.name
		from master.dbo.spt_values v, sysdatabases d
		where d.dbid = @curdbid
			and v.type = 'D'
			and d.status & v.number = 256
			and v.number <> @allopts       /* all options */
	if @bitdesc IS NOT NULL
	begin
		if @dbdesc <> ''
			select @dbdesc = @dbdesc + ', ' +  @bitdesc
		else select @dbdesc = @bitdesc
	end

	/*
	**  Check offline bit (512)
	*/
	select @bitdesc = null
	select @bitdesc = v.name
		from master.dbo.spt_values v, sysdatabases d
		where d.dbid = @curdbid
			and v.type = 'D'
			and d.status & v.number = 512
			and v.number <> @allopts       /* all options */
	if @bitdesc IS NOT NULL
	begin
		if @dbdesc <> ''
			select @dbdesc = @dbdesc + ', ' +  @bitdesc
		else select @dbdesc = @bitdesc
	end

	/*
	**  Check read only bit (1024)
	*/
	select @bitdesc = null
	select @bitdesc = v.name
		from master.dbo.spt_values v, sysdatabases d
		where d.dbid = @curdbid
			and v.type = 'D'
			and d.status & v.number = 1024
			and v.number <> @allopts       /* all options */
	if @bitdesc IS NOT NULL
	begin
		if @dbdesc <> ''
			select @dbdesc = @dbdesc + ', ' +  @bitdesc
		else select @dbdesc = @bitdesc
	end

	/*
	**  Check dbo only bit (2048)
	*/
	select @bitdesc = null
	select @bitdesc = v.name
		from master.dbo.spt_values v, sysdatabases d
		where d.dbid = @curdbid
			and v.type = 'D'
			and d.status & v.number = 2048
			and v.number <> @allopts       /* all options */
	if @bitdesc IS NOT NULL
	begin
		if @dbdesc <> ''
			select @dbdesc = @dbdesc + ', ' +  @bitdesc
		else select @dbdesc = @bitdesc
	end

	/*
	**  Check single user bit (4096)
	*/
	select @bitdesc = null
	select @bitdesc = v.name
		from master.dbo.spt_values v, sysdatabases d
		where d.dbid = @curdbid
			and v.type = 'D'
			and d.status & v.number = 4096
			and v.number <> @allopts       /* all options */
	if @bitdesc IS NOT NULL
	begin
		if @dbdesc <> ''
			select @dbdesc = @dbdesc + ', ' +  @bitdesc
		else select @dbdesc = @bitdesc
	end

	/*
	**  Check ANSI NULL default bit (16384)
	*/
	select @bitdesc = null
	select @bitdesc = v.name
		from master.dbo.spt_values v, sysdatabases d
		where d.dbid = @curdbid
			and v.type = 'D'
			and d.status & v.number = 16384
			and v.number <> @allopts       /* all options */
	if @bitdesc IS NOT NULL
	begin
		if @dbdesc <> ''
			select @dbdesc = @dbdesc + ', ' +  @bitdesc
		else select @dbdesc = @bitdesc
	end

	/*
	** Now check bits in sysdatabases.category.
	*/

	/*
	**  Check published bit (65536)
	*/
	select @bitdesc = null
	select @bitdesc = v.name
		from master.dbo.spt_values v, sysdatabases d
		where d.dbid = @curdbid
			and v.type = 'D'
			and d.category & (v.number/0x10000) = 1
			and v.number <> @allopts       /* all options */
	if @bitdesc IS NOT NULL
	begin
		if @dbdesc <> ''
			select @dbdesc = @dbdesc + ', ' +  @bitdesc
		else select @dbdesc = @bitdesc
	end


	/*
	**  Check subscribed bit (131072)
	*/
	select @bitdesc = null
	select @bitdesc = v.name
		from master.dbo.spt_values v, sysdatabases d
		where d.dbid = @curdbid
			and v.type = 'D'
			and d.category & (v.number/0x10000) = 2
			and v.number <> @allopts       /* all options */
	if @bitdesc IS NOT NULL
	begin
		if @dbdesc <> ''
			select @dbdesc = @dbdesc + ', ' +  @bitdesc
		else select @dbdesc = @bitdesc
	end

	/*
	**  If no flags are set, say so.
	*/
	if @dbdesc = ''
		select @dbdesc = 'no options set'

	/*
	**  Save the description.
	*/
	update #spdbdesc set dbdesc = @dbdesc
		from #spdbdesc
			where dbid = @curdbid

	/*
	**  Now get the next, if any dbid.
	*/
	select @curdbid = min(dbid) from #spdbdesc where dbid > @curdbid
end

/*
**  Now #spdbdesc is complete so we can print out the db info
*/
select name = substring(d.name, 1, 24),
	db_size = str(sum(convert(dec(15),u.size))*
		(select low from master.dbo.spt_values
			where type = 'E' and number = 1)
			 / 1048576,10,2)+ ' MB',
	owner = substring(l.name, 1, 24),
	dbid = d.dbid,
	created = convert(char(11), d.crdate),
	status = s.dbdesc
from sysdatabases d, syslogins l, sysusages u, #spdbdesc s
where d.dbid = s.dbid
	and d.suid = l.suid
	and s.dbid = u.dbid
group by d.name,l.name,d.dbid,d.crdate,s.dbdesc
order by d.name

/*
**  If we are looking at one database, show its device allocation.
*/
if @showdev = 1
begin
	print ''
	select device_fragments = v.name, size =
		str((select low from master.dbo.spt_values
			where type = 'E'
				and number = 1)
			* convert(dec(15), u.size)/ 1048576,10,2) +' MB',
		usage = t.name
	from sysdatabases d, sysusages u, sysdevices v,
		master.dbo.spt_values t
	where d.dbid = u.dbid
		and u.vstart between v.low and v.high
		and v.status & 2 = 2
		and d.name = @dbname
		and t.type = 'S'
		and u.segmap & 7 = t.number
	order by u.lstart


	/*
	**  If there is only one database and we are in it, show the
	**  segments.
	*/
	if exists (select * from #spdbdesc where db_id() = dbid)
	begin
		declare @curdevice	varchar(30),
			@curseg		smallint,
			@segbit		int

		delete from #spdbdesc

		select @curdevice = min(d.name)
			from  sysusages u, sysdevices d
			where u.dbid = db_id()
				and u.vstart between d.low and d.high
				and d.status & 2 = 2

		while @curdevice IS NOT NULL
		begin
			/*
			** We need an inner loop here to go through
			**  all the possible segments.
			*/
			select @curseg = min(segment) from syssegments

			while (@curseg IS NOT NULL)
			begin
				if (@curseg < 31)
					select @segbit = power(2, @curseg)
				else select @segbit = low
					from master.dbo.spt_values
					where type = 'E'
						and number = 2
				insert into #spdbdesc
					select @curseg, @curdevice
						from sysusages u,
							sysdevices d
					where u.segmap & @segbit = @segbit
						and u.vstart between d.low and d.high
						and u.dbid = db_id()
						and d.status & 2 = 2
						and d.name = @curdevice
				select @curseg = min(segment)
					from syssegments
					where segment > @curseg
			end

			select @curdevice = min(d.name)
				from  sysusages u, sysdevices d
				where u.dbid = db_id()
					and u.vstart between d.low and d.high
					and d.status & 2 = 2
					and d.name > @curdevice
		end

		/*
		**  One last check for any devices that have no segments.
		*/
		insert into #spdbdesc
			select null, d.name
			from sysusages u, sysdevices d
			where u.segmap = 0
				and u.vstart between d.low and d.high
				and u.dbid = db_id()
				and d.status & 2 = 2

		print ''

		select distinct device = substring(dbdesc, 1, 30),
			segment = isnull(name, ' -- unused by any segments --')
		from #spdbdesc, syssegments
			where dbid *= segment
		order by 1, 2
	end

end
return (0)
go



raiserror(15339,-1,-1,'sp_help_revdatabase')
go
CREATE PROCEDURE sp_help_revdatabase  --1995/12/19 13:54

       @DBNamePattern     varchar (44)   = '%'   -- LIKE pattern
AS

/****************************************************************
      This SP reverse generates CREATE & ALTER DB statements.
The output statements are in the sequence necessary to facilitate
the LOADing of a DUMP DATABASE output file onto a different
DB and possibly a different SV too.
      Perhaps sp_coalesce_fragments could be safely incorporated,
but for now we are keeping it simple.
****************************************************************/

SET nocount on

-----------------  declares  ------------------

DECLARE
       @vc80a                 varchar (80)
      ,@vc255a                varchar (255)
      ,@int1                  int
      ,@NowDateTm             datetime
      ,@CountDBs              int
      ,@Loop22Count           int
      ,@G_O                   varchar (2)
      ,@TrueFalse             varchar (5)
      ,@CommentThisDBOption   char (1)
      ,@EOLWinNT              varchar (2)
      ,@OrigDBName            varchar (30)

DECLARE
       @cu33OptName           varchar (35)
      ,@cu33OptNumber         int

DECLARE
       @cu11DBName            varchar (30)
      ,@cu11DBId              smallint
      ,@cu11DBStatus          smallint
      ,@cu11DBSUID            smallint

DECLARE
       @cu22DBName            varchar (30)
      ,@cu22DBId              smallint
      ,@cu22DVName            varchar (30)
      ,@cu22FragSizePages     int
      ,@cu22FragSizeMb        varchar (15)
      ,@cu22SegMap            int
      ,@cu22LStart            int

DECLARE
       @LoginName             varchar (30)
      ,@LoginDBName           varchar (30)
      ,@LoginLanguage         varchar (30)

DECLARE
       @SPTValAllSettableOptions    int
      ,@SPTValDataAndLog            int
      ,@SPTValPageSize              int
      ,@SPTValLogOnly               int
      ,@cSPTValAllSettableOptions   varchar (15)
      ,@cSPTValDataAndLog           varchar (15)
      ,@cSPTValPageSize             varchar (15)
      ,@cSPTValLogOnly              varchar (15)
      ,@DefaultSortOrderId          int
      ,@OneMb                       numeric(9,1)
      ,@cOneMb                      varchar (15)


----------------  initial values  -------------------

Select
       @NowDateTm       = Getdate()
      ,@OneMb           = 1048576.0
      ,@G_O             = 'g' + 'o'
      ,@CountDBs        = 0            -- RETURNed
      ,@EOLWinNT        = 0x0D0A
      ,@OrigDBName      = Db_name()


Select
       @cOneMb          = Convert(varchar,@OneMb)


IF NOT EXISTS (SELECT * From master.dbo.sysdatabases
                 Where name LIKE @DBNamePattern)
   begin
   RaisError(15329,-1,-1)
   GOTO LABEL_86RETURN
   end


/*********************************************************************
 *
 *                Initialization
 *
 ********************************************************************/


Print  @EOLWinNT


----------------  code page and sort order config  ---------------


SELECT
             @DefaultSortOrderId    = c.value
      From
             master..syscurconfigs  c
      Where
             lower(c.comment)       = 'default sortorder id'


---------------------  Selects, cursor for spt_values  -----------


SELECT
             @SPTValAllSettableOptions    = v.number
            ,@cSPTValAllSettableOptions   = Convert(varchar,v.number)
      From
             master..spt_values     v
      Where
             v.type                 = 'D'
      And    v.name                 = 'ALL SETTABLE OPTIONS'


SELECT
             @SPTValPageSize        = v.low
            ,@cSPTValPageSize       = Convert(varchar,v.low)
      From
             master..spt_values     v
      Where
             v.type                 = 'E'
      And    v.name                 = 'WINDOWS/NT'


SELECT
             @SPTValDataAndLog      = v.number
            ,@cSPTValDataAndLog     = Convert(varchar,v.number)
      From
             master..spt_values     v
      Where
             v.type                 = 'S'
      And    v.name                 = 'data and log'


SELECT
             @SPTValLogOnly         = v.number
            ,@cSPTValLogOnly        = Convert(varchar,v.number)
      From
             master..spt_values     v
      Where
             v.type                 = 'S'
      And    v.name                 = 'log only'


--  cursor


EXECUTE(
'
DECLARE cursor_33SPTValDBOpts
   Insensitive
   Cursor For
   SELECT
             v.name
            ,v.number
      From
             master..spt_values   v
'+'
      Where
             v.type    = ''D''
      And    v.number <> ' + @cSPTValAllSettableOptions + '
      And    v.number >  0
      And    v.number  & ' + @cSPTValAllSettableOptions + ' = Abs(v.number)
      Order By
             v.name

   For Read Only
'
)


--------------------  Cursor for DB names  -------------------

EXECUTE(
'
DECLARE cursor_11DBNames
   Insensitive
   Cursor For
'+'
   SELECT
             d.name
            ,d.dbid
            ,d.status
            ,d.suid
      From
             master..sysdatabases   d
'+'
      Where
             d.name        LIKE ''' + @DBNamePattern + '''
      Order By
             d.name

   For Read Only
'
)




/****************************************************************
 *
 *                MainLine
 *
 ***************************************************************/



OPEN cursor_11DBNames


----------------------------------------------------------------
--              LOOP 11:  thru DBs
----------------------------------------------------------------


WHILE (11=11)
   Begin


   FETCH
             Next
      From
             cursor_11DBNames
      Into
             @cu11DBName
            ,@cu11DBId
            ,@cu11DBStatus
            ,@cu11DBSUID


   IF (@@fetch_status < 0)
      begin
      CLOSE cursor_11DBNames

      BREAK
      end


   Select @CountDBs = @CountDBs + 1



----------------------  Print the headers  ----------------------

   Print  @EOLWinNT
   Raiserror('%s%s',0,1,'/','********1*********2*********3*********4*********5*********6**')

   Select @vc80a = 'Reverse generated at '
            +   Substring(convert(varchar,@NowDateTm,111),1 ,10) + ' '
            +   Substring(convert(varchar,@NowDateTm,113),12,13)
   Print  @vc80a

   Print  @EOLWinNT
   Print  'Server / Database / Default sortorder ID :'

   Select @vc80a = @@servername + ' / '
            +   @cu11DBName + ' / '
            +   Convert(varchar,@DefaultSortOrderId)
   Print  @vc80a
   Print  ' '

   SELECT
		 'DBName'   =db_name(ug.dbid)
		,'FromLPage'=ug.lstart
		,'ToLPage'  =ug.lstart + ug.size - 1
		,ug.segmap
	from
		 master.dbo.sysusages ug
	where
		 ug.dbid = db_id(@cu11DBName)
	order by
		 DBName ,FromLPage

   Print  ' '
   Print  @EOLWinNT
   Select @vc255a = @@version
   Select @int1 = CharIndex(char(10),@vc255a)
   Select @vc255a = '@@version:  '  +  Substring(@vc255a ,1 ,@int1 - 1)
   Print  @vc255a

   Raiserror('%s%s',0,1,'*********1*********2*********3*********4*********5*********6**','/')
   Print  @G_O

   Print  @EOLWinNT
   Print  'USE master'
   Print  @G_O


----------------------  Refresh DV usage fragment cursor  --------------


   Select @vc255a = Convert(varchar,@cu11DBId)



   EXECUTE(
   '
   DECLARE cursor_22DVUsageFrags
      Insensitive
      Cursor For
   '+'
   SELECT
             db.name
            ,db.dbid
            ,dv.name
            ,ug.size

               -- (numPages * bytesPerPage) / 1 Mb power of 2

            ,Str( (Convert(numeric(25,0),ug.size)  * ' +
                     @cSPTValPageSize +  ') / '  + @cOneMb + ' ,13  ,1)

            ,ug.segmap
            ,ug.lstart
      From
             master..sysdatabases   db
            ,master..sysusages      ug
            ,master..sysdevices     dv
            ,master..spt_values     va
      Where
             db.dbid     = ug.dbid
      And    dv.low     <= ug.size + ug.vstart
      And    dv.high    >= ug.size + ug.vstart - 1
      And    db.dbid     = ' + @vc255a + '
      And    va.type = ''S''
      And    ug.segmap & ' + @cSPTValDataAndLog + ' =
                             va.number  --1,2,4 & 7 always 1,2,4
      And    dv.status & 2 > 0
      Order By
             ug.lstart  Asc

   For Read Only
   '
   )


----------------------------------------------------------------
--              LOOP 22:  thru DV frags
----------------------------------------------------------------

   Print  @EOLWinNT
   Print  '-----------------  Space and Log allocations  ---------------'
   Print  @EOLWinNT


   Select @Loop22Count = 0

   OPEN cursor_22DVUsageFrags


   WHILE (22=22)
      Begin


      FETCH
             Next
         From
             cursor_22DVUsageFrags
         Into
             @cu22DBName
            ,@cu22DBId
            ,@cu22DVName
            ,@cu22FragSizePages
            ,@cu22FragSizeMb
            ,@cu22SegMap
            ,@cu22LStart


      IF (@@fetch_status < 0)
         begin
         DEALLOCATE cursor_22DVUsageFrags

         BREAK
         end


      Select @cu22FragSizeMb = Rtrim(Ltrim(@cu22FragSizeMb))


---------------------  massage the fragemnt size  -------------------


      IF (     (Right(@cu22FragSizeMb,2) >= '.1')
         AND   (Right(@cu22FragSizeMb,2) <= '.9')
         )
         begin   -- thus not .0


         Select @cu22FragSizeMb =
                     Convert(varchar
                            ,(Convert(numeric(12,1) ,@cu22FragSizeMb) +
                              Convert(numeric( 3,1) ,0.9) -- Adding 0.9!
                             )
                            )


         Select @cu22FragSizeMb = Rtrim(Ltrim(@cu22FragSizeMb))


         Select @cu22FragSizeMb = Substring(@cu22FragSizeMb
                                           ,1
                                           ,Datalength(@cu22FragSizeMb)-2
                                           )
                                + '.0'

         Print  @EOLWinNT
         Print  @EOLWinNT

         Raiserror('%s%s',0,1,'/','***')
         Raiserror(15316,-1,-1)  --Frag in Mb rounded up from size in pages.
         Raiserror('%s%s',0,1,'***','/')

         Print  @G_O
         Print  @EOLWinNT
         end


-----------------------  first iteration for this DB?  -------------


      Select @Loop22Count = @Loop22Count + 1


      Print  @EOLWinNT


      IF (@Loop22Count = 1)
         begin
         Select @vc80a = 'CREATE  '
         end
      ELSE
         begin
         Select @vc80a = 'ALTER   '
         end


      Select @vc80a = @vc80a + 'Database  ' + @cu22DBName
      Print  @vc80a

      Select @vc80a = ' '


---------------------  is this a special system DB?  ----------------


      IF (     @cu22DBName     IN ('master','model','msdb'
                               -- ,'pubs'
                                  ,'tempdb')
      -- AND   @Loop22Count   =  1
         )
         begin
         Raiserror(15313,-1,-1,@cu22DBName)
         end


      Select @int1 = Datalength(@cu22FragSizeMb)
      Select @cu22FragSizeMb = Substring(@cu22FragSizeMb,1,@int1-2)


      Select @vc80a = @vc80a + '      on  ' + @cu22DVName + '  =  '
                        +  Ltrim(Rtrim(@cu22FragSizeMb))
                        +  '  -- ' + Convert(varchar,@cu22FragSizePages)
                        +  '  of two Kb pages'
      Print  @vc80a


      Print  @G_O


--------------------  is this a log fragment?  --------------------


      IF (@cu22SegMap = @SPTValLogOnly)
         begin

         Select @vc80a = 'EXECute sp_logdevice '
                           +  @cu22DBName + ' ,'
                           +  @cu22DVName
         Print  @vc80a
         Print  @G_O
         end


--------------------  make pretty  -------------------------


      IF (@Loop22Count = 1)
         begin
         Print  @EOLWinNT
         Print  @EOLWinNT
         end


      End  -- loop 22


----------------------------------------------------------------
--              LOOP 33:  thru DB options
----------------------------------------------------------------


   Print  @EOLWinNT
   Print  '---------------------  DB Options  -------------------'
   Print  @EOLWinNT


   OPEN cursor_33SPTValDBOpts


   WHILE (33=33)
      Begin

      FETCH
             Next
         From
             cursor_33SPTValDBOpts
         Into
             @cu33OptName
            ,@cu33OptNumber


      IF (@@fetch_status < 0)
         begin
         CLOSE cursor_33SPTValDBOpts

         BREAK
         end



      IF (@cu33OptNumber & @cu11DBStatus = @cu33OptNumber)
         begin
         Select @TrueFalse = 'TRUE'
         end
      ELSE
         begin
         Select @TrueFalse = 'false'
         end


      Print  @EOLWinNT


      IF (@cu33OptName IN (
             'ANSI null default'
            ,'dbo use only'
            ,'no chkpt on recovery'
            ,'read only'
            ,'select into/bulkcopy'
            ,'single user'
            ,'trunc. log on chkpt.'
                         )
         )
         begin
         Select @CommentThisDBOption = 'N'
         end
      ELSE
         begin
         Select @CommentThisDBOption = 'Y'
         end


      IF (@CommentThisDBOption = 'Y')
         begin
         Raiserror('%s%s',0,1,'/','***')
         end


      Select @vc255a =
         'EXECute sp_dboption  ' + @cu11DBName + ' ,''' + @cu33OptName + ''''
      Print  @vc255a


      Select @vc255a = Space(21 + Datalength(@cu11DBName))
                       +  ' , '  + @TrueFalse
      Print  @vc255a


      IF (@CommentThisDBOption = 'Y')
         begin
         Raiserror('%s%s',0,1,'***','/')
         end


--    Print  @G_O

      End  -- loop 33


   Print  @EOLWinNT
   Print  @G_O


-----------------------  dbo login  ----------------------------


   SELECT
       @LoginName       = Null
      ,@LoginDBName     = Null
      ,@LoginLanguage   = Null


   Select @vc255a = Null


   SELECT
             @LoginName       = l.name
            ,@LoginDBName     = l.dbname
            ,@LoginLanguage   = l.language
      From
             master..sysdatabases   d
            ,master..syslogins      l
      Where
             d.suid     = l.suid
      And    d.dbid     = @cu11DBId




   IF (     @LoginName         is Null
      OR    @LoginName         =  ''
      OR    Lower(@LoginName)  =  'sa'
      )
      begin

      Print  @EOLWinNT
      Print  '--------------------  sa  is  dbo  ---------------------'
      Print  @EOLWinNT

      Goto LABEL_91SAISDBO
      end



   Print  @EOLWinNT
   Print  '---------------------  DBO Login  --------------------'
   Print  @EOLWinNT



   SELECT @vc255a =
               'EXECute  sp_addlogin  ' +  @LoginName
            +  char(13) + char(10)
            +  '   ,Null  ,'
            +  Isnull(@LoginDBName,'Null')  +  '  ,'
            +  Isnull(@LoginLanguage,'Null')


   Print  @EOLWinNT
   Print  @EOLWinNT
   Print  @EOLWinNT

   Print  @vc255a
   Print  @G_O

   Print  @EOLWinNT
   Select @vc255a = 'USE ' + @cu11DBName
   Print  @vc255a
   Print  @G_O

   Print  @EOLWinNT
   Select @vc255a = 'EXECute sp_changedbowner '  +  @LoginName
   Print  @vc255a
   Print  @G_O

   Print  @EOLWinNT
   Print  'USE master'
   Print  @G_O



LABEL_91SAISDBO:


   Print  @EOLWinNT
   Print  @G_O
   Print  '---'
   Print  @EOLWinNT
   Print  @EOLWinNT


   End  -- loop 11


---------------------------  Finalization  -----------------------


DEALLOCATE cursor_33SPTValDBOpts
DEALLOCATE cursor_11DBNames
-- DEALLOCATE cursor_22DVUsageFrags

EXECUTE('USE ' + @OrigDBName)  --For 6.0 db resumption.

LABEL_86RETURN:

RETURN @CountDBs
go





raiserror(15339,-1,-1,'sp_helpdevice')
go
create procedure sp_helpdevice
@devname varchar(30) = '%'		/* device to check out */
as

/*
**  See if the device exists.
*/
if not exists (select * from sysdevices where name like @devname)
	begin
		raiserror(15012,-1,-1,@devname)
		return (1)
	end

/*
**  Create a temporary table where we can build up a translation of
**  the device status bits.
*/
create table #spdevtab
(
	name varchar(30)		NOT NULL,
	statusdesc varchar(255)	null
)

set nocount on

/*
**  Initialize the temporary table with the names of the devices.
*/
insert into #spdevtab (name)
	select name
		from sysdevices
		where name like @devname


/*
**  Now figure out what kind of controller type it is.
**
**  cntrltype =			0	special (data disk)
**				2	disk (dump)
**				3-4	floppy (dump)
**				5	tape
**				6	pipe
**				7	cdrom
*/
update #spdevtab
	set statusdesc = 'special'
		from sysdevices d, #spdevtab
			where d.cntrltype = 0
				and #spdevtab.name = d.name
update #spdevtab
	set statusdesc = 'disk'
		from sysdevices d, #spdevtab
			where d.cntrltype = 2
				and #spdevtab.name = d.name
update #spdevtab
	set statusdesc = 'diskette'
		from sysdevices d, #spdevtab
			where d.cntrltype between 3 and 4
				and #spdevtab.name = d.name

update #spdevtab
	set statusdesc = 'pipe'
		from sysdevices d, #spdevtab
			where d.cntrltype = 6
				and #spdevtab.name = d.name
update #spdevtab
	set statusdesc = 'cdrom'
		from sysdevices d, #spdevtab
			where d.cntrltype = 7
				and #spdevtab.name = d.name
/*
**  If a floppy dump device, also figure out the floppy capacity which
**	is listed in sysdevices.high in number of 62k blocks.
*/
update #spdevtab
	set statusdesc = statusdesc + ', '
		+ ltrim(str(((d.high * 63488.0) / 1000000.0), 5,1))
		+ ' MB'
	from sysdevices d, #spdevtab
		where d.cntrltype in (3,4)
			and d.high > 0
			and #spdevtab.name = d.name

/*
**  If a tape device, also figure out the tape capacity which is listed
**  in sysdevices.high in number of 62k blocks.
*/
update #spdevtab
	set statusdesc = statusdesc + ', ' + convert(varchar(10),
		round((d.high * convert(float, 63488)) / 1048576, 0))
		+ ' MB'
	from sysdevices d, #spdevtab
		where d.cntrltype = 5
			and d.high > 0
			and #spdevtab.name = d.name

update #spdevtab
	set statusdesc = 'UNKNOWN DEVICE'
		from sysdevices d, #spdevtab
			where d.cntrltype >= 8
				and #spdevtab.name = d.name

/*
**  Now check out the status bits and turn them into english.
**  The mirror status bits are between 32 and 512.
*/
if exists (select * from sysdevices d, master.dbo.spt_values v, #spdevtab
		where v.type = 'V' and v.number > -1
			and d.status & 992 <> 0		/* 992 = (32+64+128+256+512) */
			and #spdevtab.name = d.name)
begin
	/*
	**  Check to see if the mirror is enabled. (512 bit in status)
	*/
	update #spdevtab
		set statusdesc = statusdesc + ', MIRROR ENABLED'
		from sysdevices d, #spdevtab
		where d.status & 512 = 512
			and #spdevtab.name = d.name

	update #spdevtab
		set statusdesc = statusdesc + ', MIRROR DISABLED'
		from sysdevices d, #spdevtab
		where d.status & 512 <> 512
			and d.status & 480 <> 0		/* 480 = sum of bits */
			and #spdevtab.name = d.name	/* from 32 - 256     */

	/* add mirror name */
	update #spdevtab
		set statusdesc = statusdesc + ', mirrored on ''' + d.mirrorname
			+ ''''
		from sysdevices d, #spdevtab
		where #spdevtab.name = d.name
			and d.status & 992 <> 0		/* 992 = sum of bits from 32 - 512 */
end

/*
**  Check to see if there is a mirrorname entry but mirroring not enabled.
**  If so, then one side of the mirror is off-line.
*/
else if exists (select *
	from sysdevices d, #spdevtab
	where #spdevtab.name = d.name
		and d.mirrorname IS NOT NULL)
begin
	/*
	**  Figure out which side of the mirror is disabled.
	**  If 256 is on then phyname is disabled and mirrorname is enabled.
	*/
	update #spdevtab set statusdesc = 'only device ''' + d.mirrorname
			+ ''' of mirror is enabled -- device '''
			+ d.phyname + ''' is disabled'
		from sysdevices d, master.dbo.spt_values v, #spdevtab
		where v.type = 'V' and v.number > -1
			and d.status & v.number = 256
			and #spdevtab.name = d.name

	update #spdevtab set statusdesc = 'only device ''' + d.phyname
			+ ''' of mirror is enabled -- device '''
			+ d.mirrorname + ''' is disabled'
		from sysdevices d, sysdevices e, master.dbo.spt_values v,
			#spdevtab
		where v.type = 'V' and v.number > -1
			and d.status & v.number = 64
			and #spdevtab.name = d.name
			and e.status & 256 <> 256
			and #spdevtab.name = e.name
end

/*
**  Status of 32 is 'serial writes' for mirrored disks.
*/
update #spdevtab set statusdesc = statusdesc + ', ' + rtrim(v.name)
	from sysdevices d, master.dbo.spt_values v, #spdevtab
	where v.type = 'V' and v.number > -1
		and d.status & v.number = 32
		and #spdevtab.name = d.name

update #spdevtab set statusdesc = statusdesc + ', nonserial writes'
	from sysdevices d, #spdevtab
	where d.status & 960 <> 0		/* Sum of bits from 64 to 512 */
		and d.status & 32 <> 32
		and #spdevtab.name = d.name

/*
**  Status of 128 is 'reads mirrored' for mirrored disks.
*/
update #spdevtab set statusdesc = statusdesc + ', ' + rtrim(v.name)
	from sysdevices d, master.dbo.spt_values v, #spdevtab
	where v.type = 'V' and v.number > -1
		and d.status & v.number = 128
		and #spdevtab.name = d.name

/*
**  Now check out the status bits and turn them into english.
**  Status of 16 is a dump device.
*/
update #spdevtab set statusdesc = statusdesc + ', ' + rtrim(v.name)
	from sysdevices d, master.dbo.spt_values v, #spdevtab
	where v.type = 'V' and v.number > -1
		and d.status & v.number = 16
		and #spdevtab.name = d.name

/*
**  Status of 1 is a default disk.
*/
update #spdevtab set statusdesc = statusdesc + ', ' + rtrim(v.name)
	from sysdevices d, master.dbo.spt_values v, #spdevtab
	where v.type = 'V' and v.number > -1
		and d.status & v.number = 1
		and #spdevtab.name = d.name

/*
**  Status of 2 is a physical disk.
*/
update #spdevtab
	set statusdesc = substring(statusdesc, 1, 225) + ', ' + rtrim(v.name)
	from sysdevices d, master.dbo.spt_values v, #spdevtab
	where v.type = 'V' and v.number > -1
		and d.status & v.number = 2
		and #spdevtab.name = d.name

/*
**  Add in its size in MB.
*/
update #spdevtab
	set statusdesc = statusdesc + ', ' + convert(varchar(10),
		round(((d.high - d.low)
			* convert(float, v.low)) / 1048576, 0)) + ' MB'
	from sysdevices d, #spdevtab, master.dbo.spt_values v
	where d.status & 2 = 2
		and #spdevtab.name = d.name
		and v.number = 1
		and v.type = 'E'

/*
**  Status of 4 is a logical disk.
*/
update #spdevtab
	set statusdesc = substring(statusdesc, 1, 225) + ', ' + rtrim(v.name)
	from sysdevices d, master.dbo.spt_values v, #spdevtab
	where v.type = 'V' and v.number > -1
		and d.status & v.number = 4
		and #spdevtab.name = d.name

/*
**  Status of 8 is a skip tape header.
*/
update #spdevtab
	set statusdesc = substring(statusdesc, 1, 225) + ', ' + rtrim(v.name)
	from sysdevices d, master.dbo.spt_values v, #spdevtab
	where v.type = 'V' and v.number > -1
		and d.status & v.number = 8
		and #spdevtab.name = d.name
/*
**  Status of 4096 is read only.
*/
update #spdevtab
	set statusdesc = substring(statusdesc, 1, 225) + ', ' + rtrim(v.name)
	from sysdevices d, master.dbo.spt_values v, #spdevtab
	where v.type = 'V' and v.number > -1
		and d.status & v.number = 4096
		and #spdevtab.name = d.name
/*
**  Status of 8192 is deferred.
*/
update #spdevtab
	set statusdesc = substring(statusdesc, 1, 225) + ', ' + (v.name)
	from sysdevices d, master.dbo.spt_values v, #spdevtab
	where v.type = 'V' and v.number > -1
		and d.status & v.number = 8192
		and #spdevtab.name = d.name

set nocount off

/*
**  The device number is in the high byte of sysdevices.low so
**  spt_values tells us which byte to pick out.
*/
select device_name = d.name, physical_name = substring(d.phyname, 1, 46),
	description = #spdevtab.statusdesc,
	d.status, d.cntrltype,
	device_number = convert(tinyint, substring(convert(binary(4), d.low),
		v.low, 1)),
	d.low, d.high
	from sysdevices d, #spdevtab, master.dbo.spt_values v
	where d.name = #spdevtab.name
		and v.type = 'E'
		and v.number = 3

return(0)
go


raiserror(15339,-1,-1,'sp_helpextendedproc')
go
create procedure sp_helpextendedproc
@funcname varchar(30) = NULL
as

set nocount on

/*
**  If no function name given, get 'em all.
*/
if @funcname is null
	select @funcname = '%'

/*
**  Make sure the function name exists
*/
if not exists (select * from master.dbo.sysobjects
		where type = 'X '
		  and name like @funcname)
begin
	if @funcname <> '%'
		begin
			raiserror(15019,-1,-1,@funcname)
			return (1)
		end
	else
		begin
			print 'No extended stored procedures exist.'
			return (0)
		end

end

/*
**  Now print out the function name info
*/
select distinct name = o.name, dll = substring(c.text,1,30)
	from master.dbo.sysobjects o, master.dbo.syscomments c
	where o.id = c.id
		and o.name like @funcname
		and o.type = 'X '
	order by o.name

return (0)
go


raiserror(15339,-1,-1,'sp_helpgroup')
go
create procedure sp_helpgroup
@grpname varchar(30) = NULL		/* group name of interest */
as

/*
**  If no group name given, list all the groups.
**  User ids are <= 16383 and group ids are > 16383 or = 0.
*/
if @grpname is null
begin
	select Group_name = name, Group_id = gid
		from sysusers
			where (uid > 16383 or uid = 0)
		order by name

	return (0)
end

/*
**  Check to see if group exists.
*/
if not exists (select * from sysusers where name = @grpname
		and (uid > 16383 or uid = 0))
	begin
		raiserror(15014,-1,-1,@grpname)
		return (1)
	end

/*
**  List the particulars for the group.
*/
select Group_name = substring(g.name, 1, 25), Group_id = g.gid,
	Users_in_group = substring(u.name, 1, 25), Userid = u.uid
		from sysusers u, sysusers g
			where g.name like @grpname
			and g.uid = u.gid
			and (g.uid > 16383 or g.uid = 0)
			and u.uid <> u.gid
		order by g.name, u.name

return (0)
go



raiserror(15339,-1,-1,'sp_helplog')
go
create procedure sp_helplog
as
declare @firstpage int,
	@devname varchar(255),
	@msg varchar(255)


/* Get the first page of the log from sysindexes */
select @firstpage = first
	from sysindexes
		where id = 8

/*
**  Get the name of the device which contains the first page of the log.
**  The device number is in the high byte of d.low and u.vstart and which is the
**  high byte is defined in spt_values.
*/
select @devname = d.name
	from sysdevices d, sysusages u, master.dbo.spt_values v
	where (d.status & 2 = 2 or d.status & 4 = 4)
		and @firstpage >= u.lstart
		and @firstpage < u.lstart + u.size - 1
		and convert(tinyint, substring(convert(binary(4), d.low),
			v.low, 1))
		  = convert(tinyint, substring(convert(binary(4), u.vstart),
		       v.low, 1))
		and v.type = 'E'
		and v.number = 3
		and u.dbid = db_id()

/* Print out the name of the device which contains the first page of the log */
select @msg = 'In database ''' + (select db_name())
	+ ''', the log starts on device ''' + @devname + '''.'

print @msg

return (0)
go




raiserror(15339,-1,-1,'sp_helplogins')
go
CREATE PROCEDURE sp_helplogins  --1995/11/20 16:50

    @LoginNamePattern     varchar(55)    = '%'
AS

Set nocount on

Declare
       @RetCode                        int
      ,@CountSkipPossUsers             int
      ,@Int1                           int

Declare
       @c10DBName                      varchar(30)
      ,@c10DBStatus                    smallint
      ,@c10DBSUID                      smallint

Declare
       @charMaxLenLoginName            varchar(11)
      ,@charMaxLenDBName               varchar(11)
      ,@charMaxLenUserName             varchar(11)
      ,@charMaxLenLangName             varchar(11)

Declare
       @DBOptLoading                   int   --0x0020      32  "DoNotRecover"
      ,@DBOptPreRecovery               int   --0x0040      64
      ,@DBOptRecovering                int   --0x0080     128

      ,@DBOptSuspect                   int   --0x0100     256  ("not recovered")
      ,@DBOptOffline                   int   --0x0200     512
      ,@DBOptDBOUseOnly                int   --0x0800    2048

      ,@DBOptSingleUser                int   --0x1000    4096


----------------  Initial data values  -------------------

Select
       @RetCode                        = 0  -- 0=good ,1=bad
      ,@CountSkipPossUsers             = 0


----------------  Only SA can run this  -------------------


IF (suser_id() <> 1)  --SA, else risk errors in DB's lacking GUEST.
   begin
   RaisError(15003,-1,-1)
   Select @RetCode = 1
   GOTO LABEL_86RETURN
   end


---------------  Edit input parm values  ---------------


IF (      @LoginNamePattern IS Null)
   Select @LoginNamePattern  = '%'


-------------  create work holding tables  ----------------


CREATE Table #tb2_PlainLogins
   (
    LoginName                       varchar(30)    NOT Null
   ,SUID                            char(6)        NOT Null
   ,DefDBName                       varchar(30)    NOT Null
   ,DefLangName                     varchar(30)        Null
   ,AUser                           char(5)            Null
   ,ARemote                         char(7)            Null
   )


CREATE Table #tb1_UA
   (
    LoginName                       varchar(30)    NOT Null
   ,DBName                          varchar(30)    NOT Null
   ,UserName                        varchar(30)    NOT Null
   ,UserOrAlias                     char(5)        NOT Null
   )


----------------------  spt_values  ----------------


-------- 'D'

SELECT       @DBOptLoading       = number
      from   master.dbo.spt_values
      where  type                = 'D'
      and    name                = 'loading'

SELECT       @DBOptPreRecovery   = number
      from   master.dbo.spt_values
      where  type                = 'D'
      and    name                = 'pre recovery'

SELECT       @DBOptRecovering    = number
      from   master.dbo.spt_values
      where  type                = 'D'
      and    name                = 'recovering'

SELECT       @DBOptSuspect       = number
      from   master.dbo.spt_values
      where  type                = 'D'
      and    name                = 'not recovered'

SELECT       @DBOptOffline       = number
      from   master.dbo.spt_values
      where  type                = 'D'
      and    name                = 'offline'

SELECT       @DBOptDBOUseOnly    = number
      from   master.dbo.spt_values
      where  type                = 'D'
      and    name                = 'dbo use only'

SELECT       @DBOptSingleUser    = number
      from   master.dbo.spt_values
      where  type                = 'D'
      and    name                = 'single user'



---------------  Cursor, for DBNames  -------------------


DECLARE cursor10_DB
   Insensitive
   Cursor For
SELECT
             name ,status ,suid
      from
             master.dbo.sysdatabases



OPEN cursor10_DB


-----------------  LOOP 10:  thru Databases  ------------------


--------------
WHILE (10 = 10
      )
   begin    --LOOP 10: thru Databases


   FETCH
             Next
      from
             cursor10_DB
      into
             @c10DBName
            ,@c10DBStatus
            ,@c10DBSUID


   IF (@@fetch_status <> 0)
      begin
      Deallocate cursor10_DB
      BREAK
      end


--------------------  Okay if we peek inside this DB now?


   IF (     @c10DBStatus & @DBOptDBOUseOnly  > 0
       AND  @c10DBSUID                      <> suser_id()
      )
      begin
      Select @CountSkipPossUsers = @CountSkipPossUsers + 1
      CONTINUE
      end


   IF (@c10DBStatus & @DBOptSingleUser  > 0)
      begin

      SELECT    @Int1 = count(*)
         from   master.dbo.sysprocesses
         where  spid <> @@spid
         and    dbid  = db_id(@c10DBName)

      IF (@Int1 > 0)
         begin
         Select @CountSkipPossUsers = @CountSkipPossUsers + 1
         CONTINUE
         end
      end


   IF (@c10DBStatus &
         (
           @DBOptLoading
         | @DBOptRecovering
         | @DBOptSuspect
         | @DBOptPreRecovery
         )
               > 0
      )
      begin
      Select @CountSkipPossUsers = @CountSkipPossUsers + 1
      CONTINUE
      end


   IF (@c10DBStatus &
         (
           @DBOptOffline
         )
               > 0
      )
      begin
      --Select @CountSkipPossUsers = @CountSkipPossUsers + 1
      CONTINUE
      end


---------------------  Add the User info to holding table.


   EXECUTE(
   '
   INSERT    #tb1_UA
            (
             DBName
            ,LoginName
            ,UserName
            ,UserOrAlias
            )
      Select
             ''' + @c10DBName + '''
            ,l.name
            ,u.name
            ,''User''
         from
             ' + @c10DBName + '.dbo.sysusers       u
            ,master.dbo.syslogins                  l
         where
             u.suid  = l.suid
         and
             l.name LIKE ''' + @LoginNamePattern + '''

      UNION

      Select

             ''' + @c10DBName + '''
            ,l.name
            ,u.name
            ,''Alias''
         from
             ' + @c10DBName + '.dbo.sysalternates  a
            ,' + @c10DBName + '.dbo.sysusers       u
            ,master.dbo.syslogins                  l
         where
             a.suid     = l.suid
         and a.altsuid  = u.suid
         and
             l.name LIKE ''' + @LoginNamePattern + '''
   '
   )


   end --loop 10



---------------  Populate plain logins work table  ---------------


INSERT       #tb2_PlainLogins
            (
             LoginName
            ,SUID
            ,DefDBName
            ,DefLangName
            ,AUser
            ,ARemote
            )
   SELECT
             name
            ,convert(char(6),suid)
            ,dbname
            ,isnull(language,'us_english')
            ,Null
            ,Null
      from
             master.dbo.syslogins
      where
             name LIKE @LoginNamePattern


-- AUser

UPDATE       #tb2_PlainLogins
      set
             tb2.AUser  = 'yes'
      from
             #tb2_PlainLogins    tb2
            ,#tb1_UA             tb1
      where
             tb2.LoginName     = tb1.LoginName
      and    tb2.AUser        IS Null



UPDATE       #tb2_PlainLogins
      set
             AUser    =
                  CASE @CountSkipPossUsers
                     When  0  Then  'NO'
                     Else           '?'
                  END
      where
             AUser   IS Null


-- ARemote

UPDATE       #tb2_PlainLogins
      set
             tb2.ARemote   = 'YES'
      from
             #tb2_PlainLogins             tb2
            ,master.dbo.sysremotelogins   rl
      where
             convert(smallint,tb2.SUID)   = rl.suid
      and    tb2.ARemote                 IS Null



UPDATE       #tb2_PlainLogins
      set
             ARemote  = 'no'
      where
             ARemote IS Null



------------  Optimize widths for plain Logins report  ----------


SELECT
             @charMaxLenLoginName      =
                  convert ( varchar
                           ,isnull ( max(datalength(LoginName)) ,9)
                          )
            ,@charMaxLenDBName         =
                  convert ( varchar
                           ,isnull ( max(datalength(DefDBName)) ,9)
                          )
            ,@charMaxLenLangName   =
                  convert ( varchar
                           ,isnull ( max(datalength(DefLangName)) ,11)
                          )
      from
             #tb2_PlainLogins



----------------  Print out plain Logins report  -------------

/*** Message Handlers get confused.
Raiserror('...Logins...' ,0,1)
***/

EXECUTE(
'
Set nocount off


SELECT
          ''LoginName''       = substring (LoginName     ,1 ,'
                                       + @charMaxLenLoginName   + ')

         ,SUID

         ,''DefDBName''       = substring (DefDBName     ,1 ,'
                                       + @charMaxLenDBName      + ')

         ,''DefLangName''     = substring (DefLangName   ,1 ,'
                                       + @charMaxLenLangName    + ')

         ,AUser
         ,ARemote
   from
          #tb2_PlainLogins
   order by
          LoginName


Set nocount on
'
)



------------  Optimize UA report column display widths  -----------


SELECT
             @charMaxLenLoginName   =
                  convert ( varchar
                           ,isnull ( max(datalength(LoginName)) ,9)
                          )
            ,@charMaxLenDBName      =
                  convert ( varchar
                           ,isnull ( max(datalength(DBName)) ,6)
                          )
            ,@charMaxLenUserName    =
                  convert ( varchar
                           ,isnull ( max(datalength(UserName)) ,8)
                          )
      from
             #tb1_UA



------------  Print out the UserOrAlias report  ------------

/***
Raiserror('...Logins-to-Users...' ,0,1)
***/

EXECUTE(
'
Set nocount off


SELECT
          ''LoginName''    = substring (LoginName  ,1 ,'
                                       + @charMaxLenLoginName  + ')

         ,''DBName''       = substring (DBName     ,1 ,'
                                       + @charMaxLenDBName     + ')

         ,''UserName''     = substring (UserName   ,1 ,'
                                       + @charMaxLenUserName   + ')

         ,UserOrAlias
   from
          #tb1_UA
   order by
          1 ,2 ,3


Set nocount on
'
)


-----------------------  Finalization  --------------------


IF (object_id('tempdb..#tb2_PlainLogins') IS NOT Null)
            DROP Table #tb2_PlainLogins

IF (object_id('tempdb..#tb1_UA') IS NOT Null)
            DROP Table #tb1_UA


LABEL_86RETURN:

Return @RetCode
go




raiserror(15339,-1,-1,'sp_helpindex')
go
create procedure sp_helpindex
@objname varchar(92)		/* the table to check for indexes */
as

declare @objid int		/* the object id of the table */
declare @indid int		/* the index id of an index */
declare @keys varchar(200)	/* string to build up index key in */
declare @inddesc varchar(68)	/* string to build up index desc in */
declare @dbname varchar(30)

/*
**  Check to see that the object names are local to the current database.
*/
if @objname like '%.%.%' and
	substring(@objname, 1, charindex('.', @objname) - 1) <> db_name()
	begin
		raiserror(15250,-1,-1)
		return (1)
	end

/*
**  Check to see the the table exists and initialize @objid.
*/
select @objid = object_id(@objname)

/*
**  Table doesn't exist so return.
*/
if @objid is NULL
	begin
		select @dbname=db_name()
		raiserror(15009,-1,-1,@objname,@dbname)
		return (1)
	end

/*
**  See if the object has any indexes.
**  Since there may be more than one entry in sysindexes for the object,
**  this select will set @indid to the index id of the first index.
*/
select @indid = min(indid) from sysindexes
	where id = @objid
		and indid > 0
		and indid < 255

/*
**  If no indexes, return.
*/
if @indid is NULL
	begin
		print 'Object does not have any indexes.'
		return (0)
	end

set nocount on

/*
**  Now check out each index, figure out its type and keys and
**  save the info in a temporary table that we'll print out at the end.
*/
create table #spindtab
(
	index_name		varchar(30)	NOT NULL,
	index_keys		varchar(255)	NOT NULL,
	index_description	varchar(68)	NOT NULL
)

while @indid IS NOT NULL
begin

	/*
	**  First we'll figure out what the keys are.
	*/
	declare @i int
	declare @thiskey varchar(30)
	declare @lastindid int

	select @i = 1

	while @i <= 16
	begin
		select @thiskey = index_col(@objname, @indid, @i)

		if @thiskey is null
			goto keysdone

		if @i = 1
			select @keys = index_col(@objname, @indid, @i)
		else
			select @keys = @keys + ', '+index_col(@objname, @indid, @i)

		/*
		**  Increment @i so it will check for the next key.
		*/
		select @i = @i + 1

	end


	/*
	**  When we get here we now have all the keys.
	*/
	keysdone:

	/*
	**  Initialize the index description by figuring out if it's a
	**  clustered or nonclustered index.
	*/
	if @indid = 1
		select @inddesc = 'clustered'

	if @indid > 1
		select @inddesc = 'nonclustered'

	/*
	**  Now we'll check out the status bits for this index and
	**  build an english description from them.
	*/

	/*
	**  See if the index is ignore_dupkey (0x01).
	*/
	if exists (select *
		from master.dbo.spt_values v, sysindexes i
		where i.status & v.number = v.number
			and v.type = 'I'
			and v.number = 1
			and i.id = @objid
			and i.indid = @indid)

		select @inddesc = @inddesc + ', ' + v.name
		from master.dbo.spt_values v, sysindexes i
		where i.status & v.number = v.number
			and v.type = 'I'
			and v.number = 1
			and i.id = @objid
			and i.indid = @indid

	/*
	**  See if the index is unique (0x02).
	*/
	if exists (select *
		from master.dbo.spt_values v, sysindexes i
		where i.status & v.number = v.number
			and v.type = 'I'
			and v.number = 2
			and i.id = @objid
			and i.indid = @indid)

		select @inddesc = @inddesc + ', ' + v.name
		from master.dbo.spt_values v, sysindexes i
		where i.status & v.number = v.number
			and v.type = 'I'
			and v.number = 2
			and i.id = @objid
			and i.indid = @indid

	/*
	**  See if the index is ignore_dup_row (0x04).
	*/
	if exists (select *
		from master.dbo.spt_values v, sysindexes i
		where i.status & v.number = v.number
			and v.type = 'I'
			and v.number = 4
			and i.id = @objid
			and i.indid = @indid)

		select @inddesc = @inddesc + ', ' + v.name
		from master.dbo.spt_values v, sysindexes i
		where i.status & v.number = v.number
			and v.type = 'I'
			and v.number = 4
			and i.id = @objid
			and i.indid = @indid

	/*
	**  See if the index is allow_dup_row (0x40).
	*/
	if exists (select *
		from master.dbo.spt_values v, sysindexes i
		where i.status & v.number = v.number
			and v.type = 'I'
			and v.number = 64
			and i.id = @objid
			and i.indid = @indid)

		select @inddesc = @inddesc + ', ' + v.name
		from master.dbo.spt_values v, sysindexes i
		where i.status & v.number = v.number
			and v.type = 'I'
			and v.number = 64
			and i.id = @objid
			and i.indid = @indid

	/*
	** See if the index is primary key constraint (0x800).
	*/
	if exists (select *
		from master.dbo.spt_values v, sysindexes i
		where i.status & v.number = v.number
			and v.type = 'I'
			and v.number = 2048
 			and i.id = @objid
 			and i.indid = @indid)

			select @inddesc = @inddesc + ', ' + v.name
				from master.dbo.spt_values v, sysindexes i
				where i.status & v.number = v.number
					and v.type = 'I'
					and v.number = 2048
					and i.id = @objid
					and i.indid = @indid

	/*
	**  See if the index is unique key constraint (0x1000).
 	*/
	if exists (select * from master.dbo.spt_values v, sysindexes i
			where i.status & v.number = v.number
			and v.type = 'I'
			and v.number = 4096
			and i.id = @objid
 			and i.indid = @indid)

			select @inddesc = @inddesc + ', ' + v.name
				from master.dbo.spt_values v, sysindexes i
				where i.status & v.number = v.number
					and v.type = 'I'
					and v.number = 2048
					and i.id = @objid
					and i.indid = @indid

	/*
	**  Add the location of the data.
	*/

	select @inddesc = @inddesc + ' located on ' + s.name
		from syssegments s, sysindexes i
		where s.segment = i.segment
			and i.id = @objid
			and i.indid = @indid

	/*
	**  Now we have the whole description for the index so we'll add
	**  the goods to the temporary table.
	*/
	insert into #spindtab
		select name, @keys, @inddesc
			from sysindexes
			where id = @objid
				and indid = @indid
	/*
	**  Now move @indid to the next index.
	*/
	select @lastindid = @indid
	select @indid = NULL
	select @indid = min(indid)
		from sysindexes
		where id = @objid
			and indid > @lastindid
			and indid < 255
end

/*
**  Now print out the contents of the temporary index table.
*/
select index_name = substring(index_name, 1, 20),
	index_description = substring(index_description, 1, 56),
	index_keys = index_keys from #spindtab

set nocount off

return (0)
go


raiserror(15339,-1,-1,'sp_helpjoins')
go
create procedure sp_helpjoins
@lefttab varchar(92),			/* name of first table to join */
@righttab varchar(92)			/* name of first table to join */
as

declare @leftid int			/* id of first table */
declare @rightid int			/* id of second table */
declare @foundit bit			/* flag to indicate joins found */

/*
**  This procedure returns any foreign or common joins.
**  If none exists, then it returns any joins that have the same usertype.
**  If none exists, it returns any joins that have the same physical type.
*/

/*
**  Make sure the @objname is local to the current database.
*/
if (@lefttab like '%.%.%' and
	substring(@lefttab, 1, charindex('.', @lefttab) - 1) <> db_name())
OR
	(@righttab like '%.%.%' and
	substring(@righttab, 1, charindex('.', @righttab) - 1) <> db_name())
		begin
			raiserror(15250,-1,-1)
			return (1)
		end

/*
**  See if we can find the objects.  They must be a system table, user table,
**  or view.  The low 4 bits of sysobjects.sysstat indicate what the
**  object type is -- it's more reliable than using sysobjects.type which
**  could change.
*/
select @leftid = id
	from sysobjects
		where id = object_id(@lefttab)
			and (sysstat & 0xf = 1		/* system table */
				or sysstat & 0xf = 2	/* view */
				or sysstat & 0xf = 3)	/* user table */
select @rightid = id
	from sysobjects
		where id = object_id(@righttab)
			and (sysstat & 0xf = 1		/* system table */
				or sysstat & 0xf = 2	/* view */
				or sysstat & 0xf = 3)	/* user table */

/*
**  If either of the tables don't exist, quit.
*/
if @leftid is NULL
	begin
		raiserror(15195,-1,-1)
		return (1)
	end
if @rightid is NULL
	begin
		raiserror(15196,-1,-1)
		return (1)
	end

create table #hjtab
(
	a1	varchar(30) NOT NULL,
	a2	varchar(30) NOT NULL,
	b1	varchar(30) null,
	b2	varchar(30) null,
	c1	varchar(30) null,
	c2	varchar(30) null,
	d1	varchar(30) null,
	d2	varchar(30) null,
	e1	varchar(30) null,
	e2	varchar(30) null,
	f1	varchar(30) null,
	f2	varchar(30) null,
	g1	varchar(30) null,
	g2	varchar(30) null,
	h1	varchar(30) null,
	h2	varchar(30) null
)

/*
**  Look for foreign key joins.
*/
select @foundit = 0
if exists (select *
		from syskeys
			where type = 2
				and id = @leftid
				and depid = @rightid)
begin
	insert into #hjtab
	select distinct first_pair = col_name(id, key1), col_name(depid, depkey1),
		second_pair = col_name(id, key2), col_name(depid, depkey2),
		third_pair = col_name(id, key3), col_name(depid, depkey3),
		fourth_pair = col_name(id, key4), col_name(depid, depkey4),
		fifth_pair = col_name(id, key5), col_name(depid, depkey5),
		sixth_pair = col_name(id, key6), col_name(depid, depkey6),
		seventh_pair = col_name(id, key7), col_name(depid, depkey7),
		eighth_pair = col_name(id, key8), col_name(depid, depkey8)
	from syskeys
		where type = 2
			and id = @leftid
			and depid = @rightid

	select @foundit = 1
end

if exists (select *
		from syskeys
			where type = 2
				and id = @rightid
				and depid = @leftid)
begin
	insert into #hjtab
	select distinct first_pair = col_name(depid, depkey1), col_name(id, key1),
		second_pair = col_name(depid, depkey2), col_name(id, key2),
		third_pair = col_name(depid, depkey3), col_name(id, key3),
		fourth_pair = col_name(depid, depkey4), col_name(id, key4),
		fifth_pair = col_name(depid, depkey5), col_name(id, key5),
		sixth_pair = col_name(depid, depkey6), col_name(id, key6),
		seventh_pair = col_name(depid, depkey7), col_name(id, key7),
		eighth_pair = col_name(depid, depkey8), col_name(id, key8)
	from syskeys
		where type = 2
			and id = @rightid
			and depid = @leftid

	select @foundit = 1
end

/*
**  Look for common key joins.
*/
if exists (select *
		from syskeys
			where type = 3
				and id = @leftid
				and depid = @rightid)
begin
	insert into #hjtab
	select distinct first_pair = col_name(id, key1), col_name(depid, depkey1),
		second_pair = col_name(id, key2), col_name(depid, depkey2),
		third_pair = col_name(id, key3), col_name(depid, depkey3),
		fourth_pair = col_name(id, key4), col_name(depid, depkey4),
		fifth_pair = col_name(id, key5), col_name(depid, depkey5),
		sixth_pair = col_name(id, key6), col_name(depid, depkey6),
		seventh_pair = col_name(id, key7), col_name(depid, depkey7),
		eighth_pair = col_name(id, key8), col_name(depid, depkey8)
	from syskeys
		where type = 3
			and id = @leftid
			and depid = @rightid

	select @foundit = 1
end

if exists (select *
		from syskeys
			where type = 3
				and id = @rightid
				and depid = @leftid)
begin
	insert into #hjtab
	select distinct first_pair = col_name(depid, depkey1), col_name(id, key1),
		second_pair = col_name(depid, depkey2), col_name(id, key2),
		third_pair = col_name(depid, depkey3), col_name(id, key3),
		fourth_pair = col_name(depid, depkey4), col_name(id, key4),
		fifth_pair = col_name(depid, depkey5), col_name(id, key5),
		sixth_pair = col_name(depid, depkey6), col_name(id, key6),
		seventh_pair = col_name(depid, depkey7), col_name(id, key7),
		eighth_pair = col_name(depid, depkey8), col_name(id, key8)
	from syskeys
		where type = 3
			and id = @rightid
			and depid = @leftid

	select @foundit = 1
end

/*
**  We got a foreignkey and we didn't get a common key.
*/
if @foundit = 1
	begin
		select distinct * from #hjtab

		drop table #hjtab

		return (0)
	end

/*
**  We didn't find any defined joins so we'll look for common user types.
*/
if exists (select *
		from syscolumns a, syscolumns b
			where a.id = @leftid
				and b.id = @rightid
				and a.usertype = b.usertype
				and a.usertype > 99)
begin
	select distinct first_pair = col_name(a.id, a.colid),
		col_name(b.id, b.colid)
	from syscolumns a, syscolumns b
		where a.id = @leftid
			and b.id = @rightid
			and a.usertype = b.usertype
			and a.usertype > 99
	order by first_pair

	/*
	**  If we found something, we can stop.
	*/
	if @@rowcount > 0
		return (0)
end

/*
**  We couldn't find anything so far so we'll see if we have anything
**  that just shares the same physical type.
*/
if exists (select *
		from syscolumns a, syscolumns b, master.dbo.spt_values y,
			master.dbo.spt_values z
		where a.id = @leftid
			and b.id = @rightid
			and a.id <> b.id
			and a.name = b.name
			and y.type = 'J'
			and a.type = y.low
			and z.type = 'J'
			and b.type = z.low
			and y.number = z.number)
begin
	select distinct first_pair = col_name(a.id, a.colid),
		col_name(b.id, b.colid)
	from syscolumns a, syscolumns b, master.dbo.spt_values y,
		master.dbo.spt_values z
	where a.id = @leftid
		and b.id = @rightid
		and a.id <> b.id
		and a.name = b.name
		and y.type = 'J'
		and a.type = y.low
		and z.type = 'J'
		and b.type = z.low
		and y.number = z.number
	order by first_pair

end

return (0)
go


raiserror(15339,-1,-1,'sp_helpkey')
go
create procedure sp_helpkey
@tabname varchar(92) = null,	/* table or view we want to check */
@nomsg varchar(5) = 'msg'	/* option param. to suppress printing
				** of suggestion to also use
				** sp_helpconstraint to view additional
				** constraints. */
as

declare	@tabid	int	/* id of table or view we want to check */
declare @dbname varchar(30)

/*
**  Check to see that the object names are local to the current database.
*/
if @tabname like '%.%.%' and
	substring(@tabname, 1, charindex('.', @tabname) - 1) <> db_name()
	begin
		raiserror(15078,-1,-1)
		return (1)
	end

/*
**  If @tabname is null, show all the keys.
*/
print ''

if @tabname is null
begin
	select keytype = substring(v.name, 1, 10), object = object_name(k.id),
		related_object = isnull(object_name(k.depid), ' -- none --'),
		object_keys = substring(col_name(k.id, key1) + ', '
			+ isnull(col_name(k.id, key2), '*') + ', '
			+ isnull(col_name(k.id, key3), '*') + ', '
			+ isnull(col_name(k.id, key4), '*') + ', '
			+ isnull(col_name(k.id, key5), '*') + ', '
			+ isnull(col_name(k.id, key6), '*') + ', '
			+ isnull(col_name(k.id, key7), '*') + ', '
			+ isnull(col_name(k.id, key8), '*'), 1, 70),
		related_keys = substring(isnull(col_name(k.depid, depkey1), '*')
			+ ', '
			+ isnull(col_name(k.depid, depkey2), '*') + ', '
			+ isnull(col_name(k.depid, depkey3), '*') + ', '
			+ isnull(col_name(k.depid, depkey4), '*') + ', '
			+ isnull(col_name(k.depid, depkey5), '*') + ', '
			+ isnull(col_name(k.depid, depkey6), '*') + ', '
			+ isnull(col_name(k.depid, depkey7), '*') + ', '
			+ isnull(col_name(k.depid, depkey8), '*'), 1, 70)
	from syskeys k, master.dbo.spt_values v
	where k.type = v.number
		and v.type = 'K'
	order by object, keytype, related_object

	return (0)
end

/*
**  We're looking for a particular table or view.
*/
else
begin
	/*
	**  Make sure the table or view exists.
	*/
	select @tabid = object_id(@tabname)

	if @tabid is null
		begin
			select @dbname=db_name()
			raiserror(15009,-1,-1,@tabname,@dbname)
			return (1)
		end

	/*
	**  See if any keys exist.
	*/
	if not exists (select *
			from syskeys
			where id = @tabid or depid = @tabid)
		begin
			if @nomsg <> 'nomsg'
				begin
					print 'No keys were defined for this object using sp_primarykey,'
					print 'sp_foreignkey or sp_commonkey.  Please use sp_helpconstraint'
					print 'to list any ''system enforced'' keys.'
				end

			return (0)
		end

	select keytype = substring(v.name, 1, 10), object = object_name(k.id),
		related_object = isnull(object_name(k.depid), ' -- none --'),
		object_keys = substring(col_name(k.id, key1) + ', '
			+ isnull(col_name(k.id, key2), '*') + ', '
			+ isnull(col_name(k.id, key3), '*') + ', '
			+ isnull(col_name(k.id, key4), '*') + ', '
			+ isnull(col_name(k.id, key5), '*') + ', '
			+ isnull(col_name(k.id, key6), '*') + ', '
			+ isnull(col_name(k.id, key7), '*') + ', '
			+ isnull(col_name(k.id, key8), '*'), 1, 70),
		related_keys = substring(isnull(col_name(k.depid, depkey1), '*')
			+ ', '
			+ isnull(col_name(k.depid, depkey2), '*') + ', '
			+ isnull(col_name(k.depid, depkey3), '*') + ', '
			+ isnull(col_name(k.depid, depkey4), '*') + ', '
			+ isnull(col_name(k.depid, depkey5), '*') + ', '
			+ isnull(col_name(k.depid, depkey6), '*') + ', '
			+ isnull(col_name(k.depid, depkey7), '*') + ', '
			+ isnull(col_name(k.depid, depkey8), '*'), 1, 70)
	from syskeys k, master.dbo.spt_values v
	where k.type = v.number
		and v.type = 'K'
		and (k.id = @tabid or k.depid = @tabid)
	order by object, keytype, related_object

	return (0)
end
go


raiserror(15339,-1,-1,'sp_objectsegment')
go
create procedure sp_objectsegment
@objid	int
as
	/*
	** Print out the object's data segment if applicable.
	*/

	if exists (select * from sysobjects
			where id = @objid
			and sysstat & 0xf in (1,3))	-- system table or user table.
		begin
			select Data_located_on_segment = s.name
			from syssegments s, sysindexes i
			where i.id = @objid
				and i.indid < 2
				and i.segment = s.segment
		end

	/*
	**  It's not a table so segment is not applicable.
	*/
	else
		select Data_located_on_segment = 'not applicable'

return (0)
go



raiserror(15339,-1,-1,'sp_help')
go
create procedure sp_help  -- 1995/09/13 18:23
    @objname varchar(92) = NULL      /* object name we're after */
as

declare @objid int         /* id of the object */
declare @sysstat smallint     /* the type of the object */
declare @dbname varchar(30)
declare @colname varchar(30)

set nocount on

/*
**  If no @objname given, give a little info about all objects.
*/
if @objname is null
begin
   select Name = o.name,
      Owner = user_name(uid),
      Object_type = v.name + x.name
   from sysobjects o, master.dbo.spt_values v, master.dbo.spt_values x
   where o.sysstat & 0xf = v.number
      and v.type = 'O'
      and x.type = 'R'
      and o.userstat & -32768 = x.number
   order by Object_type desc, Name asc

   print ''

   select User_type = s.name,
          'Storage_type' = st.name,
          'Length' = s.length,
          'Prec' =
         case when s.type in  -- Prec. only valid for numerics.
         (38,48,52,55,56,59,60,62,63,106,108,109,110,122)
         then convert(varchar(4),s.prec)
         else ' '
         end,
          'Scale' =
         case when s.type in  -- Scale only valid for numerics.
         (38,48,52,55,56,59,60,62,63,106,108,109,110,122)
         then convert(varchar(4),s.scale)
         else ' '
         end,
          'Nullable' = (select name from spt_values
            where number = convert(int,s.allownulls)
            and type = 'B'),
          'Default_name' = isnull(object_name(s.tdefault),
         (select name from master.dbo.spt_values
            where number = 2 and type = 'B')),
          'Rule_name' = isnull(object_name(s.domain),
         (select name from master.dbo.spt_values
            where number = 2 and type = 'B'))
   from systypes s, systypes st
   where s.type = st.type
      and s.usertype > 99
      and st.usertype <= 99
      and st.usertype <> 18 /* ignore sysname */
   order by s.name

   return(0)
end

/*
**  Make sure the @objname is local to the current database.
*/
if @objname like '%.%.%' and
   substring(@objname, 1, charindex('.', @objname) - 1) <> db_name()
   begin
      raiserror(15250,-1,-1)
      return(1)
   end

/*
**  Now check to see if the @objname is in sysobjects.  It has to be either
**  in sysobjects or systypes.
*/
select @objid = id, @sysstat = sysstat from sysobjects
   where id = object_id(@objname)

/*
**  It wasn't in sysobjects so we'll check in systypes.
*/
if @objid is null
begin
   select @objid = usertype from systypes where name = @objname

   /*
   **  Time to give up -- @objname is not in sysobjects or systypes.
   */
   if @objid is null
      begin
         select @dbname=db_name()
         raiserror(15009,-1,-1,@objname,@dbname)
         return(1)
      end

   /*
   ** Print help about a data type.  Prec and scale only valid
   ** for numerics.
   */

   if (select type from systypes
      where usertype = @objid) in
         (38,48,52,55,56,59,60,62,63,106,108,109,110,122)

      select Type_name = s.name,
         Storage_type = st.name,
         Length = s.length,
         Prec = s.prec,
         Scale = s.scale,
         'Nullable' = (select name from master.dbo.spt_values
               where number =
                  convert(int,s.allownulls)
                  and type = 'B'),
         Default_name = isnull(object_name(s.tdefault),
            (select name from master.dbo.spt_values
               where number = 2 and type = 'B')),
         Rule_name = isnull(object_name(s.domain),
            (select name from master.dbo.spt_values
               where number = 2 and type = 'B'))
      from systypes s, systypes st
      where s.usertype = @objid
         and s.type = st.type
         and st.usertype <> 18 /* ignore sysname */
         and st.usertype < 100
   else
      select Type_name = s.name,
         Storage_type = st.name,
         Length = s.length,
         Prec = '',
         Scale = '',
         'Nullable' = (select name from master.dbo.spt_values
               where number =
                  convert(int,s.allownulls)
                  and type = 'B'),
         Default_name = isnull(object_name(s.tdefault),
            (select name from master.dbo.spt_values
               where number = 2 and type = 'B')),
         Rule_name =
            isnull(object_name(s.domain),
            (select name from master.dbo.spt_values
               where number = 2 and type = 'B'))
      from systypes s, systypes st
      where s.usertype = @objid
         and s.type = st.type
         and st.usertype <> 18 /* ignore sysname */
         and st.usertype < 100


   return(0)
end

/*
**  It's in sysobjects so print out the information.
*/
select Name = o.name,
   Owner = user_name(uid),
   Type = v.name + x.name,
   When_created = o.crdate
from sysobjects o, master.dbo.spt_values v, master.dbo.spt_values x
where o.id = @objid
   and o.sysstat & 0xf = v.number
   and v.type = 'O'
   and x.type = 'R'
   and o.userstat & -32768 = x.number

print ''
execute sp_objectsegment @objid

/*
**  If the object is a system table, view, or user table, we want to check
**  out the object's columns here.
*/

if @sysstat & 0xf in (1, 2, 3)   -- system table, view, or user table.
begin

   create table #sphelptab
   (
      col_name char (30)   NULL,
      col_type char (30)   NULL,
      col_len     tinyint     NULL,
      col_prec char (5) NULL,
      col_scale   char (5) NULL,
      col_status  tinyint     NULL,
      colid    tinyint     NULL,
      type_systemdata  tinyint  NULL
   )

   insert into #sphelptab
      select c.name, t.name, c.length, convert(char(5),c.prec),
         convert(char(5),c.scale), c.status,
         c.colid, t.type
      from syscolumns c, systypes t
      where c.id = @objid
         and c.usertype *= t.usertype

   /*
   ** Don't display precision and scale for datatypes
   ** for which they not applicable.
   */
   update #sphelptab
      set col_prec = '', col_scale = ''
      where col_type in
         (select name from systypes where type not in
         (38,48,52,55,56,59,60,62,63,106,108,109,110,122))

   print ''
   select Column_name = col_name,
          Type = col_type,
          Length = col_len,
          Prec = col_prec,
          Scale = col_scale,

          Nullable = case when (col_status & 8) = 0 then
            (select name from master.dbo.spt_values
               where number = 0
                  and type = 'B')
            else
            (select name from master.dbo.spt_values
               where number = 1
                  and type = 'B')
            end,

          TrimTrailingBlanks =  -- Blanks; or 0x00 bytes for varbinary
               CASE
                  When (type_systemdata NOT IN (45,47,39,37))
                     Then '(n/a)'
                  When (col_status & 0x10) > 0
                     Then (Select   name
                              from  master.dbo.spt_values
                              where number = 0 and type = 'B')
                  Else
                          (Select   name
                              from  master.dbo.spt_values
                              where number = 1 and type = 'B')
               END,

          FixedLenNullInSource =
               CASE
                  When (type_systemdata NOT IN (45,47,39,37))
                     Then '(n/a)'
                  When (col_status & 0x20) = 0
                     Then (Select   name
                              from  master.dbo.spt_values
                              where number = 0 and type = 'B')
                  Else
                          (Select   name
                              from  master.dbo.spt_values
                              where number = 1 and type = 'B')
               END

   from #sphelptab
   order by colid

   -- See if there is an identity column.

   print ''
   select @colname = null
   select @colname = col_name from #sphelptab
      where (col_status & 128) = 128

   select 'Identity' = isnull(@colname,'No identity column defined.'),
          'Seed' = ident_seed(@objname),
          'Increment' = ident_incr(@objname)

end

/*
**  For procedures, the parameters of the procedures are stored in syscolumns.
*/
if @sysstat & 0xf = 4   -- stored procedure
begin
   /*
   **  Does the sproc have any parameters?
   */
   if exists (select * from syscolumns c, systypes t
         where c.id = @objid
            and c.usertype *= t.usertype)
   begin
      print ''
      select Parameter_name = c.name,
         Type = t.name,
         Length = c.length,
         Prec = c.prec,
         Scale = c.scale,
         Param_order=c.colid
      from syscolumns c, systypes t
      where c.id = @objid
         and c.usertype *= t.usertype
   end
end

/*
**  If the object is a table, check out the indexes.
*/
if @sysstat & 0xf in (1, 3)   -- system table or user table
   begin
      print ''
      execute sp_helpindex @objname
   end

/*
**  If the object is a table or view, check out the constraints.
*/
if @sysstat & 0xf in (1, 2, 3)   -- system table, view, or user table
   begin
      print ''
      execute sp_helpconstraint @objname,'nomsg'
   end

return (0)
go


dump transaction master with no_log
go



raiserror(15339,-1,-1,'sp_a_count_bits_on')
go
CREATE Procedure sp_a_count_bits_on --1995/10/01 21:04

/**
Originally written 1995/10/01 for sp_helprotect.
Count of ON bits is RETURNed.
**/

    @p_int                       int

   ,@p_num_bits_want_counted     smallint    = 32 --  1 thru 32 are valid.
AS

Set nocount on

Declare
    @one_bit_int_value           int
   ,@count_on_bits               smallint
   ,@count_bits                  smallint
   ,@test_bit_int_value          int
   ,@two_byte_int                int
   ,@b4_p_int                    binary(4)
   ,@pos_b4                      smallint
   ,@bits_counted_previously     smallint
   ,@bits_counted_on_previously  smallint


-----------------  Edit input parms  --------------------


IF (@p_int IS Null)
   RETURN 0

Select @b4_p_int = convert(binary(4),@p_int)



IF (     isnull(@p_num_bits_want_counted,0) <  1
    OR   isnull(@p_num_bits_want_counted,0) > 32
   )
   RETURN 0



------------------  Mainline  -----------------------------


----------------  LOOP 10:  Halves of datatype INTEGER  ---


Select
       @bits_counted_previously     = 0
      ,@bits_counted_on_previously  = 0

Select @pos_b4 = 3




WHILE (@pos_b4 >= 1
      )
   begin --loop 10


   Select @two_byte_int = convert(int,substring(@b4_p_int,@pos_b4,2))



----------------  LOOP 20:  Individual bit tests  -----------


   Select
       @test_bit_int_value    = 1
      ,@count_bits            = 0
      ,@count_on_bits         = 0



   WHILE (     @count_bits                              < 16
          AND  (@bits_counted_previously + @count_bits) <
                                           @p_num_bits_want_counted
         )
      begin --loop 20

      Select @count_bits = @count_bits + 1

      Select @one_bit_int_value = @two_byte_int & @test_bit_int_value


      if (@one_bit_int_value > 0)
         Select @count_on_bits = @count_on_bits + 1


      select @test_bit_int_value = @test_bit_int_value * 2

      end --loop 20




   Select @bits_counted_previously =
          @bits_counted_previously + @count_bits

   Select @bits_counted_on_previously =
          @bits_counted_on_previously + @count_on_bits


   Select @pos_b4 = @pos_b4 - 2


   end --loop 10



-----------------  Finalization  ---------------------------


RETURN @bits_counted_on_previously
go



raiserror(15339,-1,-1,'sp_helprotect')
go
CREATE PROCEDURE sp_helprotect -- 1996/03/04 13:02

    @name   character varying(99)  = '%'
   ,@username                character varying(66)  = '%'
   ,@grantorname                character varying(66)  = '%'
   ,@permissionarea      character varying(10)  = 'o s'
AS

/********
      Explanation of the parms...
      ---------------------------
@name:  Name of [Owner.]Object and Statement; meaning
                             for sysprotects.id and sysprotects.action at the
                             same time; thus see parm @permissionarea.
   Examples-   'user2.tb%'  , 'create%'

@username:  Name of the grantee (for sysprotects.uid).
   Examples-   'user2'

@grantorname:  Name of the grantor (for sysprotects.grantor).
   Examples-   '[^d]%' --Would prevent report rows which would
                       --  have 'dbo' as grantor.

@permissionarea:  O=Object, S=Statement; include all which apply.
   Examples-   'o'  , ',s'  , 'os'  , 'so'  , 's o'  , 's,o'
GeneMi
********/


Set nocount on

Declare
       @RetCode               integer
      ,@High2Int              integer
      ,@High4Int              integer
      ,@Int1                  integer
      ,@Int2                  integer
      ,@Int3                  integer
      ,@Int4                  integer
      ,@vc1                   varchar(64)

Declare
       @full_len_need_sysprots_cols    integer
      ,@num_col_bits_on                integer

Declare
       @Tinyint1              tinyint

Declare
       @charMaxLenOwner       character varying(11)
      ,@charMaxLenObject      character varying(11)
      ,@charMaxLenGrantee     character varying(11)
      ,@charMaxLenGrantor     character varying(11)
      ,@charMaxLenAction      character varying(11)
      ,@charMaxLenColumnName  character varying(11)

Declare
       @OrderBy               character varying(255)

      ,@OwnerName             character varying(66)
      ,@ObjectStatementName   character varying(66)

      ,@VarBinFront           varbinary(32)
      ,@VarBinBack            varbinary(32)
      ,@Bin1                  varbinary(1)

Declare
       @c1pkid                integer
      ,@c1id                  integer
      ,@c1uid                 smallint
      ,@c1action              tinyint
      ,@c1protecttype         tinyint
      ,@c1columns             varbinary(32)
      ,@c1grantor             smallint

Declare
       @t1_all_col_bits_on             tinyint
      ,@t1_columns_orig_datalength     smallint
      ,@t1_columns_flip_datalength     smallint
      ,@t1_columns_flipped             varbinary(32)
      ,@t1_columns_orig                varbinary(32)
      ,@t1_max_colid                   smallint
      ,@t1_type1code                   char(6)
      ,@t1_objtype                     char(2)
      ,@t1_new_bit_on                  tinyint



----------------------------------------------------------------------------
--
--             Initialization
--
----------------------------------------------------------------------------


Select
       @RetCode               = 0            -- 0=good ,1=bad
      ,@High2Int              =      32767
      ,@High4Int              = 2147483647


-----------------------  Input parm edits  ---------------------


Select @permissionarea = upper( isnull(@permissionarea,'?') )

IF (     charindex('O',@permissionarea) <= 0
    AND  charindex('S',@permissionarea) <= 0
   )
   begin
   RaisError(15300,-1,-1 ,@permissionarea,'o,s')
   Select @RetCode = 1
   GOTO LABEL_86RETURN
   end


--------


IF (     @name IS Null
    OR   @username              IS Null
    OR   @grantorname              IS Null
   )
   begin
   RaisError(15008,-1,-1)  --No nulls allowed, use '%' instead.
   Select @RetCode = 1
   GOTO LABEL_86RETURN
   end


--------  Derive OwnerName


IF (@name LIKE '%.%.%')
   begin
   Select @Int1 = charindex('.',@name)

   Select @vc1 = substring(@name,1,(@Int1-1))

   IF (@vc1 <> db_name())
      begin
      RaisError(15302,-1,-1)  --Do not qualify with DB name.
      Select @RetCode = 1
      GOTO LABEL_86RETURN
      end

   Select @name =
            substring(@name,(@Int1+1),255)
   end




IF (@name LIKE '%.%')

   Select
          @OwnerName    =
               substring ( @name
                          ,1
                          ,charindex('.' ,@name) - 1
                         )
         ,@ObjectStatementName   =
               substring ( @name
                          ,charindex('.' ,@name) + 1
                          ,Datalength(@name)
                         )
ELSE

   Select
          @OwnerName             = '%'
         ,@ObjectStatementName   = @name



----------------------------------------------------------------------------
--
--             MainLine:  Phase 10
--
----------------------------------------------------------------------------



--------------  create a fixed copy of sysprotects for processing  --------


-- Simpler when bit 0 is OFF, because other ON bits really mean ON.


CREATE Table #t1_Prots
   (
    pkid                int               not null    identity(1,1)
   ,type1code           char(6)           not null
   ,status              smallint          not null -- 0=unprocessed
   ,objtype             char(2)               null

   ,id                  int                   null
   ,uid                 smallint              null
   ,action              tinyint               null
   ,protecttype         tinyint               null
   ,grantor             smallint              null
   ,columns_orig        varbinary(32)         null

   ,columns_flipped     varbinary(32)         null
   ,max_colid           smallint              null
   ,all_col_bits_on     tinyint               null
   ,new_bit_on          tinyint               null  -- 1=yes on
   )



CREATE Unique    Clustered Index ix1 on #t1_Prots (pkid)
CREATE Unique Nonclustered Index ix2 on #t1_Prots (id,uid,action,protecttype,grantor)




-- Will loop through OFF=ON rows to finish the copy of sysprotects.
-- For these rows, ON=ON only for the 0x01 bit.

DECLARE Cursor1_Prots
   Insensitive
   Cursor For
SELECT
             id
            ,uid
            ,action
            ,protecttype
            ,columns
            ,grantor
      from
             sysprotects
   For Read Only



OPEN Cursor1_Prots



---------------
WHILE (110=110)
   begin


   FETCH
             Next
      from
             Cursor1_Prots
      into
             @c1id
            ,@c1uid
            ,@c1action
            ,@c1protecttype
            ,@c1columns
            ,@c1grantor



   IF (@@fetch_status <> 0)
      begin
      Deallocate Cursor1_Prots
      BREAK
      end


   Select
          @t1_type1code               = Null
         ,@t1_columns_orig            = @c1columns
         ,@t1_columns_orig_datalength = datalength(@c1columns) --Null if null.
         ,@t1_max_colid               = Null
         ,@t1_all_col_bits_on         = Null
         ,@t1_new_bit_on              = Null
         ,@t1_type1code               = '1Regul'
         ,@t1_objtype                 = Null


   SELECT
             @t1_objtype   = ob.type
      from
             sysobjects    ob
      where
             ob.id         = @c1id




---------------  Handle the simple cases first.


   IF (@c1columns IS Null)
      begin
      Select @t1_type1code       = '2Simpl'
            ,@t1_columns_flipped = @c1columns
      GOTO LABEL_49INSERT1
      end



---------------------  Upcoming steps...  ------------------


--Pad columns bits variable to full needed length.
--Flip most bits in columns bits var, so ON=ON consistently.
--Count ON bits in columns bits var, compare to max syscolumns colid.
--Insert #t2



------------------------------------------------------------------------
--------------  Must first append some 0x00 paddings perhaps  ----------


   SELECT
             @t1_max_colid = max(colid)  --Cannot come back Null this time.
      from
             syscolumns
      where
             id    = @c1id



--As needed, later add 1 bitcount for the All/New 0x01 "zero" bit
--   of the first sysprotects.columns bytes.


   Select @full_len_need_sysprots_cols =
          (@t1_max_colid + 1) / 8       --8 bits per byte; trunc not round


   IF ((@t1_max_colid + 1) % 8 > 0)
      Select @full_len_need_sysprots_cols =
             @full_len_need_sysprots_cols + 1


   --------------------------------------
   WHILE (datalength(@c1columns) < @full_len_need_sysprots_cols
         )
      begin                      --Pad the columns bits variable.

      Select @c1columns = @c1columns + 0x00 --convert(binary(1),0x00) is trimmed

      end --loop to pad


   Select @t1_columns_flip_datalength = datalength(@c1columns)




------------------------------------------------------------------------
-----------------  Flip most bits, so never OFF=ON anymore  ------------



--Bit operators do not work against binarys, so use int.



   Select @Int1 = convert(int,substring(@c1columns,1,1))

   IF (@Int1 & 1 = 0
      )
      begin
      Select @t1_new_bit_on = 0

      GOTO LABEL_73BITONMEANSON  --ON already means ON consistently

      end
   ELSE
      Select @t1_new_bit_on = 1

   IF (@t1_objtype = 'V ')
      begin
      Select @t1_new_bit_on = 0

      Select @Int1 = @Int1 & ~1  --Views cannot get "New" columns.
      Select @Bin1 = convert(binary(1),@Int1)  --Len 1 not trimmed.
      Select @c1columns = @Bin1 +
                          substring(@c1columns,2,@t1_columns_flip_datalength-1)
      end





   Select @Int1 = 0  --Position in substring


--Proper for Revoke rows as well as for Grant rows.
   ------------------------------------
   WHILE (@t1_columns_flip_datalength > @Int1
         )
      begin  --Total length > current byte position, more Flipping.


      Select @Int1 = @Int1 + 1  --increment pos


      IF (1 = @Int1)
         begin
         Select @VarBinFront = Null
         Select @Bin1        = substring(@c1columns ,@Int1   ,1)
         Select @VarBinBack  = substring(@c1columns ,@Int1+1
                                        ,@t1_columns_flip_datalength-@Int1)
         end


      IF (     1                           < @Int1
          AND  @t1_columns_flip_datalength > @Int1
         )
         begin
         Select @VarBinFront = substring(@c1columns ,1       ,@Int1-1)
         Select @Bin1        = substring(@c1columns ,@Int1   ,1)
         Select @VarBinBack  = substring(@c1columns ,@Int1+1
                                        ,@t1_columns_flip_datalength-@Int1)
         end


      IF (@t1_columns_flip_datalength = @Int1)
         begin
         Select @VarBinFront = substring(@c1columns ,1       ,@Int1-1)
         Select @Bin1        = substring(@c1columns ,@Int1   ,1)
         Select @VarBinBack  = Null
         end



      Select @Int3 = convert(int,@Bin1)



      IF (     @Int1     = 1  --Byte containing the All/NewCols bit (0x01)
          AND  @Int3 & 1 > 0  --NewCols bit on
         )
         Select @Int3 = @Int3 & (~1)  --Flip (here Turn off) the 0x01 bit



      --Flip all the bits (so 0x01 bit return to its initial setting).
      Select @Int3 = ~@Int3

      Select @Bin1      = convert(binary(1),@Int3)

      Select @c1columns = @VarBinFront + @Bin1 + @VarBinBack


      end  --loop to flip most bits



LABEL_73BITONMEANSON:



------------------------------------------------------------------------------
-----------  Determine whether all current columns are individually ON!  -----



LABEL_42LOOP110EACHBIT:



   Select
       @Int1                     = 0  --POSition
      ,@num_col_bits_on          = 0



   WHILE (37=37)
      begin

      Select @Int1 = @Int1 + 1

      IF (@Int1 > @full_len_need_sysprots_cols)
         BREAK


      Select @Int2 = convert( int ,substring(@c1columns ,@Int1 ,1) )



      IF (@Int1 <= (@t1_max_colid + 1) / 8    --truncated, not rounded
         )
         Select @Int4 = 8  --num of Bits want tested in this byte/number
      ELSE
         Select @Int4 = (@t1_max_colid + 1) % 8



      Execute @Int3 =  sp_a_count_bits_on  @Int2 ,@Int4


      IF (     @Int1       = 1
          AND  @Int2 & 1   > 0
         )
         Select @Int3 = @Int3 - 1  --Do not count "New" bit.


      Select @num_col_bits_on = @num_col_bits_on + @Int3


      end --loop 37




   IF (@t1_max_colid = @num_col_bits_on)   --  @ < @  impossible
      Select @t1_all_col_bits_on = 1
   ELSE
      Select @t1_all_col_bits_on = 0




-------------------------------------------------------------------------
----------------  Finally, Ins modified copy of sysprotects row  --------



LABEL_49INSERT1:



   INSERT
             #t1_Prots
         (
           --pkid,
             type1code
            ,status
            ,objtype
            ,id
            ,uid
            ,action
            ,protecttype
            ,grantor
            ,columns_orig
            ,columns_flipped
            ,max_colid
            ,all_col_bits_on
            ,new_bit_on
         )
      Values
         (
             --identity,
             @t1_type1code
            ,0
            ,@t1_objtype
            ,@c1id
            ,@c1uid
            ,@c1action
            ,@c1protecttype
            ,@c1grantor
            ,@t1_columns_orig
            ,@c1columns
            ,@t1_max_colid
            ,@t1_all_col_bits_on
            ,@t1_new_bit_on
         )


   end  --loop 110




----------------------------------------------------------------------------
--
--             MainLine:  Phase 20
--
----------------------------------------------------------------------------



------------------  create smart temporary holding table  --------------


CREATE TABLE #t2_SmartProts    --The z's avoid label/colname sorting conflicts
   (
    zPKId               int            NOT Null    Identity(1,1)
   ,zType2Code          char(6)        NOT Null
   ,zStatus             smallint       NOT Null
   ,zOwnerName          varchar(30)    NOT Null
   ,zObjectName         varchar(30)    NOT Null
   ,zGranteeName        varchar(30)    NOT Null
   ,zGrantorName        varchar(30)    NOT Null
   ,zProtectTypeNum     tinyint        NOT Null --Revoke rows have effects.
   ,zProtectTypeName    char(10)           Null
   ,zProtectCategory    char(2)            Null
   ,zWGO                char(3)            Null
   ,zActionNum          tinyint        NOT Null
   ,zActionName         varchar(20)        Null
   ,zActionCategory     char(2)            Null
   ,zColumnName         varchar(30)    NOT Null
   ,zColId              smallint           Null
   )


CREATE Unique Clustered
   INDEX    ix1
   ON       #t2_SmartProts
      (zPKId)



---------------------  Populate the holding table  -----------------------


--------  Only non objects  ---


IF (charindex('S',@permissionarea) <= 0)
   GOTO LABEL_1NOOBJ



INSERT
      into   #t2_SmartProts
   (
  --zPKId,
    zType2Code
   ,zStatus
   ,zOwnerName
   ,zObjectName
   ,zGranteeName
   ,zGrantorName
   ,zProtectTypeNum
   ,zProtectTypeName
   ,zProtectCategory
   ,zWGO
   ,zActionNum
   ,zActionName
   ,zActionCategory
   ,zColumnName
   ,zColId
   )
   SELECT
           --identity,
             '1NOOBJ'
            ,0
            ,'.'
            ,'.'
            ,usr2.name
            ,usr3.name

            ,prot1.protecttype
            ,Null
            ,Null

            ,Null

            ,prot1.action
            ,Null
            ,Null

            ,'.'
            ,Null
      from
             #t1_Prots              prot1
            ,sysusers               usr2
            ,sysusers               usr3
            ,master.dbo.spt_values  val1
      where
             prot1.columns_orig IS Null
      and    prot1.objtype      IS Null
      and
             usr2.uid      =  prot1.uid
      and    usr3.uid      =  prot1.grantor
      and
             usr2.name  LIKE  @username
      and    usr3.name  LIKE  @grantorname
      and
             val1.number   =  prot1.action
      and    val1.type     = 'T'
      and
             val1.name  LIKE  @ObjectStatementName
      and
             prot1.id      = 0
      and
             prot1.status  = 0




UPDATE
             #t1_Prots
   set
             prot1.status  = prot1.status + Power(2,1)
   from
             #t1_Prots              prot1
            ,sysusers               usr2
            ,sysusers               usr3
            ,master.dbo.spt_values  val1
      where
             prot1.columns_orig IS Null
      and    prot1.objtype      IS Null
      and
             usr2.uid      =  prot1.uid
      and    usr3.uid      =  prot1.grantor
      and
             usr2.name  LIKE  @username
      and    usr3.name  LIKE  @grantorname
      and
             val1.number   =  prot1.action
      and    val1.type     = 'T'
      and
             val1.name  LIKE  @ObjectStatementName
      and
             prot1.id      = 0
      and
             prot1.status  = 0



LABEL_1NOOBJ:




-----------  Only objects, and only null columns  ---------



IF (charindex('O',@permissionarea) <= 0)
   GOTO LABEL_2OBNUL



INSERT
      into   #t2_SmartProts
   (
  --zPKId,
    zType2Code
   ,zStatus
   ,zOwnerName
   ,zObjectName
   ,zGranteeName
   ,zGrantorName
   ,zProtectTypeNum
   ,zProtectTypeName
   ,zProtectCategory
   ,zWGO
   ,zActionNum
   ,zActionName
   ,zActionCategory
   ,zColumnName
   ,zColId
   )
   SELECT
           --identity,
             '2OBNUL'
            ,0
            ,usr1.name
            ,obj1.name
            ,usr2.name
            ,usr3.name

            ,prot1.protecttype
            ,Null
            ,Null

            ,Null

            ,prot1.action
            ,Null
            ,Null

            ,'.'

            ,Null
      from
             #t1_Prots              prot1
            ,sysobjects             obj1
            ,sysusers               usr1  --obj owner
            ,sysusers               usr2  --grantee
            ,sysusers               usr3  --grantor
      where
             prot1.id         = obj1.id
      and    prot1.objtype    = obj1.type
      and
             obj1.type       IN ('X ' ,'P ' ,'U ' ,'S ' ,'V ')
      and
             usr1.uid      =  obj1.uid
      and    usr2.uid      =  prot1.uid
      and    usr3.uid      =  prot1.grantor
      and
             usr1.name  LIKE  @OwnerName
      and    obj1.name  LIKE  @ObjectStatementName
      and    usr2.name  LIKE  @username
      and    usr3.name  LIKE  @grantorname
      and
             prot1.columns_orig IS Null
      and
             prot1.status  = 0





UPDATE
             #t1_Prots
   set
             prot1.status  = prot1.status + Power(2,2)
      from
             #t1_Prots              prot1
            ,sysobjects             obj1
            ,sysusers               usr1  --obj owner
            ,sysusers               usr2  --grantee
            ,sysusers               usr3  --grantor
      where
             prot1.id         = obj1.id
      and    prot1.objtype    = obj1.type
      and
             obj1.type       IN ('X ' ,'P ' ,'U ' ,'S ' ,'V ')
      and
             usr1.uid      =  obj1.uid
      and    usr2.uid      =  prot1.uid
      and    usr3.uid      =  prot1.grantor
      and
             usr1.name  LIKE  @OwnerName
      and    obj1.name  LIKE  @ObjectStatementName
      and    usr2.name  LIKE  @username
      and    usr3.name  LIKE  @grantorname
      and
             prot1.columns_orig IS Null
      and
             prot1.status  = 0




LABEL_2OBNUL:




---------------------  Only tables where both All & New  -----------



IF (charindex('O',@permissionarea) <= 0)
   GOTO LABEL_3TBAN



INSERT
      into   #t2_SmartProts
   (
  --zPKId,
    zType2Code
   ,zStatus
   ,zOwnerName
   ,zObjectName
   ,zGranteeName
   ,zGrantorName
   ,zProtectTypeNum
   ,zProtectTypeName
   ,zProtectCategory
   ,zWGO
   ,zActionNum
   ,zActionName
   ,zActionCategory
   ,zColumnName
   ,zColId
   )
   SELECT
           --identity,
             '3TBAN '
            ,0
            ,usr1.name
            ,obj1.name
            ,usr2.name
            ,usr3.name

            ,prot1.protecttype
            ,Null
            ,Null

            ,Null

            ,prot1.action
            ,Null
            ,Null

            ,'(All+New)'
            ,Null
      from
             #t1_Prots              prot1
            ,sysobjects             obj1
            ,sysusers               usr1  --obj owner
            ,sysusers               usr2  --grantee
            ,sysusers               usr3  --grantor
      where
             prot1.id         = obj1.id
      and    prot1.objtype    = obj1.type
      and
             obj1.type       IN ('S ' ,'U ')
      and
             usr1.uid         =  obj1.uid
      and    usr2.uid         =  prot1.uid
      and    usr3.uid         =  prot1.grantor
      and
             usr1.name     LIKE  @OwnerName
      and    obj1.name     LIKE  @ObjectStatementName
      and    usr2.name     LIKE  @username
      and    usr3.name     LIKE  @grantorname
      and
             prot1.all_col_bits_on  = 1
      and    prot1.new_bit_on       = 1
      and
             prot1.status     = 0




UPDATE
             #t1_Prots
   set
             prot1.status  = prot1.status + Power(2,3)
      from
             #t1_Prots              prot1
            ,sysobjects             obj1
            ,sysusers               usr1  --obj owner
            ,sysusers               usr2  --grantee
            ,sysusers               usr3  --grantor
      where
             prot1.id         = obj1.id
      and    prot1.objtype    = obj1.type
      and
             obj1.type       IN ('S ' ,'U ')
      and
             usr1.uid         =  obj1.uid
      and    usr2.uid         =  prot1.uid
      and    usr3.uid         =  prot1.grantor
      and
             usr1.name     LIKE  @OwnerName
      and    obj1.name     LIKE  @ObjectStatementName
      and    usr2.name     LIKE  @username
      and    usr3.name     LIKE  @grantorname
      and
             prot1.all_col_bits_on  = 1
      and    prot1.new_bit_on       = 1
      and
             prot1.status     = 0



/********
sysprotects.columns  and  spt_values.type='P':

NUMBER=ColId  ,LOW=SubstringPosOfByte  ,HIGH=Power(2,0-7 bit pos)

      NUMBER=0 detects 0x01 bit.   The 0x01 bit being on means New
columns will automatically be picked up.   And if sysprotects.columns
equals 0x01 entirely, then All Current and New columns are included.
Note that for Ins/Del Null should be treated as =0x01.
      In the first post-6.0 release the bit on/off meanings have reversed,
except for the 0x01 bit.

      Note that in MS SQL 6.0 and back, when the 0x01 bit was on
in sysprotects.columns, that no other bits were ever on or mattered.
After 6.0 the 0x01 can be on with a mixture of other bits which matter.
********/



LABEL_3TBAN:




-----------------  TBs & VWs where only All columns (not New)  -----------



IF (charindex('O',@permissionarea) <= 0)
   GOTO LABEL_4TBVWA



INSERT
      into   #t2_SmartProts
   (
  --zPKId,
    zType2Code
   ,zStatus
   ,zOwnerName
   ,zObjectName
   ,zGranteeName
   ,zGrantorName
   ,zProtectTypeNum
   ,zProtectTypeName
   ,zProtectCategory
   ,zWGO
   ,zActionNum
   ,zActionName
   ,zActionCategory
   ,zColumnName
   ,zColId
   )
   SELECT
           --identity,
             '4TBVWA'
            ,0
            ,usr1.name
            ,obj1.name
            ,usr2.name
            ,usr3.name

            ,prot1.protecttype
            ,Null
            ,Null

            ,Null

            ,prot1.action
            ,Null
            ,Null

            ,'(All)'
            ,Null
      from
             #t1_Prots              prot1
            ,sysobjects             obj1
            ,sysusers               usr1  --obj owner
            ,sysusers               usr2  --grantee
            ,sysusers               usr3  --grantor
      where
             prot1.id         = obj1.id
      and    prot1.objtype    = obj1.type
      and
             obj1.type       IN ('S ' ,'U ' ,'V ')
      and
             usr1.uid         =  obj1.uid
      and    usr2.uid         =  prot1.uid
      and    usr3.uid         =  prot1.grantor
      and
             usr1.name     LIKE  @OwnerName
      and    obj1.name     LIKE  @ObjectStatementName
      and    usr2.name     LIKE  @username
      and    usr3.name     LIKE  @grantorname
      and
             prot1.all_col_bits_on  = 1
      and    prot1.new_bit_on       = 0
      and
             prot1.status     = 0




UPDATE
             #t1_Prots
   set
             prot1.status  = prot1.status + Power(2,4)
      from
             #t1_Prots              prot1
            ,sysobjects             obj1
            ,sysusers               usr1  --obj owner
            ,sysusers               usr2  --grantee
            ,sysusers               usr3  --grantor
      where
             prot1.id         = obj1.id
      and    prot1.objtype    = obj1.type
      and
             obj1.type       IN ('S ' ,'U ' ,'V ')
      and
             usr1.uid         =  obj1.uid
      and    usr2.uid         =  prot1.uid
      and    usr3.uid         =  prot1.grantor
      and
             usr1.name     LIKE  @OwnerName
      and    obj1.name     LIKE  @ObjectStatementName
      and    usr2.name     LIKE  @username
      and    usr3.name     LIKE  @grantorname
      and
             prot1.all_col_bits_on  = 1
      and    prot1.new_bit_on       = 0
      and
             prot1.status     = 0




LABEL_4TBVWA:




--------  TBs & VWs where not all columns were granted (avoid New)  ---------



IF (charindex('O',@permissionarea) <= 0)
   GOTO LABEL_5TBVWC



INSERT
      into   #t2_SmartProts
   (
  --zPKId,
    zType2Code
   ,zStatus
   ,zOwnerName
   ,zObjectName
   ,zGranteeName
   ,zGrantorName
   ,zProtectTypeNum
   ,zProtectTypeName
   ,zProtectCategory
   ,zWGO
   ,zActionNum
   ,zActionName
   ,zActionCategory
   ,zColumnName
   ,zColId
   )
   SELECT
           --identity,
             '5TBVWC'
            ,0
            ,usr1.name
            ,obj1.name
            ,usr2.name
            ,usr3.name

            ,prot1.protecttype
            ,Null
            ,Null

            ,Null

            ,prot1.action
            ,Null
            ,Null

            ,col_name ( prot1.id ,val1.number )
            ,val1.number
      from
             #t1_Prots              prot1
            ,sysobjects             obj1
            ,sysusers               usr1  --obj owner
            ,sysusers               usr2  --grantee
            ,sysusers               usr3  --grantor
            ,master.dbo.spt_values  val1
      where
             prot1.id         = obj1.id
      and    prot1.objtype    = obj1.type
      and
             obj1.type       IN ('S ' ,'U ' ,'V ')
      and
             usr1.uid      =  obj1.uid
      and    usr2.uid      =  prot1.uid
      and    usr3.uid      =  prot1.grantor
      and
             usr1.name  LIKE  @OwnerName
      and    obj1.name  LIKE  @ObjectStatementName
      and    usr2.name  LIKE  @username
      and    usr3.name  LIKE  @grantorname
      and
             prot1.all_col_bits_on = 0
      and
             val1.type        = 'P'
      and
             val1.number BETWEEN 1 AND prot1.max_colid
      and
             val1.low        <= Datalength(prot1.columns_flipped)
      and
             val1.high  &
               convert(int, substring(prot1.columns_flipped ,val1.low ,1) )
                        <> 0
      and    prot1.status     = 0




UPDATE
             #t1_Prots
   set
             prot1.status  = prot1.status + Power(2,5)
      from
             #t1_Prots              prot1
            ,sysobjects             obj1
            ,sysusers               usr1  --obj owner
            ,sysusers               usr2  --grantee
            ,sysusers               usr3  --grantor
            ,master.dbo.spt_values  val1
      where
             prot1.id         = obj1.id
      and    prot1.objtype    = obj1.type
      and
             obj1.type       IN ('S ' ,'U ' ,'V ')
      and
             usr1.uid      =  obj1.uid
      and    usr2.uid      =  prot1.uid
      and    usr3.uid      =  prot1.grantor
      and
             usr1.name  LIKE  @OwnerName
      and    obj1.name  LIKE  @ObjectStatementName
      and    usr2.name  LIKE  @username
      and    usr3.name  LIKE  @grantorname
      and
             prot1.all_col_bits_on = 0
      and
             val1.type        = 'P'
      and
             val1.number BETWEEN 1 AND prot1.max_colid
      and
             val1.low        <= Datalength(prot1.columns_flipped)
      and
             val1.high  &
               convert(int, substring(prot1.columns_flipped ,val1.low ,1) )
                        <> 0
      and    prot1.status     = 0




LABEL_5TBVWC:



----------  Only tables where only New columns are authorized!  ---------



IF (charindex('O',@permissionarea) <= 0)
   GOTO LABEL_6TBNEW



INSERT
      into   #t2_SmartProts
   (
  --zPKId,
    zType2Code
   ,zStatus
   ,zOwnerName
   ,zObjectName
   ,zGranteeName
   ,zGrantorName
   ,zProtectTypeNum
   ,zProtectTypeName
   ,zProtectCategory
   ,zWGO
   ,zActionNum
   ,zActionName
   ,zActionCategory
   ,zColumnName
   ,zColId
   )
   SELECT
           --identity,
             '6TBNEW'
            ,0
            ,usr1.name
            ,obj1.name
            ,usr2.name
            ,usr3.name

            ,prot1.protecttype
            ,Null
            ,Null

            ,Null

            ,prot1.action
            ,Null
            ,Null

            ,'(New)'
            ,Null
      from
             #t1_Prots              prot1
            ,sysobjects             obj1
            ,sysusers               usr1  --obj owner
            ,sysusers               usr2  --grantee
            ,sysusers               usr3  --grantor
      where
             prot1.id         = obj1.id
      and    prot1.objtype    = obj1.type
      and
             obj1.type       IN ('S ' ,'U ')
      and
             usr1.uid         =  obj1.uid
      and    usr2.uid         =  prot1.uid
      and    usr3.uid         =  prot1.grantor
      and
             usr1.name     LIKE  @OwnerName
      and    obj1.name     LIKE  @ObjectStatementName
      and    usr2.name     LIKE  @username
      and    usr3.name     LIKE  @grantorname
      and
             prot1.all_col_bits_on  = 0
      and    prot1.new_bit_on       = 1
      and
             prot1.status     IN (0,32)  --Some rows processed twice.




UPDATE
             #t1_Prots
   set
             prot1.status  = prot1.status + Power(2,6)
      from
             #t1_Prots              prot1
            ,sysobjects             obj1
            ,sysusers               usr1  --obj owner
            ,sysusers               usr2  --grantee
            ,sysusers               usr3  --grantor
      where
             prot1.id         = obj1.id
      and    prot1.objtype    = obj1.type
      and
             obj1.type       IN ('S ' ,'U ')
      and
             usr1.uid         =  obj1.uid
      and    usr2.uid         =  prot1.uid
      and    usr3.uid         =  prot1.grantor
      and
             usr1.name     LIKE  @OwnerName
      and    obj1.name     LIKE  @ObjectStatementName
      and    usr2.name     LIKE  @username
      and    usr3.name     LIKE  @grantorname
      and
             prot1.all_col_bits_on  = 0
      and    prot1.new_bit_on       = 1
      and
             prot1.status     IN (0,32)  --Some rows processed twice.




LABEL_6TBNEW:



----------------------------------------------------------------------------
--
--             MainLine:  Phase 30
--
----------------------------------------------------------------------------


IF NOT EXISTS (SELECT * From #t2_SmartProts)
   begin
   RaisError(15330,-1,-1)  --Error, no matching rows.
   Select @RetCode = 1

   GOTO LABEL_86RETURN
   end


----------------  Translations on the held data  --------------------


UPDATE
             #t2_SmartProts
      set
             zProtectTypeName = val2.name

            ,zProtectCategory =
               CASE
                  When zProtectTypeNum IN (204,205) Then 'Gr' --Grant
                  When zProtectTypeNum IN (206    ) Then 'Re' --Revoke
                  Else                   '??'
               END

            ,zWGO             =
               CASE zProtectTypeNum
                  When 204 Then 'YES'
                  When 205 Then 'no'
                  When 206 Then '.'
                  Else          '??'
                END

            ,zActionName      = val1.name

            ,zActionCategory  =
               CASE
                  When zActionNum IN ( 26, 57,193,195,196,197,224,228,235)
                                                    Then 'Ob' --Object
                  When zActionNum IN (198,203,207,222,233,236) Then 'St' --Statement
                  Else                   '??'
               END

            ,zColId        = isnull ( zColId , -123 )
      from
             #t2_SmartProts         tbz
            ,master.dbo.spt_values  val1
            ,master.dbo.spt_values  val2
      where
             val1.number   = tbz.zActionNum
      and    val1.type     = 'T'
      and
             val2.number   = tbz.zProtectTypeNum
      and    val2.type     = 'T' --T is overloaded.




-----------------  Dynamically tailor the report sorted order  ---------


--Specialized sort sequences not really needed in this app.

Select @OrderBy = '
  zActionCategory
 ,Owner            ,Object
 ,Grantee          ,Grantor
 ,zProtectCategory ,ProtectType
 ,Action
'



-----------------  Calculate dynamic display col widths  ---------------


SELECT
             @charMaxLenOwner       =
                  convert ( varchar
                           ,isnull ( max(datalength(zOwnerName)) ,5)
                          )
            ,@charMaxLenObject      =
                  convert ( varchar
                           ,isnull ( max(datalength(zObjectName)) ,6)
                          )
            ,@charMaxLenGrantee     =
                  convert ( varchar
                           ,isnull ( max(datalength(zGranteeName)) ,7)
                          )
            ,@charMaxLenGrantor     =
                  convert ( varchar
                           ,isnull ( max(datalength(zGrantorName)) ,7)
                          )
            ,@charMaxLenAction      =
                  convert ( varchar
                           ,isnull ( max(datalength(zActionName)) ,6)
                          )
            ,@charMaxLenColumnName  =
                  convert ( varchar
                           ,isnull ( max(datalength(zColumnName)) ,6)
                          )
      from
             #t2_SmartProts



--------------------  Output the report  -----------------------


EXECUTE(
'
Set nocount off


SELECT
          ''Owner''       = substring (zOwnerName   ,1 ,'
                                       + @charMaxLenOwner   + ')

         ,''Object''      = substring (zObjectName  ,1 ,'
                                       + @charMaxLenObject  + ')

         ,''Grantee''     = substring (zGranteeName ,1 ,'
                                       + @charMaxLenGrantee + ')

         ,''Grantor''     = substring (zGrantorName ,1 ,'
                                       + @charMaxLenGrantor + ')

      -- ,''WGO''         = zWGO

         ,''ProtectType'' = zProtectTypeName

         ,''Action''      = substring (zActionName ,1 ,'
                                       + @charMaxLenAction + ')

         ,''Column''      = substring (zColumnName ,1 ,'
                                       + @charMaxLenColumnName + ')
   from
          #t2_SmartProts
   order by
          ' + @OrderBy + '
         ,zColId  --Multiple  -123s  ( <0 )  possible
         ,zColumnName


Set nocount on
'
)



----------------------------------------------------------------------------
--
--             Finalization
--
----------------------------------------------------------------------------


LABEL_86RETURN:



IF (object_id('tempdb..#t1_Prots') IS NOT Null)
            DROP TABLE #t1_Prots

IF (object_id('tempdb..#t2_SmartProts') IS NOT Null)
            DROP TABLE #t2_SmartProts

Return @RetCode
go



raiserror(15339,-1,-1,'sp_helptext')
go
create procedure sp_helptext
@objname varchar(92)
as
declare @dbname varchar(30)

set nocount on

/*
**  Make sure the @objname is local to the current database.
*/
if (@objname like '%.%.%' and
        substring(@objname, 1, charindex('.', @objname) - 1) <> db_name() )
        begin
                raiserror(15250,-1,-1)
                return (1)
        end

/*
**  See if @objname exists.
*/
if (object_id(@objname) is null)
        begin
		select @dbname = db_name()
		raiserror(15009,-1,-1,@objname,@dbname)
                return (1)
        end

/*
**  Find out how many lines of text are coming back,
**  and return if there are none.
*/
if (select count(*) from syscomments where id = object_id(@objname)) = 0
        begin
                raiserror(15197,-1,-1,@objname)
                return (1)
        end

if (select count(*) from syscomments where id = object_id(@objname)
	and texttype & 4 = 0) = 0
        begin
                print 'The object''s comments have been encrypted.'
                return (0)
        end

/*
**  Else get the text.
*/
select text from syscomments where id = object_id(@objname)

return (0)
go



--Running this create AFTER create sp_helpgroup.
raiserror(15339,-1,-1,'sp_helpuser')
go
CREATE PROCEDURE sp_helpuser  --1996/03/04 13:06

    @name_in_db       varchar(30)    = NULL --User,Group,Alias
AS

Set nocount on


Declare
    @RetCode               int
   ,@_rowcount             int

Declare
    @charMaxLen_UsName     varchar(11)
   ,@charMaxLen_GrName     varchar(11)
   ,@charMaxLen_LoName     varchar(11)
   ,@charMaxLen_DbName     varchar(11)

Declare
    @Name1Type             char(2)
   ,@CMaxUsUID             smallint

--------

Select
    @RetCode               = 0
   ,@Name1Type             = Null
   ,@CMaxUsUID             = 16383


-----------------------  create holding table  --------------------


Create Table #tb1_uga
   (
    zUserName        varchar(30)        Null
   ,zGroupName       varchar(30)        Null
   ,zLoginName       varchar(30)        Null
   ,zDefDBName       varchar(30)        Null
   ,zUID             smallint           Null
   ,zSUID            smallint           Null
   )



-------------  What type of value (U,G,A) was input?  --------------


-------- NULL

IF (@name_in_db IS Null)
   begin

   Select @Name1Type = '-'


   INSERT
         into   #tb1_uga
               (
                zUserName
               ,zGroupName
               ,zLoginName
               ,zDefDBName
               ,zUID
               ,zSUID
               )
      SELECT
                   usu.name
                  ,usg.name
                  ,lo.name
                  ,lo.dbname
                  ,usu.uid
                  ,usu.suid
         from
                   sysusers               usu
                  ,sysusers               usg
                  ,master.dbo.syslogins   lo
         where
                   usu.suid   *= lo.suid
         and       usu.gid    *= usg.uid
         and
                   usg.uid     > @CMaxUsUID
         and
                   usu.uid    <= @CMaxUsUID
         and       usu.uid    >  0



   GOTO LABEL_25NAME1TYPEKNOWN

   end


-------- USER

INSERT
         into   #tb1_uga
               (
                zUserName
               ,zGroupName
               ,zLoginName
               ,zDefDBName
               ,zUID
               ,zSUID
               )
      SELECT
                   usu.name
                  ,usg.name
                  ,lo.name
                  ,lo.dbname
                  ,usu.uid
                  ,usu.suid
         from
                   sysusers               usu
                  ,sysusers               usg
                  ,master.dbo.syslogins   lo
         where
                   usu.suid   *= lo.suid
         and       usu.gid    *= usg.uid
         and
                   usg.uid     > @CMaxUsUID
         and
                   usu.uid    <= @CMaxUsUID
         and       usu.uid    >  0
         and
                   usu.name    = @name_in_db


Select @_rowcount = @@rowcount


IF (@_rowcount > 0)
   begin
   Select @Name1Type = 'US'

   GOTO LABEL_25NAME1TYPEKNOWN

   end


-------- ALIAS

INSERT
         into   #tb1_uga
               (
                zUserName
               ,zGroupName
               ,zLoginName
               ,zDefDBName
               ,zUID
               ,zSUID
               )
      SELECT
                   usu.name
                  ,usg.name
                  ,lo.name
                  ,lo.dbname
                  ,usu.uid
                  ,usu.suid
         from
                   sysusers               usu
                  ,sysusers               usg
                  ,master.dbo.syslogins   lo
                  ,sysalternates          al
         where
                   usu.suid   *= lo.suid
         and       usu.gid    *= usg.uid
         and
                   usg.uid     > @CMaxUsUID
         and
                   usu.uid    <= @CMaxUsUID
         and       usu.uid    >  0
         and
                   al.suid     = suser_id(@name_in_db)
         and       al.altsuid  = usu.suid


Select @_rowcount = @@rowcount


IF (@_rowcount > 0)
   begin
   Select @Name1Type = 'AL'

   GOTO LABEL_25NAME1TYPEKNOWN

   end


-------- GROUP

IF EXISTS
      (SELECT * FROM sysusers
         WHERE  name = @name_in_db
         AND   (uid > @CMaxUsUID  OR  uid = 0)
      )
   begin
   Select @Name1Type = 'GR'

   Execute sp_helpgroup @name_in_db

   GOTO LABEL_75FINAL  --Done

   end


-------- Error

Raiserror(15198,-1,-1 ,@name_in_db)  --Input Name is unfound
Select @RetCode = @RetCode | 1

GOTO LABEL_75FINAL



--------


LABEL_25NAME1TYPEKNOWN:



-----------------------  Printout the report  -------------------------

-------- Preparations for dynamic exec

SELECT
          @charMaxLen_UsName  = convert( varchar,
                  isnull( max( datalength( zUserName)),8))

         ,@charMaxLen_GrName  = convert( varchar,
                  isnull( max( datalength( zGroupName)),9))

         ,@charMaxLen_LoName  = convert( varchar,
                  isnull( max( datalength( zLoginName)),9))

         ,@charMaxLen_DbName  = convert( varchar,
                  isnull( max( datalength( zDefDBName)),9))
   from
          #tb1_uga


-------- Dynamic EXEC() to printout report


EXECUTE(
'
SELECT
             ''UserName''  =
                     substring(zUserName ,1,' + @charMaxLen_UsName + ')

            ,''GroupName'' =
                     substring(zGroupName,1,' + @charMaxLen_GrName + ')

            ,''LoginName'' =
                     substring(zLoginName,1,' + @charMaxLen_LoName + ')

            ,''DefDBName'' =
                     substring(zDefDBName,1,' + @charMaxLen_DbName + ')

            ,''UserID''    = convert(char(6),zUID)

            ,''SUserID''   = convert(char(7),zSUID)
      from
             #tb1_uga
      order by
             1
'
)


-----------------------  A little extra nice-to-have

IF (@Name1Type IN ('-','US'))
   begin

   IF EXISTS (SELECT * FROM #tb1_uga tb1 ,sysalternates al
                       WHERE tb1.zSUID = al.altsuid
             )
      begin

      SELECT   'LoginName' = suser_name(al.suid)
              ,'UserNameAliasedTo' = tb1.zUserName
         from  #tb1_uga tb1 ,sysalternates al
         where tb1.zSUID   = al.altsuid
         order by 1

      end
   end



-----------------------  Finalization  ----------------------


LABEL_75FINAL:


IF (object_id('tempdb..#tb1_uga') IS Null)
            Drop Table #tb1_uga

RETURN @RetCode
go



raiserror(15339,-1,-1,'sp_lock')
go
create procedure sp_lock
@spid1 int = NULL,		/* server process id to check for locks */
@spid2 int = NULL		/* other process id to check for locks */
as

/*
**  Show the locks for both parameters.
*/
if @spid1 is not NULL
begin
	select spid, locktype = name, table_id = id, page,
		dbname = substring(db_name(dbid), 1, 15)
		from syslocks, master.dbo.spt_values v
		where syslocks.type = v.number
			and v.type = 'L'
			and spid in (@spid1, @spid2)
end

/*
**  No parameters, so show all the locks.
*/
else
begin
	select spid, locktype = name, table_id = id, page,
		dbname = substring(db_name(dbid), 1, 15)
	from syslocks, master.dbo.spt_values v
	where syslocks.type = v.number
		and v.type = 'L'
	order by spid, dbname, table_id, locktype, page
end

return (0)
go



raiserror(15339,-1,-1,'sp_lock2')
go
CREATE PROCEDURE sp_lock2  --1995/11/20 07:19

    @spid1    integer = NULL
   ,@spid2    integer = NULL  --A second spid can be given also.
as
---Concerned in 6.5 that temp #tables accumulate if @@trancount>0.
---This sproc does not ins/upd/del any system data (not counting tempdb..sysobj).

Set nocount on

Declare
       @RetCode                  integer
      ,@SUID                     smallint
      ,@cu111_DBName             character varying(30)
      ,@OrigDBName               character varying(30)

Declare
       @charMaxLenLockTypeName   character varying(10)
      ,@charMaxLenTBName         character varying(10)
      ,@charMaxLenDBName         character varying(10)
      ,@charMaxLenOwner          character varying(10)

--------

Select
       @RetCode     = 0  -- 0=good ,1=bad
      ,@SUID        = suser_id()
      ,@OrigDBName  = db_name()

-------------  Capture raw lock info  ----------------

CREATE TABLE #Lock
   (
    TableId         int             Null
   ,DBName          varchar(30)     Null
   ,Page            int             Null

   ,LockTypeId      smallint        Null
   ,LockTypeName    varchar(35)     Null
   ,SPID            smallint        Null

   ,OwnerName       varchar(30)     Null
   ,TableName       varchar(30)     Null
   )


INSERT
      into   #Lock
   SELECT
             l.id
            ,db_name(dbid)
            ,l.page

            ,l.type
            ,Null
            ,l.spid

            ,'-'
            ,'-'
      from
             master.dbo.syslocks    l   (nolock)
      where
            (
             (     @spid1 IS NOT Null
              AND  l.spid  IN (@spid1 ,@spid2)
             )
            OR
             (     @spid1 IS     Null
             )
            )



UPDATE --Delayed until here to avoid reporting on self (spt_values).
               #Lock
      set
               LockTypeName    = v.name
      from
               #Lock                  tb2
              ,master.dbo.spt_values  v      (nolock)
      where
               tb2.LockTypeId  = v.number
        and    v.type          = 'L'


--------------  This phase will convert IDs to Names  --------------


EXECUTE(
'
DECLARE Cursor111_LockDB
   Insensitive
   Cursor
For
SELECT
      DISTINCT
             DBName
      from
             #Lock
'
)


OPEN Cursor111_LockDB


WHILE (111=111)   -----------  LOOP 111: thru DBNames  ------------
   begin


   FETCH
                NEXT
         from
                Cursor111_LockDB
         into
                @cu111_DBName


   IF (@@fetch_status <> 0)
      begin
      Deallocate Cursor111_LockDB
      BREAK
      end


   IF (     @SUID         = 1
      or    @cu111_DBName IN ('tempdb' ,'master',@OrigDBName)
      or    @cu111_DBName = (SELECT   dbname
                               from  master.dbo.syslogins (nolock)
                               where suid=@SUID)
      or    @SUID         = (SELECT   suid
                               from  master.dbo.sysdatabases (nolock)
                               where name=@cu111_DBName)
      )
      begin --USE will not error.  sysdb.status bit for GUEST?


      EXECUTE(
      '
      USE ' + @cu111_DBName + '

      UPDATE
                #Lock
         set
                TableName  = object_name(tb2.TableId)
               ,OwnerName  = user_name(o.uid)
         from
                #Lock      tb2
               ,sysobjects o   (nolock)
         where
                tb2.TableId   = o.id
         and    tb2.DBName    = ''' + @cu111_DBName + '''
      '
      )

      end


   end  --loop 111


-------------------  Shorten those pesky temp #tb names  ------------

UPDATE
             #Lock
      set
             TableName = substring(TableName,1
                               ,patindex('%[_][_]%',TableName)-1) + '-'
      where
             TableName LIKE '#%[_][_]%'  --Start of system assigned portion.

/***
-------------------  special case  ----------------------

UPDATE
             #Lock
      set
             TableName  =  '(work)'  --Little white lie; syslogs?
            ,OwnerName  =  '(n/a)'
      where
             TableId    =  0
      and    TableName  =  '-'
***/
----------------  Prepare dynamic column widths  ------------------

SELECT
             @charMaxLenLockTypeName =
                  convert( varchar
                          ,isnull( max( datalength(LockTypeName)) ,8)
                         )
            ,@charMaxLenTBName       =
                  convert( varchar
                          ,isnull( max( datalength(TableName)) ,6)
                         )
            ,@charMaxLenDBName       =
                  convert( varchar
                          ,isnull( max( datalength(DBName)) ,6)
                         )
            ,@charMaxLenOwner        =
                  convert( varchar
                          ,isnull( max( datalength(OwnerName)) ,5)
                         )
      from
             #Lock

----------------  Output the report  --------------------

EXECUTE(
'
SELECT
             SPID
            ,LockType  = substring(LockTypeName,1,' + @charMaxLenLockTypeName + ')
            ,TableId
            ,Page
            ,DBName    = substring(DBName,1,' + @charMaxLenDBName + ')
            ,TableName = substring(TableName,1,' + @charMaxLenTBName + ')
            ,Owner     = substring(OwnerName,1,' + @charMaxLenOwner + ')
      from
             #Lock
      order by
             SPID ,DBName ,Owner ,TableName ,TableId ,LockType ,Page
'
)

-----------------  Finalization  ------------------


LABEL_86RETURN:


EXECUTE('USE ' + @OrigDBName) --So this will work on 6.0 too.

IF (object_id('tempdb..#Lock') IS NOT Null)
            Drop table #Lock

Return @RetCode
go




raiserror(15339,-1,-1,'sp_lockinfo')
go

create procedure sp_lockinfo
as
/* ====================================================*/
/* sp_lockinfo - returns detailed lock                 */
/* information for the server                          */
/* Microsoft SQL Server 6.5 		               */
/* ====================================================*/

declare @lkdbnm varchar(32),@lkdbid smallint,@lkobjid int,@UserDBID smallint,@usrdbnm varchar(32),@User_ID smallint, @grpid smallint, @stmt varchar(255)

/* ===========================================================*/
/* build temp table with processes and their associated locks */
/* ===========================================================*/

select
	'FullName'	= case when p.spid > 9 then lo.name else 'System' end,
	'UserID'= p.uid,
	'UName'	= '                                ',
	'ProcessID'= p.spid,
	'Status'= p.status,
	'DBID'		= p.dbid,
	'DbName'= d.name,
	'Command'= p.cmd,
	'Host'= p.hostname,
	'Application'= p.program_name,
	'Blocking'	= case when l.type >= 256 then 100 else 0 end,
	'Blockedby'= p.blocked,
	'LockType'	= l.type,
	'LKObjDBID'	= l.dbid,
	'LKObjDB'	= '                                ',
	'LKObjID'	= l.id,
	'LKObj'		= case when l.id = 0 then 
(select name from master..spt_values where number = l.type and type = 'SFL')			else '                                                                                     ' end,
	'LockedPage'	= l.page,'GroupID'	= p.gid,'GName'	= '                                ','CPUUsage'	= p.cpu,
	'PhysicalIO'	= p.physical_io,'HostProcess'	= p.hostprocess
into #info from master..sysprocesses p,	master..syslocks l,master..syslogins lo (nolock),master..sysdatabases d (nolock)
where	p.spid 	= l.spid and p.suid = lo.suid and p.dbid= d.dbid

/* ======================= */
/* flag blocking processes */	
/* ======================= */

update #info set Blocking = 1 where ProcessID in (select Blockedby from #info where Blockedby > 0)

/* =============================================*/
/* get list of locked object IDs */
/* =============================================*/

exec ('declare c1 cursor for select distinct LockedDBName = d.name, LKObjID , LKObjDBID from #info, master..sysdatabases d
where LKObjDBID = d.dbid  and LKObjID > 0 FOR READ ONLY')

open c1

fetch c1 into @lkdbnm, @lkobjid, @lkdbid

while @@fetch_status >= 0
   begin
	/* set database.owner.name of locked objects in #info */
	select @stmt ='Update #info set LKObj=' + '''' + @lkdbnm + '''' + '+ ''.'' + u.name + ''.'' + o.name' + 
 	',LKObjDB=' + ''''+@lkdbnm + '''' + ' from '+@lkdbnm+'..sysobjects o,'+@lkdbnm+'..sysusers u ' +
	'where o.id='+convert(char(10),@lkobjid)+ 
	' and LKObjID=o.id' +  
	' and u.uid=o.uid and LKObjDBID=' + convert(char,@lkdbid)  
	exec(@stmt )
	/* next please */
	fetch c1 into @lkdbnm, @lkobjid, @lkdbid
   end
deallocate c1

/* =============================================*/
/* resolve UserID, GroupID for all processes    */
/* get a list of each database we need to visit */
/* =============================================*/

exec ('declare c1 cursor for select distinct DBID, DbName from #info FOR READ ONLY')

open c1

fetch c1 into  @UserDBID, @usrdbnm

while @@fetch_status >= 0
   begin
	/* set user/group in #info */
	select @stmt = 'Update #info set GName=g.name,UName=u.name from '+@usrdbnm+'..sysusers u,'+
	@usrdbnm+'..sysusers g,#info '+	'where u.uid=UserID and g.uid=GroupID and DBID=' + convert(char(10),@UserDBID)
	exec(@stmt )
	/* next please */
	fetch c1 into  @UserDBID, @usrdbnm
   end
deallocate c1

/* ====== return the #info table ===== */
SELECT	
	FullName , ProcessID, 	Status,	DName = DbName,	Command,Host, Application, Blockedby,LockType,LKObj,
	GName = GName,CPUUsage, PhysicalIO, HostProcess,UName,Blocking
FROM #info (nolock) ORDER BY LKObj, ProcessID

return(0)
GO

raiserror(15339,-1,-1,'sp_logdevice')
go
create procedure sp_logdevice  --1995/11/11 16:45
@dbname	varchar(30),		/* database name that has the syslogs */
@devname	varchar(30)	/* device name to put syslogs on */
as

declare @dbid	smallint	/* dbid of the database to be changed */
declare @dbuid	smallint	/* id of the owner of the database */
declare @logbit	int		/* this is the bit to turn on in sysusages */
declare @sum_rowcount int	/* used to determine no action needed */

select @logbit = 4		/* bit 3 is the one to turn on */
select @sum_rowcount = 0

/*
**  Verify the database name and get the @dbid and @dbuid
*/
select @dbid = dbid, @dbuid = suid
	from sysdatabases
		where name = @dbname

/*
**  If @dbname not found, say so.
*/
if @dbid is null
begin
	raiserror(15010,-1,-1,@dbname)
	return (1)
end

/*
**  See if the device exists.
*/
if not exists (select * from sysdevices where name like @devname)
	begin
		raiserror(15012,-1,-1,@devname)
		return (1)
	end

/*
**  You must be SA or the dbo to execute this sproc.
*/
if suser_id() <> 1 and suser_id() <> @dbuid
	begin
		raiserror(15199,-1,-1)
		return (1)
	end

/*
**  Now see if the @dbname uses the @devname
*/
if not exists (select * from sysusages u, sysdevices d
		where d.name = @devname
			and u.vstart between d.low and d.high
			and u.dbid = @dbid)
	begin
		raiserror(15073,-1,-1,@devname,@dbname)
		return (1)
	end

/*
**  Check to see if the user specified that the original device be used
**  as dedicated log which would necessitate mixing fragment usage on
**  the device.
*/
if exists (select * from sysusages u, sysdevices d
		where d.name = @devname
			and u.vstart between d.low and d.high
			and u.dbid = @dbid
			and lstart = 0)
	begin
	raiserror(15320,-1,-1,@dbname,@devname) --lpage 0 never log only
	return (1)
	end

/*
**  clear the bit from any database segments that aren't already log only
*/
update sysusages
	set   segmap  = segmap & ~@logbit
	from  sysusages
	where dbid    = @dbid
	and   segmap <> @logbit
	and   segmap  & @logbit > 0

select @sum_rowcount = @sum_rowcount + @@rowcount

/*
**  Now set the segments on @devname as log-only.
*/
update sysusages
	set   segmap = @logbit
	from  sysusages u, sysdevices d
	where d.name = @devname
	and   u.dbid = @dbid
	and   u.vstart between d.low and d.high
	and   u.segmap <> @logbit

select @sum_rowcount = @sum_rowcount + @@rowcount


--Any action needed?
if @sum_rowcount = 0
	begin
	raiserror(15317,-1,-1,@dbname,@devname) --okay no action
	return (0)
	end

/*
**  Now we need to activate the new segment map.
*/
dbcc dbrepair (@dbname, remap)

if @@error=0
	begin
	raiserror(15318,-1,-1,@dbname,@devname)
	end
else
	begin
	raiserror(15319,-1,-1,@dbname,@devname)
	return (1)
	end

return (0)
go



raiserror(15339,-1,-1,'sp_lookup')
go
create procedure sp_lookup
@typearg int,
@owname	varchar(30) = '%', /* owner of the desired objects (except indexes) */
@obname	varchar(30) = '%', /* name of the desired objects (including indexes) */
@indtab	varchar(30) = '%', /* table associated with the desired index */
@indown varchar(30) = '%'  /* owner of the desired index */
as
	create table #ddtab
	(
		name	varchar(30)	NOT NULL,
		owner	varchar(30)	NOT NULL,
		type	int		NOT NULL,
		indtab	varchar(30)	NOT NULL
	)

declare @table	int
declare @view	int
declare @index	int
declare @sproc	int
declare @trgr	int
declare @dflt	int
declare @rule	int
declare @dtype	int
declare @db	int
declare @report int

select @table = 1, @view = 2, @index = 4, @sproc = 8, @trgr = 16, @dflt = 32,
	@rule = 64, @dtype = 128, @db = 256, @report = 512

if (@typearg & @table = @table)
	insert into #ddtab
		select name, user_name(uid), @table, '' from dbo.sysobjects
			where name like @obname
				and user_name(uid) like @owname
				and (sysstat & 0xf = 1	/* system table */
		  		or sysstat & 0xf = 3)	/* user table */


if (@typearg & @view = @view)
	insert into #ddtab
		select name, user_name(uid), @view, '' from dbo.sysobjects
			where name like @obname
				and user_name(uid) like @owname
			  	and sysstat & 0xf = 2		/* view */

if (@typearg & @index = @index)
	insert into #ddtab
		select i.name, user_name(o.uid), @index, o.name
			from dbo.sysindexes i, dbo.sysobjects o
				where i.name like @obname
					and i.indid > 0
					and i.id = o.id
					and user_name(o.uid) like @indown
					and o.name = object_name(o.id)
					and o.name like @indtab

if (@typearg & @report = @report)
	insert into #ddtab
		select name, user_name(uid), @report, '' from dbo.sysobjects
			where name like @obname
				and user_name(uid) like @owname
				and sysstat & 0xf = 4		/* sproc */
				and userstat & -32768 = -32768	/* report */

if (@typearg & @sproc = @sproc)
begin
	/*
	**  If we also wanted reports, don't pick up reports again as sprocs.
	*/
	if (@typearg & @report = @report)
	begin
		insert into #ddtab
			select name, user_name(uid), @sproc, '' from dbo.sysobjects
			 where name like @obname
				and user_name(uid) like @owname
				and sysstat & 0xf = 4	/* stored procedure */
				and userstat & -32768 <> -32768	/* report */
	end
	else
	begin
		insert into #ddtab
			select name, user_name(uid), @sproc, '' from dbo.sysobjects
			 where name like @obname
				and user_name(uid) like @owname
				and sysstat & 0xf = 4	/* stored procedure */
	end
end

if (@typearg & @trgr = @trgr)
	insert into #ddtab
		select name, user_name(uid), @trgr, '' from dbo.sysobjects
			where name like @obname
				and user_name(uid) like @owname
			  	and sysstat & 0xf = 8	/* trigger */

if (@typearg & @dflt = @dflt)
	insert into #ddtab
		select name, user_name(uid), @dflt, '' from dbo.sysobjects
			 where name like @obname
			 	and user_name(uid) like @owname
			 	and sysstat & 0xf = 6		/* default */

if (@typearg & @rule = @rule)
	insert into #ddtab
		select name, user_name(uid), @rule, '' from dbo.sysobjects
			 where name like @obname
			 	and user_name(uid) like @owname
			 	and sysstat & 0xf = 7		/* rule */

if (@typearg & @dtype = @dtype)
	insert into #ddtab
		select name, user_name(uid), @dtype, '' from dbo.systypes
			where name like @obname
				and user_name(uid) like @owname

if (@typearg & @db = @db)
	insert into #ddtab
		select name, suser_name(suid), @db, ''
			from sysdatabases
				where (name like @obname)
					and suser_name(suid) like @owname

select * from #ddtab order by name
return (0)
go


dump transaction master with no_log
go



raiserror(15339,-1,-1,'sp_helpremotelogin')
go
create procedure sp_helpremotelogin
@remoteserver varchar(30) = NULL,	/* remote server name */
@remotename varchar(30) = NULL		/* remote login name */
as

set nocount on

/*
**  If no server given, get 'em all.
*/
if @remoteserver is null
	select @remoteserver = '%'
else
	if not exists (select *
		from sysservers s, sysremotelogins r
		where s.srvid = r.remoteserverid
			and s.srvname like @remoteserver
			and s.srvid > 0)
		begin
			if @remoteserver = '%'
				begin
					raiserror(15200,-1,-1)
					return (1)
				end

			raiserror(15201,-1,-1,@remoteserver)
			return (1)
		end

/*
**  If no remotename given, get 'em all.
*/
if @remotename is null
	begin
		select @remotename = '%'
	end
else
	begin
		if not exists (select * from sysremotelogins
			where isnull(remoteusername, '') like @remotename)

			begin
				if @remotename = '%'
					begin
						raiserror(15202,-1,-1)
						return (1)
					end

				raiserror(15203,-1,-1,@remotename)
				return (1)
			end
	end

/*
**  Check for empty results.
*/
if not exists (select *
		from sysremotelogins r, sysservers s
		where isnull(r.remoteusername, '') like @remotename
			and s.srvid = r.remoteserverid
			and s.srvname like @remoteserver)
	begin
		if ((@remoteserver = '%') and (@remotename = '%'))
			raiserror(15202,-1,-1)
		else
			raiserror(15204,-1,-1,@remotename,@remoteserver)

		return (1)
	end

/*
**  Select the information.
*/
select server = substring(s.srvname, 1, 22),
	local_user_name =
		substring(isnull(suser_name(r.suid), '** use local name **'), 1, 22),
	remote_user_name =
		substring(isnull(r.remoteusername, '** mapped locally **'), 1, 22),
	options = substring(v.name, 1, 9)
		from sysservers s, sysremotelogins r,
			master.dbo.spt_values v
	where s.srvid = r.remoteserverid
		and s.srvname like @remoteserver
		and s.srvid > 0
		and isnull(r.remoteusername, '') like @remotename
		and v.type = 'F'
		and v.number = r.status
order by server, remote_user_name

return (0)
go


raiserror(15339,-1,-1,'sp_helpsegment')
go
create procedure sp_helpsegment
@segname varchar(30) = NULL		/* segment name */
as

declare	@segbit	int	/* this is the bit version of the segment # */
declare	@segment int	/* the segment number of the segment */

set nocount on

/*
**  If no segment name given, get 'em all.
*/
if @segname is null
	begin
		select * from syssegments
		return (0)
	end

/*
**  Make sure the segment exists
*/
if not exists (select * from syssegments where name = @segname)
	begin
		raiserror(15018,-1,-1,@segname)
		return (1)
	end

/*
**  Show the syssegment entry, then the fragments and size it is on,
**  then any dependent objects in the database.
*/
select * from syssegments where name = @segname

/*
**  Set the bit position for the segment.
*/
select @segment = segment from syssegments where name = @segname

/*
**  Now set the segments on @devname sysusages.
*/
if (@segment < 31)
	select @segbit = power(2, @segment)
else
	/*
	**  Since this is segment 31, power(2, 31) will overflow
	**  since segmap is an int.  We'll grab the machine-dependent
	**  bit mask from spt_values to set the right bit.
	*/
	select @segbit = low
		from master.dbo.spt_values
		where type = 'E'
			and number = 2

select device = d.name, size = convert(varchar(20),
		round((u.size * convert(float, v.low)) / 1048576, 0)) + 'MB'
	from sysusages u, sysdevices d, master.dbo.spt_values v
	where u.segmap & @segbit = @segbit
		and d.low <= u.size + u.vstart
		and d.high >= u.size + u.vstart - 1
		and u.dbid = db_id()
		and d.status & 2 = 2
		and v.number = 1
		and v.type = 'E'

if exists (select * from sysindexes i, syssegments s
		where s.name = @segname
			and s.segment = i.segment)

	select table_name = object_name(i.id), index_name = i.name, i.indid
		from sysindexes i, syssegments s
		where s.name = @segname
			and s.segment = i.segment
		order by table_name, indid

return (0)
go


raiserror(15339,-1,-1,'sp_helpsort')
go
create procedure sp_helpsort
AS
set nocount on

declare @sortid tinyint, @charid tinyint, @status tinyint
declare @image binary(1)
declare @count int, @end_flag int

declare @string varchar(66)
create table #helpsort3 (string varchar(66)	NOT NULL)

/*
** Get the sortorder ID, character set ID, and status
*/
select @sortid = id, @charid = csid, @image = convert(binary(1), definition)
    from syscharsets, syscurconfigs
    where id = value and config = 1123

/*
** Get the name and description of the character set and sort order
*/
if (@sortid is null)
 BEGIN
    select @sortid = 0, @charid = 0, @status = 0
    insert #helpsort3 values ('Character Set = 0, ascii_8')
    insert #helpsort3 values ('    8-bit ASCII Character Set - System Default')
    insert #helpsort3 values ('Sort Order = 0, bin_ascii_8')
    insert #helpsort3 values ('    Binary Ordering - System Default')
 END
else
 BEGIN
    select @status = convert(tinyint, @image)

    select @string = 'Character Set = ' + convert(varchar(4), id) + ', ' + name
	    from syscharsets
	    where id = @charid and type = 1001

    insert into #helpsort3 values (@string)

    select @count = 1
    select @end_flag = 1

    while (@end_flag <> 0)
    begin
	    select @string = substring(description, @count, 62)
		    from syscharsets
		    where id = @charid and type = 1001
	    select @end_flag = isnull(ascii(@string), 0)

	    if (@end_flag <> 0)
	    begin
		    select @string = '    ' + @string
		    insert #helpsort3 values (@string)
	    end
	    select @count = @count + 62
    end

    select @string = 'Sort Order = ' + convert(varchar(4), id) + ', ' + name
	    from syscharsets
	    where id = @sortid and type = 2001

    insert into #helpsort3 values (@string)

    select @count = 1
    select @end_flag = 1

    while (@end_flag <> 0)
    begin
	    select @string = substring(description, @count, 62)
		    from syscharsets
		    where id = @sortid and type = 2001
	    select @end_flag = isnull(ascii(@string), 0)

	    if (@end_flag <> 0)
	    begin
		    select @string = '    ' + @string
		    insert #helpsort3 values (@string)
	    end

	    select @count = @count + 62
    end
END

select 'Sort Order Description' = string from #helpsort3

/*
** Build the sorted list of characters for this sort order.
** If it's not stored in spt_values yet, then put the results in there.
*/
if not exists (select * from master.dbo.spt_values
		where type = 'Z'
			and high = @sortid)
BEGIN

    create table #helpsort1 (char_val char(1) NOT NULL)



    if @charid = 0       /*    ascii_8 has valid values from 32 to 126 */
	begin
    		select @count = 32
    		while @count <= 126
    		begin
    			insert #helpsort1 (char_val)
    				values (char(@count))
    			select @count = @count + 1
    		end
	end

    if @charid = 1  	/*     iso_1 has values from 32 to 126, 160 to 255 */
	begin
    		select @count = 32
    		while @count <= 126
    		begin
    			insert #helpsort1 (char_val)
    				values (char(@count))
    			select @count = @count + 1
    		end

        	select @count = 160
        	while @count <= 255
        	begin
    	   		insert #helpsort1 (char_val)
    		    		values (char(@count))
    	    		select @count = @count + 1
        	end
	end

    if @charid = 2      /*     cp850 has valid values from 0 to 255 */
	begin
    		select @count = 32
    		while @count <= 255
    		begin
    			insert #helpsort1 (char_val)
    				values (char(@count))
    			select @count = @count + 1
    		end
	end

    if @charid = 3      /*     cp437 has valid values from 0 to 255 */
	begin
    		select @count =32
    		while @count <= 255
    		begin
    			insert #helpsort1 (char_val)
    				values (char(@count))
    			select @count = @count + 1
    		end
	end

    if @charid = 4
        begin
                select @count =32
                while @count <= 255
                begin
                        insert #helpsort1 (char_val)
                                values (char(@count))
                        select @count = @count + 1
                end

                /* Remove unwanted values */

                delete #helpsort1 where ascii(char_val) in (127, 128, 129, 131, 136, 144, 152)

        end

    if @charid = 5
        begin
                select @count =32
                while @count <= 255
                begin
                        insert #helpsort1 (char_val)
                                values (char(@count))
                        select @count = @count + 1
                end

                /* Remove unwanted values */

                delete #helpsort1 where ascii(char_val) in (127, 136, 152)

        end

    if @charid = 6
        begin
                select @count =32
                while @count <= 255
                begin
                        insert #helpsort1 (char_val)
                                values (char(@count))
                        select @count = @count + 1
                end

                /* Remove unwanted values */

                delete #helpsort1 where ascii(char_val) in (128, 129, 136, 138, 140, 141, 142, 143, 144, 152, 154, 156, 157, 158, 159)

        end

    if @charid = 7
        begin
                select @count =32
                while @count <= 255
                begin
                        insert #helpsort1 (char_val)
                                values (char(@count))
                        select @count = @count + 1
                end

                /* Remove unwanted values */

                delete #helpsort1 where ascii(char_val) in (128, 129, 141, 142, 143, 144, 157, 158)

        end

    if @charid = 8
        begin
                select @count =32
                while @count <= 255
                begin
                        insert #helpsort1 (char_val)
                                values (char(@count))
                        select @count = @count + 1
                end

                /* Remove unwanted values */

                delete #helpsort1 where ascii(char_val) in (128, 129, 138, 140, 141, 142, 143, 144, 154, 156, 157, 158, 159)

        end

    if @charid = 9
        begin
                select @count =32
                while @count <= 255
                begin
                        insert #helpsort1 (char_val)
                                values (char(@count))
                        select @count = @count + 1
                end

                /* Remove unwanted values */

                delete #helpsort1 where ascii(char_val) in (128, 138, 143, 152, 154, 159)

        end

    if @charid = 10
        begin
                select @count =32
                while @count <= 255
                begin
                        insert #helpsort1 (char_val)
                                values (char(@count))
                        select @count = @count + 1
                end

                /* Remove unwanted values */

                delete #helpsort1 where ascii(char_val) in (128, 129, 131, 136, 138, 140, 143, 144, 156, 159)

        end


    if @charid > 10     /* User Character Set Not recognised by sp_helpsort */
	begin
   		select 'Unknown Character Set : ' + name from syscharsets
	    			where id = @sortid and type = 2001
		return (0)
	end

    select (char_val) into #helpsort2
    	from #helpsort1
    	order by char_val

    declare @char char(1)
    declare @char_str varchar(20)
    declare @incr int
    declare @seq_num int
    declare @rel char(1)
    declare @old_char char(1)

    select @char = char(0), @seq_num = 0, @count = 0, @incr = 8, @end_flag = 1
    select @rel = ' ', @old_char = NULL

    set rowcount 1

    while @end_flag <> 0
    begin
        select @char_str = ''

		/* If PST_NOTUNIQ is set, then possibly caseless */
		if (@status & 8 = 8)
            while (@count < @incr  and  @end_flag <> 0)
            begin
                select @char = char_val
    	            from #helpsort2
    	            where char_val >= @char
                select @end_flag = @@rowcount

		if (@end_flag <> 0)
		begin
		    if (@char = @old_char)
			select @rel = '='
		    else
			select @rel = ' '

		    select @old_char = @char
		    select @char_str = @char_str + @rel + @char
		    delete from #helpsort2 where ascii(@char) = ascii(char_val)

		    select @count = @count + 1
		end
	end
	else
		while (@count < @incr  and  @end_flag <> 0)
			begin
				select @char = char_val
					from #helpsort2
					where char_val > @char
				select @end_flag = @@rowcount
				if (@end_flag <> 0)
					begin
						select @char_str = @char_str + @rel + @char
						delete from #helpsort2 where ascii(@char) = ascii(char_val)
						select @count = @count + 1
					end
			end

        select @incr = @incr + 8, @seq_num = @seq_num + 1
        insert master.dbo.spt_values (name, number, type, low, high)
		values (@char_str, @seq_num, 'Z', @charid, @sortid)
    end

    set rowcount 0

END

/*
** Display the sorted list of characters.
*/
select @count = 1
select @end_flag = 1
select @incr = 5

truncate table #helpsort3

while @end_flag <> 0
BEGIN
	select @string = ''

	while @count < @incr
	begin
		select @string = @string + substring(name, 2, 19)
			from master.dbo.spt_values
			where type = 'Z'
				and number = @count
				and high = @sortid
		select @end_flag = @@rowcount
		select @count = @count + 1
	end

	select @incr = @incr + 4

	insert into #helpsort3 values (@string)
END

select 'Characters, in Order' = string
	from #helpsort3
set nocount off
return(0)
go


raiserror(15339,-1,-1,'sp_helpstartup')
go
create procedure sp_helpstartup
as

/*
**  Only the SA can do this.
*/
if suser_id() <> 1
begin
	raiserror(15003,-1,-1)
	return (1)
end

select 'Startup stored procedures:' = name from sysobjects
	where category & 16 = 16 order by name

return(0)
go


raiserror(15339,-1,-1,'sp_helpserver')
go
create procedure sp_helpserver  --1996/01/26 20:20
@server varchar(30) = NULL,		/* server name */
@optname varchar(20) = NULL,
@show_topology varchar(1) = NULL
as

declare @optbit int

set nocount on

/*
**  If no server name given, get 'em all.
*/
if @server = null
	select @server = '%'

/*
**  Does the server exist?
*/
if not exists (select *
		from sysservers
			where srvname like @server
				or srvnetname like @server)
	begin
		if @server = '%'
			raiserror(15205,-1,-1)
		else
			raiserror(15015,-1,-1,@server)

		return (1)
	end

/*
**  Display server information.
**  First put it into a temp table so we can blot inappropriate status
**  for the local server.  It's ugly but I couldn't think of another way.
*/
declare @cursrvid smallint	/* the one we're currently working on */
declare @srvdesc varchar(11)	/* the total description for the server */
declare @bitdesc varchar(30)    /* the bit description for the server  */
create table #spt_server
(
	name	varchar(30)	NOT NULL,
	network	varchar(30)	NOT NULL,
	status	varchar(11)	NOT NULL,
	id	smallint        NOT NULL,
	topx	int		NULL,
	topy	int		NULL
)

/*
**  Allow option specification.
*/
select @optbit = -1     /* 0xffffffff */
if @optname is not null
	begin
		select @optbit = null
		select @optbit = number
			from master.dbo.spt_values
			where type = 'A'
				and name = @optname
		if @optbit is null or @optbit <= 0
			begin
				raiserror(15206,-1,-1,@optname)
				return(1)
			end
	end

/*
**  Set @cursrvid to the first or only srvid.
**  Mask out 'rpc' for the local server.
*/
if @server = '%'
	select @cursrvid = min(srvid) from sysservers
		where (@optname is null or srvstatus & @optbit <> 0)
		and not (@optbit = 1 and srvid = 0)
else
	select @cursrvid = srvid from sysservers
		where srvname =	@server
		and (@optname is null or srvstatus & @optbit <> 0)
		and not (@optbit = 1 and srvid = 0)

while @cursrvid <> NULL
begin
	/*
	**  Initialize @srvdesc.
	*/
	select @srvdesc = ''

	/*
	** Check rpc bit (1)
	*/
	if @cursrvid <> 0
		begin
			select @bitdesc = null
			select @bitdesc = v.name
				from master.dbo.spt_values v, sysservers s
				where s.srvid = @cursrvid
					and v.type = 'A'
					and v.name = 'rpc'
					and s.srvstatus & v.number = 1

			if @bitdesc <> null
				begin
					if @srvdesc <> ''
						select @srvdesc = @srvdesc + ',' +  @bitdesc
					else
						select @srvdesc = @bitdesc
				end
         end

	/*
	** Check publisher bit (2)
	*/
	select @bitdesc = null
	select @bitdesc = v.name
		from master.dbo.spt_values v, sysservers s
		where s.srvid = @cursrvid
			and v.type = 'A'
			and v.name = 'pub'
			and s.srvstatus & v.number = 2
	
	if @bitdesc <> null
		begin
			if @srvdesc <> ''
				select @srvdesc = @srvdesc + ',' +  @bitdesc
			else
				select @srvdesc = @bitdesc
		end

	/*
	** Check subscriber bit (4)
	*/
	select @bitdesc = null
	select @bitdesc = v.name
		from master.dbo.spt_values v, sysservers s
		where s.srvid = @cursrvid
			and v.type = 'A'
			and v.name = 'sub'
			and s.srvstatus & v.number = 4

	if @bitdesc <> null
		begin
			if @srvdesc <> ''
				select @srvdesc = @srvdesc + ',' +  @bitdesc
			else
				select @srvdesc = @bitdesc
		end

	/*
	** Check distribution bit (8)
	*/
	select @bitdesc = null
	select @bitdesc = v.name
		from master.dbo.spt_values v, sysservers s
		where s.srvid = @cursrvid
			and v.type = 'A'
			and v.name = 'dist'
			and s.srvstatus & v.number = 8

	if @bitdesc <> null
		begin
			if @srvdesc <> ''
				select @srvdesc = @srvdesc + ',' +  @bitdesc
			else
				select @srvdesc = @bitdesc
		end

	/*
	** Check distribution publisher bit (16)
	*/
	select @bitdesc = null
	select @bitdesc = v.name
		from master.dbo.spt_values v, sysservers s
		where s.srvid = @cursrvid
			and v.type = 'A'
			and v.name = 'dpub'
			and s.srvstatus & v.number = 16

	if @bitdesc <> null
		begin
			if @srvdesc <> ''
				select @srvdesc = @srvdesc + ',' +  @bitdesc
			else
				select @srvdesc = @bitdesc
		end

	/*
	** Check ODBC DSN bit (32)
	*/
	select @bitdesc = null
	select @bitdesc = v.name
		from master.dbo.spt_values v, sysservers s
		where s.srvid = @cursrvid
			and v.type = 'A'
			and v.name = 'dsn'
			and s.srvstatus & v.number = 32

	if @bitdesc <> null
		begin
			if @srvdesc <> ''
				select @srvdesc = @srvdesc + ',' +  @bitdesc
			else
				select @srvdesc = @bitdesc
		end

	/*
	** Check Fallback bit (64)
	*/
	select @bitdesc = null
	select @bitdesc = v.name
		from master.dbo.spt_values v, sysservers s
		where s.srvid = @cursrvid
			and v.type = 'A'
			and v.name = 'fallback'
			and s.srvstatus & v.number = 64

	if @bitdesc <> null
		begin
			if @srvdesc <> ''
				select @srvdesc = @srvdesc + ',' +  @bitdesc
			else
				select @srvdesc = @bitdesc
		end

	insert into #spt_server
		select s.srvname, s.srvnetname, @srvdesc, id = s.srvid,
			s.topologyx,s.topologyy
		from sysservers s
			where s.srvid = @cursrvid

	/*
	** Quit if server specified or increment to next server.
	*/
	if @server = '%'
		select @cursrvid = min(srvid) from sysservers
			where srvid > @cursrvid
			and (@optname is null or srvstatus & @optbit <> 0)
			and not (@optbit = 1 and srvid = 0)
	else
		select @cursrvid = NULL
end

if lower(@show_topology) <> 't' or @show_topology is null
	select name, network_name = substring(network, 1, 28),
		status = status, id = convert(char(4), id)
	from #spt_server
	order by name
else
	select name, network_name = substring(network, 1, 28),
		status = status, id = convert(char(4), id),
		topx,topy
	from #spt_server
	order by name

return(0)
go


raiserror(15339,-1,-1,'sp_makestartup')
go
create procedure sp_makestartup
@procname varchar(30) = null
as
declare @objid	int
declare @dbname varchar(30)

/*
**	Check to see if a proc. name was specified.
*/
if @procname is null
	begin
		raiserror(15207,-1,-1)
		return (1)
	end

/*
**  Only the SA can do this.
*/
if suser_id() <> 1
begin
	raiserror(15003,-1,-1)
	return (1)
end

/*
**  Make sure the @procname is in the master database.
*/
if @procname like '%.%.%' and	-- Explicit db name supplied
	substring(@procname, 1, charindex('.', @procname) - 1) <> 'master'
	begin
		raiserror(15272,-1,-1,@procname)
		return (1)
	end
else	-- No explicit name supplied so we need to be in the 'master' db
	if db_name() <> 'master'
		begin
			select @dbname = db_name()
			raiserror(15273,-1,-1,@procname,@dbname)
			return(1)
		end
/*
**  Make sure the procedure exists and get its id so we can use
**	a simpler where clause.
*/
select @objid = id from master..sysobjects
	where sysstat & 0xf = 4
	and name = @procname

if @objid is null
	begin
		raiserror(15020,-1,-1,@procname)
		return (1)
	end

/*
**  Make sure that the procedure is owned by the SA.  We're looking
**	in master so if the uid indicates dbo it has to be the SA.
*/
if (select uid from master..sysobjects
		where id = @objid) <> 1
	begin
		raiserror(15208,-1,-1)
		return (1)
	end

/*
**  Check to see that the procedure doesn't expect any parameters.
*/
if exists (select * from master..syscolumns
			where id = @objid)
	begin
		raiserror(15209,-1,-1)
		return (1)
	end

/*
**	If we made it to here, make it autoexec.
*/
if (select category & 16 from master..sysobjects where id = @objid) <> 16
	begin
		update master..sysobjects set category = category | 16
			where id = @objid
		print 'Procedure has been marked as ''startup''.'
	end
else
	print 'Procedure is already designated as ''startup''.'

return (0)
go


raiserror(15339,-1,-1,'sp_markreport')
go
create procedure sp_markreport
@reportname varchar(92)		/* proc name we want to mark as report */
as

/*
**  Make sure the @reportname is local to the current database.
*/
if @reportname like '%.%.%' and
	substring(@reportname, 1, charindex('.', @reportname) - 1) <> db_name()
	begin
		raiserror(15083,-1,-1)
		return (1)	
	end

/*
**  Make sure that it actually exists as a sproc.
*/
if not exists (select * from sysobjects
		where id = object_id(@reportname)
		and sysstat & 0xf = 4	/* stored procedure */
		and uid = user_id())
	begin
		raiserror(15021,-1,-1,@reportname)
		return (1)
	end

update sysobjects
	set userstat = userstat | -32768	/* high bit means report */
		from sysobjects
			where id = object_id(@reportname)
			and sysstat & 0xf = 4		/* stored procedure */
			and uid = user_id()

return (0)
go


raiserror(15339,-1,-1,'sp_monitor')
go
create procedure sp_monitor
as

/*
**  Declare variables to be used to hold current monitor values.
*/
declare @now 		datetime
declare @cpu_busy 	int
declare @io_busy	int
declare @idle		int
declare @pack_received	int
declare @pack_sent	int
declare @pack_errors	int
declare @connections	int
declare @total_read	int
declare @total_write	int
declare @total_errors	int

declare @oldcpu_busy 	int	/* used to see if DataServer has been rebooted */
declare @interval	int
declare @mspertick	int	/* milliseconds per tick */

/*
**  If we're in a transaction, disallow this since it might make recovery
**  impossible.
*/
if @@trancount > 0
	begin
		raiserror(15002,-1,-1,'sp_monitor')
		return (1)
	end

/*
**  Set @mspertick.  This is just used to make the numbers easier to handle
**  and avoid overflow.
*/
select @mspertick = convert(int, @@timeticks / 1000.0)

/*
**  Get current monitor values.
*/
select
	@now = getdate(),
	@cpu_busy = @@cpu_busy,
	@io_busy = @@io_busy,
	@idle = @@idle,
	@pack_received = @@pack_received,
	@pack_sent = @@pack_sent,
	@connections = @@connections,
	@pack_errors = @@packet_errors,
	@total_read = @@total_read,
	@total_write = @@total_write,
	@total_errors = @@total_errors

/*
**  Check to see if DataServer has been rebooted.  If it has then the
**  value of @@cpu_busy will be less than the value of spt_monitor.cpu_busy.
**  If it has update spt_monitor.
*/
select @oldcpu_busy = cpu_busy
	from spt_monitor
if @oldcpu_busy > @cpu_busy
begin
	update spt_monitor
		set
			lastrun = @now,
			cpu_busy = @cpu_busy,
			io_busy = @io_busy,
			idle = @idle,
			pack_received = @pack_received,
			pack_sent = @pack_sent,
			connections = @connections,
			pack_errors = @pack_errors,
			total_read = @total_read,
			total_write = @total_write,
			total_errors = @total_errors
end

/*
**  Now print out old and new monitor values.
*/
set nocount on
select @interval = datediff(ss, lastrun, @now)
	from spt_monitor
/* To prevent a divide by zero error when run for the first
** time after boot up
*/
if @interval = 0
	select @interval = 1
select last_run = lastrun, current_run = @now, seconds = @interval
	from spt_monitor

select
	cpu_busy = substring(convert(varchar(11),
		convert(int, ((@cpu_busy * @mspertick) / 1000)))
		+ '('
		+ convert(varchar(11), convert(int, (((@cpu_busy - cpu_busy)
		* @mspertick) / 1000)))
		+ ')'
		+ '-'
		+ convert(varchar(11), convert(int, ((((@cpu_busy - cpu_busy)
		* @mspertick) / 1000) * 100) / @interval))
		+ '%',
		1, 25),
	io_busy = substring(convert(varchar(11),
		convert(int, ((@io_busy * @mspertick) / 1000)))
		+ '('
		+ convert(varchar(11), convert(int, (((@io_busy - io_busy)
		* @mspertick) / 1000)))
		+ ')'
		+ '-'
		+ convert(varchar(11), convert(int, ((((@io_busy - io_busy)
		* @mspertick) / 1000) * 100) / @interval))
		+ '%',
		1, 25),
	idle = substring(convert(varchar(11),
		convert(int, ((@idle * @mspertick) / 1000)))
		+ '('
		+ convert(varchar(11), convert(int, (((@idle - idle)
		* @mspertick) / 1000)))
		+ ')'
		+ '-'
		+ convert(varchar(11), convert(int, ((((@idle - idle)
		* @mspertick) / 1000) * 100) / @interval))
		+ '%',
		1, 25)
from spt_monitor

select
	packets_received = substring(convert(varchar(11), @pack_received) + '(' +
		convert(varchar(11), @pack_received - pack_received) + ')', 1, 25),
	packets_sent = substring(convert(varchar(11), @pack_sent) + '(' +
		convert(varchar(11), @pack_sent - pack_sent) + ')', 1, 25),
	packet_errors = substring(convert(varchar(11), @pack_errors) + '(' +
		convert(varchar(11), @pack_errors - pack_errors) + ')', 1, 25)
from spt_monitor

select
	total_read = substring(convert(varchar(11), @total_read) + '(' +
		convert(varchar(11), @total_read - total_read) + ')', 1, 19),
	total_write = substring(convert(varchar(11), @total_write) + '(' +
		convert(varchar(11), @total_write - total_write) + ')', 1, 19),
	total_errors = substring(convert(varchar(11), @total_errors) + '(' +
		convert(varchar(11), @total_errors - total_errors) + ')', 1, 19),
	connections = substring(convert(varchar(11), @connections) + '(' +
		convert(varchar(11), @connections - connections) + ')', 1, 18)
from spt_monitor

/*
**  Now update spt_monitor
*/
update spt_monitor
	set
		lastrun = @now,
		cpu_busy = @cpu_busy,
		io_busy = @io_busy,
		idle = @idle,
		pack_received = @pack_received,
		pack_sent = @pack_sent,
		connections = @connections,
		pack_errors = @pack_errors,
		total_read = @total_read,
		total_write = @total_write,
		total_errors = @total_errors

return (0)
go


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


raiserror(15339,-1,-1,'sp_placeobject')
go
create procedure sp_placeobject
@segname	varchar(30),	/* segment name */
@objname	varchar(62)	/* object name */
as

declare	@msg		char(100)
declare @objid		int
declare @tabname	varchar(30)
declare @indexname	varchar(30)
declare	@indexid	smallint
declare	@segment	smallint
declare @dbname		varchar(30)

/*
**  Check to see if the segment exists.
*/
select @segment = segment from syssegments where name = @segname
if @segment is null
	begin
		raiserror(15018,-1,-1,@segname)
		return (1)
	end

select @dbname = db_name()

/*
**  Are we dealing with a table or its index?
*/
if @objname like '%.%'
	begin

		/*
		**  Get the table name out.
		*/
		select @tabname = substring(@objname, 1, charindex('.', @objname) - 1)
		select @indexname = substring(@objname, charindex('.', @objname) + 1, 61)
	end
else
		select @tabname = @objname, @indexname = null

/*
**  Can't use placeobject to move syslogs.
*/
if (object_id(@tabname) = 8)
	begin
		raiserror(15212,-1,-1)
		return (1)
	end

/*
**  Can't move system tables.
*/
if object_id(@tabname) < 100
	begin
		raiserror(15213,-1,-1)
		return (1)
	end

/*
**  If the user is not the dbo or the SA, make sure he/she owns it.
*/
if suser_id() <> 1 and user_id() <> 1
	begin
		if not exists (select *
			from sysobjects
				where name = @tabname
					and uid = user_id())
			begin
				raiserror(15214,-1,-1,@tabname)
				return (1)
			end
	end
else
	begin
		/*
		**  If dbo or SA, make sure the table exists.
		*/
		if not exists (select * from sysobjects where name = @tabname)
			begin
				raiserror(15009,-1,-1,@tabname,@dbname)
				return (1)
			end
	end

/*
**  If we are dealing with an index, make sure it exists.
*/
if @indexname IS NOT NULL and not exists (select *
					from sysindexes
						where id = object_id(@tabname)
							and name = @indexname)
begin
	raiserror(15215,-1,-1,@indexname,@tabname)
	return (1)

end

/*
**  Everything's ok so change the segment for the object.
*/
if @indexname is null
begin
	select @indexid = indid
		from sysindexes
			where id = object_id(@tabname)
				and indid < 2
end
else
begin
	select @indexid = indid
		from sysindexes
			where id = object_id(@tabname)
				and name = @indexname
end

update sysindexes
set segment = @segment
	from sysindexes
		where id = object_id(@tabname)
			and indid = @indexid

dbcc dbrepair (@dbname, remap)
select @msg = '''' + @objname + ''' is now on segment ''' + @segname + '''.'
print @msg

return (0)
go


raiserror(15339,-1,-1,'sp_primarykey')
go
create procedure sp_primarykey
@tabname  varchar(92),			/* table name that has the key */
@col1     varchar(30), 			/* columns that make up the key */
@col2     varchar(30) = NULL,
@col3     varchar(30) = NULL,
@col4     varchar(30) = NULL,
@col5     varchar(30) = NULL,
@col6     varchar(30) = NULL,
@col7     varchar(30) = NULL,
@col8     varchar(30) = NULL
as

declare @objid int			/* object id of the table */
declare @uid smallint			/* owner id of the object */
declare @cnt smallint			/* howmany columns in key */
declare @key1 tinyint			/* colids of the columns in the key */
declare @key2 tinyint
declare @key3 tinyint
declare @key4 tinyint
declare @key5 tinyint
declare @key6 tinyint
declare @key7 tinyint
declare @key8 tinyint
declare @dbname	varchar(30)

/*
**  Check to see that the tabname is local.
*/
if @tabname like '%.%.%' and
	substring(@tabname, 1, charindex('.', @tabname) - 1) <> db_name()
	begin
		raiserror(15078,-1,-1)
		return (1)
	end

/*
**  See if we can find the object.  It must be a system table, user table,
**  or view.  The low 4 bits of sysobjects.sysstat indicate what the
**  object type is -- it's more reliable than using sysobjects.type which
**  could change.
*/
select @objid = id, @uid = uid
	from sysobjects
		where id = object_id(@tabname)
			and (sysstat & 0xf = 1		/* system table */
				or sysstat & 0xf = 2	/* view */
				or sysstat & 0xf = 3)	/* user table */

if @objid is NULL
	begin
		select @dbname=db_name()
		raiserror(15009,-1,-1,@tabname,@dbname)
		return (1)
	end

/*
**  Check to see that object owner is adding the key.
*/
if @uid <> user_id()
	begin
		raiserror(15172,-1,-1)
		return (1)
	end

/*
**  See if the primary key exists already.  Syskeys.type = 1 indicates
**  a primary key.
*/
if exists (select * from syskeys where id = @objid and type = 1)
	begin
		raiserror(15173,-1,-1)
		return (1)
	end

/*
**  Now check out each column argument to verify its existence.
*/
select @cnt = 1, @key1 = colid
	from syscolumns
		where name = @col1
			and id = @objid
if @key1 is NULL
	begin
		raiserror(15154,-1,-1)
		return (1)
	end

if @col2 IS NOT NULL
	begin
		select @cnt = @cnt + 1, @key2 = colid
			from syscolumns
			where name = @col2
			and id = @objid

		if @key2 is NULL
			begin
				raiserror(15155,-1,-1)
				return (1)
			end
	end
else goto doinsert

if @col3 IS NOT NULL
	begin
		select @cnt = @cnt + 1, @key3 = colid
			from syscolumns
			where name = @col3
			and id = @objid

		if @key3 is NULL
			begin
				raiserror(15156,-1,-1)
				return (1)
			end
	end
else goto doinsert

if @col4 IS NOT NULL
	begin
		select @cnt = @cnt + 1, @key4 = colid
			from syscolumns
			where name = @col4
			and id = @objid

		if @key4 is NULL
			begin
				raiserror(15157,-1,-1)
				return (1)
			end
	end
else goto doinsert

if @col5 IS NOT NULL
	begin
		select @cnt = @cnt + 1, @key5 = colid
			from syscolumns
			where name = @col5
			and id = @objid

		if @key5 is NULL
			begin
				raiserror(15158,-1,-1)
				return (1)
			end
	end
else goto doinsert

if @col6 IS NOT NULL
	begin
		select @cnt = @cnt + 1, @key6 = colid
			from syscolumns
			where name = @col6
			and id = @objid

		if @key6 is NULL
			begin
				raiserror(15159,-1,-1)
				return (1)
			end
	end
else goto doinsert

if @col7 IS NOT NULL
	begin
		select @cnt = @cnt + 1, @key7 = colid
			from syscolumns
			where name = @col7
			and id = @objid

		if @key7 is NULL
			begin
				raiserror(15160,-1,-1)
				return (1)
			end
	end
else goto doinsert

if @col8 IS NOT NULL
	begin
		select @cnt = @cnt + 1, @key8 = colid
			from syscolumns
			where name = @col8
			and id = @objid

		if @key8 is NULL
			begin
				raiserror(15161,-1,-1)
				return (1)
			end
	end

/*
**  Type 1 is a primary key, 2 is a foreignkey, and 3 is a commonjoin.
*/
doinsert:

insert into syskeys
	(id, type, depid, keycnt, size, key1, key2, key3, key4, key5,
		key6, key7, key8)
values (@objid, 1, NULL, @cnt, 0, @key1, @key2, @key3, @key4, @key5,
		@key6, @key7, @key8)

print 'New primary key added.'
return (0)
go


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


raiserror(15339,-1,-1,'sp_recompile')
go
create procedure sp_recompile
@objname	varchar(92)
as
declare @id	int
declare @type	smallint
declare @uid	smallint
declare @errmsg	varchar(250)
declare @dbname varchar(30)

if @objname like '%.%.%'
	begin
		select @dbname=substring(@objname, 1, charindex('.', @objname) - 1)

		if @dbname <> db_name()
			begin
				raiserror(15250,-1,-1)
				return(1)
			end
	end
else
	select @dbname=db_name()

select @id = id, @type = sysstat & 0xf, @uid = uid
from sysobjects
where id = object_id(@objname)

if @id is null
	begin
		raiserror(15009,-1,-1,@objname,@dbname)
		return(1)
	end

if @type = 1
	begin
		raiserror(15217,-1,-1,@objname)
		return(1)
	end
if @type <> 3
	begin
		raiserror(15218,-1,-1,@objname)
		return(1)
end

if user_id() <> 1 and @uid <> user_id()
	begin
		raiserror(15219,-1,-1,@objname)
		return(1)
	end

set nocount on

update sysobjects set schema_ver = schema_ver + 1 where id = @id

select @errmsg = 'Each stored procedure and trigger that uses table ' + @objname
print @errmsg
print 'will be recompiled the next time it is executed.'
go


raiserror(15339,-1,-1,'sp_remoteoption')
go
create procedure sp_remoteoption
@remoteserver varchar(30) = NULL,	/* server name to change */
@loginame varchar(30) = NULL,		/* user's remote name */
@remotename varchar(30) = NULL,		/* user's local user name */
@optname varchar(20) = NULL,		/* option name to turn on/off */
@optvalue varchar(10) = NULL		/* true or false */
as

declare @statvalue smallint		/* number of option */
declare @optcount int			/* number of options like @optname */
declare	@msg	varchar(100)
declare @suid	smallint

/*
**  If no @server given, just list the possible remote login options.
**  Only certain status bits may be set or cleared.
**	   settable                        not settable
**     ------------------------------  --------------------------
**     trusted (1)
*/
if @remoteserver is null
begin
	print 'Settable remote login options.'
	select remotelogin_option = name
		from master.dbo.spt_values
		where type = 'F'
			and number in (1, 1)
			and number > 0
		order by name
	return (0)
end

/*
**  If @loginame is NULL then we want to set @suid = -1. Otherwise get
**  it real value.
*/
if @loginame is null
	select @suid = -1
else select @suid = suser_id(@loginame)

/*
**  Verify the server name, local, and remote names.
*/
if not exists (select *
		from sysremotelogins r, sysservers s
			where r.remoteserverid = s.srvid
				and s.srvname = @remoteserver
				and r.remoteusername = @remotename
				and r.suid = @suid)
	begin

		raiserror(15185,-1,-1,@remotename,@loginame,@remoteserver)
		return (1)
	end

/*
**  Check remaining parameters.
*/
if @optname is NULL or lower(@optvalue) not in ('true', 'false') or @optvalue = null
	begin
		raiserror(15220,-1,-1)
		return (1)
	end

/*
**  Use @optname and try to find the right option.
**  If there isn't just one, print appropriate diagnostics and return.
*/
select @optcount = count(*)
	from master.dbo.spt_values
	where name like '%' + @optname + '%'
		and type = 'F'
		and number in (1, 1)
		and number > 0

/*
**  If no option, show the user what the options are.
*/
if @optcount = 0
	begin
		raiserror(15221,-1,-1)
		return (1)
	end

/*
**  If more than one option like @optname, show the duplicates and return.
*/
if @optcount > 1
	begin
		raiserror(15222,-1,-1,@optname)
		print ''

		select duplicate_option = name
		from master.dbo.spt_values
		where name like '%' + @optname + '%'
			and type = 'F'
			and number in (1, 1)
			and number > 0

		return (1)
	end

/*
**  Only the SA or the dbo of @dbnbame can execute the update part
**  of this procedure so check.  Also the sproc must be executed in the master
**  database.
*/
if suser_id() <> 1
begin
	print 'Only the System Administrator (SA) may set remote login options.'
	return (1)
end

/*
**  If we're in a transaction, disallow this since it might make recovery
**  impossible.
*/
if @@trancount > 0
	begin
		raiserror(15002,-1,-1,'sp_remoteoption')
		return (1)
	end

/*
**  Get the number which is the bit value to set
*/
select @statvalue = number
	from master.dbo.spt_values
	where name like '%' + @optname + '%'
		and type = 'F'
		and number in (1, 1)
		and number > 0


/*
**  Now update sysremotelogins.
*/
if lower(@optvalue) = 'true'
	begin
		update sysremotelogins
		set status = status | @statvalue
			from sysremotelogins r, sysservers s
			where r.remoteserverid = s.srvid
				and s.srvname = @remoteserver
				and r.remoteusername = @remotename
				and r.suid = @suid
		select @msg = 'Option ''' + @optname + ''' turned on.'
		print @msg
		return (0)
	end

/*
**  We want to turn it off.
*/
else
	begin
		update sysremotelogins
		set status = status & ~@statvalue
			from sysremotelogins r, sysservers s
			where r.remoteserverid = s.srvid
				and s.srvname = @remoteserver
				and r.remoteusername = @remotename
				and r.suid = @suid
		select @msg = 'Option ''' + @optname + ''' turned off.'
		print @msg
		return (0)
	end

return (0)
go



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



raiserror(15339,-1,-1,'sp_renamedb')
go
create procedure sp_renamedb
@dbname sysname,			/* old (current) db name */
@newname sysname			/* new name we want to call it */
as

declare @objid int			/* object id of the thing to rename */
declare @bitdesc varchar(30)		/* bit description for the db */
declare @curdbid int			/* id of database to be changed */

/*
**  If we're in a transaction, disallow this since it might make recovery
**  impossible.
*/
if @@trancount > 0
	begin
		raiserror(15002,-1,-1,'sp_renamedb')
		return (1)
	end

/*
**  Only the SA can do this.
*/
if suser_id() <> 1
	begin
		raiserror(15226,-1,-1)
		return (1)
	end

/*
**  Make sure the database exists.
*/
if not exists (select * from sysdatabases where name = @dbname)
	begin
		raiserror(15010,-1,-1,@dbname)
		return (1)
	end

/*
**  Make sure that the @newname db doesn't already exist.
*/
if exists (select * from sysdatabases where name = @newname)
	begin
		raiserror(15032,-1,-1,@newname)
		return (1)
	end

/*
**  Check to see that the @newname is valid.
*/
declare @returncode int
exec @returncode = sp_validname @newname
if @returncode <> 0
	return(1)

/*
**  Don't allow the names of master, tempdb, and model to be changed.
*/
if @dbname in ('master', 'model', 'tempdb')
	begin
		raiserror(15227,-1,-1,@dbname)
		return (1)
	end

/* Get current database id */
select @curdbid = db_id(@dbname)

/*
** Check single user bit (4096)
** Database must be in single user mode to necessitate the rid update in the
** database's DBTABLE
*/
select @bitdesc = null
select @bitdesc = v.name
	from master.dbo.spt_values v, sysdatabases d
	where d.dbid = @curdbid
		and v.type = 'D'
		and d.status & v.number = 4096

if @bitdesc is null
	begin
		raiserror(15228,-1,-1,@dbname)
		return(1)
	end

/*
**  Everything is ok so do it.
*/
update sysdatabases set name = @newname
	where name = @dbname

/*
** Mark dbtable status dirty to update to the new name in Resources.
**
*/
dbcc dbrepair (@newname,markdirty)

print 'Database is renamed and in single-user mode.'
print 'System Administrator (SA) must reset it to multi-user mode with sp_dboption.'

return (0)
go


raiserror(15339,-1,-1,'sp_serveroption')
go
create procedure sp_serveroption --1996/02/28 13:56
@server varchar(30) = NULL,	/* server name to change */
@optname varchar(20) = NULL,	/* option name to turn on/off */
@optvalue varchar(10) = NULL	/* true or false */
as

set ansi_nulls on

declare @srvid int			/* id of the server */
declare @statvalue smallint		/* number of option */
declare @optcount int			/* number of options like @optname */
declare @fallback_bit integer

/*
**  If no @server given, just list the possible server options.
**  Only certain status bits may be set or cleared.
**    settable                        not settable
**    ------------------------------  --------------------------
**	  rpc (1)
**	  pub (2)
**	  sub (4)
**	  dist (8)
**	  dpub (16)
**	  dsn (32)
**	  fallback (64)
*/
if @server is null
	begin
		print 'Settable server options.'
		select server_option = name
			from master.dbo.spt_values
			where type = 'A'
				and number not in (-1)
			order by name
		return (0)
	end

/*
**  Verify the server name and get the @srvid
*/
select @srvid = srvid
	from sysservers
		where srvname = @server

/*
**  If @server not found, say so.
*/
if @srvid is NULL
	begin
		raiserror(15015,-1,-1,@server)
		return (1)
	end

/*
**  Check remaining parameters.
*/
if @optname is NULL or lower(@optvalue) not in ('true', 'false')
	begin
		raiserror(15229,-1,-1)
		return (1)
	end

/*
**  Use @optname and try to find the right option.
**  If there isn't just one, print appropriate diagnostics and return.
*/
select @optcount = count(*) from master.dbo.spt_values
	where name like @optname
		and type = 'A'
		and number not in (-1)

/*
**  If no option, show the user what the options are.
*/
if @optcount = 0
	begin
		raiserror(15230,-1,-1,@optname)
		return (1)
	end

/*
**  If more than one option like @optname, show the duplicates and return.
*/
if @optcount > 1
	begin
		raiserror(15231,-1,-1,@optname)
		print ''

		select duplicate_option = name
			from master.dbo.spt_values
			where name like @optname
				and type = 'A'
				and number not in (-1)
		return (1)
	end

/*
**  Only the SA or the dbo of @dbnbame can execute the update part
**  of this procedure so check.  Also the sproc must be executed in the master
**  database.
*/
if suser_id() <> 1
	begin
		raiserror(15232,-1,-1)
		return (1)
	end

/*
**  Get the number which is the bit value to set
*/
select @statvalue = number
	from master.dbo.spt_values
	where name like @optname
		and type = 'A'
		and number > 0

/*
** Only one server may be a distribution server.
*/
if @optname like 'dist'
  begin
     if lower(@optvalue) = 'true'
        begin
	if exists (select * from sysservers where (srvstatus & @statvalue) <>
	   0)
	   begin
	      raiserror(14090,-1,-1)
	      return (1)
	   end
	/* Add remote mapping of sa, if the distributor is remote. */
	if @srvid <> 0
	   begin
    	      execute sp_addremotelogin @server, sa, sa
              execute sp_remoteoption @server, sa, sa, trusted, true
	   end
	end
     else /* @optvalue = 'false' */
	/* Drop remote mapping of sa, if the distributor is remote. */
        if @srvid <> 0
    	   execute sp_dropremotelogin @server, sa, sa
  end

/*
**  If we're in a transaction, disallow this since it might make recovery
**  impossible.
*/
if @@trancount > 0
	begin
		raiserror(15002,-1,-1,'sp_serveroption')
		return (1)
	end

/*
**  Now update sysservers.
*/
if lower(@optvalue) = 'true'
	begin

	select @fallback_bit = min(val.number) from master..spt_values val
		where val.type='A  ' and val.name='fallback'

	---- Ensure max of 1 fallback server.
	if not 0 = (select count(*) from master..sysservers
			where srvstatus & 64 > 0
		)
		begin
		RaisError(15378,-1,-1)
		return(1)
		end

	update sysservers
		set srvstatus = srvstatus | @statvalue
		where srvid = @srvid
	end

/*
**  We want to turn it off.
*/
else
	update sysservers set srvstatus = srvstatus & ~@statvalue
		where srvid = @srvid

return (0)
go


raiserror(15339,-1,-1,'sp_setnetname')
go
create procedure sp_setnetname  --1995/12/22 13:07
	 @server	varchar(30)	-- server name
	,@netname	varchar(30)	-- new net name
as

---- The server must exist, so check.

if not exists (select * from sysservers where srvname = @server)
	begin
		raiserror(15015,-1,-1,@server)
		return (1)
	end

-- sysservers.srvnetname need not be unique, logic and Msg 15233 removed (1995/12/22).

-- We will not validate @server name here, would be redundant (1995/11/28).

update sysservers set srvnetname = @netname where srvname = @server

print 'Server network name set.'
return (0)
go

dump transaction master with no_log
go


raiserror(15339,-1,-1,'sp_spaceused')
go
create procedure sp_spaceused
@objname varchar(92) = null,		-- The object we want size on.
@updateusage varchar(5) = false		-- Param. for specifying that
					-- usage info. should be updated.
as

declare @id	int			-- The object id of @objname.
declare @type	smallint		-- The object type.
declare	@pages	int			-- Working variable for size calc.
declare @dbname varchar(30)
declare @dbsize dec(15,0)

/*
**  Check to see if user wants usages updated.
*/

if @updateusage is not null
	begin
		select @updateusage=lower(@updateusage)

		if @updateusage not in ('true','false')
			begin
				raiserror(15143,-1,-1,@updateusage)
				return(1)
			end
	end
/*
**  Check to see that the objname is local.
*/
if @objname IS NOT NULL
begin
	if @objname like '%.%.%'
		begin
			select @dbname =
				substring(@objname, 1, charindex('.', @objname) - 1)

			if @dbname <> db_name()
				begin
					raiserror(15250,-1,-1)
					return (1)
				end
		end
	else
		select @dbname = db_name()

	/*
	**  Try to find the object.
	*/
	select @id = null
	select @id = id, @type = sysstat & 0xf
		from sysobjects
			where id = object_id(@objname)

	/*
	**  Does the object exist?
	*/
	if @id is null
		begin
			raiserror(15009,-1,-1,@objname,@dbname)
			return (1)
		end

	/*
	**  See if it's a space object.
	**  types are:
	**	1 - system table
	**	2 - view
	**	3 - user table
	**	4 - sproc
	**	6 - default
	**	7 - rule
	**	8 - trigger
	**	9 - primary key
	**	10 - check constraint
	**	11 - foreign key
	**	12 - replication filter stored proc
	*/

	if not exists (select * from sysindexes
				where @id = id and indid < 2)

		if @type in (4, 6, 7, 8, 10, 12)	-- data stored
				begin			-- in sysprocedures
					raiserror(15234,-1,-1)
					return (1)
				end
		else if @type = 2	-- View => no physical data storage.
				begin
					raiserror(15235,-1,-1)
					return (1)
				end
		else if @type = 9	-- PK => no physical data storage.
				begin
					raiserror(15064,-1,-1)
					return (1)
				end
		else if @type = 11	-- FK => no physical data storage.
				begin
					raiserror(15275,-1,-1)
					return (1)
				end
end

/*
**  Update usages if user specified to do so.
*/

if @updateusage = 'true'
	begin
		if @objname is null
			dbcc updateusage(0) with no_infomsgs
		else
			dbcc updateusage(0,@objname) with no_infomsgs
		print ''
	end

/*
**  We need to create a temp table to do the calculation.
**  reserved: sum(reserved) where indid in (0, 1, 255)
**  data: sum(dpages) where indid < 2 + sum(used) where indid = 255 (text)
**  indexp: sum(used) where indid in (0, 1, 255) - data
**  unused: sum(reserved) - sum(used) where indid in (0, 1, 255)
*/
create table #spt_space
(
	rows		int null,
	reserved	dec(15) null,
	data		dec(15) null,
	indexp		dec(15) null,
	unused		dec(15) null
)
set nocount on

/*
**  If @id is null, then we want summary data.
*/
if @id is null
begin
	select @dbsize = sum(convert(dec(15),size))
		from sysusages
		where dbid = db_id()

	select  database_name = db_name(),
		database_size =
			ltrim(str(@dbsize / 512,15,2) + ' MB'),
		'unallocated space' =
			ltrim(str((@dbsize -
				(select sum(convert(dec(15),reserved))
					from sysindexes
						where indid in (0, 1, 255)
				))/512,15,2)+ ' MB')

	print ''
	/*
	**  Now calculate the summary data.
	**  reserved: sum(reserved) where indid in (0, 1, 255)
	*/
	insert into #spt_space (reserved)
		select sum(convert(dec(15),reserved))
			from sysindexes
				where indid in (0, 1, 255)

	/*
	** data: sum(dpages) where indid < 2
	**	+ sum(used) where indid = 255 (text)
	*/
	select @pages = sum(convert(dec(15),dpages))
			from sysindexes
				where indid < 2
	select @pages = @pages + isnull(sum(convert(dec(15),used)), 0)
		from sysindexes
			where indid = 255
	update #spt_space
		set data = @pages


	/* index: sum(used) where indid in (0, 1, 255) - data */
	update #spt_space
		set indexp = (select sum(convert(dec(15),used))
				from sysindexes
					where indid in (0, 1, 255))
			    - data

	/* unused: sum(reserved) - sum(used) where indid in (0, 1, 255) */
	update #spt_space
		set unused = reserved
				- (select sum(convert(dec(15),used))
					from sysindexes
						where indid in (0, 1, 255))

	select reserved = ltrim(str(reserved * d.low / 1024.,15,0) +
				' ' + 'KB'),
		data = ltrim(str(data * d.low / 1024.,15,0) +
				' ' + 'KB'),
		index_size = ltrim(str(indexp * d.low / 1024.,15,0) +
				' ' + 'KB'),
		unused = ltrim(str(unused * d.low / 1024.,15,0) +
				' ' + 'KB')
		from #spt_space, master.dbo.spt_values d
		where d.number = 1
			and d.type = 'E'
end

/*
**  We want a particular object.
*/
else
begin
	/*
	**  Now calculate the summary data.
	**  reserved: sum(reserved) where indid in (0, 1, 255)
	*/
	insert into #spt_space (reserved)
		select sum(reserved)
			from sysindexes
				where indid in (0, 1, 255)
					and id = @id

	/*
	** data: sum(dpages) where indid < 2
	**	+ sum(used) where indid = 255 (text)
	*/
	select @pages = sum(dpages)
			from sysindexes
				where indid < 2
					and id = @id
	select @pages = @pages + isnull(sum(used), 0)
		from sysindexes
			where indid = 255
				and id = @id
	update #spt_space
		set data = @pages


	/* index: sum(used) where indid in (0, 1, 255) - data */
	update #spt_space
		set indexp = (select sum(used)
				from sysindexes
					where indid in (0, 1, 255)
						and id = @id)
			    - data

	/* unused: sum(reserved) - sum(used) where indid in (0, 1, 255) */
	update #spt_space
		set unused = reserved
				- (select sum(used)
					from sysindexes
						where indid in (0, 1, 255)
							and id = @id)
	update #spt_space
		set rows = i.rows
			from sysindexes i
				where i.indid < 2
					and i.id = @id

	select name = substring(object_name(@id), 1, 20),
		rows = convert(char(11), rows),
		reserved = ltrim(str(reserved * d.low / 1024.,15,0) +
				' ' + 'KB'),
		data = ltrim(str(data * d.low / 1024.,15,0) +
				' ' + 'KB'),
		index_size = ltrim(str(indexp * d.low / 1024.,15,0) +
				' ' + 'KB'),
		unused = ltrim(str(unused * d.low / 1024.,15,0) +
				' ' + 'KB')
	from #spt_space, master.dbo.spt_values d
		where d.number = 1
			and d.type = 'E'
end

return (0)
go


raiserror(15339,-1,-1,'sp_sqlexec')
go
create procedure sp_sqlexec @p1 text as exec(@p1)
go



raiserror(15339,-1,-1,'sp_unbindefault')
go
create procedure sp_unbindefault
--1995/09/14 15:06
@objname varchar(61),         /* table/column or datatype name */
@futureonly varchar(15) = NULL   /* flag to indicate extent of binding */
as

declare @olddefault int  --current default for type
declare @tabname varchar(92)  --possible name of table
       ,@colname varchar(92)  --possible name of column

declare @futurevalue varchar(15) /* the value of @futureonly that causes
               ** the binding to be limited */

select @futurevalue = 'futureonly'  /* initialize @futurevalue */

/*
**  When a default or rule is bound to a user-defined datatype, it is also
**  bound, by default, to any columns of the user datatype that are currently
**  using the existing default or rule as their default or rule.  This default
**  action may be overridden by setting @futureonly = @futurevalue when the
**  procedure is invoked.  In this case existing columns with the user
**  datatype won't have their existing default or rule changed.
*/

/*
**  Check to see that the object names are local to the current database.
*/
if @objname like '%.%.%'  --61?
   and substring(@objname, 1, charindex('.', @objname) - 1) <> db_name()
   begin
      raiserror(15084,-1,-1)
      return (1)
   end

/*
**  If @objname is of the form tab.col then we are unbinding a column.
**  Otherwise its a datatype.  In the column case, we need to extract
**  and verify the table and column names and make sure the user owns
**  the table that is getting the default unbound.
*/
if @objname like '%.%'
begin
   /*
   **  Get the table name out.
   */
   select @tabname = substring(@objname, 1, charindex('.', @objname) - 1)
   select @colname = substring(@objname, charindex('.', @objname) + 1, 61)

   if (     datalength(@tabname) > 30
      or    datalength(@colname) > 30
      )
      begin
      raiserror(15299,-1,-1,@objname)
      return (1)
      end

   /*
   **  Find it and unbind it.
   */
   if exists (select *
         from syscolumns, sysobjects
      where syscolumns.id = object_id(@tabname)
         and syscolumns.name = @colname
         and sysobjects.id = object_id(@tabname)
         and uid = user_id()
         and sysobjects.sysstat & 0xf = 3)      /* user table */
   begin
      /*
      **  Is there something bound to it?
      */
      if exists (select *
         from syscolumns, sysobjects
      where syscolumns.id = object_id(@tabname)
         and syscolumns.name = @colname
         and sysobjects.id = object_id(@tabname)
         and uid = user_id()
         and sysobjects.sysstat & 0xf = 3      /* user table */
         and cdefault <> 0)
      begin
         update syscolumns
            set cdefault = 0
               from syscolumns, sysobjects
            where syscolumns.id = object_id(@tabname)
               and syscolumns.name = @colname
               and sysobjects.id = object_id(@tabname)
               and uid = user_id()
               and sysobjects.sysstat & 0xf = 3

         /*
         **  Since binding a default is a schema change,
         **  update schema count
         **  for the object in the sysobjects table.
         */
         update sysobjects set schema_ver = schema_ver + 1
            from sysobjects
            where id = object_id(@tabname)
               and uid = user_id()

         print 'Default unbound from table column.'
         return (0)
      end

      else
         begin
            raiserror(15236,-1,-1,@objname)
            return (1)
         end
   end

   /*
   **  If we got here, there was no such object.
   */
   raiserror(15104,-1,-1,@tabname,@colname)
   return (1)

end

else

begin
   /*
   **  We're unbinding to a user type.  In this case, the @objname
   **  is really the name of the user datatype.
   **  When we unbind to a user type, any existing columns get changed
   **  to the new binding unless their current binding is not equal
   **  to the current binding for the usertype or if they set the
   **  @futureonly parameter to @futurevalue.
   */

   if (     datalength(@objname) > 30)
      begin
      raiserror(15299,-1,-1,@objname)
      return (1)
      end

   /*
   **  Get the current default for the datatype.
   */
   select @olddefault = tdefault
      from systypes
         where name = @objname
            and uid = user_id()
            and usertype > 100

   if @olddefault is null
      begin
         raiserror(15036,-1,-1,@objname)
         return (1)
      end

   if @olddefault = 0
      begin
         raiserror(15237,-1,-1,@objname)
         return (1)
      end

   update systypes set tdefault = 0
      from systypes
      where name = @objname
         and uid = user_id()
         and usertype > 100

   print 'Default unbound from datatype.'

   /*
   **  Now see if there are any columns with the usertype that
   **  need the new binding.
   */
   if isnull(@futureonly, '') <> @futurevalue
      and exists (select *
         from syscolumns, systypes
            where syscolumns.usertype = systypes.usertype
               and systypes.name = @objname
               and systypes.usertype > 100
               and systypes.uid = user_id()
               and syscolumns.cdefault = @olddefault)
   begin
      /*
      **  Update syscolumns with new binding.
      */
      update syscolumns set cdefault = systypes.tdefault
         from syscolumns, systypes
         where syscolumns.usertype = systypes.usertype
            and systypes.name = @objname
            and systypes.usertype > 100
            and systypes.uid = user_id()
            and syscolumns.cdefault = @olddefault

      /*
      **  Update the table schema to indicate that something
      **  has changed in the table's schema.
      */
      update sysobjects set schema_ver = schema_ver + 1
         from sysobjects, syscolumns, systypes
         where sysobjects.id = syscolumns.id
            and syscolumns.usertype = systypes.usertype
            and systypes.name = @objname
            and systypes.usertype > 100
            and systypes.uid = user_id()
            and syscolumns.cdefault = systypes.tdefault

      print 'Columns of the user datatype specified had their defaults unbound.'
   end
end

return (0)
go



raiserror(15339,-1,-1,'sp_unbindrule')
go
create procedure sp_unbindrule
--1995/09/14 15:24
@objname varchar(61),         /* table/column or datatype name */
@futureonly varchar(15) = NULL      /* flag to indicate extent of binding */
as

declare @oldrule int /* current rule for type */
declare @tabname varchar(30)     /* name of table */
declare @colname varchar(30)     /* name of column */
declare @futurevalue varchar(15) /* the value of @futureonly that causes
                           ** the binding to be limited */

select @futurevalue = 'futureonly'  /* initialize @futurevalue */

/*
**  When a default or rule is bound to a user-defined datatype, it is also
**  bound, by default, to any columns of the user datatype that are currently
**  using the existing default or rule as their default or rule.  This default
**  action may be overridden by setting @futureonly = @futurevalue when the
**  procedure is invoked.  In this case existing columns with the user
**  datatype won't have their existing default or rule changed.
*/

/*
**  Check to see that the object names are local to the current database.
*/
if @objname like '%.%.%'
   begin
      raiserror(15084,-1,-1)
      return (1)
   end

/*
**  If @objname is of the form tab.col then we are unbinding a column.
**  Otherwise its a datatype.  In the column case, we need to extract
**  and verify the table and column names and make sure the user owns
**  the table that is getting the default unbound.
*/
if @objname like '%.%'
begin
   /*
   **  Get the table name out.
   */
   select @tabname = substring(@objname, 1, charindex('.', @objname) - 1)
   select @colname = substring(@objname, charindex('.', @objname) + 1, 61)

   if (     datalength(@tabname) > 30
      or    datalength(@colname) > 30
      )
      begin
      raiserror(15299,-1,-1,@objname)
      return (1)
      end

   /*
   **  Find it and unbind it.
   */
   if exists (select * from syscolumns, sysobjects
      where syscolumns.id = object_id(@tabname)
         and syscolumns.name = @colname
         and sysobjects.id = object_id(@tabname)
         and uid = user_id()
         and sysobjects.sysstat & 0xf = 3)      /* user table */
   begin
      /*
      **  Is there something bound to it?
      */
      if exists (select * from syscolumns, sysobjects
      where syscolumns.id = object_id(@tabname)
         and syscolumns.name = @colname
         and sysobjects.id = object_id(@tabname)
         and uid = user_id()
         and sysobjects.sysstat & 0xf = 3      /* user table */
         and domain <> 0)
      begin
         update syscolumns set domain = 0
            from syscolumns, sysobjects
            where syscolumns.id = object_id(@tabname)
               and syscolumns.name = @colname
               and sysobjects.id = object_id(@tabname)
               and uid = user_id()
               and sysobjects.sysstat & 0xf = 3

         /*
         **  Since binding a rule is a schema change,
         **  update schema count
         **  for the object in the sysobjects table.
         */
         update sysobjects set schema_ver = schema_ver + 1
            from sysobjects
            where id = object_id(@tabname)
               and uid = user_id()

         print 'Rule unbound from table column.'
         return (0)
      end

      else
         begin
            raiserror(15238,-1,-1,@colname)
            return (1)
         end
   end

   /*
   **  If we got here, there was no such object.
   */
   raiserror(15104,-1,-1,@tabname,@colname)
   return (1)

end

else

begin
   /*
   **  We're unbinding to a user type.  In this case, the @objname
   **  is really the name of the user datatype.
   **  When we unbind to a user type, any existing columns get changed
   **  to the new binding unless their current binding is not equal
   **  to the current binding for the usertype or if they set the
   **  @futureonly parameter to @futurevalue.
   */

   if (     datalength(@objname) > 30)
      begin
      raiserror(15299,-1,-1,@objname)
      return (1)
      end

   /*
   **  Get the current rule for the datatype.
   */
   select @oldrule = domain
   from systypes
   where name = @objname
      and uid = user_id()
      and usertype > 100

   if @oldrule is null
      begin
         raiserror(15036,-1,-1,@objname)
         return (1)
      end

   if @oldrule = 0
      begin
         raiserror(15239,-1,-1,@objname)
         return (1)
      end

   update systypes set domain = 0
      from systypes
      where name = @objname
         and uid = user_id()
         and usertype > 100

   print 'Rule unbound from datatype.'

   /*
   **  Now see if there are any columns with the usertype that
   **  need the new binding.
   */
   if isnull(@futureonly, '') <> @futurevalue
      and exists (select * from syscolumns, systypes
            where syscolumns.usertype = systypes.usertype
               and systypes.name = @objname
               and systypes.usertype > 100
               and systypes.uid = user_id()
               and syscolumns.domain = @oldrule)
   begin
      /*
      **  Update syscolumns with new binding.
      */
      update syscolumns set domain = systypes.domain
         from syscolumns, systypes
         where syscolumns.usertype = systypes.usertype
            and systypes.name = @objname
            and systypes.usertype > 100
            and systypes.uid = user_id()
            and syscolumns.domain = @oldrule

      /*
      **  Update the table schema to indicate that something
      **  has changed in the table's schema.
      */
      update sysobjects set schema_ver = schema_ver + 1
         from sysobjects, syscolumns, systypes
         where sysobjects.id = syscolumns.id
            and syscolumns.usertype = systypes.usertype
            and systypes.name = @objname
            and systypes.usertype > 100
            and systypes.uid = user_id()
            and syscolumns.domain = systypes.domain

      print 'Columns of the user datatype specified had their rules unbound.'
   end
end

return (0)
go



raiserror(15339,-1,-1,'sp_unmakestartup')
go
create procedure sp_unmakestartup
@procname varchar(30) = null
as
declare @objid	int
declare @dbname varchar(30)

/*
**	Check to see if a proc. name was specified.
*/
if @procname is null
	begin
		raiserror(15240,-1,-1)
		return (1)
	end

/*
**  Only the SA can do this.
*/
if suser_id() <> 1
	begin
		raiserror(15003,-1,-1)
		return (1)
	end

/*
**  Make sure the @procname is in the master database.
*/
if @procname like '%.%.%' and	-- Explicit db name supplied
	substring(@procname, 1, charindex('.', @procname) - 1) <> 'master'
	begin
		raiserror(15272,-1,-1,@procname)
		return (1)
	end
else	-- No explicit name supplied so we need to be in the 'master' db
	if db_name() <> 'master'
		begin
			select @dbname = db_name()
			raiserror(15274,-1,-1,@procname,@dbname)
			return(1)
		end

/*
**  Make sure the procedure exists and get its id so we can use
**	a simpler where clause.
*/
select @objid = id from master..sysobjects
	where sysstat & 0xf = 4
	and name = @procname

if @objid is null
	begin
		raiserror(15020,-1,-1,@procname)
		return (1)
	end

/*
**	If we made it to here, set it to no longer be 'startup'.
*/
if (select category & 16 from master..sysobjects where id = @objid) = 16
	begin
		update master..sysobjects set category = category & ~16
			where id = @objid
		print 'Procedure is no longer marked as ''startup''.'
	end
else
	print 'Procedure was not designated as ''startup''.'

return (0)
go



raiserror(15339,-1,-1,'sp_who')
go
create procedure sp_who  --1995/11/28 15:48
       @loginame varchar(30) = NULL --or 'active'
as

declare	 @low		int
	,@high		int
	,@spidlow	int
	,@spidhigh	int

select	 @low		=     0
	,@high		= 32767

select	 @spidlow	=     0
	,@spidhigh	= 32767


if (	@loginame is not NULL
   AND	upper(@loginame) <> 'ACTIVE'
   )
begin
	select @low = suser_id(@loginame), @high = suser_id(@loginame)
	if @low is NULL
	begin
		if @loginame like '[0-9]%'
		begin
			select	@spidlow  = convert(int, @loginame),
				@spidhigh = convert(int, @loginame),
				@low = 0, @high = 32767
		end
		else
		begin
			raiserror(15007,-1,-1,@loginame)
			return (1)
		end
	end
end


if (	@loginame is not NULL
   AND	upper(@loginame) = 'ACTIVE'
   )
	begin

	select spid ,status
              ,loginame=substring(suser_name(suid),1,12)
	      ,hostname ,blk=convert(char(5),blocked)
	      ,dbname=substring(db_name(dbid),1,10),cmd
	from  sysprocesses
	where suid >= @low     and suid <= @high
	and   spid >= @spidlow and spid <= @spidhigh
	AND   upper(cmd) <> 'AWAITING COMMAND'

	end
else
	begin

	select spid ,status
              ,loginame=substring(suser_name(suid),1,12)
	      ,hostname ,blk=convert(char(5),blocked)
	      ,dbname=substring(db_name(dbid),1,10),cmd
	from  sysprocesses
	where suid >= @low     and suid <= @high
	and   spid >= @spidlow and spid <= @spidhigh

	end

return (0)
go



raiserror(15339,-1,-1,'sp_who2')
go
CREATE PROCEDURE sp_who2  --1995/11/03 10:16

    @loginame     varchar(30) = NULL
as

set nocount on

declare
    @retcode         int
   ,@max_suid_spid   int
   ,@int1            int

declare
    @suidlow         int
   ,@suidhigh        int
   ,@spidlow         int
   ,@spidhigh        int

declare
    @charMaxLenLoginName      varchar(6)
   ,@charMaxLenDBName         varchar(6)
   ,@charMaxLenCPUTime        varchar(10)
   ,@charMaxLenDiskIO         varchar(10)
   ,@charMaxLenHostName       varchar(10)
   ,@charMaxLenProgramName    varchar(10)
   ,@charMaxLenLastBatch      varchar(10)
   ,@charMaxLenCommand        varchar(10)

declare
    @charsuidlow              varchar(11)
   ,@charsuidhigh             varchar(11)

   ,@charspidlow              varchar(11)
   ,@charspidhigh             varchar(11)

--------

select
    @retcode         = 0      -- 0=good ,1=bad.
   ,@max_suid_spid   = 32767

--------defaults
select
    @suidlow         = 0
   ,@suidhigh        = @max_suid_spid

   ,@spidlow         = 0
   ,@spidhigh        = @max_suid_spid

--------------------------------------------------------------


IF (@loginame IS     NULL)  --Simple default to all LoginNames.
      GOTO LABEL_17PARM1EDITED

--------

select @int1 = suser_id(@loginame)

IF (@int1 IS NOT NULL)  --Parm is a recognized login name.
   begin
   select @suidlow  = suser_id(@loginame)
         ,@suidhigh = suser_id(@loginame)
   GOTO LABEL_17PARM1EDITED
   end

--------

IF (lower(@loginame) IN ('active'))  --Special action, not sleeping.
   begin
   select @loginame = lower(@loginame)
   GOTO LABEL_17PARM1EDITED
   end

--------

IF (patindex ('%[^0-9]%' , isnull(@loginame,'z')) = 0)  --Is a number.
   begin
   select
             @spidlow   = convert(int, @loginame)
            ,@spidhigh  = convert(int, @loginame)
   GOTO LABEL_17PARM1EDITED
   end

--------

RaisError(15007,-1,-1,@loginame)
select @retcode = 1
GOTO LABEL_86RETURN


LABEL_17PARM1EDITED:


--------------------  Capture consistent sysprocesses.  -------------------

SELECT

  spid
 ,kpid
 ,status
 ,suid
 ,hostname
 ,program_name
 ,hostprocess
 ,cmd
 ,cpu
 ,physical_io
 ,memusage
 ,blocked
 ,waittype
 ,dbid
 ,uid
 ,gid
 ,login_time
 ,last_batch
 ,nt_domain
 ,nt_username
 ,net_address
 ,net_library

 ,spid as 'spid_sort'

 ,  substring( convert(varchar,last_batch,111) ,6  ,5 ) + ' '
  + substring( convert(varchar,last_batch,113) ,13 ,8 )
       as 'last_batch_char'

      INTO    #tb1_sysprocesses
      from master..sysprocesses   (nolock)



--------Screen out any rows?

IF (@loginame IN ('active'))
   DELETE #tb1_sysprocesses
         where   lower(status)  = 'sleeping'
         and     upper(cmd)    IN (
                     'AWAITING COMMAND'
                    ,'MIRROR HANDLER'
                    ,'LAZY WRITER'
                    ,'CHECKPOINT SLEEP'
                    ,'RA MANAGER'
                                  )

         and     blocked       = 0



--------Prepare to dynamically optimize column widths.


Select
    @charsuidlow     = convert(varchar,@suidlow)
   ,@charsuidhigh    = convert(varchar,@suidhigh)
   ,@charspidlow     = convert(varchar,@spidlow)
   ,@charspidhigh    = convert(varchar,@spidhigh)



SELECT
             @charMaxLenLoginName =
                  convert( varchar
                          ,isnull( max( datalength( convert(varchar,suser_name(suid)))) ,5)
                         )

            ,@charMaxLenDBName    =
                  convert( varchar
                          ,isnull( max( datalength( convert(varchar,db_name(dbid)))) ,6)
                         )

            ,@charMaxLenCPUTime   =
                  convert( varchar
                          ,isnull( max( datalength( convert(varchar,cpu))) ,7)
                         )

            ,@charMaxLenDiskIO    =
                  convert( varchar
                          ,isnull( max( datalength( convert(varchar,physical_io))) ,6)
                         )

            ,@charMaxLenCommand  =
                  convert( varchar
                          ,isnull( max( datalength( convert(varchar,cmd))) ,7)
                         )

            ,@charMaxLenHostName  =
                  convert( varchar
                          ,isnull( max( datalength( convert(varchar,hostname))) ,8)
                         )

            ,@charMaxLenProgramName =
                  convert( varchar
                          ,isnull( max( datalength( convert(varchar,program_name))) ,11)
                         )

            ,@charMaxLenLastBatch =
                  convert( varchar
                          ,isnull( max( datalength( convert(varchar,last_batch_char))) ,9)
                         )
      from
             #tb1_sysprocesses
      where
             suid >= @suidlow
      and    suid <= @suidhigh
      and
             spid >= @spidlow
      and    spid <= @spidhigh



--------Output the report.


EXECUTE(
'
SET nocount off

SELECT
             SPID          = convert(char(5),spid)

            ,Status        =
                  CASE lower(status)
                     When ''sleeping'' Then lower(status)
                     Else                   upper(status)
                  END

            ,Login         = substring(suser_name(suid),1,' + @charMaxLenLoginName + ')

            ,HostName      =
                  CASE hostname
                     When Null  Then ''  .''
                     When '' '' Then ''  .''
                     Else    substring(hostname,1,' + @charMaxLenHostName + ')
                  END

            ,BlkBy         =
                  CASE               isnull(convert(char(5),blocked),''0'')
                     When ''0'' Then ''  .''
                     Else            isnull(convert(char(5),blocked),''0'')
                  END

            ,DBName        = substring(db_name(dbid),1,' + @charMaxLenDBName + ')
            ,Command       = substring(cmd,1,' + @charMaxLenCommand + ')

            ,CPUTime       = substring(convert(varchar,cpu),1,' + @charMaxLenCPUTime + ')
            ,DiskIO        = substring(convert(varchar,physical_io),1,' + @charMaxLenDiskIO + ')

            ,LastBatch     = substring(last_batch_char,1,' + @charMaxLenLastBatch + ')

            ,ProgramName   = substring(program_name,1,' + @charMaxLenProgramName + ')
            ,SPID          = convert(char(5),spid)  --Handy extra for right-scrolling users.
      from
             #tb1_sysprocesses  --Usually DB qualification is needed in exec().
      where
             suid >= ' + @charsuidlow  + '
      and    suid <= ' + @charsuidhigh + '
      and
             spid >= ' + @charspidlow  + '
      and    spid <= ' + @charspidhigh + '

      -- (Seems always auto sorted.)   order by spid_sort

SET nocount on
'
)



LABEL_86RETURN:


if (object_id('tempdb..#tb1_sysprocesses') is not null)
            drop table #tb1_sysprocesses

return @retcode
go


---------------------------------------------------------------------------------------------
---------------------------------------------------------------------------------------------
---------------------------------------------------------------------------------------------


/*
** Create following procs last, since they reference other procedures.
*/



raiserror(15339,-1,-1,'sp_check_removable')
go
create procedure sp_check_removable @autofix varchar(4)
as

declare @dbouid int
declare @dbname varchar(30)
declare @logdev varchar(30)

select @dbname=db_name()
	
/* Verify that SA owns the database. */

select @dbouid = suid from sysdatabases where name = @dbname
if @dbouid <> 1
	if @autofix='auto'
		begin
			print 'Setting db owner to SA.'
			update sysdatabases set suid = 1
				where name = @dbname
			update sysusers set suid = 1
				where uid = 1
		end
	else
		begin
			raiserror(15258,-1,-1)
			return(1)
		end

/* Check for any objects not owned by dbo. */
if exists (select * from sysobjects where uid <> 1)

	if @autofix='auto'
		begin
			print 'Giving ownership of all objects to dbo.'
			update sysobjects set uid = 1
		end
	else
		begin
			raiserror(15053,-1,-1)
			return(1)
		end

/* See if any non-dbo/guest users exist in db */
if exists (select * from sysusers where uid between 3 and 16382)
	if @autofix='auto'
		begin
			print 'Deleting non guest/dbo users from ''sysusers''.'
			delete from sysusers where uid between 3 and 16382
			delete from sysprotects where uid not in
				(select uid from sysusers)
			delete from sysalternates where altsuid not in
				(select suid from sysusers)
		end
	else
		begin
			raiserror(15254,-1,-1)
			return(1)
		end

/* If we made it to here, move the log segment to the system segment. */
update sysusages set segmap = 5 where dbid = db_id(@dbname) and segmap = 1
dbcc dbrepair (@dbname,remap) with no_infomsgs

/* Save the old log device's name and unmap the log segment from it. */
select @logdev = v.name from sysusages u,sysdevices v
	where u.dbid = db_id(@dbname)
		and u.segmap = 4
		and u.vstart = low

update sysusages set segmap = 0
	where dbid = db_id(@dbname) and segmap = 4
dbcc dbrepair (@dbname,remap) with no_infomsgs

/* Force first log page onto new device. */
while exists
	(select * from sysindexes where name = 'syslogs' and root not between
		(select lstart from sysusages
			where dbid = db_id(@dbname) and segmap = 5)
		and
		(select lstart+size-1 from sysusages
		 where dbid = db_id(@dbname) and segmap = 5))
or exists
	(select * from sysindexes where name = 'syslogs' and first not between
		(select lstart from sysusages
			where dbid = db_id(@dbname) and segmap = 5)
		and
		(select lstart+size-1 from sysusages
			where dbid = db_id(@dbname) and segmap = 5))

	begin
		update sysobjects set name = name	-- no_op to generate
							-- log activity

		exec('dump tran '+@dbname+' with no_log')
	end

/* Delete the old log segment and drop its device. */

delete from sysusages where dbid = db_id(@dbname) and segmap = 0
dbcc dbrepair (@dbname,remap) with no_infomsgs

exec sp_dropdevice @logdev
return(0)
go



raiserror(15339,-1,-1,'sp_certify_removable')
go
CREATE PROCEDURE sp_certify_removable  --1996/03/12 12:02

        @dbname varchar(30) = null,
        @autofix varchar(4) = null
as

set nocount on

declare @ret_value int,
        @vc1 varchar(255),
        @SQLPath varchar(255),
        @OneMb int,
        @countloop1 int,
	@char_autofix varchar(25)

declare
       @cu1PKSequence      int
      ,@cu1DeviceName      varchar( 30)
      ,@cu1SegmentType     varchar( 12)
      ,@cu1FragmentSizeMb  int
      ,@cu1DeviceFileName  varchar(127)


select  @OneMb = 1048576


if suser_id() <> 1      -- Make sure that it is the SA executing this.
        begin
                raiserror(15003,-1,-1)
                return(1)
        end


select @autofix = lower(@autofix)

if @autofix <> 'auto' and @autofix is not null
        begin
                raiserror(15255,-1,-1,@autofix)
                return(1)
        end


if @dbname is null      -- Show usage diagram if no dbname supplied.
        begin
                raiserror(15256,-1,-1)
                return(1)
        end


--See if DB exists.
if not exists (select * from sysdatabases where name = @dbname)
        begin
                raiserror(15010,-1,-1,@dbname)
                return(1)
        end


--Cannot take master, tempdb or model databases offline.
if lower(@dbname) in ('master','tempdb','model')
        begin
                raiserror(15266,-1,-1,@dbname)
                return(1)
        end


-- Will not be able to take db offline if user is in it.
if @dbname = db_name()
        begin
                raiserror(15257,-1,-1)
                return(1)
        end


------------------  validate fragment continuity  -----------------


--Coalesce any contiguous device fragments for this database.
exec sp_coalesce_fragments @dbname


--Verify that two or more noncontiguous fragments do not exist on any
--   device for the database.
if exists (select * from sysusages u1,sysdevices v
                where u1.dbid = db_id(@dbname)
                and u1.vstart between v.low and v.high
                and exists (
                            select * from sysusages u2
                                where u2.dbid=u1.dbid
                                and u2.vstart between v.low and v.high
                                and u2.vstart <> u1.vstart
                           )
          )
        begin
                raiserror(15260,-1,-1)
                return(1)
        end


--Verify that the database spans at least 3 devices (system, data and
--   log) and that segment mappings are proper.
if
        (select count(*) from sysusages where dbid = db_id(@dbname)
                and segmap = 1) <> 1    -- dedicated system device
        or (select count(*) from sysusages where dbid = db_id(@dbname)
                and segmap = 4) <> 1    -- dedicated log device
        or (select count(*) from sysusages where dbid = db_id(@dbname)
                and segmap = 2) < 1     -- dedicated data device

                begin
                        raiserror(15259,-1,-1)
                        return(1)
                end


-------------------  Ensure IRL not On for any user-tables

EXECUTE(  --13677
'
use ' +@dbname+ '

if (0 < (select count(*) from sysobjects where type=''U '' and userstat & 1 > 0))
   exec sp_tableoption ''%.%'' ,''insert row lock'' ,''false''
')


-------------  Check things that exist only in the db.  -------------------


select @char_autofix =
	CASE
	   When @autofix IS NOT Null Then '''Auto'''
	   Else                           'Null'
	END


execute(
'
use ' + @dbname + '

declare @inx_ret_value int ,@int1 int
select  @inx_ret_value = 1

exec @inx_ret_value = sp_check_removable ' + @char_autofix + '

--Use @@rowcount for a user_assignable global variable for communication.
if @inx_ret_value <> 0	--bad
	begin
	select @int1 = suid from master.dbo.syslogins where suid=1 --SA
	return
	end
else
	begin
	select @int1 = suid from master.dbo.syslogins where suid=1 and 1=2
	return
	end
'
)

if @@rowcount > 0
        return (1)  --Error was returned by other proc, so exit


-------------------  report on the db devices  ----------------------


raiserror('',0,1)
raiserror(15309,-1,-1)  --"Please write down this crucial info."
raiserror('' ,0,1)


create table #tb1_ForDBInstall
   (
    PKSequence          int            not null    identity(1,1)
   ,UG_LStart           int            not null
   ,DeviceName          varchar(30)        null
   ,SegmentType         varchar(12)        null
   ,FragmentSizeMb      int                null
   ,DeviceFileName      varchar(127)       null
   )



insert
      into #tb1_ForDBInstall
         (
          -- PKSequence,
          UG_LStart
         ,DeviceName
         ,SegmentType
         ,FragmentSizeMb
         ,DeviceFileName
         )
   select
             -- identitycol,
             u.lstart
            ,v.name

            ,case u.segmap
                when 5 then 'System + Log'
                when 2 then 'Data'
             end

            ,convert( int
                     ,round( (
                (select low
                   from master.dbo.spt_values
                   where type = 'E' and number = 1
                )
                     * convert(float, u.size)
                             ) / @OneMb
                          ,0
                           )
                    )

            ,phyname
   from
             sysdevices    v
            ,sysusages     u
   where
             u.dbid   = db_id(@dbname)
   and       u.vstart = v.low
   order by
             lstart



select
             'Sequence'    = convert(char(8),PKSequence)
            ,DeviceName
            ,SegmentType
            ,FragmentSizeMb
            ,DeviceFileName
      from
             #tb1_ForDBInstall
      order by
             UG_LStart ,PKSequence  --Should match plain PKSequence.

raiserror('',0,1) with nowait


------------  Make a ReadMe style file named Log\CertifyR_<dbname>.TXT  ------

exec master..xp_regread
       'HKEY_LOCAL_MACHINE'
      ,'SOFTWARE\Microsoft\MSSQLServer\Setup'
      ,'SQLPath'
            ,@SQLPath   OUTPUT  --example:  C:\SQL60  (missing trailing \ )

select @SQLPath = rtrim(@SQLPath) + '\Log\CertifyR_' + rtrim(@dbname) + '.TXT'



--Prime the TXT file.
select @vc1 = 'echo --sp_certify_removable, db='
            + rtrim(@dbname) + ',  ' + convert(varchar,getdate(),113)
               + '  >' + rtrim(@SQLPath)
exec master..xp_cmdshell @vc1 , 'no_output'


select @vc1 =
    'echo --Sequence,DeviceName,SegmentType,FragmentSizeMb,DeviceFileName'
               + ' >>' + rtrim(@SQLPath)
exec master..xp_cmdshell @vc1 , 'no_output'



--Loop thru temp #table rows.
declare cursor1_tb1
   insensitive
   cursor for
select
       PKSequence
      ,DeviceName
      ,SegmentType
      ,FragmentSizeMb
      ,DeviceFileName
   from
       #tb1_ForDBInstall
   order by
       PKSequence


open cursor1_tb1

select @countloop1 = 0


-----------------------
while (     1=1
      and   @@error = 0
      )
   begin

   fetch     next
      from   cursor1_tb1
      into
             @cu1PKSequence
            ,@cu1DeviceName
            ,@cu1SegmentType
            ,@cu1FragmentSizeMb
            ,@cu1DeviceFileName

   if (@@fetch_status <> 0)
      begin
      deallocate cursor1_tb1
      break
      end

   select @countloop1 = @countloop1 + 1


   select @vc1 = 'echo '

               + convert(varchar ,@cu1PKSequence)
         + ',' +                  @cu1DeviceName
         + ',' +                  @cu1SegmentType
         + ',' + convert(varchar ,@cu1FragmentSizeMb)
         + ',' +                  @cu1DeviceFileName

               + ' >>' + rtrim(@SQLPath)


   exec master..xp_cmdshell @vc1 ,'no_output'

   end --loop 1



select @vc1 = convert(varchar,@countloop1)
            + ' noncomment records were written to LOG\CertifyR*.TXT file.'

raiserror(15310,-1,-1 ,@countloop1)

-- - - -

select @vc1 = 'echo --' + @vc1
               + ' >>' + rtrim(@SQLPath)
exec master..xp_cmdshell @vc1 , 'no_output'


if object_id('tempdb..#tb1_ForDBInstall') is null
           drop table #tb1_ForDBInstall


raiserror('' ,0,1)
exec sp_dboption @dbname,'offline','true'

return(0)
go



select count(*) ,'= the number of system stored procs in master..sysobjects'
	from sysobjects where type='P ' and name like 'sp[_]%'

go

---------------------------------------------------------------
raiserror('Now done creating the general system stored procs.  Start with miscellaneous tasks like xp_ and grants.',0,1)
---------------------------------------------------------------
go

/*
** Drop extended procs. if they already exist now that sp_dropextendedproc
** has been created.
*/

print ''
print 'Dropping any existing extended stored procedures.'
go

if exists (select * from sysobjects
		where sysstat & 0xf = 4
			and name = 'xp_addattach')
	exec sp_dropextendedproc 'xp_addattach'

if exists (select * from sysobjects
		where sysstat & 0xf = 4
			and name = 'xp_addmsgline')
	exec sp_dropextendedproc 'xp_addmsgline'

if exists (select * from sysobjects
		where sysstat & 0xf = 4
			and name = 'xp_cmdshell')
	exec sp_dropextendedproc 'xp_cmdshell'

if exists (select * from sysobjects
		where sysstat & 0xf = 4
			and name = 'sp_cursor')
	exec sp_dropextendedproc 'sp_cursor'

if exists (select * from sysobjects
		where sysstat & 0xf = 4
			and name = 'sp_cursorclose')
	exec sp_dropextendedproc 'sp_cursorclose'

if exists (select * from sysobjects
		where sysstat & 0xf = 4
			and name = 'sp_cursorfetch')
	exec sp_dropextendedproc 'sp_cursorfetch'

if exists (select * from sysobjects
		where sysstat & 0xf = 4
			and name = 'sp_cursoropen')
	exec sp_dropextendedproc 'sp_cursoropen'

if exists (select * from sysobjects
		where sysstat & 0xf = 4
			and name = 'sp_cursoroption')
	exec sp_dropextendedproc 'sp_cursoroption'

if exists (select * from sysobjects
		where sysstat & 0xf = 4
			and name = 'xp_deletemail')
	exec sp_dropextendedproc 'xp_deletemail'

if exists (select * from sysobjects
		where sysstat & 0xf = 4
			and name = 'xp_enumgroups')
	exec sp_dropextendedproc 'xp_enumgroups'

if exists (select * from sysobjects
		where sysstat & 0xf = 4
			and name = 'xp_findnextmsg')
	exec sp_dropextendedproc 'xp_findnextmsg'

if exists (select * from sysobjects
		where sysstat & 0xf = 4
			and name = 'xp_grantlogin')
	exec sp_dropextendedproc 'xp_grantlogin'

if exists (select * from sysobjects
		where sysstat & 0xf = 4
			and name = 'xp_logevent')
	exec sp_dropextendedproc 'xp_logevent'

if exists (select * from sysobjects
		where sysstat & 0xf = 4
			and name = 'xp_loginconfig')
	exec sp_dropextendedproc 'xp_loginconfig'

if exists (select * from sysobjects
		where sysstat & 0xf = 4
			and name = 'xp_logininfo')
	exec sp_dropextendedproc 'xp_logininfo'

if exists (select * from sysobjects
		where sysstat & 0xf = 4
			and name = 'xp_loginmail')
	exec sp_dropextendedproc 'xp_loginmail'

if exists (select * from sysobjects
		where sysstat & 0xf = 4
			and name = 'xp_logoffmail')
	exec sp_dropextendedproc 'xp_logoffmail'

if exists (select * from sysobjects
		where sysstat & 0xf = 4
			and name = 'xp_mailproclist')
	exec sp_dropextendedproc 'xp_mailproclist'

if exists (select * from sysobjects
		where sysstat & 0xf = 4
			and name = 'xp_msver')
	exec sp_dropextendedproc 'xp_msver'

if exists (select * from sysobjects
		where sysstat & 0xf = 4
			and name = 'xp_prepmsg')
	exec sp_dropextendedproc 'xp_prepmsg'

if exists (select * from sysobjects
		where sysstat & 0xf = 4
			and name = 'xp_readmail')
	exec sp_dropextendedproc 'xp_readmail'

if exists (select * from sysobjects
		where sysstat & 0xf = 4
			and name = 'xp_revokelogin')
	exec sp_dropextendedproc 'xp_revokelogin'

if exists (select * from sysobjects
		where sysstat & 0xf = 4
			and name = 'xp_sendmail')
	exec sp_dropextendedproc 'xp_sendmail'

if exists (select * from sysobjects
		where sysstat & 0xf = 4
			and name = 'xp_sendmsg')
	exec sp_dropextendedproc 'xp_sendmsg'

if exists (select * from sysobjects
		where sysstat & 0xf = 4
			and name = 'xp_sprintf')
	exec sp_dropextendedproc 'xp_sprintf'

if exists (select * from sysobjects
		where sysstat & 0xf = 4
			and name = 'xp_sscanf')
	exec sp_dropextendedproc 'xp_sscanf'

if exists (select * from sysobjects
		where sysstat & 0xf = 4
			and name = 'xp_startmail')
	exec sp_dropextendedproc 'xp_startmail'

if exists (select * from sysobjects
		where sysstat & 0xf = 4
			and name = 'xp_stopmail')
	exec sp_dropextendedproc 'xp_stopmail'
go

if exists (select * from sysobjects
		where sysstat & 0xf = 4
			and name = 'xp_unc_to_drive')
	exec sp_dropextendedproc 'xp_unc_to_drive'

if exists (select * from sysobjects
		where sysstat & 0xf = 4
			and name = 'sp_bindsession')
	exec sp_dropextendedproc 'sp_bindsession'

if exists (select * from sysobjects
		where sysstat & 0xf = 4
			and name = 'sp_getbindtoken')
	exec sp_dropextendedproc 'sp_getbindtoken'

if exists (select * from sysobjects
		where sysstat & 0xf = 4
			and name = 'sp_sdidebug')
	exec sp_dropextendedproc 'sp_sdidebug'
go

-- Add extended stored procedures.
print ''
print 'Adding extended stored procedures.'
go

sp_addextendedproc 'xp_cmdshell','xpsql60.dll'
go

sp_addextendedproc 'xp_logevent','xpsql60.dll'
go

sp_addextendedproc 'xp_sprintf','xpsql60.dll'
go

sp_addextendedproc 'xp_sscanf','xpsql60.dll'
go

sp_addextendedproc 'xp_unc_to_drive','xpsql60.dll'
go

sp_addextendedproc 'xp_msver','xpsql60.dll'
go

sp_addextendedproc 'xp_enumgroups','xplog60.dll'
go

-- Add mail enabling extended procedures
--
sp_addextendedproc 'xp_startmail', 'sqlmap60.dll'
go
sp_addextendedproc 'xp_stopmail', 'sqlmap60.dll'
go
sp_addextendedproc 'xp_sendmail', 'sqlmap60.dll'
go
sp_addextendedproc 'xp_deletemail', 'sqlmap60.dll'
go
sp_addextendedproc 'xp_findnextmsg', 'sqlmap60.dll'
go
sp_addextendedproc 'xp_readmail', 'sqlmap60.dll'
go

--Add extended stored procedures for NT integrated login security.
--
sp_addextendedproc 'xp_loginconfig', 'xplog60.dll'
go
sp_addextendedproc 'xp_logininfo', 'xplog60.dll'
go
sp_addextendedproc 'xp_revokelogin', 'xplog60.dll'
go
sp_addextendedproc 'xp_grantlogin', 'xplog60.dll'
go

-- Add extended stored procedures for cursor support.
--
sp_addextendedproc  'sp_cursor', '(server internal)'
go
sp_addextendedproc  'sp_cursorclose', '(server internal)'
go
sp_addextendedproc  'sp_cursorfetch', '(server internal)'
go
sp_addextendedproc  'sp_cursoropen', '(server internal)'
go
sp_addextendedproc  'sp_cursoroption', '(server internal)'
go

-- Add extended stored procedures for bound session support.
--
sp_addextendedproc  'sp_bindsession', '(server internal)'
go
sp_addextendedproc  'sp_getbindtoken', '(server internal)'
go

-- This xproc will be used for SQL debugging and will be invoked either via RPC or SQL language.
--
exec sp_addextendedproc 'sp_sdidebug', '(server internal)'
go


print ''
raiserror(15339,-1,-1,'MS_sqlctrs_users')
go
create procedure MS_sqlctrs_users
as
select syslogins.name+' - '+convert(varchar(30),
		sysprocesses.spid), memusage 'Memory (2K Pages)',
		cpu 'CPU time', physical_io,
		count(syslocks.spid) 'Locks held',sysprocesses.spid
from sysprocesses, syslocks, syslogins where sysprocesses.spid *= syslocks.spid and sysprocesses.suid=syslogins.suid
group by syslogins.name,sysprocesses.spid,memusage,cpu,physical_io
go

/*
**  Add the login and user named 'probe'.  These are used for the
**  2-phased commit service.
*/
if not exists (select * from syslogins where name = 'probe')
	exec sp_addlogin 'probe'
go

if not exists (select * from sysusers where name = 'probe')
	exec sp_adduser 'probe'
go

print ''
print 'Dropping keys'
print ''
go

/*
** Drop 'old-style' common keys.
*/
if exists (select * from syskeys where type = 3 and id = object_id('syscolumns')
		and depid = object_id('syscomments'))
	execute sp_dropkey common, syscolumns, syscomments
go
if exists (select * from syskeys where type = 3 and id = object_id('syscolumns')
		and depid = object_id('syskeys'))
	execute sp_dropkey common, syscolumns, syskeys
go
if exists (select * from syskeys where type = 3 and id = object_id('syscolumns')
		and depid = object_id('sysobjects'))
	execute sp_dropkey common, syscolumns, sysobjects
go
if exists (select * from syskeys where type = 3 and id = object_id('syscolumns')
		and depid = object_id('systypes'))
	execute sp_dropkey common, syscolumns, systypes
go
if exists (select * from syskeys where type = 3 and id = object_id('syscomments')
		and depid = object_id('sysobjects'))
	execute sp_dropkey common, syscomments, sysobjects
go
if exists (select * from syskeys where type = 3 and id = object_id('sysdepends')
		and depid = object_id('sysobjects'))
	execute sp_dropkey common, sysdepends, sysobjects
go
if exists (select * from syskeys where type = 3 and id = object_id('sysindexes')
		and depid = object_id('sysobjects'))
	execute sp_dropkey common, sysindexes, sysobjects
go
if exists (select * from syskeys where type = 3 and id = object_id('sysindexes')
		and depid = object_id('syssegments'))
	execute sp_dropkey common, sysindexes, syssegments
go
if exists (select * from syskeys where type = 3 and id = object_id('syskeys')
		and depid = object_id('sysobjects'))
	execute sp_dropkey common, syskeys, sysobjects
go
if exists (select * from syskeys where type = 3 and id = object_id('sysobjects')
		and depid = object_id('sysprocedures'))
	execute sp_dropkey common, sysobjects, sysprocedures
go
if exists (select * from syskeys where type = 3 and id = object_id('sysobjects')
		and depid = object_id('sysprotects'))
	execute sp_dropkey common, sysobjects, sysprotects
go
if exists (select * from syskeys where type = 3 and id = object_id('sysobjects')
		and depid = object_id('sysusers'))
	execute sp_dropkey common, sysobjects, sysusers
go
if exists (select * from syskeys where type = 3 and id = object_id('sysprotects')
		and depid = object_id('sysusers'))
	execute sp_dropkey common, sysprotects, sysusers
go
if exists (select * from syskeys where type = 3 and id = object_id('systypes')
		and depid = object_id('sysusers'))
	execute sp_dropkey common, systypes, sysusers
go
if exists (select * from syskeys where type = 3 and id = object_id('syscurconfigs')
		and depid = object_id('sysconfigures'))
	execute sp_dropkey common, syscurconfigs, sysconfigures
go
if exists (select * from syskeys where type = 3 and id = object_id('sysdatabases')
		and depid = object_id('syslocks'))
	execute sp_dropkey common, sysdatabases, syslocks
go
if exists (select * from syskeys where type = 3 and id = object_id('sysdatabases')
		and depid = object_id('syslogins'))
	execute sp_dropkey common, sysdatabases, syslogins
go
if exists (select * from syskeys where type = 3 and id = object_id('sysdatabases')
		and depid = object_id('sysprocesses'))
  	execute sp_dropkey common, sysdatabases, sysprocesses
go
if exists (select * from syskeys where type = 3 and id = object_id('sysdatabases')
		and depid = object_id('sysusages'))
	execute sp_dropkey common, sysdatabases, sysusages
go
if exists (select * from syskeys where type = 3 and id = object_id('syslocks')
		and depid = object_id('sysprocesses'))
	execute sp_dropkey common, syslocks, sysprocesses
go
if exists (select * from syskeys where type = 3 and id = object_id('syslogins')
		and depid = object_id('sysprocesses'))
	execute sp_dropkey common, syslogins, sysprocesses
go
if exists (select * from syskeys where type = 3 and id = object_id('sysservers')
		and depid = object_id('sysremotelogins'))
	execute sp_dropkey common, sysservers, sysremotelogins
go
if exists (select * from syskeys where type = 3 and id = object_id('sysremotelogins')
		and depid = object_id('sysusers'))
	execute sp_dropkey common, sysremotelogins, sysusers
go

/*
** Drop 'old-style' foreign keys.
*/
if exists (select * from syskeys where type = 2 and id = object_id('sysalternates')
		and depid = object_id('sysusers'))
	execute sp_dropkey 'foreign', sysalternates, sysusers
go
if exists (select * from syskeys where type = 2 and id = object_id('syskeys')
		and depid = object_id('sysobjects'))
	execute sp_dropkey 'foreign', syskeys, sysobjects
go

/*
** Drop 'old-style' primary keys.
*/
if exists (select * from syskeys where type = 1 and id = object_id('sysalternates'))
	execute sp_dropkey 'primary', sysalternates
go
if exists (select * from syskeys where type = 1 and id = object_id('syscolumns'))
	execute sp_dropkey 'primary', syscolumns
go
if exists (select * from syskeys where type = 1 and id = object_id('syscomments'))
	execute sp_dropkey 'primary', syscomments
go
if exists (select * from syskeys where type = 1 and id = object_id('sysconfigures'))
	execute sp_dropkey 'primary', sysconfigures
go
if exists (select * from syskeys where type = 1 and id = object_id('sysdatabases'))
	execute sp_dropkey 'primary', sysdatabases
go
if exists (select * from syskeys where type = 1 and id = object_id('sysdepends'))
	execute sp_dropkey 'primary', sysdepends
go
if exists (select * from syskeys where type = 1 and id = object_id('sysdevices'))
	execute sp_dropkey 'primary', sysdevices
go
if exists (select * from syskeys where type = 1 and id = object_id('sysindexes'))
	execute sp_dropkey 'primary', sysindexes
go
if exists (select * from syskeys where type = 1 and id = object_id('syskeys'))
	execute sp_dropkey 'primary', syskeys
go
if exists (select * from syskeys where type = 1 and id = object_id('syslogins'))
	execute sp_dropkey 'primary', syslogins
go
if exists (select * from syskeys where type = 1 and id = object_id('sysmessages'))
	execute sp_dropkey 'primary', sysmessages
go
if exists (select * from syskeys where type = 1 and id = object_id('sysobjects'))
	execute sp_dropkey 'primary', sysobjects
go
if exists (select * from syskeys where type = 1 and id = object_id('sysprocedures'))
	execute sp_dropkey 'primary', sysprocedures
go
if exists (select * from syskeys where type = 1 and id = object_id('sysremotelogins'))
	execute sp_dropkey 'primary', sysremotelogins
go
if exists (select * from syskeys where type = 1 and id = object_id('sysprotects'))
	execute sp_dropkey 'primary', sysprotects
go
if exists (select * from syskeys where type = 1 and id = object_id('syssegments'))
	execute sp_dropkey 'primary', syssegments
go
if exists (select * from syskeys where type = 1 and id = object_id('sysservers'))
	execute sp_dropkey 'primary', sysservers
go
if exists (select * from syskeys where type = 1 and id = object_id('systypes'))
	execute sp_dropkey 'primary', systypes
go
if exists (select * from syskeys where type = 1 and id = object_id('sysusers'))
	execute sp_dropkey 'primary', sysusers
go
if exists (select * from syskeys where type = 1 and id = object_id('sysusages'))
	execute sp_dropkey 'primary', sysusages
go

print ''
print 'Granting privileges on system tables.'
go

grant select on sysalternates to public
grant select on syscharsets to public
grant select on syscolumns to public
grant select on syscomments to public
grant select on sysconfigures to public
grant select on syscurconfigs to public
grant select on sysdatabases to public
grant select on sysdepends to public
grant select on sysdevices to public
grant select on sysindexes to public
grant select on syskeys to public
grant select on syslanguages to public
grant select on syslocks to public
go
grant select on syslogins to public
revoke select (password) on syslogins from public
go
grant select on syslogs to public
grant select on sysmessages to public
grant select on sysobjects to public
grant select on sysprocedures to public
grant select on sysprocesses to public
grant select on sysprotects to public
grant select on sysremotelogins to public
grant select on syssegments to public
grant select on sysservers to public
grant select on systypes to public
grant select on sysusages to public
grant select on sysusers to public
go

print ''
print 'Granting privileges on system stored procedures'
go

grant execute on MS_sqlctrs_users to public
go

grant execute on sp_user_counter1   to probe
grant execute on sp_user_counter2   to probe
grant execute on sp_user_counter3   to probe
grant execute on sp_user_counter4   to probe
grant execute on sp_user_counter5   to probe
grant execute on sp_user_counter6   to probe
grant execute on sp_user_counter7   to probe
grant execute on sp_user_counter8   to probe
grant execute on sp_user_counter9   to probe
grant execute on sp_user_counter10  to probe
grant execute on sp_userdefcounters to probe
go

grant execute on sp_a_count_bits_on to public
grant execute on sp_abort_xact to public
grant execute on sp_addalias to public
grant execute on sp_addgroup to public
grant execute on sp_addmessage to public
grant execute on sp_addsegment to public
grant execute on sp_addtype to public
grant execute on sp_adduser to public
grant execute on sp_altermessage to public
grant execute on sp_bindefault to public
grant execute on sp_bindrule to public
grant execute on sp_change_users_login to public
grant execute on sp_changegroup to public
grant execute on sp_checknames to public
grant execute on sp_coalesce_fragments to public
grant execute on sp_commit_xact to public
grant execute on sp_commonkey to public
grant execute on sp_configure to public
grant execute on sp_cursor to public
grant execute on sp_cursorclose to public
grant execute on sp_cursorfetch to public
grant execute on sp_cursoropen to public
grant execute on sp_cursoroption to public
grant execute on sp_dboption to public
grant execute on sp_bindsession to public
grant execute on sp_getbindtoken to public
go
grant execute on sp_defaultdb to public
grant execute on sp_defaultlanguage to public
grant execute on sp_depends to public
grant execute on sp_devoption to public
grant execute on sp_dropalias to public
grant execute on sp_dropgroup to public
grant execute on sp_dropkey to public
grant execute on sp_dropmessage to public
grant execute on sp_dropsegment to public
grant execute on sp_droptype to public
grant execute on sp_dropuser to public
grant execute on sp_extendsegment to public
go
grant execute on sp_fallback_activate_svr       to public
grant execute on sp_fallback_activate_svr_db   to public
grant execute on sp_fallback_deactivate_svr     to public
grant execute on sp_fallback_deactivate_svr_db to public
grant execute on sp_fallback_enroll_svr_db     to public
grant execute on sp_fallback_help               to public
grant execute on sp_fallback_help_db_dev        to public
grant execute on sp_fallback_MS_enroll_db      to public
grant execute on sp_fallback_MS_enroll_dev      to public
grant execute on sp_fallback_MS_enroll_usg      to public
grant execute on sp_fallback_MS_sel_fb_svr      to public
grant execute on sp_fallback_MS_verify_ri       to public
grant execute on sp_fallback_permanent_svr      to public
grant execute on sp_fallback_upd_dev_drive      to public
grant execute on sp_fallback_withdraw_svr_db   to public
go
grant execute on sp_help to public
grant execute on sp_help_revdatabase to public
grant execute on sp_helpconstraint to public
grant execute on sp_helpdb to public
grant execute on sp_helpdevice to public
grant execute on sp_helpextendedproc to public
grant execute on sp_foreignkey to public
grant execute on sp_helpgroup to public
grant execute on sp_helpindex to public
grant execute on sp_helpjoins to public
grant execute on sp_helpkey to public
grant execute on sp_helplanguage to public
grant execute on sp_helplog to public
grant execute on sp_helplogins to public
grant execute on sp_helpremotelogin to public
grant execute on sp_helprotect to public
grant execute on sp_helpsegment to public
grant execute on sp_helpserver to public
grant execute on sp_helpsort to public
grant execute on sp_helptext to public
grant execute on sp_helpuser to public
grant execute on sp_lock to public
grant execute on sp_lock2 to public
grant execute on sp_lockinfo to public
grant execute on sp_logdevice to public
grant execute on sp_lookup to public
grant execute on sp_markreport to public
grant execute on sp_namecrack to public
grant execute on sp_namecrack_qi to public
go
grant execute on sp_objectsegment to public
grant execute on sp_password to public
grant execute on sp_placeobject to public
grant execute on sp_primarykey to public
grant execute on sp_processinfo to public
grant execute on sp_probe_xact to public
grant execute on sp_recompile to public
grant execute on sp_remoteoption to public
grant execute on sp_remove_xact to public
grant execute on sp_rename to public
grant execute on sp_scan_xact to public
grant execute on sp_serveroption to public
grant execute on sp_spaceused to public
grant execute on sp_sqlexec to public
grant execute on sp_start_xact to public
grant execute on sp_stat_xact to public
go

grant execute on sp_blockcnt to probe
grant execute on sp_tableoption to public
grant execute on sp_tempdbspace to probe  --Not designed for public use.
go

grant execute on sp_unbindefault to public
grant execute on sp_unbindrule to public
grant execute on sp_validname to public
grant execute on sp_validlang to public
grant execute on sp_who to public
grant execute on sp_who2 to public
go

print ''
print 'Granting privileges on extended stored procedures'
go

grant execute on xp_regread to probe  --Not documented, so not to public.
go

grant execute on xp_sprintf to public
grant execute on xp_sscanf to public
grant execute on xp_unc_to_drive to public
grant execute on xp_msver to public
go

print ''
print 'Granting privileges on objects in model database.'
go
use model
go
grant select on sysalternates to public
grant select on syscolumns to public
grant select on syscomments to public
grant select on sysconstraints to public
grant select on sysdepends to public
grant select on sysindexes to public
grant select on syskeys to public
grant select on sysobjects to public
grant select on sysprocedures to public
grant select on sysprotects to public
grant select on syssegments to public
grant select on systypes to public
grant select on sysusers to public
grant select on syslogs to public
go

use master
go

/*
** Grant privileges on SQL 6.0 system tables that exist in all databases.
*/

declare @dbname sysname
declare db_name_cursor cursor for select name from sysdatabases
open db_name_cursor
fetch db_name_cursor into @dbname

while @@fetch_status >= 0
	begin

	exec ('use ' + @dbname
		+ ' grant select on sysarticles      to public'
		+ ' grant select on sysconstraints   to public'
		+ ' grant select on syspublications  to public'
		+ ' grant select on sysreferences    to public'
		+ ' grant select on syssubscriptions to public'
	     )

	fetch db_name_cursor into @dbname

	end

deallocate db_name_cursor
go


use master
go

/*
** Set some things now that sp_configure has been created.
*/
go
print ''
print 'Making final database configuration settings.'
go

/********  sdf

raiserror('If this is a new install, shrink size of model db (helps floppy RemovableMedia).',0,1)
--go

********/

declare @i varchar(255)

exec master..xp_regread 'HKEY_LOCAL_MACHINE',
	 'SOFTWARE\Microsoft\MSSQLServer\Setup', 'SetupStatus',
	  @i out

print ''

if isnull(lower(@i),'null') not in ('upgrade','installed')	-- That key will only exist during an upgrade.
	begin -- NewInstall
		select 'Will leave database size config=111 alone at',value from sysconfigures where config=111

--		print 'Setting default database size to 1 Meg.'
--		exec sp_configure 'database size',1
	end
else
		begin
			select 'Re-Initializing free buffer setting, from....',value from sysconfigures where config=1515
			update sysconfigures set value =
				(select value/20 from sysconfigures
					where config = 104)
				where config = 1515
		end
go
print 'Forcing config for remote access to 1'
exec sp_configure 'remote access',1
go

-- Unnecessary in 6.5  --->  exec sp_check_objects 'general'

exec sp_MS_upd_sysobj_category 2  --Now do catalog updates.

go

sp_configure 'allow updates',0
go
sp_dboption 'master','trunc. log on chkpt.','true'
go
reconfigure with override
go

print ''
-- print 'Checking objects created by ProcSyst.SQL ....'
go

declare @vdt varchar(99)
select  @vdt = convert(varchar,getdate(),113)
raiserror('Finishing Install\ProcSyst.SQL at  %s',0,1,@vdt)
go

dump transaction master with no_log
go
checkpoint
go
-- - -----

