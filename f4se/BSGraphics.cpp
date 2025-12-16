#include "f4se/BSGraphics.h"


RelocAddr<BSShaderutil::_ClearRenderPasses> BSShaderutil::ClearRenderPasses(0x021C1190);

// 
RelocPtr <ImageSpaceManager>		g_imageSpaceManager(0x03E47AF0);

// 
RelocPtr <BSGraphics::Renderer>		g_renderManager(0x03A0F400);

// 
RelocPtr <BSGraphics::RenderTargetManager>	g_renderTargetManager(0x02F42710);

// 
RelocPtr <BSShaderResourceManager>	g_shaderResourceManager(0x034380A8);

// 
RelocPtr <ID3D11Device>				g_D3D11Device(0x038CAAA8);

// 
RelocPtr <ID3D11DeviceContext>		g_D3D11DeviceContext(0x038CAA90);
