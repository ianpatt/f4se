#include "f4se/BSGraphics.h"


RelocAddr<BSShaderutil::_ClearRenderPasses> BSShaderutil::ClearRenderPasses(0x021BB340);

// 
RelocPtr <ImageSpaceManager>		g_imageSpaceManager(0x03E31A60);

// 
RelocPtr <BSGraphics::Renderer>		g_renderManager(0x039F9380);

// 
RelocPtr <BSGraphics::RenderTargetManager>	g_renderTargetManager(0x02F31700);

// 
RelocPtr <BSShaderResourceManager>	g_shaderResourceManager(0x03422028);

// 
RelocPtr <ID3D11Device>				g_D3D11Device(0x038B4A28);

// 
RelocPtr <ID3D11DeviceContext>		g_D3D11DeviceContext(0x038B4A10);
