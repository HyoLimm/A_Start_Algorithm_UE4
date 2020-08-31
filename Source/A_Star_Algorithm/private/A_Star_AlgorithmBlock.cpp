// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#include "A_Star_AlgorithmBlock.h"
#include "A_Star_AlgorithmBlockGrid.h"
#include "UObject/ConstructorHelpers.h"
#include "Components/StaticMeshComponent.h"
#include "Components/TextRenderComponent.h"
#include "Engine/StaticMesh.h"
#include "Materials/MaterialInstance.h"

AA_Star_AlgorithmBlock::AA_Star_AlgorithmBlock()
{
	// Structure to hold one-time initialization
	struct FConstructorStatics
	{
		ConstructorHelpers::FObjectFinderOptional<UStaticMesh> PlaneMesh;
		ConstructorHelpers::FObjectFinderOptional<UMaterial> BaseMaterial;
		ConstructorHelpers::FObjectFinderOptional<UMaterialInstance> BlueMaterial;
		ConstructorHelpers::FObjectFinderOptional<UMaterialInstance> OrangeMaterial;
		ConstructorHelpers::FObjectFinderOptional<UMaterialInstance> PinkMaterial; //추가
		ConstructorHelpers::FObjectFinderOptional<UMaterialInstance> RedMaterial; //추가
		ConstructorHelpers::FObjectFinderOptional<UMaterialInstance> PurpleMaterial;
		ConstructorHelpers::FObjectFinderOptional<UMaterialInstance> BlackMaterial;
		FConstructorStatics()
			: PlaneMesh(TEXT("/Game/Puzzle/Meshes/PuzzleCube.PuzzleCube"))
			, BaseMaterial(TEXT("/Game/Puzzle/Meshes/BaseMaterial.BaseMaterial"))
			, BlueMaterial(TEXT("/Game/Puzzle/Meshes/BlueMaterial.BlueMaterial"))
			, OrangeMaterial(TEXT("/Game/Puzzle/Meshes/OrangeMaterial.OrangeMaterial"))
			, PinkMaterial(TEXT("/Game/Puzzle/Meshes/PinkMaterial.PinkMaterial")) //추가
			, RedMaterial(TEXT("/Game/Puzzle/Meshes/RedMaterial.RedMaterial")) //추가
			, PurpleMaterial(TEXT("/Game/Puzzle/Meshes/PurpleMaterial.PurpleMaterial")) //추가
			, BlackMaterial(TEXT("/Game/Puzzle/Meshes/BlackMaterial.BlackMaterial"))//추가
		{}
	};
	static FConstructorStatics ConstructorStatics;

	// Create dummy root scene component
	DummyRoot = CreateDefaultSubobject<USceneComponent>(TEXT("Dummy0"));
	RootComponent = DummyRoot;

	// Create static mesh component
	BlockMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("BlockMesh0"));
	BlockMesh->SetStaticMesh(ConstructorStatics.PlaneMesh.Get());
	BlockMesh->SetRelativeScale3D(FVector(1.f,1.f,0.25f));
	BlockMesh->SetRelativeLocation(FVector(0.f,0.f,25.f));
	BlockMesh->SetMaterial(0, ConstructorStatics.BlueMaterial.Get());
	BlockMesh->SetupAttachment(DummyRoot);
	//BlockMesh->OnClicked.AddDynamic(this, &AA_Star_AlgorithmBlock::BlockClicked);
	//BlockMesh->OnInputTouchBegin.AddDynamic(this, &AA_Star_AlgorithmBlock::OnFingerPressedBlock);


	//추가
	BlockText=CreateDefaultSubobject<UTextRenderComponent>(TEXT("TEXT"));
	BlockText->SetupAttachment(DummyRoot);

	//TEXT 위치조절
	BlockText->SetRelativeLocation(FVector(-60.f,-100.f,60.0f));
	BlockText->SetRelativeRotation(FRotator(90.f,180.f,0.f));
	BlockText->SetXScale(5.f);
	BlockText->SetYScale(5.f);

	//색상,초반 숨김
	BlockText->SetTextRenderColor(FColor::Black);
	BlockText->SetVisibility(false);

	// Save a pointer to the orange material
	MouseOverlapMaterial = ConstructorStatics.BaseMaterial.Get();
	BlueMaterial = ConstructorStatics.BlueMaterial.Get();
	OrangeMaterial = ConstructorStatics.OrangeMaterial.Get();
	PurpleMaterial = ConstructorStatics.PurpleMaterial.Get();
	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	//추가
	PinkMaterial = ConstructorStatics.PinkMaterial.Get();
	RedMaterial = ConstructorStatics.RedMaterial.Get();
	BlackMaterial = ConstructorStatics.BlackMaterial.Get();


	//값 초기화
	Cur_BlockNumber = FVector2D(0.0f, 0.0f);
	bIsPath=false;
}

void AA_Star_AlgorithmBlock::HandleClicked(){
	//Check OwningGrid
	if (OwningGrid == nullptr) return;

	if (!bIsClicked){
		if (OwningGrid->Cur_StartBlock == nullptr) { //시작 지점 지정
			BlockMesh->SetMaterial(0, OrangeMaterial); //시작 블럭 색상 변경
			OwningGrid->SelectStartBlock(this);//시작 블럭 지정			
			bIsClicked = true;

		}
		else if(OwningGrid->Cur_TargetBlock == nullptr) { //목표지정 지정
			BlockMesh->SetMaterial(0, RedMaterial); //목표지점 색상 변경
			OwningGrid->SelectTargetBlock(this); //목표 블록 지정
			bIsClicked = true;
		}
	}
}

void AA_Star_AlgorithmBlock::Highlight(bool p_bOnOverlap){
	
	//Check OwningGrid
	if (OwningGrid == nullptr) return;

	// 하이라이트 효과 전 검사 (이미 눌러져있는제/경로블록/벽 여부)
	if (bIsClicked||bIsPath||bIsWall) return;


	//마우스 커서가 오버랩 되었을때 색상 변경
	if (p_bOnOverlap){
		if (OwningGrid->Cur_StartBlock == nullptr && OwningGrid->Cur_TargetBlock == nullptr) {
			BlockMesh->SetMaterial(0, MouseOverlapMaterial);
		}else if (OwningGrid->Cur_TargetBlock == nullptr) {
			BlockMesh->SetMaterial(0, PinkMaterial);
		}		
	}
	else{
		this->SetBasicMaterial();
	}
}
//////////////////Setter//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*기본 상태 색상변경*/
void AA_Star_AlgorithmBlock::SetBasicMaterial(){
	BlockMesh->SetMaterial(0, BlueMaterial);
}

//길 블록 여부
void AA_Star_AlgorithmBlock::SetPathBlock(){
	//IsPath(현재상태)검사하여 경로로 지정되어 있지 않으면 경로 블록으로 설정
	if(bIsPath==false){
		bIsPath=true;
		BlockMesh->SetMaterial(0, PurpleMaterial);
		BlockText->SetVisibility(true);
	}
	else if(bIsPath) {
		bIsPath = false;
		this->SetBasicMaterial();
		bIsClicked=false;
		BlockText->SetVisibility(false);
	}
}
//벽 여부 변경
void AA_Star_AlgorithmBlock::SetWallBlock(bool IsWall){
	//이미 클릭되어 있거나, 경로로 되어있다면 불가능
	if(bIsClicked ||bIsPath) return;

	if (IsWall) {
		BlockMesh->SetMaterial(0, BlackMaterial);
		bIsWall=true;
		OwningGrid->SetWalllBlock(this->GetBlockNumber(),true);
	}
	else {
		this->SetBasicMaterial();
		bIsWall = false;
		OwningGrid->SetWalllBlock(this->GetBlockNumber(),false);
	}
}

//블록 넘버 관련//////////////////////////////////////////////////////////////////////////

//블록 넘버 할당
void AA_Star_AlgorithmBlock::SetBlockNumber(const FVector2D p_NewBlockNum){
	Cur_BlockNumber=p_NewBlockNum; 
} 
//해당 Text에 글씨 설정
void AA_Star_AlgorithmBlock::SetTextNumber(const FText p_NewNumber){
	BlockText->SetText(p_NewNumber);
}
//기타//////////////////////////////////////////////////////////////////////////////////////////
//주인 Grid 설정
void AA_Star_AlgorithmBlock::SetOwingGrid(AA_Star_AlgorithmBlockGrid* p_OwningGrid){
	OwningGrid=p_OwningGrid;
}

//눌리는지 여부
void AA_Star_AlgorithmBlock::SetIsClicked(bool p_bIsClicked){
	bIsClicked=p_bIsClicked;
}

