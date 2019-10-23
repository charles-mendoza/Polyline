#pragma once
#ifndef RENDERER_H
#define RENDERER_H

#include "Structures.h"

#include <d3d9.h>

#pragma comment(lib, "d3d9.lib")
#pragma comment(lib, "d3dx9.lib")

class Renderer
{
public:

	Renderer(IDirect3DDevice9* pDevice = NULL);
	~Renderer();

	void Begin();
	void End();
	void Release();

	void SetDrawColor(Color color);

	void Draw(Vector2* pVertexList, float fWidth, int iVertexCount);
	void DrawFilledCircle(Vector2 pCenter, float fRadius, int iSides);
	void DrawFilledRect(Rect rect);

	Rect GetClientBounds();

	void ResetDevice();

protected:

	void*				m_pCurrentTexture;
	IDirect3DDevice9*	m_pDevice;
	DWORD				m_Color;

	void Flush();
	void AddVert(int x, int y);
	void AddVert(int x, int y, float u, float v);

protected:

	struct Vertex
	{
		float x, y, z, rhw;
		DWORD color;
		float u, v;
	};

	static const int	MaxVerts = 1024;
	int					m_iVertNum;
	Vertex				m_pVerts[MaxVerts];
	Vertex				m_pLineVerts[MaxVerts];
};

#endif