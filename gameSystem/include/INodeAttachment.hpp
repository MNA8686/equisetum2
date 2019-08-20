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

class INodeAttachment
{
public:
	static const int32_t nullNode = -2;

	virtual void SetNodeHandler(const NodeHandler& handler) = 0;
	virtual NodeHandler GetNodeHandler() const = 0;
};

#endif
