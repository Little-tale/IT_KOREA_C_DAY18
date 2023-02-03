#include "../../headers/structure.h"

// 이진트리가 2개의 다리를 만들어서 검색을 빠르게 했다면
// 다리가 3개라면 검색이 더 빠르지 않을까?
// 4개? 5개? 10개? ... N개?

// 숫자의 대소(크다, 작다, 같다)를 이용하여 검색
// 트리 구조
struct TreeNode; // 구조체 선언

typedef struct Node {
	struct TreeNode* data;
	struct Node* next;
	struct Node* prev;
}Node, * pNode;

typedef struct TreeNodeList {
	pNode root, end;
}TreeNodeList, * pTreeNodeList;

pTreeNodeList CreateTreeNodeList() {
	return Allocation(sizeof(TreeNodeList));
}

Void ReleaseTreeNodeList(pTreeNodeList this) {
	if (!this) return;
	pNode now = this->root;
	pNode rem;
	while (now != 0) {
		rem = now;
		now = now->next;
		free(rem);
	}
	free(this);
}

Void RemoveTreeNodeList(pTreeNodeList this, struct TreeNode* find) {
	pNode now = this->root;
	while (now != 0) {
		if (now->data == find) {
			if (now->next) now->next->prev = now->prev;
			else this->end = now->prev;
			if (now->prev) now->prev->next = now->next;
			else this->root = now->next;
			free(now);
			return;
		}
		now = now->next;
	}
}

Void InsertTreeNodeList(pTreeNodeList this, struct TreeNode* data) {
	pNode node = Allocation(sizeof(Node));
	node->data = data;
	if (this->root) {
		this->end->next = node;
		node->prev = this->end;
	}
	else this->root = node;
	this->end = node;
}

typedef struct ControlData {
	Str name;
	Dec isfolder;
}ControlData, * pControlData;

typedef struct TreeNode {
	pControlData data;
	struct TreeNode* parent;
	pTreeNodeList childs;
}TreeNode, * pTreeNode;

typedef struct Tree {
	pTreeNode root;
}Tree, * pTree;

pTree CreateTree() {
	return Allocation(sizeof(Tree));
}

Void ReleaseTreeNode(pTreeNode node) {
	if (!node) return;
	free(node->data);
	if (node->childs) {
		pNode cnode = node->childs->root;
		while (cnode != 0) {
			ReleaseTreeNode(cnode->data);
			cnode = cnode->next;
		}
		ReleaseTreeNodeList(node->childs);
	}
	free(node);
}

Void ReleaseTree(pTree this) {
	ReleaseTreeNode(this->root);
}

pControlData CD(pChar str) {
	pControlData cd = Allocation(sizeof(ControlData));
	strcpy_s(cd->name, STR_MAX, str);
	return cd;
}

pChar FullpathFromNode(pTreeNode now) {
	if (now->parent)
		return StrAdd(StrAdd(FullpathFromNode(now->parent), "/"), now->data->name);
	else return now->data->name;
}

Void ReadFolder(pTreeNode now) {
	Path fullpath;
	strcpy_s(fullpath, PATH_MAX, StrAdd(FullpathFromNode(now), "/*"));
	WIN32_FIND_DATAA data; HANDLE fp;
	fp = FindFirstFileA(fullpath, &data);
	do {
		if (strcmp(data.cFileName, ".") == 0 || strcmp(data.cFileName, "..") == 0) continue;
		pTreeNode node = Allocation(sizeof(TreeNode));
		node->data = CD(data.cFileName);
		InsertTreeNodeList(now->childs, node);
		node->parent = now;
		if (data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
			node->data->isfolder = 1;
			node->childs = CreateTreeNodeList();
			ReadFolder(node);
		}
	} while (FindNextFileA(fp, &data));
	FindClose(fp);
}

Void ReadTree(pTree this, pChar path) {
	WIN32_FIND_DATAA data;
	HANDLE fp;
	fp = FindFirstFileA(path, &data);
	if (fp == INVALID_HANDLE_VALUE) return;
	this->root = Allocation(sizeof(TreeNode));
	this->root->data = CD(data.cFileName);
	if (data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
		this->root->childs = Allocation(sizeof(TreeNodeList));
		this->root->data->isfolder = 1;
		ReadFolder(this->root);
	}

	FindClose(fp);
}

Void ShowNode(pTreeNode data) {
	if (data->data->isfolder) SetColor(COLOR_BLACK, COLOR_LIGHT_YELLOW);
	else SetColor(COLOR_BLACK, COLOR_LIGHT_GREEN);
	printf("%s\n", FullpathFromNode(data));
	SetColor(COLOR_BLACK, COLOR_WHITE);
}

Void ShowTreeNode(pTreeNode this) {
	if (!this) return;
	ShowNode(this);
	if (this->childs) {
		pNode now = this->childs->root;
		while (now != 0) {
			ShowTreeNode(now->data);
			now = now->next;
		}
	}
}

Void ShowTree(pTree this) {
	ShowTreeNode(this->root);
}

Void FindTreeNode(pTreeNode this, pChar find) {
	if (!this) return;
	if (FindStr(this->data->name, find))
		ShowNode(this);
	if (this->childs) {
		pNode now = this->childs->root;
		while (now != 0) {
			FindTreeNode(now->data, find);
			now = now->next;
		}
	}
}

Void FindTree(pTree this, pChar find) {
	FindTreeNode(this->root, find);
}

Dec RemoveTreeNode(pTreeNode this, pChar find) {
	if (!this) return;
	if (strcmp(this->data->name, find) == 0) {
		if (this->parent) {
			RemoveTreeNodeList(this->parent->childs, this);
			ReleaseTreeNode(this);
			return 1;
		}
	}
	else {
		if (this->childs) {
			pNode now = this->childs->root;
			pNode rem;
			while (now != 0) {
				rem = now;
				now = now->next;
				RemoveTreeNode(rem->data, find);
			}
		}
	}
	return 0;
}

Void RemoveTree(pTree this, pChar find) {
	RemoveTreeNode(this->root, find);
}

// 내가 입력한 파일을 검색하는 기능
// 내가 입력한 파일을 삭제하는 기능






Void main() {
	SetColor(COLOR_BLACK, COLOR_WHITE);
	pTree tree = CreateTree();
	
	Str input;

	ReadTree(tree, "Resources");
	

	ReleaseTree(tree);
}