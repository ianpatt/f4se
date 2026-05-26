#include "f4se/BSGraphics.h"


RelocAddr<BSShaderutil::_ClearRenderPasses> BSShaderutil::ClearRenderPasses(0x021C1270);

// 
RelocPtr <ImageSpaceManager>		g_imageSpaceManager(0x03E47B70);

// 
RelocPtr <BSGraphics::Renderer>		g_renderManager(0x03A0F480);

// 
RelocPtr <BSGraphics::RenderTargetManager>	g_renderTargetManager(0x02F42760);

// 
RelocPtr <BSShaderResourceManager>	g_shaderResourceManager(0x03438128);

// 
RelocPtr <ID3D11Device>				g_D3D11Device(0x038CAB28);

// 
RelocPtr <ID3D11DeviceContext>		g_D3D11DeviceContext(0x038CAB10);
