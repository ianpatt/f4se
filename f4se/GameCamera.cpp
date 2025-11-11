#include "f4se/GameCamera.h"

// 
RelocPtr <PlayerCamera *> g_playerCamera(0x030CAAD8);

SInt32 PlayerCamera::GetCameraStateId(TESCameraState * state)
{
	for(int i = 0; i < kNumCameraStates; i++) {
		if(state == cameraStates[i])
			return i;
	}

	return -1;
}
