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
	BlockSpacing = 300.f; // 사이 간격
	
	//초기화
	bAllowDiagonal = false;
	bdontCrossCorner = false;
	MovingTime = 0.1f;
	Cur_PathCountNum=0;
	//Set Null
	Cur_StartBlock=nullptr;
	Cur_TargetBlock=nullptr;

	//길찾기용 방향
	DirectionInfo[0] = FVector2D(1, 0);	//up
	DirectionInfo[1] = FVector2D(0, 1); //right
	DirectionInfo[2] = FVector2D(-1, 0); //down 
	DirectionInfo[3] = FVector2D(0, -1); //left


}//end of AA_Star_AlgorithmBlockGrid();


void AA_Star_AlgorithmBlockGrid::BeginPlay(){
	Super::BeginPlay();
	//블록 생성
	SpawnBlocks();	

}//end of BeginPlay()


//예외 상황 검사-여기서 false로 리턴이라면 경로에 적합하지 않다는뜻
bool AA_Star_AlgorithmBlockGrid::CheckException(FVector2D p_CheckPosition){
	const int X=p_CheckPosition.X;
	const int Y=p_CheckPosition.Y;

	//X_정해진 범위 이내 ( 0이하거나 GridSize 이상)
	if (X < 0 || X >= GridSize)
		return false;
	
	//Y_정해진 범위 이내 ( 0이하거나 GridSize 이상)
	if (Y < 0 || Y >= GridSize) 
		return false;

	//이미 CloseList에 포함되어있다면 리턴
	if ((ClosedList.Contains(&NodeArray[X][Y])))
		return false;

	//벽이라면 리턴
	if (NodeArray[X][Y].GetIsWall()) 
		return false;	

	return true;
}//end of CheckException

//시작 블록지정
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


	//길 재귀 찾기 시작 //길 재귀 찾기 함수 탈락 -> 비효율적임	
	//CharacterPath=GetPath_Recursive(StartBlock->GetBlockNumber(), TargetBlock->GetBlockNumber());	

	//재귀 찾기대신 사용하는 우선순위 큐 사용 
	//길 while 찾기 시작하여 CharacterPath에 경로 저장->재귀 함수대신 채택
	CharacterPath=GetPath_While(Cur_StartBlock->GetBlockNumber(), Cur_TargetBlock->GetBlockNumber());

	//경로를 찾지 못해서 실패 했을때 (0이하일떄)
	if(CharacterPath.Num()<=0){
		UE_LOG(LogTemp, Warning, TEXT("Not Found Path"));
		return;
	}
		
	//타이머 초기화 후 Draw진행
	Cur_PathCountNum= 1; //1부터 시작하는 이유는 시작지점을 제외한 1번 길부터 출력되기 위함. 
	GetWorldTimerManager().SetTimer(CountdownTimerHandle,this,&AA_Star_AlgorithmBlockGrid::MovingTimer, MovingTime,true);
	
}//end of SelectTargetBlock
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//현재 위치가 목표지점인지?
bool AA_Star_AlgorithmBlockGrid::GetArriveTarget(FVector2D p_CurrentPoistion, FVector2D p_TargetPosition){	
	return (p_CurrentPoistion.X == p_TargetPosition.X) && (p_CurrentPoistion.Y == p_TargetPosition.Y) ? true : false;
}

//길찾기 재귀 기능-탈락
TArray<FVector2D> AA_Star_AlgorithmBlockGrid::GetPath_Recursive(FVector2D p_StartPosition, FVector2D p_TargetPosition){
	//리스트 초기화
	this->SetRelease();

	//예외상황 확인
	if (!CheckException(p_TargetPosition)) 	return FinalPathList;

	//정상적인 조건에서 탐색 시작
	//처음 시작이므로 첫 시작지점이 부모 노드가 됌.
	Node_Info * First_ParentNode = new Node_Info(p_StartPosition,nullptr,0,p_TargetPosition);

	/*첫번째 선택 노드 ClosedList에 삽입*/
	ClosedList.Push(First_ParentNode);	

	/*길찾기 시작*/	
	Node_Info * Finded_Path=Recursive_FindPath(First_ParentNode,p_TargetPosition);


	/*최종 리스트에 찾은 경로대로 넣어줌*/
	while (Finded_Path->GetParent()!=nullptr){
		FinalPathList.Push(Finded_Path->GetPoistion());	 //최종경로에 찾은 경로 위치 넣기
		Finded_Path= Finded_Path->GetParent();
	}

	/*최종 리스트 역으로 뒤집어줌*/
	ReverseArray();

	/*최종 경로 리턴*/
	return FinalPathList;

}//end of GetPath

//(재귀용)현재 넣으려는 노드가 오픈리스트에 있는지 검사-탈락
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

//재귀 - 탈락
Node_Info * AA_Star_AlgorithmBlockGrid::Recursive_FindPath(Node_Info* p_Parent, FVector2D p_Target) {

	/*부모 노드 도착여부 확인*/
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
	//리스트 초기화
	this->SetRelease();

	//오픈리스트에 시작점 노드 넣어줌/ 부모 null/비용 0 
	OpenList.Push(new Node_Info(p_Startposition, nullptr, 0, p_TargetPosition));

 	while (OpenList.Num()>0){
		CurrentNode=OpenList[0]; //오픈리스트의 첫번째 있는걸 현재 노드로 대입

		//*F=>시작부터 지금까지 거리+목표까지의 거리 절대값
		//열린 리스트 중 가장 F가 작거나 같고, H가 작은 걸 
		 //현재노드로 하고 열린리스트에서 닫힌리스트로 옮기기
		int maxNumber=OpenList.Num();
		for (int i = 0; i < maxNumber; i++) {
			if (OpenList[i]->GetCostF() <= CurrentNode->GetCostF() &&
																	OpenList[i]->GetCostH() < CurrentNode->GetCostH()) {
				CurrentNode=OpenList[i];
				CurrentNode->SetCurBlock(OpenList[i]->GetCurBlock());
			}
		}//end of for

		//현재 노드 오픈리스트에서 지우고
		OpenList.Remove(CurrentNode);
		//닫힌 노드에 삽입
		ClosedList.Push(CurrentNode);


		//현재 노드 위치가 도착 위치랑 같은지? 같다면 return 종료
		if (GetArriveTarget(CurrentNode->GetPoistion(), p_TargetPosition)){
			const int x=p_TargetPosition.X;
			const int y=p_TargetPosition.Y;
			Node_Info *  TargetCurrent_node = &NodeArray[x][y]; //목표지점의 노드 정보 
			while (TargetCurrent_node->GetPoistion() != p_Startposition){
				FinalPathList.Push(TargetCurrent_node->GetPoistion()); //최종경로에 목표지점부터 push함 (추후에 역으로 뒤집음)
				TargetCurrent_node = TargetCurrent_node->GetParent(); //해당 노드의 부모노드를 가져옴( 다음 갈 길을 가져옴)
			}

			FinalPathList.Push(p_Startposition);

			//배열 역으로 뒤집기 보토 List였다면 간단하게 했을텐데, TArray사용으로 인해 따로 만들어줌
			ReverseArray();

			return FinalPathList;
		}

		//대각선방향 ↗↘↙↖
		if (bAllowDiagonal) {
			OpenListAdd(CurrentNode->GetPositionX() + 1, CurrentNode->GetPositionY() + 1);
			OpenListAdd(CurrentNode->GetPositionX() - 1, CurrentNode->GetPositionY() + 1);
			OpenListAdd(CurrentNode->GetPositionX() - 1, CurrentNode->GetPositionY() - 1);
			OpenListAdd(CurrentNode->GetPositionX() + 1, CurrentNode->GetPositionY() - 1);
		}


		//↑→↓← 순서 진행
		for (int i = 0; i < 4; i++) {
			int x= CurrentNode->GetPositionX() + DirectionInfo[i].X;
			int y = CurrentNode->GetPositionY() + DirectionInfo[i].Y;
			OpenListAdd(x,y);
		}

		
	}//end of while

	//최종 경로 리턴
	return FinalPathList;
}

void AA_Star_AlgorithmBlockGrid::OpenListAdd(int p_CheckX, int p_CheckY){
	if (!CheckException(FVector2D(p_CheckX, p_CheckY))) return;
	
	//해당 노드의 벽 여부 확인
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


	//체크하는 블록의 노드 정보 가져옴
	Node_Info * NeighborNode=&NodeArray[p_CheckX][p_CheckY]; 


	//10 or 14
	int movecost=CurrentNode->GetCostG() + (CurrentNode->GetPositionX()- p_CheckX ==0 || CurrentNode->GetPositionY()- p_CheckY == 0 ? 10 : 14);

	//해당 이웃노드가 오픈리스트에 포함되어 있지 않거나,
	//현재노드의 이동비용이 이웃노드 이동비용보다 저렴하다면
	if (movecost < NeighborNode->GetCostG() || !OpenList.Contains(NeighborNode)) {
		NeighborNode->SetCostG(movecost);  //이동비용 설정
		NeighborNode->SetCostH(Cur_TargetBlock->GetBlockNumber());//목표까지의 추정값 설정
		NeighborNode->SetParentNode(CurrentNode); //부모노드 설정
		if (NeighborNode->GetParent() != CurrentNode) {
			return;
		}
		OpenList.Push(NeighborNode); //오픈리스트에 추가
	}
}

/*초기화 담당*/
void AA_Star_AlgorithmBlockGrid::SetRelease(){	

	if (OpenList.Num()) 
		OpenList.Empty();			

	if (ClosedList.Num()) 
		ClosedList.Empty();	

	if (FinalPathList.Num()) 
		FinalPathList.Empty();	
}//end of SetRelase


/*최종 리스트(FinalPositionList) 뒤집기*/
void AA_Star_AlgorithmBlockGrid::ReverseArray(){
	FVector2D temp;
	const int max= FinalPathList.Num();
	for (int i = 0; i < max / 2; i++) {
		temp=FinalPathList[i];
		FinalPathList[i]= FinalPathList[max -i-1];
		FinalPathList[max - i - 1]=temp;
	}
}

/*벽 블록 지정 토글*/
void AA_Star_AlgorithmBlockGrid::SetWalllBlock(FVector2D p_Position,bool p_IsWall){
	const int x=p_Position.X;
	const int y=p_Position.Y;

	NodeArray[x][y].SetWall(p_IsWall);

}//end of SetWallBlock


void AA_Star_AlgorithmBlockGrid::AllClearBlock(){
	/*경로로 지정 되어 있는 블록 초기화*/
	const int size= Current_PathBlocks.Num();
	for (int i = 0; i < size; i++) {
		Current_PathBlocks[i]->SetPathBlock();
	}
	Current_PathBlocks.Empty();

	if (Cur_StartBlock != nullptr) { //시작 블록 초기화
		Cur_StartBlock->SetBasicMaterial();
		Cur_StartBlock->SetIsClicked(false);
		Cur_StartBlock = nullptr;
	}

	if (Cur_TargetBlock != nullptr) { //타겟 블록 초기화
		Cur_TargetBlock->SetBasicMaterial();
		Cur_TargetBlock->SetIsClicked(false);
		Cur_TargetBlock = nullptr;
	}
	
}


void AA_Star_AlgorithmBlockGrid::DrawPath(FVector2D p_DrawPosition,int p_CurBlockNumber){
	const int x = p_DrawPosition.X;
	const int y = p_DrawPosition.Y;

	//해당 블록을 경로 블록으로 지정
	NodeArray[x][y].GetCurBlock()->SetPathBlock();

	//숫자표시
	NodeArray[x][y].GetCurBlock()->SetTextNumber(FText::FromString(FString::FromInt(p_CurBlockNumber)));

	//길블록 배열 저장-추후 지우기 위함
	Current_PathBlocks.Push(NodeArray[x][y].GetCurBlock());
}


void AA_Star_AlgorithmBlockGrid::SpawnBlocks(){
	// 블록의 총 갯수
	const int32 NumBlocks = GridSize * GridSize;

	// 블록 생성 반복
	for (int32 BlockIndex = 0; BlockIndex < NumBlocks; BlockIndex++) {

		const float XOffset = (BlockIndex / GridSize) * BlockSpacing; // Divide by dimension
		const float YOffset = (BlockIndex%GridSize) * BlockSpacing; // Modulo gives remainder

		// Make position vector, offset from Grid location
		const FVector BlockLocation = FVector(XOffset, YOffset, 0.f) + GetActorLocation();

		// Spawn a block
		AA_Star_AlgorithmBlock* NewBlock = GetWorld()->SpawnActor<AA_Star_AlgorithmBlock>(BlockLocation, FRotator(0, 0, 0));


		// Tell the block about its owner
		if (NewBlock != nullptr) {

			//생성 된 블록에 주인 Grid 지정
			NewBlock->SetOwingGrid(this);

			const int X_Value = XOffset / BlockSpacing;
			const int Y_Value = YOffset / BlockSpacing;

			//추가 각 블록당 숫자 부여해줌
			NewBlock->SetBlockNumber(FVector2D(X_Value, Y_Value));

			//전체 노드배열에 각각 블록 지정
			NodeArray[X_Value][Y_Value].SetCurBlock(NewBlock);

			NodeArray[X_Value][Y_Value].SetPosition(X_Value, Y_Value);
		}//end of if
	}//end of for
}

void AA_Star_AlgorithmBlockGrid::MovingTimer(){
	if (CharacterPath.Num() <= Cur_PathCountNum) {
		GetWorldTimerManager().ClearTimer(CountdownTimerHandle); //타이머 정지
		return;
	}
	else {
		UE_LOG(LogTemp, Log, TEXT("%d"), Cur_PathCountNum);
		const FVector2D Cur_DrawPosition = this->CharacterPath[Cur_PathCountNum];

		//경로 블록 그리기(위치,번호)
		DrawPath(Cur_DrawPosition, Cur_PathCountNum);
	}
	Cur_PathCountNum++;
}

#undef LOCTEXT_NAMESPACE
