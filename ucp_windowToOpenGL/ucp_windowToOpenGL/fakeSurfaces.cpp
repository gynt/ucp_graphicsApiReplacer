
#include "pch.h"

#include <memory>

#include "windowCore.h"
#include "fakeSurfaces.h"

namespace UCPtoOpenGL
{

  /** fake helper **/

  void FakeSurface::FakeBlt(unsigned short* bltTo, int toX, int toY, int toWidth,
    unsigned short* bltFrom, int fromX, int fromY, int lenX, int lenY, int fromWidth)
  {
    // assumption:
    //  - the rectangles are ok
    //  - coords are between the pixels: 0->1600 means running from 0 to 1599

    // init
    bltFrom += fromY * fromWidth + fromX;
    bltTo += toY * toWidth + toX;
    for (size_t yRun{ 0 }; yRun < lenY; yRun++)
    {
      for (bltTo; bltTo < lenX + bltTo; bltTo++)
      {
        short fromColor{ *bltFrom };

        if (fromColor != NULL) // black is not blt
        {
          *bltTo = fromColor;
        }

        ++bltFrom;
      }

      bltFrom += fromWidth - lenX;
      bltTo += toWidth - lenX;
    }
  }


  /** fake primary **/

  STDMETHODIMP_(HRESULT __stdcall) FakePrimary::Flip(LPDIRECTDRAWSURFACE, DWORD)
  {
    return win->renderNextScreen(back->getBitmapPtr());
  }

  STDMETHODIMP_(HRESULT __stdcall) FakePrimary::GetAttachedSurface(LPDDSCAPS, LPDIRECTDRAWSURFACE* ptrToBackbuffer)
  {
    *ptrToBackbuffer = back;  // return fake backbuffer
    return DD_OK;
  }


  /** fake backbuffer **/

  STDMETHODIMP_(HRESULT __stdcall) FakeBackbuffer::Blt(LPRECT toRect, LPDIRECTDRAWSURFACE fromSurf, LPRECT fromRect,
    DWORD, LPDDBLTFX)
  {
    // should not be reached by anything but fake
    FakeSurface* otherSurf{ (FakeSurface*)fromSurf };
    FakeBlt(bitData.get(), toRect->left, toRect->top, width, otherSurf->getBitmapPtr(), fromRect->left,
      fromRect->top, toRect->right - toRect->left, toRect->bottom - toRect->top, otherSurf->getSurfaceWidth());
    return DD_OK;
  }

  STDMETHODIMP_(HRESULT __stdcall) FakeBackbuffer::BltFast(DWORD x, DWORD y, LPDIRECTDRAWSURFACE fromSurf,
    LPRECT fromRect, DWORD)
  {
    // should not be reached by anything but fake
    FakeSurface* otherSurf{ (FakeSurface*)fromSurf };
    FakeBlt(bitData.get(), x, y, width, otherSurf->getBitmapPtr(), fromRect->left,
      fromRect->top, fromRect->right - fromRect->left, fromRect->bottom - fromRect->top, otherSurf->getSurfaceWidth());
    return DD_OK;
  }

  STDMETHODIMP_(HRESULT __stdcall) FakeBackbuffer::GetSurfaceDesc(LPDDSURFACEDESC)
  {
    return E_NOTIMPL;
  }

}