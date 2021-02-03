// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#include "A_Star_AlgorithmBlockGrid.h"
#include "A_Star_AlgorithmBlock.h"
#include "Components/TextRenderComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/World.h"

#define LOCTEXT_NAMESPACE "PuzzleBlockGrid"

AA_Star_AlgorithmBlockGrid::AA_Star_AlgorithmBlockGrid() {
	// Create dummy root scene component
	RootComp = CreateDefaultSubobject<USceneComponent>(TEXT("Dummy0"));
	RootComponent = RootComp;

	// Set defaults
	GridSize = 20;
	BlockSpacing = 300.f; 


	bAllowDiagonal = false;
	bDontCrossCorner = false;
	mMovingTime = 0.1f;
	mCurPathCountNum = 0;
	//Set Null
	curStartBlock = nullptr;
	curTargetBlock = nullptr;


	mDirInfo[0] = FVector2D(1, 0);	//up
	mDirInfo[1] = FVector2D(0, 1); //right
	mDirInfo[2] = FVector2D(-1, 0); //down 
	mDirInfo[3] = FVector2D(0, -1); //left

	mDirDiagnolInfo[0] = FVector2D(1, 1);
	mDirDiagnolInfo[1] = FVector2D(-1, 1); 
	mDirDiagnolInfo[2] = FVector2D(-1, -1); 
	mDirDiagnolInfo[3] = FVector2D(1, -1);


}//end of AA_Star_AlgorithmBlockGrid();


void AA_Star_AlgorithmBlockGrid::BeginPlay() {
	Super::BeginPlay();

	SpawnBlocks();

}//end of BeginPlay()

bool AA_Star_AlgorithmBlockGrid::CheckException(FVector2D p_CheckPosition)
{
	const int X = p_CheckPosition.X;
	const int Y = p_CheckPosition.Y;	
	if (X < 0 || X >= GridSize || Y < 0 || Y >= GridSize)
	{
		return false;
	}
	if ((mClosedList.Contains(&mNodeArr[X][Y])) || mNodeArr[X][Y].GetIsWall())
	{
		return false;
	}

	return true;

}//end of CheckException


void AA_Star_AlgorithmBlockGrid::SelectStartBlock(class AA_Star_AlgorithmBlock * p_StartingBlock)
{
	//Check StartBlock
	if (curStartBlock != nullptr)
	{
		return;
	}

	curStartBlock = p_StartingBlock;

}//End of SelectStartBlock

void AA_Star_AlgorithmBlockGrid::SelectTargetBlock(class AA_Star_AlgorithmBlock * p_TargetingBlock)
{
	if (curTargetBlock != nullptr)
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

bool AA_Star_AlgorithmBlockGrid::GetArriveTarget(FVector2D p_CurrentPoistion, FVector2D p_TargetPosition)
{
	return (p_CurrentPoistion.X == p_TargetPosition.X) && (p_CurrentPoistion.Y == p_TargetPosition.Y) ? true : false;
}


TArray<FVector2D> AA_Star_AlgorithmBlockGrid::GetPath_While(FVector2D p_Startposition, FVector2D p_TargetPosition)
{	
	this->SetRelease();

	mOpenList.Push(new Node_Info(p_Startposition, nullptr, 0, p_TargetPosition));

	while (mOpenList.Num() > 0)
	{
		mCurrentNode = mOpenList[0];


		int openSize = mOpenList.Num();
		for (int i = 0; i < openSize; i++)
		{
			int curListF = mOpenList[i]->GetCostF();
			int curListH = mOpenList[i]->GetCostH();

			if ((curListF <= mCurrentNode->GetCostF()) && (curListH < mCurrentNode->GetCostH()))
			{
				mCurrentNode = mOpenList[i];
				mCurrentNode->SetCurBlock(mOpenList[i]->GetCurBlock());
			}
		}

		mOpenList.Remove(mCurrentNode);
		mClosedList.Push(mCurrentNode);

 		const int curX = mCurrentNode->GetPoistion().X;
		const int curY = mCurrentNode->GetPoistion().Y;

		if (GetArriveTarget(mCurrentNode->GetPoistion(), p_TargetPosition))
		{

			Node_Info* TargetCurrent_node = &mNodeArr[curX][curY];

			while (TargetCurrent_node->GetPoistion() != p_Startposition)
			{
				mFinalPathList.Push(TargetCurrent_node->GetPoistion());
				TargetCurrent_node = TargetCurrent_node->GetParent();
			}

			mFinalPathList.Push(p_Startposition);

			ReverseArray();

			delete TargetCurrent_node;
			return mFinalPathList; 
		}


		//Diagonal
		if (bAllowDiagonal)
		{
			for (int i = 0; i < 4; i++) {
				const int x = curX + mDirDiagnolInfo[i].X;
				const int y = curY + mDirDiagnolInfo[i].Y;
				OpenListAdd(x, y);
			}
		}


		//Up Down Left Right
		for (int i = 0; i < 4; i++)
		{
			const int x = curX + mDirInfo[i].X;
			const int y = curY + mDirInfo[i].Y;
			OpenListAdd(x, y);
		}


	}//end of while

	return mFinalPathList;
}//end of GetPath_While

void AA_Star_AlgorithmBlockGrid::OpenListAdd(const int p_CheckX,const int p_CheckY)
{
	if (!CheckException(FVector2D(p_CheckX, p_CheckY)))
	{
		return;
	}

	if (bAllowDiagonal)
	{
		if (mNodeArr[mCurrentNode->GetX()][p_CheckY].GetIsWall() && mNodeArr[p_CheckX][mCurrentNode->GetY()].GetIsWall())
		{
			return;
		}
	}

	if (bDontCrossCorner)
	{
		if (mNodeArr[mCurrentNode->GetX()][p_CheckY].GetIsWall() || mNodeArr[p_CheckX][mCurrentNode->GetY()].GetIsWall())
		{
			return;
		}
	}

	Node_Info* neighborNode = &mNodeArr[p_CheckX][p_CheckY];


	//10 or 14
	int moveAddtive = (mCurrentNode->GetX() - p_CheckX) == 0 || (mCurrentNode->GetY() - p_CheckY) == 0 ? 10 : 14;
	int moveCost = mCurrentNode->GetCostG() + moveAddtive;


	if (moveCost < neighborNode->GetCostG() || !mOpenList.Contains(neighborNode))
	{
		neighborNode->SetCostG(moveCost);  
		neighborNode->SetCostH(curTargetBlock->GetBlockNumber());		
		neighborNode->SetParentNode(mCurrentNode);		
		mOpenList.Push(neighborNode);		
	}	
}//end of OpenListAdd


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


void AA_Star_AlgorithmBlockGrid::SetWalllBlock(FVector2D p_Position, bool p_IsWall)
{
	const int x = p_Position.X;
	const int y = p_Position.Y;

	mNodeArr[x][y].SetWall(p_IsWall);
}//end of SetWallBlock


void AA_Star_AlgorithmBlockGrid::AllClearBlock()
{
	
	const int size = Current_PathBlocks.Num();

	for (int i = 0; i < size; i++)
	{
		Current_PathBlocks[i]->SetPathBlock();
	}

	Current_PathBlocks.Empty();


	if (curStartBlock != nullptr)
	{ 
		curStartBlock->SetBasicMaterial();
		curStartBlock->SetIsClicked(false);
		curStartBlock = nullptr;
	}

	if (curTargetBlock != nullptr) { 
		curTargetBlock->SetBasicMaterial();
		curTargetBlock->SetIsClicked(false);
		curTargetBlock = nullptr;
	}

}


void AA_Star_AlgorithmBlockGrid::DrawPath(FVector2D p_DrawPosition, int p_CurBlockNumber)
{
	const int x = p_DrawPosition.X;
	const int y = p_DrawPosition.Y;


	mNodeArr[x][y].GetCurBlock()->SetPathBlock();


	mNodeArr[x][y].GetCurBlock()->SetTextNumber(FText::FromString(FString::FromInt(p_CurBlockNumber)));

	Current_PathBlocks.Push(mNodeArr[x][y].GetCurBlock());
}


void AA_Star_AlgorithmBlockGrid::SpawnBlocks()
{
	// 釉붾줉??珥?媛?닔
	const int32 NumBlocks = GridSize * GridSize;

	// 釉붾줉 ?앹꽦 諛섎났
	for (int32 BlockIndex = 0; BlockIndex < NumBlocks; BlockIndex++)
	{

		const float XOffset = (BlockIndex / GridSize) * BlockSpacing; // Divide by dimension
		const float YOffset = (BlockIndex%GridSize) * BlockSpacing; // Modulo gives remainder

		// Make position vector, offset from Grid location
		const FVector BlockLocation = FVector(XOffset, YOffset, 0.f) + GetActorLocation();

		// Spawn a block
		AA_Star_AlgorithmBlock* NewBlock = GetWorld()->SpawnActor<AA_Star_AlgorithmBlock>(BlockLocation, FRotator(0, 0, 0));


		// Tell the block about its owner
		if (NewBlock != nullptr)
		{


			NewBlock->SetOwingGrid(this);

			const int X_Value = XOffset / BlockSpacing;
			const int Y_Value = YOffset / BlockSpacing;


			NewBlock->SetBlockNumber(FVector2D(X_Value, Y_Value));

	
			mNodeArr[X_Value][Y_Value].SetCurBlock(NewBlock);

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

		const FVector2D Cur_DrawPosition = this->mCharacterPath[mCurPathCountNum];


		DrawPath(Cur_DrawPosition, mCurPathCountNum); 
	}
	mCurPathCountNum++;
}

#undef LOCTEXT_NAMESPACE
