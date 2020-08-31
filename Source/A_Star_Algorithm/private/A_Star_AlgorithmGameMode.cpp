// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#include "A_Star_AlgorithmGameMode.h"
#include "A_Star_AlgorithmPlayerController.h"
#include "A_Star_AlgorithmPawn.h"

AA_Star_AlgorithmGameMode::AA_Star_AlgorithmGameMode()
{
	// no pawn by default
	DefaultPawnClass = AA_Star_AlgorithmPawn::StaticClass();
	// use our own player controller class
	PlayerControllerClass = AA_Star_AlgorithmPlayerController::StaticClass();
}
