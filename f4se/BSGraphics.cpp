#include "f4se/BSGraphics.h"


RelocAddr<BSShaderutil::_ClearRenderPasses> BSShaderutil::ClearRenderPasses(0x021BC6A0);

// 
RelocPtr <ImageSpaceManager>		g_imageSpaceManager(0x03E37C60);

// 
RelocPtr <BSGraphics::Renderer>		g_renderManager(0x039FF580);

// 
RelocPtr <BSGraphics::RenderTargetManager>	g_renderTargetManager(0x02F37700);

// 
RelocPtr <BSShaderResourceManager>	g_shaderResourceManager(0x03428228);

// 
RelocPtr <ID3D11Device>				g_D3D11Device(0x038BAC28);

// 
RelocPtr <ID3D11DeviceContext>		g_D3D11DeviceContext(0x038BAC10);
