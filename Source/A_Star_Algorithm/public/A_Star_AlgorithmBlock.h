// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "A_Star_AlgorithmBlockGrid.h"
#include "A_Star_AlgorithmBlock.generated.h"

UCLASS(minimalapi)
class AA_Star_AlgorithmBlock : public AActor
{
	GENERATED_BODY()

private:
	UPROPERTY(Category = Block, VisibleDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	class USceneComponent* DummyRoot;

	UPROPERTY(Category = Block, VisibleDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	class UStaticMeshComponent* BlockMesh;
	
public:
	AA_Star_AlgorithmBlock();

public:
	void HandleClicked();

	void Highlight(bool bOn);

//Setter
public:
	void SetBasicMaterial();

	void SetPathBlock();

	void SetWallBlock(bool IsWall);

	void SetBlockNumber(const FVector2D NewNum);

	void SetTextNumber(const FText NewNumber);

	void SetOwingGrid(AA_Star_AlgorithmBlockGrid* p_OwningGrid);

	void SetIsClicked(bool p_bIsClicked);

//Getter
public:
	bool GetIsWall() const{return bIsWall;}
	FVector2D GetBlockNumber() const{return Cur_BlockNumber;}
	bool GetIsClicked() const{return bIsClicked;}

private:

	/** Are we currently active? */
	UPROPERTY(VisibleAnywhere, Category = "State")
	bool bIsClicked;

	UPROPERTY(VisibleAnywhere, Category = "State")
	bool bIsWall;

	bool bIsPath;


	/*ǥ�ÿ�*/
	UPROPERTY(Category = Block, VisibleDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	class UTextRenderComponent* BlockText;


	UPROPERTY(VisibleAnywhere, Category = "Grid")
	FVector2D Cur_BlockNumber;

	/** Pointer to blue material used on inactive blocks */
	class UMaterialInstance* BlueMaterial; //�⺻����

	/** Pointer to white material used on the focused block */

	class UMaterial* MouseOverlapMaterial; //�������� ���콺 ������

	class UMaterialInstance* PinkMaterial; //Ÿ������ ������

	class UMaterialInstance* OrangeMaterial; //��������

	class UMaterialInstance* RedMaterial; //Ÿ������

	class UMaterialInstance* PurpleMaterial; //���

	class UMaterialInstance* BlackMaterial; //��

	//Owner Gird
	AA_Star_AlgorithmBlockGrid* OwningGrid;


};



