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
		ConstructorHelpers::FObjectFinderOptional<UMaterialInstance> PinkMaterial; //�߰�
		ConstructorHelpers::FObjectFinderOptional<UMaterialInstance> RedMaterial; //�߰�
		ConstructorHelpers::FObjectFinderOptional<UMaterialInstance> PurpleMaterial;
		ConstructorHelpers::FObjectFinderOptional<UMaterialInstance> BlackMaterial;
		FConstructorStatics()
			: PlaneMesh(TEXT("/Game/Puzzle/Meshes/PuzzleCube.PuzzleCube"))
			, BaseMaterial(TEXT("/Game/Puzzle/Meshes/BaseMaterial.BaseMaterial"))
			, BlueMaterial(TEXT("/Game/Puzzle/Meshes/BlueMaterial.BlueMaterial"))
			, OrangeMaterial(TEXT("/Game/Puzzle/Meshes/OrangeMaterial.OrangeMaterial"))
			, PinkMaterial(TEXT("/Game/Puzzle/Meshes/PinkMaterial.PinkMaterial")) //�߰�
			, RedMaterial(TEXT("/Game/Puzzle/Meshes/RedMaterial.RedMaterial")) //�߰�
			, PurpleMaterial(TEXT("/Game/Puzzle/Meshes/PurpleMaterial.PurpleMaterial")) //�߰�
			, BlackMaterial(TEXT("/Game/Puzzle/Meshes/BlackMaterial.BlackMaterial"))//�߰�
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


	//�߰�
	BlockText=CreateDefaultSubobject<UTextRenderComponent>(TEXT("TEXT"));
	BlockText->SetupAttachment(DummyRoot);

	//TEXT ��ġ����
	BlockText->SetRelativeLocation(FVector(-60.f,-100.f,60.0f));
	BlockText->SetRelativeRotation(FRotator(90.f,180.f,0.f));
	BlockText->SetXScale(5.f);
	BlockText->SetYScale(5.f);

	//����,�ʹ� ����
	BlockText->SetTextRenderColor(FColor::Black);
	BlockText->SetVisibility(false);

	// Save a pointer to the orange material
	MouseOverlapMaterial = ConstructorStatics.BaseMaterial.Get();
	BlueMaterial = ConstructorStatics.BlueMaterial.Get();
	OrangeMaterial = ConstructorStatics.OrangeMaterial.Get();
	PurpleMaterial = ConstructorStatics.PurpleMaterial.Get();
	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	//�߰�
	PinkMaterial = ConstructorStatics.PinkMaterial.Get();
	RedMaterial = ConstructorStatics.RedMaterial.Get();
	BlackMaterial = ConstructorStatics.BlackMaterial.Get();


	//�� �ʱ�ȭ
	Cur_BlockNumber = FVector2D(0.0f, 0.0f);
	bIsPath=false;
}

void AA_Star_AlgorithmBlock::HandleClicked(){
	//Check OwningGrid
	if (OwningGrid == nullptr) return;

	if (!bIsClicked){
		if (OwningGrid->Cur_StartBlock == nullptr) { //���� ���� ����
			BlockMesh->SetMaterial(0, OrangeMaterial); //���� �� ���� ����
			OwningGrid->SelectStartBlock(this);//���� �� ����			
			bIsClicked = true;

		}
		else if(OwningGrid->Cur_TargetBlock == nullptr) { //��ǥ���� ����
			BlockMesh->SetMaterial(0, RedMaterial); //��ǥ���� ���� ����
			OwningGrid->SelectTargetBlock(this); //��ǥ ��� ����
			bIsClicked = true;
		}
	}
}

void AA_Star_AlgorithmBlock::Highlight(bool p_bOnOverlap){
	
	//Check OwningGrid
	if (OwningGrid == nullptr) return;

	// ���̶���Ʈ ȿ�� �� �˻� (�̹� �������ִ���/��κ��/�� ����)
	if (bIsClicked||bIsPath||bIsWall) return;


	//���콺 Ŀ���� ������ �Ǿ����� ���� ����
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
/*�⺻ ���� ���󺯰�*/
void AA_Star_AlgorithmBlock::SetBasicMaterial(){
	BlockMesh->SetMaterial(0, BlueMaterial);
}

//�� ��� ����
void AA_Star_AlgorithmBlock::SetPathBlock(){
	//IsPath(�������)�˻��Ͽ� ��η� �����Ǿ� ���� ������ ��� ������� ����
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
//�� ���� ����
void AA_Star_AlgorithmBlock::SetWallBlock(bool IsWall){
	//�̹� Ŭ���Ǿ� �ְų�, ��η� �Ǿ��ִٸ� �Ұ���
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

//��� �ѹ� ����//////////////////////////////////////////////////////////////////////////

//��� �ѹ� �Ҵ�
void AA_Star_AlgorithmBlock::SetBlockNumber(const FVector2D p_NewBlockNum){
	Cur_BlockNumber=p_NewBlockNum; 
} 
//�ش� Text�� �۾� ����
void AA_Star_AlgorithmBlock::SetTextNumber(const FText p_NewNumber){
	BlockText->SetText(p_NewNumber);
}
//��Ÿ//////////////////////////////////////////////////////////////////////////////////////////
//���� Grid ����
void AA_Star_AlgorithmBlock::SetOwingGrid(AA_Star_AlgorithmBlockGrid* p_OwningGrid){
	OwningGrid=p_OwningGrid;
}

//�������� ����
void AA_Star_AlgorithmBlock::SetIsClicked(bool p_bIsClicked){
	bIsClicked=p_bIsClicked;
}

