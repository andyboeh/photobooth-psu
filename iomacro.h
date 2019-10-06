#ifndef IOMACRO_H
#define IOMACRO_H

#include <avr/io.h>
/**
 * \file iomacro.h
 * \brief I/O Macro Functions
 * Definitions of various macro functions used in various parts of the system,
 * especially concerning I/O ports.
 */

/** \def RESET(x)
 * A macro that resets the I/O port given in \a x.
 */
#define	RESET(x)		_XRS(x)
/** \def SET(x)
 * A macro that sets the I/O port given in \a x.
 */
#define	SET(x)			_XS(x)
/** \def TOGGLE(X)
 * A macro that toggles the state of the I/O port given in \a x.
 */
#define	TOGGLE(x)		_XT(x)
/** \def SET_OUTPUT(X)
 * A macro that sets the direction of the I/O port given in \a x to output.
 */
#define	SET_OUTPUT(x)	_XSO(x)
/** \def SET_INPUT(x)
 * A macro that sets the direction of the I/O port given in \a x to input.
 */
#define	SET_INPUT(x)	_XSI(x)
/** \def IS_SET(x)
 * A macro that checks whether the I/O port given in \a x is set or not.
 */
#define	IS_SET(x)		_XR(x)

#define	PORT(x)			_port2(x)
#define	DDR(x)			_ddr2(x)
#define	PIN(x)			_pin2(x)

#define	_XRS(x,y)	PORT(x) &= ~(1<<y)
#define	_XS(x,y)	PORT(x) |= (1<<y)
#define	_XT(x,y)	PORT(x) ^= (1<<y)

#define	_XSO(x,y)	DDR(x) |= (1<<y)
#define	_XSI(x,y)	DDR(x) &= ~(1<<y)

#define	_XR(x,y)	((PIN(x) & (1<<y)) != 0)

#define	_port2(x)	PORT ## x
#define	_ddr2(x)	DDR ## x
#define	_pin2(x)	PIN ## x

#endif // IOMACRO_H
