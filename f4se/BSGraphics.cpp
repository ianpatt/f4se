#include "f4se/BSGraphics.h"


RelocAddr<BSShaderutil::_ClearRenderPasses> BSShaderutil::ClearRenderPasses(0x021C8CA0);

// 
RelocPtr <ImageSpaceManager>		g_imageSpaceManager(0x03E5E4B0);

// 
RelocPtr <BSGraphics::Renderer>		g_renderManager(0x03A25D00);

// 
RelocPtr <BSGraphics::RenderTargetManager>	g_renderTargetManager(0x02F4D750);

// 
RelocPtr <BSShaderResourceManager>	g_shaderResourceManager(0x034485A8);

// 
RelocPtr <ID3D11Device>				g_D3D11Device(0x038E13A8);

// 
RelocPtr <ID3D11DeviceContext>		g_D3D11DeviceContext(0x038E1390);
