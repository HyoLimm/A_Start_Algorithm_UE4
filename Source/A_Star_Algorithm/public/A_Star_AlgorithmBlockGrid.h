// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.
#pragma once
#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Containers/List.h"
#include "Node_Information.h"
#include "A_Star_AlgorithmBlockGrid.generated.h"

class AA_Star_AlgorithmBlock;
class AA_Star_AlgorithmPawn;
class USceneComponent;

UCLASS(minimalapi)
class AA_Star_AlgorithmBlockGrid : public AActor
{
	GENERATED_BODY()
protected:
	virtual void BeginPlay() override;
public:
	UPROPERTY(VisibleAnywhere, Category = "Block")
	AA_Star_AlgorithmBlock* curStartBlock;

	UPROPERTY(VisibleAnywhere, Category = "Block")
	AA_Star_AlgorithmBlock* curTargetBlock;

	AA_Star_AlgorithmPawn* PawnOwner;
public:
	UPROPERTY(Category = Grid, VisibleDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	USceneComponent* dummyRoot;

	AA_Star_AlgorithmBlockGrid();

	/** Number of blocks along each side of grid */
	UPROPERTY(Category = Grid, EditAnywhere, BlueprintReadOnly)
	int32 GridSize;

	/** Spacing of blocks */
	UPROPERTY(Category = Grid, EditAnywhere, BlueprintReadOnly)
	float BlockSpacing;

	UPROPERTY(Category = Grid, EditAnywhere, BlueprintReadWrite)
	bool bAllowDiagonal;

	UPROPERTY(Category = Grid, EditAnywhere, BlueprintReadWrite)
	bool bDontCrossCorner;

public:
	void SelectStartBlock(AA_Star_AlgorithmBlock * StartBlock);

	void SelectTargetBlock(AA_Star_AlgorithmBlock * TargetBlock);

	void SetWallBlock(const FVector2D& targetPosition, const bool& IsWall=false);


	void AllClearBlock();
private:	
	void SetRelease();

	TArray<FVector2D>& GetPath_While(const FVector2D& startPosition,const FVector2D& targetPosition);

	bool GetArriveTarget(const FVector2D& startPosition, const FVector2D& targetPosition) const;

	void ReverseArray();

	void  OpenListAdd(const int& currentX, const int& currentY);

	bool CheckException(const FVector2D& targetPosition);

	void DrawPath(const FVector2D& p_DrawPosition,const int& p_CurBlockNumber);

	void SpawnBlocks();
private:
	UPROPERTY()
	TArray<AA_Star_AlgorithmBlock *> curPathBlocks;

	FVector2D mDirInfo[4];

	FVector2D mDirDiagnolInfo[4];

	TArray<Node_Info*> mOpenList;

	TArray<Node_Info*> mClosedList;

	Node_Info* mCurrentNode;

	TArray<FVector2D> mFinalPathList;

	TArray<FVector2D> mCharacterPath;

	Node_Info mNodeArr[20][20];
private:
	void MovingTimer();

	int32 mCurPathCountNum;

	float mMovingTime;

	FTimerHandle mCountdownTimerHandle;
};



