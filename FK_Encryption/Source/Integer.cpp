#include "../Include/Integer.h"
#include "../Inline/Integer.inl"


//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------
Integer::Integer()
	: mSign		(Positive)
{
	_SetUnitsZero( IntUnitsMin );
}
//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------
Integer::Integer( const Integer& rOther )
{
	*this = rOther;
}
//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------
Integer::Integer( const unsigned int nValue )
{
	*this = nValue;
}
//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------
Integer::Integer( const int nValue )
{
	*this = nValue;
}
//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------
Integer::~Integer()
{
}
//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------
const Integer& Integer::operator = ( const Integer& rOther )
{
	mSign = rOther.mSign;

	const unsigned int nUnits = rOther._GetUnits();
	_SetUnits( nUnits );
	memcpy( &mArrayUnits[0], &rOther.mArrayUnits[0], sizeof(IntUnit) * nUnits );

	return *this;
}
//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------
const Integer& Integer::operator = ( const unsigned int nValue )
{
	_SetUnitsZero( IntUnitsMin );

	for( unsigned int i=0; i<IntUnits32; ++i )
	{
		mArrayUnits[i] = (IntUnit)( ( nValue >> ( IntUnitBits * ( IntUnits32 - i + 1 ) ) ) & IntUnitMask );
	}

	return *this;
}
//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------
const Integer& Integer::operator = ( const int nValue )
{
	_SetUnitsZero( IntUnitsMin );

	unsigned int nUnsignedValue;

	if( nValue >= 0 )
	{
		mSign = Positive;
		nUnsignedValue = (unsigned int)nValue;
	}
	else
	{
		mSign = Negative;
		nUnsignedValue = (unsigned int)-nValue;
	}

	for( unsigned int i=0; i<IntUnits32; ++i )
	{
		mArrayUnits[i] = (IntUnit)( ( nUnsignedValue >> ( IntUnitBits * ( IntUnits32 - i + 1 ) ) ) & IntUnitMask );
	}

	return *this;
}
//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------
bool Integer::operator == ( const Integer& rOther )
{
	if( mSign != rOther.mSign )
		return false;

	const unsigned int nUnits = rOther._GetUnits();
	if( nUnits != rOther._GetUnits() )
		return false;

	if( _Compare( &mArrayUnits[0], &rOther.mArrayUnits[0], nUnits ) != 0 )
		return false;

	return true;
}
//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------
bool Integer::operator ! () const
{
	if ( IsNegative() )
	{
		return false;
	}
	if ( mArrayUnits[0] != 0 )
	{
		return false;
	}
	if ( _GetUnitsNotZero() != 0 )
	{
		return false;
	}
	return true;
}
//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------
bool Integer::operator != ( const Integer& rOther )
{
	return !( *this == rOther );
}
//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------
const Integer Integer::operator + ( const Integer& rOther ) const
{
	// ��ʼ��Sum�����ͣ�ȷ��������͵�Ԫ�㹻
	Integer Sum( Max( _GetUnits(), rOther._GetUnits() ) );
	_Add( Sum, *this, rOther );
	return Sum;
}
//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------
void Integer::operator += ( const Integer& rOther )
{
	_SetUnitsZeroPadding( Max( _GetUnits(), rOther._GetUnits() ) );
	_Add( *this, *this, rOther );
}
//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------
const Integer Integer::operator - ( const Integer& rOther ) const
{
	Integer Diff( Max( _GetUnits(), rOther._GetUnits() ) );
	_Subtract( Diff, *this, rOther );
	return Diff;
}
//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------
void Integer::operator -= ( const Integer& rOther )
{
	_SetUnitsZeroPadding( Max( _GetUnits(), rOther._GetUnits() ) );
	_Subtract( *this, *this, rOther );
}
//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------
const Integer Integer::operator * ( const Integer& rOther ) const
{
	Integer Product;
	_Mutiply( Product, *this, rOther );
	return Product;
}
//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------
void Integer::operator *= ( const Integer& rOther )
{
	_Mutiply( *this, *this, rOther );
}
//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------
const Integer Integer::operator / ( const Integer& rOther ) const
{
	Integer Remainder, Quotient;
	_Divide( Remainder, Quotient, *this, rOther );
	return Quotient;
}
//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------
void Integer::operator /= ( const Integer& rOther )
{
	Integer Remainder, Quotient;
	_Divide( Remainder, Quotient, *this, rOther );
	*this = Quotient;
}
//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------
const Integer Integer::operator % ( const Integer& rOther ) const
{
	Integer Remainder, Quotient;
	_Divide( Remainder, Quotient, *this, rOther );
	return Remainder;
}
//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------
void Integer::operator %= ( const Integer& rOther )
{
	Integer Remainder, Quotient;
	_Divide( Remainder, Quotient, *this, rOther );
	*this = Remainder;
}
//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------
const Integer& Integer::operator ++ ()
{
	if( NotNegative() )
	{
		if( _Increment( &mArrayUnits[0], _GetUnits() ) )
		{
			_SetUnitsZeroPadding( 2 * _GetUnits() );
			mArrayUnits[ _GetUnits() / 2 ] = 1;
		}
	}
	else
	{
		_Decrement( &mArrayUnits[0], _GetUnits() );

		if( _GetUnitsNotZero() == 0 )
			_SetUnitsZero( 2 );
	}

	return *this;
}
//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------
const Integer Integer::operator ++ ( int )
{
	Integer Temp = *this;
	++*this;
	return Temp;
}
//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------
const Integer& Integer::operator -- ()
{
	if( IsNegative() )
	{
		if( _Increment( &mArrayUnits[0], _GetUnits() ) )
		{
			_SetUnitsZeroPadding( 2 * _GetUnits() );
			mArrayUnits[ _GetUnits() / 2 ] = 1;
		}
	}
	else
	{
		if( _Decrement( &mArrayUnits[0], _GetUnits() ) )
		{
			_SetUnitsZero( 2 );
			mArrayUnits[0] = 1;
			mSign = Negative;
		}
	}

	return *this;
}
//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------
const Integer Integer::operator -- ( int )
{
	Integer Temp = *this;
	--*this;
	return Temp;
}
//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------
Integer::operator unsigned int () const
{
	if( !CanConvertToUInt() )
	{
		throw;
		return 0;
	}

	return *(unsigned int*)&mArrayUnits[0];
}
//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------
Integer::operator int () const
{
	if( _GetUnitsNotZero() > sizeof(unsigned int) / sizeof(IntUnit) )
	{
		throw;
		return 0;
	}

	const unsigned int nAbsoluteValue = *(unsigned int*)&mArrayUnits[0];
	if( nAbsoluteValue > ( IntUnitMask >> 1 ) )
	{
		throw;
		return 0;
	}

	return ( mSign == Positive ) ? (int)nAbsoluteValue : -(int)nAbsoluteValue;
}
//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------
void Integer::Divide( Integer& Remainder, Integer& Quotient, const Integer& Divisor )
{
	_Divide( Remainder, Quotient, *this, Divisor );
}
//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------
// �ж�һ���������ܷ�ת��Ϊ unsigned int ����
bool Integer::CanConvertToUInt() const
{
	if( mSign == Positive && _GetUnitsNotZero() <= sizeof(unsigned int) / sizeof(IntUnit) )
		return true;

	return false;
}
//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------
// �ж�һ���������ܷ�ת��Ϊ int ����
bool Integer::CanConvertToInt() const
{
	if( _GetUnitsNotZero() > sizeof(unsigned int) / sizeof(IntUnit) )
		return false;

	// �����λ��Ϊ0��1���򲻿�ת��Ϊ int 
	const unsigned int nAbsoluteValue = *(unsigned int*)&mArrayUnits[0];
	if( nAbsoluteValue > ( IntUnitMask >> 1 ) )
		return false;

	return true;
}
//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------
// �Ƚ�����������������������Ϊ������
int Integer::PositiveCompare( const Integer& rOther ) const
{
	unsigned int nUnitsNotZeroThis = _GetUnitsNotZero();
	unsigned int nUnitsNotZeroOther = rOther._GetUnitsNotZero();

	if( nUnitsNotZeroThis == nUnitsNotZeroOther )
	{
		return _Compare( &mArrayUnits[0], &rOther.mArrayUnits[0], nUnitsNotZeroThis );
	}
	else
	{
		return nUnitsNotZeroThis > nUnitsNotZeroOther ? 1 : -1;
	}
}
//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------
// �Ƚ�����������
/*
	return :	1 ������������Ŀ�������
				0 ������������ȫ���
				-1 ��������СĿ�������
*/
int Integer::Compare( const Integer& rOther ) const
{
	if( NotNegative() )
	{
		if( rOther.NotNegative() )
			return PositiveCompare( rOther );
		else
			return 1;
	}
	else
	{
		if( rOther.NotNegative() )
			return -1;
		else
			return -PositiveCompare( rOther );
	}
}
//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------
// ��һ����������Ϊ 0
void Integer::SetZero()
{
	const unsigned int nUnits = _GetUnits();
	for( unsigned int i=0; i<nUnits; ++i )
	{
		mArrayUnits[i] = 0;
	}

	mSign = Positive;
}
//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------
// ȡ��
void Integer::Negate()
{
	mSign = ( mSign == Positive ) ? Negative : Positive;
}
//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------
// ȡһ�������͵ľ���ֵ
Integer Integer::AbsoluteValue() const
{
	Integer Result( *this );
	Result.mSign = Positive;
	return Result;
}
//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------

//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------
// ��չ/������ һ�������͵�Ԫ����
void Integer::_SetUnits( const unsigned int nCount )
{
	mArrayUnits.resize( nCount );
}
//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------
// ��չһ�������͵�Ԫ������������ֵ���䡣���ú����������������͵�Ԫ������
void Integer::_SetUnitsZeroPadding( const unsigned int nCount )
{
	const unsigned int nOldSize = (unsigned int)mArrayUnits.size();
	if( nCount > nOldSize )
	{
		mArrayUnits.resize( nCount );
		memset( &mArrayUnits[ nOldSize ], 0, ( nCount - nOldSize ) * sizeof(IntUnit) );
	}
}
//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------
// ��չ/������ һ�������͵�Ԫ�����������ô����͸���ԪΪ 0
void Integer::_SetUnitsZero( const unsigned int nCount )
{
	mArrayUnits.resize( nCount );
	memset( &mArrayUnits[0], 0, nCount * sizeof(IntUnit) );
	mSign = Positive;
}
//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------
// ��ȡ�����͵�Ԫ����
unsigned int Integer::_GetUnits() const
{
	return (unsigned int)mArrayUnits.size();
}
//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------
// ��ȡ��һ���� 0 �Ĵ����͵�Ԫ���
unsigned int Integer::_GetUnitsNotZero() const
{
	unsigned int nUnitsNotZero = _GetUnits();
	if( nUnitsNotZero != 0 )
	{
		while( mArrayUnits[ nUnitsNotZero - 1 ] == 0 )
			--nUnitsNotZero;
	}

	return nUnitsNotZero;
}
//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------

//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------
// ���������͵ļӷ�����
void _Add( Integer& Sum, const Integer& A, const Integer& B )
{
	if( A.NotNegative() )
	{
		if( B.NotNegative() )
			_PositiveAdd( Sum, A, B );
		else
			_PositiveSubtract( Sum, A, B );
	}
	else
	{
		if( B.NotNegative() )
		{
			_PositiveSubtract( Sum, B, A );
		}
		else
		{
			_PositiveAdd( Sum, A, B );
			Sum.mSign = Integer::Negative;
		}
	}
}
//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------
// ���������ͼ�������
void _Subtract( Integer& Diff, const Integer& A, const Integer& B )
{
	if( A.NotNegative() )
	{
		if( B.NotNegative() )
			_PositiveSubtract( Diff, A, B );
		else
			_PositiveAdd( Diff, A, B );
	}
	else
	{
		if( B.NotNegative() )
		{
			_PositiveAdd( Diff, B, A );
			Diff.mSign = Integer::Negative;
		}
		else
		{
			_PositiveSubtract( Diff, A, B );
		}
	}
}
//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------
// ���������ͳ˷�����
void _Mutiply( Integer& Product, const Integer& A, const Integer& B )
{
	// ��ʱ���Է��Ž��м���
	_PositiveMutiply( Product, A, B );

	// �����������ȡ��
	if( A.NotNegative() != B.NotNegative() )
		Product.Negate();
}
//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------
// ���������ͳ�������
/*
	param: OUT Remainder ����
	param: OUT Quotient  ��
	param: IN  Dividend  ������������
	param: IN  Divisor	 ��������ĸ
*/
void _Divide( Integer& Remainder, Integer& Quotient, const Integer& Dividend, const Integer& Divisor )
{
	_PositiveDivide( Remainder, Quotient, Dividend, Divisor );

	if( Dividend.IsNegative() )
	{
		Quotient.Negate();
		if( Remainder.NotZero() )
		{
			--Quotient;
			Remainder = Divisor.AbsoluteValue() - Remainder;
		}
	}

	if( Divisor.IsNegative() )
		Quotient.Negate();
}
//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------

//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------

const unsigned int nRecursionLimit = 16;

typedef void (*FnMultiply)( IntUnit* pC, const IntUnit* pA, const IntUnit* pB );
typedef void (*FnSquare)( IntUnit* pC, const IntUnit* pA);

static FnMultiply	gs_pFnMultiply[9] =
{
	_DefaultMultiply2,
	_DefaultMultiply4,
	_DefaultMultiply8,
	0,
	_DefaultMultiply16,
	0, 0, 0, 0
};

static FnSquare		gs_pFnSquare[9] =
{
	_DefaultSquare2,
	_DefaultSquare4,
	_DefaultSquare8,
	0,
	_DefaultSquare16,
	0, 0, 0, 0
};

//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------
// �����ӷ����üӷ�����ȫ���Դ������ķ�����Ϣ��
void _PositiveAdd( Integer& Sum, const Integer& A, const Integer& B )
{
	int nCarry;
	const unsigned int nUnitsA = A._GetUnits();
	const unsigned int nUnitsB = B._GetUnits();

	if( nUnitsA == nUnitsB )
	{
		nCarry = _DefaultAdd( &Sum.mArrayUnits[0], &A.mArrayUnits[0], &B.mArrayUnits[0], nUnitsA );
	}
	else if( nUnitsA > nUnitsB )
	{
		nCarry = _DefaultAdd( &Sum.mArrayUnits[0], &A.mArrayUnits[0], &B.mArrayUnits[0], nUnitsB );
		CopyUnits( &Sum.mArrayUnits[0] + nUnitsB, &A.mArrayUnits[0] + nUnitsB, nUnitsA - nUnitsB );
		nCarry = _Increment( &Sum.mArrayUnits[0] + nUnitsB, nUnitsA - nUnitsB, nCarry );
	}
	else
	{
		nCarry = _DefaultAdd( &Sum.mArrayUnits[0], &A.mArrayUnits[0], &B.mArrayUnits[0], nUnitsA );
		CopyUnits( &Sum.mArrayUnits[0] + nUnitsA, &B.mArrayUnits[0] + nUnitsA, nUnitsB - nUnitsA );
		nCarry = _Increment( &Sum.mArrayUnits[0] + nUnitsA, nUnitsB - nUnitsA, nCarry );
	}

	if( nCarry != 0 )
	{
		Sum._SetUnitsZeroPadding( 2 * Sum._GetUnits() );
		Sum.mArrayUnits[ Sum._GetUnits() / 2 ] = 1;
	}

	Sum.mSign = Integer::Positive;
}
//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------
// �����������ü�������ȫ���Դ������ķ�����Ϣ��
void _PositiveSubtract( Integer& Diff, const Integer& A, const Integer& B )
{
	const unsigned int nUnitsA = A._GetUnits();
	const unsigned int nUnitsB = B._GetUnits();

	if( nUnitsA == nUnitsB )
	{
		if( _Compare( &A.mArrayUnits[0], &B.mArrayUnits[0], nUnitsA ) >= 0 )
		{
			_DefaultSubtract( &Diff.mArrayUnits[0], &A.mArrayUnits[0], &B.mArrayUnits[0], nUnitsA );
			Diff.mSign = Integer::Positive;
		}
		else
		{
			_DefaultSubtract( &Diff.mArrayUnits[0], &B.mArrayUnits[0], &A.mArrayUnits[0], nUnitsA );
			Diff.mSign = Integer::Negative;
		}
	}
	else if( nUnitsA > nUnitsB )
	{
		IntUnit nBorrow = _DefaultSubtract( &Diff.mArrayUnits[0], &A.mArrayUnits[0], &B.mArrayUnits[0], nUnitsB );
		CopyUnits( &Diff.mArrayUnits[0] + nUnitsB, &A.mArrayUnits[0] + nUnitsB, nUnitsA - nUnitsB );
		nBorrow = _Decrement( &Diff.mArrayUnits[0] + nUnitsB, nUnitsA - nUnitsB, nBorrow );
		Diff.mSign = Integer::Positive;
	}
	else
	{
		IntUnit nBorrow = _DefaultSubtract( &Diff.mArrayUnits[0], &B.mArrayUnits[0], &A.mArrayUnits[0], nUnitsA );
		CopyUnits( &Diff.mArrayUnits[0] + nUnitsA, &B.mArrayUnits[0] + nUnitsA, nUnitsB - nUnitsA );
		nBorrow = _Decrement( &Diff.mArrayUnits[0] + nUnitsA, nUnitsB - nUnitsA, nBorrow );
		Diff.mSign = Integer::Negative;
	}
}
//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------
// �����˷����ó˷�����ȫ���Դ������ķ�����Ϣ��
void _PositiveMutiply( Integer& Product, const Integer& A, const Integer& B )
{
	const unsigned int nRoundupSizeA = _RoundupSize( A._GetUnits() );		// Aʵ�ʷ���ռ䳤��
	const unsigned int nRoundupSizeB = _RoundupSize( A._GetUnits() );		// Bʵ�ʷ���ռ䳤��

	const unsigned int nRoundupSize = nRoundupSizeA + nRoundupSizeB;		// ���Ԥ�����ܳ���

	Product._SetUnitsZero( _RoundupSize( nRoundupSize ) );					// ���ʵ�ʷ���ռ䳤�ȣ����ÿռ��ʼ��Ϊ 0

	std::vector< IntUnit > vWorkspace;										// һ����ʱ�����ռ��С
	vWorkspace.resize( nRoundupSize );

	// ʵ�ʳ˷�����
	_AsymmetricMultiply( &Product.mArrayUnits[0], &vWorkspace[0], &A.mArrayUnits[0], nRoundupSizeA, &B.mArrayUnits[0], nRoundupSizeB );
}
//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------
// �����������ó�������ȫ���Դ������ķ�����Ϣ��
void _PositiveDivide( Integer& Remainder, Integer& Quotient, const Integer& A, const Integer& B )
{
	unsigned int nUnitsNotZeroA = A._GetUnitsNotZero();
	unsigned int nUnitsNotZeroB = B._GetUnitsNotZero();

	if( nUnitsNotZeroB == 0 )
	{
		throw;
		return;
	}

	if( A.PositiveCompare( B ) == -1 )
	{
		Remainder = A;
		Remainder.mSign = Integer::Positive;
		Quotient._SetUnitsZero( 2 );
		return;
	}

	nUnitsNotZeroA += nUnitsNotZeroA % 2;	
	nUnitsNotZeroB += nUnitsNotZeroB % 2;

	Remainder._SetUnitsZero( _RoundupSize( nUnitsNotZeroB ) );
	Remainder.mSign = Integer::Positive;

	Quotient._SetUnitsZero( _RoundupSize( nUnitsNotZeroA - nUnitsNotZeroB + 2 ) );
	Quotient.mSign = Integer::Positive;

	std::vector< IntUnit > vWorkspace;
	vWorkspace.resize( nUnitsNotZeroA + 3 * ( nUnitsNotZeroB + 2 ) );

	_Divide( &Remainder.mArrayUnits[0], &Quotient.mArrayUnits[0], &vWorkspace[0], &A.mArrayUnits[0], nUnitsNotZeroA, &B.mArrayUnits[0], nUnitsNotZeroB );
}
//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------

//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------
// ������������Ԫ�����ĳ˷�
/*
	param: OUT pResult	������
				pTemp	��ʱ�ļ���ռ�
				pA		Դ��Ԫ����A
				nUnitsA Դ��Ԫ����A����
				pB		Դ��Ԫ����B
				nUnitsB Դ��Ԫ����B����
*/
void _AsymmetricMultiply( IntUnit* pResult, IntUnit* pTemp, const IntUnit* pA, unsigned int nUnitsA, const IntUnit* pB, unsigned int nUnitsB )
{
	if( nUnitsA == nUnitsB )
	{
		if( pA == pB )
			_RecursiveSquare( pResult, pTemp, pA, nUnitsA );
		else
			_RecursiveMultiply( pResult, pTemp, pA, pB, nUnitsA );

		return;
	}

	if( nUnitsA > nUnitsB )
	{
		const IntUnit* pT = pA;
		pA = pB;
		pB = pT;

		const unsigned int nUnitsT = nUnitsA;
		nUnitsA = nUnitsB;
		nUnitsB = nUnitsT;
	}

	if( nUnitsA == 2 && pA[1] != 0 )
	{
		switch( pA[0] )
		{
		case 0:
			{
				SetUnits( pResult, 0, nUnitsB + 2 );
				return;
			}
		case 1:
			{
				CopyUnits( pResult, pB, nUnitsB );
				pResult[nUnitsB] = pResult[nUnitsB+1] = 0;
				return;
			}
		default:
			{
				pResult[nUnitsB] = _LinearMultiply( pResult, pB, pA[0], nUnitsB );
				pResult[nUnitsB+1] = 0;
				return;
			}
		}
	}

	if( ( nUnitsB / nUnitsA ) % 2 == 0 )
	{
		_RecursiveMultiply( pResult, pTemp, pA, pB, nUnitsA );
		CopyUnits( pTemp+2*nUnitsA, pResult+nUnitsA, nUnitsA );

		for( unsigned int i=2*nUnitsA; i<nUnitsB; i+=2*nUnitsA )
			_RecursiveMultiply( pTemp+nUnitsA+i, pTemp, pA, pB+i, nUnitsA );

		for( unsigned int i=nUnitsA; i<nUnitsB; i+=2*nUnitsA )
			_RecursiveMultiply( pResult+i, pTemp, pA, pB+i, nUnitsA );
	}
	else
	{
		for( unsigned int i=0; i<nUnitsB; i+=2*nUnitsA )
			_RecursiveMultiply( pResult+i, pTemp, pA, pB+i, nUnitsA );

		for( unsigned int i=nUnitsA; i<nUnitsB; i+=2*nUnitsA )
			_RecursiveMultiply( pTemp+nUnitsA+i, pTemp, pA, pB+i, nUnitsA );
	}

	if( _DefaultAdd( pResult+nUnitsA, pResult+nUnitsA, pTemp+2*nUnitsA, nUnitsB-nUnitsA ) )
		_Increment( pResult + nUnitsB, nUnitsA, 1 );
}
//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------
IntUnit _LinearMultiply( IntUnit* pC, const IntUnit* pA, IntUnit B, unsigned int N )
{
	IntUnit nCarry = 0;
	for( unsigned int i=0; i<N; ++i )
	{
		IntDUnit p = (IntDUnit)pA[i] * (IntDUnit)B;
		p += (IntDUnit)nCarry;
		pC[i] = LowUnit( p );
		nCarry = HighUnit( p );
	}

	return nCarry;
}
//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------
void _RecursiveMultiply( IntUnit* pResult, IntUnit* pTemp, const IntUnit* pA, const IntUnit* pB, unsigned int nUnits )
{
	if( nUnits <= nRecursionLimit )
	{
		gs_pFnMultiply[ nUnits / 4 ]( pResult, pA, pB );
	}
	else
	{
		const unsigned int nHalfUnits = nUnits / 2;

		const IntUnit* pA0 = pA;
		const IntUnit* pA1 = pA + nHalfUnits;
		const IntUnit* pB0 = pB;
		const IntUnit* pB1 = pB + nHalfUnits;

		IntUnit* pT0 = pTemp;
		IntUnit* pT1 = pTemp + nHalfUnits;
		IntUnit* pT2 = pTemp + nUnits;
		IntUnit* pT3 = pTemp + nUnits + nHalfUnits;

		IntUnit* pR0 = pResult;
		IntUnit* pR1 = pResult + nHalfUnits;
		IntUnit* pR2 = pResult + nUnits;
		IntUnit* pR3 = pResult + nUnits + nHalfUnits;

		const unsigned int nHalfUnitsA = _Compare( pA0, pA1, nHalfUnits ) > 0 ? 0 : nHalfUnits;
		_DefaultSubtract( pR0, pA + nHalfUnitsA, pA + ( nHalfUnits ^ nHalfUnitsA ), nHalfUnits );

		const unsigned int nHalfUnitsB = _Compare( pB0, pB1, nHalfUnits ) > 0 ?  0 : nHalfUnits;
		_DefaultSubtract( pR1, pB + nHalfUnitsB, pB + ( nHalfUnits ^ nHalfUnitsB ), nHalfUnits );

		_RecursiveMultiply( pR2, pT2, pA1, pB1, nHalfUnits);
		_RecursiveMultiply( pT0, pT2, pR0, pR1, nHalfUnits );
		_RecursiveMultiply( pR0, pT2, pA0, pB0, nHalfUnits );

		// now pTemp[01] holds (pA1-pA0)*(pB0-pB1), pResult[01] holds pA0*pB0, pResult[23] holds pA1*pB1

		int c2 = _DefaultAdd( pR2, pR2, pR1, nHalfUnits );
		int c3 = c2;
		c2 += _DefaultAdd( pR1, pR2, pR0, nHalfUnits );
		c3 += _DefaultAdd( pR2, pR2, pR3, nHalfUnits );

		if( nHalfUnitsA == nHalfUnitsB )
			c3 -= _DefaultSubtract( pR1, pR1, pT0, nUnits );
		else
			c3 += _DefaultAdd( pR1, pR1, pT0, nUnits );

		c3 += _Increment( pR2, nHalfUnits, c2 );
		_Increment( pR3, nHalfUnits, c3 );
	}
}
//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------
void _RecursiveSquare( IntUnit* pResult, IntUnit* pTemp, const IntUnit* pA, unsigned int nUnits )
{
	if( nUnits <= nRecursionLimit )
	{
		gs_pFnSquare[ nUnits / 4 ]( pResult, pA );
	}
	else
	{
		const unsigned int nHalfUnits = nUnits / 2;

		const IntUnit* pA0 = pA;
		const IntUnit* pA1 = pA + nHalfUnits;

		IntUnit* pT0 = pTemp;
		IntUnit* pT2 = pTemp + nUnits;

		IntUnit* pR0 = pResult;
		IntUnit* pR1 = pResult + nHalfUnits;
		IntUnit* pR2 = pResult + nUnits;
		IntUnit* pR3 = pResult + nUnits + nHalfUnits;

		_RecursiveSquare( pR0, pT2, pA0, nHalfUnits );
		_RecursiveSquare( pR2, pT2, pA1, nHalfUnits );
		_RecursiveMultiply( pT0, pT2, pA0, pA1, nHalfUnits );

		int nCarry = _DefaultAdd( pR1, pR1, pT0, nUnits );
		nCarry += _DefaultAdd( pR1, pR1, pT0, nUnits );
		_Increment( pR3, nHalfUnits, nCarry );
	}
}
//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------

//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------
void _Divide( IntUnit* pRem, IntUnit* pQuot, IntUnit* pTemp, const IntUnit* pA, unsigned int nUnitsA, const IntUnit* pB, unsigned int nUnitsB )
{
	// ������ʱ�����ռ�
	IntUnit* const TA = pTemp;
	IntUnit* const TB = pTemp + nUnitsA + 2;
	IntUnit* const TP = pTemp + nUnitsA + 2 + nUnitsB;

	// ��B������TempB�в��ҽ��б�׼����TempB�����λ����Ϊ1
	unsigned nShiftUnits = ( pB[nUnitsB-1] == 0 );
	TB[0] = TB[nUnitsB-1] = 0;
	CopyUnits( TB + nShiftUnits, pB, nUnitsB - nShiftUnits );
	unsigned nShiftBits = IntUnitBits - _BitPrecision( TB[nUnitsB-1] );
	_ShiftUnitsLeftByBits( TB, nUnitsB, nShiftBits );

	// ��A������TempA�в��ҽ��б�׼��
	TA[0] = TA[nUnitsA] = TA[nUnitsA+1] = 0;
	CopyUnits( TA + nShiftUnits, pA, nUnitsA );
	_ShiftUnitsLeftByBits( TA, nUnitsA+2, nShiftBits );

	if( TA[nUnitsA+1] == 0 && TA[nUnitsA] <= 1 )
	{
		pQuot[nUnitsA-nUnitsB+1] = pQuot[nUnitsA-nUnitsB] = 0;
		while( TA[nUnitsA] || _Compare( TA+nUnitsA-nUnitsB, TB, nUnitsB ) >= 0 )
		{
			TA[nUnitsA] -= _DefaultSubtract( TA+nUnitsA-nUnitsB, TA+nUnitsA-nUnitsB, TB, nUnitsB );
			++pQuot[nUnitsA-nUnitsB];
		}
	}
	else
	{
		nUnitsA += 2;
	}

	IntUnit BT[2];
	BT[0] = TB[nUnitsB-2] + 1;
	BT[1] = TB[nUnitsB-1] + ( BT[0] == 0 );

	for( unsigned int i=nUnitsA-2; i>=nUnitsB; i-=2 )
	{
		_AtomicDivide( pQuot+i-nUnitsB, TA+i-2, BT );
		_CorrectQuotientEstimate( TA+i-nUnitsB, TP, pQuot+i-nUnitsB, TB, nUnitsB );
	}

	// ���� TA �� pRem �У�������׼�����
	CopyUnits( pRem, TA+nShiftUnits, nUnitsB );
	_ShiftUnitsRightByBits( pRem, nUnitsB, nShiftBits );
}
//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------
// 3����Ԫ����2����Ԫ�Ĵ��������������2����Ԫ�Ĵ�����
IntUnit _DivideThreeUnitsByTwo( IntUnit* A, IntUnit B0, IntUnit B1 )
{
	// ���̽���������Ԥ���̵Ĵ�С
	IntUnit Q;
	if( B1+1 == 0 )
		Q = A[2];
	else
		Q = (IntUnit)( _MakeDoubleUnits( A[1], A[2] ) / (IntDUnit)( B1+1 ) );

	// ��A��ĸ�м�ȥQ*B ����*Ԥ������ 
	IntDUnit p = (IntDUnit)B0 * (IntDUnit)Q;
	IntDUnit u = (IntDUnit)A[0] - (IntDUnit)_GetLowUnit( p );
	A[0] = _GetLowUnit( u );
	u = (IntDUnit)A[1] - _GetHighUnit( p ) - _GetHighUnitAsBorrow( u ) - (IntDUnit)B1 * (IntDUnit)Q;
	A[1] = _GetLowUnit( u );
	A[2] += _GetHighUnit( u );

	// ��ΪԤ����QС���������̣�����������
	while( A[2] || A[1] > B1 || ( A[1]==B1 && A[0]>=B0 ) )
	{
		u = (IntDUnit)A[0] - (IntDUnit)B0;
		A[0] = _GetLowUnit( u );
		u = (IntDUnit)A[1] - (IntDUnit)B1 - _GetHighUnitAsBorrow( u );
		A[1] = _GetLowUnit( u );
		A[2] += _GetHighUnit( u );
		++Q;
	}

	return Q;
}
//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------
// 4����Ԫ����2����Ԫ�Ĵ��������������2����Ԫ�Ĵ�����
IntDUnit _DivideFourUnitsByTwo( IntUnit* T, const IntDUnit& Al, const IntDUnit& Ah, const IntDUnit& B )
{
	// ������Ϊ0���Ƿ������Ǽ������Ϊ1��ֱ�ӷ��ط�ĸ��
	if( B == 0 )
	{
		return _MakeDoubleUnits( _GetLowUnit( Ah ), _GetHighUnit( Ah ) );
	}
	else
	{
		IntUnit Q[2];
		T[0] = _GetLowUnit( Al );
		T[1] = _GetHighUnit( Al ); 
		T[2] = _GetLowUnit( Ah );
		T[3] = _GetHighUnit( Ah );
		Q[1] = _DivideThreeUnitsByTwo( T+1, _GetLowUnit( B ), _GetHighUnit( B ) );
		Q[0] = _DivideThreeUnitsByTwo( T, _GetLowUnit( B ), _GetHighUnit( B ) );
		return _MakeDoubleUnits( Q[0], Q[1] );
	}
}
//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------
// ��������������
void _AtomicDivide( IntUnit* pQ, const IntUnit* pA, const IntUnit* pB )
{
	IntUnit pTemp[4];
	const IntDUnit q = _DivideFourUnitsByTwo( pTemp, _MakeDoubleUnits( pA[0], pA[1] ), _MakeDoubleUnits( pA[2], pA[3] ), _MakeDoubleUnits( pB[0], pB[1] ) );
	pQ[0] = _GetLowUnit( q );
	pQ[1] = _GetHighUnit( q );
}
//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------
// ���������
void _CorrectQuotientEstimate( IntUnit* pRem, IntUnit* pTemp, IntUnit* pQuot, const IntUnit* pB, unsigned int nUnits )
{
	_AsymmetricMultiply( pTemp, pTemp+nUnits+2, pQuot, 2, pB, nUnits );

	_DefaultSubtract( pRem, pRem, pTemp, nUnits+2 );

	while( pRem[nUnits] != 0 || _Compare(pRem, pB, nUnits) >= 0 )
	{
		pRem[nUnits] -= _DefaultSubtract( pRem, pRem, pB, nUnits );
		pQuot[1] += ( ++pQuot[0] == 0 );
	}
}
//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------

//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------
// ��׼�ӷ�����һ�� unsigned __int64 Ϊ������λ�����㣩
int _DefaultAdd( IntUnit* pC, const IntUnit* A, const IntUnit* B, const unsigned int nUnits )
{
	IntDUnit u = 0;
	// ÿ�����ֽڽ���һ�ε�������
	for( unsigned int i=0; i<nUnits; i+=2 )
	{
		u = (IntDUnit)A[i] + (IntDUnit)B[i] + (IntDUnit)GetCarry( u );
		pC[i] = LowUnit( u );

		u = (IntDUnit)A[i+1] + (IntDUnit)B[i+1] + (IntDUnit)GetCarry( u );
		pC[i+1] = LowUnit( u );
	}

	return (int)GetCarry( u );
}
//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------
// ��׼��������һ�� unsigned __int64 Ϊ������λ�����㣩
int _DefaultSubtract( IntUnit* pC, const IntUnit* A, const IntUnit* B, const unsigned int nUnits )
{
	IntDUnit u = 0;
	// ÿ�����ֽڽ���һ�μ���.����
	for( unsigned int i=0; i<nUnits; i+=2 )
	{
		u = (IntDUnit)A[i] - (IntDUnit)B[i] - (IntDUnit)GetBorrow( u );
		pC[i] = LowUnit( u );

		u = (IntDUnit)A[i+1] - (IntDUnit)B[i+1] - (IntDUnit)GetBorrow( u );
		pC[i+1] = LowUnit( u );
	}

	return (int)GetBorrow( u );
}
//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------
void _DefaultMultiply2( IntUnit* pResult, const IntUnit* pA, const IntUnit* pB )
{
	Mul_2
}
//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------
void _DefaultMultiply4( IntUnit* pResult, const IntUnit* pA, const IntUnit* pB )
{
	Mul_4
}
//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------
void _DefaultMultiply8( IntUnit* pResult, const IntUnit* pA, const IntUnit* pB )
{
	Mul_8
}
//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------
void _DefaultMultiply16( IntUnit* pResult, const IntUnit* pA, const IntUnit* pB )
{
	Mul_16
}
//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------
void _DefaultSquare2( IntUnit* pResult, const IntUnit* pA )
{
	Squ_2
}
//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------
void _DefaultSquare4( IntUnit* pResult, const IntUnit* pA )
{
	Squ_4
}
//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------
void _DefaultSquare8( IntUnit* pResult, const IntUnit* pA )
{
	Squ_8
}
//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------
void _DefaultSquare16( IntUnit* pResult, const IntUnit* pA )
{
	Squ_16
}
//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------
