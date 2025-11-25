// limits -- statics for numeric_num
#include <limits>

//	ASSUMES:
//	wraparound 2's complement integer arithmetic w/o traps
//	all CHAR_BITs of each byte used by integers
//	IEC559 (IEEE 754) floating-point arithmetic
//	floating-point errors can trap
//	tinyness detected before floating-point rounding

const float LOG2 = 0.30103;

		// statics for class _Num_base
const bool _Num_base::has_denorm = false;
const bool _Num_base::has_denorm_loss = false;
const bool _Num_base::has_infinity = false;
const bool _Num_base::has_quiet_NaN = false;
const bool _Num_base::has_signaling_NaN = false;
const bool _Num_base::is_bounded = false;
const bool _Num_base::is_exact = false;
const bool _Num_base::is_iec559 = false;
const bool _Num_base::is_integer = false;
const bool _Num_base::is_modulo = false;
const bool _Num_base::is_signed = false;
const bool _Num_base::is_specialized = false;
const bool _Num_base::tinyness_before = false;
const bool _Num_base::traps = false;
const float_round_style _Num_base::round_style =
	round_toward_zero;
const int _Num_base::digits = 0;
const int _Num_base::digits10 = 0;
const int _Num_base::max_exponent = 0;
const int _Num_base::max_exponent10 = 0;
const int _Num_base::min_exponent = 0;
const int _Num_base::min_exponent10 = 0;
const int _Num_base::radix = 0;

		// statics for class _Num_int_base
const bool _Num_int_base::is_bounded = true;
const bool _Num_int_base::is_exact = true;
const bool _Num_int_base::is_integer = true;
const bool _Num_int_base::is_modulo = true;
const bool _Num_int_base::is_specialized = true;
const int _Num_int_base::radix = 2;

		// statics for class _Num_float_base
const bool _Num_float_base::has_denorm = true;
const bool _Num_float_base::has_denorm_loss = true;
const bool _Num_float_base::has_infinity = true;
const bool _Num_float_base::has_quiet_NaN = true;
const bool _Num_float_base::has_signaling_NaN = true;
const bool _Num_float_base::is_bounded = true;
const bool _Num_float_base::is_exact = false;
const bool _Num_float_base::is_iec559 = true;
const bool _Num_float_base::is_integer = false;
const bool _Num_float_base::is_modulo = false;
const bool _Num_float_base::is_signed = true;
const bool _Num_float_base::is_specialized = true;
const bool _Num_float_base::tinyness_before = true;
const bool _Num_float_base::traps = true;
const float_round_style _Num_float_base::round_style =
	round_to_nearest;
const int _Num_float_base::radix = FLT_RADIX;

		// statics for class numeric_limits<char>
const bool numeric_limits<char>::is_signed = CHAR_MIN < 0;
const int numeric_limits<char>::digits =
	CHAR_BIT - (CHAR_MIN < 0 ? 1 : 0);
const int numeric_limits<char>::digits10 = LOG2 *
	(CHAR_BIT - (CHAR_MIN < 0 ? 1 : 0));


		// statics for class numeric_limits<_Bool>
const bool numeric_limits<_Bool>::is_signed = false;
const int numeric_limits<_Bool>::digits = 1;
const int numeric_limits<_Bool>::digits10 = 0;

		// statics for class numeric_limits<signed char>
const bool numeric_limits<signed char>::is_signed = true;
const int numeric_limits<signed char>::digits =
	CHAR_BIT - 1;
const int numeric_limits<signed char>::digits10 = LOG2 *
	(CHAR_BIT - 1);

		// statics for class numeric_limits<unsigned char>
const bool numeric_limits<unsigned char>::is_signed = false;
const int numeric_limits<unsigned char>::digits =
	CHAR_BIT;
const int numeric_limits<unsigned char>::digits10 = LOG2 *
	(CHAR_BIT);

		// statics for class numeric_limits<short>
const bool numeric_limits<short>::is_signed = true;
const int numeric_limits<short>::digits =
	CHAR_BIT * sizeof (short) - 1;
const int numeric_limits<short>::digits10 = LOG2 *
	(CHAR_BIT * sizeof (short) - 1);

		// statics for class numeric_limits<unsigned short>
const bool numeric_limits<unsigned short>::is_signed = false;
const int numeric_limits<unsigned short>::digits =
	CHAR_BIT * sizeof (unsigned short);
const int numeric_limits<unsigned short>::digits10 = LOG2 *
	(CHAR_BIT * sizeof (unsigned short));

		// statics for class numeric_limits<int>
const bool numeric_limits<int>::is_signed = true;
const int numeric_limits<int>::digits =
	CHAR_BIT * sizeof (int) - 1;
const int numeric_limits<int>::digits10 = LOG2 *
	(CHAR_BIT * sizeof (int) - 1);

		// statics for class numeric_limits<unsigned int>
const bool numeric_limits<unsigned int>::is_signed = false;
const int numeric_limits<unsigned int>::digits =
	CHAR_BIT * sizeof (unsigned int);
const int numeric_limits<unsigned int>::digits10 = LOG2 *
	(CHAR_BIT * sizeof (unsigned int));

		// statics for class numeric_limits<long>
const bool numeric_limits<long>::is_signed = true;
const int numeric_limits<long>::digits =
	CHAR_BIT * sizeof (long) - 1;
const int numeric_limits<long>::digits10 = LOG2 *
	(CHAR_BIT * sizeof (long) - 1);

		// statics for class numeric_limits<unsigned long>
const bool numeric_limits<unsigned long>::is_signed = false;
const int numeric_limits<unsigned long>::digits =
	CHAR_BIT * sizeof (unsigned long);
const int numeric_limits<unsigned long>::digits10 = LOG2 *
	(CHAR_BIT * sizeof (unsigned long));

//		statics for class numeric_limits<float>
const int numeric_limits<float>::digits = FLT_MANT_DIG;
const int numeric_limits<float>::digits10 = FLT_DIG;
const int numeric_limits<float>::max_exponent = FLT_MAX_EXP;
const int numeric_limits<float>::max_exponent10 = FLT_MAX_10_EXP;
const int numeric_limits<float>::min_exponent = FLT_MIN_EXP;
const int numeric_limits<float>::min_exponent10 = FLT_MIN_10_EXP;

//		statics for class numeric_limits<double>
const int numeric_limits<double>::digits = DBL_MANT_DIG;
const int numeric_limits<double>::digits10 = DBL_DIG;
const int numeric_limits<double>::max_exponent = DBL_MAX_EXP;
const int numeric_limits<double>::max_exponent10 = DBL_MAX_10_EXP;
const int numeric_limits<double>::min_exponent = DBL_MIN_EXP;
const int numeric_limits<double>::min_exponent10 = DBL_MIN_10_EXP;

//		statics for class numeric_limits<long double>
const int numeric_limits<long double>::digits = LDBL_MANT_DIG;
const int numeric_limits<long double>::digits10 = LDBL_DIG;
const int numeric_limits<long double>::max_exponent =
	LDBL_MAX_EXP;
const int numeric_limits<long double>::max_exponent10 =
	LDBL_MAX_10_EXP;
const int numeric_limits<long double>::min_exponent =
	LDBL_MIN_EXP;
const int numeric_limits<long double>::min_exponent10 =
	LDBL_MIN_10_EXP;

/*
 * Copyright (c) 1995 by P.J. Plauger.  ALL RIGHTS RESERVED. 
 * Consult your license regarding permissions and restrictions.
 */
