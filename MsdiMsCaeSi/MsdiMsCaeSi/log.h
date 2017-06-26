
/**
* @file  log.h
* @brief �������Ͷ���
* @author       Vincent
* @date     2015-5-24
* @version  A001
* @copyright Vincent
*/
#pragma  once
#ifndef LOG_H  
#define LOG_H  



/** log�ļ�·��*/
#define LOG_FILE_NAME "D:\\log.txt"

/** ���ÿ���*/
#define LOG_ENABLE

#include <fstream>  
#include <string>  
#include <ctime>  
#include <windows.h>  

using namespace std;
/**
* @brief �������log�ļ�����1TYPENUM3.
*/
class CLog
{
public:
	/**
	* @brief ��ȡLog�ļ���·��
	* @param[in] szPath ·��ָ��
	* @return �޷���ֵ
	*/
	static void GetLogFilePath(char* szPath)
	{
		/**
		* @brief getmoduleFileName.
		*/
		GetModuleFileNameA(NULL, szPath, MAX_PATH);
		ZeroMemory(strrchr(szPath, ('\\')), strlen(strrchr(szPath, ('\\')))*sizeof(char));
		//strcat(szPath, "\\");
		memset(szPath, 0, sizeof(szPath));
		strcat(szPath,LOG_FILE_NAME);
	}
	/**
	* @brief ���һ�����ݣ��������ַ���(ascii)����������������������ö��
	* ��ʽΪ��[2011-11-11 11:11:11] aaaaaaa������
	* @param[in] x Ҫд��log������
	* @return �޷���ֵ
	*/
	template <class T>
	static void WriteLog(T x)
	{
		//if (!StartOrNot)
		//{
		//	return;
		//}
		CHAR szPath[MAX_PATH] = { 0 };
		GetLogFilePath(szPath);
		ofstream fout(szPath, ios::app);
		fout.seekp(ios::end);
		fout << GetSystemTime() << x << endl;
		fout.close();
	}
	/**
	* @brief ���2�����ݣ��ԵȺ����ӡ�һ������ǰ����һ�������������ַ�������������������ֵ
	* @param[in] x1 Ҫд��log������
	* @param[in] x2 Ҫд��log������  
	* @return �޷���ֵ
	*/
	template<class T1, class T2>
	static void WriteLog2(T1 x1, T2 x2)
	{
		if (!StartOrNot)
		{
			return;
		}
		CHAR szPath[MAX_PATH] = { 0 };
		GetLogFilePath(szPath);
		ofstream fout(szPath, ios::app);
		fout.seekp(ios::end);
		fout << GetSystemTime() << x1 << " = " << x2 << endl;
		fout.close();
	}
	/**
	* @brief ���һ�е�ǰ������ʼ�ı�־,�괫��__FUNCTION__
	* @param[in] xҪд��log������
	* @return �޷���ֵ
	*/
	template <class T>
	static void WriteFuncBegin(T x)
	{
		if (!StartOrNot)
		{
			return;
		}
		CHAR szPath[MAX_PATH] = { 0 };
		GetLogFilePath(szPath);	
		remove(szPath);
			//DeleteFile(szPath);
		ofstream fout(szPath, ios::app);
		fout.seekp(ios::end);
		fout << GetSystemTime() << "	--------------------" << x << "  Begin--------------------" << endl;
		fout.close();
	}
	/**
	* @brief ���һ�е�ǰ���������ı�־���괫��__FUNCTION__
	* @param[in] xҪд��log������
	* @return �޷���ֵ
	*/
	template <class T>
	static void WriteFuncEnd(T x)
	{
		if (!StartOrNot)
		{
			return;
		}
		CHAR szPath[MAX_PATH] = { 0 };
		GetLogFilePath(szPath);
		ofstream fout(szPath, ios::app);
		fout.seekp(ios::end);
		fout << GetSystemTime() << "--------------------" << x << "  End  --------------------" << endl;
		fout.close();
	}
	/**
	* @brief ��log�������
	*/
	static void Open()
	{
		StartOrNot = true;
	}
	/**
	* @brief �ر�log�������
	*/
	static void End()
	{
		StartOrNot = false;
	}
	/** ���忪�ر��� StartOrNot*/
	static bool StartOrNot;
private:
	/**
	* @brief ��ȡ����ʱ�䣬��ʽ��"[2011-11-11 11:11:11] 
	*/
	static string GetSystemTime()
	{
		time_t tNowTime;
		time(&tNowTime);
		tm tLocalTime ;
		localtime_s(&tLocalTime, &tNowTime);
		char szTime[30] = { '\0' };
		strftime(szTime, 30, "[%Y-%m-%d %H:%M:%S] ", &tLocalTime);
		string strTime = szTime;
		return strTime;
	}

};

#ifdef LOG_ENABLE

/** ���Log�ĺ궨��1�������ڿ������ַ���(ascii)����������������bool��*/
#define LOG(x)			 CLog::WriteLog(x);		
/** ���Log�ĺ궨��2��x1,x2�������ַ���(ascii)����������������bool��*/
#define LOG2(x1,x2)		 CLog::WriteLog2(x1,x2);
/** �����ǰ���ں������ĺ궨��*/
#define LOG_FUNC		 LOG(__FUNCTION__)	
/** �����ǰ�кŵĺ궨��*/
#define LOG_LINE		 LOG(__LINE__)			
/** ��ʼ�����־�ĺ궨����ʽ�磺[ʱ��]"------------FuncName  Begin------------"*/
#define LOG_FUNC_BEGIN   CLog::WriteFuncBegin(__FUNCTION__);	
/** ��ʼ�����־�ĺ궨����ʽ�磺[ʱ��]"------------FuncName  End------------"*/
#define LOG_FUNC_END     CLog::WriteFuncEnd(__FUNCTION__);		
/** ��Log������غ궨��*/
#define LOG_OPEN         CLog::Open();
/** �ر�Log������غ궨��*/
#define LOG_END          CLog::End();
#else

#define LOG(x)				
#define LOG2(x1,x2)		
#define LOG_FUNC		
#define LOG_LINE		
#define LOG_FUNC_BEGIN  
#define LOG_FUNC_END  
#define LOG_OPEN
#define LOG_END 
#endif

#endif 
/** @}*/ // �Զ�ע���ĵ�����