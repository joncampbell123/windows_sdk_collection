/**
 * This class is designed to be packaged with a COM DLL output format.
 * The class has no standard entry points, other than the constructor.
 * Public methods will be exposed as methods on the default COM interface.
 * @com.register ( clsid=%GUID1%, typelib=%GUID2% )
 */
public class COMPlus
{
	// TODO: Add additional methods and code here

	// Sample method which uses the COM+ Server Context.  
	// See the Microsoft COM+ documentation for more information
	// about use the Context class and creating COM+ components.
	public void Method1()
	{
		boolean bSuccess = false;
		try {
			// TODO: Add your method code here.  If successful, set bSuccess 
		    // to true 
			bSuccess = true;
			return;
		}

		// Most methods in COM+ objects should call setComplete if successful,
		// or setAbort if not. This causes this object to be deactivated and
		// lose state after the return of this function.  See the COM+ 
		// documentation for more information.
		finally {
			if (bSuccess)
				Context.setComplete();
			else
				Context.setAbort();
		}
	}
	
	//Component that supports object pooling must support IObjectControl
	public void Activate()
	{
	}
	
	public void Deactivate()
	{
	}
	
	public boolean CanBePooled()
	{
		return true;
	}

	//TODO: Add code to construct object using construction string.
	// IObjectConstruct
	//public void Construct(Object pObj);
	//{
	//}

	/**
	 * NOTE: To add auto-registration code, refer to the documentation
	 * on the following method
	 *   public static void onCOMRegister(boolean unRegister) {}
	 */
}
