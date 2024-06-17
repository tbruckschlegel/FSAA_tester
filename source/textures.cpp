//-----------------------------------------------------------------------------
// File: Textures.cpp
//
// Desc: FSAA tester
//-----------------------------------------------------------------------------
#define STRICT
//#define DXVERSION9
#define DXVERSION8

#include <Windows.h>
#include <stdio.h>
#include "d3dfont.h"

#ifdef DXVERSION9
#include <d3d9.h>
#include <d3dx9.h>
#endif

#ifdef DXVERSION8
#include <d3d8.h>
#include <d3dx8.h>
#endif

bool g_bAnimate=false;
int StartBMP_Sequence=0;
int Sequence_Length=25;
int Sequence_Speed=100;
int TimeStamp=0;
//-----------------------------------------------------------------------------
// Global variables
//-----------------------------------------------------------------------------
#ifdef DXVERSION9
static LPDIRECT3D9             g_pD3D       = NULL; // used to create the D3DDevice
static LPDIRECT3DDEVICE9       g_pd3dDevice = NULL; // our rendering device
static LPDIRECT3DVERTEXBUFFER9 g_pVB        = NULL; // buffer to hold vertices
static LPDIRECT3DVERTEXBUFFER9 g_pVB2       = NULL; // second buffer to hold vertices
static LPDIRECT3DVERTEXBUFFER9 g_pVB3       = NULL; // second buffer to hold vertices
static LPDIRECT3DTEXTURE9      g_pTexture   = NULL; // our texture
#endif
#ifdef DXVERSION8
static LPDIRECT3D8             g_pD3D       = NULL; // used to create the D3DDevice
static LPDIRECT3DDEVICE8       g_pd3dDevice = NULL; // our rendering device
static LPDIRECT3DVERTEXBUFFER8 g_pVB        = NULL; // buffer to hold vertices
static LPDIRECT3DVERTEXBUFFER8 g_pVB2       = NULL; // second buffer to hold vertices
static LPDIRECT3DVERTEXBUFFER8 g_pVB3       = NULL; // second buffer to hold vertices
static LPDIRECT3DTEXTURE8      g_pTexture   = NULL; // our texture
#endif
static D3DMULTISAMPLE_TYPE	   FSAAMode		= D3DMULTISAMPLE_NONE;
static BOOL					   bFog			= FALSE;//TRUE;
static DWORD				   Obj1_NumVertices	= 25;
static DWORD				   Obj2_NumVertices	= 180;
static DWORD				   Obj3_NumVertices	= 4;
static DWORD				   Device=D3DDEVTYPE_HAL;
static DWORD				   LastDevice=Device;
static CD3DFont				   *g_pFont		= NULL;
static unsigned long		   memory=0;
static unsigned long		   memory_sub=0;
static DWORD KeyDown		   = 0;
static char g_tmpStr[255];
static bool highzbuffer=true;
static bool bCleanup=true;
static bool mode=true;
static bool fullscreen=false;
static int bMipEnable=1;
static HRESULT hr;
static HANDLE  g_hInstance;
#define MAX_X 800;
#define MAX_Y 600;
static int X_Res = MAX_X;
static int Y_Res = MAX_Y;
static int TexFilter=0;
static int MaxAniso=0;
// A structure for our custom vertex type. We added texture coordinates
struct CUSTOMVERTEX
{
    D3DXVECTOR3 position; // The position
    D3DCOLOR    color;    // The color
    FLOAT       tu, tv;   // The texture coordinates
};

// Our custom FVF, which describes our custom vertex structure
#define D3DFVF_CUSTOMVERTEX (D3DFVF_XYZ|D3DFVF_DIFFUSE|D3DFVF_TEX1)

struct CUSTOMVERTEX2
{
    D3DXVECTOR4 position; // The position
    D3DCOLOR    color;    // The color
};

// Our custom FVF, which describes our custom vertex structure
#define D3DFVF_CUSTOMVERTEX2 (D3DFVF_XYZRHW|D3DFVF_DIFFUSE)

struct CUSTOMVERTEX3
{
    D3DXVECTOR4 position; // The position
    D3DCOLOR    color;    // The color
    FLOAT       tu, tv;   // The texture coordinates
};

// Our custom FVF, which describes our custom vertex structure
#define D3DFVF_CUSTOMVERTEX3 (D3DFVF_XYZRHW|D3DFVF_DIFFUSE|D3DFVF_TEX1)

VOID Cleanup_Quick();

//-----------------------------------------------------------------------------
// Name: InitD3D()
// Desc: Initializes Direct3D
//-----------------------------------------------------------------------------
HRESULT PreInit( HWND hWnd )
{
		bCleanup=false;
		// Create the D3D object.
#ifdef DXVERSION9
		if( NULL == ( g_pD3D = Direct3DCreate9( D3D_SDK_VERSION ) ) )
			return E_FAIL;
#endif
#ifdef DXVERSION8
		if( NULL == ( g_pD3D = Direct3DCreate8( D3D_SDK_VERSION ) ) )
			return E_FAIL;
#endif
		// Create the D3D object.
		g_pFont = new CD3DFont( _T("Arial"), 8, D3DFONT_BOLD );

		D3DDISPLAYMODE d3ddm;
		//if(!fullscreen)
			if( FAILED( g_pD3D->GetAdapterDisplayMode( D3DADAPTER_DEFAULT, &d3ddm ) ) )
				return E_FAIL;
		
		X_Res = MAX_X;
		Y_Res = MAX_Y;

		if(!fullscreen)
		{
			RECT Size;
			GetClientRect(hWnd, &Size);

			X_Res=(Size.right-Size.left);
			Y_Res=(Size.bottom-Size.top);
		}
		// Set up the structure used to create the D3DDevice. Since we are now
		// using more complex geometry, we will create a device with a zbuffer.
		D3DPRESENT_PARAMETERS d3dpp;
		ZeroMemory( &d3dpp, sizeof(d3dpp) );

		d3dpp.Windowed = !fullscreen;
		d3dpp.SwapEffect = /*fullscreen?D3DSWAPEFFECT_FLIP:*/D3DSWAPEFFECT_DISCARD;
		d3dpp.BackBufferCount = 1;
		d3dpp.BackBufferWidth = X_Res;
		d3dpp.BackBufferHeight = Y_Res;
		d3dpp.BackBufferFormat = /*fullscreen?D3DFMT_D32:*/d3ddm.Format;
		d3dpp.MultiSampleType = D3DMULTISAMPLE_NONE;//FSAAMode;
		d3dpp.EnableAutoDepthStencil = true;
		d3dpp.AutoDepthStencilFormat = (highzbuffer==true)?D3DFMT_D24X8:D3DFMT_D16;//24X8;//24X8; //kyro FIX - no stencil buffer
		d3dpp.hDeviceWindow=hWnd;
		d3dpp.Flags=D3DPRESENTFLAG_LOCKABLE_BACKBUFFER;

		// Create the D3DDevice
		if( FAILED( hr=g_pD3D->CreateDevice( D3DADAPTER_DEFAULT, (enum _D3DDEVTYPE)Device, hWnd,
										  //D3DCREATE_FPU_PRESERVE|
										  //D3DCREATE_MULTITHREADED|
										  //D3DCREATE_PUREDEVICE| 	// kyro FIX - no hw TnL
										  //D3DCREATE_HARDWARE_VERTEXPROCESSING,
										  D3DCREATE_FPU_PRESERVE|D3DCREATE_SOFTWARE_VERTEXPROCESSING,
										  &d3dpp, &g_pd3dDevice ) ) )
		{
			// maybe it doesn't work
			highzbuffer=false;
			d3dpp.AutoDepthStencilFormat = (highzbuffer==true)?D3DFMT_D24X8:D3DFMT_D16;//24X8;//24X8; //kyro FIX - no stencil buffer
			if( FAILED( hr=g_pD3D->CreateDevice( D3DADAPTER_DEFAULT, (enum _D3DDEVTYPE)Device, hWnd,
											  //D3DCREATE_FPU_PRESERVE|
											  //D3DCREATE_MULTITHREADED|
											  //D3DCREATE_PUREDEVICE| 	// kyro FIX - no hw TnL
											  //D3DCREATE_HARDWARE_VERTEXPROCESSING,
											  D3DCREATE_FPU_PRESERVE|D3DCREATE_SOFTWARE_VERTEXPROCESSING,
											  &d3dpp, &g_pd3dDevice ) ) )
			{
				bCleanup=true;
				//PostQuitMessage( 0 );
				return E_FAIL;
			}
		}

		memory=memory_sub=g_pd3dDevice->GetAvailableTextureMem();
		Cleanup_Quick();

		return S_OK;

}

HRESULT InitD3D( HWND hWnd )
{
	/*
		bCleanup=false;
		// Create the D3D object.
#ifdef DXVERSION9
		if( NULL == ( g_pD3D = Direct3DCreate9( D3D_SDK_VERSION ) ) )
			return E_FAIL;
#endif
#ifdef DXVERSION8
		if( NULL == ( g_pD3D = Direct3DCreate8( D3D_SDK_VERSION ) ) )
			return E_FAIL;
#endif
		// Create the D3D object.
		g_pFont = new CD3DFont( _T("Arial"), 10, D3DFONT_BOLD );
*/
		// Get the current desktop display mode, so we can set up a back
		// buffer of the same format
		D3DDISPLAYMODE d3ddm;
		//if(!fullscreen)
			if( FAILED( g_pD3D->GetAdapterDisplayMode( D3DADAPTER_DEFAULT, &d3ddm ) ) )
				return E_FAIL;
		
		X_Res = MAX_X;
		Y_Res = MAX_Y;

		if(!fullscreen)
		{
			RECT Size;
			GetClientRect(hWnd, &Size);

			X_Res=(Size.right-Size.left);
			Y_Res=(Size.bottom-Size.top);
		}
		// Set up the structure used to create the D3DDevice. Since we are now
		// using more complex geometry, we will create a device with a zbuffer.
		D3DPRESENT_PARAMETERS d3dpp;
		ZeroMemory( &d3dpp, sizeof(d3dpp) );

		d3dpp.Windowed = !fullscreen;
		d3dpp.SwapEffect = /*fullscreen?D3DSWAPEFFECT_FLIP:*/D3DSWAPEFFECT_DISCARD;
		d3dpp.BackBufferCount = 0;
		d3dpp.BackBufferWidth = X_Res;
		d3dpp.BackBufferHeight = Y_Res;
		d3dpp.BackBufferFormat = /*fullscreen?D3DFMT_D32:*/d3ddm.Format;
		d3dpp.MultiSampleType = FSAAMode;
		d3dpp.EnableAutoDepthStencil = true;
		d3dpp.AutoDepthStencilFormat = (highzbuffer==true)?D3DFMT_D24X8:D3DFMT_D16;//24X8;//24X8; //kyro FIX - no stencil buffer
		d3dpp.hDeviceWindow=hWnd;
		
		if( FAILED( hr=g_pd3dDevice->Reset(&d3dpp) ) ) 
		// Create the D3DDevice
/*
		if( FAILED( hr=g_pD3D->CreateDevice( D3DADAPTER_DEFAULT, (enum _D3DDEVTYPE)Device, hWnd,
										  //D3DCREATE_FPU_PRESERVE|
										  //D3DCREATE_MULTITHREADED|
										  //D3DCREATE_PUREDEVICE| 	// kyro FIX - no hw TnL
										  //D3DCREATE_HARDWARE_VERTEXPROCESSING,
										  D3DCREATE_SOFTWARE_VERTEXPROCESSING,
										  &d3dpp, &g_pd3dDevice ) ) )
										  */
		{
			bCleanup=true;
			//PostQuitMessage( 0 );
			return E_FAIL;
		}

		
		//		if(memory==0)
		memory=g_pd3dDevice->GetAvailableTextureMem();
		/*
		if(D3DMULTISAMPLE_NONE!=FSAAMode)
		{
			g_pd3dDevice->Release();
			g_pd3dDevice=NULL;


			d3dpp.MultiSampleType = FSAAMode;

			// Create the D3DDevice
			if( FAILED( hr=g_pD3D->CreateDevice( D3DADAPTER_DEFAULT, (enum _D3DDEVTYPE)Device, hWnd,
											  //D3DCREATE_FPU_PRESERVE|
											  //D3DCREATE_MULTITHREADED|
											  //D3DCREATE_PUREDEVICE| 	// kyro FIX - no hw TnL
											  //D3DCREATE_HARDWARE_VERTEXPROCESSING,
											  D3DCREATE_SOFTWARE_VERTEXPROCESSING,
											  &d3dpp, &g_pd3dDevice ) ) )
			{
				bCleanup=true;
				//PostQuitMessage( 0 );
				return E_FAIL;
			}
			memory=g_pd3dDevice->GetAvailableTextureMem();
		}*/

		// Get the device caps
#ifdef DXVERSION9
		D3DCAPS9 d3dCaps;
#endif
#ifdef DXVERSION8
		D3DCAPS8 d3dCaps;
#endif
	    

		g_pd3dDevice->GetDeviceCaps( &d3dCaps );
		MaxAniso=d3dCaps.MaxAnisotropy;
		
		if(bFog)
		{
			float Density = 0.0125;  // For exponential modes
 
			// Enable fog blending.
			g_pd3dDevice->SetRenderState(D3DRS_FOGENABLE, TRUE);
 
			// Set the fog color.
			g_pd3dDevice->SetRenderState(D3DRS_FOGCOLOR, 0x00ff0000);
    
			g_pd3dDevice->SetRenderState(D3DRS_FOGTABLEMODE, D3DFOG_EXP);
			g_pd3dDevice->SetRenderState(D3DRS_FOGDENSITY, *(DWORD *)(&Density));
		}

    
		// Turn on clipping
		g_pd3dDevice->SetRenderState( D3DRS_CLIPPING, TRUE );

		// Turn off culling
		g_pd3dDevice->SetRenderState( D3DRS_CULLMODE, D3DCULL_NONE );

		// Turn off D3D lighting
		g_pd3dDevice->SetRenderState( D3DRS_LIGHTING, FALSE );

		// Turn on the zbuffer
		g_pd3dDevice->SetRenderState( D3DRS_ZENABLE, TRUE );

		return S_OK;
}




//-----------------------------------------------------------------------------
// Name: InitGeometry(0)
// Desc: Create the textures and vertex buffers
//-----------------------------------------------------------------------------

HRESULT InitTexture()
{
	if(g_pTexture!=NULL)
	{
		g_pTexture->Release();
		g_pTexture=NULL;
	}

    // Use D3DX to create a texture from a file based image
	if( FAILED( D3DXCreateTextureFromResourceEx(g_pd3dDevice,
												GetModuleHandle(NULL),
												MAKEINTRESOURCE(101),
												D3DX_DEFAULT,
												D3DX_DEFAULT,
												(!bMipEnable), // mip
												0,
												D3DFMT_UNKNOWN,
												D3DPOOL_MANAGED,//DEFAULT,
												D3DX_FILTER_TRIANGLE,
												D3DX_FILTER_TRIANGLE,
												0xff000000,
												NULL,
												NULL,
												&g_pTexture ) ) )
		return E_FAIL;

	return S_OK;
}

HRESULT InitGeometry(double time)
{


#ifdef DXVERSION9
if( g_pVB == NULL )
    // Create the vertex buffer for the first object
    if( FAILED( g_pd3dDevice->CreateVertexBuffer( Obj1_NumVertices*2*sizeof(CUSTOMVERTEX),
                                                  0, D3DFVF_CUSTOMVERTEX,
                                                  D3DPOOL_MANAGED, &g_pVB, 0 ) ) )
    {
        return E_FAIL;
    }
#endif
#ifdef DXVERSION8
if( g_pVB == NULL )
    // Create the vertex buffer for the first object
    if( FAILED( g_pd3dDevice->CreateVertexBuffer( Obj1_NumVertices*2*sizeof(CUSTOMVERTEX),
                                                  0, D3DFVF_CUSTOMVERTEX,
                                                  D3DPOOL_MANAGED, &g_pVB ) ) )
    {
        return E_FAIL;
    }
#endif

    // Fill the vertex buffer. We are setting the tu and tv texture
    // coordinates, which range from 0.0 to 1.0
    CUSTOMVERTEX* pVertices;
#ifdef DXVERSION9
    if( FAILED( g_pVB->Lock( 0, 0, (VOID**)&pVertices, 0 ) ) )
        return E_FAIL;
#endif
#ifdef DXVERSION8
    if( FAILED( g_pVB->Lock( 0, 0, (unsigned char**)&pVertices, 0 ) ) )
        return E_FAIL;
#endif
	double i2;
    for( DWORD i=0; i<Obj1_NumVertices; i++ )
    {
		if(g_bAnimate)
			i2=((double)i) + (time * 0.00005);
        else
			i2=i;

		double theta = ((double)2*D3DX_PI*i2)/((double)Obj1_NumVertices-1);

        pVertices[2*i+0].position = D3DXVECTOR3( sinl(theta)*1.5,-1.0, 2.0*cosl(theta) );
        pVertices[2*i+0].color    = 0xffffffff;
        pVertices[2*i+0].tu       = (((DOUBLE)i*2.0)/((DOUBLE)Obj1_NumVertices-1))-0.5;
        pVertices[2*i+0].tv       = 8.0f;

        pVertices[2*i+1].position = D3DXVECTOR3( sinl(theta)*0.25, 32.0, 0.8*cosl(theta) );
        pVertices[2*i+1].color    = 0xff808080;
        pVertices[2*i+1].tu       = (((DOUBLE)i*2.0)/((DOUBLE)Obj1_NumVertices-1))-0.5;
        pVertices[2*i+1].tv       = 4.0f;
    }
    g_pVB->Unlock();


	
#ifdef DXVERSION9
    // Create the vertex buffer for the first object
if( g_pVB2 == NULL )
    if( FAILED( g_pd3dDevice->CreateVertexBuffer( Obj2_NumVertices*sizeof(CUSTOMVERTEX2),
                                                  0, D3DFVF_CUSTOMVERTEX2,
                                                  D3DPOOL_MANAGED, &g_pVB2, 0 ) ) )
    {
        return E_FAIL;
    }

if( g_pVB3 == NULL )
    if( FAILED( g_pd3dDevice->CreateVertexBuffer( Obj3_NumVertices*sizeof(CUSTOMVERTEX3),
                                                  0, D3DFVF_CUSTOMVERTEX3,
                                                  D3DPOOL_MANAGED, &g_pVB3, 0 ) ) )
    {
        return E_FAIL;
    }
#endif
#ifdef DXVERSION8
    // Create the vertex buffer for the first object
if( g_pVB2 == NULL )
    if( FAILED( g_pd3dDevice->CreateVertexBuffer( Obj2_NumVertices*sizeof(CUSTOMVERTEX2),
                                                  0, D3DFVF_CUSTOMVERTEX2,
                                                  D3DPOOL_MANAGED, &g_pVB2 ) ) )
    {
        return E_FAIL;
    }
if( g_pVB3 == NULL )
    if( FAILED( g_pd3dDevice->CreateVertexBuffer( Obj3_NumVertices*sizeof(CUSTOMVERTEX3),
                                                  0, D3DFVF_CUSTOMVERTEX3,
                                                  D3DPOOL_MANAGED, &g_pVB3 ) ) )
    {
        return E_FAIL;
    }
#endif

    // Fill the vertex buffer. We are setting the tu and tv texture
    // coordinates, which range from 0.0 to 1.0
    CUSTOMVERTEX2* pVertices2;
#ifdef DXVERSION9
    if( FAILED( g_pVB2->Lock( 0, 0, (VOID**)&pVertices2, 0 ) ) )
        return E_FAIL;
#endif
#ifdef DXVERSION8
    if( FAILED( g_pVB2->Lock( 0, 0, (unsigned char**)&pVertices2, 0 ) ) )
        return E_FAIL;
#endif

    double theta;
	double Size=Y_Res/2.1;
	double X=X_Res/2.0;
	double Y=Y_Res/2.0;
//	float i2=0.0;
	for( i=0; i<Obj2_NumVertices; i+=3 )
    {
		
		if(g_bAnimate)
			i2=((double)i) + (time * 0.0001);
        else
			i2=i;

        theta = (2.0*D3DX_PI*(double)(i2))/((double)Obj2_NumVertices);

        pVertices2[i+0].position = D3DXVECTOR4( X+Size*sinl(theta), Y+Size*cosl(theta)+8.0, 0.5, 1.0 );
        pVertices2[i+0].color    = D3DCOLOR_ARGB(255, 255, i, i*5);

        pVertices2[i+1].position = D3DXVECTOR4( 8.0+X+Size*sinl(theta), Y+Size*cosl(theta)-8.0, 0.5, 1.0);
        pVertices2[i+1].color    = D3DCOLOR_ARGB(255, 255, i, i*4);

        pVertices2[i+2].position = D3DXVECTOR4( X, Y, 0.5f, 1.0f );
        pVertices2[i+2].color    = D3DCOLOR_ARGB(255, 255-i, i*3, i*4);


    }
    g_pVB2->Unlock();
	

	
	
    // Fill the vertex buffer. We are setting the tu and tv texture
    // coordinates, which range from 0.0 to 1.0
    CUSTOMVERTEX3* pVertices3;
#ifdef DXVERSION9
    if( FAILED( g_pVB3->Lock( 0, 0, (VOID**)&pVertices3, 0 ) ) )
        return E_FAIL;
#endif
#ifdef DXVERSION8
    if( FAILED( g_pVB3->Lock( 0, 0, (unsigned char**)&pVertices3, 0 ) ) )
        return E_FAIL;
#endif

		int x, y, xsize, ysize;

		xsize=180;
		ysize=xsize/3;
		x=(X_Res-xsize)-2;
		y=(Y_Res-ysize)-1;

        pVertices3[0].position = D3DXVECTOR4( x+0.0f, y+0.0f, 0.2f, 1.0f );
        pVertices3[0].color    = D3DCOLOR_ARGB(255, 255, 255, 255);
        pVertices3[0].tu    = 0.0f;
        pVertices3[0].tv    = 0.0f;

        pVertices3[1].position = D3DXVECTOR4( x+xsize*1.0f, y+0.0f, 0.2f, 1.0f );
        pVertices3[1].color    = D3DCOLOR_ARGB(255, 255, 255, 255);
        pVertices3[1].tu    = 1.0f;
        pVertices3[1].tv    = 0.0f;

        pVertices3[3].position = D3DXVECTOR4( x+xsize*1.0f, y+ysize*1.0f, 0.2f, 1.0f );
        pVertices3[3].color    = D3DCOLOR_ARGB(255, 255, 255, 255);
        pVertices3[3].tu    = 1.0f;
        pVertices3[3].tv    = 1.0f;

        pVertices3[2].position = D3DXVECTOR4( x+0.0f, y+ysize*1.0f, 0.2f, 1.0f );
        pVertices3[2].color    = D3DCOLOR_ARGB(255, 255, 255, 255);
        pVertices3[2].tu    = 0.0f;
        pVertices3[2].tv    = 1.0f;


    
    g_pVB3->Unlock();
	
	bCleanup=false;

    return S_OK;
}

//-----------------------------------------------------------------------------
// Name: SaveBMP()
// Desc: Saves a BMP from the backbuffer
//-----------------------------------------------------------------------------

void SaveBMP()
{

#ifdef DXVERSION9
	LPDIRECT3DSURFACE9 pSrcSurface;
#endif
#ifdef DXVERSION8
	LPDIRECT3DSURFACE8 pSrcSurface;
#endif
	if(D3D_OK==g_pd3dDevice->GetBackBuffer(0, D3DBACKBUFFER_TYPE_MONO, &pSrcSurface))
	{
		static char tmp[255];
		sprintf(tmp, "pic_%d_%d.bmp", TimeStamp, Sequence_Length-StartBMP_Sequence);
    	if(D3D_OK==D3DXSaveSurfaceToFile(tmp,
										D3DXIFF_BMP,
										pSrcSurface,
										NULL,
										NULL))
		{
			//Beep(1000,15);
			//MessageBox(hWnd, "Saved!", "Info", MB_OK);

		}
		//else
		//	MessageBox(hWnd, "Saveing failed! (D3DXSaveSurfaceToFile)", "Info", MB_OK);

		pSrcSurface->Release();
		pSrcSurface=NULL;
	}
	//else
	//	MessageBox(hWnd, "Saveing failed! (GetBackBuffer)", "Info", MB_OK);

}
//-----------------------------------------------------------------------------
// Name: Cleanup()
// Desc: Releases all previously initialized objects
//-----------------------------------------------------------------------------
VOID Cleanup()
{
	bCleanup=true;

    if( g_pTexture != NULL )
	{
        g_pTexture->Release();
		g_pTexture=NULL;
	}
   
	if( g_pVB != NULL )
	{
        g_pVB->Release();
		g_pVB=NULL;

    }

	if( g_pVB2 != NULL )
	{
        g_pVB2->Release();
		g_pVB2=NULL;

    }

	if( g_pVB3 != NULL )
	{
        g_pVB3->Release();
		g_pVB3=NULL;

    }
	
	if( g_pFont != NULL )
	{
		g_pFont->InvalidateDeviceObjects();
		g_pFont->DeleteDeviceObjects();
		//SAFE_DELETE(g_pFont);
		//g_pFont->InvalidateDeviceObjects
		g_pFont=NULL;
	}

    if( g_pd3dDevice != NULL )
	{
        g_pd3dDevice->Release();
		g_pd3dDevice=NULL;
	}

    if(g_pD3D != NULL )
	{
        g_pD3D->Release();
		g_pD3D=NULL;
	}
}

//-----------------------------------------------------------------------------
// Name: Cleanup_Quick()
// Desc: Releases all previously initialized objects
//-----------------------------------------------------------------------------
VOID Cleanup_Quick()
{
	bCleanup=true;
/*
    if( g_pTexture != NULL )
	{
        g_pTexture->Release();
		g_pTexture=NULL;
	}
    
	if( g_pVB != NULL )
	{
        g_pVB->Release();
		g_pVB=NULL;

    }

	if( g_pVB2 != NULL )
	{
        g_pVB2->Release();
		g_pVB2=NULL;

    }
	*/
	/*
	if( g_pFont != NULL )
	{
		g_pFont->InvalidateDeviceObjects();
		g_pFont->DeleteDeviceObjects();
		//SAFE_DELETE(g_pFont);
		//g_pFont->InvalidateDeviceObjects
		g_pFont=NULL;
	}

    if( g_pd3dDevice != NULL )
	{
        g_pd3dDevice->Release();
		g_pd3dDevice=NULL;
	}

    if(g_pD3D != NULL )
	{
        g_pD3D->Release();
		g_pD3D=NULL;
	}
	*/
}


//-----------------------------------------------------------------------------
// Name: SetupMatrices()
// Desc: Sets up the world, view, and projection transform matrices.
//-----------------------------------------------------------------------------
VOID SetupMatrices()
{
    // For our world matrix, we will just leave it as the identity
    D3DXMATRIX matWorld, matWorld2;
    D3DXMatrixIdentity( &matWorld );
    D3DXMatrixRotationX( &matWorld, 2 );

    g_pd3dDevice->SetTransform( D3DTS_WORLD, &matWorld );

    // Set up our view matrix. A view matrix can be defined given an eye point,
    // a point to lookat, and a direction for which way is up. Here, we set the
    // eye five units back along the z-axis and up three units, look at the
    // origin, and define "up" to be in the y-direction.
    D3DXMATRIX matView;
    D3DXMatrixLookAtLH( &matView, &D3DXVECTOR3( 0.0f, 3.0f,-5.0f ),
                                  &D3DXVECTOR3( 0.0f, 0.0f, 0.0f ),
                                  &D3DXVECTOR3( 0.0f, 1.0f, 0.0f ) );
    g_pd3dDevice->SetTransform( D3DTS_VIEW, &matView );

    // For the projection matrix, we set up a perspective transform (which
    // transforms geometry from 3D view space to 2D viewport space, with
    // a perspective divide making objects smaller in the distance). To build
    // a perpsective transform, we need the field of view (1/4 pi is common),
    // the aspect ratio, and the near and far clipping planes (which define at
    // what distances geometry should be no longer be rendered).
    D3DXMATRIX matProj;
    D3DXMatrixPerspectiveFovLH( &matProj, D3DX_PI/4, 1.0f, 1.0f, 100.0f );
    g_pd3dDevice->SetTransform( D3DTS_PROJECTION, &matProj );
}

//-----------------------------------------------------------------------------
// Name: ClearMatrices()
// Desc: Sets up the world, view, and projection transform matrices.
//-----------------------------------------------------------------------------
VOID ClearMatrices()
{
    // For our world matrix, we will just leave it as the identity
    D3DXMATRIX matWorld;
    D3DXMatrixIdentity( &matWorld );
    //D3DXMatrixRotationX( &matWorld, 2 );
    g_pd3dDevice->SetTransform( D3DTS_WORLD, &matWorld );

    // Set up our view matrix. A view matrix can be defined given an eye point,
    // a point to lookat, and a direction for which way is up. Here, we set the
    // eye five units back along the z-axis and up three units, look at the
    // origin, and define "up" to be in the y-direction.
    D3DXMATRIX matView;
    D3DXMatrixIdentity( &matView );
    /*
	D3DXMatrixLookAtLH( &matView, &D3DXVECTOR3( 0.0f, 3.0f,-5.0f ),
                                  &D3DXVECTOR3( 0.0f, 0.0f, 0.0f ),
                                  &D3DXVECTOR3( 0.0f, 1.0f, 0.0f ) );
    */
	g_pd3dDevice->SetTransform( D3DTS_VIEW, &matView );

    // For the projection matrix, we set up a perspective transform (which
    // transforms geometry from 3D view space to 2D viewport space, with
    // a perspective divide making objects smaller in the distance). To build
    // a perpsective transform, we need the field of view (1/4 pi is common),
    // the aspect ratio, and the near and far clipping planes (which define at
    // what distances geometry should be no longer be rendered).
    D3DXMATRIX matProj;
    D3DXMatrixIdentity( &matProj );
    //D3DXMatrixPerspectiveFovLH( &matProj, D3DX_PI/4, 1.0f, 1.0f, 100.0f );
    g_pd3dDevice->SetTransform( D3DTS_PROJECTION, &matProj );
}



//-----------------------------------------------------------------------------
// Name: Render()
// Desc: 7s the scene
//-----------------------------------------------------------------------------
VOID Render()
{
	if(bCleanup)
		return;
    
	// Clear the backbuffer and the zbuffer
	D3DRECT ClearRect;

	ClearRect.x1=0;
	ClearRect.x2=X_Res/2;
	ClearRect.y1=0;
	ClearRect.y2=Y_Res;

    g_pd3dDevice->Clear( 0, NULL, D3DCLEAR_TARGET|D3DCLEAR_ZBUFFER,
                         D3DCOLOR_XRGB(0, 0, 0), 1.0f, 0 );
	ClearRect.x1=X_Res/2;
	ClearRect.x2=X_Res-1;
	ClearRect.y1=0;
	ClearRect.y2=Y_Res;

    g_pd3dDevice->Clear( 1, &ClearRect, D3DCLEAR_TARGET|D3DCLEAR_ZBUFFER,
                         D3DCOLOR_XRGB(255, 255, 255), 1.0f, 0 );

    // Begin the scene
    g_pd3dDevice->BeginScene();

    // Setup the world, view, and projection matrices
    SetupMatrices();

    // Setup our texture. Using textures introduces the texture stage states,
    // which govern how textures get blended together (in the case of multiple
    // textures) and lighting information. In this case, we are modulating
    // (blending) our texture with the diffuse color of the vertices.
    g_pd3dDevice->SetTexture( 0, g_pTexture );

#ifdef DXVERSION9	
	/*
	g_pd3dDevice->SetSamplerState( 0, D3DSAMP_COLOROP,   D3DTOP_MODULATE );
    g_pd3dDevice->SetSamplerState( 0, D3DSAMP_ALPHAARG1, D3DTA_TEXTURE );
    g_pd3dDevice->SetSamplerState( 0, D3DSAMP_COLORARG2, D3DTA_TEXTURE );
    g_pd3dDevice->SetSamplerState( 0, D3DSAMP_ALPHAOP,   D3DTOP_SELECTARG1 );
   */

	g_pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE,   TRUE );
    g_pd3dDevice->SetRenderState( D3DRS_SRCBLEND,  D3DBLEND_SRCALPHA );
    g_pd3dDevice->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA );

    g_pd3dDevice->SetRenderState( D3DRS_ALPHATESTENABLE, TRUE );
    g_pd3dDevice->SetRenderState( D3DRS_ALPHAREF, 0xFF );
    g_pd3dDevice->SetRenderState( D3DRS_ALPHAFUNC,  D3DCMP_ALWAYS );

	switch(TexFilter)
	{
		case 0:
			g_pd3dDevice->SetSamplerState( 0, D3DSAMP_MAGFILTER ,   D3DTEXF_POINT);
			g_pd3dDevice->SetSamplerState( 0, D3DSAMP_MINFILTER ,   D3DTEXF_POINT);
			g_pd3dDevice->SetSamplerState( 0, D3DSAMP_MIPFILTER ,   D3DTEXF_POINT);
			break;
	
		case 1:
			g_pd3dDevice->SetSamplerState( 0, D3DSAMP_MAGFILTER ,   D3DTEXF_LINEAR);
			g_pd3dDevice->SetSamplerState( 0, D3DSAMP_MINFILTER ,   D3DTEXF_LINEAR);
			g_pd3dDevice->SetSamplerState( 0, D3DSAMP_MIPFILTER ,   D3DTEXF_LINEAR);
			break;

		case 2:
			g_pd3dDevice->SetSamplerState( 0, D3DSAMP_MAGFILTER, D3DTEXF_ANISOTROPIC);
			g_pd3dDevice->SetSamplerState( 0, D3DSAMP_MINFILTER, D3DTEXF_ANISOTROPIC);
			g_pd3dDevice->SetSamplerState( 0, D3DSAMP_MIPFILTER, D3DTEXF_LINEAR);
			g_pd3dDevice->SetSamplerState( 0, D3DSAMP_MAXANISOTROPY, MaxAniso);
			break;
/*
		case 3:
			g_pd3dDevice->SetSamplerState( 0, D3DSAMP_MAGFILTER ,   D3DTEXF_PYRAMIDALQUAD);
			g_pd3dDevice->SetSamplerState( 0, D3DSAMP_MINFILTER ,   D3DTEXF_PYRAMIDALQUAD);
			g_pd3dDevice->SetSamplerState( 0, D3DSAMP_MIPFILTER ,   D3DTEXF_LINEAR);
			break;

		case 4:
			g_pd3dDevice->SetSamplerState( 0, D3DSAMP_MAGFILTER ,   D3DTEXF_GAUSSIANQUAD);
			g_pd3dDevice->SetSamplerState( 0, D3DSAMP_MINFILTER ,   D3DTEXF_GAUSSIANQUAD);
			g_pd3dDevice->SetSamplerState( 0, D3DSAMP_MIPFILTER ,   D3DTEXF_LINEAR);
			break;
*/
		default:
			break;
	}
#endif
#ifdef DXVERSION8	
	/*
	g_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_MODULATE );
    g_pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE );
    g_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG2, D3DTA_TEXTURE );
    g_pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAOP,   D3DTOP_SELECTARG1 );
   */

	g_pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE,   TRUE );
    g_pd3dDevice->SetRenderState( D3DRS_SRCBLEND,  D3DBLEND_SRCALPHA );
    g_pd3dDevice->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA );

    g_pd3dDevice->SetRenderState( D3DRS_ALPHATESTENABLE, TRUE );
    g_pd3dDevice->SetRenderState( D3DRS_ALPHAREF, 0xFF );
    g_pd3dDevice->SetRenderState( D3DRS_ALPHAFUNC,  D3DCMP_ALWAYS );

	switch(TexFilter)
	{
		case 0:
			g_pd3dDevice->SetTextureStageState( 0, D3DTSS_MAGFILTER ,   D3DTEXF_POINT);
			g_pd3dDevice->SetTextureStageState( 0, D3DTSS_MINFILTER ,   D3DTEXF_POINT);
			g_pd3dDevice->SetTextureStageState( 0, D3DTSS_MIPFILTER ,   D3DTEXF_POINT);
			break;
	
		case 1:
			g_pd3dDevice->SetTextureStageState( 0, D3DTSS_MAGFILTER ,   D3DTEXF_LINEAR);
			g_pd3dDevice->SetTextureStageState( 0, D3DTSS_MINFILTER ,   D3DTEXF_LINEAR);
			g_pd3dDevice->SetTextureStageState( 0, D3DTSS_MIPFILTER ,   D3DTEXF_LINEAR);
			break;

		case 2:
			g_pd3dDevice->SetTextureStageState( 0, D3DTSS_MAGFILTER, D3DTEXF_ANISOTROPIC);
			g_pd3dDevice->SetTextureStageState( 0, D3DTSS_MINFILTER, D3DTEXF_ANISOTROPIC);
			g_pd3dDevice->SetTextureStageState( 0, D3DTSS_MIPFILTER, D3DTEXF_LINEAR);
			g_pd3dDevice->SetTextureStageState( 0, D3DTSS_MAXANISOTROPY, MaxAniso);
			break;
/*
		case 3:
			g_pd3dDevice->SetTextureStageState( 0, D3DTSS_MAGFILTER ,   D3DTEXF_FLATCUBIC);
			g_pd3dDevice->SetTextureStageState( 0, D3DTSS_MINFILTER ,   D3DTEXF_LINEAR);
			g_pd3dDevice->SetTextureStageState( 0, D3DTSS_MIPFILTER ,   D3DTEXF_LINEAR);
			break;

		case 4:
			g_pd3dDevice->SetTextureStageState( 0, D3DTSS_MAGFILTER ,   D3DTEXF_GAUSSIANCUBIC);
			g_pd3dDevice->SetTextureStageState( 0, D3DTSS_MINFILTER ,   D3DTEXF_LINEAR);
			g_pd3dDevice->SetTextureStageState( 0, D3DTSS_MIPFILTER ,   D3DTEXF_LINEAR);
			break;
*/
		default:
			break;
	}
#endif

    // Render the vertex buffer contents
if(mode)
{
#ifdef DXVERSION9	
	g_pd3dDevice->SetStreamSource( 0, g_pVB, 0, sizeof(CUSTOMVERTEX) );

    g_pd3dDevice->SetVertexShader( NULL );
    g_pd3dDevice->SetFVF( D3DFVF_CUSTOMVERTEX );
#endif
#ifdef DXVERSION8	
	g_pd3dDevice->SetStreamSource( 0, g_pVB, sizeof(CUSTOMVERTEX) );
    g_pd3dDevice->SetVertexShader( D3DFVF_CUSTOMVERTEX );
#endif
	g_pd3dDevice->DrawPrimitive( D3DPT_TRIANGLESTRIP, 0, 2*Obj1_NumVertices-2 );
}
else
{

    ClearMatrices();
    g_pd3dDevice->SetTexture( 0, NULL );

#ifdef DXVERSION9	
    // Render the vertex buffer contents
    g_pd3dDevice->SetStreamSource( 0, g_pVB2, 0, sizeof(CUSTOMVERTEX2) );

    g_pd3dDevice->SetVertexShader( NULL );
    g_pd3dDevice->SetFVF( D3DFVF_CUSTOMVERTEX2 );
#endif
#ifdef DXVERSION8	
	g_pd3dDevice->SetStreamSource( 0, g_pVB2, sizeof(CUSTOMVERTEX2) );
    g_pd3dDevice->SetVertexShader( D3DFVF_CUSTOMVERTEX2 );
#endif

	g_pd3dDevice->DrawPrimitive( D3DPT_TRIANGLELIST, 0, Obj2_NumVertices/3 );
}

	g_pFont->InitDeviceObjects( g_pd3dDevice );
    g_pFont->RestoreDeviceObjects();
	g_pFont->DrawText( 2,  0, D3DCOLOR_ARGB(255,255,255,0), g_tmpStr );
	char tmpStr[255];
	itoa( ( (memory_sub-g_pd3dDevice->GetAvailableTextureMem())-(memory-g_pd3dDevice->GetAvailableTextureMem()) ), &tmpStr[0], 10);
	lstrcat(tmpStr, " bytes used for FSAA");
	char tmpStr2[255];
	sprintf(tmpStr2, "\n(LEFT/RIGHT) animation speed: %li\n(UP/DOWN) number of pics in a sequence: %li\n(current %li)", Sequence_Speed, Sequence_Length, Sequence_Length-StartBMP_Sequence); 
	lstrcat(tmpStr, tmpStr2);
	g_pFont->DrawText( 2,  20, D3DCOLOR_ARGB(255,255,255,0), tmpStr );
    
	g_pFont->InvalidateDeviceObjects();
	g_pFont->DeleteDeviceObjects();

    // End the scene
    g_pd3dDevice->EndScene();
}




//-----------------------------------------------------------------------------
// Name: MsgProc()
// Desc: The window's message handler
//-----------------------------------------------------------------------------
LRESULT WINAPI MsgProc( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam )
{
	bCleanup=true;
	RECT rcWindowBounds, rcWindowClient;
    switch( msg )
    {
		case WM_SYSKEYDOWN:
		case WM_SYSKEYUP:
		case WM_SIZE:
		case WM_SIZING:
		case WM_EXITSIZEMOVE:
			//SetWindowPos(hWnd, HWND_TOPMOST, 1,1, X_Res+1, Y_Res+1, NULL);
			return 1;

		case WM_KEYDOWN:
			if(!KeyDown)
			{
				if (((int)wParam)==VK_ESCAPE)
					PostQuitMessage( 0 );

				if(StartBMP_Sequence==0)
				{
					switch( ((int)wParam) )
					{
						case 'F':
							KeyDown='F';
							bFog=!bFog;
							
							// reinit the whole scene

							// Clean up everything and exit the app
							Cleanup_Quick();
							if( SUCCEEDED( InitD3D( hWnd ) ) )
							{
								// Create the scene geometry
							//	if( SUCCEEDED( InitGeometry(0) ) )
							//	{
									bCleanup=false;
									return 1;
							//	}
							}
							PostQuitMessage( 0 );
							break;
						case VK_F1:
							MessageBox(hWnd, "Usage:\n\nkey --- action:\n---------------\n'F1' - Help\n\n'1' - no FSAA\n'2' - 2x FSAA\n'3' - 3x FSAA\n'4' - 4x FSAA\n'5' - 5x FSAA\n'6' - 6x FSAA\n'7' - 7x FSAA\n'8' - 8x FSAA\n'9' - 9x FSAA\n\n's' - save a screenshot/sequence\n'a' - animate\n'b' - change zbuffer depth(16/32)\n'v' - change fullscreen/windowed\n'd' - change render device (HAL/REF)\n'z' - change 3d object\n'm' - enable/disable mipmapping\n'f' - enable/disable fog\n\n'q' - point filter\n'w' - linear filter\n'e' - anisotropic (max.) filter\n\nESC - exit\n\nPlease report errors to\nthomas@tommti-systems.com", "http://www.ToMMTi-Systems.com - FSAA Tester V2.2", MB_APPLMODAL|MB_OK|MB_ICONSTOP);
							break;

						case 'M':
							KeyDown='M';
							bMipEnable=!bMipEnable;
							// reinit the whole scene

							// Clean up everything and exit the app
							Cleanup_Quick();
									if( SUCCEEDED( InitTexture() ) )
									{
							//if( SUCCEEDED( InitD3D( hWnd ) ) )
							//{
								// Create the scene geometry
							//	if( SUCCEEDED( InitGeometry(0) ) )
							//	{
									bCleanup=false;
									return 1;
							//	}bmp
							}
							PostQuitMessage( 0 );
							break;

						case 'A':
							KeyDown='A';
							g_bAnimate=!g_bAnimate;
							break;

						case 'V':
							KeyDown='V';
							fullscreen=!fullscreen;
							if(fullscreen)
								ShowCursor(false);
							else
								ShowCursor(true);
							// Clean up everything and exit the app
							Cleanup();
							if( SUCCEEDED( PreInit( hWnd ) ) )
							{
								if( SUCCEEDED( InitD3D( hWnd ) ) )
								{
								// Create the scene geometry
									if( SUCCEEDED( InitTexture() ) )
									{
										if( SUCCEEDED( InitGeometry(0) ) )
										{
											bCleanup=false;
											return 1;
										}
									}
								}
							}
							PostQuitMessage( 0 );
							break;
						
						case 'B':
							KeyDown='B';
							highzbuffer=!highzbuffer;
							// Clean up everything and exit the app
							Cleanup();
							if( SUCCEEDED( PreInit( hWnd ) ) )
							{
								if( SUCCEEDED( InitD3D( hWnd ) ) )
								{
									// Create the scene geometry
									if( SUCCEEDED( InitTexture() ) )
									{
										if( SUCCEEDED( InitGeometry(0) ) )
										{
											bCleanup=false;
											return 1;
										}
									}
								}
							}
							PostQuitMessage( 0 );
							break;

						case 'Z':
							KeyDown='Z';
							mode=!mode;
							break;

						case 'Q':
							KeyDown='Q';
							TexFilter=0;
							break;

						case 'W':
							KeyDown='W';
							TexFilter=1;
							break;

						case 'D':
							KeyDown='D';
							if(Device==D3DDEVTYPE_HAL)
								Device=D3DDEVTYPE_REF;
							else
								Device=D3DDEVTYPE_HAL;

							// Clean up everything and exit the app
							Cleanup();
							if( SUCCEEDED( PreInit( hWnd ) ) )
							{
								if( SUCCEEDED( InitD3D( hWnd ) ) )
								{
									if( SUCCEEDED( InitTexture() ) )
									{
										// Create the scene geometry
										if( SUCCEEDED( InitGeometry(0) ) )
										{
											bCleanup=false;
											return 1;
										}
									}
								}
							}
							PostQuitMessage( 0 );
							break;

						case 'E':
							KeyDown='E';
							TexFilter=2;
							break;
						/*
						case 'R':
							KeyDown='R';
							TexFilter=3;
							break;

						case 'T':
							KeyDown='T';
							TexFilter=4;
							break;
						*/
						case VK_RIGHT:
							Sequence_Speed+=1;//(int)sqrt(Sequence_Speed);
							break;
						case VK_LEFT:
							if(Sequence_Speed>1)
//								Sequence_Speed-=(int)sqrt(Sequence_Speed);
								Sequence_Speed-=1;
							break;
						case VK_UP:
							Sequence_Length+=5;
							break;
						case VK_DOWN:
							if(Sequence_Length>5)
								Sequence_Length-=5;
							break;
						case 'S':
							KeyDown='S';
							if(!g_bAnimate)
							{
								TimeStamp=timeGetTime();
								StartBMP_Sequence=0;
								SaveBMP();
							}
							else
							{
								TimeStamp=timeGetTime();
								StartBMP_Sequence=Sequence_Length;
							}
							break;
					
						case '1':
							KeyDown='1';
							FSAAMode = D3DMULTISAMPLE_NONE;
							
							// reinit the whole scene

							// Clean up everything and exit the app
							Cleanup_Quick();
							if( SUCCEEDED( InitD3D( hWnd ) ) )
							{
								// Create the scene geometry
								if( SUCCEEDED( InitGeometry(0) ) )
								{
									bCleanup=false;
									return 1;
								}
							}
							PostQuitMessage( 0 );
							break;

							
						case '2':
							KeyDown='2';
							FSAAMode = D3DMULTISAMPLE_2_SAMPLES;
							
							// reinit the whole scene

							// Clean up everything and exit the app
							Cleanup_Quick();
							if( SUCCEEDED( InitD3D( hWnd ) ) )
							{
								// Create the scene geometry
								if( SUCCEEDED( InitGeometry(0) ) )
								{
									bCleanup=false;
									return 1;
								}
							}
							else
							{
								KeyDown=0;
								//MessageBox(hWnd, "FSAA-Mode not supported!", "Error", MB_OK);
								FSAAMode = D3DMULTISAMPLE_NONE;
								if( SUCCEEDED( InitD3D( hWnd ) ) )
								{
									// Create the scene geometry
									if( SUCCEEDED( InitGeometry(0) ) )
									{
										bCleanup=false;
										return 1;
									}
								}
							}

							PostQuitMessage( 0 );
							break;

						case '3':
							KeyDown='3';
							FSAAMode = D3DMULTISAMPLE_3_SAMPLES;
							
							// reinit the whole scene

							// Clean up everything and exit the app
							Cleanup_Quick();
							if( SUCCEEDED( InitD3D( hWnd ) ) )
							{
								// Create the scene geometry
								if( SUCCEEDED( InitGeometry(0) ) )
								{
									bCleanup=false;
									return 1;
								}
							}
							else
							{
								KeyDown=0;
								//MessageBox(hWnd, "FSAA-Mode not supported!", "Error", MB_OK);
								FSAAMode = D3DMULTISAMPLE_NONE;
								if( SUCCEEDED( InitD3D( hWnd ) ) )
								{
									// Create the scene geometry
									if( SUCCEEDED( InitGeometry(0) ) )
									{
										bCleanup=false;
										return 1;
									}
								}
							}

							PostQuitMessage( 0 );
							break;


						case '4':
							KeyDown='4';
							FSAAMode = D3DMULTISAMPLE_4_SAMPLES;
							
							// reinit the whole scene

							// Clean up everything and exit the app
							Cleanup_Quick();
							if( SUCCEEDED( InitD3D( hWnd ) ) )
							{
								// Create the scene geometry
								if( SUCCEEDED( InitGeometry(0) ) )
								{
									bCleanup=false;
									return 1;
								}
							}
							else
							{
								KeyDown=0;
								//MessageBox(hWnd, "FSAA-Mode not supported!", "Error", MB_OK);
								FSAAMode = D3DMULTISAMPLE_NONE;
								if( SUCCEEDED( InitD3D( hWnd ) ) )
								{
									// Create the scene geometry
									if( SUCCEEDED( InitGeometry(0) ) )
									{
										bCleanup=false;
										return 1;
									}
								}
							}

							PostQuitMessage( 0 );
							break;

						case '5':
							KeyDown='5';
							FSAAMode = D3DMULTISAMPLE_5_SAMPLES;
							
							// reinit the whole scene

							// Clean up everything and exit the app
							Cleanup_Quick();
							if( SUCCEEDED( InitD3D( hWnd ) ) )
							{
								// Create the scene geometry
								if( SUCCEEDED( InitGeometry(0) ) )
								{
									bCleanup=false;
									return 1;
								}
							}
							else
							{
								KeyDown=0;
								//MessageBox(hWnd, "FSAA-Mode not supported!", "Error", MB_OK);
								FSAAMode = D3DMULTISAMPLE_NONE;
								if( SUCCEEDED( InitD3D( hWnd ) ) )
								{
									// Create the scene geometry
									if( SUCCEEDED( InitGeometry(0) ) )
									{
										bCleanup=false;
										return 1;
									}
								}
							}

							PostQuitMessage( 0 );
							break;
						
						case '6':
							KeyDown='6';
							FSAAMode = D3DMULTISAMPLE_6_SAMPLES;
							
							// reinit the whole scene

							// Clean up everything and exit the app
							Cleanup_Quick();
							if( SUCCEEDED( InitD3D( hWnd ) ) )
							{
								// Create the scene geometry
								if( SUCCEEDED( InitGeometry(0) ) )
								{
									bCleanup=false;
									return 1;
								}
							}
							else
							{
								KeyDown=0;
								//MessageBox(hWnd, "FSAA-Mode not supported!", "Error", MB_OK);
								FSAAMode = D3DMULTISAMPLE_NONE;
								if( SUCCEEDED( InitD3D( hWnd ) ) )
								{
									// Create the scene geometry
									if( SUCCEEDED( InitGeometry(0) ) )
									{
										bCleanup=false;
										return 1;
									}
								}
							}

							PostQuitMessage( 0 );
							break;
						
						case '7':
							KeyDown='7';
							FSAAMode = D3DMULTISAMPLE_7_SAMPLES;
							
							// reinit the whole scene

							// Clean up everything and exit the app
							Cleanup_Quick();
							if( SUCCEEDED( InitD3D( hWnd ) ) )
							{
								// Create the scene geometry
								if( SUCCEEDED( InitGeometry(0) ) )
								{
									bCleanup=false;
									return 1;
								}
							}
							else
							{
								KeyDown=0;
								//MessageBox(hWnd, "FSAA-Mode not supported!", "Error", MB_OK);
								FSAAMode = D3DMULTISAMPLE_NONE;
								if( SUCCEEDED( InitD3D( hWnd ) ) )
								{
									// Create the scene geometry
									if( SUCCEEDED( InitGeometry(0) ) )
									{
										bCleanup=false;
										return 1;
									}
								}
							}

							PostQuitMessage( 0 );
							break;
						
						case '8':
							KeyDown='8';
							FSAAMode = D3DMULTISAMPLE_8_SAMPLES;
							
							// reinit the whole scene

							// Clean up everything and exit the app
							Cleanup_Quick();
							if( SUCCEEDED( InitD3D( hWnd ) ) )
							{
								// Create the scene geometry
								if( SUCCEEDED( InitGeometry(0) ) )
								{
									bCleanup=false;
									return 1;
								}
							}
							else
							{
								KeyDown=0;
								//MessageBox(hWnd, "FSAA-Mode not supported!", "Error", MB_OK);
								FSAAMode = D3DMULTISAMPLE_NONE;
								if( SUCCEEDED( InitD3D( hWnd ) ) )
								{
									// Create the scene geometry
									if( SUCCEEDED( InitGeometry(0) ) )
									{
										bCleanup=false;
										return 1;
									}
								}
							}

							PostQuitMessage( 0 );
							break;
						

						case '9':
							KeyDown='9';
							FSAAMode = D3DMULTISAMPLE_9_SAMPLES;
							
							// reinit the whole scene

							// Clean up everything and exit the app
							Cleanup_Quick();
							if( SUCCEEDED( InitD3D( hWnd ) ) )
							{
								// Create the scene geometry
								if( SUCCEEDED( InitGeometry(0) ) )
								{
									bCleanup=false;
									return 1;
								}
							}
							else
							{
								KeyDown=0;
								//MessageBox(hWnd, "FSAA-Mode not supported!", "Error", MB_OK);
								FSAAMode = D3DMULTISAMPLE_NONE;
								if( SUCCEEDED( InitD3D( hWnd ) ) )
								{
									// Create the scene geometry
									if( SUCCEEDED( InitGeometry(0) ) )
									{
										bCleanup=false;
										return 1;
									}
								}
							}

							PostQuitMessage( 0 );
							break;
					}

				}
			}
			break;

		case WM_KEYUP:
			KeyDown=0;
			break;

        
		case WM_DESTROY:
            PostQuitMessage( 0 );
            return 0;
    }

	
	switch(TexFilter)
	{
		case 0:
			sprintf(g_tmpStr, "%uxFSAA, texture filter: point",FSAAMode);
			break;
		
		case 1:
			sprintf(g_tmpStr, "%uxFSAA, texture filter: linear",FSAAMode);
			break;
		
		case 2:
			sprintf(g_tmpStr, "%uxFSAA, texture filter: anisotropic (%li)",FSAAMode, MaxAniso);
			break;

		case 3:
			sprintf(g_tmpStr, "%uxFSAA, texture filter: pyramidal quad",FSAAMode);
			break;
		
		case 4:
			sprintf(g_tmpStr, "%uxFSAA, texture filter: gaussian quad",FSAAMode);
			break;
		
		default:
			break;
	}
	
		
	if(highzbuffer)
		lstrcat(g_tmpStr, ", zb: 24X8");
	else
		lstrcat(g_tmpStr, ", zb: 16");

	if(bMipEnable)
		lstrcat(g_tmpStr, ", mipmap");
	else
		lstrcat(g_tmpStr, ", no mipmap");

	if(Device==D3DDEVTYPE_REF)
		lstrcat(g_tmpStr, ", REF");
	else
		lstrcat(g_tmpStr, ", HAL");

	if(bFog)
		lstrcat(g_tmpStr, ", fog");
	else
		lstrcat(g_tmpStr, ", no fog");

	bCleanup=false;
    return DefWindowProc( hWnd, msg, wParam, lParam );
}




//-----------------------------------------------------------------------------
// Name: WinMain()
// Desc: The application's entry point
//-----------------------------------------------------------------------------
INT WINAPI WinMain( HINSTANCE hInst, HINSTANCE, LPSTR, INT )
{
	static DWORD diff;

    // Register the window class
    WNDCLASSEX wc = { sizeof(WNDCLASSEX), CS_CLASSDC, MsgProc, 0L, 0L,
                      GetModuleHandle(NULL), NULL, NULL, NULL, NULL,
                      "http://www.ToMMTi-Systems.com - FSAA Tester", NULL };
    RegisterClassEx( &wc );

    // Create the application's window
    HWND hWnd = CreateWindow( "http://www.ToMMTi-Systems.com - FSAA Tester", "http://www.ToMMTi-Systems.com - FSAA Tester",
                              WS_OVERLAPPED, 0, 0, X_Res, Y_Res,
                              GetDesktopWindow(), NULL, wc.hInstance, NULL );
	g_hInstance=wc.hInstance;

	SendMessage(hWnd, WM_SETICON, ICON_SMALL, (LPARAM)LoadIcon(g_hInstance, MAKEINTRESOURCE(101)));
	SendMessage(hWnd, WM_SETICON, ICON_BIG, (LPARAM)LoadIcon(g_hInstance, MAKEINTRESOURCE(101)));
	
	MessageBox(hWnd, "Usage:\n\nkey --- action:\n---------------\n'F1' - Help\n\n'1' - no FSAA\n'2' - 2x FSAA\n'3' - 3x FSAA\n'4' - 4x FSAA\n'5' - 5x FSAA\n'6' - 6x FSAA\n'7' - 7x FSAA\n'8' - 8x FSAA\n'9' - 9x FSAA\n\n's' - save a screenshot/sequence\n'a' - animate\n'b' - change zbuffer depth(16/32)\n'v' - change fullscreen/windowed\n'd' - change render device (HAL/REF)\n'z' - change 3d object\n'm' - enable/disable mipmapping\n'f' - enable/disable fog\n\n'q' - point filter\n'w' - linear filter\n'e' - anisotropic (max.) filter\n\nESC - exit\n\nPlease report errors to\nthomas@tommti-systems.com", "http://www.ToMMTi-Systems.com - FSAA Tester V2.2", MB_APPLMODAL|MB_OK|MB_ICONSTOP);
	//ShowCursor(false);
    // Initialize Direct3D

	if( SUCCEEDED( PreInit( hWnd ) ) )
	if( SUCCEEDED( InitD3D( hWnd ) ) )
    {
        if( SUCCEEDED( InitTexture() ) )
		{
			// Create the scene geometry
			if( SUCCEEDED( InitGeometry(0) ) )
			{
				// Show the window
				ShowWindow( hWnd, SW_SHOWDEFAULT );
				//ShowCursor (FALSE);
				UpdateWindow( hWnd );

				// Enter the message loop
				MSG msg;
				ZeroMemory( &msg, sizeof(msg) );
				while( msg.message!=WM_QUIT )
				{
					if( PeekMessage( &msg, NULL, 0U, 0U, PM_REMOVE ) )
					{
						TranslateMessage( &msg );
						DispatchMessage( &msg );
					}
					else
					{
						if(StartBMP_Sequence==0)
						{
							if(!g_bAnimate)
								diff=0;//timeGetTime();
							else
								diff+=Sequence_Speed;

							InitGeometry(diff);//timeGetTime()-diff);
							Render();

						}
						else
						{
			
							if(!g_bAnimate ||StartBMP_Sequence==Sequence_Length)
								diff=0;//timeGetTime();
							else
								diff+=Sequence_Speed;

							InitGeometry(diff);//timeGetTime()-diff);
							Render();

							SaveBMP();
							StartBMP_Sequence--;

						}
						// Present the backbuffer contents to the display
						g_pd3dDevice->Present( NULL, NULL, NULL, NULL );

						g_pd3dDevice->SetRenderState( D3DRS_ALPHATESTENABLE,    FALSE );
						g_pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE,   FALSE );
					}
				}
			}
		}
    }

    // Clean up everything and exit the app
	Cleanup();
    UnregisterClass( "http://www.ToMMTi-Systems.com - FSAA Tester", wc.hInstance );
    return 0;
}
