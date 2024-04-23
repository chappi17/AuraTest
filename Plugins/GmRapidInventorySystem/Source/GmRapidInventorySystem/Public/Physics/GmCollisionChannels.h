// Copyright Dev.GaeMyo 2024. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Engine/EngineTypes.h"

/**
 * when you modify this, please note that this information can be saved with instances
 * also DefaultEngine.ini [/Script/Engine.CollisionProfile] should match with this list
 **/

// Trace against Actors/Components which provide interactions.
#define Gm_TraceChannel_Interaction					ECC_GameTraceChannel1

// Trace used by weapons, will hit physics assets instead of capsules
#define Gm_TraceChannel_Weapon						ECC_GameTraceChannel2
