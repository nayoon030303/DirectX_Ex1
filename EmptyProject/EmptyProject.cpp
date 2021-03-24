//--------------------------------------------------------------------------------------
// File: EmptyProject.cpp
//
// Copyright (c) Microsoft Corporation. All rights reserved.
//--------------------------------------------------------------------------------------
#include "DXUT.h"
#include "resource.h"

#define width = 640
#define height = 480
#define MAP_PROPERTY_EDGE 100
#define MAP_PROPERTY_VISITING 200
#define MAP_PROPERTY_VISIT 300
#define MAP_PROPERTY_EMPTY 400


LPD3DXSPRITE spr;
LPDIRECT3DTEXTURE9* backgroundTex = nullptr;
LPDIRECT3DTEXTURE9* maskTex = nullptr;
LPDIRECT3DTEXTURE9* dotTex = nullptr;
LPDIRECT3DTEXTURE9* playerTex = nullptr;

int px, py;
int map[640 * 480];
DWORD pixelData[640 * 480];


void updateTex();

//--------------------------------------------------------------------------------------
// Rejects any D3D9 devices that aren't acceptable to the app by returning false
//--------------------------------------------------------------------------------------
bool CALLBACK IsD3D9DeviceAcceptable(D3DCAPS9* pCaps, D3DFORMAT AdapterFormat, D3DFORMAT BackBufferFormat,
    bool bWindowed, void* pUserContext)
{
    // Typically want to skip back buffer formats that don't support alpha blending
    IDirect3D9* pD3D = DXUTGetD3D9Object();
    if (FAILED(pD3D->CheckDeviceFormat(pCaps->AdapterOrdinal, pCaps->DeviceType,
        AdapterFormat, D3DUSAGE_QUERY_POSTPIXELSHADER_BLENDING,
        D3DRTYPE_TEXTURE, BackBufferFormat)))
        return false;

    return true;
}


//--------------------------------------------------------------------------------------
// Before a device is created, modify the device settings as needed
//--------------------------------------------------------------------------------------
bool CALLBACK ModifyDeviceSettings(DXUTDeviceSettings* pDeviceSettings, void* pUserContext)
{
    return true;
}


//--------------------------------------------------------------------------------------
// Create any D3D9 resources that will live through a device reset (D3DPOOL_MANAGED)
// and aren't tied to the back buffer size
//--------------------------------------------------------------------------------------
HRESULT CALLBACK OnD3D9CreateDevice(IDirect3DDevice9* pd3dDevice, const D3DSURFACE_DESC* pBackBufferSurfaceDesc,
    void* pUserContext)
{
    px = 400;
    py = 200;
    //release Map
    for (int i = 0; i < 640 * 480; ++i)
    {
        map[i] = MAP_PROPERTY_EMPTY;
    }
    for (int y = 200; y <= 300; ++y)
    {
        for (int x = 400; x <= 600; ++x)
        {
            map[y * 640 + x] = MAP_PROPERTY_VISIT;
        }
    }
    //edge columns
    for (int y = 200; y <= 300; ++y)
    {
        map[y * 640 + 400] = MAP_PROPERTY_EDGE;
        map[y * 640 + 600] = MAP_PROPERTY_EDGE;
    }
    //edge rows
    for (int x = 400; x <= 600; ++x)
    {
        map[200* 640 + x] = MAP_PROPERTY_EDGE;
        map[300 * 640 + x] = MAP_PROPERTY_EDGE;
    }

    backgroundTex = new LPDIRECT3DTEXTURE9();
    D3DXCreateTextureFromFileExA(
        pd3dDevice,
        "background.png",
        D3DX_DEFAULT_NONPOW2,
        D3DX_DEFAULT_NONPOW2,
        0,
        0,
        D3DFMT_UNKNOWN,
        D3DPOOL_MANAGED,
        D3DX_DEFAULT,
        D3DX_DEFAULT,
        0,
        nullptr,
        nullptr, backgroundTex);


    maskTex = new LPDIRECT3DTEXTURE9();
    D3DXCreateTextureFromFileExA(pd3dDevice, "mask.png", D3DX_DEFAULT_NONPOW2, D3DX_DEFAULT_NONPOW2, 0,
        0,
        D3DFMT_UNKNOWN,
        D3DPOOL_MANAGED,
        D3DX_DEFAULT,
        D3DX_DEFAULT,
        0,
        nullptr,
        nullptr, maskTex);

    dotTex = new LPDIRECT3DTEXTURE9();
    D3DXCreateTextureFromFileExA(pd3dDevice, "dot.png", D3DX_DEFAULT_NONPOW2, D3DX_DEFAULT_NONPOW2, 0,
        0,
        D3DFMT_UNKNOWN,
        D3DPOOL_MANAGED,
        D3DX_DEFAULT,
        D3DX_DEFAULT,
        0,
        nullptr,
        nullptr, dotTex);

    playerTex = new LPDIRECT3DTEXTURE9();
    D3DXCreateTextureFromFileExA(pd3dDevice, "player.png", D3DX_DEFAULT_NONPOW2, D3DX_DEFAULT_NONPOW2, 0,
        0,
        D3DFMT_UNKNOWN,
        D3DPOOL_MANAGED,
        D3DX_DEFAULT,
        D3DX_DEFAULT,
        0,
        nullptr,
        nullptr, playerTex);


    RECT rc = { 0,0,640,480 };
    D3DLOCKED_RECT lockRc;
    if (SUCCEEDED((*backgroundTex)->LockRect(0, &lockRc, &rc, 0)))
    {
        DWORD* d = (DWORD*)lockRc.pBits;
        memcpy(pixelData, d, 640 * 480 * 4);
        (*backgroundTex)->UnlockRect(0);
    }

    updateTex();

    D3DXCreateSprite(pd3dDevice, &spr);

    return S_OK;
}
void updateTex()
{
    RECT rc = { 0,0,640,480 };
    D3DLOCKED_RECT lockRc;
    if (SUCCEEDED((*maskTex)->LockRect(0, &lockRc, &rc, 0)))
    {
        for (int i = 0; i < 640 * 480; ++i)
        {
            if (map[i] == MAP_PROPERTY_VISIT)
            {
                DWORD* d = (DWORD*)lockRc.pBits;
                d[i] = pixelData[i];
            }
       }
    }
}

//--------------------------------------------------------------------------------------
// Create any D3D9 resources that won't live through a device reset (D3DPOOL_DEFAULT) 
// or that are tied to the back buffer size 
//--------------------------------------------------------------------------------------
HRESULT CALLBACK OnD3D9ResetDevice(IDirect3DDevice9* pd3dDevice, const D3DSURFACE_DESC* pBackBufferSurfaceDesc,
    void* pUserContext)
{
    return S_OK;
}


//--------------------------------------------------------------------------------------
// Handle updates to the scene.  This is called regardless of which D3D API is used
//--------------------------------------------------------------------------------------
void CALLBACK OnFrameMove(double fTime, float fElapsedTime, void* pUserContext)
{
    if ((GetAsyncKeyState(VK_LEFT) * 0x800) != 0)
    {
        px -= 1;
    }
    if ((GetAsyncKeyState(VK_RIGHT) * 0x800) != 0)
    {
        px += 1;
    }
    if ((GetAsyncKeyState(VK_UP) * 0x800) != 0)
    {
        py -= 1;
    }
    if ((GetAsyncKeyState(VK_DOWN) * 0x800) != 0)
    {
        py += 1;
    }
}


//--------------------------------------------------------------------------------------
// Render the scene using the D3D9 device
//--------------------------------------------------------------------------------------
void CALLBACK OnD3D9FrameRender(IDirect3DDevice9* pd3dDevice, double fTime, float fElapsedTime, void* pUserContext)
{
    HRESULT hr;

    // Clear the render target and the zbuffer 
    V(pd3dDevice->Clear(0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, D3DCOLOR_ARGB(0, 45, 50, 170), 1.0f, 0));

    // Render the scene
    if (SUCCEEDED(pd3dDevice->BeginScene()))
    {
        spr->Begin(D3DXSPRITE_ALPHABLEND);
        spr->Draw(*maskTex, nullptr, nullptr, nullptr, D3DCOLOR_RGBA(255, 255, 255, 255));
        
        //EDGE
        for (int y = 0; y < 480; ++y)
        {
            for (int x = 0; x < 640; ++x)
            {
                if (map[y * 640 + x] == MAP_PROPERTY_EDGE)
                {
                    D3DXVECTOR3 pos(x, y, 0);
                    spr->Draw(*dotTex, nullptr, nullptr, &pos, D3DCOLOR_RGBA(0, 0, 0, 255));
                }

            }

        }
        
        D3DXVECTOR3 playerPos(px-4, py-4, 0);
        spr->Draw(*playerTex, nullptr, nullptr, &playerPos, D3DCOLOR_RGBA(255, 255, 255, 255));
        
        spr->End();

        V(pd3dDevice->EndScene());
    }
}


//--------------------------------------------------------------------------------------
// Handle messages to the application 
//--------------------------------------------------------------------------------------
LRESULT CALLBACK MsgProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam,
    bool* pbNoFurtherProcessing, void* pUserContext)
{
    return 0;
}


//--------------------------------------------------------------------------------------
// Release D3D9 resources created in the OnD3D9ResetDevice callback 
//--------------------------------------------------------------------------------------
void CALLBACK OnD3D9LostDevice(void* pUserContext)
{
}


//--------------------------------------------------------------------------------------
// Release D3D9 resources created in the OnD3D9CreateDevice callback 
//--------------------------------------------------------------------------------------
void CALLBACK OnD3D9DestroyDevice(void* pUserContext)
{
    spr->Release();
    (*maskTex)->Release();
    (*backgroundTex)->Release();
    (*dotTex)->Release();
    (*playerTex)->Release();
}


//--------------------------------------------------------------------------------------
// Initialize everything and go into a render loop
//--------------------------------------------------------------------------------------
INT WINAPI wWinMain(HINSTANCE, HINSTANCE, LPWSTR, int)
{
    // Enable run-time memory check for debug builds.
#if defined(DEBUG) | defined(_DEBUG)
    _CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
#endif

    // Set the callback functions
    DXUTSetCallbackD3D9DeviceAcceptable(IsD3D9DeviceAcceptable);
    DXUTSetCallbackD3D9DeviceCreated(OnD3D9CreateDevice);
    DXUTSetCallbackD3D9DeviceReset(OnD3D9ResetDevice);
    DXUTSetCallbackD3D9FrameRender(OnD3D9FrameRender);
    DXUTSetCallbackD3D9DeviceLost(OnD3D9LostDevice);
    DXUTSetCallbackD3D9DeviceDestroyed(OnD3D9DestroyDevice);
    DXUTSetCallbackDeviceChanging(ModifyDeviceSettings);
    DXUTSetCallbackMsgProc(MsgProc);
    DXUTSetCallbackFrameMove(OnFrameMove);

    // TODO: Perform any application-level initialization here

    // Initialize DXUT and create the desired Win32 window and Direct3D device for the application
    DXUTInit(true, true); // Parse the command line and show msgboxes
    DXUTSetHotkeyHandling(true, true, true);  // handle the default hotkeys
    DXUTSetCursorSettings(true, true); // Show the cursor and clip it when in full screen
    DXUTCreateWindow(L"EmptyProject");
    DXUTCreateDevice(true, 640, 480);

    // Start the render loop
    DXUTMainLoop();

    // TODO: Perform any application-level cleanup here

    return DXUTGetExitCode();
}


