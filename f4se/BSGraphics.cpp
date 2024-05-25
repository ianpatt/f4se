#include "f4se/BSGraphics.h"


RelocAddr<BSShaderutil::_ClearRenderPasses> BSShaderutil::ClearRenderPasses(0x0206BB00);

// 
RelocPtr <ImageSpaceManager>		g_imageSpaceManager(0x03CA2BA0);

// 
RelocPtr <BSGraphics::Renderer>		g_renderManager(0x03769600);

// 
RelocPtr <BSGraphics::RenderTargetManager>	g_renderTargetManager(0x02CDBED0);

// 
RelocPtr <BSShaderResourceManager>	g_shaderResourceManager(0x03192288);

// 
RelocPtr <ID3D11Device>				g_D3D11Device(0x03624CA8);

// 
RelocPtr <ID3D11DeviceContext>		g_D3D11DeviceContext(0x03624C90);
