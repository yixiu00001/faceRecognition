/*
serialization opera
*/
#ifndef Serialization_h__
#define Serialization_h__


#include "Unified.h"

#ifdef _MSC_VER
#define uint64 unsigned __int64
#else
#define uint64 unsigned long long
#endif

inline uint64 qbswap(uint64 source)
{
	return 0
		| ((source & (0x00000000000000ff)) << 56)
		| ((source & (0x000000000000ff00)) << 40)
		| ((source & (0x0000000000ff0000)) << 24)
		| ((source & (0x00000000ff000000)) << 8)
		| ((source & (0x000000ff00000000)) >> 8)
		| ((source & (0x0000ff0000000000)) >> 24)
		| ((source & (0x00ff000000000000)) >> 40)
		| ((source & (0xff00000000000000)) >> 56);
}

inline unsigned int qbswap(unsigned int source)
{
	return 0
		| ((source & 0x000000ff) << 24)
		| ((source & 0x0000ff00) << 8)
		| ((source & 0x00ff0000) >> 8)
		| ((source & 0xff000000) >> 24);
}

inline int qbswap(int source)
{
	return qbswap((unsigned int)(source));
}

class CDataStream
{
private:

	fstream mFile;

public:
	CDataStream()
	{}
	~CDataStream()
	{}

	CDataStream& operator>>(bool &rightval)
	{
		char v = 0;

		readraw(&v,sizeof(bool));
		rightval = static_cast<bool>(v);
		return *this;
	}

	CDataStream& operator>>(float &rightval)
	{
		double f;
		*this>>f;
		rightval = (float)f;
		return *this;
	}

	CDataStream& operator>>(double &rightval)
	{
		double f;

		readraw((char*)&f,sizeof(double));
		union {
			double val1;
			uint64 val2;
		} x;
		x.val2 = qbswap(*reinterpret_cast<uint64 *>(&f));
		rightval = x.val1;
		return *this;
	}

	CDataStream& operator>>(int &rightval)
	{
		int i;
		readraw((char*)&i,sizeof(int));
		rightval = qbswap(i);
		return *this;
	}

	CDataStream& operator<<(bool rightval)
	{
		writeraw((char*)&rightval,sizeof(bool));
		return *this;
	}

	CDataStream& operator<<(float rightval)
	{
		double f = (double)rightval;
		*this<<f;
		return *this;
	}

	CDataStream& operator<<(double rightval)
	{
		union {
			double val1;
			uint64 val2;
		} x;
		x.val1 = rightval;
		x.val2 = qbswap(x.val2);

		writeraw((char*)&x.val2,sizeof(double));
		
		return *this;
	}

	CDataStream& operator<<(int rightval)
	{
		int i;
		
		i = qbswap(rightval);
		writeraw((char*)&i,sizeof(int));
		return *this;
	}

	bool load(const string &filepath,bool write = false)
	{
		mFile.open(filepath.c_str(),std::ios::binary|(write?std::ios::out:std::ios::in));

		return mFile.is_open();
	}

	void close()
	{
		mFile.close();
	}

	virtual int readraw(char *buff,int size)
	{
		mFile.read(buff,size);

		return (int)mFile.gcount();
	}

	virtual int writeraw(char *buff,int size)
	{
		mFile.write(buff,size);

		return size;
	}

	virtual bool isEof()
	{
		return mFile.peek() == EOF;
	}
};

typedef CDataStream CFileDataStream;

#endif // Serialization_h__
