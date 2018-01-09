#ifndef SRC_SINGLETON_H
#define SRC_SINGLETON_H


#include "common/type_def.h"
#include "LockInterface.h"

/*
 * 单例，线程安全，非懒加载
 */
namespace sjtu
{

    template <typename T>
    class Singleton
    {
    public:
        static T *GetInstance()
        {
            return sInstance;
        }

        static void Release()
        {
            delete sInstance;
            sInstance = 0;
        }

    private:
        static T *sInstance;

    protected:
    };

    template <typename T>
    T *Singleton<T>::sInstance = new T;

}


#endif //SRC_SINGLETON_H
