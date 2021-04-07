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
	/*루트 컴포넌트*/
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

	//시작블록 지정
	void SelectStartBlock(class AA_Star_AlgorithmBlock * StartBlock);

	//목표블록 지정
	void SelectTargetBlock(class AA_Star_AlgorithmBlock * TargetBlock);

	//벽 블록 토글
	void SetWallBlock(const FVector2D& targetPosition, const bool& IsWall=false);

	/*선택 되어 있는 경로,시작,타겟 블록들)*/
	void AllClearBlock();
private:
	/*초기화*/
	void SetRelease();

	TArray<FVector2D> GetPath_While(const FVector2D& startPosition,const FVector2D& targetPosition);

	/*목표지점 도착했는가?*/
	bool GetArriveTarget(const FVector2D& startPosition, const FVector2D& targetPosition) const;

	//배열 역순
	void ReverseArray();

	void  OpenListAdd(const int& currentX, const int& currentY);

	bool CheckException(const FVector2D& targetPosition);

	//경로 그리기
	void DrawPath(const FVector2D& p_DrawPosition,const int& p_CurBlockNumber);

	//블록 생성
	void SpawnBlocks();

private:
	//경로로 지정 되어 있는 블록들 배열
	TArray<AA_Star_AlgorithmBlock *> curPathBlocks;


	FVector2D mDirInfo[4];

	FVector2D mDirDiagnolInfo[4];

	/*갈수 있는 주변길을 담아 놓는곳*/
	TArray<Node_Info*> mOpenList;

	/*지나온 경로*/

	TArray<Node_Info*> mClosedList;

	Node_Info* mCurrentNode;

	/*최종 경로*/
	TArray<FVector2D> mFinalPathList;

	/*캐릭터 경로*/
	TArray<FVector2D> mCharacterPath;


	/*전체 블록 관리*/
	Node_Info mNodeArr[20][20];
private://타이머 관련

	void MovingTimer();

	int32 mCurPathCountNum;

	float mMovingTime;

	FTimerHandle mCountdownTimerHandle;
};



