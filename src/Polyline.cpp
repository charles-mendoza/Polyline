#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

#include <d3d9.h>
#include <vector>

#include "Input.h"
#include "Renderer.h"

HWND					g_hWnd = NULL;
IDirect3D9*				g_pD3D = NULL;
IDirect3DDevice9*		g_pD3DDevice = NULL;
D3DPRESENT_PARAMETERS	g_D3DParams;

HWND CreateGameWindow();
void CreateD3DDevice();
bool ProcessMessage(MSG msg);

int main()
{
	MSG msg;
	std::vector<Vector2> points;
	bool bLeftMouseDown = false;

	g_pD3D = Direct3DCreate9(D3D_SDK_VERSION);
	g_hWnd = CreateGameWindow();
	CreateD3DDevice();

	Renderer* pRenderer = new Renderer(g_pD3DDevice);

	while (true)
	{
		if (!IsWindowVisible(g_hWnd)) { break; }

		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			ProcessMessage(msg);

			if (msg.message == WM_QUIT) { break; }

			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		else
		{
			g_pD3DDevice->BeginScene();
			g_pD3DDevice->Clear(0, NULL, D3DCLEAR_TARGET, D3DCOLOR_XRGB(0, 0, 0), 1, 0);
			pRenderer->Begin();

			// clear out the window with white
			pRenderer->SetDrawColor(Color(255, 255, 255, 255));
			pRenderer->DrawFilledRect(pRenderer->GetClientBounds());

			if (IsLeftMouseDown())
			{
				// start dragging
				points.push_back(GetMousePosition());
				pRenderer->SetDrawColor(Color(127, 127, 127, 127));
				pRenderer->Draw(points.data(), 50, points.size());
				points.pop_back();
				pRenderer->SetDrawColor(Color(255, 255, 0, 0));
				pRenderer->DrawFilledCircle(GetMousePosition(), 5, 20);
			}
			else
			{
				// draw all points as line
				pRenderer->SetDrawColor(Color(127, 127, 127, 127));
				pRenderer->Draw(points.data(), 50, points.size());
			}

			// not dragging, create a new point
			if (IsLeftMouseClicked())
				points.push_back(GetMousePosition());

			// draw all points as red circle
			for (Vector2 p : points)
			{
				pRenderer->SetDrawColor(Color(255, 255, 0, 0));
				pRenderer->DrawFilledCircle(p, 5, 20);
			}

			pRenderer->End();
			g_pD3DDevice->EndScene();
			g_pD3DDevice->Present(NULL, NULL, NULL, NULL);
		}
	}

	delete pRenderer;

	if (g_pD3DDevice)
	{
		g_pD3DDevice->Release();
		g_pD3DDevice = NULL;
	}

	if (g_pD3D)
	{
		g_pD3D->Release();
		g_pD3D = NULL;
	}

	return 0;
}

HWND CreateGameWindow()
{
	WNDCLASS wc;
	ZeroMemory(&wc, sizeof(wc));
	wc.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
	wc.lpfnWndProc = DefWindowProc;
	wc.hInstance = GetModuleHandle(NULL);
	wc.lpszClassName = L"PolylineWindow";
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	RegisterClass(&wc);
	HWND hWnd = CreateWindowEx((WS_EX_APPWINDOW | WS_EX_WINDOWEDGE), wc.lpszClassName, L"Polyline", (WS_OVERLAPPEDWINDOW | WS_CLIPSIBLINGS | WS_CLIPCHILDREN) & ~(WS_MINIMIZEBOX | WS_MAXIMIZEBOX | WS_THICKFRAME), -1, -1, 800, 600, NULL, NULL, wc.hInstance, NULL);
	ShowWindow(hWnd, SW_SHOW);
	SetForegroundWindow(hWnd);
	SetFocus(hWnd);
	return hWnd;
}

void CreateD3DDevice()
{
	ZeroMemory(&g_D3DParams, sizeof(g_D3DParams));
	RECT ClientRect;
	GetClientRect(g_hWnd, &ClientRect);
	g_D3DParams.Windowed = TRUE;
	g_D3DParams.SwapEffect = D3DSWAPEFFECT_DISCARD;
	g_D3DParams.BackBufferWidth = ClientRect.right;
	g_D3DParams.BackBufferHeight = ClientRect.bottom;
	g_D3DParams.FullScreen_RefreshRateInHz = D3DPRESENT_RATE_DEFAULT;
	g_D3DParams.BackBufferFormat = D3DFMT_X8R8G8B8;
	g_D3DParams.PresentationInterval = D3DPRESENT_INTERVAL_IMMEDIATE;
	HRESULT hr = g_pD3D->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, g_hWnd, D3DCREATE_SOFTWARE_VERTEXPROCESSING, &g_D3DParams, &g_pD3DDevice);

	if (FAILED(hr))
	{
		OutputDebugString(L"Couldn't create D3D Device for some reason!");
		return;
	}
}

bool ProcessMessage(MSG msg)
{
	switch (msg.message)
	{
	case WM_MOUSEMOVE:
	{
		int x = (signed short)LOWORD(msg.lParam);
		int y = (signed short)HIWORD(msg.lParam);
		return OnMouseMoved(x, y);
	}
	case WM_LBUTTONDOWN:
	{
		bool ret = OnMouseClicked(0, true);
		if (ret) { SetCapture(msg.hwnd); }
		return ret;
	}
	case WM_LBUTTONUP:
	{
		bool ret = OnMouseClicked(0, false);
		if (ret) { ReleaseCapture(); }
		return ret;
	}
	default:
		break;
	}

	return false;
}