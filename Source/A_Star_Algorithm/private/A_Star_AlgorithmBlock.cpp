// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#include "A_Star_AlgorithmBlock.h"
#include "A_Star_AlgorithmBlockGrid.h"
#include "UObject/ConstructorHelpers.h"
#include "Components/StaticMeshComponent.h"
#include "Components/TextRenderComponent.h"
#include "Engine/StaticMesh.h"
#include "Materials/MaterialInstance.h"

AA_Star_AlgorithmBlock::AA_Star_AlgorithmBlock()
	: bIsClicked(false)
	, bIsWall(false)
	, bIsPath(false)
	, mOwningGridRef(nullptr)
	, mNumBlock(FVector2D(0.0f, 0.0f))
{
	// Structure to hold one-time initialization
	struct FConstructorStatics
	{
		ConstructorHelpers::FObjectFinderOptional<UStaticMesh> PlaneMesh;
		ConstructorHelpers::FObjectFinderOptional<UMaterial> BaseMaterial;
		ConstructorHelpers::FObjectFinderOptional<UMaterialInstance> BlueMaterial;
		ConstructorHelpers::FObjectFinderOptional<UMaterialInstance> OrangeMaterial;
		ConstructorHelpers::FObjectFinderOptional<UMaterialInstance> PinkMaterial; //추가
		ConstructorHelpers::FObjectFinderOptional<UMaterialInstance> RedMaterial;
		ConstructorHelpers::FObjectFinderOptional<UMaterialInstance> PurpleMaterial;
		ConstructorHelpers::FObjectFinderOptional<UMaterialInstance> BlackMaterial;
		ConstructorHelpers::FObjectFinderOptional<UMaterialInstance> mSearchMaterial;
		FConstructorStatics()
			: PlaneMesh(TEXT("/Game/Puzzle/Meshes/PuzzleCube.PuzzleCube"))
			, BaseMaterial(TEXT("/Game/Puzzle/Meshes/BaseMaterial.BaseMaterial"))
			, BlueMaterial(TEXT("/Game/Puzzle/Meshes/BlueMaterial.BlueMaterial"))
			, OrangeMaterial(TEXT("/Game/Puzzle/Meshes/OrangeMaterial.OrangeMaterial"))
			, PinkMaterial(TEXT("/Game/Puzzle/Meshes/PinkMaterial.PinkMaterial")) //추가
			, RedMaterial(TEXT("/Game/Puzzle/Meshes/RedMaterial.RedMaterial"))
			, PurpleMaterial(TEXT("/Game/Puzzle/Meshes/PurpleMaterial.PurpleMaterial"))
			, BlackMaterial(TEXT("/Game/Puzzle/Meshes/BlackMaterial.BlackMaterial"))
			, mSearchMaterial(TEXT("/Game/Puzzle/Meshes/SeachMaterial.SeachMaterial"))
		{}
	};
	static FConstructorStatics ConstructorStatics;

	mDummyRoot = CreateDefaultSubobject<USceneComponent>(TEXT("Dummy0"));
	RootComponent = mDummyRoot;

	//Set StaticMesh
	mBlockMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("BlockMesh0"));
	mBlockMesh->SetStaticMesh(ConstructorStatics.PlaneMesh.Get());
	mBlockMesh->SetRelativeScale3D(FVector(1.f, 1.f, 0.25f));
	mBlockMesh->SetRelativeLocation(FVector(0.f, 0.f, 25.f));
	mBlockMesh->SetMaterial(0, ConstructorStatics.BlueMaterial.Get());
	mBlockMesh->SetupAttachment(mDummyRoot);

	//추가
	mBlockText = CreateDefaultSubobject<UTextRenderComponent>(TEXT("TEXT"));
	mBlockText->SetupAttachment(mDummyRoot);

	//TEXT 위치조절
	mBlockText->SetRelativeLocation(FVector(-60.f, -100.f, 60.0f));
	mBlockText->SetRelativeRotation(FRotator(90.f, 180.f, 0.f));
	mBlockText->SetXScale(5.f);
	mBlockText->SetYScale(5.f);

	//색상,초반 숨김
	mBlockText->SetTextRenderColor(FColor::Black);
	mBlockText->SetVisibility(false);

	// Save a pointer to the orange material
	MouseOverlapMaterial = ConstructorStatics.BaseMaterial.Get();
	mBaseMaterial = ConstructorStatics.BlueMaterial.Get();
	mOrangeMaterial = ConstructorStatics.OrangeMaterial.Get();
	mPurpleMaterial = ConstructorStatics.PurpleMaterial.Get();
	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	//추가
	mPinkMaterial = ConstructorStatics.PinkMaterial.Get();
	mRedMaterial = ConstructorStatics.RedMaterial.Get();
	mBlackMaterial = ConstructorStatics.BlackMaterial.Get();
	mSearchMaterial = ConstructorStatics.mSearchMaterial.Get();
}

void AA_Star_AlgorithmBlock::HandleClicked() {
	//Check OwningGrid
	if (mOwningGridRef == nullptr) return;

	if (!bIsClicked) {
		if (mOwningGridRef->curStartBlock == nullptr) { //시작 지점 지정
			mBlockMesh->SetMaterial(0, mOrangeMaterial); //시작 블럭 색상 변경
			mOwningGridRef->SelectStartBlock(this);//시작 블럭 지정			
			bIsClicked = true;

		}
		else if (mOwningGridRef->curTargetBlock == nullptr) { //목표지정 지정
			mBlockMesh->SetMaterial(0, mRedMaterial); //목표지점 색상 변경
			mOwningGridRef->SelectTargetBlock(this); //목표 블록 지정
			bIsClicked = true;
		}
	}
}

void AA_Star_AlgorithmBlock::Highlight(bool p_bOnOverlap) {

	//Check OwningGrid
	if (mOwningGridRef == nullptr)
		return;

	// 하이라이트 효과 전 검사 (이미 눌러져있는제/경로블록/벽 여부)
	if (bIsClicked || bIsPath || bIsWall)
		return;


	//마우스 커서가 오버랩 되었을때 색상 변경
	if (p_bOnOverlap) 
	{
		if (mOwningGridRef->curStartBlock == nullptr && mOwningGridRef->curTargetBlock == nullptr) 
		{
			mBlockMesh->SetMaterial(0, MouseOverlapMaterial);
		}
		else if (mOwningGridRef->curTargetBlock == nullptr) 
		{
			mBlockMesh->SetMaterial(0, mPinkMaterial);
		}
	}
	else
	{
		this->SetBasicMaterial();
	}
}
//////////////////Setter//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*기본 상태 색상변경*/
void AA_Star_AlgorithmBlock::SetBasicMaterial()
{
	mBlockMesh->SetMaterial(0, mBaseMaterial);
}

//길 블록 여부
void AA_Star_AlgorithmBlock::SetPathBlock() 
{
	//IsPath(현재상태)검사하여 경로로 지정되어 있지 않으면 경로 블록으로 설정
	if (bIsPath == false) 
	{
		bIsPath = true;
		mBlockMesh->SetMaterial(0, mPurpleMaterial);
		mBlockText->SetVisibility(true);
	}
	else if (bIsPath)
	{
		bIsPath = false;
		this->SetBasicMaterial();
		bIsClicked = false;
		mBlockText->SetVisibility(false);
	}
}

void AA_Star_AlgorithmBlock::SetSearchBlock()
{
	mBlockMesh->SetMaterial(0, mSearchMaterial);
}

//벽 여부 변경
void AA_Star_AlgorithmBlock::SetWallBlock(const bool& IsWall) {
	//이미 클릭되어 있거나, 경로로 되어있다면 벽으로 설정 불가능
	if (bIsClicked || bIsPath)
	{
		return;
	}

	if (IsWall)
	{
		mBlockMesh->SetMaterial(0, mBlackMaterial);
		bIsWall = true;
		mOwningGridRef->SetWallBlock(this->GetBlockNumber(), true);
	}
	else 
	{
		this->SetBasicMaterial();
		bIsWall = false;
		mOwningGridRef->SetWallBlock(this->GetBlockNumber(), false);
	}
}

void AA_Star_AlgorithmBlock::SetBlockNumber(const FVector2D& newNum) 
{
	mNumBlock = newNum;
}

void AA_Star_AlgorithmBlock::SetTextNumber(const FText& newNumText)
{
	mBlockText->SetText(newNumText);
}


void AA_Star_AlgorithmBlock::SetOwingGrid(AA_Star_AlgorithmBlockGrid* owningGrid)
{
	mOwningGridRef = owningGrid;
}

void AA_Star_AlgorithmBlock::SetIsClicked(const bool& IsClicked)
{
	bIsClicked = IsClicked;
}

