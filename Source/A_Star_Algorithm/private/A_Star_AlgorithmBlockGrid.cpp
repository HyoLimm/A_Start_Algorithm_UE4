// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#include "A_Star_AlgorithmBlockGrid.h"
#include "A_Star_AlgorithmBlock.h"
#include "Components/TextRenderComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/World.h"

#define LOCTEXT_NAMESPACE "PuzzleBlockGrid"

AA_Star_AlgorithmBlockGrid::AA_Star_AlgorithmBlockGrid(){
	// Create dummy root scene component
	RootComp = CreateDefaultSubobject<USceneComponent>(TEXT("Dummy0"));
	RootComponent = RootComp;

	// Set defaults
	GridSize = 20; 
	BlockSpacing = 300.f; // ���� ����
	
	//�ʱ�ȭ
	bAllowDiagonal = false;
	bdontCrossCorner = false;
	MovingTime = 0.1f;
	Cur_PathCountNum=0;
	//Set Null
	Cur_StartBlock=nullptr;
	Cur_TargetBlock=nullptr;

	//��ã��� ����
	DirectionInfo[0] = FVector2D(1, 0);	//up
	DirectionInfo[1] = FVector2D(0, 1); //right
	DirectionInfo[2] = FVector2D(-1, 0); //down 
	DirectionInfo[3] = FVector2D(0, -1); //left


}//end of AA_Star_AlgorithmBlockGrid();


void AA_Star_AlgorithmBlockGrid::BeginPlay(){
	Super::BeginPlay();
	//��� ����
	SpawnBlocks();	

}//end of BeginPlay()


//���� ��Ȳ �˻�-���⼭ false�� �����̶�� ��ο� �������� �ʴٴ¶�
bool AA_Star_AlgorithmBlockGrid::CheckException(FVector2D p_CheckPosition){
	const int X=p_CheckPosition.X;
	const int Y=p_CheckPosition.Y;

	//X_������ ���� �̳� ( 0���ϰų� GridSize �̻�)
	if (X < 0 || X >= GridSize)
		return false;
	
	//Y_������ ���� �̳� ( 0���ϰų� GridSize �̻�)
	if (Y < 0 || Y >= GridSize) 
		return false;

	//�̹� CloseList�� ���ԵǾ��ִٸ� ����
	if ((ClosedList.Contains(&NodeArray[X][Y])))
		return false;

	//���̶�� ����
	if (NodeArray[X][Y].GetIsWall()) 
		return false;	

	return true;
}//end of CheckException

//���� �������
void AA_Star_AlgorithmBlockGrid::SelectStartBlock(class AA_Star_AlgorithmBlock * p_StartingBlock){
	//Check StartBlock
	if(Cur_StartBlock!=nullptr) return;

	Cur_StartBlock=p_StartingBlock;
}

void AA_Star_AlgorithmBlockGrid::SelectTargetBlock(class AA_Star_AlgorithmBlock * p_TargetingBlock){
	//Check TargetBlock
	if (Cur_TargetBlock != nullptr) return;

	Cur_TargetBlock=p_TargetingBlock;
	

	if (Cur_StartBlock == nullptr || Cur_TargetBlock == nullptr)	return;


	//�� ��� ã�� ���� //�� ��� ã�� �Լ� Ż�� -> ��ȿ������	
	//CharacterPath=GetPath_Recursive(StartBlock->GetBlockNumber(), TargetBlock->GetBlockNumber());	

	//��� ã���� ����ϴ� �켱���� ť ��� 
	//�� while ã�� �����Ͽ� CharacterPath�� ��� ����->��� �Լ���� ä��
	CharacterPath=GetPath_While(Cur_StartBlock->GetBlockNumber(), Cur_TargetBlock->GetBlockNumber());

	//��θ� ã�� ���ؼ� ���� ������ (0�����ϋ�)
	if(CharacterPath.Num()<=0){
		UE_LOG(LogTemp, Warning, TEXT("Not Found Path"));
		return;
	}
		
	//Ÿ�̸� �ʱ�ȭ �� Draw����
	Cur_PathCountNum= 1; //1���� �����ϴ� ������ ���������� ������ 1�� ����� ��µǱ� ����. 
	GetWorldTimerManager().SetTimer(CountdownTimerHandle,this,&AA_Star_AlgorithmBlockGrid::MovingTimer, MovingTime,true);
	
}//end of SelectTargetBlock
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//���� ��ġ�� ��ǥ��������?
bool AA_Star_AlgorithmBlockGrid::GetArriveTarget(FVector2D p_CurrentPoistion, FVector2D p_TargetPosition){	
	return (p_CurrentPoistion.X == p_TargetPosition.X) && (p_CurrentPoistion.Y == p_TargetPosition.Y) ? true : false;
}

//��ã�� ��� ���-Ż��
TArray<FVector2D> AA_Star_AlgorithmBlockGrid::GetPath_Recursive(FVector2D p_StartPosition, FVector2D p_TargetPosition){
	//����Ʈ �ʱ�ȭ
	this->SetRelease();

	//���ܻ�Ȳ Ȯ��
	if (!CheckException(p_TargetPosition)) 	return FinalPathList;

	//�������� ���ǿ��� Ž�� ����
	//ó�� �����̹Ƿ� ù ���������� �θ� ��尡 ��.
	Node_Info * First_ParentNode = new Node_Info(p_StartPosition,nullptr,0,p_TargetPosition);

	/*ù��° ���� ��� ClosedList�� ����*/
	ClosedList.Push(First_ParentNode);	

	/*��ã�� ����*/	
	Node_Info * Finded_Path=Recursive_FindPath(First_ParentNode,p_TargetPosition);


	/*���� ����Ʈ�� ã�� ��δ�� �־���*/
	while (Finded_Path->GetParent()!=nullptr){
		FinalPathList.Push(Finded_Path->GetPoistion());	 //������ο� ã�� ��� ��ġ �ֱ�
		Finded_Path= Finded_Path->GetParent();
	}

	/*���� ����Ʈ ������ ��������*/
	ReverseArray();

	/*���� ��� ����*/
	return FinalPathList;

}//end of GetPath

//(��Ϳ�)���� �������� ��尡 ���¸���Ʈ�� �ִ��� �˻�-Ż��
void AA_Star_AlgorithmBlockGrid::AddOpenList(Node_Info* p_CheckNode) {
	for (auto it : ClosedList) {
		if (GetArriveTarget(it->GetPoistion(), p_CheckNode->GetPoistion())) {
			return;
		}
	}
	for (auto it : OpenList) {
		if (GetArriveTarget(it->GetPoistion(), p_CheckNode->GetPoistion())) {
			if (it->GetCostF() > p_CheckNode->GetCostF()) {
				OpenList.Remove(it);
				OpenList.Push(p_CheckNode);
				return;
			}
		}
	}

	OpenList.Push(p_CheckNode);
}//end of AddOpenList

//��� - Ż��
Node_Info * AA_Star_AlgorithmBlockGrid::Recursive_FindPath(Node_Info* p_Parent, FVector2D p_Target) {

	/*�θ� ��� �������� Ȯ��*/
	if (GetArriveTarget(p_Parent->GetPoistion(), p_Target)) return p_Parent;

	for (int i = 0; i < 4; i++) {
		FVector2D _ChildPosition = p_Parent->GetPoistion() + DirectionInfo[i];
		if (CheckException(_ChildPosition)) {
			Node_Info * _Child = new Node_Info(_ChildPosition, p_Parent, p_Parent->GetCostG() + 1, p_Target);
			AddOpenList(_Child);
		}
	}


	if (OpenList[0] == nullptr) {
		UE_LOG(LogTemp, Warning, TEXT("NotValid_Recursive_Path"));
		return p_Parent;
	}
	Node_Info * BestNode = OpenList[0];

	for (auto it : OpenList) {
		if (BestNode->GetCostF() >= it->GetCostF()) {
			BestNode = it;
		}
	}

	OpenList.Remove(BestNode);

	ClosedList.Push(BestNode);

	return Recursive_FindPath(BestNode, p_Target);
}

TArray<FVector2D> AA_Star_AlgorithmBlockGrid::GetPath_While(FVector2D p_Startposition, FVector2D p_TargetPosition){
	//����Ʈ �ʱ�ȭ
	this->SetRelease();

	//���¸���Ʈ�� ������ ��� �־���/ �θ� null/��� 0 
	OpenList.Push(new Node_Info(p_Startposition, nullptr, 0, p_TargetPosition));

 	while (OpenList.Num()>0){
		CurrentNode=OpenList[0]; //���¸���Ʈ�� ù��° �ִ°� ���� ���� ����

		//*F=>���ۺ��� ���ݱ��� �Ÿ�+��ǥ������ �Ÿ� ���밪
		//���� ����Ʈ �� ���� F�� �۰ų� ����, H�� ���� �� 
		 //������� �ϰ� ��������Ʈ���� ��������Ʈ�� �ű��
		int maxNumber=OpenList.Num();
		for (int i = 0; i < maxNumber; i++) {
			if (OpenList[i]->GetCostF() <= CurrentNode->GetCostF() &&
																	OpenList[i]->GetCostH() < CurrentNode->GetCostH()) {
				CurrentNode=OpenList[i];
				CurrentNode->SetCurBlock(OpenList[i]->GetCurBlock());
			}
		}//end of for

		//���� ��� ���¸���Ʈ���� �����
		OpenList.Remove(CurrentNode);
		//���� ��忡 ����
		ClosedList.Push(CurrentNode);


		//���� ��� ��ġ�� ���� ��ġ�� ������? ���ٸ� return ����
		if (GetArriveTarget(CurrentNode->GetPoistion(), p_TargetPosition)){
			const int x=p_TargetPosition.X;
			const int y=p_TargetPosition.Y;
			Node_Info *  TargetCurrent_node = &NodeArray[x][y]; //��ǥ������ ��� ���� 
			while (TargetCurrent_node->GetPoistion() != p_Startposition){
				FinalPathList.Push(TargetCurrent_node->GetPoistion()); //������ο� ��ǥ�������� push�� (���Ŀ� ������ ������)
				TargetCurrent_node = TargetCurrent_node->GetParent(); //�ش� ����� �θ��带 ������( ���� �� ���� ������)
			}

			FinalPathList.Push(p_Startposition);

			//�迭 ������ ������ ���� List���ٸ� �����ϰ� �����ٵ�, TArray������� ���� ���� �������
			ReverseArray();

			return FinalPathList;
		}

		//�밢������ �֢٢ע�
		if (bAllowDiagonal) {
			OpenListAdd(CurrentNode->GetPositionX() + 1, CurrentNode->GetPositionY() + 1);
			OpenListAdd(CurrentNode->GetPositionX() - 1, CurrentNode->GetPositionY() + 1);
			OpenListAdd(CurrentNode->GetPositionX() - 1, CurrentNode->GetPositionY() - 1);
			OpenListAdd(CurrentNode->GetPositionX() + 1, CurrentNode->GetPositionY() - 1);
		}


		//����� ���� ����
		for (int i = 0; i < 4; i++) {
			int x= CurrentNode->GetPositionX() + DirectionInfo[i].X;
			int y = CurrentNode->GetPositionY() + DirectionInfo[i].Y;
			OpenListAdd(x,y);
		}

		
	}//end of while

	//���� ��� ����
	return FinalPathList;
}

void AA_Star_AlgorithmBlockGrid::OpenListAdd(int p_CheckX, int p_CheckY){
	if (!CheckException(FVector2D(p_CheckX, p_CheckY))) return;
	
	//�ش� ����� �� ���� Ȯ��
	if (bAllowDiagonal) {
		if (NodeArray[CurrentNode->GetPositionX()][p_CheckY].GetIsWall() &&
			NodeArray[p_CheckX][CurrentNode->GetPositionY()].GetIsWall()) {
			return;
		}
	}

	if (bdontCrossCorner) {
		if (NodeArray[CurrentNode->GetPositionX()][p_CheckY].GetIsWall() ||
			NodeArray[p_CheckX][CurrentNode->GetPositionY()].GetIsWall()) {
			return;
		}
	}


	//üũ�ϴ� ����� ��� ���� ������
	Node_Info * NeighborNode=&NodeArray[p_CheckX][p_CheckY]; 


	//10 or 14
	int movecost=CurrentNode->GetCostG() + (CurrentNode->GetPositionX()- p_CheckX ==0 || CurrentNode->GetPositionY()- p_CheckY == 0 ? 10 : 14);

	//�ش� �̿���尡 ���¸���Ʈ�� ���ԵǾ� ���� �ʰų�,
	//�������� �̵������ �̿���� �̵���뺸�� �����ϴٸ�
	if (movecost < NeighborNode->GetCostG() || !OpenList.Contains(NeighborNode)) {
		NeighborNode->SetCostG(movecost);  //�̵���� ����
		NeighborNode->SetCostH(Cur_TargetBlock->GetBlockNumber());//��ǥ������ ������ ����
		NeighborNode->SetParentNode(CurrentNode); //�θ��� ����
		if (NeighborNode->GetParent() != CurrentNode) {
			return;
		}
		OpenList.Push(NeighborNode); //���¸���Ʈ�� �߰�
	}
}

/*�ʱ�ȭ ���*/
void AA_Star_AlgorithmBlockGrid::SetRelease(){	

	if (OpenList.Num()) 
		OpenList.Empty();			

	if (ClosedList.Num()) 
		ClosedList.Empty();	

	if (FinalPathList.Num()) 
		FinalPathList.Empty();	
}//end of SetRelase


/*���� ����Ʈ(FinalPositionList) ������*/
void AA_Star_AlgorithmBlockGrid::ReverseArray(){
	FVector2D temp;
	const int max= FinalPathList.Num();
	for (int i = 0; i < max / 2; i++) {
		temp=FinalPathList[i];
		FinalPathList[i]= FinalPathList[max -i-1];
		FinalPathList[max - i - 1]=temp;
	}
}

/*�� ��� ���� ���*/
void AA_Star_AlgorithmBlockGrid::SetWalllBlock(FVector2D p_Position,bool p_IsWall){
	const int x=p_Position.X;
	const int y=p_Position.Y;

	NodeArray[x][y].SetWall(p_IsWall);

}//end of SetWallBlock


void AA_Star_AlgorithmBlockGrid::AllClearBlock(){
	/*��η� ���� �Ǿ� �ִ� ��� �ʱ�ȭ*/
	const int size= Current_PathBlocks.Num();
	for (int i = 0; i < size; i++) {
		Current_PathBlocks[i]->SetPathBlock();
	}
	Current_PathBlocks.Empty();

	if (Cur_StartBlock != nullptr) { //���� ��� �ʱ�ȭ
		Cur_StartBlock->SetBasicMaterial();
		Cur_StartBlock->SetIsClicked(false);
		Cur_StartBlock = nullptr;
	}

	if (Cur_TargetBlock != nullptr) { //Ÿ�� ��� �ʱ�ȭ
		Cur_TargetBlock->SetBasicMaterial();
		Cur_TargetBlock->SetIsClicked(false);
		Cur_TargetBlock = nullptr;
	}
	
}


void AA_Star_AlgorithmBlockGrid::DrawPath(FVector2D p_DrawPosition,int p_CurBlockNumber){
	const int x = p_DrawPosition.X;
	const int y = p_DrawPosition.Y;

	//�ش� ����� ��� ������� ����
	NodeArray[x][y].GetCurBlock()->SetPathBlock();

	//����ǥ��
	NodeArray[x][y].GetCurBlock()->SetTextNumber(FText::FromString(FString::FromInt(p_CurBlockNumber)));

	//���� �迭 ����-���� ����� ����
	Current_PathBlocks.Push(NodeArray[x][y].GetCurBlock());
}


void AA_Star_AlgorithmBlockGrid::SpawnBlocks(){
	// ����� �� ����
	const int32 NumBlocks = GridSize * GridSize;

	// ��� ���� �ݺ�
	for (int32 BlockIndex = 0; BlockIndex < NumBlocks; BlockIndex++) {

		const float XOffset = (BlockIndex / GridSize) * BlockSpacing; // Divide by dimension
		const float YOffset = (BlockIndex%GridSize) * BlockSpacing; // Modulo gives remainder

		// Make position vector, offset from Grid location
		const FVector BlockLocation = FVector(XOffset, YOffset, 0.f) + GetActorLocation();

		// Spawn a block
		AA_Star_AlgorithmBlock* NewBlock = GetWorld()->SpawnActor<AA_Star_AlgorithmBlock>(BlockLocation, FRotator(0, 0, 0));


		// Tell the block about its owner
		if (NewBlock != nullptr) {

			//���� �� ��Ͽ� ���� Grid ����
			NewBlock->SetOwingGrid(this);

			const int X_Value = XOffset / BlockSpacing;
			const int Y_Value = YOffset / BlockSpacing;

			//�߰� �� ��ϴ� ���� �ο�����
			NewBlock->SetBlockNumber(FVector2D(X_Value, Y_Value));

			//��ü ���迭�� ���� ��� ����
			NodeArray[X_Value][Y_Value].SetCurBlock(NewBlock);

			NodeArray[X_Value][Y_Value].SetPosition(X_Value, Y_Value);
		}//end of if
	}//end of for
}

void AA_Star_AlgorithmBlockGrid::MovingTimer(){
	if (CharacterPath.Num() <= Cur_PathCountNum) {
		GetWorldTimerManager().ClearTimer(CountdownTimerHandle); //Ÿ�̸� ����
		return;
	}
	else {
		UE_LOG(LogTemp, Log, TEXT("%d"), Cur_PathCountNum);
		const FVector2D Cur_DrawPosition = this->CharacterPath[Cur_PathCountNum];

		//��� ��� �׸���(��ġ,��ȣ)
		DrawPath(Cur_DrawPosition, Cur_PathCountNum);
	}
	Cur_PathCountNum++;
}

#undef LOCTEXT_NAMESPACE
