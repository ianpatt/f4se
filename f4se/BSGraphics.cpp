#include "f4se/BSGraphics.h"

// 
RelocPtr <ImageSpaceManager>		g_imageSpaceManager(0x03CA1BA0);

// 
RelocPtr <BSGraphics::Renderer>		g_renderManager(0x03768600);

// 
RelocPtr <BSRenderTargetManager>	g_renderTargetManager(0x02CDAEB0);

// 
RelocPtr <BSShaderResourceManager>	g_shaderResourceManager(0x03191288);

// 
RelocPtr <ID3D11Device>				g_D3D11Device(0x03623CA8);

// 
RelocPtr <ID3D11DeviceContext>		g_D3D11DeviceContext(0x03623C90);
