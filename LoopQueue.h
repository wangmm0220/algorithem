/************************************************************************/
///*功能： 数据池 及管理
///*作者：winner
///*修改时间：2018.07.27
/************************************************************************/

#ifndef __LOOPQUEUE_H__
#define __LOOPQUEUE_H__

#include <set>
#include "AssertEx.h"

template<typename DataType>
class LoopQueue : protected TC_ThreadLock, public taf::TC_Singleton< LoopQueue<DataType> >
{
public:

    LoopQueue()
	{
		m_data = NULL;
		m_MaxCount = 0;
	}

	~LoopQueue()
	{
		__ENTER_FUNCTION

			SAFE_DELETE_ARRAY(m_data);

		__LEAVE_FUNCTION
	}

	bool Init(UINT maxCount)
	{
		__ENTER_FUNCTION
		Lock lock(*this);
		m_MaxCount = maxCount;

		m_data = new DataType[m_MaxCount];

		CrashAssert(m_data);

        front = 0;
        rear = 0;
		
		return TRUE;

		__LEAVE_FUNCTION

		return FALSE;
	}

    bool isEmpty() {
        if (front == rear)
        {
            return true;
        }
        return false;
    }

    bool isFull() {
        if (front == (rear + 1) % m_MaxCount)
        {
            return true;
        }
        return false;
    }

	DataType* NewData()
	{
		__ENTER_FUNCTION

		Lock lock(*this);
        if (isFull())
        {
            return NULL;
        }

        notify();
        DataType* res = &(m_data[rear]);
        rear = (rear + 1) % m_MaxCount;
		return res;

		__LEAVE_FUNCTION

		return NULL;
	}



    DataType* DelData(size_t millsecond)
	{
		__ENTER_FUNCTION
		Lock lock(*this);

        if (isEmpty())
        {
            if (millsecond == 0)
            {
                return NULL;
            }
            if (millsecond == (size_t)-1)
            {
                wait();
            }
            else
            {
                //超时了
                if (!timedWait(millsecond))
                {
                    return NULL;
                }
            }
        }

        if (isEmpty())
        {
            return NULL;
        }

        DataType* res = &(m_data[front]);
        front = (front + 1) % m_MaxCount;
	
		return res;

		__LEAVE_FUNCTION

		return NULL;
	}
	
	UINT MaxSize(){ 
		Lock lock(*this);
		return m_MaxCount; 
	}

    UINT Size() {
        Lock lock(*this);
        return (rear + m_MaxCount - front) % m_MaxCount;
    }

private:
	DataType*			m_data;
	UINT				m_MaxCount;				//总的
	int front;          //队首
    int rear;           //对尾
};

#endif


