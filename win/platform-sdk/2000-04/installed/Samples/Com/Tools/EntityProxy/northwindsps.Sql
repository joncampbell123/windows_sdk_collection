use Northwind
go

if exists (select * from sysobjects where id = object_id(N'[dbo].[spCustomersDelete]') and OBJECTPROPERTY(id, N'IsProcedure') = 1)
drop procedure [dbo].[spCustomersDelete]
GO

if exists (select * from sysobjects where id = object_id(N'[dbo].[spCustomersInsert]') and OBJECTPROPERTY(id, N'IsProcedure') = 1)
drop procedure [dbo].[spCustomersInsert]
GO

if exists (select * from sysobjects where id = object_id(N'[dbo].[spCustomersLoad]') and OBJECTPROPERTY(id, N'IsProcedure') = 1)
drop procedure [dbo].[spCustomersLoad]
GO

if exists (select * from sysobjects where id = object_id(N'[dbo].[spCustomersUpdate]') and OBJECTPROPERTY(id, N'IsProcedure') = 1)
drop procedure [dbo].[spCustomersUpdate]
GO

if exists (select * from sysobjects where id = object_id(N'[dbo].[spOrderDetailsDelete]') and OBJECTPROPERTY(id, N'IsProcedure') = 1)
drop procedure [dbo].[spOrderDetailsDelete]
GO

if exists (select * from sysobjects where id = object_id(N'[dbo].[spOrderDetailsInsert]') and OBJECTPROPERTY(id, N'IsProcedure') = 1)
drop procedure [dbo].[spOrderDetailsInsert]
GO

if exists (select * from sysobjects where id = object_id(N'[dbo].[spOrderDetailsLoad]') and OBJECTPROPERTY(id, N'IsProcedure') = 1)
drop procedure [dbo].[spOrderDetailsLoad]
GO

if exists (select * from sysobjects where id = object_id(N'[dbo].[spOrderDetailsUpdate]') and OBJECTPROPERTY(id, N'IsProcedure') = 1)
drop procedure [dbo].[spOrderDetailsUpdate]
GO

if exists (select * from sysobjects where id = object_id(N'[dbo].[spOrdersDelete]') and OBJECTPROPERTY(id, N'IsProcedure') = 1)
drop procedure [dbo].[spOrdersDelete]
GO

if exists (select * from sysobjects where id = object_id(N'[dbo].[spOrdersInsert]') and OBJECTPROPERTY(id, N'IsProcedure') = 1)
drop procedure [dbo].[spOrdersInsert]
GO

if exists (select * from sysobjects where id = object_id(N'[dbo].[spOrdersLoad]') and OBJECTPROPERTY(id, N'IsProcedure') = 1)
drop procedure [dbo].[spOrdersLoad]
GO

if exists (select * from sysobjects where id = object_id(N'[dbo].[spOrdersUpdate]') and OBJECTPROPERTY(id, N'IsProcedure') = 1)
drop procedure [dbo].[spOrdersUpdate]
GO

if exists (select * from sysobjects where id = object_id(N'[dbo].[spProductsDelete]') and OBJECTPROPERTY(id, N'IsProcedure') = 1)
drop procedure [dbo].[spProductsDelete]
GO

if exists (select * from sysobjects where id = object_id(N'[dbo].[spProductsInsert]') and OBJECTPROPERTY(id, N'IsProcedure') = 1)
drop procedure [dbo].[spProductsInsert]
GO

if exists (select * from sysobjects where id = object_id(N'[dbo].[spProductsLoad]') and OBJECTPROPERTY(id, N'IsProcedure') = 1)
drop procedure [dbo].[spProductsLoad]
GO

if exists (select * from sysobjects where id = object_id(N'[dbo].[spProductsUpdate]') and OBJECTPROPERTY(id, N'IsProcedure') = 1)
drop procedure [dbo].[spProductsUpdate]
GO

SET QUOTED_IDENTIFIER  OFF    SET ANSI_NULLS  ON 
GO



Create Procedure spCustomersDelete
(
	@CustomerID nvarchar(5)
)
As
DELETE from [Customers] WHERE CustomerID = @CustomerID
Return

GO
SET QUOTED_IDENTIFIER  OFF    SET ANSI_NULLS  ON 
GO

SET QUOTED_IDENTIFIER  OFF    SET ANSI_NULLS  ON 
GO



CREATE Procedure spCustomersInsert
(
	@CustomerID nvarchar(5), 
	@CompanyName nvarchar(40), 
	@ContactName nvarchar(30), 
	@ContactTitle nvarchar(30), 
	@Address nvarchar(60), 
	@City nvarchar(15), 
	@Region nvarchar(15), 
	@PostalCode nvarchar(10), 
	@Country nvarchar(15), 
	@Phone nvarchar(24), 
	@Fax nvarchar(24)
)
As
SET NOCOUNT ON
	INSERT INTO [Customers]
	(
		CompanyName, 
		ContactName, 
		ContactTitle, 
		Address, 
		City, 
		Region, 
		PostalCode, 
		Country, 
		Phone, 
		Fax
	)
	VALUES
	(
		@CompanyName, 
		@ContactName, 
		@ContactTitle, 
		@Address, 
		@City, 
		@Region, 
		@PostalCode, 
		@Country, 
		@Phone, 
		@Fax
	)
	SELECT @@identity
Return

GO
SET QUOTED_IDENTIFIER  OFF    SET ANSI_NULLS  ON 
GO

SET QUOTED_IDENTIFIER  OFF    SET ANSI_NULLS  ON 
GO


Create Procedure spCustomersLoad
(
	@CustomerID nvarchar(5)
)
As
SELECT CustomerID, CompanyName, ContactName, ContactTitle, Address, City, Region, PostalCode, Country, Phone, Fax
FROM [Customers] WHERE CustomerID = @CustomerID
Return

GO
SET QUOTED_IDENTIFIER  OFF    SET ANSI_NULLS  ON 
GO

SET QUOTED_IDENTIFIER  OFF    SET ANSI_NULLS  ON 
GO



CREATE Procedure spCustomersUpdate
(
	@CustomerID nvarchar(5), 
	@CompanyName nvarchar(40), 
	@ContactName nvarchar(30), 
	@ContactTitle nvarchar(30), 
	@Address nvarchar(60), 
	@City nvarchar(15), 
	@Region nvarchar(15), 
	@PostalCode nvarchar(10), 
	@Country nvarchar(15), 
	@Phone nvarchar(24), 
	@Fax nvarchar(24)
)
As
SET NOCOUNT ON
	UPDATE [Customers]
	SET 
		CompanyName = @CompanyName, 
		ContactName = @ContactName, 
		ContactTitle = @ContactTitle, 
		Address = @Address, 
		City = @City, 
		Region = @Region, 
		PostalCode = @PostalCode, 
		Country = @Country, 
		Phone = @Phone, 
		Fax = @Fax
	WHERE CustomerID = @CustomerID
	SELECT @CustomerID
Return

GO
SET QUOTED_IDENTIFIER  OFF    SET ANSI_NULLS  ON 
GO

SET QUOTED_IDENTIFIER  OFF    SET ANSI_NULLS  ON 
GO



Create Procedure spOrderDetailsDelete
(
	@OrderID int
)
As
DELETE from [Order Details] WHERE OrderID = @OrderID
Return

GO
SET QUOTED_IDENTIFIER  OFF    SET ANSI_NULLS  ON 
GO

SET QUOTED_IDENTIFIER  OFF    SET ANSI_NULLS  ON 
GO



CREATE Procedure spOrderDetailsInsert
(
	@OrderID int, 
	@ProductID int, 
	@UnitPrice money, 
	@Quantity smallint, 
	@Discount real
)
As
SET NOCOUNT ON
	INSERT INTO [Order Details]
	(
		ProductID, 
		UnitPrice, 
		Quantity, 
		Discount
	)
	VALUES
	(
		@ProductID, 
		@UnitPrice, 
		@Quantity, 
		@Discount
	)
	SELECT @@identity
Return

GO
SET QUOTED_IDENTIFIER  OFF    SET ANSI_NULLS  ON 
GO

SET QUOTED_IDENTIFIER  OFF    SET ANSI_NULLS  ON 
GO


Create Procedure spOrderDetailsLoad
(
	@OrderID int
)
As
SELECT OrderID, ProductID, UnitPrice, Quantity, Discount
FROM [Order Details] WHERE OrderID = @OrderID
Return

GO
SET QUOTED_IDENTIFIER  OFF    SET ANSI_NULLS  ON 
GO

SET QUOTED_IDENTIFIER  OFF    SET ANSI_NULLS  ON 
GO



CREATE Procedure spOrderDetailsUpdate
(
	@OrderID int, 
	@ProductID int, 
	@UnitPrice money, 
	@Quantity smallint, 
	@Discount real
)
As
SET NOCOUNT ON
	UPDATE [Order Details]
	SET 
		ProductID = @ProductID, 
		UnitPrice = @UnitPrice, 
		Quantity = @Quantity, 
		Discount = @Discount
	WHERE OrderID = @OrderID
	SELECT @OrderID
Return

GO
SET QUOTED_IDENTIFIER  OFF    SET ANSI_NULLS  ON 
GO

SET QUOTED_IDENTIFIER  OFF    SET ANSI_NULLS  ON 
GO



Create Procedure spOrdersDelete
(
	@OrderID int
)
As
DELETE from [Orders] WHERE OrderID = @OrderID
Return

GO
SET QUOTED_IDENTIFIER  OFF    SET ANSI_NULLS  ON 
GO

SET QUOTED_IDENTIFIER  OFF    SET ANSI_NULLS  ON 
GO



CREATE Procedure spOrdersInsert
(
	@OrderID int, 
	@CustomerID nvarchar(5), 
	@EmployeeID int, 
	@OrderDate datetime, 
	@RequiredDate datetime, 
	@ShippedDate datetime, 
	@ShipVia int, 
	@Freight money, 
	@ShipName nvarchar(40), 
	@ShipAddress nvarchar(60), 
	@ShipCity nvarchar(15), 
	@ShipRegion nvarchar(15), 
	@ShipPostalCode nvarchar(10), 
	@ShipCountry nvarchar(15)
)
As
SET NOCOUNT ON
	INSERT INTO [Orders]
	(
		CustomerID, 
		EmployeeID, 
		OrderDate, 
		RequiredDate, 
		ShippedDate, 
		ShipVia, 
		Freight, 
		ShipName, 
		ShipAddress, 
		ShipCity, 
		ShipRegion, 
		ShipPostalCode, 
		ShipCountry
	)
	VALUES
	(
		@CustomerID, 
		@EmployeeID, 
		@OrderDate, 
		@RequiredDate, 
		@ShippedDate, 
		@ShipVia, 
		@Freight, 
		@ShipName, 
		@ShipAddress, 
		@ShipCity, 
		@ShipRegion, 
		@ShipPostalCode, 
		@ShipCountry
	)
	SELECT @@identity
Return

GO
SET QUOTED_IDENTIFIER  OFF    SET ANSI_NULLS  ON 
GO

SET QUOTED_IDENTIFIER  OFF    SET ANSI_NULLS  ON 
GO

Create Procedure spOrdersLoad
(
	@OrderID int
)
As
SELECT OrderID, CustomerID, EmployeeID, OrderDate, RequiredDate, ShippedDate, ShipVia, Freight, ShipName, ShipAddress, ShipCity, ShipRegion, ShipPostalCode, ShipCountry
FROM [Orders] WHERE OrderID = @OrderID
Return

GO
SET QUOTED_IDENTIFIER  OFF    SET ANSI_NULLS  ON 
GO

SET QUOTED_IDENTIFIER  OFF    SET ANSI_NULLS  ON 
GO



CREATE Procedure spOrdersUpdate
(
	@OrderID int, 
	@CustomerID nvarchar(5), 
	@EmployeeID int, 
	@OrderDate datetime, 
	@RequiredDate datetime, 
	@ShippedDate datetime, 
	@ShipVia int, 
	@Freight money, 
	@ShipName nvarchar(40), 
	@ShipAddress nvarchar(60), 
	@ShipCity nvarchar(15), 
	@ShipRegion nvarchar(15), 
	@ShipPostalCode nvarchar(10), 
	@ShipCountry nvarchar(15)
)
As
SET NOCOUNT ON
	UPDATE [Orders]
	SET 
		CustomerID = @CustomerID, 
		EmployeeID = @EmployeeID, 
		OrderDate = @OrderDate, 
		RequiredDate = @RequiredDate, 
		ShippedDate = @ShippedDate, 
		ShipVia = @ShipVia, 
		Freight = @Freight, 
		ShipName = @ShipName, 
		ShipAddress = @ShipAddress, 
		ShipCity = @ShipCity, 
		ShipRegion = @ShipRegion, 
		ShipPostalCode = @ShipPostalCode, 
		ShipCountry = @ShipCountry
	WHERE OrderID = @OrderID
	SELECT @OrderID
Return

GO
SET QUOTED_IDENTIFIER  OFF    SET ANSI_NULLS  ON 
GO

SET QUOTED_IDENTIFIER  OFF    SET ANSI_NULLS  ON 
GO



Create Procedure spProductsDelete
(
	@ProductID int
)
As
DELETE from [Products] WHERE ProductID = @ProductID
Return

GO
SET QUOTED_IDENTIFIER  OFF    SET ANSI_NULLS  ON 
GO

SET QUOTED_IDENTIFIER  OFF    SET ANSI_NULLS  ON 
GO



CREATE Procedure spProductsInsert
(
	@ProductID int, 
	@ProductName nvarchar(40), 
	@SupplierID int, 
	@CategoryID int, 
	@QuantityPerUnit nvarchar(20), 
	@UnitPrice money, 
	@UnitsInStock smallint, 
	@UnitsOnOrder smallint, 
	@ReorderLevel smallint, 
	@Discontinued bit
)
As
SET NOCOUNT ON
	INSERT INTO [Products]
	(
		ProductName, 
		SupplierID, 
		CategoryID, 
		QuantityPerUnit, 
		UnitPrice, 
		UnitsInStock, 
		UnitsOnOrder, 
		ReorderLevel, 
		Discontinued
	)
	VALUES
	(
		@ProductName, 
		@SupplierID, 
		@CategoryID, 
		@QuantityPerUnit, 
		@UnitPrice, 
		@UnitsInStock, 
		@UnitsOnOrder, 
		@ReorderLevel, 
		@Discontinued
	)
	SELECT @@identity
Return

GO
SET QUOTED_IDENTIFIER  OFF    SET ANSI_NULLS  ON 
GO

SET QUOTED_IDENTIFIER  OFF    SET ANSI_NULLS  ON 
GO

Create Procedure spProductsLoad
(
	@ProductID int
)
As
SELECT ProductID, ProductName, SupplierID, CategoryID, QuantityPerUnit, UnitPrice, UnitsInStock, UnitsOnOrder, ReorderLevel, Discontinued
FROM [Products] WHERE ProductID = @ProductID
Return

GO
SET QUOTED_IDENTIFIER  OFF    SET ANSI_NULLS  ON 
GO

SET QUOTED_IDENTIFIER  OFF    SET ANSI_NULLS  ON 
GO



CREATE Procedure spProductsUpdate
(
	@ProductID int, 
	@ProductName nvarchar(40), 
	@SupplierID int, 
	@CategoryID int, 
	@QuantityPerUnit nvarchar(20), 
	@UnitPrice money, 
	@UnitsInStock smallint, 
	@UnitsOnOrder smallint, 
	@ReorderLevel smallint, 
	@Discontinued bit
)
As
SET NOCOUNT ON
	UPDATE [Products]
	SET 
		ProductName = @ProductName, 
		SupplierID = @SupplierID, 
		CategoryID = @CategoryID, 
		QuantityPerUnit = @QuantityPerUnit, 
		UnitPrice = @UnitPrice, 
		UnitsInStock = @UnitsInStock, 
		UnitsOnOrder = @UnitsOnOrder, 
		ReorderLevel = @ReorderLevel, 
		Discontinued = @Discontinued
	WHERE ProductID = @ProductID
	SELECT @ProductID
Return

GO
SET QUOTED_IDENTIFIER  OFF    SET ANSI_NULLS  ON 
GO

