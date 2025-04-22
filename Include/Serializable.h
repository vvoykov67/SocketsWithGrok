#include "Buffer.h"
#include "Error.h"

class Serializable
{
public:
    virtual ~Serializable() = default;
    virtual bool Serialize(Buffer& buffer, Error& error) const = 0;
    virtual bool Deserialize(Buffer& buffer, Error& error) = 0;
};
