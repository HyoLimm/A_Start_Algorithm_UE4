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

	/*��Ʈ ������Ʈ*/
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

	/*���� �ִ� �ֺ����� ��� ���°�*/
	TArray<Node_Info*> OpenList;

	/*������ ���*/
	//TList<Node_Info>* ClosedList;
	TArray<Node_Info*> ClosedList;
	   
	Node_Info * CurrentNode;

	/*���� ���*/
	TArray<FVector2D> FinalPathList;
	
	/*ĳ���� ���*/
	TArray<FVector2D> CharacterPath;


	/*��ü ��� ����*/
	Node_Info NodeArray[20][20];

	UPROPERTY(VisibleAnywhere, Category = "Block")
	class AA_Star_AlgorithmBlock * Cur_StartBlock;


	UPROPERTY(VisibleAnywhere, Category = "Block")
	class AA_Star_AlgorithmBlock * Cur_TargetBlock;



	class AA_Star_AlgorithmPawn * PawnOwner;

	//���ۺ�� ����
	void SelectStartBlock(class AA_Star_AlgorithmBlock * StartBlock);

	//��ǥ��� ����
	void SelectTargetBlock(class AA_Star_AlgorithmBlock * TargetBlock);

	//�� ��� ���
	void SetWalllBlock(FVector2D p_Position, bool p_IsWall = false);

	/*���� �Ǿ� �ִ� ���,����,Ÿ�� ��ϵ�)*/
	void AllClearBlock();


private:
	//��ã�� ���� - ���
	TArray<FVector2D> GetPath_Recursive(FVector2D Start, FVector2D Target);
	/*��͸� �̿��� ������ ��ã��*/
	Node_Info * Recursive_FindPath(Node_Info* parent, FVector2D target);

	//���� ����Ʈ �߰�
	void AddOpenList(Node_Info* _node);

	//��ã�� ���� 
	TArray<FVector2D> GetPath_While(FVector2D Start, FVector2D Target);

	void  OpenListAdd(int x,int y);

	//��η� ���� �Ǿ� �ִ� ��ϵ� �迭
	class TArray<AA_Star_AlgorithmBlock *> Current_PathBlocks;

	//���ܹ߻� üũ (�׸��� �ݰ� ����ų�, �� ����, CloseList�� �ִ��� ���� ��)
	bool CheckException(FVector2D _Position);

	/*��ǥ���� �����ߴ°�?*/
	bool GetArriveTarget(FVector2D Start, FVector2D Target);

	/*�ʱ�ȭ*/
	void SetRelease();

	//�迭 ����
	void ReverseArray();

	//��� �׸���
	void DrawPath(FVector2D p_DrawPosition, int p_CurBlockNumber);
		
	//��� ����
	void SpawnBlocks();

private://Ÿ�̸� ����
	void MovingTimer();

	int32 Cur_PathCountNum;

	FTimerHandle CountdownTimerHandle;

	float MovingTime;
};



