#include "Renderer.h"

#include <d3dx9core.h>

#define MITER_LIMIT 0.75f

#define D3DFVF_VERTEXFORMAT2D ( D3DFVF_XYZRHW | D3DFVF_DIFFUSE | D3DFVF_TEX1 )

Renderer::Renderer(IDirect3DDevice9* pDevice)
{
	m_pDevice = pDevice;
	m_iVertNum = 0;

	for (int i = 0; i < MaxVerts; i++)
	{
		m_pVerts[i].z = 0.5f;
		m_pVerts[i].rhw = 1.0f;
	}
}

Renderer::~Renderer()
{
}

void Renderer::Begin()
{
	m_pDevice->GetTexture(0, (IDirect3DBaseTexture9**)&m_pCurrentTexture);

	m_pDevice->SetPixelShader(NULL);
	m_pDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
	m_pDevice->SetRenderState(D3DRS_ZENABLE, FALSE);
	m_pDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
	m_pDevice->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
	m_pDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
	m_pDevice->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
	m_pDevice->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
	m_pDevice->SetSamplerState(0, D3DSAMP_MIPFILTER, D3DTEXF_NONE);
	m_pDevice->SetSamplerState(0, D3DSAMP_ADDRESSU, D3DTADDRESS_CLAMP);
	m_pDevice->SetSamplerState(0, D3DSAMP_ADDRESSV, D3DTADDRESS_CLAMP);
	m_pDevice->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_MODULATE);
	m_pDevice->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
	m_pDevice->SetTextureStageState(0, D3DTSS_COLORARG2, D3DTA_CURRENT);
	m_pDevice->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_MODULATE);
	m_pDevice->SetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);
	m_pDevice->SetTextureStageState(0, D3DTSS_ALPHAARG2, D3DTA_CURRENT);
	m_pDevice->SetTextureStageState(1, D3DTSS_ALPHAOP, D3DTOP_DISABLE);
	m_pDevice->SetTextureStageState(1, D3DTSS_COLOROP, D3DTOP_DISABLE);
	m_pDevice->SetFVF(D3DFVF_VERTEXFORMAT2D);
}

void Renderer::End()
{
	Flush();
	m_pDevice->SetTexture(0, NULL);
	m_pCurrentTexture = NULL;
}

void Renderer::Flush()
{
	if (m_iVertNum > 0)
	{
		m_pDevice->SetFVF(D3DFVF_VERTEXFORMAT2D);
		m_pDevice->DrawPrimitiveUP(D3DPT_TRIANGLELIST, m_iVertNum / 3, &m_pVerts[0], sizeof(Vertex));
		m_iVertNum = 0;
	}
}

void Renderer::AddVert(int x, int y)
{
	if (m_iVertNum >= MaxVerts - 1)
	{
		Flush();
	}

	m_pVerts[m_iVertNum].x = (float)x;
	m_pVerts[m_iVertNum].y = (float)y;
	m_pVerts[m_iVertNum].color = m_Color;
	m_iVertNum++;
}

void Renderer::AddVert(int x, int y, float u, float v)
{
	if (m_iVertNum >= MaxVerts - 1)
	{
		Flush();
	}

	m_pVerts[m_iVertNum].x = -0.5f + (float)x;
	m_pVerts[m_iVertNum].y = -0.5f + (float)y;
	m_pVerts[m_iVertNum].u = u;
	m_pVerts[m_iVertNum].v = v;
	m_pVerts[m_iVertNum].color = m_Color;
	m_iVertNum++;
}

Rect Renderer::GetClientBounds()
{
	D3DVIEWPORT9 viewport;
	m_pDevice->GetViewport(&viewport);
	return Rect(viewport.X, viewport.Y, viewport.Width, viewport.Height);
}

void Renderer::Draw(Vector2* pVertexList, float fWidth, int iVertexCount)
{
	if (fWidth < 1.0f) { return; }
	if (iVertexCount < 2) { return; }

	Flush();

	if (m_pCurrentTexture != NULL)
	{
		m_pDevice->SetTexture(0, NULL);
		m_pCurrentTexture = NULL;
	}

	int iCurVert = 0;

	// simply draw basic line if width is 1 pixel
	if (fWidth == 1.0f)
	{
		for (; iCurVert < iVertexCount; iCurVert++)
			m_pLineVerts[iCurVert] = { pVertexList[iCurVert].x, pVertexList[iCurVert].y, 1.0f, 1.0f, m_Color };

		m_pDevice->DrawPrimitiveUP(D3DPT_LINESTRIP, iCurVert - 1, &m_pLineVerts[0], sizeof(Vertex));

		return;
	}

	float length;
	Vector2 a, b;
	Vector2 p0, p1, p2;
	Vector2 v0, v1, n0, n1, miter;

	p0 = pVertexList[0];
	fWidth *= 0.5f;

	for (int i = 0; i < iVertexCount; i++)
	{
		p1 = pVertexList[i];
		p2 = pVertexList[i != iVertexCount - 1 ? i + 1 : i];

		// define the line between the two points
		v0 = (p1 - p0).Normalized();
		v1 = (p2 - p1).Normalized();

		// find the normal vector of these lines
		n0 = Vector2(-v0.y, v0.x).Normalized();
		n1 = Vector2(-v1.y, v1.x).Normalized();

		// find the miter line
		miter = (n0 + n1).Normalized();

		// find length of miter by projecting the miter onto the normal,
		// take the length of the projection, invert it and multiply it by the thickness:
		//		length = thickness * ( 1 / |normal|.|miter| )
		length = fWidth / n1.Dot(miter);

		if (v0.Dot(v1) < -MITER_LIMIT)
		{
			// create sharp corner
			a = p1 - n0 * fWidth;
			b = p1 + n0 * fWidth;
			m_pLineVerts[iCurVert++] = { a.x, a.y, 1.0f, 1.0f, m_Color };
			m_pLineVerts[iCurVert++] = { b.x, b.y, 1.0f, 1.0f, m_Color };
			a = p1 - n1 * fWidth;
			b = p1 + n1 * fWidth;
		}
		else
		{
			// cut off sharp corner
			a = p1 - miter * length;
			b = p1 + miter * length;
		}

		m_pLineVerts[iCurVert++] = { a.x, a.y, 1.0f, 1.0f, m_Color };
		m_pLineVerts[iCurVert++] = { b.x, b.y, 1.0f, 1.0f, m_Color };

		// close off line
		if (i == iVertexCount - 2)
		{
			a = p2 - n1 * fWidth;
			b = p2 + n1 * fWidth;
			m_pLineVerts[iCurVert++] = { a.x, a.y, 1.0f, 1.0f, m_Color };
			m_pLineVerts[iCurVert++] = { b.x, b.y, 1.0f, 1.0f, m_Color };
			break;
		}

		p0 = p1;
	}

	m_pDevice->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP, iCurVert - 2, &m_pLineVerts[0], sizeof(Vertex));

	// draw line mesh
	for (int i = 0; i < iCurVert; i++)
		m_pLineVerts[i].color = D3DCOLOR_XRGB(127, 127, 127);
	m_pDevice->DrawPrimitiveUP(D3DPT_LINESTRIP, iCurVert - 2, &m_pLineVerts[0], sizeof(Vertex));
}

void Renderer::DrawFilledCircle(Vector2 pCenter, float fRadius, int iSides)
{
	// clamp iSides with min of 2 and max of 360 sides
	if (iSides >= 360) { iSides = 360; }
	if (iSides <= 2) { iSides = 2; }

	float angle = D3DX_PI * 2 / iSides;
	float _cos = cos(angle);
	float _sin = sin(angle);
	float x1 = fRadius, y1 = 0, x2, y2;

	Vertex circle[360];

	for (int i = 0; i < iSides+1; i++)
	{
		x2 = _cos * x1 - _sin * y1 + pCenter.x;
		y2 = _sin * x1 + _cos * y1 + pCenter.y;
		x1 += pCenter.x;
		y1 += pCenter.y;
		circle[i] = { x1, y1, 1.0f, 1.0f, m_Color };
		x1 = x2 - pCenter.x;
		y1 = y2 - pCenter.y;
	}
	
	m_pDevice->DrawPrimitiveUP(D3DPT_TRIANGLEFAN, iSides - 1, &circle, sizeof(Vertex));
}

void Renderer::DrawFilledRect(Rect rect)
{
	if (m_pCurrentTexture != NULL)
	{
		Flush();
		m_pDevice->SetTexture(0, NULL);
		m_pCurrentTexture = NULL;
	}

	AddVert(rect.x, rect.y);
	AddVert(rect.x + rect.w, rect.y);
	AddVert(rect.x, rect.y + rect.h);
	AddVert(rect.x + rect.w, rect.y);
	AddVert(rect.x + rect.w, rect.y + rect.h);
	AddVert(rect.x, rect.y + rect.h);
}

void Renderer::SetDrawColor(Color color)
{
	m_Color = D3DCOLOR_ARGB(color.a, color.r, color.g, color.b);
}

void Renderer::ResetDevice()
{
}

void Renderer::Release()
{
}