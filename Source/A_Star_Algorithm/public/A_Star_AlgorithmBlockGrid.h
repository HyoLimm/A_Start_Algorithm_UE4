// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Containers/List.h"
#include "Node_Information.h"
#include "A_Star_AlgorithmBlockGrid.generated.h"


/** Class used to spawn blocks and manage score */
UCLASS(minimalapi)
class AA_Star_AlgorithmBlockGrid : public AActor
{
	GENERATED_BODY()

	/*루트 컴포넌트*/
	UPROPERTY(Category = Grid, VisibleDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	class USceneComponent* RootComp;

public:
	AA_Star_AlgorithmBlockGrid();

	/** Number of blocks along each side of grid */
	UPROPERTY(Category=Grid, EditAnywhere, BlueprintReadOnly)
	int32 GridSize;

	/** Spacing of blocks */
	UPROPERTY(Category=Grid, EditAnywhere, BlueprintReadOnly)
	float BlockSpacing;

	UPROPERTY(Category = Grid, EditAnywhere, BlueprintReadWrite)
	bool bAllowDiagonal;

	UPROPERTY(Category = Grid, EditAnywhere, BlueprintReadWrite)
	bool bdontCrossCorner;
protected:
	// Begin AActor interface
	virtual void BeginPlay() override;
	// End AActor interface
	   	 
public:
	FVector2D DirectionInfo[4];

	/*갈수 있는 주변길을 담아 놓는곳*/
	TArray<Node_Info*> OpenList;

	/*지나온 경로*/
	//TList<Node_Info>* ClosedList;
	TArray<Node_Info*> ClosedList;
	   
	Node_Info * CurrentNode;

	/*최종 경로*/
	TArray<FVector2D> FinalPathList;
	
	/*캐릭터 경로*/
	TArray<FVector2D> CharacterPath;


	/*전체 블록 관리*/
	Node_Info NodeArray[20][20];

	UPROPERTY(VisibleAnywhere, Category = "Block")
	class AA_Star_AlgorithmBlock * Cur_StartBlock;


	UPROPERTY(VisibleAnywhere, Category = "Block")
	class AA_Star_AlgorithmBlock * Cur_TargetBlock;



	class AA_Star_AlgorithmPawn * PawnOwner;

	//시작블록 지정
	void SelectStartBlock(class AA_Star_AlgorithmBlock * StartBlock);

	//목표블록 지정
	void SelectTargetBlock(class AA_Star_AlgorithmBlock * TargetBlock);

	//벽 블록 토글
	void SetWalllBlock(FVector2D p_Position, bool p_IsWall = false);

	/*선택 되어 있는 경로,시작,타겟 블록들)*/
	void AllClearBlock();


private:
	//길찾기 시작 - 재귀
	TArray<FVector2D> GetPath_Recursive(FVector2D Start, FVector2D Target);
	/*재귀를 이용한 최적의 길찾기*/
	Node_Info * Recursive_FindPath(Node_Info* parent, FVector2D target);

	//오픈 리스트 추가
	void AddOpenList(Node_Info* _node);

	//길찾기 시작 
	TArray<FVector2D> GetPath_While(FVector2D Start, FVector2D Target);

	void  OpenListAdd(int x,int y);

	//경로로 지정 되어 있는 블록들 배열
	class TArray<AA_Star_AlgorithmBlock *> Current_PathBlocks;

	//예외발생 체크 (그리드 반격 벗어나거나, 벽 여부, CloseList에 있는지 여부 등)
	bool CheckException(FVector2D _Position);

	/*목표지점 도착했는가?*/
	bool GetArriveTarget(FVector2D Start, FVector2D Target);

	/*초기화*/
	void SetRelease();

	//배열 역순
	void ReverseArray();

	//경로 그리기
	void DrawPath(FVector2D p_DrawPosition, int p_CurBlockNumber);
		
	//블록 생성
	void SpawnBlocks();

private://타이머 관련
	void MovingTimer();

	int32 Cur_PathCountNum;

	FTimerHandle CountdownTimerHandle;

	float MovingTime;
};



