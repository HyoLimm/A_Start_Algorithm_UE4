// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "A_Star_AlgorithmBlockGrid.h"
#include "A_Star_AlgorithmBlock.generated.h"

class UMaterialInstance;
class UTextRenderComponent;
class USceneComponent;
class UStaticMeshComponent;

UCLASS(minimalapi)
class AA_Star_AlgorithmBlock : public AActor
{
	GENERATED_BODY()
			   	
public:
	AA_Star_AlgorithmBlock();

public:
	void HandleClicked();

	void Highlight(bool bOn);

//Setter
public:
	void SetBasicMaterial();

	void SetPathBlock();

	void SetSearchBlock();

	void SetWallBlock(const bool& bIsWall);

	void SetBlockNumber(const FVector2D& newNum);

	void SetTextNumber(const FText& newNumText);

	void SetOwingGrid(AA_Star_AlgorithmBlockGrid* p_OwningGrid);

	void SetIsClicked(const bool& bIsClicked);

//Getter
public:
	FORCEINLINE bool GetIsWall() const{return bIsWall;}
	FORCEINLINE FVector2D GetBlockNumber() const{return mNumBlock;}
	FORCEINLINE bool GetIsClicked() const{return bIsClicked;}

private:
	UPROPERTY(Category = Block, VisibleDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	USceneComponent* mDummyRoot;

	UPROPERTY(Category = Block, VisibleDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	UStaticMeshComponent* mBlockMesh;
private:

	/** Are we currently active? */
	UPROPERTY(VisibleAnywhere, Category = "State")
	bool bIsClicked;

	UPROPERTY(VisibleAnywhere, Category = "State")
	bool bIsWall;

	bool bIsPath;

	AA_Star_AlgorithmBlockGrid* mOwningGridRef;
private: /*Visible*/	
	UPROPERTY(Category = Block, VisibleDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	UTextRenderComponent* mBlockText;

	UPROPERTY(VisibleAnywhere, Category = "Grid")
	FVector2D mNumBlock;

	class UMaterial* MouseOverlapMaterial; //시작지점 마우스 오버랩

	UMaterialInstance* mBaseMaterial; //기본상태

	UMaterialInstance* mPinkMaterial; //타겟지점 오버랩

	UMaterialInstance* mOrangeMaterial; //시작지점

	UMaterialInstance* mRedMaterial; //타겟지점

	UMaterialInstance* mPurpleMaterial; //경로

	UMaterialInstance* mBlackMaterial; //벽

	UMaterialInstance* mSearchMaterial; //벽
};



