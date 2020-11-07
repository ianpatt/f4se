#include "f4se/GameCamera.h"

// 05C2864F8B39388E329CE28479B6A5636B4D529C+11
RelocPtr <PlayerCamera *> g_playerCamera(0x058CEB28);

SInt32 PlayerCamera::GetCameraStateId(TESCameraState * state)
{
	for(int i = 0; i < kNumCameraStates; i++) {
		if(state == cameraStates[i])
			return i;
	}

	return -1;
}
