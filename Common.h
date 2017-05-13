#ifndef Common_h

#define	Common_h

typedef	unsigned char		U8;
typedef	signed char		S8;
typedef	unsigned short int	U16;
typedef	signed short int	S16;
typedef	unsigned long		U32;
typedef	signed long		S32; 

// -- Control debug code generation
//#define	_Debug_

// ============= Macro definitions ===========================

#define	BitSet(word, bit_mask)		((word) |= (bit_mask))
#define	BitClr(word, bit_mask)		((word) &= ~(bit_mask))
#define	BitTest(word, bit_mask)		((word) & (bit_mask))
#define	BitAnd(word, bit_mask)		((word) &= (bit_mask))
#define	BitOr(word, bit_mask)		((word) |= (bit_mask))
#define	BitXor(word, bit_mask)		((word) ^= (bit_mask))
#define BitClrIfSet(word, bit_mask)	(((word) & (bit_mask)) \
						? BitClr((word), (bit_mask)), 1 \
						: 0)						
#define	Port_BitSet(port, bit_mask) 	(port->BSRR = bit_mask)
#define	Port_BitClr(port, bit_mask) 	(port->BRR = bit_mask)

#define	SetToLow(port, bit_mask)	(port->BRR = bit_mask)		
#define	SetToHigh(port, bit_mask)	(port->BSRR = bit_mask)		


// ===========================================================
//	Function Prototype Declarations
// ===========================================================
//
void	Delay(U16 count);

#endif // Common_h 

