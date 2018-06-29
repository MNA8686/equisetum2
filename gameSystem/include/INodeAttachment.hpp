#if !defined(_EQINODEATTACHMENT_H_)
#define _EQINODEATTACHMENT_H_

#include <stdint.h>
using NodeID = int32_t;

class INodeAttachment
{
public:
	virtual void SetNodeID(NodeID id) = 0;
	virtual NodeID GetNodeID() const = 0;
};

#endif
