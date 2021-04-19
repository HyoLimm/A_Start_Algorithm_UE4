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
		ConstructorHelpers::FObjectFinderOptional<UMaterialInstance> PinkMaterial; //異붽?
		ConstructorHelpers::FObjectFinderOptional<UMaterialInstance> RedMaterial;
		ConstructorHelpers::FObjectFinderOptional<UMaterialInstance> PurpleMaterial;
		ConstructorHelpers::FObjectFinderOptional<UMaterialInstance> BlackMaterial;
		ConstructorHelpers::FObjectFinderOptional<UMaterialInstance> mSearchMaterial;
		FConstructorStatics()
			: PlaneMesh(TEXT("/Game/Puzzle/Meshes/PuzzleCube.PuzzleCube"))
			, BaseMaterial(TEXT("/Game/Puzzle/Meshes/BaseMaterial.BaseMaterial"))
			, BlueMaterial(TEXT("/Game/Puzzle/Meshes/BlueMaterial.BlueMaterial"))
			, OrangeMaterial(TEXT("/Game/Puzzle/Meshes/OrangeMaterial.OrangeMaterial"))
			, PinkMaterial(TEXT("/Game/Puzzle/Meshes/PinkMaterial.PinkMaterial")) //異붽?
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

	//異붽?
	mBlockText = CreateDefaultSubobject<UTextRenderComponent>(TEXT("TEXT"));
	mBlockText->SetupAttachment(mDummyRoot);

	//TEXT ?꾩튂議곗젅
	mBlockText->SetRelativeLocation(FVector(-60.f, -100.f, 60.0f));
	mBlockText->SetRelativeRotation(FRotator(90.f, 180.f, 0.f));
	mBlockText->SetXScale(5.f);
	mBlockText->SetYScale(5.f);

	//?됱긽,珥덈컲 ?④?
	mBlockText->SetTextRenderColor(FColor::Black);
	mBlockText->SetVisibility(false);

	// Save a pointer to the orange material
	MouseOverlapMaterial = ConstructorStatics.BaseMaterial.Get();
	mBaseMaterial = ConstructorStatics.BlueMaterial.Get();
	mOrangeMaterial = ConstructorStatics.OrangeMaterial.Get();
	mPurpleMaterial = ConstructorStatics.PurpleMaterial.Get();
	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	//異붽?
	mPinkMaterial = ConstructorStatics.PinkMaterial.Get();
	mRedMaterial = ConstructorStatics.RedMaterial.Get();
	mBlackMaterial = ConstructorStatics.BlackMaterial.Get();
	mSearchMaterial = ConstructorStatics.mSearchMaterial.Get();
}

void AA_Star_AlgorithmBlock::HandleClicked() {
	//Check OwningGrid
	if (mOwningGridRef == nullptr) return;

	if (!bIsClicked) {
		if (mOwningGridRef->GetCurStartBlock() == nullptr) { //?쒖옉 吏??吏??
			mBlockMesh->SetMaterial(0, mOrangeMaterial); //?쒖옉 釉붾윮 ?됱긽 蹂寃?
			mOwningGridRef->SelectStartBlock(this);//?쒖옉 釉붾윮 吏??		
			bIsClicked = true;

		}
		else if (mOwningGridRef->GetCurTargetBlock() == nullptr) { //紐⑺몴吏??吏??
			mBlockMesh->SetMaterial(0, mRedMaterial); //紐⑺몴吏???됱긽 蹂寃?
			mOwningGridRef->SelectTargetBlock(this); //紐⑺몴 釉붾줉 吏??
			bIsClicked = true;
		}
	}
}

void AA_Star_AlgorithmBlock::Highlight(bool p_bOnOverlap) {

	//Check OwningGrid
	if (mOwningGridRef == nullptr)
		return;

	// ?섏씠?쇱씠???④낵 ??寃??(?대? ?뚮윭?몄엳?붿젣/寃쎈줈釉붾줉/踰??щ?)
	if (bIsClicked || bIsPath || bIsWall)
		return;


	//留덉슦??而ㅼ꽌媛 ?ㅻ쾭???섏뿀?꾨븣 ?됱긽 蹂寃?
	if (p_bOnOverlap) 
	{
		if (mOwningGridRef->GetCurStartBlock() == nullptr && mOwningGridRef->GetCurTargetBlock() == nullptr) 
		{
			mBlockMesh->SetMaterial(0, MouseOverlapMaterial);
		}
		else if (mOwningGridRef->GetCurTargetBlock() == nullptr) 
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
//湲곕낯 ?곹깭 ?됱긽蹂寃?/
void AA_Star_AlgorithmBlock::SetBasicMaterial()
{
	mBlockMesh->SetMaterial(0, mBaseMaterial);
}

//湲?釉붾줉 ?щ?
void AA_Star_AlgorithmBlock::SetPathBlock() 
{
	//IsPath(?꾩옱?곹깭)寃?ы븯??寃쎈줈濡?吏?뺣릺???덉? ?딆쑝硫?寃쎈줈 釉붾줉?쇰줈 ?ㅼ젙
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

//踰??щ? 蹂寃?
void AA_Star_AlgorithmBlock::SetWallBlock(const bool& IsWall) {
	//?대? ?대┃?섏뼱 ?덇굅?? 寃쎈줈濡??섏뼱?덈떎硫?踰쎌쑝濡??ㅼ젙 遺덇???
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

