/********************************************************************
// FileName: e:\Code\FK_3D\FK_Encryption\FK_Encryption\Inline\FKEncrypt.inl
// Date:     2007��11��30��
// Author:   FreeKnightDuzhi
// Comment:  
*********************************************************************/

#pragma once
#include "../Include/md5.h"
#include <string>
#include <windows.h>

//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------

//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------
//������Կ�������ֽڻ�λ����ֵ
template < unsigned int Key1, unsigned int Key2, unsigned int Key3 >
void FKGetEncryptFourByteIndex( unsigned int pByteIndex[4] )
{
	pByteIndex[0] = ( Key1 & 0xFF ) % 4;

	pByteIndex[1] = ( ( Key1 >> 7 ) & 0x7FF ) % 4;
	while( pByteIndex[1] == pByteIndex[0] )
		pByteIndex[1] = ( pByteIndex[1] + 1 ) % 4;

	pByteIndex[2] = ( ( Key1 >> 18 ) & 0x3FF ) % 4;
	while( pByteIndex[2] == pByteIndex[0] || pByteIndex[2] == pByteIndex[1] )
		pByteIndex[2] = ( pByteIndex[2] + 1 ) % 4;

	pByteIndex[3] = 0;
	while( pByteIndex[3] == pByteIndex[0] || pByteIndex[3] == pByteIndex[1] || pByteIndex[3] == pByteIndex[2] )
		pByteIndex[3] = ( pByteIndex[3] + 1 ) % 4;
}
//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------
//��ȡ���ܺ�����ݳ���
__inline unsigned int FKGetEncryptedSize( const unsigned int nInLen )
{
	//����ѭ���������
	const unsigned int nRound = ( nInLen / 4 ) + ( ( ( nInLen % 4 ) != 0 ) ? 1 : 0 );

	//����ÿ 4 �ֽڲ��� 1 �ֽں�ĳ���
	const unsigned int nAfterPadLen = nRound * ( 4 + 1 );

	//���� 4 �ֽڶ����ĳ��Ȳ�����ԭʼ���ȼ�¼ֵռ�õĳ���
	const unsigned int nEncryptedLen = ( ( nAfterPadLen / 4 ) + ( ( ( nAfterPadLen % 4 ) != 0 ) ? 1 : 0 ) ) * 4 + sizeof(unsigned int);

	return nEncryptedLen;
}
//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------
//ʹ��ָ������Կ�������ݲ����ؼ��ܺ�����ݳ���
template < unsigned int Key1, unsigned int Key2, unsigned int Key3 >
unsigned int FKEncrypt( void* pOut, const unsigned int nOutLen, const void* pIn, const unsigned int nInLen )
{
	const unsigned int nEncryptedLen = FKGetEncryptedSize( nInLen );
	if( nOutLen < nEncryptedLen )
		return 0;

	const unsigned char* pInPtr = (const unsigned char*)pIn;

	*(unsigned int*)pOut = nInLen;
	unsigned char* pOutPtr = (unsigned char*)pOut + sizeof(unsigned int);

	//������Կ�����������
	unsigned int pMixKey[4];
	pMixKey[0] = Key1;
	pMixKey[1] = Key2;
	pMixKey[2] = Key3;
	pMixKey[3] = ( Key1 & 0x3FF ) | ( Key2 & ( 0x3F << 10 ) ) | ( Key3 & ( 0xFFFF << 16 ) );

	RawRandomGenerator sRawRandomGenerator;
	sRawRandomGenerator.ResetSeed();
	sRawRandomGenerator.MixSeed( (unsigned char*)&pMixKey, 16 );

	//������Կ�������ֽڻ�λ����ֵ
	unsigned int pByteIndex[4];
	FKGetEncryptFourByteIndex< Key1, Key2, Key3 >( pByteIndex );

	//����ÿ 4 �ֽ������������� 1 �ֽ����ֵ�����ѭ������
	unsigned int nOutByte = 0;
	for( unsigned int nInByte=0; nInByte<nInLen; nInByte+=4, nOutByte+=5 )
	{
		//��� 1 �ֽڵ��������
		sRawRandomGenerator.GenerateBytes( &pOutPtr[nOutByte], 1 );

		//��������������ݽ�β����
		unsigned char pInBytes[4] = { 0 };
		for( unsigned int i=0; i<4; ++i )
		{
			if( nInByte + i < nInLen )
			{
				pInBytes[i] = pInPtr[ nInByte + i ];
			}
			else
			{
				break;
			}
		}

		//�������ֽڻ�λ
		pOutPtr[nOutByte+1] = pInBytes[ pByteIndex[0] ];
		pOutPtr[nOutByte+2] = pInBytes[ pByteIndex[1] ];
		pOutPtr[nOutByte+3] = pInBytes[ pByteIndex[2] ];
		pOutPtr[nOutByte+4] = pInBytes[ pByteIndex[3] ];

		//������Կ����ѭ��λ��
		unsigned int* pBlock = (unsigned int*)&pOutPtr[nOutByte+1];

		const unsigned char nKey3 = ( Key3 & 0xFF );

		__asm
		{
			mov ebx, pBlock;
			mov eax, [ebx];
			mov cl, nKey3;
			ror eax, cl;
			mov [ebx], eax;
		}
	}

	const unsigned int nRemLen = nEncryptedLen - sizeof(unsigned int);

	//����������δ 4 �ֽڶ�������
	for(; nOutByte<nRemLen; ++nOutByte )
	{
		pOutPtr[ nOutByte ] = 0;
	}

	//����ÿ 4 �ֽڵ�ѭ������
	for( unsigned int nByte=0; nByte<nRemLen; nByte+=4 )
	{
		//������Կ����ѭ��λ��
		const unsigned char nKey2 = ( Key2 & 0xFF );

		__asm
		{
			mov ebx, pOutPtr;
			mov eax, [ebx];
			mov cl, nKey2;
			rol eax, cl;
			mov [ebx], eax;
		}

		//������Կ���в���λλ��
		const unsigned int nOriginalBlock = *(unsigned int*)pOutPtr;
		*(unsigned int*)pOutPtr = ( nOriginalBlock & ( ~Key2 ) ) | ( ~nOriginalBlock & Key2 );

		pOutPtr += 4;
	}

	return nEncryptedLen;
}
//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------
//ʹ��ָ������Կ�������ݲ����ؽ��ܺ�����ݳ���
template < unsigned int Key1, unsigned int Key2, unsigned int Key3 >
unsigned int FKDecrypt( void* pInOut, const unsigned int nInLen )
{
	//��ȡԭ���ݳ���
	const unsigned int nOutLen = *(unsigned int*)pInOut;

	const unsigned char* pInPtr = (const unsigned char*)pInOut + sizeof(unsigned int);
	unsigned char* pOutPtr = (unsigned char*)pInOut;

	//����ÿ 4 �ֽڵ�ѭ������
	const unsigned int nRemLen = nInLen - sizeof(unsigned int);
	for( unsigned int nByte=0; nByte<nRemLen; nByte+=4 )
	{
		//������Կ���в���λλ��
		unsigned int nBlock = *(unsigned int*)&pInPtr[ nByte ];
		nBlock = ( nBlock & ( ~Key2 ) ) | ( ~nBlock & Key2 );

		//������Կ����ѭ��λ��
		const unsigned char nKey2 = ( Key2 & 0xFF );

		__asm
		{
			mov eax, nBlock;
			mov cl, nKey2;
			ror eax, cl;
			lea ebx, nBlock;
			mov [ebx], eax;
		}

		*(unsigned int*)&pOutPtr[ nByte ] = nBlock;
	}

	//������Կ�������ֽڻ�λ����ֵ
	unsigned int pByteIndex[4];
	FKGetEncryptFourByteIndex< Key1, Key2, Key3 >( pByteIndex );

	//����ÿ 5 �ֽ���������ѭ������
	unsigned int nInByte = 0;
	for( unsigned int nOutByte=0; nOutByte<nOutLen; nOutByte+=4, nInByte+=5 )
	{
		//���� 1 �ֽڵ����ֵ
		unsigned int nBlock = *(unsigned int*)&pOutPtr[ nInByte + 1 ];

		//������Կ����ѭ��λ��
		const unsigned char nKey3 = ( Key3 & 0xFF );

		__asm
		{
			mov eax, nBlock;
			mov cl, nKey3;
			rol eax, cl;
			lea ebx, nBlock;
			mov [ebx], eax;
		}

		//�������ֽڻ�λ
		unsigned char* pBytes = (unsigned char*)&nBlock;

		pOutPtr[ nOutByte + pByteIndex[0] ] = pBytes[0];
		pOutPtr[ nOutByte + pByteIndex[1] ] = pBytes[1];
		pOutPtr[ nOutByte + pByteIndex[2] ] = pBytes[2];
		pOutPtr[ nOutByte + pByteIndex[3] ] = pBytes[3];
	}

	return nOutLen;
}
//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------
// �� unsigned char ����ת��Ϊ ASC2 �ַ�
static std::string ByteToHEX( unsigned char p_ucIn )
{
	static const char szDigitStr [16] = { '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'A', 'B', 'C', 'D', 'E', 'F' };
	char* szOutBuffer = new char[2];
	szOutBuffer[0] = szDigitStr[(p_ucIn >> 4) & 0x0F];	// ȡ��4λ
	szOutBuffer[1] = szDigitStr[ p_ucIn & 0x0F ];		// ȡ��4λ
	std::string szOut( szOutBuffer );
	return szOut;
}
//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------
// �Ƿ���UTF8�ַ���
static bool IsUTF8( const char* p_szInfo )
{
	int nSize = MultiByteToWideChar( CP_UTF8, MB_ERR_INVALID_CHARS, p_szInfo, -1, NULL, 0 );
	int nError = GetLastError();
	if ( nError == ERROR_NO_UNICODE_TRANSLATION )
	{
		return false;
	}
	return true;
}
//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------
// �Ƿ���GB2312�ַ���
static bool IsUnicode( const char* p_szInfo )
{
	int nSize = MultiByteToWideChar( 936, MB_ERR_INVALID_CHARS, p_szInfo, -1, NULL, 0 );
	int nError = GetLastError();
	if ( nError == ERROR_NO_UNICODE_TRANSLATION )
	{
		return false;
	}
	return true;
}
//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------
// �Ƿ���GB
static int IsGB( char* p_szInfo )
{
	unsigned char* pChar = ( unsigned char* )p_szInfo;
	if ( pChar[0] >= 0xa1 )
	{
		if ( pChar[0] == 0xa3 )
		{
			return 1;	// ȫ���ַ�
		}
		else
		{
			return 2;	// ����
		}
	}
	else
	{
		return 0;		// Ӣ�ģ����֣�Ӣ�ı��
	}
}
//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------
// �Ƿ��������ַ�
static bool IsHaveChinese( const char* p_szInfo )
{
	int i = 0;
	char* pInfo = ( char* )p_szInfo;
	while( *pInfo != '\0' )
	{
		i = IsGB( pInfo );
		switch( i )
		{
		case 0:
			{
				pInfo++;
			}
			break;
		case 1:
			{
				pInfo++;
				pInfo++;
			}
			break;
		case 2:
			{
				pInfo++;
				pInfo++;
				return true;
			}
			break;
		}
	}
	return false;
}
//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------
// Unicode ת��Ϊ UTF8
void UnicodeToUtf8(char *utf8) 
{
	int len = 0;
	int size_d = 8;
	DWORD dwNum = MultiByteToWideChar (CP_ACP, 0, utf8, -1, NULL, 0);
	wchar_t *wchar;
	wchar = new wchar_t[dwNum];
	if(!wchar)
	{
		delete []wchar;
	}
	MultiByteToWideChar (CP_ACP, 0,utf8, -1, wchar, dwNum);
	for(int i = 0; i <dwNum; i++)
	{
		if ((wchar[i]) < 0x80)
		{  
			utf8[len++] = (char)(wchar[i]);
		}
		else if((wchar[i]) < 0x800)
		{
			utf8[len++] = 0xc0 | ( (*wchar) >> 6 );
			utf8[len++] = 0x80 | ( (*wchar) & 0x3f );
		}
		else if((wchar[i]) < 0x10000 )
		{
			utf8[len++] = 0xE0 | ((wchar[i]) >> 12);
			utf8[len++] = 0x80 | (((wchar[i])>>6) & 0x3F);
			utf8[len++] = 0x80 | ((wchar[i]) & 0x3F);  
		}
		else if((wchar[i]) < 0x200000 ) 
		{
			utf8[len++] = 0xf0 | ( (int)(wchar[i]) >> 18 );
			utf8[len++] = 0x80 | ( ((wchar[i]) >> 12) & 0x3f );
			utf8[len++] = 0x80 | ( ((wchar[i]) >> 6) & 0x3f );
			utf8[len++] = 0x80 | ( (wchar[i]) & 0x3f );
		}
	}
}
//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------
#ifdef _WINDOWS_
	#if _MSC_VER <= 1310
		// GBK ת��Ϊ UTF8
		static char* Utf8ToGBK(const char* strUtf8)
		{
			int len=MultiByteToWideChar(CP_UTF8, 0, (LPCTSTR)strUtf8, -1, NULL,0); 
			unsigned short * wszGBK = new unsigned short[len+1]; 
			memset(wszGBK, 0, len * 2 + 2); 
			MultiByteToWideChar(CP_UTF8, 0, (LPCTSTR)strUtf8, -1, wszGBK, len); 
			len = WideCharToMultiByte(CP_ACP, 0, wszGBK, -1, NULL, 0, NULL, NULL); 
			char *szGBK=new char[len + 1]; 
			memset(szGBK, 0, len + 1); 
			WideCharToMultiByte (CP_ACP, 0, wszGBK, -1, szGBK, len, NULL,NULL); 
			return szGBK; 
		}
		// UTF8 ת���� GBK
		static char* GBKToUtf8(const char* strGBK)
		{ 
			int len=MultiByteToWideChar(CP_ACP, 0, (LPCTSTR)strGBK, -1, NULL,0); 
			unsigned short * wszUtf8 = new unsigned short[len+1]; 
			memset(wszUtf8, 0, len * 2 + 2); 
			MultiByteToWideChar(CP_ACP, 0, (LPCTSTR)strGBK, -1, wszUtf8, len); 
			len = WideCharToMultiByte(CP_UTF8, 0, wszUtf8, -1, NULL, 0, NULL, NULL); 
			char *szUtf8=new char[len + 1]; 
			memset(szUtf8, 0, len + 1); 
			WideCharToMultiByte (CP_UTF8, 0, wszUtf8, -1, szUtf8, len, NULL,NULL); 
			return szUtf8; 
		} 

	#else
		// GBK ת��Ϊ UTF8
		static char* Utf8ToGBK(const char* strUtf8)
		{
			int len=MultiByteToWideChar(CP_UTF8, 0, (LPCTSTR)strUtf8, -1, NULL,0); 
			unsigned short * wszGBK = new unsigned short[len+1];       
			memset(wszGBK, 0, len * 2 + 2); 
			MultiByteToWideChar(CP_UTF8, 0, (LPCTSTR)strUtf8, -1, (LPWSTR)wszGBK, len); 
			len = WideCharToMultiByte(CP_ACP, 0, (LPCWSTR)wszGBK, -1, NULL, 0, NULL, NULL); 
			char *szGBK=new char[len + 1]; 
			memset(szGBK, 0, len + 1); 
			WideCharToMultiByte (CP_ACP, 0, (LPCWSTR)wszGBK, -1, (LPSTR)szGBK, len, NULL,NULL); 
			return szGBK; 
		}
		// UTF8 ת���� GBK
		static char* GBKToUtf8(const char* strGBK)
		{ 
			int len=MultiByteToWideChar(CP_ACP, 0, (LPCTSTR)strGBK, -1, NULL,0); 
			unsigned short * wszUtf8 = new unsigned short[len+1]; 
			memset(wszUtf8, 0, len * 2 + 2); 
			MultiByteToWideChar(CP_ACP, 0, (LPCTSTR)strGBK, -1, (LPWSTR)wszUtf8, len); 
			len = WideCharToMultiByte(CP_UTF8, 0, (LPCWSTR)wszUtf8, -1, NULL, 0, NULL, NULL); 
			char *szUtf8=new char[len + 1]; 
			memset(szUtf8, 0, len + 1); 
			WideCharToMultiByte (CP_UTF8, 0, (LPCWSTR)wszUtf8, -1, (LPSTR)szUtf8, len, NULL,NULL); 
			return szUtf8; 
		}
	#endif // _MSC_VER <= 1310
#else
	static char* Utf8ToGBK(const char* strUtf8)
	{
		#pragma message( "��Windowsϵͳ����ʹ�� UnicodeToUtf8 ����.")
		#error ��Windowsϵͳ����ʹ�� UnicodeToUtf8 ����.
		return NULL;
	}
	static char* GBKToUtf8(const char* strGBK)
	{ 
		#pragma message( "��Windowsϵͳ����ʹ�� UnicodeToUtf8 ����.")
		#error ��Windowsϵͳ����ʹ�� UnicodeToUtf8 ����.
		return NULL;
	}
#endif // _WINDOWS_
//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------
// ����MD5����
std::string FK_MD5Encrypt( char* pIn )
{
	char* pTemp = NULL;
	if( IsHaveChinese( pIn ) )
	{
		cout << "������" << endl;
		pTemp = GBKToUtf8( pIn );
	}
	else
	{
		pTemp = pIn;
	}

	unsigned char szOutUnit[16];
	unsigned int unInLen = strlen( pTemp );
	MD5_CTX sMD5Contex;
	MD5Init( &sMD5Contex );
	MD5Update( &sMD5Contex, (unsigned char*)pTemp, unInLen );
	MD5Final( szOutUnit, &sMD5Contex );

	char p_szOut[33] = { 0 };

	for( unsigned int i=0; i<16; i++ )
	{	
		sprintf( &( p_szOut[2*i] ), "%02x", szOutUnit[i] );
	}
	return p_szOut;
}
//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------