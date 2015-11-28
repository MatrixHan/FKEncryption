

// Author : FreeKnightDuzhi


#include <memory.h>
#include <stdlib.h>

#include "../Include/DESEncryption.h"


typedef void (*DES_FnKey)( unsigned char*, short );
typedef void (*DES_FnProcess)( unsigned char*, unsigned char* );

// DES��ļ���Key��
static const DES_FnKey DES_pFnKey[3] =
{
	deskey,
	des2key,
	des3key,
};
// DES��ļ��ܹ��ܺ�����
static const DES_FnProcess DES_pFnProcess[3] =
{
	Ddes,
	D2des,
	D3des,
};

//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------
// DES���ݼӹ�
void DES_Process( DES_Layer eLayer, void* pData, const size_t nLen )
{
	unsigned char* pDataPtr = (unsigned char*)pData;

	const size_t nRound = nLen / DES_pBlockLength[ eLayer ];

	for( size_t i=0; i<nRound; ++i )
	{
		DES_pFnProcess[ eLayer ]( pDataPtr, pDataPtr );
		pDataPtr += DES_pBlockLength[ eLayer ];
	}
}
//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------
// Ԥ�������һ����������ռ��С
size_t DES_GetEncryptedSize( DES_Layer eLayer, const size_t nLen )
{
	const size_t nFinalLen = nLen + sizeof(size_t);

	size_t nRound = nFinalLen / DES_pBlockLength[ eLayer ];
	const size_t nPad = nFinalLen % DES_pBlockLength[ eLayer ];

	if( nPad > 0 )
	{
		++nRound;
	}

	return nRound * DES_pBlockLength[ eLayer ];
}
//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------
// ����һ������
/*
param : eLayer	���ܷ�ʽ
pKey	Key
OUT pOutData	�������
OUT	nOutLen		������ĳ���
pInData			��Ҫ���ܵ�����
nInLen			���ĳ���
return : ��������ĳ���

//-------------------------------------------------------------------------------
Warning!!! ���ıض���������ͬ��С�������������ģ����ɸ���һ��С�����ĵļ��ܿռ䡣
//-------------------------------------------------------------------------------
*/		
size_t DES_Encrypt( DES_Layer eLayer, const unsigned char* pKey, void* pOutData, const size_t nOutLen, const void* pInData, const size_t nInLen )
{
	const size_t nFinalLen = nInLen + sizeof(size_t);

	size_t nRound = nFinalLen / DES_pBlockLength[ eLayer ];
	const size_t nPad = nFinalLen % DES_pBlockLength[ eLayer ];

	if( nPad > 0 )
	{
		++nRound;
	}

	const size_t nRoundUpSize = nRound * DES_pBlockLength[ eLayer ];;

	if( nOutLen < nRoundUpSize )
		return 0;

	DES_pFnKey[ eLayer ]( (unsigned char*)pKey, EN0 );

	*( (size_t*)pOutData ) = nInLen;

	memcpy( (size_t*)pOutData + 1, pInData, nInLen );

	memset( (char*)pOutData + nFinalLen, 0, nPad );

	DES_Process( eLayer, pOutData, nOutLen );

	return nOutLen;
}
//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------
// ����һ������
/*
param : eLayer	���ܷ�ʽ
pKey	Key
pData	��Ҫ���ܵ��ļ� / ͬʱ�����ܺ������Ҳ�� OUT ����
nInLen	��Ҫ���ܵ��ļ�����
return : ���ܺ��ļ�����
*/
size_t DES_Decrypt( DES_Layer eLayer, const unsigned char* pKey, void* pData, const size_t nInLen )
{
	if( ( nInLen % DES_pBlockLength[ eLayer ] ) != 0 )
		return 0;

	DES_pFnKey[ eLayer ]( (unsigned char*)pKey, DE1 );

	DES_Process( eLayer, pData, nInLen );

	const size_t nOutLen = *( (size_t*)pData );
	if( nOutLen > nInLen )
		return 0;

	memcpy( pData, (size_t*)pData + 1, nOutLen );

	return nOutLen;
}
//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------
