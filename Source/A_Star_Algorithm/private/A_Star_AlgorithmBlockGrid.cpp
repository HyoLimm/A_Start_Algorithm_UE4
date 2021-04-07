// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#include "A_Star_AlgorithmBlockGrid.h"
#include "A_Star_AlgorithmBlock.h"
#include "Components/TextRenderComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/World.h"

#define LOCTEXT_NAMESPACE "PuzzleBlockGrid"

AA_Star_AlgorithmBlockGrid::AA_Star_AlgorithmBlockGrid()
	: curStartBlock(nullptr)
	, curTargetBlock(nullptr)
	, PawnOwner(nullptr)
	, GridSize(20)
	, BlockSpacing(300.f)
	, bAllowDiagonal(false)
	, bDontCrossCorner(false)
	, mCurPathCountNum(0)
	, mMovingTime(0.5f)
{
	dummyRoot = CreateDefaultSubobject<USceneComponent>(TEXT("Dummy0"));
	RootComponent = dummyRoot;

	mDirInfo[0] = FVector2D(1, 0);	//up
	mDirInfo[1] = FVector2D(0, 1); //right
	mDirInfo[2] = FVector2D(-1, 0); //down 
	mDirInfo[3] = FVector2D(0, -1); //left

	mDirDiagnolInfo[0] = FVector2D(1, 1);
	mDirDiagnolInfo[1] = FVector2D(-1, 1);
	mDirDiagnolInfo[2] = FVector2D(-1, -1);
	mDirDiagnolInfo[3] = FVector2D(1, -1);


}//end of AA_Star_AlgorithmBlockGrid();

void AA_Star_AlgorithmBlockGrid::BeginPlay()
{
	Super::BeginPlay();
	SpawnBlocks();

}//end of BeginPlay()

void AA_Star_AlgorithmBlockGrid::SelectStartBlock(AA_Star_AlgorithmBlock * p_StartingBlock)
{	//Check StartBlock
	if (curStartBlock != nullptr)
	{
		return;
	}
	curStartBlock = p_StartingBlock;
}//End of SelectStartBlock


void AA_Star_AlgorithmBlockGrid::SelectTargetBlock(AA_Star_AlgorithmBlock * p_TargetingBlock)
{
	if (curTargetBlock != nullptr) //Check Already TargetBlock
	{
		return;
	}

	curTargetBlock = p_TargetingBlock;

	if (curStartBlock == nullptr || curTargetBlock == nullptr)
	{
		return;
	}

	mCharacterPath = GetPath_While(curStartBlock->GetBlockNumber(), curTargetBlock->GetBlockNumber());

	//Not Found Path
	if (mCharacterPath.Num() <= 0)
	{
		UE_LOG(LogTemp, Warning, TEXT("Not Found Path"));
		return;
	}

	mCurPathCountNum = 1;
	GetWorldTimerManager().SetTimer(mCountdownTimerHandle, this, &AA_Star_AlgorithmBlockGrid::MovingTimer, mMovingTime, true);

}//end of SelectTargetBlock

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
TArray<FVector2D> AA_Star_AlgorithmBlockGrid::GetPath_While(const FVector2D& startPosition, const FVector2D& targetPosition)
{
	this->SetRelease();
	//StartPosition Push

	Node_Info* startNode = new Node_Info(startPosition, nullptr, 0, targetPosition);
	mOpenList.Push(startNode);

	while (mOpenList.Num() > 0)
	{
		mCurrentNode = mOpenList[0];

		int openSize = mOpenList.Num();
		for (int i = 0; i < openSize; i++)
		{
			int curListF = mOpenList[i]->GetCostF(); //F == TotalValue
			int curListH = mOpenList[i]->GetCostH(); //H == RemainValue

			//Find More Little Value
			if ((curListF <= mCurrentNode->GetCostF()) && (curListH < mCurrentNode->GetCostH()))
			{
				mCurrentNode = mOpenList[i];
				mCurrentNode->SetCurBlock(mOpenList[i]->GetCurBlock());
			}
		}
		mOpenList.Remove(mCurrentNode); //Delete OpenList
		mClosedList.Push(mCurrentNode); //Add ClosedList

		//mCurrentNode->GetCurBlock()->SetSearchBlock();

		const int curX = mCurrentNode->GetPoistion().X;
		const int curY = mCurrentNode->GetPoistion().Y;

		if (GetArriveTarget(mCurrentNode->GetPoistion(), targetPosition))
		{
			Node_Info* targetNode = &mNodeArr[curX][curY];

			while (targetNode->GetPoistion() != startPosition)
			{
				mFinalPathList.Push(targetNode->GetPoistion());
				targetNode = targetNode->GetParent();
			}

			mFinalPathList.Push(startPosition);

			ReverseArray();

			delete targetNode;

			return mFinalPathList;
		}


		//Diagonal
		if (bAllowDiagonal == true)
		{
			for (int i = 0; i < 4; i++) 
			{
				const int nextX = curX + mDirDiagnolInfo[i].X;
				const int nextY = curY + mDirDiagnolInfo[i].Y;
				OpenListAdd(nextX, nextY);
			}
		}

		//Up Down Left Right
		for (int i = 0; i < 4; i++)
		{
			const int nextX = curX + mDirInfo[i].X;
			const int nextY = curY + mDirInfo[i].Y;
			OpenListAdd(nextX, nextY);
		}


	}//end of while

	return mFinalPathList;
}//end of GetPath_While


bool AA_Star_AlgorithmBlockGrid::GetArriveTarget(const FVector2D& startPosition, const FVector2D& targetPosition) const
{
	return (startPosition.X == targetPosition.X) && (startPosition.Y == targetPosition.Y) ? true : false;
}

void AA_Star_AlgorithmBlockGrid::OpenListAdd(const int& currentX, const int& currentY)
{
	if (!CheckException(FVector2D(currentX, currentY)))
	{
		return;
	}

	if (bAllowDiagonal == true)
	{
		if (mNodeArr[mCurrentNode->GetX()][currentY].GetIsWall() && mNodeArr[currentX][mCurrentNode->GetY()].GetIsWall())
		{
			return;
		}
	}

	if (bDontCrossCorner == true)
	{
		if (mNodeArr[mCurrentNode->GetX()][currentY].GetIsWall() || mNodeArr[currentX][mCurrentNode->GetY()].GetIsWall())
		{
			return;
		}
	}

	Node_Info* neighborNode = &mNodeArr[currentX][currentY];
	
	//neighborNode->GetCurBlock()->SetSearchBlock();

	//10 or 14
	int moveAddtive = (mCurrentNode->GetX() - currentX) == 0 || (mCurrentNode->GetY() - currentY) == 0 ? 10 : 14;
	int moveCost = mCurrentNode->GetCostG() + moveAddtive;


	if (moveCost < neighborNode->GetCostG() || !mOpenList.Contains(neighborNode))
	{
		neighborNode->SetCostG(moveCost);
		neighborNode->SetCostH(curTargetBlock->GetBlockNumber());
		neighborNode->SetParentNode(mCurrentNode);
		mOpenList.Push(neighborNode);
	}
}//end of OpenListAdd


bool AA_Star_AlgorithmBlockGrid::CheckException(const FVector2D& targetPosition)
{
	const int X = targetPosition.X;
	const int Y = targetPosition.Y;
	if (X < 0 || X >= GridSize || Y < 0 || Y >= GridSize)
	{
		return false; //out Of Range
	}
	if ((mClosedList.Contains(&mNodeArr[X][Y])) || mNodeArr[X][Y].GetIsWall())
	{
		return false; //already Valid or Is Wall
	}

	return true;
}//end of CheckException


void AA_Star_AlgorithmBlockGrid::SetRelease()
{
	if (mOpenList.Num())
		mOpenList.Empty();

	if (mClosedList.Num())
		mClosedList.Empty();

	if (mFinalPathList.Num())
		mFinalPathList.Empty();
}//end of SetRelase



void AA_Star_AlgorithmBlockGrid::ReverseArray()
{
	FVector2D temp;
	const int max = mFinalPathList.Num();

	for (int i = 0; i < max / 2; i++)
	{
		temp = mFinalPathList[i];
		mFinalPathList[i] = mFinalPathList[max - i - 1];
		mFinalPathList[max - i - 1] = temp;
	}
}

void AA_Star_AlgorithmBlockGrid::SetWallBlock(const FVector2D& targetPosition,const bool& IsWall)
{
	const int x = targetPosition.X;
	const int y = targetPosition.Y;

	mNodeArr[x][y].SetWall(IsWall);
}//end of SetWallBlock


void AA_Star_AlgorithmBlockGrid::AllClearBlock()
{

	const int SIZE = curPathBlocks.Num();

	for (int i = 0; i < SIZE; i++)
	{
		curPathBlocks[i]->SetPathBlock();
	
	}

	curPathBlocks.Empty();


	if (curStartBlock != nullptr)
	{ 
		curStartBlock->SetBasicMaterial();
		curStartBlock->SetIsClicked(false);
		curStartBlock = nullptr;
	}

	if (curTargetBlock != nullptr)
	{ 
		curTargetBlock->SetBasicMaterial();
		curTargetBlock->SetIsClicked(false);
		curTargetBlock = nullptr;
	}
}


void AA_Star_AlgorithmBlockGrid::DrawPath(const FVector2D& drawPosition,const int& blockNumber)
{
	const int x = drawPosition.X;
	const int y = drawPosition.Y;

	mNodeArr[x][y].GetCurBlock()->SetPathBlock();

	mNodeArr[x][y].GetCurBlock()->SetTextNumber(FText::FromString(FString::FromInt(blockNumber)));


	curPathBlocks.Push(mNodeArr[x][y].GetCurBlock());
}


void AA_Star_AlgorithmBlockGrid::SpawnBlocks()
{
	const int32 NumBlocks = (GridSize * GridSize);

	for (int32 BlockIndex = 0; BlockIndex < NumBlocks; BlockIndex++)
	{
		const float XOffset = (BlockIndex / GridSize) * BlockSpacing; // Divide by dimension
		const float YOffset = (BlockIndex%GridSize) * BlockSpacing; // Modulo gives remainder

		// Make position vector, offset from Grid location
		const FVector BlockLocation = FVector(XOffset, YOffset, 0.f) + GetActorLocation();

		// Spawn a block
		AA_Star_AlgorithmBlock* spawnedBlock = GetWorld()->SpawnActor<AA_Star_AlgorithmBlock>(BlockLocation, FRotator(0, 0, 0));

		// Tell the block about its owner
		if (spawnedBlock != nullptr)
		{
			spawnedBlock->SetOwingGrid(this);

			const int X_Value = XOffset / BlockSpacing;
			const int Y_Value = YOffset / BlockSpacing;

			spawnedBlock->SetBlockNumber(FVector2D(X_Value, Y_Value));

			mNodeArr[X_Value][Y_Value].SetCurBlock(spawnedBlock);

			mNodeArr[X_Value][Y_Value].SetPosition(X_Value, Y_Value);
		}//end of if
	}//end of for
}

void AA_Star_AlgorithmBlockGrid::MovingTimer()
{
	if (mCharacterPath.Num() <= mCurPathCountNum)
	{
		GetWorldTimerManager().ClearTimer(mCountdownTimerHandle); 
		return;
	}
	else
	{
		UE_LOG(LogTemp, Log, TEXT("%d"), mCurPathCountNum);

		const FVector2D curDrawPosition = this->mCharacterPath[mCurPathCountNum];

		DrawPath(curDrawPosition, mCurPathCountNum); 
	}
	mCurPathCountNum++;
}

#undef LOCTEXT_NAMESPACE
