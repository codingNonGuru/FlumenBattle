#pragma once

template <class ObjectType>
class Singleton
{
    static ObjectType *instance;

protected:
    Singleton()
    {
        instance = this;
    }

public:
    static ObjectType * Get()
    {
        if(instance == nullptr)
        {
            return new ObjectType();
        }

        return instance;
    }
};

template <class ObjectType>
ObjectType * Singleton<ObjectType>::instance = nullptr;