#ifndef __ProtocolUtil_H__
#define __ProtocolUtil_H__

#include "jce/Jce.h"

using namespace taf;
using namespace std;

namespace Observer
{

class ProtocolUtil
{
public:
    
    template<typename T>
    static void writeTo(const T &from, vector<char> &to)
    {
        string s;

        JceOutputStream<BufferWriter> os;
        from.writeTo(os);
        s.assign(os.getBuffer(), os.getLength());

        to.assign(s.begin(), s.end());
    }

    template<typename T>
    static void writeTo(const T &from, string &to)
    {
        JceOutputStream<BufferWriter> os;
        from.writeTo(os);
        to.assign(os.getBuffer(), os.getLength());
    }

    template<typename T>
    static void readFrom(const string &from, T &to)
    {
        if(from.empty())
        {
            return;
        }

        JceInputStream<BufferReader> is;
        is.setBuffer(from.c_str(), from.length());
        to.readFrom(is);
    }

private:

};
}

#endif


