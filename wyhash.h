/*	Author: Wang Yi <godspeed_china@yeah.net>	*/
#ifndef wyhash_version_3
#define wyhash_version_3
#include	<stdint.h>
#include	<string.h>
#if defined(_MSC_VER) && defined(_M_X64)
#include <intrin.h>
#pragma	intrinsic(_umul128)
#endif
const	uint64_t	_wyp0=0xa0761d6478bd642full,	_wyp1=0xe7037ed1a0b428dbull,	_wyp2=0x8ebc6af09c88c6e3ull,	_wyp3=0x589965cc75374cc3ull,	_wyp4=0x1d8e4e27c47d124full;
static	inline	uint64_t	_wymum(uint64_t	A,	uint64_t	B) {
#ifdef __SIZEOF_INT128__
	__uint128_t	r=A;	r*=B;
	return	(r>>64)^r;
#elif	defined(_MSC_VER) && defined(_M_X64)
	A=_umul128(A, B, &B);
	return	A^B;
#else
	uint64_t	ha=A>>32,	hb=B>>32,	la=(uint32_t)A,	lb=(uint32_t)B,	hi, lo;
	uint64_t	rh=ha*hb,	rm0=ha*lb,	rm1=hb*la,	rl=la*lb,	t=rl+(rm0<<32),	c=t<rl;
	lo=t+(rm1<<32);	c+=lo<t;	hi=rh+(rm0>>32)+(rm1>>32)+c;	return hi^lo;
#endif
}
static	inline	uint64_t	_wymix0(uint64_t	A,	uint64_t	B,	uint64_t	seed) {	return	_wymum(A^seed^_wyp0,	B^seed^_wyp1);}
static	inline	uint64_t	_wymix1(uint64_t	A,	uint64_t	B,	uint64_t	seed) {	return	_wymum(A^seed^_wyp2,	B^seed^_wyp3);}
static	inline	uint64_t	_wyr1(const	uint8_t	*p) {	uint8_t v;	memcpy(&v,	p,	1);	return	v;}
static	inline	uint64_t	_wyr2(const	uint8_t	*p) {	uint16_t v;	memcpy(&v,	p,	2);	return	v;}
static	inline	uint64_t	_wyr4(const	uint8_t	*p) {	uint32_t v;	memcpy(&v,	p,	4);	return	v;}
static	inline	uint64_t	_wyr8(const	uint8_t	*p) {	return	(_wyr4(p)<<32)|_wyr4(p+4);}
static	inline	uint64_t	_wyr(const	uint8_t	*p,	unsigned	k){
	switch(k){
		case	1:	return	_wyr1(p);
		case	2:	return	_wyr2(p);
		case	3:	return	(_wyr2(p)<<8)|_wyr1(p+2);
		case	4:	return	_wyr4(p);
		case	5:	return	(_wyr4(p)<<8)|_wyr1(p+4);
		case	6:	return	(_wyr4(p)<<16)|_wyr2(p+4);
		case	7:	return	(_wyr4(p)<<24)|(_wyr2(p+4)<<8)|_wyr1(p+6);
		case	8:	return	_wyr8(p);
		default	:	return	0;
	}
}
static	inline	uint64_t	__wyr8(const	uint8_t	*p) {	uint64_t v;	memcpy(&v,	p,	8);	return	v;}
//to avoid attacks, seed should be initialized as a secret.
static	inline	uint64_t	wyhash(const void* key,	uint64_t	len,	uint64_t	seed) {
	const	uint8_t	*p=(const	uint8_t*)key;	uint64_t	see1=seed,	i=len;
	for(;	i>=32;	i-=32,	p+=32) {	seed=_wymix0(__wyr8(p),__wyr8(p+8),seed);	see1=_wymix1(__wyr8(p+16),__wyr8(p+24),see1);	}
	if(i<=8){	seed=_wymix0(_wyr(p,i),0,seed);	return	_wymum(seed^see1,len^_wyp4);	}
	if(i<=16){	seed=_wymix0(_wyr8(p),_wyr(p+8,i-8),seed);	return	_wymum(seed^see1,len^_wyp4);	}
	if(i<=24){	seed=_wymix0(_wyr8(p),_wyr8(p+8),seed);	see1=_wymix1(_wyr(p+16,i-16),0,see1);	return	_wymum(seed^see1,len^_wyp4);	}
	seed=_wymix0(_wyr8(p),_wyr8(p+8),seed);	see1=_wymix1(_wyr8(p+16),_wyr(p+24,i-24),see1);	return	_wymum(seed^see1,len^_wyp4);
}
static	inline	uint64_t	wyhash64(uint64_t	A, uint64_t	B) {	return	_wymum(_wymum(A^_wyp0,	B^_wyp1),	_wyp2);}
static	inline	double	wy2u01(uint64_t	r) {	const	double	_wynorm=1.0/(1ull<<52);	return	(r&0x000fffffffffffffull)*_wynorm;}
static	inline	float	wy2gau(uint64_t	r) {	const	float	_wynorm1=1.0f/(1ull<<15);	return	(((r>>16)&0xffff)+((r>>32)&0xffff)+(r>>48))*_wynorm1-3.0f;}
static	inline	uint64_t	wyrand(uint64_t	*seed) {	*seed+=_wyp0;	return	_wymum(*seed^_wyp1,*seed);}
static	uint64_t	_wyrand_seed=0;
#define	WYRAND_MAX	0xffffffffffffffffull
static	inline	void	wysrand(uint64_t	seed) {	_wyrand_seed=seed;}
static	inline	uint64_t	wygrand(void) {
	uint64_t s;
#if defined(_OPENMP)
	#pragma omp atomic capture
#endif
	{	_wyrand_seed += _wyp0;	s = _wyrand_seed;	}
	return	_wymum(s^_wyp1,s);
}
#endif
