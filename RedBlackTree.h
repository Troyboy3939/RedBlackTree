#pragma once
#include "BinarySearchTree.h"

template<typename T>
class RBTNode : public Node<T>
{
public:
	RBTNode() : Node<T>()
	{
		m_bColour = true;
	}


	//  default to red
	RBTNode(T value, bool bColour = true, RBTNode* pLower = nullptr, RBTNode* pHigher = nullptr, RBTNode* pParent = nullptr) : Node<T>(value,pLower,pHigher,pParent)
	{
		m_bColour = bColour;
	}

	virtual ~RBTNode()
	{
		
	}
	bool& GetColour()
	{
		return m_bColour;
	}

	void SetColour(bool bColour)
	{
		m_bColour = bColour;
	}

	RBTNode* GetParent() override
	{
		return dynamic_cast<RBTNode*>(Node<T>::GetParent());
	}

	RBTNode* GetLower() override
	{
		return dynamic_cast<RBTNode*>(Node<T>::GetLower());
	}

	RBTNode* GetHigher() override
	{
		return dynamic_cast<RBTNode*>(Node<T>::GetHigher());
	}

private:
	//what colour the node is. false = black, true = red
	bool m_bColour;
};

template<typename T>
using BST = BinarySearchTree<T, RBTNode<T>>;

template<typename T, typename RbtNode = RBTNode<T>>
class RedBlackTree : public BinarySearchTree<T,RbtNode>
{

	//-----------------------------------------
	//False means that the colour is Black, vise verse
	//-----------------------------------------
public:


	//-------------------------------------
	// Initialises the root node of the tree, to pRootNode
	// COMPARISON FUNCTION HAS TO BE A FUNCTION/LAMBDA ||
	// true if left parameter greater than right
	//-------------------------------------

	RedBlackTree(Function<T> comparisonFunction, int nDefaultNodeCount = 5) : BST<T>(comparisonFunction ,nDefaultNodeCount)
	{
		
		m_pHighest = nullptr;
		m_pLowest = nullptr;

	}

	virtual ~RedBlackTree()
	{
		
	}



	//-------------------------------------
	//Inserts the nNumber onto the tree
	//-------------------------------------
	RbtNode* Add(T data)
	{
		Node<T>* pBSTNode = BST<T>::Add(data);

		auto pNode = static_cast<RbtNode*>(pBSTNode);

		

		//Add the number, and then repair the tree starting at the node created
		RepairTreeInsert(pNode);

		return pNode;
	}

	//-------------------------------------
	//Gets the root node
	//-------------------------------------
	RbtNode* GetRoot()
	{
		return (RbtNode*)this->m_pRoot;
	}

	//-------------------------------------
	//Removes nNumber from the tree
	//-------------------------------------
	void RemoveValue(T rValue)
	{
		RbtNode* pNode = FindNode(rValue);

		if (pNode)
		{
			if (pNode != this->m_pRoot)
			{

				RbtNode* pParent = pNode->GetParent();

				//if pNode is Red or they have exactly 1 red child 
				if (pNode->GetColour() || HasRedChild(pNode) && (!pNode->GetLower() || !pNode->GetHigher()))
				{
					RbtNode* pHigher = pNode->GetHigher();
					RbtNode* pLower = pNode->GetLower();

					//----------------------------------------------
					// Basic case removal and recolouring based on number of children
					//----------------------------------------------

					//if they are red and have 2 children
					if(pNode->GetColour() && pLower && pHigher)
					{
						//set to black
						pNode->SetColour(false);

						pLower->SetColour(true);
						pHigher->SetColour(true);

						this->RemoveNode(pNode);
					}
					else
					{
						RbtNode* pParent = pNode->GetParent();

						if(pParent->GetHigher() == pNode)
						{
							this->RemoveNode(pNode);

							if(pParent->GetHigher())
							{
								pParent->GetHigher()->SetColour(false);
							}
						}
						else
						{
							this->RemoveNode(pNode);

							if (pParent->GetLower())
							{
								pParent->GetLower()->SetColour(false);
							}
						}
					}
					

					return;
				}

				RepairTreeDelete(RemoveNode(pNode), pParent);
			}
			else
			{
				
				RbtNode* pSuccessorParent = this->InOrderSuccessor(this->m_pRoot);

				if (pSuccessorParent)
				{
					//Get parent
					pSuccessorParent = pSuccessorParent->GetParent();



					this->m_pRoot = this->RemoveNode(this->m_pRoot);

					//recolour to black
					static_cast<RbtNode*>(this->m_pRoot)->SetColour(false);


					//BST::MoveNode was called -> RemoveNode was called, therefore->

					TwoChildRepair(pSuccessorParent);

				}

			}
		}
		
	}




	//------------------------
	// FindNode but return RBTNodes instead
	//------------------------
	RbtNode* FindNode(T data) override
	{
		return static_cast<RbtNode*>(BST<T>::FindNode(data));
	}


	RbtNode* GetLowest()
	{
		return m_pLowest;
	}

	RbtNode* GetHighest()
	{
		return m_pHighest;
	}


protected:

	//------------------------
	// RemoveNode but returns RBTNodes instead
	//------------------------
	RbtNode* RemoveNode(RbtNode* pNode)
	{
		return static_cast<RbtNode*>(BST<T>::RemoveNode(pNode));
	}

	//------------------------
	// RemoveNode but returns RBTNodes instead, takes BSTNode
	//------------------------
	RbtNode* RemoveNode(Node<T>* pNode) 
	{
		return static_cast<RbtNode*>(BST<T>::RemoveNode(pNode));
	}

	void TwoChildRepair(RbtNode* pSuccessorParent)
	{
		RepairTreeDelete(nullptr, pSuccessorParent);

		RbtNode* pSib = this->GetSibling(pSuccessorParent);

		if (pSib)
		{
			//perform last recolour
			if (pSib->GetColour())
			{
				pSib->SetColour(false);
				pSuccessorParent->SetColour(false);
				pSuccessorParent->GetParent()->SetColour(true);
			}
		}
	}


	bool HasRedChild(RbtNode* pNode)
	{
		if (pNode)
		{

			RbtNode* pHigher = pNode->GetHigher();
			RbtNode* pLower = pNode->GetLower();

			if (pHigher && pHigher->GetColour())
			{
				return true;
			}
			else if (pLower && pLower->GetColour())
			{
				return true;
			}
		}

		return false;
	}

	static RbtNode* GetOnlyChild(RbtNode* pParent)
	{
		if (pParent)
		{
			if (pParent->GetLower())
			{
				return pParent->GetLower();
			}

			return pParent->GetHigher();
		}

		return nullptr;
	}

	//-------------------------------------
	// Fixes the Tree, starting at pNode after deletion
	//-------------------------------------
	void RepairTreeDelete(RbtNode* pNode, RbtNode* pParent)
	{
		//if the node is not the root node
		if (pNode != this->m_pRoot)
		{

			//if pNode is nullptr, or is black and has no red children 
			if (!pNode || (pNode && !pNode->GetColour() && !HasRedChild(pNode)))
			{
				RbtNode* pSibling = nullptr;

				if (pParent)
				{
					if (!pNode)
					{
						//if a leaf was removed, then we need to get the sibling node
						pSibling = GetOnlyChild(pParent);
					}
					else
					{
						//Otherwise, it exists, just get its sibling like this
						this->GetSibling(pNode);
					}

					if (pSibling)
					{

						//if sibling exists and has a red child
						if (HasRedChild(pSibling) && !pSibling->GetColour())
						{

							//RbtNode* pHigher = pSibling->GetHigher();
							RbtNode* pLower = pSibling->GetLower();

							if (pParent->GetLower() == pSibling)
							{

								if (pLower && pLower->GetColour())
								{
									//----------------
									// LEFT LEFT CASE
									//----------------

									RotateRight(pParent);
								}
								else
								{
									//----------------
									// LEFT RIGHT CASE
									//----------------

									RotateLeft(pSibling,true);
									RotateRight(pParent);
								}

							}
							else
							{

								if (pLower && pLower->GetColour())
								{
									//----------------
									// RIGHT LEFT CASE
									//----------------

									RotateRight(pSibling,true);
									RotateLeft(pParent);
								}
								else
								{
									//----------------
									// RIGHT RIGHT CASE
									//----------------

									RotateLeft(pParent);

								}

							}



						} //sibling is black, and children are black
						else if (!pSibling->GetColour())
						{
							//recolour to red,
							pSibling->SetColour(true);

							//if parent is red 
							if (pParent->GetColour())
							{
								//recolour to black
								pParent->SetColour(false);
							}
							else //parent is black
							{
								//recursion
								RepairTreeDelete(pParent, pParent->GetParent());
							}
						} 
						else if (pSibling->GetColour()) //if the sibling is red
						{



							
							if (pParent->GetHigher() == pSibling)
							{
								//-------------------
								// Right Case
								//-------------------

								//Rotate left, if sibling is red, then parent has to be black (no 2 reds in a row)
								//therefore we can simply swap colours 
								RotateLeft(pParent,true);

								//Recursion, 
								RepairTreeDelete(pParent,pSibling);
							}
							else
							{
								//-------------------
								// Left Case
								//-------------------

								//Rotate Right, if sibling is red, then parent has to be black (no 2 reds in a row)
								//therefore we can simply swap colours, instead of recolouring both
								RotateRight(pParent, true);

								//Recursion, 
								RepairTreeDelete(pParent, pSibling);

							}
						}
					}
				}
			}
		}
	}

	//-------------------------------------
	// Fixes the Tree, starting at pNode
	//-------------------------------------
	void RepairTreeInsert(RbtNode* pNode)
	{
		//setup relevant pointers  for use
		RbtNode* pParent = nullptr;
		RbtNode* pUncle = nullptr;
		RbtNode* pGramps = nullptr;


		//-----------------------------------------------
		// 2. If pNode is root, recolour
		//-----------------------------------------------
		//if the node passed in is the root node
		if (pNode == this->m_pRoot)
		{
			m_pLowest = this->m_pRoot;
			m_pHighest = this->m_pRoot;

			//root node is always black, so set it 
			pNode->SetColour(false);
		}
		else if(pNode) //else if the node passed in is not the root node
		{
			if (pNode->GetParent() == this->m_pRoot)
			{
				if (pNode->GetParent()->GetLower() == pNode && !pNode->GetLower())
				{
					m_pLowest = pNode;
				}
				else if (pNode->GetParent()->GetHigher() == pNode && !pNode->GetHigher())
				{
					m_pHighest = pNode;
				}

			}
			else if (m_pLowest && pNode->GetParent() == m_pLowest && m_pLowest->GetLower() == pNode)
			{
				m_pLowest = pNode;
			}
			else if (m_pHighest && pNode->GetParent() == m_pHighest && m_pHighest->GetHigher() == pNode)
			{
				m_pHighest = pNode;
			}


			//then get a pointer to the uncle
			pUncle = static_cast<RbtNode*>(this->GetUncle(pNode));

			//get the parent node
			pParent = static_cast<RbtNode*>( pNode->GetParent());

			//get the grandparent node
			pGramps = static_cast<RbtNode*>(pParent->GetParent());


			//---------------------
			// 3. If parent is red and not root
			//---------------------

			//if the parent exists, and the parents colour is red
			if (pParent && pParent->GetColour() && pNode != this->m_pRoot)
			{
				bool bUncleColour = false;

				if(pUncle)
				{
					bUncleColour = pUncle->GetColour();
				}

			
				

				//If uncle is red 3a
				if (bUncleColour)
				{
					//Set both parent and the uncle to black
					pParent->SetColour(false);
					pUncle->SetColour(false);

					if (pGramps)
					{
						//Set gramps colour to red
						pGramps->SetColour(true);
					}

					//recursively repair the tree
					RepairTreeInsert(pGramps);
				}    //-----------------------------------------
				else // 3b if uncle is black
				{    //-----------------------------------------
				
					//pParent is the lower child
					if (pParent == pGramps->GetLower())
					{
						//pNode is the lower child
						if (pNode == pParent->GetLower())
						{
							//Left left case


							//Right rotation, then colour swap
							RotateRight(pGramps, true);


						}
						else //pNode is the greater child
						{
							//Left Right case

							//left rotation of parent
							RotateLeft(pParent, false);

							//LL rotation case
							RotateRight(pGramps, true);


						}
					}
					else // pParent is the higher child
					{
						//pNode is the lower child
						if (pNode == pParent->GetHigher())
						{
							//Right Left Case

							

							RotateLeft(pGramps, true);
						}
						else //pNode is the greater child
						{
							//Right Right Case

							RotateRight(pParent, false);

							RotateLeft(pGramps, true);

						}
					}
				
				}
			}

		}
	}

	//-------------------------------------
	//Rotates the node left FIX GRANDPARENT CHILDREN NODES
	//-------------------------------------
	void RotateLeft(RbtNode* pNode, bool bSwapColours = false)
	{
		if (pNode)
		{

			//Get a pointer to the higher child
			RbtNode* pHigher = pNode->GetHigher();

			//Swap the colours if needed
			if (bSwapColours)
			{
				bool bColour = pNode->GetColour();

				pNode->SetColour(pHigher->GetColour());



				pHigher->SetColour(bColour);
			}

			//if the child exists
			if (pHigher)
			{
				RbtNode* pNewHigher = pHigher->GetLower();

				pNode->SetHigher(pNewHigher);

				if (pNewHigher)
				{
					pNewHigher->SetParent(pNode);
				}
				pHigher->SetParent(pNode->GetParent());

				if(!pNode->GetParent())
				{
					this->m_pRoot = pHigher;
				}
				else if(pNode == pNode->GetParent()->GetLower()) //pNode is left child
				{
					pNode->GetParent()->SetLower(pHigher);
				}
				else
				{
					pNode->GetParent()->SetHigher(pHigher);
				}

				pHigher->SetLower(pNode);
				pNode->SetParent(pHigher);



			}
		}



	}


	//-------------------------------------
	//Rotates the node right
	//-------------------------------------
	void RotateRight(RbtNode* pNode, bool bSwapColours = false)
	{

		//Get the lower node of pNode
		RbtNode* pLower = pNode->GetLower();

		//swaps the colours of pNode and pLower
		if (bSwapColours)
		{
			bool bColour = pNode->GetColour();

			pNode->SetColour(pLower->GetColour());

			pLower->SetColour(bColour);
		}

		//if the child exists
		if (pLower)
		{
			RbtNode* pNewLower = pLower->GetHigher();

			pNode->SetLower(pNewLower);

			if (pNewLower)
			{
				pNewLower->SetParent(pNode);
			}
			pLower->SetParent(pNode->GetParent());

			if (!pNode->GetParent())
			{
				this->m_pRoot = pLower;
			}
			else if (pNode == pNode->GetParent()->GetLower()) //pNode is left child
			{
				pNode->GetParent()->SetLower(pLower);
			}
			else
			{
				pNode->GetParent()->SetHigher(pLower);
			}

			pLower->SetHigher(pNode);
			pNode->SetParent(pLower);
		}
	}


	RbtNode* m_pHighest;
	RbtNode* m_pLowest;

};


