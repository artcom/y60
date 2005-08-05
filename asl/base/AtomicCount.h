/* __ ___ ____ _____ ______ _______ ________ _______ ______ _____ ____ ___ __
//
// Copyright (C) 1993-2001, ART+COM Berlin GmbH
//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM GmbH Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM GmbH Berlin.
// __ ___ ____ _____ ______ _______ ________ _______ ______ _____ ____ ___ __
//
//    $RCSfile: AtomicCount.h,v $
//
//   $Revision: 1.6 $
//
// Description: atomic counter for different processing models
//
//
// __ ___ ____ _____ ______ _______ ________ _______ ______ _____ ____ ___ __
*/
#ifndef _included_asl_AtomicCount_
#define _included_asl_AtomicCount_


/*! \addtogroup aslbase */
/* @{ */

// ATTENTION: The Mac OS X atomic counter is not tested on multiprocessor 
//            machines because I don't have one. (HINT!) DS

#ifdef LINUX

typedef struct { volatile int counter; } atomic_t;

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
#endif

#ifdef OSX

// taken from http://www.mulle-kybernetik.com/artikel/Optimization/opti-4-atomic.html

static inline
int
atomic_inc( int *p) {
    int tmp;
    asm volatile(
            "1:     lwarx   %0,0,%1\n"
            "       addic   %0,%0,1\n"
            "       stwcx.  %0,0,%1\n"
            "       bne-    1b"
            : "=&r" (tmp)
            : "r" (p)
            : "cc", "memory");

    return( tmp);
}

static inline
int
atomic_dec( int *p) {
    int tmp;
    asm volatile(
            "1:     lwarx   %0,0,%1\n"
            "       addic   %0,%0,-1\n"
            "       stwcx.  %0,0,%1\n"
            "       bne-    1b"
            : "=&r" (tmp)
            : "r" (p)
            : "cc", "memory");

    return( tmp);
}

#endif

#ifdef WIN32
extern "C" __declspec(dllimport) long __stdcall InterlockedIncrement(long volatile *);
extern "C" __declspec(dllimport) long __stdcall InterlockedDecrement(long volatile *);
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
#ifdef LINUX
                atomic_t init = ATOMIC_INIT(v);
				value = init;
#endif
#ifdef WIN32
				value = v;
#endif
#ifdef OSX
                value = v;
#endif
            }
	        inline
	    	void increment() {
#ifdef LINUX
               	atomic_inc_SingleProcessor(&value);
#endif
#ifdef WIN32
        		InterlockedIncrement(&value);
#endif
#ifdef OSX
                atomic_inc( & value);
#endif
	    	}
            inline
            long post_increment() {
#ifdef LINUX
               	return atomic_post_inc_SingleProcessor(&value);
#endif
#ifdef WIN32
        		return InterlockedIncrement(&value)-1;
#endif
#ifdef OSX
                return atomic_inc(&value) - 1;
#endif
	    	}
            inline
	    	long decrement_and_test() {
#ifdef LINUX
                return atomic_dec_and_test_SingleProcessor(&value);
#endif
#ifdef WIN32
        		return InterlockedDecrement(&value) == 0;
#endif
#ifdef OSX
                return atomic_dec(&value) == 0;
#endif
	    	}
            inline
            operator long() const
            {
#ifdef LINUX
            	return atomic_read(&value);
#endif
#ifdef WIN32
				return value;
#endif
#ifdef OSX
                return value;
#endif
            }
            inline void set(long i) 
            {
#ifdef LINUX
            	atomic_set(&value, i);
#endif
#ifdef WIN32
				value = i;
#endif
#ifdef OSX
                value = i;
#endif
            }
        private:
            AtomicCount(AtomicCount const &);
            AtomicCount & operator=(AtomicCount const &);
#ifdef LINUX
            atomic_t value;
#endif
#ifdef WIN32
			volatile long value;
#endif
#ifdef OSX
			int value;
#endif
	};
    template <>
    class AtomicCount<MultiProcessor> {
        public:
            inline
            explicit AtomicCount(long v)
            {
#ifdef LINUX
                atomic_t init = ATOMIC_INIT(v);
				value = init;
#endif
#ifdef WIN32
				value = v;
#endif
#ifdef OSX
				value = v;
#endif
            }
	
            inline
	    	void increment() {
#ifdef LINUX
               	atomic_inc(&value);
#endif
#ifdef WIN32
        		InterlockedIncrement(&value);
#endif
#ifdef OSX
               	atomic_inc(&value);
#endif
	    	}

            inline
            long post_increment() {
#ifdef LINUX
               	return atomic_post_inc(&value);
#endif
#ifdef WIN32
        		return InterlockedIncrement(&value)-1;
#endif
#ifdef OSX
               	return atomic_inc(&value) - 1;
#endif
	    	}

            inline
	    	long decrement_and_test() {
#ifdef LINUX
                return atomic_dec_and_test(&value);
#endif
#ifdef WIN32
        		return InterlockedDecrement(&value) == 0;
#endif
#ifdef OSX
               	return atomic_dec(&value) == 0;
#endif
	    	}

            inline
            operator long() const
            {
#ifdef LINUX
            	return atomic_read(&value);
#endif
#ifdef WIN32
				return value;
#endif
#ifdef OSX
               	return value;
#endif
            }
            inline void set(long i) 
            {
#ifdef LINUX
            	atomic_set(&value, i);
#endif
#ifdef WIN32
				value = i;
#endif
#ifdef OSX
				value = i;
#endif
            }
        private:
            AtomicCount(AtomicCount const &);
            AtomicCount & operator=(AtomicCount const &);
#ifdef LINUX
            atomic_t value;
#endif
#ifdef WIN32
			volatile long value;
#endif
#ifdef OSX
			int value;
#endif
	};

}

/* @} */


#endif 
