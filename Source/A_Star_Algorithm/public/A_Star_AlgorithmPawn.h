// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "A_Star_AlgorithmPawn.generated.h"

UCLASS(config=Game)
class AA_Star_AlgorithmPawn : public APawn
{
	GENERATED_UCLASS_BODY()

public:

	virtual void BeginPlay() override;

	virtual void Tick(float DeltaSeconds) override;

	virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;

	virtual void CalcCamera(float DeltaTime, struct FMinimalViewInfo& OutResult) override;

protected:

	void TriggerClick();
	void TraceForBlock(const FVector& Start, const FVector& End, bool bDrawDebugHelpers);

	void TriggerWall_Click();

	UFUNCTION(BlueprintCallable)
	void ClickAllClearButton();



	UPROPERTY(EditInstanceOnly, BlueprintReadWrite)
	class AA_Star_AlgorithmBlock* CurrentBlockFocus;

	UPROPERTY(EditInstanceOnly, BlueprintReadWrite)
	class AA_Star_AlgorithmBlockGrid* OwnerGrid;

	UPROPERTY(EditInstanceOnly, BlueprintReadWrite)
	class AA_Star_AlgorithmBlock* PressedBlockFocus;
};
