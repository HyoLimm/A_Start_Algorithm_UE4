#pragma once

class Node_Info {

public:
	Node_Info()
		: m_NodePosition(FVector2D(0, 0)), m_ParentNode(nullptr), G(0), m_bIsWall(false)
	{}
	Node_Info(FVector2D _position, Node_Info* _Parent, int _g, FVector2D _H, bool _IsWall = false)
		: m_NodePosition(_position), m_ParentNode(_Parent), G(_g), m_bIsWall(_IsWall)
	{
		SetCostH(_H);
		SetCostF();
	}


public://Setter

	bool operator==(const Node_Info& ref) const
	{
		if (ref.GetCostG() != GetCostG())
		{
			return false;
		}

		if (ref.GetCostF() != GetCostF())
		{
			return false;
		}

		if (ref.m_NodePosition.X != m_NodePosition.X)
		{
			return false;
		}

		if (ref.m_NodePosition.Y != m_NodePosition.Y)
		{
			return false;
		}

		if (m_CurBlock != ref.m_CurBlock)
		{
			return false;
		}
		return true;
	}
	void SetPosition(const int x, const int y) {
		m_NodePosition.X = x;
		m_NodePosition.Y = y;
	}
	void SetCurBlock(class AA_Star_AlgorithmBlock * p_CurBlock) { m_CurBlock = p_CurBlock; }
	void SetParentNode(Node_Info*  p_ParentNode) { m_ParentNode = p_ParentNode; }
	void SetWall(const int _isWall) { m_bIsWall = _isWall; }


	void SetCostG(const int _G) { G = _G; }
	void SetCostF() { F = G + H; }
	void SetCostH(const FVector2D Target)
	{
		H = (abs(Target.X - m_NodePosition.X) + abs(Target.Y - m_NodePosition.Y)) * 10;
	}

public://Getter
	int GetCostG() const { return G; } //이동했던 거리
	int GetCostH() const { return H; }  //목표까지의 거리
	int GetCostF() const { return G + H; }

	FVector2D GetPoistion() const { return m_NodePosition; }
	int GetX() const { return m_NodePosition.X; }
	int GetY() const { return m_NodePosition.Y; }
	bool GetIsWall() const { return m_bIsWall; }

	Node_Info*  GetParent() const { return m_ParentNode; }

	class AA_Star_AlgorithmBlock * GetCurBlock() const { return  m_CurBlock; }


	
private:
	class AA_Star_AlgorithmBlock * m_CurBlock;
	FVector2D m_NodePosition;
	Node_Info* m_ParentNode;

	int	H;//목표노드까지의 예상 거리(휴리스틱 : 추정 값)
	int G;// 시작부터 지금까지 이동한 노드의 합
	int F;//H+G;



	bool m_bIsWall;
};
