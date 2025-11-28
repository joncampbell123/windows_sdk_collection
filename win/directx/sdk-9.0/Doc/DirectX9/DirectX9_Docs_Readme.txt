
            Microsoft DirectX 9.0 SDK Readme File
                        December 2002
  (c) Microsoft Corporation, 2002. All rights reserved.

This document provides late-breaking or other information that supplements the Microsoft DirectX 9.0 software development kit (SDK) documentation.

------------------------
How to Use This Document
------------------------

To view the Readme file on-screen in Windows Notepad, maximize the Notepad window. On the Edit or Format menu, click Word Wrap. To print the Readme file, open it in Notepad or another word processor, and then use the Print command on the File menu.

---------
CONTENTS
---------

1.  PRINTING ISSUES
 
2.  DIRECTX GRAPHICS

    2.1  Shader Updates
    2.2  Effect Framework  
    2.3  High-Level Language
    2.4  Formats
    2.5  New Vertex Declaration Usages
    2.6  Tutorial #3 Performance
    2.7  CopyRects Removed
    2.8  Version 3_0 Shaders Instruction Slot Cap
    2.9  Scissor Rectangle
    2.10 Centroid

3.  MANAGED CODE
    3.1  Viewing Managed DirectX Documentation
    3.2  Samples


-------------------
1.   PRINTING ISSUES

     If you are trying to print a heading and all of its subtopics, there are several known issues.
     - The pages will have no formatting.
     - If you are using Microsoft Internet Explorer 5.5, content may be missing from your printed pages. 
       Use Internet Explorer 5.0 or 6.0 instead. The recommended printing method is to print one topic at a time. 
       The printed page should then contain all the formatting and content.


	   
2.  DIRECTX GRAPHICS

     2.1 Shader Updates

     Vertex and pixel shaders have a new architecture implemented by register and instruction changes. The documentation now includes a reference section for each of the shader versions. Also, check BetaPlace.com for the design specifications that were used to design the new 2_0, 2_x, 2_sw, 3_0, and 3_sw versions.   

     Software only versions, vs_2_sw, vs_3_sw, ps_2_sw, and ps_3_sw are supported for software vertex processing, or the reference rasterizer. 
 
     Shader versions are referred to with an underscore (_) rather than a period (.) separating the version numbers, so ps.2.0 becomes ps_2_0 and vs.3.0 becomes vs_3_0, for example.

     Vertex shaders version 2_0 and above create immediate constants with def (float constants), defi (integer constants), and defb (Boolean constants) instructions. 

     Pixel shaders version 2_0 and above create immediate constants with def (float constants), defi (integer constants), and defb (Boolean constants) instructions. 

 
     2.2  Effect Framework

     The effect framework has undergone some exciting changes for DirectX 9.0. It is now designed from a base class and derived interfaces with ID3DXBaseEffect, ID3DXEffect, and ID3DXEffectCompiler. It has been extended to include fragments and include files with ID3DXFragmentLinker and ID3DXInclude. The SDK sample, EffectEdit, demonstrates the new effect framework.


     2.3  High-Level Language

     The documentation will continue to be filled in for the next release. Check BetaPlace.com for the design specifications describing the feature set.


     2.4 Formats

     Valid back buffer formats in Direct3D 9.0 are: 
     - A2R10G10B10
     - A8R8G8B8
     - X8R8G8B8
     - A1R5G5B5
     - X1R5G5B5
     - R5G6B5 

     The runtime does not include a software fallback for color-conversion during presentation. Device creation will fail if IDirect3D9::CheckDeviceType(…) and IDirect3D::CheckDeviceFormatConversion(…) request conversion and the hardware does not support it.


     2.5 New Vertex Declaration Usages

     Several new vertex usages were added, including D3DDECLUSAGE_POSITIONTL, D3DDECLUSAGE_COLOR, D3DDECLUSAGE_FOG, D3DDECLUSAGE_DEPTH, and D3DDECLUSAGE_SAMPLE. See D3DDECLUSAGE for details.


     2.6  Tutorial #3 Performance

     You might experience unusually poor performance when running tutorial 3 because of the D3DXMatrixRotation function's use of the sin function. The sin(x) function outputs very rough numbers when the input gets far out of its domain of -2 Pi to 2 Pi. So instead of using D3DXMatrixRotationY( &matWorld, timeGetTime()/150.0f ); in line 156, use the mod function to clamp the range like this:

     D3DXMatrixRotationY( &matWorld, (timeGetTime()%((int)(150.0f * 2.0f * 3.1415926f)))/150.0f );

     The period of rotation is then clamped between -2 Pi and + 2 Pi.


     2.7 CopyRects Removed

     IDirect3DDevice9::CopyRects has been removed. Some of the functionality that this method offered is available through two new methods: IDirect3DDevice9::UpdateSurface and IDirect3DDevice9::GetRenderTargetData.  
     UpdateSurface enables the transfer of data from a system memory surface to a video memory surface. GetRenderTargetData enables the transfer of data from a render target surface to a system memory surface.  If an application needs to transfer data from a video memory texture (or cubemap surface) to a system memory surface, it can set the D3DUSAGE_DYNAMIC flag when creating the resource, thus enabling locking of the component surfaces.  (Note that many display adapter drivers do not support the creation of dynamic resources.)  Alternatively, the application can proceed indirectly by rendering the texture (or cubemap surface) and applying GetRenderTargetData to the render target.


     2.8  Version 3_0 Shaders Instruction Slot Cap
	
     The D3DCAPS9 structure has two new caps: MaxVertexShader30InstructionSlots and  
MaxPixelShader30InstructionSlots. Each cap indicates the maximum number of instruction slots supported by the corresponding shader type. 


     2.9 Scissor Rectangle

     The following description of how a RECT is used by the scissor test was left out. The .right and .bottom extents of the provided RECT indicate one pixel beyond the desired edge of the rectangle. For example, given a 1024x768 render target, with top left coordinate (0,0) and bottom right coordinate (1023,767), the scissor rect that would cover it exactly is:

     Rect.left = 0;
     Rect.right = 1024;
     Rect.top = 0;
     Rect.bottom = 768;


     2.10 Centroid

     The following content about applying the pixel shader centroid instruction modifier was left out.

     Centroid is only a hint. Implementations can ignore it if it is not supported.

     The _color usage has special behavior: It assumes that _centroid is set (as a default) so specifying color along with _centroid has no effect.  

     In ps_3_0 and ps_3_sw, input registers can be declared with multiple usage plus index names for various components of a given input v# register. Therefore, _centroid must be specified on either all of the usages defined for a particular v# register, or none of the usages defined for a particular v# register. 



3.   MANAGED CODE

     3.1  Viewing Managed DirectX Documentation

     The DirectX managed code application programming interface (API) is automatically installed with DirectX 9.0. Open Visual Studio .NET and then, from the Help menu at the top, choose Contents. The documentation node in the Table of Contents is called DirectX 9.0 (Managed). The documentation can also be viewed on the Web at http://www.msdn.microsoft.com. See the Help topic, "Tips and Tricks Using Managed Code," for the basics of how to install and use DirectX managed code and documentation.


     3.2  Samples

     The samples for using Managed DirectX are located at (SDK root)\Samples\C#\ and (SDK root)\Samples\VB.Net\.
	 
	 