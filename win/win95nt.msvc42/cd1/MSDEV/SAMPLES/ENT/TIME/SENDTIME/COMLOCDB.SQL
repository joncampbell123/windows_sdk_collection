/* Database : ComLocDB                                        */

disk init name = 'ComLocDV',
physname = 'c:\mssql\data\comloc.dat',
vdevno = 10,
size = 1024
/*go*/

create database ComLocDB on ComLocDV
go

use ComLocDB
go

create table dbo.ComLoc (
        cl_machine varchar (32) NOT NULL ,
        cl_progid varchar (32) NOT NULL ,
        CONSTRAINT PK___1__10 PRIMARY KEY  NONCLUSTERED 
        (
                cl_machine
        )
)
go

insert ComLoc
values('machine1', 'settime')
go

insert ComLoc
values('machine2', 'settime')
go
