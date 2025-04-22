template <typename T>
class List : public Serializable
{
public:
    bool Serialize(Buffer& buffer, Error& error) const override
    {
        uint32_t size = static_cast<uint32_t>(Items.size());
        if (!buffer.Write(size, error))
        {
            error.SetError(1, "Failed to serialize list size");
            return false;
        }
        for (const auto& item : Items)
        {
            if (!item.Serialize(buffer, error))
            {
                return false;
            }
        }
        return true;
    }

    bool Deserialize(Buffer& buffer, Error& error) override
    {
        uint32_t size;
        if (!buffer.Read(size, error))
        {
            error.SetError(2, "Failed to deserialize list size");
            return false;
        }
        Items.clear();
        Items.reserve(size);
        for (uint32_t i = 0; i < size; ++i)
        {
            T newItem;
            if (!newItem.Deserialize(buffer, error))
            {
                return false;
            }
            Items.push_back(newItem);
        }
        return true;
    }

private:
    std::vector<T> Items;
};
