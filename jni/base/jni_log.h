#ifndef BASE_ANDROID_LOG_H_
#define BASE_ANDROID_LOG_H_

#include <android/log.h>
#include <sstream>

#define ENABLE_LOG 0

using namespace std;

/**
* @brief 临时类, 析够的时候写日志
*/
class JNILog
{
public:
	/**
	 * @brief 构造.
	 *
	 * @param stream
	 * @param mutex
	 */
	JNILog(int level) : _level(level)
	{
	}

	/**
	 * @brief 析构
	 */
	~JNILog()
	{
#if ENABLE_LOG
		__android_log_print(_level, "fileobserver", "%s", _stream.str().c_str());
#endif
	}

	/**
	* @brief 重载<<
	*/
	template <typename P>
	JNILog& operator << (const P &t)  { _stream << t;return *this;}

	/**
	 * @brief endl,flush等函数
	 */
	typedef ostream& (*F)(ostream& os);
	JNILog& operator << (F f)         { (f)(_stream);return *this;}

	/**
	 * @brief  hex等系列函数
	 */
	typedef ios_base& (*I)(ios_base& os);
	JNILog& operator << (I f)         { (f)(_stream);return *this;}

	/**
	 * @brief 字段转换成ostream类型.
	 *
	 * @return ostream&
	 */
	operator ostream&()
	{
		return _stream;
	}

	//不实现
	JNILog(const JNILog& lt);
	JNILog& operator=(const JNILog& lt);

protected:

	int					_level;

	/**
	 * 输出流
	 */
	std::stringstream	_stream;
};


#define LOG(x) JNILog(ANDROID_LOG_##x)

#endif // endof BASE_ANDROID_LOG_H_
