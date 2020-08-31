#pragma once

class Node_Info {

public:
	Node_Info()
		: m_NodePosition(FVector2D(0,0)), m_ParentNode(nullptr), G(0), m_bIsWall(false)
		{}
	Node_Info(FVector2D _position, Node_Info* _Parent, int _g, FVector2D _H,bool _IsWall=false)
		: m_NodePosition(_position), m_ParentNode(_Parent),G(_g),m_bIsWall(_IsWall)
	{		
		SetCostH(_H);		
		SetCostF();
	}


public://Setter
	void SetPosition(const int x, const int y) {
		m_NodePosition.X = x;
		m_NodePosition.Y = y;
	}
	void SetCurBlock(class AA_Star_AlgorithmBlock * p_CurBlock) {m_CurBlock = p_CurBlock;}
	void SetParentNode(Node_Info * p_ParentNode) { m_ParentNode = p_ParentNode; }
	void SetWall(const int _isWall) { m_bIsWall = _isWall; }


	void SetCostG(const int _G) { G = _G; }
	void SetCostF() { F = G + H; }
	void SetCostH(const FVector2D Target) {
		H = (abs(Target.X - m_NodePosition.X) + abs(Target.Y - m_NodePosition.Y)) * 10;
	}

public://Getter
	int GetCostG() const { return G; } //�̵��ߴ� �Ÿ�
	int GetCostH() const { return H; }  //��ǥ������ �Ÿ�
	int GetCostF() const { return G + H; }

	FVector2D GetPoistion() const { return m_NodePosition; }
	int GetPositionX() const { return m_NodePosition.X; }
	int GetPositionY() const { return m_NodePosition.Y; }
	bool GetIsWall() const {return m_bIsWall;}

	Node_Info * GetParent() const { return m_ParentNode; }

	class AA_Star_AlgorithmBlock * GetCurBlock() const{return  m_CurBlock;}



private:
	class AA_Star_AlgorithmBlock * m_CurBlock;
	FVector2D m_NodePosition;
	Node_Info* m_ParentNode;

	int	H;//��ǥ�������� ���� �Ÿ�(�޸���ƽ : ���� ��)
	int G;// ���ۺ��� ���ݱ��� �̵��� ����� ��
	int F;//H+G;



	bool m_bIsWall;
};