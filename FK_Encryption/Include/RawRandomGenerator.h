/********************************************************************
// FileName: e:\Code\FK_3D\FK_Encryption\FK_Encryption\Include\RawRandomGenerator.h
// Date:     2007��11��27��
// Author:   FreeKnightDuzhi
// Comment:  
*********************************************************************/


#pragma once


//�������������
class RawRandomGenerator
{
public:
	RawRandomGenerator();

	//�����������
	void ResetSeed();

	//����������
	unsigned int MixSeed( const unsigned char* pMixData, unsigned int nMixDataLen );

	//��ȡ���������Ҫ�Ļ�������ֽ���
	unsigned int GetSeedMixBytesNeed() const;

	//���ݵ�ǰʱ�������������
	void SetSeedByTime();

	//����ָ�����ȵ��������
	bool GenerateBytes( unsigned char* pOutData, unsigned int nOutLen );

	//���ݵ�ǰʱ������ָ�����ȵ��������
	void GenerateBytesByTime( unsigned char* pOutData, unsigned int nOutLen );

protected:
	unsigned int	mnSeedMixBytesNeed;		//���������Ҫ�ֽ���
	unsigned char	mpSeedState[16];		//��Ϻ�����״̬
	unsigned int	mnOutputAvailable;		//ʣ����������������ֽ���
	unsigned char	mpOutput[16];			//����������
};
