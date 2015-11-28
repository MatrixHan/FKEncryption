// TestEncryption.cpp : �������̨Ӧ�ó������ڵ㡣
//

#include "stdafx.h"
#include <iostream>
#include <vector>
using namespace std;

//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------

#include "../FK_Encryption/Include/RawRandomGenerator.h"
#include "../FK_Encryption/Include/FKEncrypt.h"

#if defined( _DEBUG ) || defined( DEBUG )
	#pragma comment( lib, "../FK_Encryption/Lib/FK_Encryption_D.lib")
#else
	#pragma comment( lib, "../FK_Encryption/Lib/FK_Encryption.lib")
#endif

//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------

unsigned char					g_DESKey[8];				// ��ǰʹ�� DES ��Կ
vector<unsigned char>			g_vecEncryptBuffer;			// ��Ϣ���ܻ���
vector<unsigned char>			g_vecDecryptBuffer;			// ��Ϣ���ܻ���
unsigned char					g_DataBuffer[128];			// ��������		����ʱ��128Ϊ��׼��С������Ϸ��Ϊ��������Ҳ�ɣ�����ʱ����仯����
unsigned int					g_EncryptedLen;				// ���ܺ�����ĳ���

//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------
//�ͻ��˷�������δ��֤ʱ���õĹ̶������㷨��Կ������Կ���ᱻ��Ϊģ��������룬�Զ�����Ϊ�����������������Ϊ��������
#define			FK_DEFAULT_ENCRYPT_KEY1		981345123
#define			FK_DEFAULT_ENCRYPT_KEY2		165252668
#define			FK_DEFAULT_ENCRYPT_KEY3		496212351
//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------

// MD5��֤ http://www.md5.com.cn/index.php


int _tmain(int argc, _TCHAR* argv[])
{
	// ����DES��Կ
	RawRandomGenerator sKeyGenerator;
	sKeyGenerator.GenerateBytesByTime( g_DESKey, 8 );

	cout << "====================================" << endl;
	cout << "���ɵ�DES��ԿΪ:" << endl;
	for( int i = 0; i < 8; ++i )
	{
		cout << i << " : "<< unsigned int( g_DESKey[i] ) << "  \t\t-----\t" << g_DESKey[i] << endl;
	}
	cout << "====================================" << endl;
	cout << "������һ���ַ�����Ϊ��������( ���ڲ���������С��128�ֽ� )" << endl;
	cin >> g_DataBuffer;

	// �������ĳ���
	unsigned char* pEncryptedBuffer = NULL;					// ���ܺ������
	unsigned int nDataSize			= sizeof(g_DataBuffer);	// ����ǰ�����ĳ���
	g_EncryptedLen = FKGetEncryptedSize( sizeof(g_DataBuffer) );
	g_vecEncryptBuffer.resize( g_EncryptedLen );
	pEncryptedBuffer = &g_vecEncryptBuffer[0];

	FKEncrypt< FK_DEFAULT_ENCRYPT_KEY1, FK_DEFAULT_ENCRYPT_KEY2, FK_DEFAULT_ENCRYPT_KEY3 >( pEncryptedBuffer, g_EncryptedLen, g_DataBuffer, sizeof(g_DataBuffer) );
	cout << "====================================" << endl;
	cout << "���ܺ����ĳ���Ϊ:  " << g_EncryptedLen << endl;
	for( int i = 0; i < g_EncryptedLen; ++i )
	{
		cout << i << " : "<< unsigned int( pEncryptedBuffer[i] ) << "  \t\t-----\t" << pEncryptedBuffer[i] << endl;
	}

	cout << "====================================" << endl;
	nDataSize = FKDecrypt< FK_DEFAULT_ENCRYPT_KEY1, FK_DEFAULT_ENCRYPT_KEY2, FK_DEFAULT_ENCRYPT_KEY3 >( pEncryptedBuffer, g_EncryptedLen );
	cout << "���ܺ����ĳ���Ϊ:  " << nDataSize << endl;
	for( int i = 0; i < nDataSize; ++i )
	{
		if ( unsigned int( pEncryptedBuffer[i] ) != 0 )
		{
			cout << i << " : "<< unsigned int( pEncryptedBuffer[i] ) << "  \t\t-----\t" << pEncryptedBuffer[i] << endl;
		}
	}
	cout << "====================================" << endl;
	cout << "��������Ϊ : " << pEncryptedBuffer << endl;

	// MD5����
	for ( ;; )
	{
		cout << "====================================================================================" << endl;
		char szSrcBuffer[512] = { 0 };
		cout << "������һ������:( ������0���˳������� ) " << endl;
		cin >> szSrcBuffer;
		if ( szSrcBuffer == 0 )
		{
			break;
		}
		char szBuffer[33] = { 0 };
		memcpy( szBuffer, FK_MD5Encrypt( szSrcBuffer ).c_str(), 33 );

		cout << "====================================" << endl;
		cout << "MD���ܺ�Ϊ : " << endl;
		for( unsigned int i = 0; i < 32; ++i )
		{
			cout << szBuffer[i];
		}
		cout << endl;
		cout << "====================================" << endl;
	}

	return 0;
}

