//==============================================================================
//
// Copyright (C) 1993-2005, ART+COM AG Berlin, Germany
//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM AG Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM AG Berlin.
// __ ___ ____ _____ ______ _______ ________ _______ ______ _____ ____ ___ __
//
//
// Description: atomic counter for different processing models
//
// Last Review: pavel 30.11.2005 
//
//  review status report: (perfect, ok, fair, poor, disaster)
//    usefullness            : perfect
//    formatting             : ok
//    documentation          : poor
//    test coverage          : good
//    names                  : fair
//    style guide conformance: poor
//    technical soundness    : ok
//    dead code              : ok
//    readability            : fair
//    understandabilty       : fair
//    interfaces             : ok
//    confidence             : ok
//    integration            : ok
//    dependencies           : ok
//    cheesyness             : ok
//
//    overall review status  : fair
//
//    recommendation: 
//       - improve documentation
//       - reduce number of #ifdefs
// __ ___ ____ _____ ______ _______ ________ _______ ______ _____ ____ ___ __

#ifndef _included_asl_AtomicCount_
#define _included_asl_AtomicCount_

#include "settings.h"

#include <iostream>

#ifdef OSX_X86
    #include <libkern/OSAtomic.h>
#endif

#ifdef LINUX
#ifdef __x86_64__
    #include <asm/atomic.h>
#else
    typedef struct { volatile asl::Signed32 counter; } atomic_t;
#endif
#endif    

/*! \addtogroup aslbase */
/* @{ */

// ATTENTION: The Mac OS X atomic counter is not tested on multiprocessor 
//            machines because I don't have one. (HINT!) DS

#if defined(LINUX) || defined(OSX_X86)
	#define UNIX_X86
#endif

#if defined(UNIX_X86)

#define ATOMIC_INIT(i)	{ (i) }

/**
 * atomic_read - read atomic variable
 * @v: pointer of type atomic_t
 * 
 * Atomically reads the value of @v.  Note that the guaranteed
 * useful range of an atomic_t is only 24 bits.
 */ 
#define atomic_read(v)		((v)->counter)

/**
 * atomic_set - set atomic variable
 * @v: pointer of type atomic_t
 * @i: required value
 * 
 * Atomically sets the value of @v to @i.  Note that the guaranteed
 * useful range of an atomic_t is only 24 bits.
 */ 
#define atomic_set(v,i)		(((v)->counter) = (i))
#define ATOMIC_INIT(i)	{ (i) }
#define atomic_read(v)		((v)->counter)

#if 0
static __inline__ int atomic_dec_and_test(atomic_t *v)
{
	unsigned char c;

	__asm__ __volatile__(
		 "lock ; decl %0; sete %1"
		:"=m" (v->counter), "=qm" (c)
		:"m" (v->counter) : "memory");
	return c != 0;
}

static __inline__ void atomic_inc(atomic_t *v)
{
	__asm__ __volatile__(
		"lock ; incl %0"
		:"=m" (v->counter)
		:"m" (v->counter));
}
#endif
static __inline__ int atomic_post_inc(atomic_t *v)
{
   register int result = 1; 
    __asm__ __volatile__(
            "lock ; xaddl %1, %0"
            :"=m" (v->counter), "=q"(result)
            :"m" (v->counter), "q" (result)
            : "memory" );
    return result;
}

inline int atomic_conditional_increment(atomic_t * pw )
{
    // int rv = *pw;
    // if( rv != 0 ) ++*pw;
    // return rv;

    int rv, tmp;

    __asm__
    (
        "movl %0, %%eax\n\t"
        "0:\n\t"
        "test %%eax, %%eax\n\t"
        "je 1f\n\t"
        "movl %%eax, %2\n\t"
        "incl %2\n\t"
        "lock\n\t"
        "cmpxchgl %2, %0\n\t"
        "jne 0b\n\t"
        "1:":
        "=m"( *pw ), "=&a"( rv ), "=&r"( tmp ): // outputs (%0, %1, %2)
        "m"( *pw ): // input (%3)
        "cc" // clobbers
    );

    return rv;
}

inline int atomic_conditional_decrement(atomic_t * pw )
{
    // int rv = *pw;
    // if( rv != 0 ) --*pw;
    // return rv == 1;

    int rv, tmp;

    __asm__
    (
        "movl %0, %%eax\n\t"
        "0:\n\t"
        "test %%eax, %%eax\n\t"
        "je 1f\n\t"
        "movl %%eax, %2\n\t"
        "decl %2\n\t"
        "lock\n\t"
        "cmpxchgl %2, %0\n\t"
        "jne 0b\n\t"
        "1:":
        "=m"( *pw ), "=&a"( rv ), "=&r"( tmp ): // outputs (%0, %1, %2)
        "m"( *pw ): // input (%3)
        "cc" // clobbers
    );

    return rv==1;
}

static __inline__ int atomic_dec_and_test_SingleProcessor(atomic_t *v)
{
	unsigned char c;

	__asm__ __volatile__(
		 "decl %0; sete %1"
		:"=m" (v->counter), "=qm" (c)
		:"m" (v->counter) : "memory");
	return c != 0;
}

static __inline__ void atomic_inc_SingleProcessor(atomic_t *v)
{
	__asm__ __volatile__(
		 "incl %0"
		:"=m" (v->counter)
		:"m" (v->counter));
}

static __inline__ int atomic_post_inc_SingleProcessor(atomic_t *v)
{
   register int result = 1; 
    __asm__ __volatile__(
            "xaddl %1, %0"
            :"=m" (v->counter), "=q"(result)
            :"m" (v->counter), "q" (result)
            : "memory" );
    return result;
}

inline int atomic_conditional_increment_SingleProcessor(atomic_t * pw )
{
    // int rv = *pw;
    // if( rv != 0 ) ++*pw;
    // return rv;

    int rv, tmp;

    __asm__
    (
        "movl %0, %%eax\n\t"
        "0:\n\t"
        "test %%eax, %%eax\n\t"
        "je 1f\n\t"
        "movl %%eax, %2\n\t"
        "incl %2\n\t"
        "cmpxchgl %2, %0\n\t"
        "jne 0b\n\t"
        "1:":
        "=m"( *pw ), "=&a"( rv ), "=&r"( tmp ): // outputs (%0, %1, %2)
        "m"( *pw ): // input (%3)
        "cc" // clobbers
    );

    return rv;
}

inline int atomic_conditional_decrement_SingleProcessor(atomic_t * pw )
{
    // int rv = *pw;
    // if( rv != 0 ) --*pw;
    // return rv == 1;

    int rv, tmp;

    __asm__
    (
        "movl %0, %%eax\n\t"
        "0:\n\t"
        "test %%eax, %%eax\n\t"
        "je 1f\n\t"
        "movl %%eax, %2\n\t"
        "decl %2\n\t"
        "cmpxchgl %2, %0\n\t"
        "jne 0b\n\t"
        "1:":
        "=m"( *pw ), "=&a"( rv ), "=&r"( tmp ): // outputs (%0, %1, %2)
        "m"( *pw ): // input (%3)
        "cc" // clobbers
    );

    return rv==1;
}

#endif

#ifdef OSX_PPC

// Taken from boost/detail/sp_counted_base_gcc_ppc.hpp.
// This hasn't even been compiled, much less tested!

inline int atomic_inc( int * pw )
{
    // ++*pw;

    int tmp;

    __asm__ __volatile__
    (
        "sync\n\t"
        "0:\n\t"
        "lwarx %1, 0, %2\n\t"
        "addi %1, %1, 1\n\t"
        "stwcx. %1, 0, %2\n\t"
        "bne- 0b\n\t"
        "isync":

        "=m"( *pw ), "=&b"( tmp ):
        "r"( pw ):
        "memory", "cc"
    );
    return tmp;
}

inline int atomic_dec( int * pw )
{
    // return --*pw;

    int rv;

    __asm__ __volatile__
    (
        "sync\n\t"
        "0:\n\t"
        "lwarx %1, 0, %2\n\t"
        "addi %1, %1, -1\n\t"
        "stwcx. %1, 0, %2\n\t"
        "bne- 0b\n\t"
        "isync":

        "=m"( *pw ), "=&b"( rv ):
        "r"( pw ):
        "memory", "cc"
    );

    return rv;
}


inline int atomic_conditional_increment( volatile int * pw )
{
    // if( *pw != 0 ) ++*pw;
    // return *pw;

    int rv, tmp;
    __asm__ __volatile__
    (
        "sync\n\t"
        "0:\n\t"
        "lwarx %1, 0, %3\n\t"
        "cmpwi %1, 0\n\t"
        "beq 1f\n\t"         // XXX was: beq 1f
        "mr %2, %1\n\t"
        "addi %2, %2, 1\n\t"
        "stwcx. %2, 0, %3\n\t"
        "bne- 0b\n\t"
        "1:\n\t"
        "isync":
        "=m"( *pw ), "=&b"( rv ), "=&b"( tmp ):
        "r"( pw ):
        "memory", "cc"
    );

    return rv;
}

inline int atomic_conditional_decrement( volatile int * pw )
{
    int rv, tmp;
    __asm__ __volatile__
    (
        "sync\n\t"
        "0:\n\t"
        "lwarx %1, 0, %3\n\t"
        "cmpwi %1, 0\n\t"
        "beq 1f\n\t"         // XXX was: beq 1f
        "mr %2, %1\n\t"
        "subi %2, %2, 1\n\t"
        "stwcx. %2, 0, %3\n\t"
        "bne- 0b\n\t"
        "1:\n\t"
        "isync":
        "=m"( *pw ), "=&b"( rv ), "=&b"( tmp ):
        "r"( pw ):
        "memory", "cc"
    );

    return rv==1;
}

inline int atomic_conditional_decrement_SingleProcessor(int volatile* pw) {
    return atomic_conditional_decrement(pw);
}

inline int atomic_conditional_increment_SingleProcessor(int volatile* pw) {
    return atomic_conditional_increment(pw);
}


#endif

#ifdef WIN32
extern "C" __declspec(dllimport) long __stdcall InterlockedIncrement(long volatile *);
extern "C" __declspec(dllimport) long __stdcall InterlockedDecrement(long volatile *);
extern "C" __declspec(dllimport) long __stdcall 
        InterlockedCompareExchange( long volatile *, long, long );
        
inline int atomic_conditional_increment(long volatile * pw )
{
    // int rv = *pw;
    // if( rv != 0 ) ++*pw;
    // return rv;

    for( ;; ) {
        long tmp = *pw;
        if (tmp == 0) {
            return 0;
        }
        if (InterlockedCompareExchange(pw, tmp + 1, tmp) == tmp) {
            return tmp;
        }
    }
}

inline int atomic_conditional_decrement(long volatile* pw )
{
    // int rv = *pw;
    // if( rv != 0 ) --*pw;
    // return rv == 1;

    for(;;) {
        long tmp = *pw;
        if (tmp == 0) {
            return 0;
        }
        if (InterlockedCompareExchange(pw, tmp-1, tmp) == tmp) {
            return tmp == 1;
        }
    }
}

inline int atomic_conditional_decrement_SingleProcessor(long volatile* pw) {
    return atomic_conditional_decrement(pw);
}

inline int atomic_conditional_increment_SingleProcessor(long volatile* pw) {
    return atomic_conditional_increment(pw);
}

#endif

namespace asl
{
    class SingleThreaded {};
    class SingleProcessor {};
    class MultiProcessor {};

    template <class Threading> class AtomicCount;

    template <>
    class AtomicCount<SingleThreaded> {
        public:
            explicit AtomicCount(long v) {
				value = v;
            }
	        inline
	    	void increment() {
                ++value;
	    	}
            inline
            long conditional_increment() {
                if (value) {
                    ++value;
                }
                return value;
            }
            inline
            long conditional_decrement() {
                int oldValue = value;
                if (value) {
                    --value;
                }
                return oldValue == 1;
            }
            inline
            long post_increment() {
                return value++;
	    	}
            inline
	    	long decrement_and_test() {
        		return --value == 0;
	    	}
            inline
            void set(long l) {
                value = l;
            }
            inline
            operator long() const {
				return value;
            }
        private:
            AtomicCount(AtomicCount const &);
            AtomicCount & operator=(AtomicCount const &);

			long value;
	};

    template <>
    class AtomicCount<SingleProcessor> {
        public:
            inline
            explicit AtomicCount(long v)
            {
#ifdef UNIX_X86
                atomic_t init = ATOMIC_INIT(v);
				value = init;
#endif
#ifdef WIN32
				value = v;
#endif
#ifdef OSX_PPC
                value = v;
#endif
            }
	        inline
	    	void increment() {
#ifdef UNIX_X86 
               	atomic_inc_SingleProcessor(&value);
#endif
#ifdef WIN32
        	InterlockedIncrement(&value);
#endif
#ifdef OSX_PPC
                atomic_inc( & value);
#endif
	    	}
            
            inline
            long conditional_increment() {
                return atomic_conditional_increment_SingleProcessor(&value);
            }
            
            inline
            long conditional_decrement() {
                return atomic_conditional_decrement_SingleProcessor(&value);
            }
            
            inline
            long post_increment() {
#ifdef UNIX_X86
               	return atomic_post_inc_SingleProcessor(&value);
#endif
#ifdef WIN32
        		return InterlockedIncrement(&value)-1;
#endif
#ifdef OSX_PPC
                return atomic_inc(&value) - 1;
#endif
	    	}
            inline
	    	long decrement_and_test() {
#ifdef UNIX_X86
                return atomic_dec_and_test_SingleProcessor(&value);
#endif
#ifdef WIN32
        		return InterlockedDecrement(&value) == 0;
#endif
#ifdef OSX_PPC
                return atomic_dec(&value) == 0;
#endif
	    	}
            inline
            operator long() const
            {
#ifdef UNIX_X86
            	return atomic_read(&value);
#endif
#ifdef WIN32
				return value;
#endif
#ifdef OSX_PPC
                return value;
#endif
            }
            inline void set(long i) 
            {
#ifdef UNIX_X86
            	atomic_set(&value, i);
#endif
#ifdef WIN32
				value = i;
#endif
#ifdef OSX_PPC
                value = i;
#endif
            }
        private:
            AtomicCount(AtomicCount const &);
            AtomicCount & operator=(AtomicCount const &);
#ifdef UNIX_X86
            atomic_t value;
#endif
#ifdef WIN32
			volatile long value;
#endif
#ifdef OSX_PPC
			int value;
#endif
	};
    template <>
    class AtomicCount<MultiProcessor> {
        public:
            inline
            explicit AtomicCount(long v)
            {
#ifdef UNIX_X86
                atomic_t init = ATOMIC_INIT(v);
				value = init;
#endif
#ifdef WIN32
				value = v;
#endif
#ifdef OSX_PPC
				value = v;
#endif
            }
	
            inline
	    	void increment() {
#ifdef UNIX_X86
               	atomic_inc(&value);
#endif
#ifdef WIN32
        		InterlockedIncrement(&value);
#endif
#ifdef OSX_PPC
               	atomic_inc(&value);
#endif
	    	}

            inline
            long conditional_increment() {
                return atomic_conditional_increment(&value);
            }
            
            inline
            long conditional_decrement() {
                return atomic_conditional_decrement(&value);
            }

            inline
            long post_increment() {
#ifdef OSX_X86
				return OSAtomicAdd32(1, (int32_t*)&value)-1;
#endif
#ifdef LINUX
#ifdef __x86_64__
                return atomic_inc_return(&value)-1;
#else
               	return atomic_post_inc(&value);
#endif                
#endif
#ifdef WIN32
        		return InterlockedIncrement(&value)-1;
#endif
#ifdef OSX_PPC
               	return atomic_inc(&value) - 1;
#endif
	    	}

            inline
	    	long decrement_and_test() {
#ifdef UNIX_X86
                return atomic_dec_and_test(&value);
#endif
#ifdef WIN32
        	return InterlockedDecrement(&value) == 0;
#endif
#ifdef OSX_PPC
               	return atomic_dec(&value) == 0;
#endif
	    	}

            inline
            operator long() const
            {
#ifdef UNIX_X86
            	return atomic_read(&value);
#endif
#ifdef WIN32
		return value;
#endif
#ifdef OSX_PPC
               	return value;
#endif
            }
            inline void set(long i) 
            {
#ifdef UNIX_X86
            	atomic_set(&value, i);
#endif
#ifdef WIN32
				value = i;
#endif
#ifdef OSX_PPC
			    value = i;
#endif
            }
        private:
            AtomicCount(AtomicCount const &);
            AtomicCount & operator=(AtomicCount const &);
#ifdef UNIX_X86
            atomic_t value;
#endif
#ifdef WIN32
			volatile long value;
#endif
#ifdef OSX_PPC
			int value;
#endif
	};

}

/* @} */


#endif 
