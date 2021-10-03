#pragma once
#include <functional>
#include "ObjectPool.h"
#include <iostream>


template<typename T>
// BstNode struct 
class Node
{
public:
	Node()
	{
		m_pHigher = nullptr;
		m_pLower = nullptr;
		m_pParent = nullptr;
	}

	Node(T value, Node<T>* pLower = nullptr, Node<T>* pHigher = nullptr, Node<T>* pParent = nullptr)
	{
		m_Value = value;
		m_pHigher = pHigher;
		m_pLower = pLower;
		m_pParent = pParent;
	}

	void Initialise(T value, Node<T>* pLower = nullptr, Node<T>* pHigher = nullptr, Node<T>* pParent = nullptr)
	{
		m_Value = value;
		m_pHigher = pHigher;
		m_pLower = pLower;
		m_pParent = pParent;
	}

	virtual ~Node()
	{
		
	}

	T GetValue()
	{
		return m_Value;
	}

	void SetValue(T value)
	{
		m_Value = value;
	}

	virtual Node<T>* GetParent()
	{
		return m_pParent;
	}

	virtual Node<T>* GetLower()
	{
		return m_pLower;
	}

	virtual Node<T>* GetHigher()
	{
		return m_pHigher;
	}

	virtual void SetParent(Node<T>* pParent)
	{
		m_pParent = pParent;
	}

	virtual void SetLower(Node<T>* pLower)
	{
		m_pLower = pLower;
	}

	virtual void SetHigher(Node<T>* pHigher)
	{
		m_pHigher = pHigher;
	}
private:
	//What node represents
	T m_Value;

	//the left node. child that is lower than it
	Node<T>* m_pLower;

	//right node, child that is higher than it
	Node<T>* m_pHigher;

	//The Parent of this node
	Node<T>* m_pParent;



};

//templated type alias  means that Function<int> ==  std::function<bool(int pData1, int pData2)>;
template<typename T>
using Function = std::function<bool(T data1, T data2)>;



template<typename T, typename BstNode = Node<T>> 
class BinarySearchTree
{
public:

	
	//-------------------------------------
	// Initialises the root node of the tree, to rootValue
	// COMPARISONFUNCTION HAS TO BE A FUNCTION/LAMBDA ||
	// true if left parameter greater than right
	//-------------------------------------
	BinarySearchTree(Function<T> comparisonFunction, int nDefaultNodeNumber = 5)
	{
	
		m_pNodePool = new ObjectPool<BstNode>(nDefaultNodeNumber);

		m_ComparisonFunction = comparisonFunction;
	}


	virtual ~BinarySearchTree()
	{
		//Delete object pool
		if(m_pNodePool)
		{
			

			delete m_pNodePool;
			m_pNodePool = nullptr;
		}

		//should be nullptr
		if(m_pRoot)
		{
			std::cout << "ERROR: m_pRoot is NOT nullptr after deleting all nodes";

			delete m_pRoot;
		}
	}


	//-------------------------------------
	//Inserts the nNumber onto the tree
	//-------------------------------------
	virtual BstNode* Add(T data)
	{
		BstNode* pNewNode = nullptr;


		//if the node pool exists
		if(m_pNodePool)
		{
			pNewNode = m_pNodePool->GetObject();

			if(pNewNode)
			{
				pNewNode->Initialise(data);
			}
			
		}
		else
		{
			pNewNode = new BstNode(data);
		}


		if(!Insert(pNewNode))
		{
			m_pNodePool->Return(pNewNode);
			return nullptr;
		}

		//make sure new node is root if nodepool doesnt exist
		if(!m_pNodePool && pNewNode != m_pRoot)
		{
			std::cout << "ERROR: NODE POOL DOES NOT EXIST, BST NOT INITIALISED PROPERLY";
		}
		//else if(!m_pNodePool)
		//{
		//	//Create node pool
		//	m_pNodePool = new NodePool<T>(5,m_pRoot);
		//	m_pDelete = m_pRoot;
		//}

		return pNewNode;
	}

	//-------------------------------------
	//Removes pData from the tree
	//-------------------------------------
	virtual void Remove(T pData)
	{
		RemoveNode(FindNode(pData));
	}

	//-------------------------------------
	// Returns whether a number exists on the tree
	//-------------------------------------
	bool Find(T pData)
	{
		return FindNode(pData);
	}
protected: 

	//-------------------------------------
	// Returns the node that contains the number
	//-------------------------------------
	virtual BstNode* FindNode(T data)
	{

		//make sure comparison function exists
		if (m_ComparisonFunction)
		{
			BstNode* pTarget = m_pRoot;


			while (pTarget != nullptr)
			{
				BstNode* pHigher = pTarget->GetHigher();
				BstNode* pLower = pTarget->GetLower();

				//if these are both false, then they are equal
				if (!m_ComparisonFunction(pTarget->GetValue(), data) && !m_ComparisonFunction(data,pTarget->GetValue()))
				{
					return pTarget;
				}

				//if data is greater than pTarget
				if (m_ComparisonFunction(data, pTarget->GetValue()))
				{
					//set the target to the higher node
					if (pTarget->GetHigher())
					{
						pTarget = pHigher;
						continue;
					}
				}

				//else set the target to the lower node
				if (pTarget->GetLower())
				{
					pTarget = pLower;
					continue;
				}

				//break if reached end of the tree
				break;

			}
		}




		return nullptr;
	}

	//-------------------------------------
	// Returns successor of a node
	//-------------------------------------

	BstNode* InOrderSuccessor(BstNode* pNode)
	{
		if(pNode)
		{
			BstNode* pHigher = pNode->GetHigher();
			BstNode* pLower = pNode->GetLower();

		
			if(pLower)
			{
				//start with the lower child
				BstNode* pTarget = pLower;

				while(pTarget)
				{
					//if the higher child exists
					if(pTarget->GetHigher())
					{
						pTarget = pTarget->GetHigher();
						continue;
					}
					
					return pTarget;
					
				}
			}
			else if(pHigher) //No left tree
			{
				//start with the lower child
				BstNode* pTarget = pHigher;

				while (pTarget)
				{
					//if the higher child exists
					if (pTarget->GetLower())
					{
						pTarget = pTarget->GetLower();
						continue;
					}

					return pTarget;

				}
			}
			
		}

		return nullptr;
	}


	//-------------------------------------
	// Removes the BstNode from the tree, returns replacement
	//-------------------------------------
	virtual BstNode* RemoveNode(BstNode* pNode)
	{
		//The node has to exist
		if (pNode)
		{

			//Get pointers to the higher and lower nodes
			BstNode* pHigher = pNode->GetHigher();
			BstNode* pLower = pNode->GetLower();

			//if pNode a leaf node
			if (!pHigher && !pLower)
			{

				//Pointer to
				BstNode* pParent = pNode->GetParent();

				if (pParent)
				{
						//if you are the higher child
						if (pParent->GetHigher() == pNode)
						{
							pParent->SetHigher(nullptr);

							ReturnNode(pNode);

							//no replacement
							return nullptr;
						}

						ReturnNode(pNode);
					
						pParent->SetLower(nullptr);
						return nullptr;
				}

			} //if one child exists
			else if (pHigher && !pLower)
			{
				//if the higher child exists, take the place of the parent


				//Get pointer to the parent
				BstNode* pParent = pNode->GetParent();

				//if the node to be deleted is the higher child of its parent, then
				if (pParent->GetHigher() == pNode)
				{
					ReturnNode(pNode);

					//set the higher node of the parent to be the higher node of the node that was deleted
					pParent->SetHigher(pHigher);

					pHigher->SetParent(pParent);

					return pHigher;

				}

				//other wise set the node to be the lower child
				ReturnNode(pNode);

				pParent->SetLower(pHigher);

				pHigher->SetParent(pParent);

				return pLower;
			}
			else if (!pHigher && pLower)
			{

				//if the lower child exists, take the place of the parent

				//get a pointer to the parent of the node to be deleted
				BstNode* pParent = pNode->GetParent();


				//if the node to be deleted is higher than its parent
				if (pParent->GetHigher() == pNode)
				{

					//set the higher node of the parent to be the lower node of the node that was deleted
					ReturnNode(pNode);

					pParent->SetHigher(pLower);

					pLower->SetParent(pParent);
					
					return pLower;

				}

				//other wise set the node to be the lower child
				ReturnNode(pNode);

				pParent->SetLower(pLower);

				pLower->SetParent(pParent);

				return pLower;

			}
			else //2 children
			{

				//find the inorder successor of the node
				BstNode* pSuccessor = InOrderSuccessor(pNode);


				//Successor node takes the place of new node
				MoveNode(pSuccessor,pNode);


				return pNode;
			}
			



		}

		return nullptr;
	}

	//-------------------------------------
	// Makes replacement node take the place of replacing node
	//-------------------------------------
	virtual void MoveNode(BstNode* pReplacement, BstNode* pReplacing)
	{

		//store copy of value
		T value = pReplacement->GetValue();


		//Remove the replacement node from the tree
		RemoveNode(pReplacement);


		//set the value
		pReplacing->SetValue(value);

	}


	//-------------------------------------
	// Inserts value into tree 
	//-------------------------------------
	BstNode* Insert(BstNode* pNode)
	{
		

		//If the root node doesn't exist
		if (!m_pRoot)
		{
			//Add the number at the root node

			m_pRoot = pNode;

			//Return, no need to do the rest of the function
			return m_pRoot;
		}

		BstNode* pTarget = m_pRoot;


		//-------------------------
		// Loop through the tree to find
		// spot
		//-------------------------
		//Only continue if the comparison function exists
		if (m_ComparisonFunction)
		{
			//Repeat until return/break is called
			while (pTarget)
			{
				//if pNode->GetValue() is greater than target value
				if (m_ComparisonFunction(pNode->GetValue(), pTarget->GetValue()))
				{
					//We need to make sure a duplicate node isn't entered
					//So check if the parent exists
					if(pTarget->GetParent())
					{
						//if the target node is the lower node
						if(pTarget->GetParent()->GetLower() == pTarget)
						{
							//pNode is > pTarget. If code is here than pTarget < pTarget->GetParent().
							//Therefore, pTarget->GetParent() > pNode. If this isn't true, than pTarget->GetParent() == pNode
							if(!m_ComparisonFunction(pTarget->GetParent()->GetValue(),pNode->GetValue()))
							{
								return nullptr;
							}
						}
					}


					//if pTarget does not have a higher child
					if (!pTarget->GetHigher())
					{
						pTarget->SetHigher(pNode);

						pNode->SetParent(pTarget);

						//placed, exit loop
						return pNode;
					}

					//switch target to the higher child
					pTarget = static_cast<BstNode*>(pTarget->GetHigher());

					//skip to next interation of loop
					continue;
				}

				//If pNode->GetValue() is lower than target value

				//If GetLower() doesn't exist
				if (!pTarget->GetLower())
				{
					//If the code reaches here, then the first check must have been false (if it was true it was retarget andd continue)
					//if this check is 
					if (m_ComparisonFunction(pTarget->GetValue(), pNode->GetValue()))
					{




						pTarget->SetLower(pNode);

						pNode->SetParent(pTarget);


						//Placed, exit loop
						return pNode;
					}

					//If code reaches here than the values are the same
					return nullptr;
				}

				pTarget = static_cast<BstNode*>(pTarget->GetLower());

			}

			return pNode;
		}


		return nullptr;
	}

	//-------------------------------------
	// Returns the node adjacent to pNode
	//-------------------------------------
	BstNode* GetSibling(BstNode* pNode)
	{

		//if node is not null
		if (pNode)
		{
			//get the parent
			BstNode* pParent = pNode->GetParent();

			//if pParent is not null
			if (pParent)
			{
				//and if this node's value is greater than its parent
				if (pNode->GetValue() > pParent->GetValue())
				{
					//then return lower
					return pParent->GetLower();
				}
				return pParent->GetHigher();

			}

		}

		return nullptr;
	}



	//-------------------------------------
	// returns the parents sibling node
	//-------------------------------------
	BstNode* GetUncle(BstNode* pNode)
	{
		//If node exists and its not the root node
		if (pNode && pNode != m_pRoot)
		{
			return GetSibling(pNode->GetParent());
		}
		return nullptr;
	}

	//Gives the pointer back to the object pool for later use
	void ReturnNode(BstNode* pNode)
	{
		if (m_pNodePool)
		{
			
			m_pNodePool->Return(pNode);
		}
		else
		{

			delete pNode;
			std::cout << "Deleting node when BstNode Pool doesn't exist";
		}
	}


	//pointer to the root node
	BstNode* m_pRoot;

	//ObjectPool of nodes, storing T
	ObjectPool<BstNode>* m_pNodePool;

	//std::function -> bool x(T,T);
	Function<T> m_ComparisonFunction;

	Function<T> m_IsEqual;


};

