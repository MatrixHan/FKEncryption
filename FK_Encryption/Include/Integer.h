/********************************************************************
// FileName: e:\Code\FK_3D\FK_Encryption\FK_Encryption\Include\Integer.h
// Date:     2007��11��21��
// Author:   FreeKnightDuzhi
// Comment:  
*********************************************************************/

#pragma once

#include <vector>


typedef unsigned short		IntHUnit;
typedef unsigned int		IntUnit;
typedef unsigned __int64	IntDUnit;

const unsigned int	IntUnitSize		= sizeof(IntUnit);							// һ�������͵�Ԫ( �޷������� )���󳤶�
const unsigned int	IntUnitBits		= IntUnitSize * 8;							// һ�������͵�Ԫ( �޷������� )�����ֽ���
const unsigned int	IntUnitMask		= 0xFFFFFFFF;								// һ�������͵�Ԫ( �޷������� )�����ֽ�����
const unsigned int	IntUnits32		= 4 / sizeof(IntUnit);						// һ�������͵�Ԫ( �޷������� )������ 32 λ���ϱ�׼��Ԫ����
const unsigned int	IntUnitsMin		= ( IntUnits32 > 2 ) ? IntUnits32 : 2;		// һ�������͵�Ԫ( �޷������� )������С��׼��Ԫ����

/*
	��������
	-----------------------------------------------------------------------------------------------------------------------------------------------------------------------
	˵����
	1��һ����������һ������͵�Ԫ�Լ�һ�����ű�־����ɡ�
	2��һ�������͵ĵ�Ԫ�������У�Խ�����Ԫ�أ�λԽ�ߡ�
	   ���� vector[0] = 0x00000010 vector[1] = 0x11111101 ��ô��������͵�ֵΪ 0x1111110100000010
	-----------------------------------------------------------------------------------------------------------------------------------------------------------------------
*/
class Integer
{
public:
	// ������־
	enum Sign
	{
		Positive,
		Negative,
	};

public:
	//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------
	// ����/��������
	//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------
	Integer();
	Integer( const Integer& rOther );
	Integer( const unsigned int nValue );
	Integer( const int nValue );
	~Integer();
	//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------
	// ���ز�����
	//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------
	const Integer& operator = ( const Integer& rOther );
	const Integer& operator = ( const unsigned int nValue );
	const Integer& operator = ( const int nValue );

	bool operator ! () const;

	bool operator == ( const Integer& rOther );
	bool operator != ( const Integer& rOther );

	const Integer operator + ( const Integer& rOther ) const;
	void operator += ( const Integer& rOther );

	const Integer operator - ( const Integer& rOther ) const;
	void operator -= ( const Integer& rOther );

	const Integer operator * ( const Integer& rOther ) const;
	void operator *= ( const Integer& rOther );

	const Integer operator / ( const Integer& rOther ) const;
	void operator /= ( const Integer& rOther );

	const Integer operator % ( const Integer& rOther ) const;
	void operator %= ( const Integer& rOther );

	const Integer& operator ++ ();
	const Integer operator ++ ( int );
	const Integer& operator -- ();
	const Integer operator -- ( int );

	//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------
	// ���ù��ܺ���
	//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------
	// ��һ��������ת��Ϊ unsigned int ����
	// return : ��ת��ʧ�����ɻ᷵�� 0
	operator unsigned int () const;
	// ��һ��������ת��Ϊ int ����
	// return : ��ת��ʧ�����ɻ᷵�� 0
	operator int () const;
	// �������㣨��������Ϊ����������
	/*
		param: OUT Remainder ����
		param: OUT Quotient  ��
		param: IN  Divisor	 ��������ĸ
	*/
	void Divide( Integer& Remainder, Integer& Quotient, const Integer& Divisor );
	// �ж�һ���������ܷ�ת��Ϊ unsigned int ����
	bool CanConvertToUInt() const;
	// �ж�һ���������ܷ�ת��Ϊ int ����
	bool CanConvertToInt() const;
	// �Ƚ�����������������������Ϊ������
	int PositiveCompare( const Integer& rOther ) const;
	// �Ƚ�����������
	/*
		return : 1 ������������Ŀ�������
				 0 ������������ȫ���
				-1 ��������СĿ�������
	*/
	int Compare( const Integer& rOther ) const;
	// ��һ����������Ϊ 0
	void SetZero();
	// �ж�һ���������Ƿ�Ϊ 0
	bool IsZero() const { return !*this; }
	// �ж�һ���������Ƿ�Ϊ 0
	bool NotZero() const { return !IsZero(); }
	// ��һ��������ȡ��
	void Negate();
	// �ж�һ���������Ƿ��Ǹ���
	bool IsNegative() const { return ( mSign == Negative ); }
	// �ж�һ���������Ƿ��Ǹ���
	bool NotNegative() const { return ( mSign != Negative ); }
	// �ж�һ���������Ƿ�������
	bool IsPositive() const { return ( mSign == Positive ); }
	// �ж�һ���������Ƿ�������
	bool NotPositive() const { return ( mSign != Positive ); }
	// ȡһ���������ľ���ֵ
	Integer AbsoluteValue() const;

public:
	//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------
	// ��������Ԫ��������
	//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------
	// ��չ/������ һ�������͵�Ԫ����
	void _SetUnits( const unsigned int nCount );
	// ��չһ�������͵�Ԫ������������ֵ���䡣���ú����������������͵�Ԫ������
	void _SetUnitsZeroPadding( const unsigned int nCount );
	// ��չ/������ һ�������͵�Ԫ�����������ô����͸���ԪΪ 0
	void _SetUnitsZero( const unsigned int nCount );
	// ��ȡ�����͵�Ԫ����
	unsigned int _GetUnits() const;
	// ��ȡ��һ���� 0 �Ĵ����͵�Ԫ���
	unsigned int _GetUnitsNotZero() const;

public:
	Sign							mSign;					// ����������
	std::vector<IntUnit>		mArrayUnits;				// ��������������Ԫ
};
//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------
// �Ƚ������޷������ͣ�ͨ����;�ǱȽ����������ͳ���
__inline const unsigned int Max( const unsigned int A, const unsigned int B )
{
	return ( A > B ) ? A : B;
}
// ����һ�������͵�Ԫ
__inline void CopyUnits( IntUnit* pA, const IntUnit* pB, unsigned int nUnits )
{
	memcpy( pA, pB, nUnits * sizeof(IntUnit) );
}
// ���һ�������͵�Ԫ
__inline void SetUnits( IntUnit* pData, IntUnit nValue, unsigned int nUnits )
{
	for( unsigned int i=0; i<nUnits; i++ )
		pData[i] = nValue;
}
//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------
// ˫����������ʵ�ʼ��㷽��
//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------
extern void _Add( Integer& Sum, const Integer& A, const Integer& B );
extern void _Subtract( Integer& Diff, const Integer& A, const Integer& B );
extern void _Mutiply( Integer& Product, const Integer& A, const Integer& B );
// ���������ͳ�������
/*
	param: OUT Remainder ����
	param: OUT Quotient  ��
	param: IN  Dividend  ������������
	param: IN  Divisor	 ��������ĸ
*/
extern void _Divide( Integer& Remainder, Integer& Quotient, const Integer& Dividend, const Integer& Divisor );
//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------
// ˫�����������ļ��㷽������Ӧ�����ã�
//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------
extern void _PositiveAdd( Integer& Sum, const Integer& A, const Integer& B );
extern void _PositiveSubtract( Integer& Diff, const Integer& A, const Integer& B );
extern void _PositiveMutiply( Integer& Product, const Integer& A, const Integer& B );
extern void _PositiveDivide( Integer& Remainder, Integer& Quotient, const Integer& A, const Integer& B );
//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------
// ˫�����������ļ��㷽������Ӧ�����ã�
//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------
// ������������Ԫ�����ĳ˷��������������ܲ��ȳ���
/*
		param: OUT pResult	������
					pTemp	��ʱ�ļ���ռ�
					pA		Դ��Ԫ����A
					nUnitsA Դ��Ԫ����A����
					pB		Դ��Ԫ����B
					nUnitsB Դ��Ԫ����B����
*/
extern void _AsymmetricMultiply( IntUnit* pResult, IntUnit* pTemp, const IntUnit* pA, unsigned int nUnitsA, const IntUnit* pB, unsigned int nUnitsB );
extern IntUnit _LinearMultiply( IntUnit* pC, const IntUnit* pA, IntUnit B, unsigned int N );
// ������������Ԫ�����ĵݹ�˷������������ȳ���
extern void _RecursiveMultiply( IntUnit* pResult, IntUnit* pTemp, const IntUnit* pA, const IntUnit* pB, unsigned int nUnits );
// ƽ��
extern void _RecursiveSquare( IntUnit* pResult, IntUnit* pTemp, const IntUnit* pA, unsigned int nUnits );

extern void _Divide( IntUnit* pRem, IntUnit* pQuot, IntUnit* pTemp, const IntUnit* pA, unsigned int nUnitsA, const IntUnit* pB, unsigned int nUnitsB );
extern IntUnit _DivideThreeUnitsByTwo( IntUnit* A, IntUnit B0, IntUnit B1 );
// 4����Ԫ����2����Ԫ�Ĵ��������������2����Ԫ�Ĵ�����
extern unsigned int _DivideFourUnitsByTwo( IntUnit* T, const unsigned int& Al, const unsigned int& Ah, const unsigned int& B );
// ��������������
extern void _AtomicDivide( IntUnit* pQ, const IntUnit* pA, const IntUnit* pB );
// ���������
extern void _CorrectQuotientEstimate( IntUnit* pRem, IntUnit* pTemp, IntUnit* pQuot, const IntUnit* pB, unsigned int nUnits );

// ��׼�ӷ�����һ�� unsigned __int64 Ϊ������λ�����㣩
extern int _DefaultAdd( IntUnit* pC, const IntUnit* pA, const IntUnit* pB, const unsigned int nUnits );
// ��׼��������һ�� unsigned __int64 Ϊ������λ�����㣩
extern int _DefaultSubtract( IntUnit* pC, const IntUnit* pA, const IntUnit* pB, const unsigned int nUnits );

extern void _DefaultMultiply2( IntUnit* pResult, const IntUnit* pA, const IntUnit* pB );
extern void _DefaultMultiply4( IntUnit* pResult, const IntUnit* pA, const IntUnit* pB );
extern void _DefaultMultiply8( IntUnit* pResult, const IntUnit* pA, const IntUnit* pB );
extern void _DefaultMultiply16( IntUnit* pResult, const IntUnit* pA, const IntUnit* pB );

extern void _DefaultSquare2( IntUnit* pResult, const IntUnit* pA );
extern void _DefaultSquare4( IntUnit* pResult, const IntUnit* pA );
extern void _DefaultSquare8( IntUnit* pResult, const IntUnit* pA );
extern void _DefaultSquare16( IntUnit* pResult, const IntUnit* pA );
