// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#include "A_Star_AlgorithmPawn.h"
#include "A_Star_AlgorithmBlock.h"
#include "A_Star_AlgorithmBlockGrid.h"
#include "HeadMountedDisplayFunctionLibrary.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/PlayerController.h"
#include "Engine/World.h"
#include "DrawDebugHelpers.h"

AA_Star_AlgorithmPawn::AA_Star_AlgorithmPawn(const FObjectInitializer& ObjectInitializer) 
	: Super(ObjectInitializer)
{
	AutoPossessPlayer = EAutoReceiveInput::Player0;
}

void AA_Star_AlgorithmPawn::BeginPlay(){
	Super::BeginPlay();

	//GridSpawn
	OwnerGrid=GetWorld()->SpawnActor<AA_Star_AlgorithmBlockGrid>(FVector(0.f,0.f,0.0f), FRotator(0, 0, 0));
}

void AA_Star_AlgorithmPawn::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	//Calc Mouse
	if (APlayerController* PC = Cast<APlayerController>(GetController())){	
		FVector Start, Dir, End;
		PC->DeprojectMousePositionToWorld(Start, Dir);
		End = Start + (Dir * 8000.0f);
		TraceForBlock(Start, End, false);
	}
}

void AA_Star_AlgorithmPawn::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent){
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	//PlayerInputComponent->BindAction("ResetVR", EInputEvent::IE_Pressed, this, &AA_Star_AlgorithmPawn::OnResetVR);
	PlayerInputComponent->BindAction("TriggerClick", EInputEvent::IE_Pressed, this, &AA_Star_AlgorithmPawn::TriggerClick);
	PlayerInputComponent->BindAction("WallTriggerClick", EInputEvent::IE_Pressed, this, &AA_Star_AlgorithmPawn::TriggerWall_Click);
	PlayerInputComponent->BindAction("ClearBlockButton", EInputEvent::IE_Pressed, this, &AA_Star_AlgorithmPawn::ClickAllClearButton);
	 
}

void AA_Star_AlgorithmPawn::CalcCamera(float DeltaTime, struct FMinimalViewInfo& OutResult){
	Super::CalcCamera(DeltaTime, OutResult);

	OutResult.Rotation = FRotator(-90.0f, -90.0f, 0.0f);
}


void AA_Star_AlgorithmPawn::TriggerClick(){
	//Block Click
	if (CurrentBlockFocus&&!CurrentBlockFocus->GetIsWall())
		CurrentBlockFocus->HandleClicked();
}

void AA_Star_AlgorithmPawn::TraceForBlock(const FVector& Start, const FVector& End, bool bDrawDebugHelpers){
	FHitResult HitResult;
	GetWorld()->LineTraceSingleByChannel(HitResult, Start, End, ECC_Visibility);

	if (HitResult.Actor.IsValid()){
		AA_Star_AlgorithmBlock* HitBlock = Cast<AA_Star_AlgorithmBlock>(HitResult.Actor.Get());
		if (CurrentBlockFocus != HitBlock){
			if (CurrentBlockFocus)			
				CurrentBlockFocus->Highlight(false);
			
			if (HitBlock)	
				HitBlock->Highlight(true);

			CurrentBlockFocus = HitBlock;
		}		
	}
	else if (CurrentBlockFocus)	{
		CurrentBlockFocus->Highlight(false);
		CurrentBlockFocus = nullptr;
	}
}


void AA_Star_AlgorithmPawn::TriggerWall_Click(){
	if (CurrentBlockFocus)	{
		if(CurrentBlockFocus->GetIsWall())
			CurrentBlockFocus->SetWallBlock(false);
		else
			CurrentBlockFocus->SetWallBlock(true);
	}
}


void AA_Star_AlgorithmPawn::ClickAllClearButton(){
	if(OwnerGrid==nullptr) return;

	if(OwnerGrid->GetCurStartBlock() !=nullptr|| OwnerGrid->GetCurTargetBlock() != nullptr)
		OwnerGrid->AllClearBlock();
}

