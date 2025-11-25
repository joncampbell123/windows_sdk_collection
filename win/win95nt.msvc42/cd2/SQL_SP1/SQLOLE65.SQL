
/**********************************************************************
 *	Microsoft SQL Server SQL-DMO scripts, version 6.5.
 *	Date created:  3/17/96.
 *	Copyright Microsoft Corporation, 1992-1996
 **********************************************************************
 *	This script must be installed for the SQL-DMO (SQLOLE) objects and
 *	the SQL Enterprise Manager to run against a Microsoft SQL Server.
 *	This script is backwards-compatible with version 6.0; following
 *	installation, either the 6.0 or 6.5 versions of SQL-DMO and SQL
 *	Enterprise Manager will run against the server.
 **********************************************************************/














/* Preprocessor directives, will be blank space in output .sql file. */


/* All 6.x versions should be downward compatible, except for explicit switching on the uplevel #define. */












/* For fetching from cursor */





















































/* status values for these. */







/* bitmask values for same; power(2, DRI_*). */


















/* DRI-generated index masks, to apply to sysindexes.status */






/* sysobjects.category bit that indicates this is an MS-internal object. */


/* sysobjects.category bit for an sp_ that indicates it's a startup proc, or an xp that should ImpersonateClient. */



/* BIT_CLUSTERED indicates the key is clustered. */
/* EXCLUDE REPLICATION value in sysconstraints.status, and system-generated name. */





/* sysobjects.sysstat bits (lower 4) that mask off the object type. */


/* bit for DEFAULTS which are really DRI-created. */


/* bits for columns - don't conflict with bit_sysgenname for DRIDefaults. */






/* sysdatabases.category bits */



/* sysobjects.category bits (from ntdbms\object.h) */

















/* The parser can't '|' 0x-prefixed types as it thinks they're binary */







/**** daVinci additions ****/

/* sp_MStablekeys @flags bitmask param values. */


/* sp_MShelpindexes @flags bitmask param values. */


/* sp_MStablechecks @flags bitmask param values. */






/* From SQLOLE.H, MUST BE SYNC'd!!! */






































/* From perm.h */















/* Localized error-string files (PRE95_ERRMSG_*) for PRE95 servers. */
/* Set the include path to determine which lang-specific version is picked up. */

/* USA-localized strings for 4.21 localization of sqlole.cxx */
/* Should match the localized string in sysmessages (see sqlole.cxx for message number). */














use master
go

/************* DUMP THE TRANSACTION LOG **************************************/
/* Comment this out if you don't want your log dumped.  If you rerun this    */
/* script periodically, you will run out of transaction log space.           */
print ''
print 'Dumping transaction log...'
print ''
go
dump tran master with no_log
go
/************* END DUMP THE TRANSACTION LOG **********************************/

/********************* Delete existing objects *********************************/
print ''
print 'Deleting existing objects...'
print ''
go





if exists (select * from master..sysobjects where sysstat & 0x0f = 4 and name = 'sp_MShelpcolumns')
	drop procedure sp_MShelpcolumns
go
if exists (select * from master..sysobjects where sysstat & 0x0f = 4 and name = 'sp_MShelpindex')
	drop procedure sp_MShelpindex
go
if exists (select * from master..sysobjects where sysstat & 0x0f = 4 and name = 'sp_MShelptype')
	drop procedure sp_MShelptype
go
if exists (select * from master..sysobjects where sysstat & 0x0f = 4 and name = 'sp_MSdependencies')
	drop procedure sp_MSdependencies
go
if exists (select * from master..sysobjects where sysstat & 0x0f = 4 and name = 'sp_MStablespace')
	drop procedure sp_MStablespace
go
if exists (select * from master..sysobjects where sysstat & 0x0f = 4 and name = 'sp_MSindexspace')
	drop procedure sp_MSindexspace
go
if exists (select * from master..sysobjects where sysstat & 0x0f = 4 and name = 'sp_MSuniquename')
	drop procedure sp_MSuniquename
go
if exists (select * from master..sysobjects where sysstat & 0x0f = 4 and name = 'sp_MSkilldb')
	drop procedure sp_MSkilldb
go
if exists (select * from master..sysobjects where sysstat & 0x0f = 4 and name = 'sp_MSdbuserprofile')
	drop procedure sp_MSdbuserprofile
go
if exists (select * from master..sysobjects where sysstat & 0x0f = 4 and name = 'sp_MSobjectprivs')
	drop procedure sp_MSobjectprivs
go
if exists (select * from master..sysobjects where sysstat & 0x0f = 4 and name = 'sp_MSloginmappings')
	drop procedure sp_MSloginmappings
go


if exists (select * from master..sysobjects where sysstat & 0x0f = 4 and name = 'sp_MSgetalertinfo')
	drop procedure sp_MSgetalertinfo
go
if exists (select * from master..sysobjects where sysstat & 0x0f = 4 and name = 'sp_MSsetalertinfo')
	drop procedure sp_MSsetalertinfo
go
if exists (select * from master..sysobjects where sysstat & 0x0f = 4 and name = 'sp_MSgetexecinfo')
	drop procedure sp_MSgetexecinfo
go
if exists (select * from master..sysobjects where sysstat & 0x0f = 4 and name = 'sp_MSsetexecinfo')
	drop procedure sp_MSsetexecinfo
go
if exists (select * from master..sysobjects where sysstat & 0x0f = 4 and name = 'sp_MStablekeys')
	drop procedure sp_MStablekeys
go
if exists (select * from master..sysobjects where sysstat & 0x0f = 4 and name = 'sp_MStablechecks')
	drop procedure sp_MStablechecks
go
if exists (select * from master..sysobjects where sysstat & 0x0f = 4 and name = 'sp_MStablerefs')
	drop procedure sp_MStablerefs
go
if exists (select * from master..sysobjects where sysstat & 0x0f = 4 and name = 'sp_MSsettopology')
	drop procedure sp_MSsettopology
go
if exists (select * from master..sysobjects where sysstat & 0x0f = 4 and name = 'sp_MSfilterclause')
	drop procedure sp_MSfilterclause
go
if exists (select * from master..sysobjects where sysstat & 0x0f = 4 and name = 'sp_MSmatchkey')
	drop procedure sp_MSmatchkey
go
if exists (select * from master..sysobjects where sysstat & 0x0f = 4 and name = 'sp_MSsubscriptions')
	drop procedure sp_MSsubscriptions
go
if exists (select * from master..sysobjects where sysstat & 0x0f = 4 and name = 'sp_MSforeachdb')
	drop procedure sp_MSforeachdb
go
if exists (select * from master..sysobjects where sysstat & 0x0f = 4 and name = 'sp_MSforeachtable')
	drop procedure sp_MSforeachtable
go
if exists (select * from master..sysobjects where sysstat & 0x0f = 4 and name = 'sp_MSforeach_worker')
	drop procedure sp_MSforeach_worker
go
if exists (select * from master..sysobjects where sysstat & 0x0f = 4 and name = 'sp_MSenumsubsystems')
	drop procedure sp_MSenumsubsystems
go


if exists (select * from master..sysobjects where sysstat & 0x0f = 4 and name = 'sp_MSSQLOLE_version')
	drop procedure sp_MSSQLOLE_version
go
if exists (select * from master..sysobjects where sysstat & 0x0f = 4 and name = 'sp_MSSQLOLE65_version')
	drop procedure sp_MSSQLOLE65_version
go

/********************* Create new objects *********************************/


/* New validation added for 6.5 */
if (@@microsoftversion >= 0x000632c0)
	exec sp_MS_upd_sysobj_category 1


/*******************************************************************************/
print ''
print 'Creating sp_MShelpcolumns'
print ''
go
create procedure sp_MShelpcolumns
@tablename varchar(92), @flags int = 0, @orderby varchar(10) = null, @flags2 int = 0
as
	if @flags is null
		select @flags = 0
	if (@tablename = '?')
	begin
		print ''
		print 'Usage:  sp_MShelpcolumns @tablename, @flags int = 0'
		print ' where @flags is a bitmask of:'
		print ' 0x0200		= No DRI (ignore Checks, Primary/Foreign/Unique Keys, etc.)'
		print ' 0x0400		= UDDTs --> Base type'
		print ' 0x0800		= 42Syntax (convert numeric/decimal, no identity)'
		print ' 0x80000		= TimestampToBinary (convert timestamp cols to binary(8))'
		print ' 0x40000000	= No Identity attribute'
		return 0
	end

























































































































/**********************************/

	declare @objid int, @category int
	select @objid = id, @category = category from sysobjects where id = object_id(@tablename)
	if (@objid is null)
	begin
		RAISERROR (15001, -1, -1, @tablename)
		return 1
	end

	set nocount on

	create table #sphelpcols
	(
		col_name		varchar(30)			NOT NULL,
		col_id			tinyint				NOT NULL,
		col_type		smallint			NOT NULL,
		col_len			tinyint				NOT NULL,
		col_prec		tinyint				NULL,
		col_scale		tinyint				NULL,
		col_numtype		smallint			NOT NULL,	/* For DaVinci to get sp_help-type filtering of prec/scale */
		col_null		bit					NOT NULL,	/* status & 8 */
		col_identity	bit					NOT NULL,	/* status & 128 */
		col_defname		varchar(61)			NULL,		/* fully-qual'd default name, or NULL */
		col_rulname		varchar(61)			NULL,		/* fully-qual'd rule name, or NULL */
		col_basetype	int					NULL,
		col_flags		int					NULL,
		col_seed		int					NULL,
		col_increment	int 				NULL,
		col_dridefname	varchar(30)			NULL,		/* DRI DEFAULT name */
		col_drideftext	varchar(255)		NULL,		/* DRI DEFAULT text */
		col_dridefid	int					NULL,		/* subselect hackaround */
	)

	/* First load stuff so we can blot off inappropriate info and massage as per @flags */
	insert #sphelpcols
		select c.name, c.colid, c.usertype, c.length, c.prec, c.scale, 
				-- col_numtype for DaVinci:  use sp_help-type prec/scale filtering for @flags2 & 1
			case when (@flags2 & 1 <> 0 and c.type in (38,48,52,55,56,59,60,62,63,106,108,109,110,122)) then 1 else 0 end,
				-- Nullable
			convert(bit, c.status & 0x0008), 
				-- Identity
			case when (@flags & 0x40000000 = 0) then convert(bit, c.status & 0x0080) else 0 end,
				-- Non-DRI Default (make sure it's not a DRI constraint).
			case when (c.cdefault = 0) then null when (d.category & 0x0800 <> 0) then null else user_name(d.uid) + '.' + d.name end,
				-- Non-DRI Rule
			case when (c.domain = 0) then null else user_name(r.uid) + '.' + r.name end,
				-- Physical base datatype
			case when (c.usertype < 100) then c.usertype else bt.usertype end,
				-- Initialize flags to whether it's a length-specifiable type, or a numeric type, or 0.
			case when c.usertype in (1, 2, 3, 4) then 0x0001 when c.usertype in (10, 24) then 0x0002 else 0 end,
				-- Identity seed and increment
			case when (c.status & 0x0080 = 0) then null else ident_seed(@tablename) end,
			case when (c.status & 0x0080 = 0) then null else ident_incr(@tablename) end,
				-- DRI Default name
			case when (@flags & 0x0200 = 0 and d.category & 0x0800 <> 0) then object_name(c.cdefault) else null end,
				-- DRI Default text, if it does not span multiple rows (if it does, SQLOLE will go get them all).
			case when (@flags & 0x0200 = 0 and d.category & 0x0800 <> 0)
					then t.text else null end,
				-- Hackaround for a subselect bug
			c.cdefault
		from syscolumns c, sysobjects d, sysobjects r, systypes bt, syscomments t
		where c.id = object_id(@tablename)
				-- NonDRI Default and Rule filters
			and d.id =* c.cdefault and r.id =* c.domain
				-- Physical base type
			and bt.type =* c.type and bt.usertype < 99 and bt.usertype not in (18, 80)
				-- DRIDefault text, if it's only one row.
			and t.id =* c.cdefault and t.colid = 1

	/* Could not do this on the fly as the subselect is not properly processed. */
	/* An "and not exists" fails on 6.0 due to parser bug so this is the only way to make both work. */
	if (@flags & 0x0200 = 0)
		update #sphelpcols set col_drideftext = NULL
			where col_dridefid is not NULL 
			and exists (select * from syscomments where id = col_dridefid and colid = 2)

	/* Convert any timestamp column to binary(8) if they asked. */
	if (@flags & 0x80000 != 0)
		update #sphelpcols set col_type = 3, col_len = 8, col_flags = col_flags | 0x0001 where col_type = 80

	/* Convert from the dblib-specific 'nullable' type to something people and the server recognize. */
	update #sphelpcols set col_type = case col_type 
			when 15 then 12	/* datetime --> datetimn */
			when 26 then 24	/* decimal --> decimaln */
			when 14 then 8	/* float --> floatn */
			when 13 then 7	/* int --> intn*/
			when 17 then 11	/* money --> moneyn*/
			when 25 then 10	/* numeric --> numericn */
			else col_type end,
		col_basetype = case col_basetype 
			when 15 then 12	/* datetime --> datetimn */
			when 26 then 24	/* decimal --> decimaln */
			when 14 then 8	/* float --> floatn */
			when 13 then 7	/* int --> intn*/
			when 17 then 11	/* money --> moneyn*/
			when 25 then 10	/* numeric --> numericn */
			else col_basetype end

	/* Now see what our flags are, if anything.  Ignore the physical base type here -- these */
	/* flags are just for scripting, which doesn't use that. */
	if (@flags is not null and @flags != 0)
	begin
		if (@flags & 0x0400 != 0)
		begin
			/* Track from usertype --> b.<base>type --> u.usertype in systypes */
			update #sphelpcols set col_type = u.usertype,
				-- ReInitialize flags to whether it's a length-specifiable type, or a numeric type, or 0.
				col_flags = case when u.usertype in (1, 2, 3, 4) then 0x0001 when u.usertype in (10, 24) then 0x0002 else 0 end
			 from #sphelpcols c, systypes b, systypes u
			where c.col_type > 100 and b.usertype = c.col_type and b.type = u.type and u.usertype < 100 and u.usertype not in (18, 80)
		end
		/* Modifided to allow Identity if NoDRI.  This is like a new datatype; OK unless 42Syntax is specified. */
		if (@flags & 0x0800 != 0)
		begin
			update #sphelpcols				/* No IDENTITY */
				set col_identity = 0
			update #sphelpcols				/* Convert numeric and decimal with prec < 8 to real */
				set col_type = 23 where col_type in (10, 24) and col_prec < 8
			update #sphelpcols				/* Convert numeric and decimal with prec >= 8 to float */
				set col_type = 8 where col_type in (10, 24)	and col_prec >= 8
		end
	end

	/* Determine if the column is in the primary key */
	if (@flags & 0x0200 = 0 and @category & 0x200 <> 0) begin
		declare @indid int
		select @indid = indid from sysindexes i where i.id = @objid and i.status & 0x0800 <> 0

		/* There can't be more columns in an index than there are in a table, so use the col_id's */
		/* to form a numeric sequence to query the index_col ordinals.  Note that this does NOT */
		/* imply a relationship between the col_id and the name returned for it; in fact they will */
		/* not be the same unless the first x columns are the index columns, in order.  col_id is */
		/* ONLY used to form an unbroken numeric sequence to query index_col; the #sphelpcols */
		/* update by name is a separate step. */
		if (@indid is not null)
			update #sphelpcols set col_flags = col_flags | 0x0004 where col_name in 
				(select index_col(@tablename, @indid, col_id) from #sphelpcols where col_id <= 16)
	end

	/* OK, now put out the data.  @flags2 added for DaVinci; currently only bit 1 (sp_help filtering of prec/scale) is relevant. */
	set nocount off
	if (@orderby is null or @orderby = 'id')
	begin
		select c.col_name, c.col_id, DataType = t.name, c.col_len,
					-- Prec/scale only for numeric/decimal
				col_prec = case when (col_basetype in (10, 24) or (@flags2 & 1 <> 0 and col_numtype & 1 <> 0))
						then c.col_prec else NULL end, 
				col_scale = case when (col_basetype in (10, 24) or (@flags2 & 1 <> 0 and col_numtype & 1 <> 0))
						then c.col_scale else NULL end,
				BaseType = b.name, c.col_defname, c.col_rulname, c.col_null, c.col_identity,
				c.col_flags, c.col_seed, c.col_increment, c.col_dridefname, c.col_drideftext
		from #sphelpcols c, systypes t, systypes b
		where t.usertype = c.col_type and b.usertype = c.col_basetype
		order by c.col_id
	end else begin
		select c.col_name, c.col_id, DataType = t.name, c.col_len,
					-- Prec/scale only for numeric/decimal
				col_prec = case when (col_basetype in (10, 24) or (@flags2 & 1 <> 0 and col_numtype & 1 <> 0))
						then c.col_prec else NULL end, 
				col_scale = case when (col_basetype in (10, 24) or (@flags2 & 1 <> 0 and col_numtype & 1 <> 0))
						then c.col_scale else NULL end,
				BaseType = b.name, c.col_defname, c.col_rulname, c.col_null, c.col_identity,
				c.col_flags, c.col_seed, c.col_increment, c.col_dridefname, c.col_drideftext
		from #sphelpcols c, systypes t, systypes b
		where t.usertype = c.col_type and b.usertype = c.col_basetype
		order by c.col_name
	end

go
/* End sp_MShelpcolumns */

/*******************************************************************************/
print ''
print 'Creating sp_MShelpindex'
print ''
go
create procedure sp_MShelpindex
@tablename varchar(92), @indexname varchar(30) = null, @flags int = null
as
	/* @flags is for daVinci */
	if (@flags is null)
		select @flags = 0

	select i.name, i.status, i.indid, i.OrigFillFactor,



		IndCol1 = index_col(@tablename, i.indid, 1), 
		IndCol2 = index_col(@tablename, i.indid, 2),
		IndCol3 = index_col(@tablename, i.indid, 3), 
		IndCol4 = index_col(@tablename, i.indid, 4),
		IndCol5 = index_col(@tablename, i.indid, 5), 
		IndCol6 = index_col(@tablename, i.indid, 6),
		IndCol7 = index_col(@tablename, i.indid, 7), 
		IndCol8 = index_col(@tablename, i.indid, 8),
		IndCol9 = index_col(@tablename, i.indid, 9), 
		IndCol10 = index_col(@tablename, i.indid, 10),
		IndCol11 = index_col(@tablename, i.indid, 11), 
		IndCol12 = index_col(@tablename, i.indid, 12),
		IndCol13 = index_col(@tablename, i.indid, 13), 
		IndCol14 = index_col(@tablename, i.indid, 14),
		IndCol15 = index_col(@tablename, i.indid, 15), 
		IndCol16 = index_col(@tablename, i.indid, 16)

		, SegName = case (@flags & 1) when 1 then convert(varchar(32), s.name) 
			else convert(varchar(32), null) end

	from sysindexes i

	, syssegments s

	where id = object_id(@tablename) and i.indid > 0 and i.indid < 255
		and (@indexname is null or i.name = @indexname)

		and s.segment = i.segment

	order by i.name
go
/* End sp_MShelpindex */

/************* DUMP THE TRANSACTION LOG **************************************/
/* Comment this out if you don't want your log dumped.  If you rerun this    */
/* script periodically, you will run out of transaction log space.           */
print ''
print 'Dumping transaction log...'
print ''
go
dump tran master with no_log
go
/************* END DUMP THE TRANSACTION LOG **********************************/

/*******************************************************************************/
print ''
print 'Creating sp_MShelptype'
print ''
go
create procedure sp_MShelptype
@typename varchar(92) = null, @flags varchar(10) = null
as
	if (@typename = '?')
	begin
		print ''
		print 'Usage:  sp_MShelptype @typeename = null, @flags varchar(10) = null'
		print ' where @flags is either:'
		print ' sdt		= look in system datatypes'
		print ' uddt  	= look in user defined datatypes'
		print ' null	= look wherever its found'
		print ''
		return 0
	end

	/* Catch typos... */
	if (@flags is not null and @flags not in ('sdt', 'uddt'))
		select @flags = null

	

	/* Find out what type we're gonna be looking in, if they gave us a name. */
	if (@typename is not null)
	begin
		declare @usertype int
		select @usertype = usertype from systypes where name = @typename
		if (@usertype is not null)
		begin
			if (@usertype < 100)
			begin
				if (@flags is null)
					select @flags = 'sdt'
				if (@flags != 'sdt')
					select @usertype = null
			end else begin
				if (@flags is null)
					select @flags = 'uddt'
				if (@flags != 'uddt')
					select @usertype = null
			end
		end
		if (@usertype is null)
		begin
			
			RAISERROR (15001, -1, -1, @typename)
			return 1
		end
	end

	/* Now go get the info, depending on the type they gave us. */
	if (@flags is null or @flags = 'sdt')
	begin
		/* Exclude the 'xxxxn' dblib-specific nullable types, and hardcode a check for variable length and numeric usertypes. */
		select 	SystemDatatypeName = t.name,
				ifvarlen_max = y.length,

					-- timestamp allows nulls even though the system tables say it doesn't.
				allownulls = case t.usertype when 80 then 1 else t.allownulls end,
				isnumeric = case when t.usertype in (24, 10) then 1 else 0 end,
				allowidentity = case when t.usertype in (24,7,10,6,5) then 1 else 0 end




			from systypes t, systypes y
			where t.usertype < 100 and t.usertype not in (15,26,14,13,17,25) and (@typename is null or t.name like @typename)
			and y.usertype =* t.usertype and y.usertype in (1,2,3,4)
			order by t.name
	end

	/* Need a temp table so we can ownerqualify nonNULL rules/defaults. */
	create table #sphelptype (
		dt_usertype		int				NULL,
		dt_basetype		int				NULL,
		dt_rul 			int				NULL,
		dt_def 			int				NULL,
		dt_rulname 		varchar(61)		NULL,
		dt_defname		varchar(61)		NULL,
		dt_flags		int				NULL
	)

	if (@flags is null or @flags = 'uddt')
	begin
		set nocount on
		insert #sphelptype (dt_usertype, dt_basetype, dt_rul, dt_def, dt_flags)
			select t.usertype,
			(select distinct b.usertype from systypes b where b.type = t.type and b.usertype < 100 and b.usertype not in (18, 80)),
			t.domain, t.tdefault, 0
			from systypes t
			where t.usertype > 99 and (@typename is null or t.name like @typename)

		/* Make a nice, presentable qualified rule/default name for those which are non-null */
		update #sphelptype set dt_defname = user_name(d.uid) + '.' + d.name
				from #sphelptype c, sysobjects d where c.dt_def is not null and d.id = c.dt_def
		update #sphelptype set dt_rulname = user_name(r.uid) + '.' + r.name
				from #sphelptype c, sysobjects r where c.dt_rul is not null and r.id = c.dt_rul

		/* For scripting, set the dt_flags -- these apply to the BASE datatype. */
		update #sphelptype set dt_flags = dt_flags | 0x0001 where dt_basetype in (1,2,3,4)
		update #sphelptype set dt_flags = dt_flags | 0x0002 where dt_basetype in (10, 24)

		set nocount off
		select distinct UserDatatypeName = t.name,
				owner = user_name(t.uid),
				basetypename = (select distinct b.name from systypes b where b.usertype = s.dt_basetype),
				defaultname = dt_defname,
				rulename = dt_rulname,
				tid = t.usertype,

				length = case when s.dt_basetype in (1,2,3,4) then t.length else 0 end,
				nullable = t.allownulls,
				dt_prec = case when s.dt_basetype in (10, 24) then t.prec else null end,
				dt_scale = case when s.dt_basetype in (10, 24) then t.scale else null end,
				dt_flags,
				allowidentity = case when (s.dt_basetype in (24,7,10,6,5) and t.scale = 0) then 1 else 0 end



			from systypes t, #sphelptype s
			where t.usertype > 99 and (@typename is null or t.name like @typename)
				and dt_usertype = t.usertype
			order by t.name
	end
go
/* End sp_MShelptype */

/*******************************************************************************/
print ''
print 'Creating sp_MSdependencies'
print ''
go

create procedure sp_MSdependencies
@objname varchar(92) = null, @objtype int = null, @flags int = 0x01fd, @objlist varchar(30) = null
as
	if (@objname = '?')
	begin
		print 'sp_MSobject_dependencies name = NULL, type = NULL, flags = 0x01fd'
		print '  name:  name or null (all objects of type)'
		print '  type:  type number (see below) or null'
		print '	  if both null, get all objects in database'
		print '  flags is a bitmask of the following values:'
		print '	  0x10000  = return multiple parent/child rows per object'
		print '	  0x20000  = descending return order'
		print '	  0x40000  = return children instead of parents'
		print '	  0x80000  = Include input object in output result set'
		print '	  0x100000 = return only firstlevel (immediate) parents/children'
		print '	  0x200000 = return only DRI dependencies'
		print '	  power(2, object type number(s))  to return in results set:'
		print '		  0 (1  	- 0x0001)	 - datatype'
		print '		  1 (2  	- 0x0002)	 - system tables or MS-internal objects'
		print '		  2 (4  	- 0x0004)	 - view'
		print '		  3 (8  	- 0x0008)	 - user table'
		print '		  4 (16		- 0x0010)	 - procedure'
		print '		  5 (32		- 0x0020)	 - log'
		print '		  6 (64 	- 0x0040)	 - default'
		print '		  7 (128	- 0x0080)	 - rule'
		print '		  8 (256	- 0x0100)	 - trigger'
		print '	  shortcuts:'
		print '		  29	 (0x011c) - trig, view, user table, procedure'
		print '		  448	(0x00c1) - rule, default, datatype'
		print '		  509	(0x01fd) - all but systables/objects'
		print '		  511	(0x01ff) - all'
		return 0
	end


	/* 
	 * If this proc is called in a tight loop, it tends to fill up the log in a small tempdb too fast 
	 * for the trunc. log on chkpt thread to keep up.  So help it out here.
	 */
	declare @dumptranpriv varchar(12)
	select @dumptranpriv = ltrim(str(convert(int, 0x0040)))
	exec ('if exists (select * from #SQLOLEDbUserProfile where dbid = db_id(''tempdb'') ' 
		+ ' and profilebits & ' + @dumptranpriv + ' = ' + @dumptranpriv + ') '
		+ ' dump tran tempdb with no_log')



	/* If they want SQLOLEDep_DRIOnly, remove all but usertable objects from @flags */

	if (@flags & 0x200000 <> 0)
		select @flags = (@flags & ~convert(int, 0x01ff)) | power(2, 3)


	if (@objtype in (0, 5, 6, 7))
	begin
		/* Print only, do not raiserror as we may be calling this blindly and this is not a real error. */
		print 'Rules, defaults, and datatypes do not have dependencies.'
		return (0)
	end

	/*
	 * Create #t1 and #t2 as temp object holding areas.  Columns are:
	 *	 tid		- temp object id
	 *	 ttype	 - temp object type
	 *	 pid		- parent or child object id
	 *	 ptype	 - parent or child object type
	 *	 bDone	 - NULL means dependencies not yet evaluated, else nonNULL.
	 */
	declare @curid int, @curcat int, @rowsaffected int
	declare @allobjs int
	declare @delinputobj int
	select @allobjs = 0, @delinputobj = 0, @curid = NULL, @curcat = NULL
	create table #t1 (
		tid				int				NULL, 
		ttype			smallint		NULL, 
		tcat			smallint		NULL, 
		pid				int				NULL, 
		ptype			smallint		NULL, 
		pcat			smallint		NULL, 
		bDone			smallint		NULL
	)
	create table #t2 (
		tid				int				NULL, 
		ttype			smallint		NULL, 
		tcat			smallint		NULL, 
		pid				int				NULL, 
		ptype			smallint		NULL, 
		pcat			smallint		NULL, 
		bDone			smallint		NULL
	)
	create table #tempudt (
		dtype			int				NOT NULL
	)

	/* Worktables we'll use for optimization.  #t3 and #t4 push us over the 64 2K page limit in 4.21, */
	/* but the real gain is from #temptrig anyway. */

	create table #t3 (
		tid				int				NOT NULL
	)
	create table #t4 (
		tid				int				NOT NULL
	)
	create clustered index #ci_t3 on #t3(tid) with allow_dup_row
	create clustered index #ci_t4 on #t4(tid) with allow_dup_row

	create table #temptrig(
		id				int				NOT NULL, 
		deltrig			int				NOT NULL, 
		sysstat			smallint		NOT NULL, 
		category		int				NOT NULL
	)
	create clustered index #ci_temptrig on #temptrig (deltrig) with allow_dup_row

	

	/*
	 * If both name and type are null, this means get every object in the
	 * database matching the specification they passed in.  Otherwise,
	 * find the passed object or all objects of the passed type.  Start off
	 * loading parent info (pid, tid); these will be put into child as needed.
	 * If Objlist is specified we simply load its contents into #t1.
	 */

	if (@objlist is not null)
	begin
		declare @mscategory varchar(12)
		select @mscategory = ltrim(str(convert(int, 0x0002)))

		exec('insert #t1 (pid, ptype, pcat) select l.objid, l.objtype, o.category &' +  @mscategory +
				' from ' + @objlist + ' l, sysobjects o where o.id = l.objid ') 
	end else begin

		if (@objname is null and @objtype is null)
		begin
			set nocount on
			select @allobjs = 1
			insert #t1 (pid, ptype, pcat) select o.id, o.sysstat & 0x0f, o.category & 0x0002 from sysobjects o
				where ((power(2, o.sysstat & 0x0f) & 0x01ff) <> 0) and (o.sysstat & 0x0f not in (6, 7))
		end else begin
			if (@objname is not null)
			begin
				select @curid = id, @objtype = o.sysstat & 0x0f, @curcat = o.category & 0x0002 from sysobjects o where id = object_id(@objname)
				if (@curid is null)
				begin
					
					RAISERROR (15001, -1, -1, @objname)
					return 1
				end
				if (@flags & 0x80000 = 0)
					select @delinputobj = @curid
			end

			set nocount on
			if (@curid is null)
				insert #t1 (pid, ptype, pcat) select o.id, o.sysstat & 0x0f, o.category & 0x0002 from sysobjects o
					where o.sysstat & 0x0f = @objtype
			else
				insert #t1 (pid, ptype, pcat) values (@curid, @objtype, @curcat)
		end

	end

	/*
	 * All initial objects are loaded as parents/children.  Now we loop, creating
	 * rows of child/parent relationships.  Use #t2 as a temp area for the selects
	 * to simulate recursion; when they find no rows, we're done with this step.
	 *
	 * Note that triggers are weird; they're part of a table definition but can
	 * also reference other tables, so we need to evaluate them both ways.  SQL
	 * Server stores the table for a trigger object as its deltrig; if a trigger
	 * references another table, that relationship is stored in sysdepends.
	 * This peculiarity of triggers requires separating the object-retrieval pass
	 * from the creation-sequence pass (below).  Also, the fact that trigger tables
	 * are stored in a non-indexed column (deltrig) requires us to use a worktable
	 * if we're returning triggers, so we don't continually tablescan sysobjects.
	 */

	if (@flags & power(2, 8) != 0)
		insert #temptrig select d.id, d.deltrig, d.sysstat, d.category from sysobjects d where d.sysstat & 0x0f = 8

	while (select count(*) from #t1 where bDone is null) > 0
	begin
		/*
		 * Remove Microsoft-internal or other system objects from #t1, unless
		 * @flags specified including system tables.  We do this here so that
		 * cascaded system dependencies are not included unless specifically
		 * requested.  For other restrictions, we wait until below so that all
		 * cascaded object types are fully evaluated.
		 */
		if (@flags & power(2, 1) = 0)
			delete #t1 where ttype = 1 or tcat = 0x0002 or pcat = 0x0002

		if (@flags & 0x40000 != 0)
		begin
			if (@flags & 0x200000 = 0) begin
				/* Table --> Triggers */
				if (@flags & power(2, 8) != 0)
					insert #t2 (tid, ttype, tcat, pid, ptype, pcat)
						select distinct t.pid, t.ptype, t.pcat, o.id, o.sysstat & 0x0f, o.category & 0x0002 from #t1 t, #temptrig o
							where t.bDone is null and t.ptype = 3 and o.deltrig = t.pid

				/* Object --> sysdepends children */
				insert #t2 (tid, ttype, tcat, pid, ptype, pcat)
					select distinct t.pid, t.ptype, t.pcat, d.id, o.sysstat & 0x0f, o.category & 0x0002
					from #t1 t, sysdepends d, sysobjects o
					where t.bDone is null and d.depid = t.pid and d.id = o.id
			end

			/* Object --> sysreferences children (FK tables referencing this one) */
			insert #t2 (tid, ttype, tcat, pid, ptype, pcat)
				select distinct t.pid, t.ptype, t.pcat, r.fkeyid, o.sysstat & 0x0f, o.category & 0x0002
				from #t1 t, sysreferences r, sysobjects o
				where t.bDone is null and r.rkeyid = t.pid and r.fkeyid = o.id

		end else begin
			if (@flags & 0x200000 = 0) begin
				/* Trigger --> Table */
				if (@flags & power(2, 3) != 0)
					insert #t2 (tid, ttype, tcat, pid, ptype, pcat)
						select distinct t.pid, t.ptype, t.pcat, o.deltrig, u.sysstat & 0x0f, u.category & 0x0002
							  from #t1 t, sysobjects o, sysobjects u
							where t.bDone is null and t.ptype = 8 and o.id = t.pid and o.deltrig != 0 and u.id = o.deltrig

				/* Object --> sysdepends parents */
				insert #t2 (tid, ttype, tcat, pid, ptype, pcat)
					select distinct t.pid, t.ptype, t.pcat, d.depid, o.sysstat & 0x0f, o.category & 0x0002
					from #t1 t, sysdepends d, sysobjects o
					where t.bDone is null and d.id = t.pid and d.depid = o.id
			end

			/* Object --> sysreferences parents (PK/UQ tables referenced by one) */
			insert #t2 (tid, ttype, tcat, pid, ptype, pcat)
				select distinct t.pid, t.ptype, t.pcat, r.rkeyid, o.sysstat & 0x0f, o.category & 0x0002
				from #t1 t, sysreferences r, sysobjects o
				where t.bDone is null and r.fkeyid = t.pid and r.rkeyid = o.id

		end

		/*
		 * We have this generation of parents in #t2, so clear the current
		 * child generation's bDone flags.  Then insert from #t2; the current
		 * parent generation becomes the next loop's child generation, with
		 * bDone = null until next loop's dependencies are selected.
		 */
		update #t1 set bDone = 1
		insert #t1 select * from #t2 where #t2.tid not in
			(select tid from #t1 where #t1.tid = #t2.tid and #t1.pid = #t2.pid)
		truncate table #t2

		/* If they only want one level, we're done.	*/
		if (@flags & 0x100000 <> 0)
			update #t1 set bDone = 1
	end

	/*
	 * The inner loop above did not put parents with no parents into the
	 * child (tid) list.  Do that now, then remove all rows where tid is
	 * NULL, because these were initial objects which now have a tid row.
	 * Just in case, remove self-refs from #t1, and also remove rows from #t1
	 * with NULL pid if a row exists for that tid where the pid is nonNULL.
	 * Avoid nested self-joins by using worktables.
	 */

	truncate table #t3
	insert #t3 select tid from #t1 where tid is not null
	update statistics #t3 #ci_t3
	insert #t1 (tid, ttype, tcat, bDone) select distinct pid, ptype, pcat, 0 from #t1 t
		where t.pid is not null and not exists (select * from #t3 where tid = t.pid)





	/*
	 * Because triggers can go in both directions, we'll need to check for
	 * circular dependencies on parent evaluation.  Since any tables referenced
	 * by the trigger must exist before the trigger can be created, remove rows
	 * where the trigger is the parent.
	 */
	if (@flags & 0x40000 = 0)
		delete #t1 where ptype = 8


	truncate table #t3
	insert #t3 select tid from #t1 where tid is not null and pid is not null
	update statistics #t3 #ci_t3
	delete #t1 where #t1.tid is null or #t1.tid = #t1.pid 
		or (#t1.pid is null and exists (select * from #t3 where tid = #t1.tid))





	/*
	 * If we're to get all objects, get all UDDTs (which aren't in sysobjects)
	 * and Rules/Defaults, assuming we're returning those types.
	 */
	if (@allobjs <> 0)
	begin
		if (@flags & power(2, 0) != 0)
			insert #tempudt
				select usertype from systypes where usertype > 99
		if (@flags & (power(2, 7) | power(2, 6)) != 0)
			insert #t2 (tid, ttype, tcat)
				select id, sysstat & 0x0f, 0 from sysobjects
				where sysstat in (7, 6)
				and category & 0x0800 = 0
	end else begin
		/*
		 * Not getting all objects.  Get any datatypes that
		 * are referenced by objects in #t1.  We don't care about specific
		 * datatype dependencies, we just want to know which ones are needed.
		 */
		if (@flags & power(2, 0) != 0)
			insert #tempudt select distinct usertype from syscolumns
				where usertype > 99 and id in (select tid from #t1)

		/*
		 * Load rules and defaults needed by datatypes and other #t1 objects
		 * into #t2.  Don't track specific object dependencies with these;
		 * we just want to know which ones are needed.  For defaults only, eliminate
		 * those which are constraints.
		 */
		if (@flags & power(2, 7) != 0)
		begin
			insert #t2 (tid, ttype, tcat)
				select distinct s.domain, 7, 0 from systypes s, #tempudt t
					where s.domain != 0 and s.usertype = t.dtype
						and s.domain not in (select tid from #t1)
			insert #t2 (tid, ttype, tcat)
				select distinct s.domain, 7, 0 from syscolumns s, #t1 t
					where s.domain != 0 and s.id = t.tid
						and s.domain not in (select tid from #t1)
		end
		if (@flags & power(2, 6) != 0)
		begin
			insert #t2 (tid, ttype, tcat)
				select distinct s.tdefault, 6, 0 from systypes s, #tempudt t
					where s.tdefault != 0 and s.usertype = t.dtype
						and s.tdefault not in (select tid from #t1)
						and s.tdefault not in (select id from sysobjects where category & 0x0800 != 0)
			insert #t2 (tid, ttype, tcat)
				select distinct s.cdefault, 6, 0 from syscolumns s, #t1 t
					where s.cdefault != 0 and s.id = t.tid
						and s.cdefault not in (select tid from #t1)
						and s.cdefault not in (select id from sysobjects where category & 0x0800 != 0)
		end
	end		/* Not getting all objects */

	/*
	 * Now that we've got all objects we want, eliminate those we don't
	 * want to return.  If @inputobj and they don't want it returned,
	 * remove it from the table.  Then eliminate object types they don't
	 * want returned.  Make sure that in doing so we retain all parent
	 * objects of the types we do want -- it is possible at this point
	 * that a tid we want has no rows except those with pids we don't want.
	 */
	if (@flags & 0x01ff != 0x01ff or @delinputobj != 0)
	begin
		delete #t1 where @flags & power(2, ttype) = 0 or tid = @delinputobj

		/*
		 * Be sure that the insert does not duplicate rows that will survive the
		 * following delete -- these are rows where the pid is not @delinputobj
		 * and ptype is either null or a type we'll keep (if ptype is null then
		 * pid hasn't been set so no need for more complex checking).
		 */
		insert #t1 (tid, ttype, tcat) select distinct tid, ttype, tcat from #t1
			where (@flags & power(2, ptype) = 0 or pid = @delinputobj)
				and tid not in (select tid from #t1 where ptype is null or
					(pid != @delinputobj and @flags & power(2, ptype) != 0))
		delete #t1 where @flags & power(2, ptype) = 0 or pid = @delinputobj
	end

	/*
	 * To determine creation order, find all objects which are not yet bDone
	 * and have no parents or whose parents are all bDone, and set their bDone
	 * to the next @curid.  This will leave bDone as the ascending order in
	 * which objects must be created (topological sort).  Again, use worktables
	 * to remove nested self-joins.
	 */
	update #t1 set bDone = 0
	select @curid = 1, @rowsaffected = 1
	while (@rowsaffected <> 0)
	begin

		if (@flags & 0x40000 != 0) begin
			truncate table #t3
			insert #t3 select pid from #t1 where pid is not null and bDone = 0
			update statistics #t3 #ci_t3
			update #t1 set bDone = @curid where bDone = 0 and tid not in (select tid from #t3)
		end else begin
			truncate table #t3
			truncate table #t4
			insert #t3 select tid from #t1 where bDone = 0				/* Parents not yet done */
			update statistics #t3 #ci_t3
			insert #t4 select tid from #t1								/* TIDs with (parents not yet done) */
				where pid is not null and pid in (select tid from #t3)
			update statistics #t4 #ci_t4
			update #t1 set #t1.bDone = @curid where #t1.bDone = 0 		/* TIDs who are not (TIDs with (parents not yet done)) */
				and not exists (select * from #t4 where tid = #t1.tid)
		end









		select @rowsaffected = @@rowcount, @curid = @curid + 1
	end

	/* For SQL60 only, we need to check circular dependencies (DRI for tables is the only way to get them). */
	/* This will have occurred if we still have any rows in #t1 where bDone = 0, after the above loop. */

	if exists (select * from #t1 where bDone = 0) begin
		RAISERROR (14300, -1, -1)
		return 1
	end


	/*
	 * Finally, return the objects.  Rules/Defaults must be created first so they're returned first,
	 * followed by UDDTs. followed by all other (sysdepends/DRI) dependencies.  @curid is the bDone
	 * value; we need to increment the #t1 value so our multi-result-set is in the proper sequence.
	 * Of course, these never have parents, so don't return them if asking for children.
	 */
	if (@flags & 0x40000 = 0) begin
		select @curid = 1
		if ((@flags & (power(2, 7) | power(2, 6)) != 0) and exists (select * from #t2)) begin
			update #t1 set bDone = bDone + 1
			select distinct oType = power(2, o.sysstat & 0x0f), oRuleDefName = o.name, oOwner = user_name(o.uid), oSequence = convert(smallint, @curid)
				from sysobjects o, #t2 t
				where o.id = t.tid
				order by power(2, o.sysstat & 0x0f), o.name
			select @curid = @curid + 1
		end
		if ((@flags & power(2, 0) != 0) and exists (select * from #tempudt)) begin
			update #t1 set bDone = bDone + 1
			select distinct oType = power(2, 0), oUDDTName = c.name, oOwner = user_name(c.uid), oSequence = convert(smallint, @curid)
				from systypes c, #tempudt t, sysobjects p
				where c.usertype = t.dtype
				order by c.name
			select @curid = @curid + 1
		end
	end

	/*
	 * Select dependency-style objects, returning parents if desired.
	 * Normally sorting is in terms of who must be created first, i.e. ascending:  parent-->child-->grandchild.
	 * Descending order (child-->parent-->grandparent) would be used for a graphical-dependencies evaluator showing
	 * the parents.  Therefore we invert bDone if descending sort.  bDone is 1-based; min + max - bDone gives inversion.
	 * Note:  Always return at least this empty set.
	 */
	if (@flags & 0x20000 != 0) begin
		select @curid = max(bDone) + min(bDone) from #t1
		update #t1 set bDone = convert(smallint, @curid) - bDone
	end
	if (@flags & 0x10000 != 0)
		select distinct oType = power(2, o.sysstat & 0x0f), oObjName = o.name, oOwner = user_name(o.uid),
				RelType = power(2, p.sysstat & 0x0f), RelName = p.name, RelOwner = user_name(p.uid),
				oSequence = t.bDone
			from sysobjects o, sysobjects p, #t1 t
			where o.id = t.tid and p.id =* t.pid
			order by t.bDone, power(2, o.sysstat & 0x0f), o.name
	else
		select distinct oType = power(2, o.sysstat & 0x0f), oObjName = o.name, oOwner = user_name(o.uid),
				oSequence = t.bDone
			from sysobjects o, #t1 t
			where o.id = t.tid
			order by t.bDone, power(2, o.sysstat & 0x0f), o.name

go
/* End sp_MSdependencies */

/************* DUMP THE TRANSACTION LOG **************************************/
/* Comment this out if you don't want your log dumped.  If you rerun this    */
/* script periodically, you will run out of transaction log space.           */
print ''
print 'Dumping transaction log...'
print ''
go
dump tran master with no_log
go
/************* END DUMP THE TRANSACTION LOG **********************************/

/*******************************************************************************/
print ''
print 'Creating sp_MStablespace'
print ''
go

create procedure sp_MStablespace
@name varchar(92), @id int = null
as
	declare @rows int, @datasizeused int, @indexsizeused int, @pagesize int
	declare @dbname varchar(30)
	select @dbname = db_name()
	

	if (@id is null)
		select @id = id from sysobjects where id = object_id(@name) and sysstat & 0x0f in (1, 3)
	if (@id is null)
	begin
		
		RAISERROR (15009, -1, -1, @name, @dbname)
		return 1
	end

	/* rows */
	SELECT @rows = rows
		FROM sysindexes
		WHERE indid < 2 and id = @id

	/* data */
	SELECT @datasizeused =
	(SELECT sum(dpages)
	 FROM sysindexes
	 WHERE indid < 2 and id = @id)
	+
	(SELECT isnull(sum(used), 0)
	 FROM sysindexes
	 WHERE indid = 255 and id = @id)

	/* index */
	SELECT @indexsizeused =
	(SELECT sum(used)
	 FROM sysindexes
	 WHERE indid in (0, 1, 255) and id = @id)
	 - @datasizeused

	/* Pagesize on this server (sysindexes stores size info in pages) */
	select @pagesize = v.low / 1024 from master..spt_values v where v.number=1 and v.type='E'

	select Rows = @rows, DataSpaceUsed = @datasizeused * @pagesize, IndexSpaceUsed = @indexsizeused	* @pagesize
go

/* End sp_MStablespace */

/*******************************************************************************/
print ''
print 'Creating sp_MSindexspace'
print ''
go

CREATE PROCEDURE sp_MSindexspace
	@tablename varchar(92), @index_name varchar(30) = NULL
AS
BEGIN
  DECLARE @table_id int
  DECLARE @index_id int
  DECLARE @msg varchar(255)
  

  /* Make sure @tablename is local to the current database */
  IF (@tablename like '%.%.%') AND
     (substring(@tablename, 1, charindex('.', @tablename) - 1) <> db_name())
  BEGIN
    
    RAISERROR (15078, -1, -1, '')
    RETURN(1)
  END
  /* Make sure that @tablename and @index_name exist */
  SELECT @table_id = id
  FROM sysobjects
  WHERE (id = object_id(@tablename))
    AND (sysstat & 0xf = 3)
  IF (@table_id = NULL)
  BEGIN
    
    RAISERROR (15001, -1, -1, @tablename)
    RETURN(1)
  END
  IF (@index_name <> NULL)
  BEGIN
    SELECT @index_id = indid
    FROM sysindexes
    WHERE (name = @index_name)
      AND (id = object_id(@tablename))
    IF (@index_id = NULL)
    BEGIN
      SELECT @msg = @tablename + '.' + @index_name
      
      RAISERROR (15001, -1, -1, @msg)
      RETURN(1)
    END
  END
  /* Ok, we're good to go */
  IF (user_id() = 1) 
    CHECKPOINT
  IF (@index_name = NULL)
  BEGIN
    CREATE TABLE #IndexSizeTemp (
		IndexID			tinyint			NOT NULL, 
		IndexName		varchar(30)		NOT NULL, 
		IndexSize		int				NOT NULL, 
		Comments		varchar(28)		NOT NULL
	)  
    INSERT INTO #IndexSizeTemp
    SELECT indid, name, 0, ''
    FROM sysindexes 
    WHERE (id = object_id(@tablename))
      AND ((indid > 0) AND (indid < 255)) 
    UPDATE #IndexSizeTemp
    SET IndexSize = used * 2,
        Comments = '(None)' 
    FROM sysindexes si, #IndexSizeTemp ist
    WHERE (id = object_id(@tablename))
      AND (indid > 1) AND (indid < 255)
      AND (si.indid = ist.IndexID)
    UPDATE #IndexSizeTemp
    SET IndexSize = (used - dpages - isnull((SELECT sum(used) 
                                             FROM sysindexes
                                             WHERE (indid > 1) AND (indid < 255) 
                                               AND (id = object_id(@tablename))), 0)) * 2,
        Comments = 'Size excludes actual data.' 
    FROM sysindexes si, #IndexSizeTemp ist
    WHERE (id = object_id(@tablename))
      AND (indid = 1)
      AND (si.indid = ist.IndexID)
    SELECT 'Index ID' = IndexID, 'Index Name' = IndexName, 'Size (KB)' = IndexSize, Comments 
    FROM #IndexSizeTemp
    ORDER BY IndexID
    DROP TABLE #IndexSizeTemp
  END
  ELSE
  BEGIN 
    DECLARE @indid int
    SELECT @indid = indid 
    FROM sysindexes 
    WHERE (id = object_id(@tablename))
      AND (name = @index_name)
    /* The non-clustered index case */
    IF ((@indid > 1) AND (@indid < 255))
    BEGIN
      SELECT 'Size (KB)' = used * 2 
      FROM sysindexes 
      WHERE (id = object_id(@tablename))
        AND (name = @index_name)
      RETURN(0) 
    END
    /* The clustered index case */
    IF (@indid = 1)
    BEGIN
      SELECT 'Size (KB)' =
             (used - dpages - isnull((SELECT sum(used) 
                                      FROM sysindexes
                                      WHERE (indid > 1) AND (indid < 255) 
                                        AND (id = object_id(@tablename))), 0)) * 2 
      FROM sysindexes 
      WHERE (id = object_id(@tablename))
        AND (name = @index_name)
    END
  END
  RETURN(0)
END
go
/* End sp_MSindexspace */

/*******************************************************************************/

print ''
print 'Creating sp_MStablekeys'
print ''
go

create procedure sp_MStablekeys
@tablename varchar(92) = null, @colname varchar(30) = null, @type int = null, @keyname varchar(30) = null, @flags int = null
as
	/* This proc returns the table's DRI keys.  @type is the type(s) of key(s) to return. */
	/* Make sure @type is only the key types (DRI_PRIMARYKEY, DRI_UNIQUE, DRI_REFERENCE). */
	if (@type is null)
		select @type = 0x000e
	else
		select @type = @type & 0x000e

	/* Flags usage:  For daVinci, to pass call thru to sp_MStablerefs. */
	if (@flags is null)
		select @flags = 0

	set nocount on
	create table #spkeys
	(
		cType				tinyint			NOT NULL,	/* key Type */
		cName				varchar(30)		NOT NULL,	/* key Name */
		cFlags				int				NULL,		/* e.g., 1 = clustered for PK/Unique */
		cColCount			int				NULL,		/* number of columns (or column pairs) in the key */
		cFillFactor			tinyint			NULL,		/* Fill factor of index creation */
		cRefTable			varchar(64)		NULL,		/* owner-qual Referenced table name for FKs */
		cRefKey				varchar(32)		NULL,		/* name of referenced key in referenced table */
			-- Note:  cConstID replaces the column list used in 6.0, for speed.
			-- The output set MUST replace this with either index_col(@tablename, cIndexID, 1-16) and NULL * 16
			-- (for PK/UQ) UNION col_name(r.fkeyid, r.fkey1-16) and col_name(r.rkeyid, r.rkey1-16), for SQLOLE,
			-- and these MUST BE varchar(32) for alignment in the SQLOLE cache structure!
		cConstID			int				NULL,		/* Reference constraint ID, if Foreign Key  */
		cIndexID			int				NULL		/* ID of this key's index, if PK/UQ */
	)

	declare @cType int, @cName varchar(30), @cFlags int, @cRefTable varchar(64), @fillfactor tinyint
	declare @objid int, @constid int, @indid int, @keycnt int, @q1 varchar(255), @q2 varchar(255), @objtype int
	declare @haskeytypes int, @wantkeytypes int

	/* First see if @keyname was defined, and override @tablename and @type if so. */
	if (@keyname is not null)
	begin
		select @objid = id, @type = power(2, status & 0x0f) from sysconstraints where constid = object_id(@keyname)
		if (@objid is null)	begin
			RAISERROR (15001, -1, -1, @keyname)
			return 1
		end
		/* Now get the tablename for the index_col below */
		select @tablename = user_name(uid) + '.' + name from sysobjects where id = @objid
	end else begin
		/* Want all keys for this table (of @type type). */
		select @objid = id, @objtype = sysstat & 0x0f, @haskeytypes = category & 0x0604
			from sysobjects where id = object_id(@tablename)
		if (@objid is null)	begin
			RAISERROR (15001, -1, -1, @tablename)
			return 1
		end
		if (@objtype not in (1, 3))	begin
			RAISERROR (15218, -1, -1, @tablename)
			return 1
		end
		if @colname is not null and not exists (select * from syscolumns where id = @objid and name = @colname) begin
			RAISERROR (15253, -1, -1, @colname, @tablename)
			return 1
		end

		/* Skip cursor opening if we don't have any keys (of the type wanted); return a set anyway, for the cache. */
		if (@haskeytypes = 0)
			goto ReturnSet

		/* Map from the input bitmask to the category bitmask */
		select @wantkeytypes = 0
		if ((@type & power(2, 1)) <> 0)
			select @wantkeytypes = @wantkeytypes | 0x200
		if ((@type & power(2, 2)) <> 0)
			select @wantkeytypes = @wantkeytypes | 0x400
		if ((@type & power(2, 3)) <> 0)
			select @wantkeytypes = @wantkeytypes | 0x4
		if ((@haskeytypes & @wantkeytypes) = 0)
			goto ReturnSet
	end

	/* Preprocessor won't replace within quotes so have to use str(). */
	declare @sysgenname varchar(12), @pkstr varchar(12), @uqstr varchar(12), @fkstr varchar(12), @objtypebits varchar(12)
	select @sysgenname = ltrim(str(convert(int, 0x00020000)))
	select @pkstr = ltrim(str(convert(int, 1)))
	select @uqstr = ltrim(str(convert(int, 2)))
	select @fkstr = ltrim(str(convert(int, 3)))
	select @objtypebits = ltrim(str(convert(int, 0x0f)))

	/* Other ints we need strings for */
	declare @objidstr varchar(12), @typestr varchar(12)
	select @objidstr = ltrim(str(@objid))
	select @typestr = ltrim(str(@type))

	/* Qualifying key name. */
	declare @qualkeyname varchar(100)
	select @qualkeyname = null
	if (@keyname is not null)
		select @qualkeyname = ' and constid = object_id(''' + @keyname + ''')'

	/*********************/
	/* Main cursor loop. */
	/*********************/
	exec('declare hC insensitive cursor for select constid, status & ' + @objtypebits + ', status & ' + @sysgenname + 
			' from sysconstraints where id = ' + @objidstr + ' and (' + @typestr + ' & power(2, status & 0x0f) != 0) ' + @qualkeyname)
	open hC
	fetch hC into @constid, @cType, @cFlags
	while (@@fetch_status >= 0) begin
		if (object_name(@constid) is null) begin
			raiserror 55555 'Assert failed:  object_name(@constid) is null in sp_MStablekeys (pk/uq)'
			return 1
		end

		/* DRI_PRIMARYKEY, DRI_UNIQUE */
		if (@cType in (1, 2)) begin
			/* Get the index id enforcing this constraint. */
			select @indid = i.indid, @cName = o.name, @fillfactor = i.OrigFillFactor,
					@cFlags = @cFlags | (case indid when 1 then 0x00000001 else 0 end),		/* test for clustered index */
					@keycnt = case indid when 1 then keycnt else keycnt - 1 end					/* keycnt includes RID if nc index */
				from sysindexes i, sysobjects o
				where o.id = @constid and i.name = o.name and i.status & 0x1800 <> 0
			if (@indid is null) begin
				raiserror 77777 "Assert failed:  @indid is null in sp_MStablekeys (pk/uq)"
				return 1
			end

			/* Load our temp table. */
			insert #spkeys values (@cType, @cName, @cFlags, @keycnt, @fillfactor, null, null, null, @indid)
		end

		/* DRI_REFERENCE */
		else if (@cType in (3)) begin
			/* Get the key column information from sysreferences. */
			select @keycnt = r.keycnt, @cName = object_name(r.constid), @cRefTable = user_name(o.uid) + '.' + o.name
				from sysreferences r, sysobjects o where r.constid = @constid and o.id = r.rkeyid

			/* Follow r.rkeyindid back to sysindexes to get the ref key name. */
			declare @cRefKey varchar(32)
			select @cRefKey = i.name, @cFlags = c.status from sysreferences r, sysindexes i, sysconstraints c
				where c.constid = r.constid and r.constid = @constid
				and i.id = r.rkeyid and i.indid = r.rkeyindid and i.status & 0x1800 <> 0

			/* Load our temp table. */
			insert #spkeys values (@cType, @cName, @cFlags, @keycnt, null, @cRefTable, @cRefKey, @constid, null)
		end		/* Key type */

		/* Get the next row. */
		fetch hC into @constid, @cType, @cFlags
	end			/* PRIMARY/UNIQUE */
	deallocate hC

	/* Now output the data */
ReturnSet:
	set nocount off
	select cType, cName, cFlags, cColCount, cFillFactor, cRefTable, cRefKey,
			cKeyCol1 = convert(varchar(32), index_col(@tablename, cIndexID, 1)), 
			cKeyCol2 = convert(varchar(32), index_col(@tablename, cIndexID, 2)),	
			cKeyCol3 = convert(varchar(32), index_col(@tablename, cIndexID, 3)), 
			cKeyCol4 = convert(varchar(32), index_col(@tablename, cIndexID, 4)),
			cKeyCol5 = convert(varchar(32), index_col(@tablename, cIndexID, 5)), 
			cKeyCol6 = convert(varchar(32), index_col(@tablename, cIndexID, 6)),	
			cKeyCol7 = convert(varchar(32), index_col(@tablename, cIndexID, 7)), 
			cKeyCol8 = convert(varchar(32), index_col(@tablename, cIndexID, 8)),
			cKeyCol9 = convert(varchar(32), index_col(@tablename, cIndexID, 9)), 
			cKeyCol10 = convert(varchar(32), index_col(@tablename, cIndexID, 10)),
			cKeyCol11 = convert(varchar(32), index_col(@tablename, cIndexID, 11)), 
			cKeyCol12 = convert(varchar(32), index_col(@tablename, cIndexID, 12)),
			cKeyCol13 = convert(varchar(32), index_col(@tablename, cIndexID, 13)), 
			cKeyCol14 = convert(varchar(32), index_col(@tablename, cIndexID, 14)),	
			cKeyCol15 = convert(varchar(32), index_col(@tablename, cIndexID, 15)), 
			cKeyCol16 = convert(varchar(32), index_col(@tablename, cIndexID, 16)),
			cRefCol1 = convert(varchar(32), null), 
			cRefCol2 = convert(varchar(32), null), 
			cRefCol3 = convert(varchar(32), null), 
			cRefCol4 = convert(varchar(32), null),
			cRefCol5 = convert(varchar(32), null),
			cRefCol6 = convert(varchar(32), null),
			cRefCol7 = convert(varchar(32), null),
			cRefCol8 = convert(varchar(32), null),
			cRefCol9 = convert(varchar(32), null),
			cRefCol10 = convert(varchar(32), null),
			cRefCol11 = convert(varchar(32), null),
			cRefCol12 = convert(varchar(32), null),
			cRefCol13 = convert(varchar(32), null),
			cRefCol14 = convert(varchar(32), null),
			cRefCol15 = convert(varchar(32), null),
			cRefCol16 = convert(varchar(32), null),
			cIndexID
		from #spkeys where cType in (1, 2) 
			and (@colname is null or
				index_col(@tablename, cIndexID, 1) = @colname or
				index_col(@tablename, cIndexID, 2) = @colname or
				index_col(@tablename, cIndexID, 3) = @colname or
				index_col(@tablename, cIndexID, 4) = @colname or
				index_col(@tablename, cIndexID, 5) = @colname or
				index_col(@tablename, cIndexID, 6) = @colname or
				index_col(@tablename, cIndexID, 7) = @colname or
				index_col(@tablename, cIndexID, 8) = @colname or
				index_col(@tablename, cIndexID, 9) = @colname or
				index_col(@tablename, cIndexID, 10) = @colname or
				index_col(@tablename, cIndexID, 11) = @colname or
				index_col(@tablename, cIndexID, 12) = @colname or
				index_col(@tablename, cIndexID, 13) = @colname or
				index_col(@tablename, cIndexID, 14) = @colname or
				index_col(@tablename, cIndexID, 15) = @colname or
				index_col(@tablename, cIndexID, 16) = @colname
			)
		UNION
		select c.cType, c.cName, c.cFlags, c.cColCount, c.cFillFactor, c.cRefTable, c.cRefKey,
			cKeyCol1 = convert(varchar(32), col_name(r.fkeyid, r.fkey1)), 
			cKeyCol2 = convert(varchar(32), col_name(r.fkeyid, r.fkey2)),  
			cKeyCol3 = convert(varchar(32), col_name(r.fkeyid, r.fkey3)),  
			cKeyCol4 = convert(varchar(32), col_name(r.fkeyid, r.fkey4)), 
			cKeyCol5 = convert(varchar(32), col_name(r.fkeyid, r.fkey5)),  
			cKeyCol6 = convert(varchar(32), col_name(r.fkeyid, r.fkey6)),  
			cKeyCol7 = convert(varchar(32), col_name(r.fkeyid, r.fkey7)),  
			cKeyCol8 = convert(varchar(32), col_name(r.fkeyid, r.fkey8)), 
			cKeyCol9 = convert(varchar(32), col_name(r.fkeyid, r.fkey9)),  
			cKeyCol10 = convert(varchar(32), col_name(r.fkeyid, r.fkey10)), 
			cKeyCol11 = convert(varchar(32), col_name(r.fkeyid, r.fkey11)),
			cKeyCol12 = convert(varchar(32), col_name(r.fkeyid, r.fkey12)), 
			cKeyCol13 = convert(varchar(32), col_name(r.fkeyid, r.fkey13)), 
			cKeyCol14 = convert(varchar(32), col_name(r.fkeyid, r.fkey14)), 
			cKeyCol15 = convert(varchar(32), col_name(r.fkeyid, r.fkey15)),  
			cKeyCol16 = convert(varchar(32), col_name(r.fkeyid, r.fkey16)),
			cRefCol1 = convert(varchar(32), col_name(r.rkeyid, r.rkey1)), 
			cRefCol2 = convert(varchar(32), col_name(r.rkeyid, r.rkey2)),	
			cRefCol3 = convert(varchar(32), col_name(r.rkeyid, r.rkey3)), 
			cRefCol4 = convert(varchar(32), col_name(r.rkeyid, r.rkey4)),
			cRefCol5 = convert(varchar(32), col_name(r.rkeyid, r.rkey5)), 
			cRefCol6 = convert(varchar(32), col_name(r.rkeyid, r.rkey6)), 
			cRefCol7 = convert(varchar(32), col_name(r.rkeyid, r.rkey7)), 
			cRefCol8 = convert(varchar(32), col_name(r.rkeyid, r.rkey8)),
			cRefCol9 = convert(varchar(32), col_name(r.rkeyid, r.rkey9)), 
			cRefCol10 = convert(varchar(32), col_name(r.rkeyid, r.rkey10)), 
			cRefCol11 = convert(varchar(32), col_name(r.rkeyid, r.rkey11)), 
			cRefCol12 = convert(varchar(32), col_name(r.rkeyid, r.rkey12)),
			cRefCol13 = convert(varchar(32), col_name(r.rkeyid, r.rkey13)), 
			cRefCol14 = convert(varchar(32), col_name(r.rkeyid, r.rkey14)), 
			cRefCol15 = convert(varchar(32), col_name(r.rkeyid, r.rkey15)), 
			cRefCol16 = convert(varchar(32), col_name(r.rkeyid, r.rkey16)),
			cIndexID
		from #spkeys c, sysreferences r where c.cType = 3 and r.constid = c.cConstID
			and (@colname is null or
				col_name(r.fkeyid, r.fkey1) = @colname or
				col_name(r.fkeyid, r.fkey2) = @colname or
				col_name(r.fkeyid, r.fkey3) = @colname or
				col_name(r.fkeyid, r.fkey4) = @colname or
				col_name(r.fkeyid, r.fkey5) = @colname or
				col_name(r.fkeyid, r.fkey6) = @colname or
				col_name(r.fkeyid, r.fkey7) = @colname or
				col_name(r.fkeyid, r.fkey8) = @colname or
				col_name(r.fkeyid, r.fkey9) = @colname or
				col_name(r.fkeyid, r.fkey10) = @colname or
				col_name(r.fkeyid, r.fkey11) = @colname or
				col_name(r.fkeyid, r.fkey12) = @colname or
				col_name(r.fkeyid, r.fkey13) = @colname or
				col_name(r.fkeyid, r.fkey14) = @colname or
				col_name(r.fkeyid, r.fkey15) = @colname or
				col_name(r.fkeyid, r.fkey16) = @colname
			)
		order by cType, cName

	if (@flags & 1 <> 0)
		exec sp_MStablerefs @tablename, 'actualkeycols', 'foreign'

go
/* End sp_MStablekeys */

/*-- Another SQL60-only proc ---------------------------------------------------*/
print ''
print 'Creating sp_MSgetalertinfo'
print ''
go
create procedure sp_MSgetalertinfo
	@includeaddresses bit = 0
as
	/* Return all alert info at one go, for performance reasons. */
	declare @FailSafeOperator varchar(255)
	declare @NotificationMethod int
	declare @ForwardingServer varchar(255)
	declare @ForwardingSeverity int
	declare @PagerToTemplate varchar(255)
	declare @PagerCCTemplate varchar(255)
	declare @PagerSubjectTemplate varchar(255)
	declare @PagerSendSubjectOnly int
	declare @FailSafeEmailAddress varchar(255)
	declare @FailSafePagerAddress varchar(255)

	exec master.dbo.xp_regread 'HKEY_LOCAL_MACHINE', 'SOFTWARE\Microsoft\MSSQLServer\SQLExecutive', 'AlertFailSafeOperator', @param = @FailSafeOperator OUT
	exec master.dbo.xp_regread 'HKEY_LOCAL_MACHINE', 'SOFTWARE\Microsoft\MSSQLServer\SQLExecutive', 'AlertNotificationMethod', @param = @NotificationMethod OUT
	exec master.dbo.xp_regread 'HKEY_LOCAL_MACHINE', 'SOFTWARE\Microsoft\MSSQLServer\SQLExecutive', 'AlertForwardingServer', @param = @ForwardingServer OUT
	exec master.dbo.xp_regread 'HKEY_LOCAL_MACHINE', 'SOFTWARE\Microsoft\MSSQLServer\SQLExecutive', 'AlertForwardingSeverity', @param = @ForwardingSeverity OUT
	exec master.dbo.xp_regread 'HKEY_LOCAL_MACHINE', 'SOFTWARE\Microsoft\MSSQLServer\SQLExecutive', 'AlertPagerToTemplate', @param = @PagerToTemplate OUT
	exec master.dbo.xp_regread 'HKEY_LOCAL_MACHINE', 'SOFTWARE\Microsoft\MSSQLServer\SQLExecutive', 'AlertPagerCCTemplate', @param = @PagerCCTemplate OUT
	exec master.dbo.xp_regread 'HKEY_LOCAL_MACHINE', 'SOFTWARE\Microsoft\MSSQLServer\SQLExecutive', 'AlertPagerSubjectTemplate', @param = @PagerSubjectTemplate OUT
	exec master.dbo.xp_regread 'HKEY_LOCAL_MACHINE', 'SOFTWARE\Microsoft\MSSQLServer\SQLExecutive', 'AlertPagerSendSubjectOnly', @param = @PagerSendSubjectOnly OUT

	if (@includeaddresses <> 0) begin
		exec master.dbo.xp_regread 'HKEY_LOCAL_MACHINE', 'SOFTWARE\Microsoft\MSSQLServer\SQLExecutive', 'AlertFailSafeEmailAddress', @param = @FailSafeEmailAddress OUT
		exec master.dbo.xp_regread 'HKEY_LOCAL_MACHINE', 'SOFTWARE\Microsoft\MSSQLServer\SQLExecutive', 'AlertFailSafePagerAddress', @param = @FailSafePagerAddress OUT
	end

	select
		AlertFailSafeOperator = @FailSafeOperator,
		AlertNotificationMethod = @NotificationMethod,
		AlertForwardingServer = @ForwardingServer,
		AlertForwardingSeverity = @ForwardingSeverity,
		AlertPagerToTemplate = @PagerToTemplate,
		AlertPagerCCTemplate = @PagerCCTemplate,
		AlertPagerSubjectTemplate = @PagerSubjectTemplate,
		AlertPagerSendSubjectOnly = @PagerSendSubjectOnly

	if (@includeaddresses <> 0) 
		select
			AlertFailSafeEmailAddress = @FailSafeEmailAddress,
			AlertFailSafePagerAddress = @FailSafePagerAddress
go
/* End sp_MSgetalertinfo */

/*-- Another SQL60-only proc ---------------------------------------------------*/
print ''
print 'Creating sp_MSsetalertinfo'
print ''
go
create procedure sp_MSsetalertinfo
	@failsafeoperator varchar(255) = null,
	@notificationmethod int = null,
	@forwardingserver varchar(255) = null,
	@forwardingseverity int = null,
	@pagertotemplate varchar(255) = null,
	@pagercctemplate varchar(255) = null,
	@pagersubjecttemplate varchar(255) = null,
	@pagersendsubjectonly int = null,
	@failsafeemailaddress varchar(255) = null,
	@failsafepageraddress varchar(255) = null
as
	/* Set all alert info at one go, for performance reasons.  Translate values if needed. */
	if (@pagersendsubjectonly is not null and @pagersendsubjectonly <> 0)
		select @pagersendsubjectonly = 1

	if (@failsafeoperator is not null)
		exec master.dbo.xp_regwrite 'HKEY_LOCAL_MACHINE', 'SOFTWARE\Microsoft\MSSQLServer\SQLExecutive', 'AlertFailSafeOperator', 'REG_SZ', @failsafeoperator
	if (@notificationmethod is not null)
		exec master.dbo.xp_regwrite 'HKEY_LOCAL_MACHINE', 'SOFTWARE\Microsoft\MSSQLServer\SQLExecutive', 'AlertNotificationMethod', 'REG_DWORD', @notificationmethod
	if (@forwardingserver is not null)
		exec master.dbo.xp_regwrite 'HKEY_LOCAL_MACHINE', 'SOFTWARE\Microsoft\MSSQLServer\SQLExecutive', 'AlertForwardingServer', 'REG_SZ', @forwardingserver
	if (@forwardingseverity is not null)
		exec master.dbo.xp_regwrite 'HKEY_LOCAL_MACHINE', 'SOFTWARE\Microsoft\MSSQLServer\SQLExecutive', 'AlertForwardingSeverity', 'REG_DWORD', @forwardingseverity
	if (@pagertotemplate is not null)
		exec master.dbo.xp_regwrite 'HKEY_LOCAL_MACHINE', 'SOFTWARE\Microsoft\MSSQLServer\SQLExecutive', 'AlertPagerToTemplate', 'REG_SZ', @pagertotemplate
	if (@pagercctemplate is not null)
		exec master.dbo.xp_regwrite 'HKEY_LOCAL_MACHINE', 'SOFTWARE\Microsoft\MSSQLServer\SQLExecutive', 'AlertPagerCCTemplate', 'REG_SZ', @pagercctemplate
	if (@pagersubjecttemplate is not null)
		exec master.dbo.xp_regwrite 'HKEY_LOCAL_MACHINE', 'SOFTWARE\Microsoft\MSSQLServer\SQLExecutive', 'AlertPagerSubjectTemplate', 'REG_SZ', @pagersubjecttemplate
	if (@pagersendsubjectonly is not null)
		exec master.dbo.xp_regwrite 'HKEY_LOCAL_MACHINE', 'SOFTWARE\Microsoft\MSSQLServer\SQLExecutive', 'AlertPagerSendSubjectOnly', 'REG_DWORD', @pagersendsubjectonly
	if (@failsafeemailaddress is not null)
		exec master.dbo.xp_regwrite 'HKEY_LOCAL_MACHINE', 'SOFTWARE\Microsoft\MSSQLServer\SQLExecutive', 'AlertFailSafeEmailAddress', 'REG_SZ', @failsafeemailaddress
	if (@failsafepageraddress is not null)
		exec master.dbo.xp_regwrite 'HKEY_LOCAL_MACHINE', 'SOFTWARE\Microsoft\MSSQLServer\SQLExecutive', 'AlertFailSafePagerAddress', 'REG_SZ', @failsafepageraddress
go
/* End sp_MSgetalertinfo */

/*-- Another SQL60-only proc ---------------------------------------------------*/
print ''
print 'Creating sp_MSgetexecinfo'
print ''
go
create procedure sp_MSgetexecinfo
as
	/* Return all SQLExecutive info at one go, for performance reasons. */
	declare @AutoStart int
	declare @RestartSQLServer int
	declare @RestartSQLServerInterval int
	declare @LimitHistoryRows int
	declare @LimitHistoryRowsMax int
	declare @LimitHistoryRowsPerTaskMax int

	exec master.dbo.xp_regread 'HKEY_LOCAL_MACHINE', 'SYSTEM\CurrentControlSet\Services\SQLExecutive', 'Start', @param = @AutoStart OUT
	exec master.dbo.xp_regread 'HKEY_LOCAL_MACHINE', 'SOFTWARE\Microsoft\MSSQLServer\SQLExecutive', 'RestartSQLServer', @param = @RestartSQLServer OUT
	exec master.dbo.xp_regread 'HKEY_LOCAL_MACHINE', 'SOFTWARE\Microsoft\MSSQLServer\SQLExecutive', 'RestartSQLServerInterval', @param = @RestartSQLServerInterval OUT
	exec master.dbo.xp_regread 'HKEY_LOCAL_MACHINE', 'SOFTWARE\Microsoft\MSSQLServer\SQLExecutive', 'SyshistoryLimitRows', @param = @LimitHistoryRows OUT
	exec master.dbo.xp_regread 'HKEY_LOCAL_MACHINE', 'SOFTWARE\Microsoft\MSSQLServer\SQLExecutive', 'SyshistoryMaxRows', @param = @LimitHistoryRowsMax OUT
	exec master.dbo.xp_regread 'HKEY_LOCAL_MACHINE', 'SOFTWARE\Microsoft\MSSQLServer\SQLExecutive', 'TaskHistoryMaxRows', @param = @LimitHistoryRowsPerTaskMax OUT

	/* Remember that for the Services, 2 == AutoStart, 3 == don't (don't ask me why). */
	select
		AutoStart = case when (2 = @AutoStart) then 1 else 0 end,
		RestartSQLServer = @RestartSQLServer,
		RestartSQLServerInterval = @RestartSQLServerInterval,
		LimitHistoryRows = @LimitHistoryRows,
		LimitHistoryRowsMax = @LimitHistoryRowsMax,
		LimitHistoryRowsPerTaskMax = @LimitHistoryRowsPerTaskMax
go
/* End sp_MSgetexecinfo */

/*-- Another SQL60-only proc ---------------------------------------------------*/
print ''
print 'Creating sp_MSsetexecinfo'
print ''
go
create procedure sp_MSsetexecinfo
	@autostart int = null,
	@restartsqlserver int = null,
	@restartsqlserverinterval int = null,
	@limithistoryrows int = null,
	@limithistoryrowsmax int = null,
	@limithistoryrowspertaskmax int = null
as
	/* Set all SQLExecutive info at one go, for performance reasons.  Translate values if needed. */
	/* Remember that for the Services, 2 == AutoStart, 3 == don't (don't ask me why). */
	if (@autostart is not null)
		select @autostart = case when (0 = @autostart) then 3 else 2 end
	if (@restartsqlserver is not null and @restartsqlserver <> 0)
		select @restartsqlserver = 1
	if (@limithistoryrows is not null and @limithistoryrows <> 0)
		select @limithistoryrows = 1

	if (@autostart is not null)
		exec master.dbo.xp_regwrite 'HKEY_LOCAL_MACHINE', 'SYSTEM\CurrentControlSet\Services\SQLExecutive', 'Start', 'REG_DWORD', @autostart
	if (@restartsqlserver is not null)
		exec master.dbo.xp_regwrite 'HKEY_LOCAL_MACHINE', 'SOFTWARE\Microsoft\MSSQLServer\SQLExecutive', 'RestartSQLServer', 'REG_DWORD', @restartsqlserver
	if (@restartsqlserverinterval is not null)
		exec master.dbo.xp_regwrite 'HKEY_LOCAL_MACHINE', 'SOFTWARE\Microsoft\MSSQLServer\SQLExecutive', 'RestartSQLServerInterval', 'REG_DWORD', @restartsqlserverinterval
	if (@limithistoryrows is not null)
		exec master.dbo.xp_regwrite 'HKEY_LOCAL_MACHINE', 'SOFTWARE\Microsoft\MSSQLServer\SQLExecutive', 'SyshistoryLimitRows', 'REG_DWORD', @limithistoryrows
	if (@limithistoryrowsmax is not null)
		exec master.dbo.xp_regwrite 'HKEY_LOCAL_MACHINE', 'SOFTWARE\Microsoft\MSSQLServer\SQLExecutive', 'SyshistoryMaxRows', 'REG_DWORD', @limithistoryrowsmax
	if (@limithistoryrowspertaskmax is not null)
		exec master.dbo.xp_regwrite 'HKEY_LOCAL_MACHINE', 'SOFTWARE\Microsoft\MSSQLServer\SQLExecutive', 'TaskHistoryMaxRows', 'REG_DWORD', @limithistoryrowspertaskmax
go
/* End sp_MSsetexecinfo */

/*-- Another SQL60-only proc ---------------------------------------------------*/
print ''
print 'Creating sp_MStablechecks'
print ''
go

create procedure sp_MStablechecks
	@tablename varchar(92), @flags int = null
as
	declare @id int
	select @id = object_id(@tablename)
	if (@id is null) begin
		RAISERROR (15001, -1, -1, @tablename)
		return 1
	end

	/* @flags is for daVinci. */
	if (@flags is null)
		select @flags = 0

	/* We'll put out the check text if it's all in one row (most likely); otherwise leave it */
	/* blank for refetching in its entirety via sp_helptext, unless @flags wants it anyway. */
	select object_name(t.id), 
		case when (@flags & 1 <> 0 or not exists (select * from syscomments where id = t.id and colid = 2)) 
				then t.text else null end,
		c.status & (convert(int, 0x00200000) | convert(int, 0x00020000) | convert(int, 0x00004000))
	from syscomments t, sysconstraints c
	where t.id = c.constid and c.id = @id and c.status & 0x0f = 4
		and (@flags & 1 <> 0 or t.colid = 1)
	order by object_name(t.id), t.colid
go
/* End sp_MStablechecks */

/*-- Another SQL60-only proc ---------------------------------------------------*/
print ''
print 'Creating sp_MStablerefs'
print ''
go

create procedure sp_MStablerefs
	@tablename varchar(92),					/* table whose references are being evaluated */
	@type varchar(20) = 'actualtables',		/* '[actual | all][tables | keys | keycols]'; all candidates, or only those actually referenced */
	@direction varchar(20) = 'primary',		/* look for references from @tablename to 'primary' table(s), or to @tablename from 'foreign' table(s) */
	@reftable varchar(92) = null			/* limit scope to this table, if non-null */
as
	if (@tablename = '?') begin
		PRINT ''
		PRINT 'sp_MStablerefs:'
		PRINT '@tablename varchar(92),					/* table whose references are being evaluated */'
		PRINT '@type varchar(92) = [actualtables],		/* [[actual | all][tables | keys | keycols]]; all candidates, or only those actually referenced */'
		PRINT '@direction varchar(20) = [primary],		/* look for references from @tablename to [primary] or to @tablename from [foreign], or [both] */'
		PRINT '@reftable varchar(92) = null				/* limit scope to this table, if non-null */'
		return 0
	end

	if (lower(@direction) = 'both') begin
		select
			"PK_Table" = PKT.name,
			"FK_Table" = FKT.name,
			"Constraint" = object_name(r.constid),
			c.status,
			cKeyCol1 = convert(varchar(32), col_name(r.fkeyid, r.fkey1)), 
			cKeyCol2 = convert(varchar(32), col_name(r.fkeyid, r.fkey2)),  
			cKeyCol3 = convert(varchar(32), col_name(r.fkeyid, r.fkey3)),  
			cKeyCol4 = convert(varchar(32), col_name(r.fkeyid, r.fkey4)), 
			cKeyCol5 = convert(varchar(32), col_name(r.fkeyid, r.fkey5)),  
			cKeyCol6 = convert(varchar(32), col_name(r.fkeyid, r.fkey6)),  
			cKeyCol7 = convert(varchar(32), col_name(r.fkeyid, r.fkey7)),  
			cKeyCol8 = convert(varchar(32), col_name(r.fkeyid, r.fkey8)), 
			cKeyCol9 = convert(varchar(32), col_name(r.fkeyid, r.fkey9)),  
			cKeyCol10 = convert(varchar(32), col_name(r.fkeyid, r.fkey10)), 
			cKeyCol11 = convert(varchar(32), col_name(r.fkeyid, r.fkey11)),
			cKeyCol12 = convert(varchar(32), col_name(r.fkeyid, r.fkey12)), 
			cKeyCol13 = convert(varchar(32), col_name(r.fkeyid, r.fkey13)), 
			cKeyCol14 = convert(varchar(32), col_name(r.fkeyid, r.fkey14)), 
			cKeyCol15 = convert(varchar(32), col_name(r.fkeyid, r.fkey15)),  
			cKeyCol16 = convert(varchar(32), col_name(r.fkeyid, r.fkey16)),
			cRefCol1 = convert(varchar(32), col_name(r.rkeyid, r.rkey1)), 
			cRefCol2 = convert(varchar(32), col_name(r.rkeyid, r.rkey2)),	
			cRefCol3 = convert(varchar(32), col_name(r.rkeyid, r.rkey3)), 
			cRefCol4 = convert(varchar(32), col_name(r.rkeyid, r.rkey4)),
			cRefCol5 = convert(varchar(32), col_name(r.rkeyid, r.rkey5)), 
			cRefCol6 = convert(varchar(32), col_name(r.rkeyid, r.rkey6)), 
			cRefCol7 = convert(varchar(32), col_name(r.rkeyid, r.rkey7)), 
			cRefCol8 = convert(varchar(32), col_name(r.rkeyid, r.rkey8)),
			cRefCol9 = convert(varchar(32), col_name(r.rkeyid, r.rkey9)), 
			cRefCol10 = convert(varchar(32), col_name(r.rkeyid, r.rkey10)), 
			cRefCol11 = convert(varchar(32), col_name(r.rkeyid, r.rkey11)), 
			cRefCol12 = convert(varchar(32), col_name(r.rkeyid, r.rkey12)),
			cRefCol13 = convert(varchar(32), col_name(r.rkeyid, r.rkey13)), 
			cRefCol14 = convert(varchar(32), col_name(r.rkeyid, r.rkey14)), 
			cRefCol15 = convert(varchar(32), col_name(r.rkeyid, r.rkey15)), 
			cRefCol16 = convert(varchar(32), col_name(r.rkeyid, r.rkey16)),
			"PK_Table_Owner" = user_name(PKT.uid),
			"FK_Table_Owner" = user_name(FKT.uid)
		from sysreferences r, sysconstraints c, sysobjects PKT, sysobjects FKT
		where r.constid = c.constid and (@tablename is null or 
			(r.rkeyid = object_id(@tablename) or r.fkeyid = object_id(@tablename)))
		and PKT.id = r.rkeyid and FKT.id = r.fkeyid
		return 0
	end /* @direction = 'both' */

	declare @id int, @refid int
	select @id = object_id(@tablename), @refid = object_id(@reftable)
	if (@tablename is not null and @id is null) begin
		RAISERROR (15001, -1, -1, @tablename)
		return 1
	end
	if (@reftable is not null and @refid is null) begin
		RAISERROR (15001, -1, -1, @reftable)
		return 1
	end

	create table #sprefs (
		id					int				NOT NULL, 	/* id of reftable */
		constid				int				NULL, 		/* id of key */
		referenced			bit				NOT NULL	/* well, is it? */
	)

	declare @dotables bit, @doall bit, @doprimary bit, @docols bit
	select 	@dotables = case when (@type like 'allt%' or @type like 'actualt%') then 1 else 0 end,
			@doall = case when (@type like 'all%') then 1 else 0 end,
			@doprimary = case when (@direction like 'p%') then 1 else 0 end,
			@docols = case when (@type like '%keycol%') then 1 else 0 end

	/* If a specific @tablename specified, see if it has the kind of keys we want. */
	/* If asking for references from @tablename to 'primary', we must have an FKEY; */
	/* if asking for references to @tablename from 'foreign', we must have an active REFerence. */
	if (@id is not null) begin
		declare @wantkeytype int
		select @wantkeytype = case @doprimary when 1 then 0x4 else 0x8 end
		if not exists (select * from sysobjects where id = @id and category & @wantkeytype <> 0)
			goto ReturnSet
	end

	if (@dotables = 1) begin
		if (@doprimary = 1) begin
			/* Get all candidate tables (those with Primary/Unique keys in sysconstraints). */
			insert #sprefs
				select distinct id, null, 0 from sysconstraints where status & 0x0f in (1, 2)

			/* Update the referenced bit if this table references it. */
			update #sprefs set referenced = 1
				where id in (select rkeyid from sysreferences where fkeyid = @id)
		end else begin
			/* All user tables are foreign-key candidate tables. */
			insert #sprefs
				select distinct id, null, 0 from sysobjects where sysstat & 0x0f = 3 

			/* Update the referenced bit if it references this table. */
			update #sprefs set referenced = 1
				where id in (select fkeyid from sysreferences where rkeyid = @id)
		end	/* direction */

	end else begin	/* keys */
		if (@doprimary = 1) begin
			/* Get all candidate tables (those with Primary/Unique keys in sysconstraints) and the keys. */
			insert #sprefs
				select distinct id, constid, 0 from sysconstraints where status & 0x0f in (1, 2)

			/* Follow r.rkeyindid back to sysindexes to get the name and then 'rconstid' to see if this table references it. */
			update #sprefs set referenced = 1 from #sprefs s, sysreferences r, sysindexes i
				where r.fkeyid = @id
				and i.id = r.rkeyid and i.indid = r.rkeyindid and i.status & 0x1800 <> 0
				and s.constid = object_id(i.name)

		end else begin
			/* First add tables with FOREIGN keys defined. */
			insert #sprefs
				select distinct id, constid, 0 from sysconstraints where status & 0x0f in (3)

			/* All user tables are foreign-key candidate tables, so add any tables we haven't yet, if @doall. */
			/* (This would be used for 'push' key definition; defining FK's from the standpoint of the PK table). */
			insert #sprefs
				select distinct id, null, 0 from sysobjects where sysstat & 0x0f = 3 
					and @doall = 1 and id not in (select id from #sprefs)

			/* Update the referenced bit if it references this table. */
			update #sprefs set referenced = 1
				where constid in (select constid from sysreferences where rkeyid = @id)
		end	/* direction */
	end	/* tables or keys */
	
	/* Exclude system and MS-internal objects, or tables/keys that aren't in the @reftable we want, if any specified. */
	delete #sprefs where id in (select id from sysobjects where sysstat & 0x0f <> 3 or category & 0x0002 <> 0)
			or (@refid is not null and id != @refid)

	/* Output */
ReturnSet:
	if (@docols = 0) begin
		if (@tablename is not null)
			select candidate_table = user_name(o.uid) + '.' + object_name(o.id), 
				candidate_key = case @dotables when 1 then 'N/A' else object_name(s.constid) end, s.referenced
				from #sprefs s, sysobjects o where o.id = s.id and (@doall = 1 or s.referenced = 1)
				order by object_name(o.id), user_name(o.uid), object_name(s.constid)
		else
			select candidate_table = user_name(o.uid) + '.' + object_name(o.id), 
				candidate_key = case @dotables when 1 then 'N/A' else object_name(s.constid) end
				from #sprefs s, sysobjects o where o.id = s.id
				order by object_name(o.id), user_name(o.uid), object_name(s.constid)
	end else begin	/* @docols = 1 */
		/* This is currently just implemented for 'nonNULLtablename', 'actualkeycols', 'foreign'. */
		select candidate_table = user_name(o.uid) + '.' + object_name(o.id), 
				candidate_key = object_name(s.constid),
				cKeyCol1 = convert(varchar(32), col_name(r.fkeyid, r.fkey1)), 
				cKeyCol2 = convert(varchar(32), col_name(r.fkeyid, r.fkey2)),  
				cKeyCol3 = convert(varchar(32), col_name(r.fkeyid, r.fkey3)),  
				cKeyCol4 = convert(varchar(32), col_name(r.fkeyid, r.fkey4)), 
				cKeyCol5 = convert(varchar(32), col_name(r.fkeyid, r.fkey5)),  
				cKeyCol6 = convert(varchar(32), col_name(r.fkeyid, r.fkey6)),  
				cKeyCol7 = convert(varchar(32), col_name(r.fkeyid, r.fkey7)),  
				cKeyCol8 = convert(varchar(32), col_name(r.fkeyid, r.fkey8)), 
				cKeyCol9 = convert(varchar(32), col_name(r.fkeyid, r.fkey9)),  
				cKeyCol10 = convert(varchar(32), col_name(r.fkeyid, r.fkey10)), 
				cKeyCol11 = convert(varchar(32), col_name(r.fkeyid, r.fkey11)),
				cKeyCol12 = convert(varchar(32), col_name(r.fkeyid, r.fkey12)), 
				cKeyCol13 = convert(varchar(32), col_name(r.fkeyid, r.fkey13)), 
				cKeyCol14 = convert(varchar(32), col_name(r.fkeyid, r.fkey14)), 
				cKeyCol15 = convert(varchar(32), col_name(r.fkeyid, r.fkey15)),  
				cKeyCol16 = convert(varchar(32), col_name(r.fkeyid, r.fkey16))
			from #sprefs s, sysobjects o, sysreferences r
			where o.id = s.id and r.constid = s.constid and s.referenced = 1
			order by object_name(o.id), user_name(o.uid), object_name(s.constid)
	end
go
/* End sp_MStablerefs */

/*-- Another SQL60-only proc ---------------------------------------------------*/
print ''
print 'Creating sp_MSsettopology'
print ''
go

/* Need this because it will set sysservers columns. */
sp_configure 'allow updates', 1
go
reconfigure with override
go

create procedure sp_MSsettopology
	@server varchar(30), @X int, @Y int
as
	update master..sysservers set topologyx = @X, topologyy = @Y
		where srvname = @server
	if (@@rowcount = 0) begin
		RAISERROR (15015, -1, -1, @server)
		return 1
	end
	return 0
go
/* End sp_MSsettopology */

sp_configure 'allow updates', 0
go
reconfigure with override
go

/*-- Another SQL60-only proc ---------------------------------------------------*/
print ''
print 'Creating sp_MSfilterclause'
print ''
go

create procedure sp_MSfilterclause
	@publication varchar(30), @article varchar(30)
as
	/* Return a text column as multiple readtexts of maxcol length */
	declare @pubid int, @artid int
	select @pubid = pubid from syspublications where name = @publication
	if (@pubid is null) begin
		RAISERROR (15001, 11, -1, @publication)
		return 1
	end
	select @artid = artid from sysarticles where name = @article and pubid = @pubid
	if (@artid is null) begin
		RAISERROR (15001, 11, -1, @article)
		return 1
	end

	declare @val varbinary(16), @len int, @ii int, @chunk int
	select @val = textptr(filter_clause), @len = datalength(filter_clause) from sysarticles where artid = @artid and pubid = @pubid
	select @ii = 0, @chunk = 255

	/* Get all the rows of an maxcol size */
	while @len > @chunk begin
		readtext sysarticles.filter_clause @val @ii @chunk
		select @ii = @ii + @chunk, @len = @len - @chunk
	end

	/* Get the last chunk */
	if (@len > 0)
		readtext sysarticles.filter_clause @val @ii @len
	return 0
go
/* End sp_MSfilterclause */

/*-- Another SQL60-only proc ---------------------------------------------------*/
print ''
print 'Creating sp_MSmatchkey'
print ''
go

create proc sp_MSmatchkey
	@tablename varchar(92),
	@col1 varchar(30),
	@col2 varchar(30) = null,
	@col3 varchar(30) = null,
	@col4 varchar(30) = null,
	@col5 varchar(30) = null,
	@col6 varchar(30) = null,
	@col7 varchar(30) = null,
	@col8 varchar(30) = null,
	@col9 varchar(30) = null,
	@col10 varchar(30) = null,
	@col11 varchar(30) = null,
	@col12 varchar(30) = null,
	@col13 varchar(30) = null,
	@col14 varchar(30) = null,
	@col15 varchar(30) = null,
	@col16 varchar(30) = null
as
	declare @id int, @ii int, @colnotfound varchar(30), @keycnt int
	select @id = object_id(@tablename)
	if (@id is null) begin
		RAISERROR (15001, -1, -1, @tablename)
		return 1
	end
	select @ii = 1
	create table #t1 (		/* Join into this... */
		i					int				NOT NULL, 
		name				varchar(30)		NULL
	)
	insert #t1 values (1, @col1)
	insert #t1 values (2, @col2)
	insert #t1 values (3, @col3)
	insert #t1 values (4, @col4)
	insert #t1 values (5, @col5)
	insert #t1 values (6, @col6)
	insert #t1 values (7, @col7)
	insert #t1 values (8, @col8)
	insert #t1 values (9, @col9)
	insert #t1 values (10, @col10)
	insert #t1 values (11, @col11)
	insert #t1 values (12, @col12)
	insert #t1 values (13, @col13)
	insert #t1 values (14, @col14)
	insert #t1 values (15, @col15)
	insert #t1 values (16, @col16)
	delete #t1 where name is null

	select @colnotfound = min(name) from #t1 where name not in (select name from syscolumns where id = @id)
	if (@colnotfound is not null) begin
		RAISERROR (15253, -1, -1, @colnotfound, @tablename)
		return 1
	end
	select @ii = 1, @keycnt = count(*) from #t1

	/* Load all indexes which have the matching number of columns into a temp table, then eliminate those which don't qualify. */
	/* Remember the RID in the nc index is counted as a key */
	create table #i1 (
		i					int				NOT NULL
	)
	insert #i1 select indid from sysindexes where status & 0x1800 <> 0
		and id = @id and keycnt - (case indid when 1 then 0 else 1 end) = @keycnt
	while (@ii <= @keycnt) begin
		delete #i1 from #i1 i, #t1 t where t.i = @ii and index_col(@tablename, i.i, t.i) <> t.name
		select @ii = @ii + 1
	end

	/* The qualifying key will be the lowest indid (or the ONLY indid, if we disallow duplicate indexes), if any remain. */
	select name from sysindexes where id = @id and indid = (select min(i) from #i1)
go
/* End sp_MSmatchkey */

/*-- Another SQL60-only proc ---------------------------------------------------*/
print ''
print 'Creating sp_MSsubscriptions'
print ''
go

create procedure sp_MSsubscriptions
	@subscriber varchar(30) = '%', @dbname varchar(30) = '%', @mode varchar(10) = null
as
	if (@subscriber is null)
		select @subscriber = '%'
	if (@dbname is null)
		select @dbname = '%'

	/* Similar to sp_helpsubscription but intended for SQLOLE's EnumSubscriberSubscriptions, to report on the full */
	/* server so it needs to return the publication databasename as well, and all subscriptions, not just those active. */
	IF (@subscriber <> '%' and not exists 
			(SELECT * FROM sysservers WHERE srvname = @subscriber AND (srvstatus & 0x0004) <> 0)) begin
		RAISERROR (14010, -1, -1)
		return 1
	end
	IF (@dbname <> '%' and db_id(@dbname) is null) begin
		RAISERROR (14211, -1, -1, @dbname)
		return 1
	end
	if (@mode = 'drop' and suser_id() <> 1) begin
		RAISERROR (15003, -1, -1)
		return 1
	end

	create table #t1 (
		i					int					NOT NULL IDENTITY PRIMARY KEY, 
		cSubscriber			varchar(30)			NOT NULL, 
		cPubDBName			varchar(30)			NOT NULL, 
		cPublication		varchar(60)			NOT NULL, 
		cArticle			varchar(60)			NOT NULL
	)

	/* Preprocessor won't replace within quotes so have to use str(). */
	declare @pubbit varchar(12)
	select @pubbit = ltrim(str(convert(int, 0x0001)))
	exec ('declare hC_mssub cursor for select name from master..sysdatabases where category & ' + @pubbit + ' <> 0 and name like ''' + @dbname + '''')
	open hC_mssub
	fetch hC_mssub into @dbname
	while (@@fetch_status >= 0) begin
		exec ('insert #t1 select ss.srvname, ''' + @dbname + ''', pub.name, art.name from ' +
				@dbname + '..syssubscriptions sub, ' +
				@dbname + '..syspublications pub, ' +
				@dbname + '..sysarticles art, ' +
				'master..sysservers ss where ss.srvname like ''' + @subscriber + ''' and sub.srvid = ss.srvid and art.pubid = pub.pubid and sub.artid = art.artid')
		fetch hC_mssub into @dbname
	end
	close hC_mssub
	deallocate hC_mssub

	if (@mode = 'drop') begin
		declare @pubname varchar(60), @artname varchar(60)
		exec ('declare hC_mssub cursor for select cSubscriber, cPubDBName, cPublication, cArticle from #t1')
		open hC_mssub
		fetch hC_mssub into @subscriber, @dbname, @pubname, @artname

		/* Would be nice to do this in a transaction but schema modifications in transaction aren't supported. */
		while (@@fetch_status >= 0) begin
			exec (@dbname + '..sp_dropsubscription ''' + @pubname + ''', ''' + @artname + ''', ''' + @subscriber + '''')
			fetch hC_mssub into @subscriber, @dbname, @pubname, @artname
		end
		close hC_mssub
		deallocate hC_mssub
	end else begin
		select cSubscriber, cPubDBName, cPublication, cArticle from #t1 order by cSubscriber, cPubDBName, cPublication, cArticle
	end
	drop table #t1
go
/* End sp_MSsubscriptions */

/*-- Another SQL60-only proc ---------------------------------------------------*/
print ''
print 'Creating sp_MSforeach_worker'
print ''
go

/* 
 * This is the worker proc for all of the "for each" type procs.  Its function is to read the
 * next replacement name from the cursor (which returns only a single name), plug it into the
 * replacement locations for the commands, and execute them.  It assumes the cursor "hCForEach"
 * has already been opened by its caller.
 */
create proc sp_MSforeach_worker
	@command1 varchar(255), @replacechar char(1) = '?', @command2 varchar(255) = null, @command3 varchar(255) = null
as
	set nocount on
	declare @name varchar(100), @namelen int, @q1 varchar(255), @q2 varchar(255), @q3 varchar(255), @q4 varchar(255), @q5 varchar(255)
	declare @q6 varchar(255), @q7 varchar(255), @q8 varchar(255), @q9 varchar(255), @q10 varchar(255)
	declare @cmd varchar(255), @replacecharindex int, @useq tinyint, @usecmd tinyint, @nextcmd varchar(255)
	create table #qtemp (	/* Temp command storage */
		qnum				int				NOT NULL, 
		qchar				varchar(255)	NULL
	)
	open hCForEach
	fetch hCForEach into @name

	/* Loop for each database */
	while (@@fetch_status >= 0) begin
		/* Initialize. */
		select @useq = 1, @usecmd = 1, @cmd = @command1, @namelen = datalength(@name)
		while (@cmd is not null) begin		/* Generate @q* for exec() */
			/* 
			 * Parse each @commandX into a single executable batch.
			 * Because the expanded form of a @commandX may be > 255, we'll need to allow overflow.
			 * We also may append @commandX's (signified by '++' as first letters of next @command).
			 */
			select @replacecharindex = charindex(@replacechar, @cmd)
			while (@replacecharindex <> 0) begin
				if (datalength(@cmd) + @namelen - 1 > 255) begin
					/* Overflow; put preceding stuff into the temp table */
					if (@useq > 9) begin
						raiserror 55555 'sp_MSforeach_worker assert failed:  command too long'
						close hCForEach
						deallocate hCForEach
						return 1
					end
					if (@replacecharindex < @namelen) begin
						/* If this happened close to beginning, make sure expansion has enough room. */
						/* In this case no trailing space can occur as the row ends with @name. */
						select @nextcmd = substring(@cmd, 1, @replacecharindex)
						select @cmd = substring(@cmd, @replacecharindex + 1, 255)
						select @nextcmd = stuff(@nextcmd, @replacecharindex, 1, @name)
						select @replacecharindex = charindex(@replacechar, @cmd)
						insert #qtemp values (@useq, @nextcmd)
						select @useq = @useq + 1
						continue
					end
					/* Move the string down and stuff() in-place. */
					/* Because varchar columns trim trailing spaces, we may need to prepend one to the following string. */
					/* In this case, the char to be replaced is moved over by one. */
					insert #qtemp values (@useq, substring(@cmd, 1, @replacecharindex - 1))
					if (substring(@cmd, @replacecharindex - 1, 1) = ' ') begin
						select @cmd = ' ' + substring(@cmd, @replacecharindex, 255)
						select @replacecharindex = 2
					end else begin
						select @cmd = substring(@cmd, @replacecharindex, 255)
						select @replacecharindex = 1
					end
					select @useq = @useq + 1
				end
				select @cmd = stuff(@cmd, @replacecharindex, 1, @name)
				select @replacecharindex = charindex(@replacechar, @cmd)
			end

			/* Done replacing for current @cmd.  Get the next one and see if it's to be appended. */
			select @usecmd = @usecmd + 1
			select @nextcmd = case (@usecmd) when 2 then @command2 when 3 then @command3 else null end
			if (@nextcmd is not null and substring(@nextcmd, 1, 2) = '++') begin
				insert #qtemp values (@useq, @cmd)
				select @cmd = substring(@nextcmd, 3, 255), @useq = @useq + 1
				continue
			end

			/* Now exec() the generated @q*, and see if we had more commands to exec().  Continue even if errors. */
			/* Null them first as the no-result-set case won't. */
			select @q1 = null, @q2 = null, @q3 = null, @q4 = null, @q5 = null, @q6 = null, @q7 = null, @q8 = null, @q9 = null, @q10 = null
			select @q1 = qchar from #qtemp where qnum = 1
			select @q2 = qchar from #qtemp where qnum = 2
			select @q3 = qchar from #qtemp where qnum = 3
			select @q4 = qchar from #qtemp where qnum = 4
			select @q5 = qchar from #qtemp where qnum = 5
			select @q6 = qchar from #qtemp where qnum = 6
			select @q7 = qchar from #qtemp where qnum = 7
			select @q8 = qchar from #qtemp where qnum = 8
			select @q9 = qchar from #qtemp where qnum = 9
			select @q10 = qchar from #qtemp where qnum = 10
			truncate table #qtemp
			exec (@q1 + @q2 + @q3 + @q4 + @q5 + @q6 + @q7 + @q8 + @q9 + @q10 + @cmd)
			select @cmd = @nextcmd, @useq = 1
		end /* while @cmd is not null, generating @q* for exec() */

		/* All commands done for this name.  Go to next one. */
		fetch hCForEach into @name
	end /* while FETCH_SUCCESS */
	close hCForEach
	deallocate hCForEach
	return 0
go

/* End sp_MSforeach_worker */

/*-- Another SQL60-only proc ---------------------------------------------------*/
print ''
print 'Creating sp_MSenumsubsystems'
print ''
go

create procedure sp_MSenumsubsystems 
as
begin
	if (@@microsoftversion >= 0x000632c0) begin
		set nocount on
		create table #ss_temp (Value varchar(255), Data varchar(255))
		if (suser_id() = 1)
		begin
			-- Need to exec() as this will otherwise fail the parser in 6.0.
			exec ('insert into #ss_temp exec master..xp_regenumvalues ''HKEY_LOCAL_MACHINE'', ''SOFTWARE\Microsoft\MSSQLServer\SQLExecutive\Subsystems''')
			update #ss_temp set Data = 'Command-Line SubSystem' where Value = 'CmdExec'
			update #ss_temp set Data = 'Replication Synchronization SubSystem' where Value = 'Sync'
			update #ss_temp set Data = 'Replication Transaction-Log Reader SubSystem' where Value = 'LogReader'
			update #ss_temp set Data = 'Replication Distribution SubSystem' where Value = 'Distribution'
		end
		select Value = 'TSQL', Data = 'Transact-SQL SubSystem'
		union 
		select Value = 'CmdExec', Data = 'Command-Line SubSystem' 
		union 
		select Value, Data
		from #ss_temp
	end else begin
		-- For a 6.0 server use the same hardcoded command SQLOLE did.
		select Value = convert(varchar(255),'TSQL'), 
			Data = convert(varchar(255),'Transact-SQL Executive SubSystem')
		if suser_id() = 1 
			exec xp_regenumvalues 'HKEY_LOCAL_MACHINE', 'SOFTWARE\Microsoft\MSSQLServer\SQLExecutive\SubSystems'
	end
end
go

/* End sp_MSenumsubsystems */

/*-- Another SQL60-only proc ---------------------------------------------------*/
print ''
print 'Creating sp_MSforeachdb'
print ''
go

/* 
 * The following table definition will be created by SQLOLE at start of each connection.
 * We don't create it here temporarily because we need it in Exec() or upgrade won't work.
 *
 * 		create table #SQLOLEDbUserProfile (dbid int NOT NULL PRIMARY KEY, profilebits int NOT NULL)
 */

create proc sp_MSforeachdb
	@command1 varchar(255), @replacechar char(1) = '?', @command2 varchar(255) = null, @command3 varchar(255) = null,
	@precommand varchar(255) = null, @postcommand varchar(255) = null
as
	/* This proc returns one or more rows for each accessible db, with each db defaulting to its own result set */
	/* @precommand and @postcommand may be used to force a single result set via a temp table. */

	/* Preprocessor won't replace within quotes so have to use str(). */
	declare @inaccessible varchar(12), @invalidlogin varchar(12), @dbinaccessible varchar(12)
	select @inaccessible = ltrim(str(convert(int, 0x03e0)))
	select @invalidlogin = ltrim(str(convert(int, 0x40000000)))
	select @dbinaccessible = '0x80000000'		/* SQLOLEDbUserProf_InaccessibleDb; the negative number doesn't work in convert() */

	/* If it's a single user db and there's an entry for it in sysprocesses who isn't us, we can't use it. */
	declare @notsingle varchar(255), @dbstat_singlestr varchar(12)
	select @dbstat_singlestr = ltrim(str(convert(int, 0x1000)))

	if (@precommand is not null)
		exec(@precommand)

	declare @origdb varchar(30)
	select @origdb = db_name()

	/* Create the select */
	exec('declare hCForEach cursor for select name from master..sysdatabases d, #SQLOLEDbUserProfile p ' +
			' where (d.status & ' + @inaccessible + ' = 0) and (d.dbid = p.dbid)' +
			' and (p.profilebits & ' + @dbinaccessible + ' = 0)' + 
			' and (p.profilebits & ' + @invalidlogin + ' = 0)' + 
			' and (d.status & ' + @dbstat_singlestr + ' = 0 or not exists ' + 
			' (select * from master..sysprocesses p where dbid = d.dbid and p.spid <> @@spid))' )
	declare @retval int
	select @retval = @@error
	if (@retval = 0)
		exec @retval = sp_MSforeach_worker @command1, @replacechar, @command2, @command3

	if (@retval = 0 and @postcommand is not null)
		exec(@postcommand)

	exec ('use ' + @origdb)
	return @retval
go
/* End sp_MSforeachdb */

/*-- Another SQL60-only proc ---------------------------------------------------*/
print ''
print 'Creating sp_MSforeachtable'
print ''
go

create proc sp_MSforeachtable
	@command1 varchar(255), @replacechar char(1) = '?', @command2 varchar(255) = null, @command3 varchar(255) = null, @whereand varchar(255) = null,
	@precommand varchar(255) = null, @postcommand varchar(255) = null
as
	/* This proc returns one or more rows for each table (optionally, matching @where), with each table defaulting to its own result set */
	/* @precommand and @postcommand may be used to force a single result set via a temp table. */

	/* Preprocessor won't replace within quotes so have to use str(). */
	declare @mscat varchar(12), @objtypebits varchar(12), @usertablestat varchar(12)
	select @mscat = ltrim(str(convert(int, 0x0002)))
	select @objtypebits = ltrim(str(convert(int, 0x0f)))
	select @usertablestat = ltrim(str(convert(int, 3)))

	if (@precommand is not null)
		exec(@precommand)

	/* Create the select */
	exec('declare hCForEach cursor for select user_name(uid) + ''.'' + object_name(id) from sysobjects o '
			+ ' where o.sysstat & ' + @objtypebits + ' = ' + @usertablestat + ' and o.category & ' + @mscat + ' = 0 '
			+ @whereand)
	declare @retval int
	select @retval = @@error
	if (@retval = 0)
		exec @retval = sp_MSforeach_worker @command1, @replacechar, @command2, @command3

	if (@retval = 0 and @postcommand is not null)
		exec(@postcommand)

	return @retval
go
/* End sp_MSforeachtable */



/*******************************************************************************/

print ''
print 'Creating sp_MSloginmappings'
print ''
go



create proc sp_MSloginmappings
	@loginname varchar(30) = null, @flags int = 0
as
	/*
	 * @flags bits:
	 *		0x01	- current db only
	 */
	/* 
	 * Added @dbname so dbo can see everyone in current database.
	 * Use hacky 4.21 syntax so it will run there, instead of a case..when.
	 */
	
	declare @checkmultilogin int
	select @checkmultilogin = 1
	if ((@flags & 0x01 <> 0) and user_id() = 1) 
		select @checkmultilogin = 0

	declare @logincount int
	select @logincount = 0
	if (@loginname is not null)
		select @logincount = count(*) from syslogins where name like @loginname

	/* Gotta be sa or dbo to see other than just current login. */
	declare @numlogins int, @whereloginname varchar(100), @name varchar(30), @retval int
	if (@loginname is null)
		select @numlogins = 2
	else
		select @numlogins = count(*) from syslogins where name like @loginname

	if (@numlogins = 0) begin
		
		RAISERROR (14220, -1, -1, @loginname)		/* Login not found */
		return 1
	end
	if (@checkmultilogin <> 0) begin
		if (suser_id() <> 1 and (@numlogins > 1 or suser_id() <> suser_id(@loginname))) begin
			
			RAISERROR (14301, -1, -1, '')				/* Only sa can see other than the current login */
			return 1
		end
	end
	if (@loginname is not null)
		select @whereloginname = ' where name like ''' + @loginname + ''''

	/*
	 * This proc returns a result set with one or more rows for each database for which a login is a user or aliased to one.
	 * If loginname is specified, the results are limited to that login.  First load a temp table with all logins that are
	 * in a db, then add those which aren't mapped to any db.
	 */
	create table #loginmappings(
		LoginName			varchar(30)		NOT NULL, 
		DBName				varchar(30)		NULL, 
		UserName			varchar(30)		NULL, 
		AliasName			varchar(30)		NULL
	)
	if (@flags & 0x01 <> 0) begin
		INSERT #loginmappings select l.name, db_name(), u.name, null from master..syslogins l, sysusers u where l.suid = u.suid
		UNION select l.name, db_name(), null, u.name from master..syslogins l, sysusers u, sysalternates a where l.suid = a.suid and u.suid = a.altsuid

		/* 
		 * We only allow multi-db on a 6.x server because dynamic exec() didn't exist before then,
		 * hence there is no way to loop thru every database.  This is caught in SQLOLE so no
		 * need for error message here; we'll just return no result sets.
		 */
	end else begin
		exec @retval = sp_MSforeachdb 
			'use ? INSERT #loginmappings select l.name, db_name(), u.name, null from master..syslogins l, sysusers u where l.suid = u.suid',
			'?',
			'++ UNION select l.name, db_name(), null, u.name from master..syslogins l, sysusers u, sysalternates a where l.suid = a.suid and u.suid = a.altsuid'
		if (@retval <> 0)
			return 1
		insert #loginmappings select l.name, null, null, null from master..syslogins l where l.name not in (select LoginName from #loginmappings)

	end

	/* 
	 * Now bring them out by loginname, each in its own result set.
	 * If this is for all logins, we'll return all logins; if for curdb, 
	 * only those in #loginmappings (i.e. only those mapped in curdb).
	 */

	exec('declare hCForEachLogin cursor for select name from master..syslogins ' + @whereloginname + ' order by name')
	if (@@error = 0)
		open hCForEachLogin
	if (@@error <> 0)
		return @@error
	fetch hCForEachLogin into @name
	while (@@fetch_status >= 0) begin
		if ((@flags & 0x01 = 0) or exists (select * from #loginmappings where LoginName = @name))
			select * from #loginmappings where LoginName = @name
		fetch hCForEachLogin into @name
	end /* FETCH_SUCCESS */
	close hCForEachLogin
	deallocate hCForEachLogin
	return @@error


















go
/* End sp_MSloginmappings */

/*******************************************************************************/
print ''
print 'Creating sp_MSuniquename'
print ''
go

create procedure sp_MSuniquename 
	@seed varchar(30), @start int = null
as
	/* Return a unique name for sysobjects, based on a passed-in seed. */
	set nocount on
	declare @i int, @append varchar(10), @seedlen int, @temp varchar(30), @recalcseedlen int, @seedcharlen int
	select @i = 1, @seedlen = datalength(@seed), @recalcseedlen = 1, @seedcharlen = 0
	if (@start is not null and @start >= 0)
		select @i = @start
	while 1 < 2
	begin
		/* This is probably overkill, but start at max length of seed name, leaving room under SYSNAMELEN for @append. */
		/* We'll work our way back along the string if more room needed (pathological user). */
		select @append = ltrim(str(@i)) + '__' + ltrim(str(@@spid))
		if (@recalcseedlen = @i or @seedcharlen = 0)
		begin
			while @recalcseedlen <= @i
				select @recalcseedlen = @recalcseedlen * 10
			select @seedcharlen = @seedlen
			if ((@seedlen + datalength(@append)) > 30) begin
				select @seedlen = 30 - datalength(@append)

				/* For 6.5, get the charlen of this datalength for the substring() call. */
				select @seedcharlen = @seedlen

				if (@@microsoftversion >= 0x000632c0) begin
					exec sp_GetMBCSCharLen @seed, @seedlen, @seedcharlen out
				end		/* MBCS length */

			end		/* Recalc seedlen */
		end		/* Check seedlen */

		select @temp = substring(@seed, 1, @seedcharlen) + @append

		/* If I don't set a limit somewhere, it's gonna look hung -- I'd rather get a nonunique error. */
		if object_id(@temp) is null or @i > 999999		/* if increased, watch out for overflow of @recalcseedlen */
		begin
			set nocount off
			select Name = @temp, Next = @i + 1
			return 0
		end
		select @i = @i + 1
	end
go
/* End sp_MSuniquename */

/*******************************************************************************/
print ''
print 'Creating sp_MSkilldb'
print ''
go

sp_configure updat, 1
go
reconfigure with override
go

create proc sp_MSkilldb
	@dbname varchar(30)
as
	
	if (@@trancount > 0) begin
		
		RAISERROR (15002, -1, -1, 'sp_MSkilldb')
		return 1
	end

	if (suser_id() <> 1) begin
		
		RAISERROR (15003, -1, -1, '')
		return 1
	end

	/* Set this db to suspect, then let dbcc dbrepair kill it for us. */
	update master..sysdatabases set status = status | 0x0100
		where name = @dbname
	if (@@rowcount = 0) begin
		
		RAISERROR (14211, -1, -1, @dbname)
		return 1
	end
	dbcc dbrepair(@dbname, dropdb)
	return 0
go

sp_configure updat, 0
go
reconfigure with override
go

/* End sp_MSkilldb */

/*******************************************************************************/
print ''
print 'Creating sp_MSdbuserprofile'
print ''
go

create proc sp_MSdbuserprofile
	@mode varchar(10) = 'init', @qual varchar(30) = '%'
as

/* Order of privilege evaluation is:  user granted/revoked, then group granted/revoked, then public granted/revoked */







	set nocount on

	/* If 'srv', we're selecting the server (master db) user profile - currently, just create db priv. */
	declare @bits int, @gid int, @prot int
	if (lower(@mode) like 'serv%') begin
		select @bits = 0x0000
		if (suser_id() = 1) begin
			/* sa has everything */
			select @bits = 0x0007
		end else begin
			select @prot = null select @prot = protecttype from sysprotects where action = 203 and uid = user_id()
			if (@prot is null) select @prot = protecttype from sysprotects where action = 203 and uid = (select gid from sysusers where uid = user_id())
			if (@prot is null) select @prot = protecttype from sysprotects where action = 203 and uid = user_id('public')
			if (@prot = 205) select @bits = @bits | 0x0002

			select @prot = null select @prot = protecttype from sysprotects where action = 224 and uid = user_id() and id = object_id('sp_addextendedproc')
			if (@prot is null) select @prot = protecttype from sysprotects where action = 224 and uid = (select gid from sysusers where uid = user_id()) and id = object_id('sp_addextendedproc')
			if (@prot is null) select @prot = protecttype from sysprotects where action = 224 and uid = user_id('public') and id = object_id('sp_addextendedproc')
			if (@prot = 205) select @bits = @bits | 0x0004
		end
		select @bits
		return 0
	end


	/* Just list the current user profile */
	if (lower(@mode) = 'list') begin
		set nocount off
		exec ('select db_name(dbid), convert(binary(4), profilebits) from #SQLOLEDbUserProfile')
		return 0
	end

	/* If 'init', just load the table. */
	if (lower(@mode) like 'ini%') begin
		/* Make sure we're either in master or only doing it to current db. */
		if (db_id() <> 1)
			select @qual = db_name()

		/* First clear out the table.  Do it in an exec() in case we're just selecting from cmdline debugging. */
		exec ('delete #SQLOLEDbUserProfile from #SQLOLEDbUserProfile p, master..sysdatabases d where d.name like ''' + @qual + ''' and p.dbid = d.dbid')

		/* See if we're to select after adding. */
		declare @origmode varchar(10)
		select @origmode = @mode
		if (@mode like '%sel%')
			select @mode = '''addsel'''
		else
			select @mode = '''add'''

		/* Preprocessor won't replace within quotes so have to use str(). */
		declare @invalidlogin varchar(12)
		select @invalidlogin = ltrim(str(convert(int, 0x40000000)))

		/* We can't 'use' a database with a version below the minimum. */
		/* SQL6.0 minimum is 406; SQL65 requires 408. */
		declare @mindbver smallint
		if (@@microsoftversion >= 0x000632c0)
			select @mindbver = 408
		else
			select @mindbver = 406

		/* Select all matching databases -- we want an entry even for inaccessible ones. */
		declare @dbid smallint, @dbidstr varchar(12), @dbstat smallint, @dbname varchar(30), @dbver smallint
		exec('declare hCdbs cursor for select name, dbid, status, version from master..sysdatabases where name like ''' + @qual + '''')
		open hCdbs

		/* Loop for each database, and if it's accessible, recursively call ourselves to add it. */
		fetch hCdbs into @dbname, @dbid, @dbstat, @dbver
		while (@@fetch_status >= 0) begin
			/* Preprocessor won't replace within quotes so have to use str(). */
			select @dbidstr = ltrim(str(convert(int, @dbid)))

			/* If it's a single user db and there's an entry for it in sysprocesses who isn't us, we can't use it. */
			declare @single_lockedout int
			select @single_lockedout = @dbstat & 0x1000
			if (@single_lockedout <> 0)
				select @single_lockedout = 0 where not exists 
					(select * from master..sysprocesses p where dbid = @dbid and p.spid <> @@spid)

			/* First see if the db is accessible (not in load, recovery, offline, single-use with another user besides us, etc.) */
			if ((@single_lockedout <> 0) or ((@dbstat & 0x03e0) <> 0) or (@dbver < @mindbver)) begin
				/* Inaccessible, but we can set dbo if we're sa or suser_id() is db owner suid. */
				declare @dbbits int, @dbbitstr varchar(12)
				select @dbbits = 0x80000000
				if (suser_id() = 1 or suser_id() = (select suid from sysdatabases where dbid = @dbid))
					select @dbbits = @dbbits | 0x0001
				select @dbbitstr = ltrim(convert(varchar(12), @dbbits))
				exec ('insert #SQLOLEDbUserProfile values (' + @dbidstr + ', ' + @dbbitstr + ')')
				if (lower(@mode) like '%sel%') begin
					set nocount off
					select 'profile bitmask' = @dbbits
					set nocount on
				end
			end else begin
				exec (@dbname + '..sp_MSdbuserprofile ' + @mode)
			end
			
			/* The exec() may have failed if we are not sa login and have not been added as a user to @dbname. */
			if (@@error <> 0)
				exec ('insert #SQLOLEDbUserProfile values (' + @dbidstr + ', ' + @invalidlogin + ')')
			fetch hCdbs into @dbname, @dbid, @dbstat, @dbver
		end /* while FETCH_SUCCESS */
		close hCdbs
		deallocate hCdbs

		/* Return the list if desired. */
		if (lower(@origmode) = 'initlist')
			exec sp_MSdbuserprofile 'list'
		return 0
	end

	/* If 'drop', just do it. */
	if (@mode = 'drop') begin
		exec ('delete #SQLOLEDbUserProfile where dbid = db_id()')
		return 0
	end





	/* All other modes require a creation of a profile bitmask. */
	if (user_id() = 1) begin
		/* sa/Dbo has everything. */
		select @bits = 0x00ff
	end else begin
		/* Not dbo so get individual privileges */
		select @bits = 0x0000, @gid = gid from sysusers where uid = user_id()
		select @prot = null select @prot = protecttype from sysprotects where action = 198 and uid = user_id()
		if (@prot is null) select @prot = protecttype from sysprotects where action = 198 and uid = (select gid from sysusers where uid = user_id())
		if (@prot is null) select @prot = protecttype from sysprotects where action = 198 and uid = user_id('public')
		if (@prot = 205) select @bits = @bits | 0x0002

		select @prot = null select @prot = protecttype from sysprotects where action = 207 and uid = user_id()
		if (@prot is null) select @prot = protecttype from sysprotects where action = 207 and uid = (select gid from sysusers where uid = user_id())
		if (@prot is null) select @prot = protecttype from sysprotects where action = 207 and uid = user_id('public')
		if (@prot = 205) select @bits = @bits | 0x0004

		select @prot = null select @prot = protecttype from sysprotects where action = 222 and uid = user_id()
		if (@prot is null) select @prot = protecttype from sysprotects where action = 222 and uid = (select gid from sysusers where uid = user_id())
		if (@prot is null) select @prot = protecttype from sysprotects where action = 222 and uid = user_id('public')
		if (@prot = 205) select @bits = @bits | 0x0008

		select @prot = null select @prot = protecttype from sysprotects where action = 228 and uid = user_id()
		if (@prot is null) select @prot = protecttype from sysprotects where action = 228 and uid = (select gid from sysusers where uid = user_id())
		if (@prot is null) select @prot = protecttype from sysprotects where action = 228 and uid = user_id('public')
		if (@prot = 205) select @bits = @bits | 0x0010

		select @prot = null select @prot = protecttype from sysprotects where action = 233 and uid = user_id()
		if (@prot is null) select @prot = protecttype from sysprotects where action = 233 and uid = (select gid from sysusers where uid = user_id())
		if (@prot is null) select @prot = protecttype from sysprotects where action = 233 and uid = user_id('public')
		if (@prot = 205) select @bits = @bits | 0x0020

		select @prot = null select @prot = protecttype from sysprotects where action = 235 and uid = user_id()
		if (@prot is null) select @prot = protecttype from sysprotects where action = 235 and uid = (select gid from sysusers where uid = user_id())
		if (@prot is null) select @prot = protecttype from sysprotects where action = 235 and uid = user_id('public')
		if (@prot = 205) select @bits = @bits | 0x0040

		select @prot = null select @prot = protecttype from sysprotects where action = 236 and uid = user_id()
		if (@prot is null) select @prot = protecttype from sysprotects where action = 236 and uid = (select gid from sysusers where uid = user_id())
		if (@prot is null) select @prot = protecttype from sysprotects where action = 236 and uid = user_id('public')
		if (@prot = 205) select @bits = @bits | 0x0080
	end


	/* We have the profile bitmask, now see what to do with it. */
	if (lower(@mode) like 'add%') begin
		declare @bitstr varchar(12)
		select @bitstr = ltrim(str(convert(int, @bits)))
		exec ('insert #SQLOLEDbUserProfile values (db_id(), ' + @bitstr + ')')
	end

	if (lower(@mode) like '%sel%') begin
		set nocount off
		if (lower(@qual) like 'bin%')		/* For visual debugging */
			select 'profile bitmask' = convert(binary(4), @bits)
		else
			select 'profile bitmask' = @bits
	end 
go
/* End sp_MSdbuserprofile */

/*******************************************************************************/
print ''
print 'Creating sp_MSobjectprivs'
print ''
go

create proc sp_MSobjectprivs
	@objname varchar(92) = null,
	@mode varchar(10) = 'object',	/* or 'user' */

/*
 * Note:  This was expanded for 6.5 due to changes in sysprotects.columns usage, affecting
 * CPermission::ListPrivilegeColumns.  The following additional parameters are for this.
 */
	@objid int = null,				/* ID of the object we're querying */
	@srvpriv int = null,			/* privilege that we're querying for (e.g. select) */
	@prottype int = null,			/* Protect type, e.g. GRANT/REVOKE */
	@grantee varchar(30) = null		/* Grantee name. */
as
	/* If @objid is not null, this is for the new query for perm cols. */
	if (@objid is not null) begin
		select u.name, o.name, a = col_name(p.id, a.number), a.low, a.high, a.number
			from master.dbo.spt_values a, sysprotects p, sysobjects o, sysusers u 
			where p.id = @objid and p.action = @srvpriv and p.protecttype = @prottype
			and p.uid = user_id(@grantee)
			and p.columns != 0x01 and o.id = p.id and u.uid = o.uid 
				and convert(tinyint, substring(isnull(p.columns, 0x01), a.low, 1)) & 

					-- 6.5 changed so that the bit 0 position is an "invert the bits" indicator:
					--		when 0, behaviour is the same as in prior versions, and other bits 
					--			indicate columns with the specified privilege
					--		when 1, the other bits are indicate columns lacking the specified privilege
					a.high <> (case when (substring(isnull(p.columns, 0x00), 1, 1) & 1 = 0) then 0 else a.high end)
					and col_name(p.id, a.number) is not null 



					and a.type = 'P' and a.number <= (select count(*) from syscolumns where id = @objid) order by a
		return 0
	end

	set nocount on

	/* 
	 * To get around a 4.21 subquery bug where returning count(*) of 0 (for proc cols)
	 * causes the result set to return no rows, we need two passes; one to get the
	 * objects, and another to explicitly use a value (@cols) instead of a subquery.
	 */
	declare @id int, @uid int, @cols int
	select @id = null, @uid = null
	if (@mode like 'us%')
		select @uid = user_id(@objname)
	else
		select @id = object_id(@objname)
	if (@id is null and @uid is null) begin
		
		
		RAISERROR (15001, -1, -1, @objname)
		return 1
	end

	/* Get a temp list of objects we're interested in.  Do not include repl_* users. */
	create table #objs(
		id					int				NOT NULL
	)
	insert #objs select distinct p.id from sysprotects p 
		where (@id is null or p.id = @id)
		and (@uid is null or p.uid = @uid)
		and p.action in (193, 195, 196, 197, 224, 26) and p.uid not in (16382, 16383)

	/* Temp table will hold output for final select */
	create table #output (
		action				int				NOT NULL,
		colid				int				NULL,
		uid					int				NOT NULL,
		protecttype			int				NOT NULL,
		id					int				NOT NULL
	)

	/* Use a "fake cursor" by deleting successive id's from #objs, as this must run on 4.21 */
	select @id = min(id) from #objs
	while (@id is not null) begin
		select @cols = count(*) from syscolumns c where c.id = @id
		insert #output select p.action, a.number, p.uid, p.protecttype, p.id
			from master.dbo.spt_values a, sysprotects p
			where convert(tinyint, substring( isnull(p.columns, 0x01), a.low, 1)) & a.high !=0
			and (p.id = @id)
			and (@uid is null or p.uid = @uid)
			and a.number <= @cols
			and a.type = 'P'
		/* Increment our "fake cursor" column and get the next one. */
		delete #objs where id = @id
		select @id = min(id) from #objs
	end

	/*
	 * Organize so that the non-collist privileges are returned first.. this allows
	 * scripting to combine them.  sysprotects.action is tinyint, so the hibyte won't conflict.
	 */

	update #output set action = action | 0x10000000 where colid <> 0

	/*
	 * Order output by uid so Public will script before other groups (we need to script privs for public before
	 * other groups, before users; otherwise sysprotects doesn't hold onto things right).  Sub-order is by object id
	 * so we know when we're done with one object and onto the next, then by protecttype to group all GRANTs and
	 * REVOKEs together, and lastly by action (including ORDER_ACTION_BIT so scripting can be more efficient)
	 * because we may have multiple rows for columns.
	 */
	set nocount off
	select p.action & ~convert(int, 0x10000000), 'column' = col_name(p.id, p.colid), p.uid, 'username' = user_name(p.uid), 
			p.protecttype, o.name, 'owner' = user_name(o.uid), p.id 
		from #output p, sysobjects o 
		where o.id = p.id 
		order by p.uid, p.id, p.protecttype, p.action
go
/* End sp_MSobjectprivs */

/*******************************************************************************/
/* Need to create the version proc here so we can set its category bit */
print ''
print 'Creating sp_MSSQLOLE65_version'
print ''
go

create procedure sp_MSSQLOLE65_version
as
	/* Values for this are same as @@microsoft_version:  0xrraaiibb (reserved, major, minor, build). */
	declare @i int
	select @i = 0x00063207	/* Must be in hex! */
	/* Select the numeric value, and a conversion to make it readable */
	select 'Microsoft SQLOLE Scripts' = @i, 'Version' = convert(binary(4), @i)
go

create procedure sp_MSSQLOLE_version
as

	if (@@microsoftversion >= 0x000632c0) begin
		/* 
		 * On a 6.5 server, we want to make 6.0 SQLOLE clients upgrade to 6.5.
		 * It's fully backwards compatible and has many fixes.
		 */
		RAISERROR (14302, -1, -1)
		return 1
	end


	/* For 4.21 or 6.0, we allow any version of SQLOLE to connect. */
	exec sp_MSSQLOLE65_version
go

/********************* Grant privileges *********************************/
print ''
print 'Granting execute permissions on procedures'

grant execute on sp_MShelpcolumns to public
grant execute on sp_MShelpindex to public
grant execute on sp_MShelptype to public
grant execute on sp_MSdependencies to public
grant execute on sp_MStablespace to public
grant execute on sp_MSindexspace to public
grant execute on sp_MSuniquename to public
grant execute on sp_MSkilldb to public
grant execute on sp_MSdbuserprofile to public
grant execute on sp_MSobjectprivs to public
grant execute on sp_MSloginmappings to public

grant execute on sp_MSgetalertinfo to public
grant execute on sp_MSsetalertinfo to public
grant execute on sp_MSgetexecinfo to public
grant execute on sp_MSsetexecinfo to public
grant execute on sp_MStablekeys to public
grant execute on sp_MStablechecks to public
grant execute on sp_MStablerefs to public
grant execute on sp_MSsettopology to public
grant execute on sp_MSfilterclause to public
grant execute on sp_MSmatchkey to public
grant execute on sp_MSsubscriptions to public
grant execute on sp_MSforeachdb to public
grant execute on sp_MSforeachtable to public
grant execute on sp_MSforeach_worker to public
grant execute on sp_MSenumsubsystems to public



grant execute on sp_MSSQLOLE_version to public
grant execute on sp_MSSQLOLE65_version to public
go

/********************** Verify object creation and update category bit for objects *********************************/

/***************************************************
 ******************* SQL60 *************************
 ***************************************************
 */

print ''
print 'Updating category for objects created by sqlole65.sql.'
print ''
go

sp_configure 'allow updates', 1
go
reconfigure with override
go

/* New validation added for 6.5 */
if (@@microsoftversion >= 0x000632c0) begin
	exec sp_MS_upd_sysobj_category 2
end else begin
	update sysobjects set category = category | 2 
		where sysstat & 0x0f = 0x0002 and name in (
			'sp_MShelpcolumns',
			'sp_MShelpindex',
			'sp_MShelptype',
			'sp_MSdependencies',
			'sp_MStablespace',
			'sp_MSindexspace',
			'sp_MSuniquename',
			'sp_MSkilldb',
			'sp_MSdbuserprofile',
			'sp_MSobjectprivs',
			'sp_MSloginmappings',
			'sp_MSgetalertinfo',
			'sp_MSsetalertinfo',
			'sp_MSgetexecinfo',
			'sp_MSsetexecinfo',
			'sp_MStablekeys',
			'sp_MStablechecks',
			'sp_MStablerefs',
			'sp_MSsettopology',
			'sp_MSfilterclause',
			'sp_MSmatchkey',
			'sp_MSsubscriptions',
			'sp_MSforeachdb',
			'sp_MSforeachtable',
			'sp_MSforeach_worker',
			'sp_MSenumsubsystems',
			'xp_msver',
			'sp_MSSQLOLE_version',
			'sp_MSSQLOLE65_version'
		)
end
go

sp_configure 'allow updates', 0
go
reconfigure with override
go
















































































































































































if (object_id('sp_MSSQLOLE65_version') is not null) begin
	print ''
	print ''
	print ' Successful installation.'
	exec sp_MSSQLOLE65_version
end

/************* DUMP THE TRANSACTION LOG **************************************/
/* Comment this out if you don't want your log dumped.  If you rerun this    */
/* script periodically, you will run out of transaction log space.           */
print ''
print 'Dumping transaction log...'
print ''
go
dump tran master with no_log
go
checkpoint
go
/************* END DUMP THE TRANSACTION LOG **********************************/

