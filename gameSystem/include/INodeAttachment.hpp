#if !defined(_EQINODEATTACHMENT_H_)
#define _EQINODEATTACHMENT_H_

#include <stdint.h>
using NodeID = int32_t;

struct NodeHandler
{
	NodeID id = -1;
	uint32_t serial = 0;

	bool operator == (NodeHandler& src) const
	{
		return id == src.id && serial == src.serial;
	}
	bool operator != (NodeHandler& src) const
	{
		return !(*this == src);
	}
};

// 本来なら抽象クラスにすべきだが、virtualを使えないのでここに実装を書く
class INodeAttachment
{
public:
	void SetNodeHandler(const NodeHandler& handler)
	{
		m_hNode = handler;
	}

	NodeHandler GetNodeHandler() const
	{
		return m_hNode;
	}

protected:
	NodeHandler m_hNode;		/// アタッチしているノードのハンドラ
};

#endif
