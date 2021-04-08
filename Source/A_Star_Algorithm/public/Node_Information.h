#pragma once

class Node_Info {

public:
	Node_Info()
		: mNodePosition(FVector2D(0, 0))
		, mParentNode(nullptr)
		, G(0)
		, bIsWall(false)
	{}
	Node_Info(FVector2D curPosition, Node_Info* parentPtr, int newG, FVector2D newH, bool IsWall = false)
		: mNodePosition(curPosition)
		, mParentNode(parentPtr)
		, G(newG)
		, bIsWall(IsWall)
	{
		SetCostH(newH);
		SetCostF();
	}


public://Setter

	//bool operator==(const Node_Info& ref) const
	//{
	//	if (ref.GetCostG() != GetCostG())
	//	{
	//		return false;
	//	}

	//	if (ref.GetCostF() != GetCostF())
	//	{
	//		return false;
	//	}

	//	if (ref.m_NodePosition.X != mNodePosition.X)
	//	{
	//		return false;
	//	}

	//	if (ref.m_NodePosition.Y != mNodePosition.Y)
	//	{
	//		return false;
	//	}

	//	if (m_CurBlock != ref.m_CurBlock)
	//	{
	//		return false;
	//	}
	//	return true;
	//}
	void SetPosition(const int& x, const int& y) {
		mNodePosition.X = x;
		mNodePosition.Y = y;
	}
	void SetCurBlock(class AA_Star_AlgorithmBlock* p_CurBlock) { m_CurBlock = p_CurBlock; }
	void SetParentNode(Node_Info*  p_ParentNode) { mParentNode = p_ParentNode; }
	void SetWall(const int& _isWall) { bIsWall = _isWall; }


	void SetCostG(const int& _G) { G = _G; }
	void SetCostF() { F = G + H; }
	void SetCostH(const FVector2D& Target)
	{
		H = (abs(Target.X - mNodePosition.X) + abs(Target.Y - mNodePosition.Y)) * 10;
	}

public://Getter
	int GetCostG() const { return G; } //이동했던 거리
	int GetCostH() const { return H; }  //목표까지의 거리
	int GetCostF() const { return G + H; }

	FVector2D GetPoistion() const { return mNodePosition; }
	int GetX() const { return mNodePosition.X; }
	int GetY() const { return mNodePosition.Y; }
	bool GetIsWall() const { return bIsWall; }

	Node_Info*  GetParent() const { return mParentNode; }

	class AA_Star_AlgorithmBlock * GetCurBlock() const { return  m_CurBlock; }


	
private:
	class AA_Star_AlgorithmBlock * m_CurBlock;
	FVector2D mNodePosition;
	Node_Info* mParentNode;

	int	H;//목표노드까지의 예상 거리(휴리스틱 : 추정 값)
	int G;// 시작부터 지금까지 이동한 노드의 합
	int F;//H+G;



	bool bIsWall;
};
